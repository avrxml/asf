/*****************************************************************************
 *
 * \file
 *
 * \brief Graphical User Interface.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdio.h>

#include "gui.h"
#include "gpio.h"
#include "conf_audio_player.h"
#include "nlao_usart.h"

/*! \name USART Used for STDIO
 */
//! @{
#if !defined(STDIO_USART)
  #define STDIO_USART                   (&AVR32_USART0)
  #define STDIO_USART_RX_PIN            AVR32_USART0_RXD_0_0_PIN
  #define STDIO_USART_RX_FUNCTION       AVR32_USART0_RXD_0_0_FUNCTION
  #define STDIO_USART_TX_PIN            AVR32_USART0_TXD_0_0_PIN
  #define STDIO_USART_TX_FUNCTION       AVR32_USART0_TXD_0_0_FUNCTION
  #define STDIO_USART_IRQ               AVR32_USART0_IRQ
  #define STDIO_USART_BAUDRATE          57600
#endif
//! @}

static bool draw_navigation(void);
static bool draw_play_view(struct file_info *current_file_info);
static bool draw_play_button(void);
static bool draw_pause_button(void);
static bool draw_playtime_update(struct file_info *current_file_info);
static bool draw_file_list(struct display_file_list *lst_str);
static bool draw_startup(void);
static bool draw_volume_bar(struct file_info *current_file_info);
static bool draw_song_progress_bar(struct file_info *current_file_info, uint32_t ptime, bool init);
static bool draw_play_view_print_file(struct file_info *current_file_info);
static bool draw_play_view_print_artist(struct file_info *current_file_info);
static bool draw_play_view_print_title(struct file_info *current_file_info);
static bool draw_play_view_print_total_time(struct file_info *current_file_info);
static bool draw_cover_art(struct file_info *current_file_info);
static bool draw_in_progress(void);
static bool clear_in_progress(void);
static bool draw_device_not_supported(void);
static bool draw_device_present(void);
static bool draw_device_not_present(void);
static bool draw_cursor(struct display_file_list *lst_str);

/*! \brief Initializes STDIO.
 */
static void init_stdio(uint32_t fpba_hz)
{
#if __GNUC__ && __AVR32__

  static const gpio_map_t STDIO_USART_GPIO_MAP =
  {
    {STDIO_USART_RX_PIN, STDIO_USART_RX_FUNCTION},
    {STDIO_USART_TX_PIN, STDIO_USART_TX_FUNCTION}
  };

  // Initialize the USART used for STDIO.
  set_usart_base((void *)STDIO_USART);
  gpio_enable_module(STDIO_USART_GPIO_MAP,
                     sizeof(STDIO_USART_GPIO_MAP) / sizeof(STDIO_USART_GPIO_MAP[0]));
  usart_init(STDIO_USART_BAUDRATE);

#elif __ICCAVR32__

  static const gpio_map_t STDIO_USART_GPIO_MAP =
  {
    {STDIO_USART_RX_PIN, STDIO_USART_RX_FUNCTION},
    {STDIO_USART_TX_PIN, STDIO_USART_TX_FUNCTION}
  };

  static const usart_options_t STDIO_USART_OPTIONS =
  {
    .baudrate     = STDIO_USART_BAUDRATE,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Initialize the USART used for STDIO.
  extern volatile avr32_usart_t *volatile stdio_usart_base;
  stdio_usart_base = STDIO_USART;
  gpio_enable_module(STDIO_USART_GPIO_MAP,
                     sizeof(STDIO_USART_GPIO_MAP) / sizeof(STDIO_USART_GPIO_MAP[0]));
  usart_init_rs232(STDIO_USART, &STDIO_USART_OPTIONS, fpba_hz);

#endif
}

void gui_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz)
{
  init_stdio(fpba_hz);
  putchar('\x0C');
  printf("Audio Player\r\n");
}

void gui_shutdown()
{
}

#define call_draw_fct(elt, elt_id, fct_call, ...) \
{ \
  if ((elt) & (elt_id)) \
  { \
    if ((fct_call)(__VA_ARGS__)) \
      (elt) &= ~(elt_id); \
    break; \
  } \
}

void gui_update(enum gui_update_view view, enum gui_update_elt *element,
                struct display_file_list *lst_str, struct file_info *info,
		struct player_info *player)
{
  static enum gui_update_view last_view = GUI_UPDATE_VIEW_NONE;

  if (!(*element & GUI_UPDATE_ELT_IN_PROGRESS))
    clear_in_progress();
  else
  {
    draw_in_progress();
    *element &= ~GUI_UPDATE_ELT_IN_PROGRESS;
  }

