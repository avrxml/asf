/**
 * \file
 *
 * \brief Graphical User Interface.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdio.h>

#include "gui.h"
#include "gpio.h"
#include "et024006dhu.h"
#include "sdram_loader.h"
#include "audio_mixer.h"
#include "conf_version.h"

#include "file.h"

//! Default foreground color
#define APPLI_FG_COLOR                WHITE
//! Default background color
#define APPLI_BG_COLOR                -1

enum transition_t
{
  TRANSITION_FROM_LEFT_TO_RIGHT,
  TRANSITION_FROM_RIGHT_TO_LEFT
};

static bool draw_navigation(int offset_x, int offset_y);
static bool draw_play_view(int offset_x, int offset_y);
static bool draw_startup(int offset_x, int offset_y);
static bool draw_config(int offset_x, int offset_y);
static bool draw_play_button(void);
static bool draw_pause_button(void);
static bool draw_playtime_update(struct file_info *current_file_info);
static bool draw_no_file(struct display_file_list *lst_str);
static bool draw_file_list(struct display_file_list *lst_str);
static bool draw_volume_bar(struct file_info *current_file_info);
static bool draw_song_progress_bar(struct file_info *current_file_info);
static bool draw_play_view_print_file(struct file_info *current_file_info);
static bool draw_play_view_print_artist(struct file_info *current_file_info);
static bool draw_play_view_print_title(struct file_info *current_file_info);
static bool draw_play_view_print_total_time(struct file_info *current_file_info);
static bool draw_cover_art(struct file_info *current_file_info);
static bool draw_in_progress(enum gui_update_view view);
static bool clear_in_progress(enum gui_update_view view);
static bool draw_device_not_supported(void);
static bool draw_device_present(void);
static bool draw_device_not_present(void);
static bool draw_cursor(struct display_file_list *lst_str);
static bool draw_config_view_print_repeat(struct player_info *player);
static bool draw_config_view_print_shuffle(struct player_info *player);
static bool draw_config_view_print_cursor(struct display_file_list *lst_str);
static bool draw_transition(bool (*view_start)(int, int), bool (*view_end)(int, int), enum transition_t transition);
static void draw_background(int image_id, int offset_x, int offset_y);
static void draw_device_type(void);

void gui_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz)
{
  et024006_Init(fcpu_hz, fhsb_hz);
  et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK);
  gpio_set_gpio_pin(ET024006DHU_BL_PIN);
  load_sdram_data(fhsb_hz);
  draw_startup(0, 0);
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
  static bool clear_in_progress_done=false;

  if (!(*element & GUI_UPDATE_ELT_IN_PROGRESS))
  {
    if (!clear_in_progress_done)
    {
      clear_in_progress_done=true;
      clear_in_progress(view);
    }
  }
  else
  {
    draw_in_progress(view);
    clear_in_progress_done=false;
    *element &= ~GUI_UPDATE_ELT_IN_PROGRESS;
  }

  switch (view)
  {
  case GUI_UPDATE_VIEW_NONE:
    break;
  case GUI_UPDATE_VIEW_IDLE:
    if (last_view != GUI_UPDATE_VIEW_IDLE)
    {
      draw_startup(0, 0);
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
      switch(last_view)
      {
      case GUI_UPDATE_VIEW_IDLE_TRANSITION:
      case GUI_UPDATE_VIEW_IDLE:
        last_view = GUI_UPDATE_VIEW_IDLE_TRANSITION;
        if (!draw_transition(draw_startup, draw_navigation, TRANSITION_FROM_LEFT_TO_RIGHT))
          return;
        break;
      case GUI_UPDATE_VIEW_PLAYBACK_TRANSITION:
      case GUI_UPDATE_VIEW_PLAYBACK:
        last_view = GUI_UPDATE_VIEW_PLAYBACK_TRANSITION;
        if (!draw_transition(draw_play_view, draw_navigation, TRANSITION_FROM_RIGHT_TO_LEFT))
          return;
        break;
      default:
        break;
      }
      draw_navigation(0, 0);
      last_view = GUI_UPDATE_VIEW_NAVIGATION;
      break;
    }
    call_draw_fct(*element, GUI_UPDATE_ELT_NAVIGATION_CURSOR, draw_cursor, lst_str);
    call_draw_fct(*element, GUI_UPDATE_ELT_NAVIGATION_FILE_LIST, draw_file_list, lst_str);
    call_draw_fct(*element, GUI_UPDATE_ELT_NAVIGATION_NO_FILES, draw_no_file, lst_str);
    break;
  case GUI_UPDATE_VIEW_PLAYBACK:
    if (last_view != GUI_UPDATE_VIEW_PLAYBACK)
    {
      switch(last_view)
      {
      case GUI_UPDATE_VIEW_NAVIGATION_TRANSITION:
      case GUI_UPDATE_VIEW_NAVIGATION:
        last_view = GUI_UPDATE_VIEW_NAVIGATION_TRANSITION;
        if (!draw_transition(draw_navigation, draw_play_view, TRANSITION_FROM_LEFT_TO_RIGHT))
          return;
        break;
      case GUI_UPDATE_VIEW_CONFIG_TRANSITION:
      case GUI_UPDATE_VIEW_CONFIG:
        last_view = GUI_UPDATE_VIEW_CONFIG_TRANSITION;
        if (!draw_transition(draw_config, draw_play_view, TRANSITION_FROM_RIGHT_TO_LEFT))
          return;
        break;
      default:
        break;
      }
      draw_play_view(0, 0);
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
  case GUI_UPDATE_VIEW_CONFIG:
    if (last_view != GUI_UPDATE_VIEW_CONFIG)
    {
      switch(last_view)
      {
      case GUI_UPDATE_VIEW_PLAYBACK_TRANSITION:
      case GUI_UPDATE_VIEW_PLAYBACK:
        last_view = GUI_UPDATE_VIEW_PLAYBACK_TRANSITION;
        if (!draw_transition(draw_play_view, draw_config, TRANSITION_FROM_LEFT_TO_RIGHT))
          return;
        break;
      default:
        break;
      }
      draw_config(0, 0);
      last_view = GUI_UPDATE_VIEW_CONFIG;
      break;
    }
    call_draw_fct(*element, GUI_UPDATE_ELT_CONFIG_REPEAT, draw_config_view_print_repeat, player);
    call_draw_fct(*element, GUI_UPDATE_ELT_CONFIG_SHUFFLE, draw_config_view_print_shuffle, player);
    call_draw_fct(*element, GUI_UPDATE_ELT_CONFIG_CURSOR, draw_config_view_print_cursor, lst_str);
    break;
  default:
    break;
  }
}

void gui_enter_idle(void)
{
  et024006_enter_idle();
}
void gui_leave_idle(void)
{
  et024006_leave_idle();
}

#define REPEAT_POS_X              72
#define REPEAT_POS_Y              60
#define REPEAT_FONT               FONT8x8
#define REPEAT_FONT_HEIGHT        8
#define REPEAT_FONT_WIDTH         8
#define REPEAT_MESSAGE_WIDTH      260
#define REPEAT_MESSAGE_HEIGHT     (4*REPEAT_FONT_HEIGHT)

static bool draw_config_view_print_repeat(struct player_info *player)
{
  int offset_y;

  // Clear the previous repeat message
  if (ram_files[CONFIG_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[CONFIG_IMAGE].start_addr,
                       ET024006_WIDTH, REPEAT_POS_X + 10*REPEAT_FONT_WIDTH, REPEAT_POS_Y,
                       REPEAT_POS_X + 10*REPEAT_FONT_WIDTH, REPEAT_POS_Y,
                       REPEAT_FONT_WIDTH, REPEAT_MESSAGE_HEIGHT);

  // Print the message
  et024006_PrintString("Repeat:  [ ] Off", (unsigned char *) REPEAT_FONT,
                       REPEAT_POS_X, REPEAT_POS_Y, APPLI_FG_COLOR, APPLI_BG_COLOR);
  et024006_PrintString("         [ ] Track", (unsigned char *) REPEAT_FONT,
                       REPEAT_POS_X, REPEAT_POS_Y + REPEAT_FONT_HEIGHT, APPLI_FG_COLOR, APPLI_BG_COLOR);
  et024006_PrintString("         [ ] Folder", (unsigned char *) REPEAT_FONT,
                       REPEAT_POS_X, REPEAT_POS_Y + 2*REPEAT_FONT_HEIGHT, APPLI_FG_COLOR, APPLI_BG_COLOR);
  et024006_PrintString("         [ ] All", (unsigned char *) REPEAT_FONT,
                       REPEAT_POS_X, REPEAT_POS_Y + 3*REPEAT_FONT_HEIGHT, APPLI_FG_COLOR, APPLI_BG_COLOR);

  switch (player->repeat)
  {
  case AUDIO_REPEAT_TRACK:
    offset_y = 1*REPEAT_FONT_HEIGHT;
    break;
  case AUDIO_REPEAT_FOLDER:
    offset_y = 2*REPEAT_FONT_HEIGHT;
    break;
  case AUDIO_REPEAT_ALL:
    offset_y = 3*REPEAT_FONT_HEIGHT;
    break;
  case AUDIO_REPEAT_OFF:
  default:
    offset_y = 0;
  }

  // Draw cursor
  et024006_PrintString("X", (unsigned char *) REPEAT_FONT,
                       REPEAT_POS_X + 10*REPEAT_FONT_WIDTH, REPEAT_POS_Y + offset_y, APPLI_FG_COLOR, APPLI_BG_COLOR);

  return true;
}

#define SHUFFLE_POS_X              72
#define SHUFFLE_POS_Y              100
#define SHUFFLE_FONT               FONT8x8
#define SHUFFLE_FONT_HEIGHT        8
#define SHUFFLE_FONT_WIDTH         8
#define SHUFFLE_MESSAGE_WIDTH      260
#define SHUFFLE_MESSAGE_HEIGHT     (3*SHUFFLE_FONT_HEIGHT)

static bool draw_config_view_print_shuffle(struct player_info *player)
{
  int offset_y;

  // Clear the previous repeat message
  if (ram_files[CONFIG_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[CONFIG_IMAGE].start_addr,
                       ET024006_WIDTH, SHUFFLE_POS_X + 10*SHUFFLE_FONT_WIDTH, SHUFFLE_POS_Y,
                       SHUFFLE_POS_X + 10*SHUFFLE_FONT_WIDTH, SHUFFLE_POS_Y,
                       SHUFFLE_FONT_WIDTH, SHUFFLE_MESSAGE_HEIGHT);

  // Print the message
  et024006_PrintString("Shuffle: [ ] Off", (unsigned char *) SHUFFLE_FONT,
                       SHUFFLE_POS_X, SHUFFLE_POS_Y, APPLI_FG_COLOR, APPLI_BG_COLOR);
  et024006_PrintString("         [ ] Folder", (unsigned char *) SHUFFLE_FONT,
                       SHUFFLE_POS_X, SHUFFLE_POS_Y + REPEAT_FONT_HEIGHT, APPLI_FG_COLOR, APPLI_BG_COLOR);
  et024006_PrintString("         [ ] All", (unsigned char *) SHUFFLE_FONT,
                       SHUFFLE_POS_X, SHUFFLE_POS_Y + 2*REPEAT_FONT_HEIGHT, APPLI_FG_COLOR, APPLI_BG_COLOR);

  switch (player->shuffle)
  {
  case AUDIO_SHUFFLE_FOLDER:
    offset_y = 1*SHUFFLE_FONT_HEIGHT;
    break;
  case AUDIO_SHUFFLE_ALL:
    offset_y = 2*SHUFFLE_FONT_HEIGHT;
    break;
  case AUDIO_SHUFFLE_OFF:
  default:
    offset_y = 0;
  }

  // Draw cursor
  et024006_PrintString("X", (unsigned char *) SHUFFLE_FONT,
                       SHUFFLE_POS_X + 10*SHUFFLE_FONT_WIDTH, SHUFFLE_POS_Y + offset_y, APPLI_FG_COLOR, APPLI_BG_COLOR);

  return true;
}

#define CURSOR_POS_X              60
#define CURSOR_FONT               FONT8x8
#define CURSOR_FONT_HEIGHT        8
#define CURSOR_FONT_WIDTH         8
#define CURSOR_RANGE_HEIGHT       (SHUFFLE_POS_Y + CURSOR_FONT_HEIGHT - REPEAT_POS_Y)

static bool draw_config_view_print_cursor(struct display_file_list *lst_str)
{
  // Clear the previous cursor position
  if (ram_files[CONFIG_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[CONFIG_IMAGE].start_addr,
                       ET024006_WIDTH, CURSOR_POS_X, REPEAT_POS_Y,
                       CURSOR_POS_X, REPEAT_POS_Y,
                       CURSOR_FONT_WIDTH, CURSOR_RANGE_HEIGHT);

  switch (lst_str->mode_pos)
  {
  case MODE_POS_REPEAT:
    et024006_PrintString(">", (unsigned char *) CURSOR_FONT,
                         CURSOR_POS_X, REPEAT_POS_Y, APPLI_FG_COLOR, APPLI_BG_COLOR);
    break;
  case MODE_POS_SHUFFLE:
    et024006_PrintString(">", (unsigned char *) CURSOR_FONT,
                         CURSOR_POS_X, SHUFFLE_POS_Y, APPLI_FG_COLOR, APPLI_BG_COLOR);
    break;
  }
  return true;
}


#define STARTUP_MESSAGE_X                     0
#define STARTUP_MESSAGE_Y                     (ET024006_HEIGHT-STARTUP_MESSAGE_HEIGHT)
#define STARTUP_MESSAGE_WIDTH                 (ET024006_WIDTH-STARTUP_MESSAGE_X)
#define STARTUP_MESSAGE_HEIGHT                29
#define STARTUP_MESSAGE_LOADING_WIDTH         100
#define STARTUP_MESSAGE_NOT_SUPPORTED_WIDTH   176

static bool draw_device_not_present(void)
{
  if (ram_files[STARTUP_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[STARTUP_IMAGE].start_addr,
                       ET024006_WIDTH, STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y,
                       STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y, STARTUP_MESSAGE_WIDTH, STARTUP_MESSAGE_HEIGHT);
  return true;
}

static bool draw_device_not_supported(void)
{
  if (ram_files[STARTUP_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[STARTUP_IMAGE].start_addr,
                       ET024006_WIDTH, STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y,
                       STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y, STARTUP_MESSAGE_WIDTH, STARTUP_MESSAGE_HEIGHT);
  if (ram_files[NOT_SUPPORTED].start_addr)
    et024006_PutPixmap(ram_files[NOT_SUPPORTED].start_addr,
                       STARTUP_MESSAGE_NOT_SUPPORTED_WIDTH, 0, 0,
                       STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y, STARTUP_MESSAGE_NOT_SUPPORTED_WIDTH, STARTUP_MESSAGE_HEIGHT);
  return true;
}

static bool draw_device_present(void)
{
  if (ram_files[STARTUP_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[STARTUP_IMAGE].start_addr,
                       ET024006_WIDTH, STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y,
                       STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y, STARTUP_MESSAGE_WIDTH, STARTUP_MESSAGE_HEIGHT);
  if (ram_files[LOADING].start_addr)
    et024006_PutPixmap(ram_files[LOADING].start_addr,
                       STARTUP_MESSAGE_LOADING_WIDTH, 0, 0,
                       STARTUP_MESSAGE_X, STARTUP_MESSAGE_Y, STARTUP_MESSAGE_LOADING_WIDTH, STARTUP_MESSAGE_HEIGHT);
  return true;
}

static void draw_device_type(void)
{
  char *type = "";

  if (ai_is_usb_ms())
    type = "USB MS";
  else if (ai_is_hid_class())
    type = "USB Audio";
  else if (ai_is_sd_mmc())
    type = "SD/MMC card";

  et024006_PrintString(type, (unsigned char *) FONT8x8, 16, 5, 0xFD60, APPLI_BG_COLOR);
}

static bool draw_transition(bool (*view_start)(int, int), bool (*view_end)(int, int), enum transition_t transition)
{
  static int cur_x = ET024006_WIDTH;
  static bool altern = true;
  const int inc = 25;

  cur_x -= inc;
  if (cur_x < 0)
    cur_x = 0;

  switch (transition)
  {
  case TRANSITION_FROM_LEFT_TO_RIGHT:
    if (altern)
      view_start(cur_x - ET024006_WIDTH, 0);
    else
      view_end(cur_x, 0);
    break;
  case TRANSITION_FROM_RIGHT_TO_LEFT:
    if (altern)
      view_start(ET024006_WIDTH - cur_x, 0);
    else
      view_end(-cur_x, 0);
    break;
  }

  altern = !altern;

  if (cur_x)
    return false;

  cur_x = ET024006_WIDTH;
  return true;
}

#define FILE_LIST_POS_X           60
#define FILE_LIST_POS_Y           60
#define FILE_LIST_SPACING         15
#define FILE_LIST_HEIGHT          180
#define FILE_LIST_WIDTH           260
#define FILE_LIST_FONT            FONT8x8
#define FILE_LIST_ITEM_X          12
#define FILE_LIST_ITEM_WIDTH      8
#define FILE_LIST_ITEM_HEIGHT     8

static bool draw_no_file(struct display_file_list *lst_str)
{
  const char message[] = "Empty directory";
  et024006_PrintString((char *) message, (unsigned char *) FILE_LIST_FONT,
                       FILE_LIST_POS_X + FILE_LIST_ITEM_X, FILE_LIST_POS_Y,
                       0xFD60, APPLI_BG_COLOR);
  return true;
}

static bool draw_cursor(struct display_file_list *lst_str)
{
  // Print the cursor
  et024006_PrintString(">", (unsigned char *) FILE_LIST_FONT,
                       FILE_LIST_POS_X, FILE_LIST_POS_Y, APPLI_FG_COLOR, APPLI_BG_COLOR);
  return true;
}

/*! \brief Draws the file list on the disk navigation view.
 */
