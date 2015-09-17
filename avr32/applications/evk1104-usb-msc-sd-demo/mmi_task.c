/**************************************************************************
 *
 * \file
 *
 * \brief Management of the MMI for the USB device mass-storage task.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "usb_drv.h"
#include "board.h"
#include "gpio.h"
#include "et024006dhu.h"
#include "cycle_counter.h"
#include "avr32_logo.h"
#include "ms_key_logo.h"
#include "main.h"
#include "sd_mmc_mci.h"
#include <stdio.h>


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

typedef enum {
  MMI_IDLE =0
, MMI_TOP_MENU
, MMI_TOP_MENU_START
, MMI_MASS_STORAGE_START
, MMI_MASS_STORAGE
}s_mmi_state;
s_mmi_state mmi_state;


//_____ D E C L A R A T I O N S ____________________________________________


//!
//! @brief This function initializes the hardware/software resources
//! required for the MMI task.
//!
void mmi_task_init(uint32_t cpu_f, uint32_t pba_f)
{
  // Initialize the LCD.
  et024006_Init(  cpu_f, cpu_f /*HSB*/);

  // Clear the display i.e. make it black
  et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );

  // Set the backlight.
  gpio_set_gpio_pin(ET024006DHU_BL_PIN);

  mmi_state = MMI_TOP_MENU_START;
}


#define TIMER_MS_PROGRESS_BAR_UPDATE 555  // Unit is in ms.
#define TIMER_MS_PROGRESS_BAR_CLEAR  220  // Unit is in ms.
#define MS_N_PROGRESS_BAR              8  // Number of bars.
t_cpu_time ms_activity_timer;
t_cpu_time ms_clear_timer;
volatile uint32_t ms_cnt_read;
volatile uint32_t ms_cnt_write;
uint32_t ms_old_cnt_read;
uint32_t ms_old_cnt_write;
uint8_t  ms_cnt_screen;
uint8_t  ms_progress_bar_level[MS_N_PROGRESS_BAR];
uint16_t  ms_progress_bar_type[ MS_N_PROGRESS_BAR];
uint32_t perf_write;
uint32_t perf_read;
char string[64];

static void display_box( uint32_t x, uint32_t y, uint32_t size_x, uint32_t size_y, uint16_t color, uint16_t edge_color )
{
  et024006_DrawFilledRect(x, y, size_x, size_y, color);
  et024006_DrawHorizLine(x, y, size_x, edge_color);
  et024006_DrawVertLine(x+size_x-1, y, size_y, edge_color);
  et024006_DrawHorizLine(x, y+size_y-1, size_x, edge_color);
  et024006_DrawVertLine(x, y, size_y, edge_color);
}

static void display_perf(uint32_t x, uint32_t y, bool b_clear, uint32_t perf_kBps, uint16_t string_color)
{
  display_box(x, y, 50, 18, WHITE, BLACK);
  if( !b_clear )
  {
    sprintf(string, "%5ld", perf_kBps);
    et024006_PrintString(string, (const unsigned char *)&FONT8x8, x+5, y+6, string_color, -1);
  }
  else
  {
    et024006_PrintString("KByte/s", (const unsigned char *)&FONT8x8, x+55, y+6, BLACK, -1);
  }
}

static void mmi_ms_display( void )
{
  uint32_t i;
  for( i=0 ; i<MS_N_PROGRESS_BAR ; i++ )
  {
    if( ms_progress_bar_level[i] != 0 )
    {
      if( ms_progress_bar_type[i] == BLUE )
        et024006_DrawFilledRect(80+3 + i*(17+2), 180+3, 17, 10, BLUE_LEV(ms_progress_bar_level[i]) );
      else
        et024006_DrawFilledRect(80+3 + i*(17+2), 180+3, 17, 10, RED_LEV(ms_progress_bar_level[i]) );
      ms_progress_bar_level[i] -= 1;
    }
  }
}


