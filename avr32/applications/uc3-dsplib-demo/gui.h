/*****************************************************************************
 *
 * \file
 *
 * \brief GUI
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _GUI_H_
#define _GUI_H_

#include "et024006dhu.h"
#include "dsp_sprintf.h"

#define GUI_NO_COLOR          ((uint16_t) -1)
#define GUI_NO_SEL            -1

#define GUI_MAIN_FG_COLOR     BLACK
#define GUI_MAIN_BG_COLOR     0xF79E
#define GUI_SELECTION_COLOR   0xAA3F
#define GUI_TEXT_COLOR        0xAEAE

#define GUI_ZOOM_BOX_X        0
#define GUI_ZOOM_BOX_Y        0
#define GUI_ZOOM_BOX_WIDTH    320
#define GUI_ZOOM_BOX_HEIGHT   218

#define GUI_SOURCE1_ID        0
#define GUI_SOURCE1_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_SOURCE1_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_SOURCE1_X         13
#define GUI_SOURCE1_Y         10
#define GUI_SOURCE1_WIDTH     92
#define GUI_SOURCE1_HEIGHT    56

#define GUI_SOURCE2_ID        1
#define GUI_SOURCE2_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_SOURCE2_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_SOURCE2_X         13
#define GUI_SOURCE2_Y         153
#define GUI_SOURCE2_WIDTH     92
#define GUI_SOURCE2_HEIGHT    56

#define GUI_INPUT_ID          2
#define GUI_INPUT_FG_COLOR    GUI_MAIN_FG_COLOR
#define GUI_INPUT_BG_COLOR    GUI_MAIN_BG_COLOR
#define GUI_INPUT_X           84
#define GUI_INPUT_Y           82
#define GUI_INPUT_WIDTH       92
#define GUI_INPUT_HEIGHT      56

#define GUI_OUTPUT_ID         3
#define GUI_OUTPUT_FG_COLOR   GUI_MAIN_FG_COLOR
#define GUI_OUTPUT_BG_COLOR   GUI_MAIN_BG_COLOR
#define GUI_OUTPUT_X          210
#define GUI_OUTPUT_Y          82
#define GUI_OUTPUT_WIDTH      92
#define GUI_OUTPUT_HEIGHT     56

#define GUI_FILTER_ID         4
#define GUI_FILTER_FG_COLOR   GUI_TEXT_COLOR
#define GUI_FILTER_BG_COLOR   GUI_NO_COLOR
#define GUI_FILTER_X          130
#define GUI_FILTER_Y          32
#define GUI_FILTER_WIDTH      171
#define GUI_FILTER_HEIGHT     32

#define GUI_COMMENT_ID        5
#define GUI_COMMENT_FG_COLOR  GUI_TEXT_COLOR
#define GUI_COMMENT_BG_COLOR  GUI_NO_COLOR
#define GUI_COMMENT_X         126
#define GUI_COMMENT_Y         153
#define GUI_COMMENT_WIDTH     190
#define GUI_COMMENT_HEIGHT    56

#define GUI_FONT              FONT8x8
#define GUI_FONT_WIDTH        8
#define GUI_FONT_HEIGHT       8

//! This does not take care of the buffer overflow, so make sure this value is big enough
#define GUI_TEXT_BUFFER_SIZE  256

#define GUI_BOXES                         \
{                                         \
  .x = GUI_SOURCE1_X,                     \
  .y = GUI_SOURCE1_Y,                     \
  .width = GUI_SOURCE1_WIDTH,             \
  .height = GUI_SOURCE1_HEIGHT,           \
  .fg_color = GUI_SOURCE1_FG_COLOR,       \
  .bg_color = GUI_SOURCE1_BG_COLOR        \
},                                        \
{                                         \
  .x = GUI_SOURCE2_X,                     \
  .y = GUI_SOURCE2_Y,                     \
  .width = GUI_SOURCE2_WIDTH,             \
  .height = GUI_SOURCE2_HEIGHT,           \
  .fg_color = GUI_SOURCE2_FG_COLOR,       \
  .bg_color = GUI_SOURCE2_BG_COLOR        \
},                                        \
{                                         \
  .x = GUI_INPUT_X,                       \
  .y = GUI_INPUT_Y,                       \
  .width = GUI_INPUT_WIDTH,               \
  .height = GUI_INPUT_HEIGHT,             \
  .fg_color = GUI_INPUT_FG_COLOR,         \
  .bg_color = GUI_INPUT_BG_COLOR          \
},                                        \
{                                         \
  .x = GUI_OUTPUT_X,                      \
  .y = GUI_OUTPUT_Y,                      \
  .width = GUI_OUTPUT_WIDTH,              \
  .height = GUI_OUTPUT_HEIGHT,            \
  .fg_color = GUI_OUTPUT_FG_COLOR,        \
  .bg_color = GUI_OUTPUT_BG_COLOR         \
},                                        \
{                                         \
  .x = GUI_FILTER_X,                      \
  .y = GUI_FILTER_Y,                      \
  .width = GUI_FILTER_WIDTH,              \
  .height = GUI_FILTER_HEIGHT,            \
  .fg_color = GUI_FILTER_FG_COLOR,        \
  .bg_color = GUI_FILTER_BG_COLOR         \
},                                        \
{                                         \
  .x = GUI_COMMENT_X,                     \
  .y = GUI_COMMENT_Y,                     \
  .width = GUI_COMMENT_WIDTH,             \
  .height = GUI_COMMENT_HEIGHT,           \
  .fg_color = GUI_COMMENT_FG_COLOR,       \
  .bg_color = GUI_COMMENT_BG_COLOR        \
}

void gui_init(int cpu_hz, int hsb_hz, int pba_hz, int pbb_hz);
void gui_task(void);

void gui_clear_view(void);
//! This function change the frequency rate of the update of the display
void gui_change_update_fs(uint32_t time_ms);
uint32_t gui_get_update_fs(void);
//! Select a new box (can select only one box a time)
void gui_set_selection(int box_id);
//! Print text in a text box
void gui_text_print(int box_id, const char *text);
//! Print a text in the text area
#define gui_text_printf(box_id, format, ...)                                   \
{                                                                              \
  extern char gui_text_buffer[];                                               \
  dsp16_sprintf(gui_text_buffer, format, __VA_ARGS__);                         \
  gui_text_print(box_id, (const char *) gui_text_buffer);                      \
}


#endif //_GUI_H_