static bool draw_file_list(struct display_file_list *lst_str)
{
  size_t i;
  uint16_t pos_x, pos_y;

  // Find the 1rst line updated.
  for(i=0; i<lst_str->nb_elts; i++)
    if (lst_str->list[i].updated)
      break;
   // Compute the Y position the string to display.
   pos_x = FILE_LIST_POS_X + FILE_LIST_ITEM_X;
   pos_y = FILE_LIST_POS_Y + FILE_LIST_SPACING * i;
   // If it is the last line, then clear the rest of the list.
   if (i == lst_str->nb_elts)
   {
     if (ram_files[DISK_NAV_IMAGE].start_addr)
       et024006_PutPixmap(ram_files[DISK_NAV_IMAGE].start_addr,
                          ET024006_WIDTH, pos_x, pos_y,
                          pos_x, pos_y,
                          FILE_LIST_WIDTH - FILE_LIST_ITEM_X, FILE_LIST_SPACING * (MAX_DISPLAY_FILE - lst_str->nb_elts));
     else
        et024006_DrawFilledRect(pos_x, pos_y,
                                FILE_LIST_WIDTH - FILE_LIST_ITEM_X, FILE_LIST_SPACING * (MAX_DISPLAY_FILE - lst_str->nb_elts),
                                (et024006_color_t) APPLI_BG_COLOR);
     return true;
   }
   // Invalidate the item.
   lst_str->list[i].updated = false;
   // Draw only the 1rst line updated.
   // Print the background.
   if (ram_files[DISK_NAV_IMAGE].start_addr)
     et024006_PutPixmap(ram_files[DISK_NAV_IMAGE].start_addr,
                        ET024006_WIDTH, pos_x, pos_y,
                        pos_x, pos_y,
                        FILE_LIST_WIDTH - FILE_LIST_ITEM_X, FILE_LIST_ITEM_HEIGHT);
   else
     et024006_DrawFilledRect(pos_x, pos_y,
                             FILE_LIST_WIDTH - FILE_LIST_ITEM_X, FILE_LIST_ITEM_HEIGHT,
                             (et024006_color_t) APPLI_BG_COLOR);
   // Draw the string.
   et024006_PrintString((char*) lst_str->list[i].file_name,
                        (unsigned char *) FILE_LIST_FONT, pos_x, pos_y,
                        APPLI_FG_COLOR, APPLI_BG_COLOR);
   return false;
}