//!
//! @brief Entry point of the MMI task management
//!
void mmi_task(void)
{
  uint32_t i;
  switch( mmi_state )
  {
  case MMI_TOP_MENU_START:
    // Draw the background AVR32 logo.
    et024006_PutPixmap(avr32_logo, 320, 0, 0, 0, 0, 320, 240);

    // Display welcome string.
    et024006_PrintString("EVK1104 Demo", (const unsigned char *)&FONT8x8, 110, 220, BLACK, -1);

    mmi_state = MMI_TOP_MENU;
    break;

  case MMI_TOP_MENU:
    if( Is_usb_vbus_high() )
    {
      mmi_state = MMI_MASS_STORAGE_START;
    }
    break;

  case MMI_MASS_STORAGE_START:
    // Draw the background AVR32 logo.
    et024006_PutPixmap(avr32_logo, 320, 0, 0, 0, 0, 320, 240);

    // Display USB key logo.
    et024006_DrawFilledRect(220-1, 20-1, 80+2, 42+2, BLACK );
    et024006_PutPixmap(ms_key_logo, 80, 0, 0, 220, 20, 80, 42);

    // Display title.
    et024006_PrintString("U-Disk", (const unsigned char *)&FONT6x8, 240, 65, BLACK, -1);

    // Display Activity window.
    display_box(80, 180, 156, 16, WHITE, BLACK);

    // Display performances box.
    display_perf(120, 201, true, 0, 0);

    ms_old_cnt_read = ms_cnt_read =0;
    ms_old_cnt_write = ms_cnt_write =0;
    mmi_state = MMI_MASS_STORAGE;
    ms_cnt_screen = 0;
    for( i=0 ; i<MS_N_PROGRESS_BAR ; i++ )
    {
      ms_progress_bar_level[i] = 1;
      ms_progress_bar_type[i] = BLACK;
    }
    mmi_ms_display();

    cpu_set_timeout( cpu_ms_2_cy(TIMER_MS_PROGRESS_BAR_UPDATE, pm_freq_param.cpu_f), &ms_activity_timer);
    cpu_set_timeout( cpu_ms_2_cy(TIMER_MS_PROGRESS_BAR_CLEAR,  pm_freq_param.cpu_f), &ms_clear_timer);
    break;

  case MMI_MASS_STORAGE:
    // Manage progress-bar shading.
    //
    if( cpu_is_timeout(&ms_clear_timer) )
    {
      cpu_set_timeout( cpu_ms_2_cy(TIMER_MS_PROGRESS_BAR_CLEAR,  pm_freq_param.cpu_f), &ms_clear_timer);
      mmi_ms_display();
    }

    // Manage progress-bar box moving.
    //
    if( cpu_is_timeout(&ms_activity_timer) )
    {
      cpu_set_timeout( cpu_ms_2_cy(TIMER_MS_PROGRESS_BAR_UPDATE, pm_freq_param.cpu_f), &ms_activity_timer);
      if( ms_old_cnt_write != ms_cnt_write )
      {
        ms_cnt_screen = (unsigned char)(ms_cnt_screen-1)%MS_N_PROGRESS_BAR;
        ms_progress_bar_type[ms_cnt_screen] = RED;

        // Compute performances
        //
        perf_write = (U64)(ms_cnt_write - ms_old_cnt_write)*SD_MMC_SECTOR_SIZE/TIMER_MS_PROGRESS_BAR_UPDATE;
        display_perf(120, 201, false, perf_write, RED);
        ms_old_cnt_write = ms_cnt_write;
        ms_progress_bar_level[ms_cnt_screen] = (perf_write>10000) ? 31 :
                                               (perf_write> 7500) ? 29 :
                                               (perf_write> 5000) ? 27 : 25 ;
      }
      else if( ms_old_cnt_read != ms_cnt_read )
      {
        ms_cnt_screen = (unsigned char)(ms_cnt_screen+1)%MS_N_PROGRESS_BAR;
        ms_progress_bar_type[ms_cnt_screen] = BLUE;

        // Compute performances
        //
        perf_read = (U64)(ms_cnt_read - ms_old_cnt_read)*SD_MMC_SECTOR_SIZE/TIMER_MS_PROGRESS_BAR_UPDATE;
        display_perf(120, 201, false, perf_read, BLUE);
        ms_old_cnt_read = ms_cnt_read;
        ms_progress_bar_level[ms_cnt_screen] = (perf_read>10000) ? 31 :
                                               (perf_read> 7500) ? 29 :
                                               (perf_read> 5000) ? 27 : 25 ;
      }
      else
      {
        display_perf(120, 201, true, 0, 0);
      }
    }

    // Detect USB unplug.
    //
    if( Is_usb_vbus_low() )
    {
      mmi_state = MMI_TOP_MENU_START;
    }
    break;

  default:
    break;
  }

}
