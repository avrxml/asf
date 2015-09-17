/*****************************************************************************
 *
 * \file
 *
 * \brief GUI
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

#include <stdarg.h>
#include "board.h"
#include "et024006dhu.h"
#include "conf_demo.h"
#include "gpio.h"
#include "gui.h"
#include "background_image.h"
#include "cycle_counter.h"

#include "dsp.h"
#include "dsp_sprintf.h"

#include "compiler.h"

struct gui_box
{
  uint16_t x, y;
  uint16_t width, height;
  et024006_color_t fg_color, bg_color;
};

static void gui_draw_background(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
static void gui_buffer_print_dsp16_signal(void *buffer, uint16_t width, uint16_t height, dsp16_t *signal, uint16_t signal_size);
static void gui_buffer_print_dsp16_bars(void *buffer, uint16_t width, uint16_t height, dsp16_t *signal, uint16_t signal_size);
static inline void gui_buffer_set_pixel(void *buffer, uint16_t width, uint16_t height, uint16_t x, uint16_t y);
//static void gui_et024006_bitmap(void *bitmap, uint16_t map_width, uint16_t x, uint16_t y, uint16_t width, uint16_t height, et024006_color_t foreground, et024006_color_t background);
static void gui_print_signal_box(int box_id, dsp16_t *signal, uint16_t signal_size);
static void gui_print_fft_box(int box_id, dsp16_t *signal, uint16_t signal_size);

static const struct gui_box box[] = {
GUI_BOXES
};

#define MAX_BUFFER_WIDTH    Max(Max(Max(Max(GUI_SOURCE1_WIDTH, GUI_SOURCE2_WIDTH), GUI_INPUT_WIDTH), GUI_OUTPUT_WIDTH), GUI_ZOOM_BOX_WIDTH)
#define MAX_BUFFER_HEIGHT   Max(Max(Max(Max(GUI_SOURCE1_HEIGHT, GUI_SOURCE2_HEIGHT), GUI_INPUT_HEIGHT), GUI_OUTPUT_HEIGHT), GUI_ZOOM_BOX_HEIGHT)

static uint8_t buffer[ET024006_BITMAP_WIDTH(MAX_BUFFER_WIDTH)*MAX_BUFFER_HEIGHT];
static dsp16_t signal_buffer[BUFFER_LENGTH];

static struct
{
  t_cpu_time cpu_time;
  uint32_t time_ms;
} gui_fs;
static int gui_cpu_hz;

char gui_text_buffer[GUI_TEXT_BUFFER_SIZE];

extern dsp16_t signal1_buf[BUFFER_LENGTH];
extern dsp16_t signal2_buf[BUFFER_LENGTH];
extern dsp16_t *signal_in_buf;
extern dsp16_t signal_in_fft[BUFFER_LENGTH];
extern dsp16_t signal_out_buf[BUFFER_LENGTH];
extern dsp16_t signal_out_fft[BUFFER_LENGTH];

extern bool signals_are_updated;
void gui_change_update_fs(uint32_t time_ms)
{
  gui_fs.time_ms = time_ms;
  cpu_set_timeout(cpu_ms_2_cy(gui_fs.time_ms, gui_cpu_hz), &gui_fs.cpu_time);
}

uint32_t gui_get_update_fs(void)
{
  return gui_fs.time_ms;
}

void gui_init(int cpu_hz, int hsb_hz, int pba_hz, int pbb_hz)
{
  gui_cpu_hz = cpu_hz;
  // Init the display
  et024006_Init(cpu_hz, hsb_hz);
  gpio_set_gpio_pin(ET024006DHU_BL_PIN);
  // Draw the background
  gui_clear_view();
  // Set the frequency rate of the update of the display
  gui_change_update_fs(DEFAULT_SCREEN_UPDATE_FS_MS);
}

void gui_clear_view(void)
{
  gui_draw_background(0, 0, ET024006_WIDTH, ET024006_HEIGHT);
}

void gui_task(void)
{
  extern volatile bool input_fft_view;
  extern volatile bool output_fft_view;

  // Check the last time the printing has been done
  if (cpu_is_timeout(&gui_fs.cpu_time))
  {
    gui_print_signal_box(GUI_SOURCE1_ID, signal1_buf, sizeof(signal1_buf)/sizeof(signal1_buf[0]));
    gui_print_signal_box(GUI_SOURCE2_ID, signal2_buf, sizeof(signal2_buf)/sizeof(signal2_buf[0]));
    if (input_fft_view)
      gui_print_fft_box(GUI_INPUT_ID, signal_in_fft, FFT_LENGTH);
    else
      gui_print_signal_box(GUI_INPUT_ID, signal_in_buf, BUFFER_LENGTH);
    if (output_fft_view)
      gui_print_fft_box(GUI_OUTPUT_ID, signal_out_fft, FFT_LENGTH);
    else
      gui_print_signal_box(GUI_OUTPUT_ID, signal_out_buf, sizeof(signal2_buf)/sizeof(signal2_buf[0]));
    cpu_set_timeout(cpu_ms_2_cy(gui_fs.time_ms, gui_cpu_hz), &gui_fs.cpu_time);
  }
}

void gui_set_selection(int box_id)
{
  static int prev_selection = GUI_NO_SEL;
  struct gui_box *sb;
  int i;

  // Clear previous selections
  if (prev_selection != GUI_NO_SEL && prev_selection != box_id)
  {
    i = prev_selection;
    sb = (struct gui_box *) &box[i];
    // Don't do anything if the color is transparent
    if (sb->bg_color == GUI_NO_COLOR)
    {
      gui_draw_background(sb->x - 2, sb->y - 2, sb->width + 4, 2);
      gui_draw_background(sb->x - 2, sb->y + sb->height, sb->width + 4, 2);
      gui_draw_background(sb->x - 2, sb->y - 2, 2, sb->height + 4);
      gui_draw_background(sb->x + sb->width, sb->y - 2, 2, sb->height + 4);
    }
    else
    {
      et024006_DrawFilledRect(sb->x - 2, sb->y - 2, sb->width + 4, 2, sb->bg_color);
      et024006_DrawFilledRect(sb->x - 2, sb->y + sb->height, sb->width + 4, 2, sb->bg_color);
      et024006_DrawFilledRect(sb->x - 2, sb->y - 2, 2, sb->height + 4, sb->bg_color);
      et024006_DrawFilledRect(sb->x + sb->width, sb->y - 2, 2, sb->height + 4, sb->bg_color);
    }
  }

  prev_selection = box_id;

  if (box_id == GUI_NO_SEL)
    return;
  // Draw the selection
  sb = (struct gui_box *) &box[box_id];
  et024006_DrawFilledRect(sb->x - 2, sb->y - 2, sb->width + 4, 2, GUI_SELECTION_COLOR);
  et024006_DrawFilledRect(sb->x - 2, sb->y + sb->height, sb->width + 4, 2, GUI_SELECTION_COLOR);
  et024006_DrawFilledRect(sb->x - 2, sb->y - 2, 2, sb->height + 4, GUI_SELECTION_COLOR);
  et024006_DrawFilledRect(sb->x + sb->width, sb->y - 2, 2, sb->height + 4, GUI_SELECTION_COLOR);
}


static void gui_print_signal_box(int box_id, dsp16_t *signal, uint16_t signal_size)
{
  const struct gui_box *sb = &box[box_id];
  extern volatile bool zoom_view;
  extern volatile int zoom_view_id;

  if (zoom_view && zoom_view_id != box_id)
    return;

  signal_size = Min(signal_size, BUFFER_LENGTH);
  do
  {
    signals_are_updated = 0;
    memcpy(signal_buffer, signal, sizeof(dsp16_t)*signal_size);
  } while (signals_are_updated);

  if (zoom_view && zoom_view_id == box_id)
    gui_buffer_print_dsp16_signal(buffer, GUI_ZOOM_BOX_WIDTH, GUI_ZOOM_BOX_HEIGHT, signal_buffer, signal_size);
  else
    gui_buffer_print_dsp16_signal(buffer, sb->width, sb->height, signal_buffer, signal_size);

  if (zoom_view && zoom_view_id == box_id)
    et024006_PutBitmap(buffer, GUI_ZOOM_BOX_WIDTH, GUI_ZOOM_BOX_X, GUI_ZOOM_BOX_Y, GUI_ZOOM_BOX_WIDTH, GUI_ZOOM_BOX_HEIGHT, sb->fg_color, sb->bg_color);
  else
    et024006_PutBitmap(buffer, sb->width, sb->x, sb->y, sb->width, sb->height, sb->fg_color, sb->bg_color);
}

static void gui_print_fft_box(int box_id, dsp16_t *signal, uint16_t signal_size)
{
  const struct gui_box *sb = &box[box_id];
  extern volatile bool zoom_view;
  extern volatile int zoom_view_id;

  if (zoom_view && zoom_view_id != box_id)
    return;

  if (zoom_view && zoom_view_id == box_id)
  {
    gui_buffer_print_dsp16_bars(buffer, GUI_ZOOM_BOX_WIDTH, GUI_ZOOM_BOX_HEIGHT, signal, signal_size);
    et024006_PutBitmap(buffer, GUI_ZOOM_BOX_WIDTH, GUI_ZOOM_BOX_X, GUI_ZOOM_BOX_Y, GUI_ZOOM_BOX_WIDTH, GUI_ZOOM_BOX_HEIGHT, sb->fg_color, sb->bg_color);
  }
  else
  {
    gui_buffer_print_dsp16_bars(buffer, sb->width, sb->height, signal, signal_size);
    et024006_PutBitmap(buffer, sb->width, sb->x, sb->y, sb->width, sb->height, sb->fg_color, sb->bg_color);
  }
}

void gui_text_print(int box_id, const char *text)
{
  extern volatile bool zoom_view;
  const struct gui_box *sb = (struct gui_box *) &box[box_id];
  int i, len;
  int y = sb->y;

  if (zoom_view)
    return;

  // Clear the background
  if (sb->bg_color == GUI_NO_COLOR)
    gui_draw_background(sb->x, sb->y, sb->width, sb->height);
  else
    et024006_DrawFilledRect(sb->x, sb->y, sb->width, sb->height, sb->bg_color);

  // Calculate the number of line to be displayed
  len = 0;
  i = 0;
  do
  {
    if (text[i] == '\n' || text[i] == '\0')
      len++;
  } while (text[i++]);
  // Set the y coordinate so that it will align the text in the middle of the text box (vertical axis)
  y += (sb->height - len*GUI_FONT_HEIGHT)/2;

  // Print the text line per line
  len = 0;
  i = 0;
  do
  {
    if (text[i] == '\n' || text[i] == '\0')
    {
      if (i - len >= 0)
      {
        memmove(gui_text_buffer, &text[i - len], len);
        gui_text_buffer[len] = '\0';
        if (sb->bg_color == GUI_NO_COLOR)
          et024006_PrintString(gui_text_buffer, (const unsigned char *) &GUI_FONT, sb->x, y, sb->fg_color, -1);
        else
          et024006_PrintString(gui_text_buffer, (const unsigned char *) &GUI_FONT, sb->x, y, sb->fg_color, sb->bg_color);
        gui_text_buffer[len] = ' ';
      }
      y += GUI_FONT_HEIGHT;
      len = 0;
    }
    else
      len++;
  } while (text[i++]);
}

static void gui_draw_background(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  et024006_PutPixmap(background_image, ET024006_WIDTH, x, y, x, y, width, height);
}

static inline void gui_buffer_set_pixel(void *buffer, uint16_t width, uint16_t height, uint16_t x, uint16_t y)
{
  uint8_t *u8_buffer = (uint8_t *) buffer;
  if (x >= width || y >= height)
    return;
  u8_buffer += y * ET024006_BITMAP_WIDTH(width) + x / 8;
  *u8_buffer = *u8_buffer | (1 << (x & 7));
}

static void gui_buffer_print_dsp16_signal(void *buffer, uint16_t width, uint16_t height, dsp16_t *signal, uint16_t signal_size)
{
  uint16_t t, cur_y;
  uint32_t scale_x, scale_y;
  uint32_t i_signal;
  uint16_t i, last_y = 0;

  // Clean the graph
  memset(buffer, 0, ET024006_BITMAP_WIDTH(width)*height);
  // Compute scaling coefficients
  scale_y = (uint32_t) ((height/2) * 0x10000) / ((uint32_t) -DSP_FP_MIN(DSP16_QA, DSP16_QB));

  if (signal_size >= width)
  {
    // Speed drawing
    scale_x = (uint32_t) (signal_size * 0x10000) / width;
    for (t=0; t<width; t++)
    {
      i_signal = ((uint64_t) t * scale_x) / 0x10000;
      // Integer value
      cur_y = ((((uint64_t) signal[i_signal] * scale_y) / 0x10000) >> DSP16_QB);
      cur_y = height/2 - cur_y - 1;
      if (last_y <= cur_y && t)
      {
        for (i=last_y; i<=cur_y; i++)
          gui_buffer_set_pixel(buffer, width, height, t, i);
      }
      else if (t)
      {
        for (i=cur_y; i<=last_y; i++)
          gui_buffer_set_pixel(buffer, width, height, t, i);
      }
      last_y = cur_y;
    }
  }
  else
  {
    uint16_t prev_t = 0, prev_y = 0;
    int32_t delta_y;

    // Fine drawing
    scale_x = (((uint32_t) (width * (signal_size + 1)) / signal_size) * 0x10000) / signal_size;
    for (i_signal=0; i_signal<signal_size; i_signal++)
    {
      t = ((uint64_t) i_signal * scale_x) / 0x10000;
      cur_y = ((((uint64_t) signal[i_signal] * scale_y) / 0x10000) >> DSP16_QB);
      cur_y = height/2 - cur_y - 1;
      // Draw a line between points
      if (i_signal)
      {
        // Draw a line when its angle with the horizontal axis is bellow 45 degrees
        if (Abs(prev_y - cur_y) < t - prev_t)
        {
          int16_t b;
          // delta_y = a * 0x10000
          delta_y = (int32_t) ((prev_y - cur_y) * 0x10000) / (prev_t - t);
          b = cur_y - (delta_y * t) / 0x10000;
          for (i=prev_t; i<t; i++)
          {
            uint16_t temp_y;
            temp_y = (uint16_t) ((delta_y * i) / 0x10000 + b);
            gui_buffer_set_pixel(buffer, width, height, i, temp_y);
          }
        }
        // Draw a line when its angle with the horizontal axis is above 45 degrees
        else
        {
          int16_t b;
          // delta_y = (1 / a) * 0x10000 < 0x10000
          delta_y = (int32_t) ((prev_t - t) * 0x10000) / (prev_y - cur_y);
          b = cur_y - ((int32_t) 0x10000 * t) / delta_y;
          if (prev_y < cur_y)
          {
            for (i=prev_y; i<cur_y; i++)
            {
              uint16_t temp_x;
              temp_x = ((uint32_t) ((i - b) * delta_y)) / 0x10000;
              gui_buffer_set_pixel(buffer, width, height, temp_x, i);
            }
          }
          else
          {
            for (i=cur_y; i<prev_y; i++)
            {
              uint16_t temp_x;
              temp_x = ((uint32_t) ((i - b) * delta_y)) / 0x10000;
              gui_buffer_set_pixel(buffer, width, height, temp_x, i);
            }
          }
        }
      }
      prev_t = t;
      prev_y = cur_y;
    }
  }
}

static void gui_buffer_print_dsp16_bars(void *buffer, uint16_t width, uint16_t height, dsp16_t *signal, uint16_t signal_size)
{
  uint16_t t, cur_y;
  uint32_t scale_x, scale_y;
  uint32_t i_signal;
  uint16_t i;

  // Clean the graph
  memset(buffer, 0, ET024006_BITMAP_WIDTH(width)*height);
  // Compute scaling coefficients
  scale_y = (uint32_t) ((height) * 0x10000) / ((uint32_t) -DSP_FP_MIN(DSP16_QA, DSP16_QB));

  if (signal_size >= width)
  {
    scale_x = (uint32_t) (signal_size * 0x10000) / width;
    for (t=0; t<width; t++)
    {
      i_signal = ((uint64_t) t * scale_x) / 0x10000;
      // Integer value
      cur_y = ((((uint64_t) signal[i_signal] * scale_y) / 0x10000) >> DSP16_QB);
      if (cur_y <= 0)
	cur_y = 1;

      cur_y = height - cur_y;
      if (cur_y <= 0)
	cur_y = 1;

      // Draw vertical lines
      uint16_t temp_y;
      for (temp_y = cur_y; temp_y < height; temp_y++)
        gui_buffer_set_pixel(buffer, width, height, t, temp_y);
    }
  }
  else
  {
    uint16_t prev_t = 0;

    scale_x = (((uint32_t) (width * (signal_size + 1)) / signal_size) * 0x10000) / signal_size;
    for (i_signal=0; i_signal<signal_size; i_signal++)
    {
      t = ((uint64_t) i_signal * scale_x) / 0x10000;
      cur_y = ((((uint64_t) signal[i_signal] * scale_y) / 0x10000) >> DSP16_QB);
      if (cur_y <= 0)
	cur_y = 1;

      cur_y = height - cur_y;
      if (cur_y <= 0)
	cur_y = 1;

      // Draw vertical lines
      for (i = prev_t; i < t; i++)
      {
        uint16_t temp_y;
        for (temp_y = cur_y; temp_y < height; temp_y++)
          gui_buffer_set_pixel(buffer, width, height, i, temp_y);
      }
      prev_t = t;
    }
  }
}
/*
static void gui_et024006_copy_bit_to_screen(void *bitmap, uint32_t count, et024006_color_t foreground_color, et024006_color_t background_color)
{
  uint8_t bit_mask;
#if(ET024006_IFACE_MODE == ET024006_IFACE_MODE_EBI)
  while (count >= 8)
  {
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x01))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x02))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x04))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x08))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x10))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x20))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x40))?foreground_color:background_color;
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), 0x80))?foreground_color:background_color;
    bitmap++;
    count -= 8;
  }
  bit_mask = 1;
  while (count)
  {
    *ET024006_PARAM_ADDR = (Tst_bits(*((uint8_t *) bitmap), bit_mask))?foreground_color:background_color;
    bit_mask <<= 1;
    count--;
  }
#endif
}

static void gui_et024006_bitmap(void *bitmap, uint16_t map_width, uint16_t x, uint16_t y, uint16_t width, uint16_t height, et024006_color_t foreground_color, et024006_color_t background_color)
{
  extern void et024006_SetLimits(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  const int HIMAX_SRAMWRITE = 0x22;
  // Compute bottom right point.
  uint16_t x2 = x + width - 1;
  uint16_t y2 = y + height - 1;
  // Set up draw area.
  et024006_SetLimits(x, y, x2, y2);

  // we need access to the display SRAM now
  *ET024006_CMD_ADDR = (uint16_t) HIMAX_SRAMWRITE;

  // In case of no horizontal pixmap clipping, easier handling is possible.
  if (ET024006_BITMAP_WIDTH(map_width) == map_width)
  {
    // Compute pixel count and copy pixels to screen.
    uint32_t count = (uint32_t) width * height;
    gui_et024006_copy_bit_to_screen(bitmap, count, foreground_color, background_color);
  }
  else
  {
    // Copy line by line to screen.
    uint16_t lines_left = height;
    while (lines_left > 0)
    {
      gui_et024006_copy_bit_to_screen(bitmap, width, foreground_color, background_color);
      bitmap += ET024006_BITMAP_WIDTH(map_width);
      --lines_left;
    }
  }
}
*/