static void draw_background(int image_id, int offset_x, int offset_y)
{
  int map_x, map_y;
  int x, y, width, height;

  map_x = 0;
  map_y = 0;
  x = 0;
  y = 0;

  height = ET024006_HEIGHT;
  if (offset_x < 0)
    map_x = -offset_x;
  else
    x = offset_x;
  width = ET024006_WIDTH - Abs(offset_x);
  if (offset_y < 0)
    map_y = -offset_y;
  else
    y = offset_y;
  height = ET024006_HEIGHT - Abs(offset_y);

  if (ram_files[image_id].start_addr)
    et024006_PutPixmap(ram_files[image_id].start_addr,
                       ET024006_WIDTH, map_x, map_y, x, y, width, height);

  // Draw the device type
  draw_device_type();
}


#define VERSION_FONT            FONT8x8
#define VERSION_POS_X           5
#define VERSION_POS_Y           230
#define VERSION_FONT_COLOR      WHITE

/*! \brief Draw the startup image on the display.
 */
static bool draw_startup(int offset_x, int offset_y)
{
  if(ram_files[STARTUP_IMAGE].start_addr)
  {
    draw_background(STARTUP_IMAGE, offset_x, offset_y);
  }
  else
  {
    et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, (et024006_color_t)APPLI_BG_COLOR);
  }

    et024006_PrintString(AUDIO_PLAYER_APPLICATION_VERSION,
            (unsigned char *) VERSION_FONT, VERSION_POS_X, VERSION_POS_Y,
	    VERSION_FONT_COLOR, APPLI_BG_COLOR);

  return true;
}

