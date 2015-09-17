/**
 * \file
 *
 * \brief GUI Management Task.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _GUI_H_
#define _GUI_H_

#include "et024006dhu.h"

#define GUI_NO_COLOR          ((uint16_t) -1)
#define GUI_NO_SEL            -1

#define GUI_CURSOR_COLOR      0xFEC0       // Amber
#define GUI_MAIN_FG_COLOR     BLACK
#define GUI_MAIN_BG_COLOR     0xF79E
#define GUI_SELECTION_COLOR   0xAA3F
#define GUI_TEXT_COLOR        0xAEAE

#define GUI_ZOOM_BOX_X        0
#define GUI_ZOOM_BOX_Y        0
#define GUI_ZOOM_BOX_WIDTH    320
#define GUI_ZOOM_BOX_HEIGHT   240

#define GUI_MAX_BUFFER_WIDTH  GUI_ZOOM_BOX_WIDTH
#define GUI_MAX_BUFFER_HEIGHT GUI_ZOOM_BOX_HEIGHT

// Scope Window
#define GUI_SOURCE1_ID        0
#define GUI_SOURCE1_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_SOURCE1_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_SOURCE1_X         28
#define GUI_SOURCE1_Y         22
#define GUI_SOURCE1_WIDTH     166
#define GUI_SOURCE1_HEIGHT    139

// Noise
#define GUI_OUTPUT1_ID        1
#define GUI_OUTPUT1_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_OUTPUT1_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_OUTPUT1_X         20
#define GUI_OUTPUT1_Y         194
#define GUI_OUTPUT1_WIDTH     180
#define GUI_OUTPUT1_HEIGHT    34

// Output Generated Signal
#define GUI_OUTPUT2_ID        2
#define GUI_OUTPUT2_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_OUTPUT2_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_OUTPUT2_X         104
#define GUI_OUTPUT2_Y         80
#define GUI_OUTPUT2_WIDTH     94
#define GUI_OUTPUT2_HEIGHT    58

// DSP Filtered Signal
#define GUI_OUTPUT3_ID        3
#define GUI_OUTPUT3_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_OUTPUT3_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_OUTPUT3_X         216
#define GUI_OUTPUT3_Y         38
#define GUI_OUTPUT3_WIDTH     94
#define GUI_OUTPUT3_HEIGHT    58

// FPU Filtered Signal
#define GUI_OUTPUT4_ID        4
#define GUI_OUTPUT4_FG_COLOR  GUI_MAIN_FG_COLOR
#define GUI_OUTPUT4_BG_COLOR  GUI_MAIN_BG_COLOR
#define GUI_OUTPUT4_X         216
#define GUI_OUTPUT4_Y         128
#define GUI_OUTPUT4_WIDTH     94
#define GUI_OUTPUT4_HEIGHT    58

#define GUI_FONT              FONT8x8
#define GUI_FONT_WIDTH        8
#define GUI_FONT_HEIGHT       8

//! This does not take care of the buffer overflow, so make sure this value is
//! big enough
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
  .x = GUI_OUTPUT1_X,                     \
  .y = GUI_OUTPUT1_Y,                     \
  .width = GUI_OUTPUT1_WIDTH,             \
  .height = GUI_OUTPUT1_HEIGHT,           \
  .fg_color = GUI_OUTPUT1_FG_COLOR,       \
  .bg_color = GUI_OUTPUT1_BG_COLOR        \
},                                        \
{                                         \
  .x = GUI_OUTPUT2_X,                       \
  .y = GUI_OUTPUT2_Y,                       \
  .width = GUI_OUTPUT2_WIDTH,               \
  .height = GUI_OUTPUT2_HEIGHT,             \
  .fg_color = GUI_OUTPUT2_FG_COLOR,         \
  .bg_color = GUI_OUTPUT2_BG_COLOR          \
},                                        \
{                                         \
  .x = GUI_OUTPUT3_X,                      \
  .y = GUI_OUTPUT3_Y,                      \
  .width = GUI_OUTPUT3_WIDTH,              \
  .height = GUI_OUTPUT3_HEIGHT,            \
  .fg_color = GUI_OUTPUT3_FG_COLOR,        \
  .bg_color = GUI_OUTPUT3_BG_COLOR         \
},                                        \
{                                         \
  .x = GUI_OUTPUT4_X,                      \
  .y = GUI_OUTPUT4_Y,                      \
  .width = GUI_OUTPUT4_WIDTH,              \
  .height = GUI_OUTPUT4_HEIGHT,            \
  .fg_color = GUI_OUTPUT4_FG_COLOR,        \
  .bg_color = GUI_OUTPUT4_BG_COLOR         \
}

extern void gui_task_init(void);
extern void gui_task(void);

#endif //_GUI_H_
