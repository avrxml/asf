/**
 * \file
 *
 * \brief GUI Management Task.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