static bool draw_config(int offset_x, int offset_y)
{
  if(ram_files[CONFIG_IMAGE].start_addr)
  {
    draw_background(CONFIG_IMAGE, offset_x, offset_y);
  }
  else
  {
    et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, (et024006_color_t)APPLI_BG_COLOR);
  }

  return true;
}

#define IN_PROGRESS_SPRITE_NB         8
#define IN_PROGRESS_SPRITE_WIDTH      16
#define IN_PROGRESS_SPRITE_HEIGHT     16

static bool draw_in_progress(enum gui_update_view view)
{
  static uint8_t sprite_index = 0;

  // if file exists in RAM, we load it
  if(ram_files[IN_PROGRESS].start_addr)
  {
    et024006_PutPixmap(ram_files[IN_PROGRESS].start_addr,
                       IN_PROGRESS_SPRITE_NB * IN_PROGRESS_SPRITE_WIDTH,
                       IN_PROGRESS_SPRITE_WIDTH * sprite_index, 0,
                       0, 0, IN_PROGRESS_SPRITE_WIDTH, IN_PROGRESS_SPRITE_HEIGHT);
    sprite_index = (sprite_index + 1) % IN_PROGRESS_SPRITE_NB;
  }
  return true;
}

static bool clear_in_progress(enum gui_update_view view)
{
  et024006_color_t const *pixmap;

  switch (view)
  {
  case GUI_UPDATE_VIEW_IDLE:
  case GUI_UPDATE_VIEW_IDLE_TRANSITION:
    pixmap = ram_files[STARTUP_IMAGE].start_addr;
    break;
  case GUI_UPDATE_VIEW_NAVIGATION:
  case GUI_UPDATE_VIEW_NAVIGATION_TRANSITION:
    pixmap = ram_files[DISK_NAV_IMAGE].start_addr;
    break;
  case GUI_UPDATE_VIEW_PLAYBACK:
  case GUI_UPDATE_VIEW_PLAYBACK_TRANSITION:
    pixmap = ram_files[AUDIO_PLAYER_IMAGE].start_addr;
    break;
  case GUI_UPDATE_VIEW_CONFIG:
  case GUI_UPDATE_VIEW_CONFIG_TRANSITION:
    pixmap = ram_files[CONFIG_IMAGE].start_addr;
    break;
  default:
    pixmap = ram_files[DISK_NAV_IMAGE].start_addr;
  }

  if (pixmap)
    et024006_PutPixmap(pixmap,
                       ET024006_WIDTH, 0, 0, 0, 0,
                       IN_PROGRESS_SPRITE_WIDTH, IN_PROGRESS_SPRITE_HEIGHT);
  return true;
}