  switch (view)
  {
  case GUI_UPDATE_VIEW_NONE:
    break;
  case GUI_UPDATE_VIEW_IDLE:
    if (last_view != GUI_UPDATE_VIEW_IDLE)
    {
      draw_startup();
      last_view = GUI_UPDATE_VIEW_IDLE;
      break;
    }
    call_draw_fct(*element, GUI_UPDATE_ELT_IDLE_NO_DEVICE_PRESENT, draw_device_not_present, );
    call_draw_fct(*element, GUI_UPDATE_ELT_IDLE_DEVICE_PRESENT, draw_device_present, );
    call_draw_fct(*element, GUI_UPDATE_ELT_IDLE_NOT_SUPPORTED, draw_device_not_supported, );
    break;
  case GUI_UPDATE_VIEW_NAVIGATION:
    if (last_view != GUI_UPDATE_VIEW_NAVIGATION)
    {
      draw_navigation();
      last_view = GUI_UPDATE_VIEW_NAVIGATION;
      break;
    }
    call_draw_fct(*element, GUI_UPDATE_ELT_NAVIGATION_CURSOR, draw_cursor, lst_str);
    call_draw_fct(*element, GUI_UPDATE_ELT_NAVIGATION_FILE_LIST, draw_file_list, lst_str);
    break;
  case GUI_UPDATE_VIEW_PLAYBACK:
    if (last_view != GUI_UPDATE_VIEW_PLAYBACK)
    {
      draw_play_view(info);
      last_view = GUI_UPDATE_VIEW_PLAYBACK;
      break;
    }
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_PLAY, draw_play_button, );
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_PAUSE, draw_pause_button, );
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_ELAPSED_TIME, draw_playtime_update, info);
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_VOLUME, draw_volume_bar, info);
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_FILE_NAME, draw_play_view_print_file, info);
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_ARTIST, draw_play_view_print_artist, info);
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_TITLE, draw_play_view_print_title, info);
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_TOTAL_TIME, draw_play_view_print_total_time, info);
    call_draw_fct(*element, GUI_UPDATE_ELT_PLAYBACK_COVER_ART, draw_cover_art, info);
    break;
  }
}

static bool draw_device_not_present(void)
{
  printf("Device not present\r\n");
  return true;
}

static bool draw_device_not_supported(void)
{
  printf("Device not supported\r\n");
  return true;
}

static bool draw_device_present(void)
{
  printf("Device present\r\n");
  return true;
}

static bool draw_cursor(struct display_file_list *lst_str)
{
  return true;
}

/*! \brief Draws the file list on the disk navigation view.
 */
static bool draw_file_list(struct display_file_list *lst_str)
{
  return true;
}

/*! \brief Draw the startup image on the display.
 */
static bool draw_startup(void)
{
  return true;
}

static bool draw_in_progress(void)
{
  return true;
}

static bool clear_in_progress(void)
{
  return true;
}

/*! \brief Draws the navigation on the screen.
 */
static bool draw_navigation(void)
{
  printf("Disk Nav\r\n");
  return true;
}

/*! \brief Draws the volume bar on the playing view.
 */
static bool draw_volume_bar(struct file_info *current_file_info)
{
  return true;
}

static bool draw_pause_button(void)
{
  return true;
}

static bool draw_play_button(void)
{
  return true;
}

static bool draw_cover_art(struct file_info *current_file_info)
{
  return true;
}
/*! \brief Draw the play view.
 * Draws all elements in the play view.
 *
 * \param current_file_info Information needed to be displayed in the play view.
 */
static bool draw_play_view(struct file_info *current_file_info)
{
  printf("Audio Player\r\n");
  printf("%s\r\n", current_file_info->name);
  return true;
}

static bool draw_play_view_print_artist(struct file_info *current_file_info)
{
  return true;
}

static bool draw_play_view_print_file(struct file_info *current_file_info)
{
  return true;
}

static bool draw_play_view_print_title(struct file_info *current_file_info)
{
  return true;
}

static bool draw_play_view_print_total_time(struct file_info *current_file_info)
{
  return true;
}

/*! \brief Draws the song progress bar on the screen.
 *
 * \param init If true the progress bar is initialized which should be done after a song change
 * or after a new draw of the play view. If the progress bar should just be updated call with false
 * \param ptime Current play time in milliseconds.
 * \param current_file_info contains the total play time length
 */
static bool draw_song_progress_bar(struct file_info *current_file_info, uint32_t ptime, bool init)
{
  return true;
}
/*! \brief Draws the elements that need periodic update in the play view.
 *
 *  Updates the current play time.
 */
static bool draw_playtime_update(struct file_info *current_file_info)
{
  uint32_t hours, minutes, seconds, tmp, time_ms;
  char time_elapsed[32];

  // get the time and put it into a printable format
  time_ms = current_file_info->elapsed_time;
  tmp = time_ms;
  hours = tmp / (1000 * 60 * 60);
  tmp %= 1000 * 60 * 60;
  minutes = tmp / (1000 * 60);
  tmp %= 1000 * 60;
  seconds = tmp / 1000;
  sprintf(time_elapsed,"%02lu:%02lu:%02lu",hours, minutes, seconds);

  printf("%s\r\n", time_elapsed);

  return true;
}