/*! \brief Draws the navigation on the screen.
 */
static bool draw_navigation(int offset_x, int offset_y)
{
  if(ram_files[DISK_NAV_IMAGE].start_addr)
  {
    draw_background(DISK_NAV_IMAGE, offset_x, offset_y);
  }
  else
  {
    et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, (et024006_color_t)APPLI_BG_COLOR);
  }
  return true;
}

/* Volume levels are in dB*/
#define VOLUME_MUTED                10
#define VOLUME_LEVEL_1              30
#define VOLUME_LEVEL_2              60
#define VOLUME_LEVEL_3              90
#define VOLUME_LEVEL_4              120
#define VOLUME_LEVEL_5              150
#define VOLUME_LEVEL_6              180
#define VOLUME_LEVEL_7              200
#define VOLUME_LEVEL_8              215
#define VOLUME_LEVEL_9              230
#define VOLUME_LEVEL_10             245

#define VOLUME_BAR_POS_X            175
#define VOLUME_BAR_POS_Y            158
#define VOLUME_BAR_WIDTH            111
#define VOLUME_BAR_HEIGHT           18

#define VOLUME_BAR_COLOR            0xFD60 //BLUE
#define VOLUME_BAR_PART_WIDTH       8
#define VOLUME_BAR_SPACING          3

#define VOLUME_BAR_LEVEL1_HEIGHT    1
#define VOLUME_BAR_LEVEL2_HEIGHT    2
#define VOLUME_BAR_LEVEL3_HEIGHT    3
#define VOLUME_BAR_LEVEL4_HEIGHT    4
#define VOLUME_BAR_LEVEL5_HEIGHT    5
#define VOLUME_BAR_LEVEL6_HEIGHT    6
#define VOLUME_BAR_LEVEL7_HEIGHT    8
#define VOLUME_BAR_LEVEL8_HEIGHT    9
#define VOLUME_BAR_LEVEL9_HEIGHT    10
#define VOLUME_BAR_LEVEL10_HEIGHT   12

/*! \brief Draws the volume bar on the playing view.
 */
static bool draw_volume_bar(struct file_info *current_file_info)
{
  uint8_t volume;

  // the volume is in dB and ranges from +6 to -72 and has an extra value for muted which is the
  // smallest int8_t value
  volume = current_file_info->volume;
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
  {
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, VOLUME_BAR_POS_X, VOLUME_BAR_POS_Y, VOLUME_BAR_POS_X, VOLUME_BAR_POS_Y, VOLUME_BAR_WIDTH, VOLUME_BAR_HEIGHT);
  }
  if(volume == VOLUME_MUTED)
  {

  }
  if(volume > VOLUME_LEVEL_1)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X, VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL1_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL1_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_2)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL2_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL2_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_3)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 2* (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING)+ 1,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL3_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL3_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_4)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 3 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING) +1,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL4_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL4_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_5)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 4 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING) +1,
                            VOLUME_BAR_POS_Y +VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL5_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL5_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_6)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 5 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING) +1,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL6_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL6_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_7)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 6 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING) +1,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL7_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL7_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_8)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 7 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING) +2,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL8_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL8_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume > VOLUME_LEVEL_9)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 8 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING) +2,
                            VOLUME_BAR_POS_Y + VOLUME_BAR_LEVEL10_HEIGHT - VOLUME_BAR_LEVEL9_HEIGHT,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL9_HEIGHT, VOLUME_BAR_COLOR);
  }
  if(volume >= VOLUME_LEVEL_10)
  {
    et024006_DrawFilledRect( VOLUME_BAR_POS_X + 9 * (VOLUME_BAR_PART_WIDTH + VOLUME_BAR_SPACING)+2, VOLUME_BAR_POS_Y,
                            VOLUME_BAR_PART_WIDTH, VOLUME_BAR_LEVEL10_HEIGHT, VOLUME_BAR_COLOR);
  }

  return true;
}

#define PAUSE_BUTTON_COLOR        0xFD60
#define PAUSE_BUTTON_POS_X        35
#define PAUSE_BUTTON_POS_Y        217
#define PAUSE_BUTTON_HEIGHT       12
#define PAUSE_BUTTON_WIDTH        10
#define PAUSE_BUTTON_SPACING      1
#define PAUSE_BUTTON_PART_WIDTH   3

#define PLAY_BUTTON_POS_X         19
#define PLAY_BUTTON_POS_Y         215
#define PLAY_BUTTON_WIDTH         10
#define PLAY_BUTTON_HEIGHT        15

static bool draw_pause_button(void)
{
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
  {
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, PLAY_BUTTON_POS_X, PLAY_BUTTON_POS_Y,
                       PLAY_BUTTON_POS_X, PLAY_BUTTON_POS_Y, PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT);
  }
  else
  {
    et024006_DrawFilledRect(PLAY_BUTTON_POS_X, PLAY_BUTTON_POS_Y,
                            PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT, (et024006_color_t)APPLI_BG_COLOR);
  }

  et024006_DrawFilledRect(PAUSE_BUTTON_POS_X, PAUSE_BUTTON_POS_Y,
                          PAUSE_BUTTON_PART_WIDTH, PAUSE_BUTTON_HEIGHT, PAUSE_BUTTON_COLOR);
  et024006_DrawFilledRect(PAUSE_BUTTON_POS_X + PAUSE_BUTTON_PART_WIDTH + PAUSE_BUTTON_SPACING, PAUSE_BUTTON_POS_Y,
                          PAUSE_BUTTON_PART_WIDTH, PAUSE_BUTTON_HEIGHT, PAUSE_BUTTON_COLOR);
  return true;
}

static bool draw_play_button(void)
{
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
  {
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, PAUSE_BUTTON_POS_X, PAUSE_BUTTON_POS_Y,
                       PAUSE_BUTTON_POS_X, PAUSE_BUTTON_POS_Y, PAUSE_BUTTON_WIDTH, PAUSE_BUTTON_HEIGHT);
  }
  else
  {
    et024006_DrawFilledRect(PAUSE_BUTTON_POS_X, PAUSE_BUTTON_POS_Y,
                            PAUSE_BUTTON_WIDTH, PAUSE_BUTTON_HEIGHT, PAUSE_BUTTON_COLOR);
  }
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+2, PLAY_BUTTON_POS_Y +1, 14, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+3, PLAY_BUTTON_POS_Y+2, 12, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+4, PLAY_BUTTON_POS_Y+3, 10, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+5, PLAY_BUTTON_POS_Y+4, 9, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+6, PLAY_BUTTON_POS_Y+5, 7, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+7, PLAY_BUTTON_POS_Y+5, 6, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+8, PLAY_BUTTON_POS_Y+6, 4, PAUSE_BUTTON_COLOR);
  et024006_DrawVertLine( PLAY_BUTTON_POS_X+9, PLAY_BUTTON_POS_Y+7, 3, PAUSE_BUTTON_COLOR);
  return true;
}

#define ARTIST_POS_X       110
#define ARTIST_POS_Y       60
#define ARTIST_HEIGHT      16
#define ARTIST_WIDTH       (ET024006_WIDTH-ARTIST_POS_X)
#define TITLE_POS_X        110
#define TITLE_POS_Y        78
#define TITLE_HEIGHT       8
#define TITLE_WIDTH        (ET024006_WIDTH-TITLE_POS_X)
#define FILE_NAME_POS_X    110
#define FILE_NAME_POS_Y    90
#define FILE_NAME_HEIGHT   8
#define FILE_NAME_WIDTH    (ET024006_WIDTH-ARTIST_POS_X)

#define ARTIST_FONT        FONT8x16
#define TITLE_FONT         FONT8x8
#define FILE_NAME_FONT     FONT8x8
#define TIME_FONT          FONT8x8

#define TOTAL_TIME_POS_X    250
#define TOTAL_TIME_POS_Y    200
#define TOTAL_TIME_WIDTH    64
#define TOTAL_TIME_HEIGHT   8
#define ELAPSED_TIME_POS_X    5
#define ELAPSED_TIME_POS_Y    200

/* depends on the font size for 6x8 font this is 8*/
#define ELAPSED_TIME_HEIGHT    10
/* for the time string we have "xx:xx:xx" -> 8 characters so the width is 8*8 = 64*/
#define ELAPSED_TIME_WIDTH    64
/* Maximum characters that will be displayed for the file name*/
#define NAME_CHARS_MAX      26
#define ARTIST_CHARS_MAX    26
#define TITLE_CHARS_MAX     26

#if STR_MAX_LENGTH < NAME_CHARS_MAX
  #undef NAME_CHARS_MAX
  #define NAME_CHARS_MAX      STR_MAX_LENGTH
#endif
#if STR_MAX_LENGTH < ARTIST_CHARS_MAX
  #undef ARTIST_CHARS_MAX
  #define ARTIST_CHARS_MAX      STR_MAX_LENGTH
#endif
#if STR_MAX_LENGTH < TITLE_CHARS_MAX
  #undef TITLE_CHARS_MAX
  #define TITLE_CHARS_MAX      STR_MAX_LENGTH
#endif

#define COVER_WIDTH    100
#define COVER_HEIGHT  100
#define COVER_POS_X    5
#define COVER_POS_Y    60

#define MAX_PATH 256

static bool draw_cover_art(struct file_info *current_file_info)
{
#if defined(SUPPORT_EMBEDDED_COVER_ARTS) && SUPPORT_EMBEDDED_COVER_ARTS == true
  bool err = true;

  // Draw the background since not all covers will have the perfect size
  if ( ram_files[AUDIO_PLAYER_IMAGE].start_addr)
  {
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr, ET024006_WIDTH,
                       COVER_POS_X, COVER_POS_Y, COVER_POS_X, COVER_POS_Y, COVER_WIDTH, COVER_HEIGHT);
  }
  // Do we have a valid ID3 tag with image information
  if (current_file_info->image_data)
  {
    err = false;
    et024006_PutPixmap((unsigned short *) current_file_info->image_data,
                       current_file_info->size.width, 0, 0, COVER_POS_X, COVER_POS_Y,
                       Min(COVER_WIDTH, current_file_info->size.width), Min(COVER_HEIGHT, current_file_info->size.height));
  }
  // show a default cover if we did not find one or an error occurred
  if(err)
  {
    if (ram_files[COVER].start_addr)
    {
      et024006_PutPixmap(ram_files[COVER].start_addr,
                         COVER_WIDTH, 0, 0, COVER_POS_X, COVER_POS_Y, COVER_WIDTH, COVER_HEIGHT);
    }
  }
#endif
  return true;
}
/*! \brief Draw the play view.
 * Draws all elements in the play view.
 *
 * \param offset_x X offset of the picture.
 * \param offset_y Y offset of the picture.
 */
static bool draw_play_view(int offset_x, int offset_y)
{
  // draw background if available
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
  {
    draw_background(AUDIO_PLAYER_IMAGE, offset_x, offset_y);
  }
  else
  {
    et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK);
  }
  return true;
}

static bool draw_play_view_print_artist(struct file_info *current_file_info)
{
  char string[ARTIST_CHARS_MAX+2];

  strncpy(string, (const char *) current_file_info->artist, ARTIST_CHARS_MAX);
  string[ARTIST_CHARS_MAX+1] = '\0';
  // draw background behind the text
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, ARTIST_POS_X, ARTIST_POS_Y,
                       ARTIST_POS_X, ARTIST_POS_Y, ARTIST_WIDTH, ARTIST_HEIGHT);
  // if no artist data is available we print the file name
  if(string[0] != '\0')
  {
    et024006_PrintString(string, (unsigned char *) ARTIST_FONT, ARTIST_POS_X, ARTIST_POS_Y,
                         APPLI_FG_COLOR, APPLI_BG_COLOR);
  }
  return true;
}

static bool draw_play_view_print_file(struct file_info *current_file_info)
{
  char string[NAME_CHARS_MAX+2];

  strncpy(string, (const char *) current_file_info->name, NAME_CHARS_MAX);
  string[NAME_CHARS_MAX+1] = '\0';
  // draw background behind the text
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, FILE_NAME_POS_X, FILE_NAME_POS_Y,
                       FILE_NAME_POS_X, FILE_NAME_POS_Y, FILE_NAME_WIDTH, FILE_NAME_HEIGHT);
  // if no artist data is available we print the file name
  if(string[0] != '\0')
  {
    et024006_PrintString(string, (unsigned char *) FILE_NAME_FONT, FILE_NAME_POS_X, FILE_NAME_POS_Y,
                         APPLI_FG_COLOR, APPLI_BG_COLOR);
  }
  return true;
}

static bool draw_play_view_print_title(struct file_info *current_file_info)
{
  char string[TITLE_CHARS_MAX+2];

  strncpy(string, (const char *) current_file_info->title, TITLE_CHARS_MAX);
  string[TITLE_CHARS_MAX+1] = '\0';
  // draw background behind the text
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, TITLE_POS_X, TITLE_POS_Y,
                       TITLE_POS_X, TITLE_POS_Y, TITLE_WIDTH, TITLE_HEIGHT);
  if(string[0] != '\0')
  {
    et024006_PrintString(string, (unsigned char *) TITLE_FONT, TITLE_POS_X, TITLE_POS_Y,
                         APPLI_FG_COLOR, APPLI_BG_COLOR);
  }
  return true;
}

#define SONG_BAR_POS_X          23 // 5 end 300, delta ~275; 275 / 5
#define SONG_BAR_POS_Y          181
#define SONG_BAR_WIDTH          280
#define SONG_BAR_HEIGHT         4
#define SONG_BAR_FILL_COLOR     0xFD60

static bool draw_play_view_print_total_time(struct file_info *current_file_info)
{
  uint32_t hours, minutes, seconds, tmp;
  char time_string[32];

  tmp = current_file_info->total_time;
  hours = tmp / (1000 * 60 * 60);
  tmp %= 1000 * 60 * 60;
  minutes = tmp / (1000 * 60);
  tmp %= 1000 * 60;
  seconds = tmp / 1000;
  sprintf(time_string, "%02u:%02u:%02u", (unsigned int) hours, (unsigned int) minutes, (unsigned int) seconds);
  // Draw background behind the text.
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, TOTAL_TIME_POS_X, TOTAL_TIME_POS_Y, TOTAL_TIME_POS_X, TOTAL_TIME_POS_Y, TOTAL_TIME_WIDTH, TOTAL_TIME_HEIGHT);
  et024006_PrintString(time_string, (unsigned char *) TIME_FONT, TOTAL_TIME_POS_X, TOTAL_TIME_POS_Y, APPLI_FG_COLOR, APPLI_BG_COLOR);

  return true;
}

/*! \brief Draws the song progress bar on the screen.
 *
 * \param init If true the progress bar is initialized which should be done after a song change
 * or after a new draw of the play view. If the progress bar should just be updated call with false
 * \param ptime Current play time in milliseconds.
 * \param current_file_info contains the total play time length
 */
static bool draw_song_progress_bar(struct file_info *current_file_info)
{
  int song_bar_width;

  song_bar_width = (current_file_info->elapsed_time * SONG_BAR_WIDTH) / current_file_info->total_time;
  song_bar_width = Min(SONG_BAR_WIDTH, song_bar_width);

  et024006_DrawFilledRect(SONG_BAR_POS_X, SONG_BAR_POS_Y, song_bar_width, SONG_BAR_HEIGHT, SONG_BAR_FILL_COLOR);
  if (ram_files[AUDIO_PLAYER_IMAGE].start_addr)
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, SONG_BAR_POS_X + song_bar_width, SONG_BAR_POS_Y,
                       SONG_BAR_POS_X + song_bar_width, SONG_BAR_POS_Y,
                       SONG_BAR_WIDTH - song_bar_width, SONG_BAR_HEIGHT);
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

  // draw background behind the play time
  if(ram_files[AUDIO_PLAYER_IMAGE].start_addr)
  {
    et024006_PutPixmap(ram_files[AUDIO_PLAYER_IMAGE].start_addr,
                       ET024006_WIDTH, ELAPSED_TIME_POS_X, ELAPSED_TIME_POS_Y, ELAPSED_TIME_POS_X, ELAPSED_TIME_POS_Y, ELAPSED_TIME_WIDTH, ELAPSED_TIME_HEIGHT);
  }
  else
  {
    et024006_DrawFilledRect( ELAPSED_TIME_POS_X, ELAPSED_TIME_POS_Y, ELAPSED_TIME_WIDTH, ELAPSED_TIME_HEIGHT, (et024006_color_t)APPLI_BG_COLOR);

  }
  // draw the playtime
  et024006_PrintString(time_elapsed, (unsigned char *) TIME_FONT, ELAPSED_TIME_POS_X, ELAPSED_TIME_POS_Y, APPLI_FG_COLOR, -1);

  draw_song_progress_bar(current_file_info);

  return true;
}

