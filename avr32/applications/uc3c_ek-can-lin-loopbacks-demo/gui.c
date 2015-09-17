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

#include "compiler.h"

struct gui_box
{
  uint16_t x, y;
  uint16_t width, height;
  et024006_color_t fg_color, bg_color;
};

static void gui_draw_background(uint16_t x, uint16_t y, uint16_t width,
		uint16_t height);
static void gui_buffer_print_dsp16_signal(void *buffer, uint16_t width,
		uint16_t height, dsp16_t *signal, uint16_t signal_size);
static inline void gui_buffer_set_pixel(void *buffer, uint16_t width,
		uint16_t height, uint16_t x, uint16_t y);
static void gui_print_signal_box(int32_t box_id, dsp16_t *signal,
		uint16_t signal_size);

static const struct gui_box box[] = {
	GUI_BOXES
};

#define MAX_BUFFER_WIDTH    GUI_ZOOM_BOX_WIDTH
#define MAX_BUFFER_HEIGHT   GUI_ZOOM_BOX_HEIGHT

static uint8_t buffer[ ET024006_BITMAP_WIDTH(MAX_BUFFER_WIDTH)* \
	MAX_BUFFER_HEIGHT];
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
extern dsp16_t signal3_buf[BUFFER_LENGTH];
extern dsp16_t signal4_buf[BUFFER_LENGTH];
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

void gui_init(uint32_t cpu_hz,
		uint32_t hsb_hz,
		uint32_t pba_hz,
		uint32_t pbb_hz)
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
  // Check the last time the printing has been done
	if (cpu_is_timeout(&gui_fs.cpu_time)) {
		gui_print_signal_box(GUI_SOURCE1_ID, signal1_buf,
			sizeof(signal1_buf)/sizeof(signal1_buf[0]));
		gui_print_signal_box(GUI_OUTPUT3_ID, signal2_buf,
			sizeof(signal2_buf)/sizeof(signal2_buf[0]));
		gui_print_signal_box(GUI_OUTPUT2_ID, signal3_buf,
			sizeof(signal3_buf)/sizeof(signal3_buf[0]));
		gui_print_signal_box(GUI_OUTPUT1_ID, signal4_buf,
			sizeof(signal4_buf)/sizeof(signal4_buf[0]));
		cpu_set_timeout(cpu_ms_2_cy(gui_fs.time_ms, gui_cpu_hz),
			&gui_fs.cpu_time);
  }
}

void gui_set_selection(int32_t box_id)
{
static int32_t prev_selection = GUI_NO_SEL;
struct gui_box *sb;
uint32_t i;

  // Clear previous selections
	if (prev_selection != GUI_NO_SEL && prev_selection != box_id) {
    i = prev_selection;
    sb = (struct gui_box *) &box[i];
    // Don't do anything if the color is transparent
		if (sb->bg_color == GUI_NO_COLOR) {
			gui_draw_background(sb->x - 2,
				sb->y - 2,
				sb->width + 4,
				2);
			gui_draw_background(sb->x - 2,
				sb->y + sb->height,
				sb->width + 4,
				2);
			gui_draw_background(sb->x - 2,
				sb->y - 2,
				2,
				sb->height + 4);
			gui_draw_background(sb->x + sb->width,
				sb->y - 2,
				2,
				sb->height + 4);
		}
		else {
			et024006_DrawFilledRect(sb->x - 2,
				sb->y - 2,
				sb->width + 4,
				2,
				sb->bg_color);
			et024006_DrawFilledRect(sb->x - 2,
				sb->y + sb->height,
				sb->width + 4,
				2,
				sb->bg_color);
			et024006_DrawFilledRect(sb->x - 2,
				sb->y - 2,
				2,sb->height + 4,
				sb->bg_color);
			et024006_DrawFilledRect(sb->x + sb->width,
				sb->y - 2,
				2,
				sb->height + 4,
				sb->bg_color);
    }
  }

  prev_selection = box_id;
	if (box_id != GUI_NO_SEL) {
  // Draw the selection
  sb = (struct gui_box *) &box[box_id];
		et024006_DrawFilledRect( sb->x - 2,
			sb->y - 2,
			sb->width + 4,
			2,
			GUI_SELECTION_COLOR);
		et024006_DrawFilledRect(sb->x - 2,
			sb->y + sb->height,
			sb->width + 4,
			2,
			GUI_SELECTION_COLOR);
		et024006_DrawFilledRect(sb->x - 2,
			sb->y - 2,
			2,
			sb->height + 4,
			GUI_SELECTION_COLOR);
		et024006_DrawFilledRect(sb->x + sb->width,
			sb->y - 2,
			2,
			sb->height + 4,
			GUI_SELECTION_COLOR);
	}
}


static void gui_print_signal_box(int32_t box_id, dsp16_t *signal,
		uint16_t signal_size)
{
const struct gui_box *sb = &box[box_id];
extern volatile bool zoom_view;
extern volatile int32_t zoom_view_id;

	if (zoom_view && zoom_view_id != box_id)
		return;

	signal_size = Min(signal_size, BUFFER_LENGTH);

	memcpy(signal_buffer, signal, sizeof(dsp16_t)*signal_size);

	if (zoom_view && zoom_view_id == box_id) {
		gui_buffer_print_dsp16_signal(buffer,
			GUI_ZOOM_BOX_WIDTH,
			GUI_ZOOM_BOX_HEIGHT,
			signal_buffer,
			signal_size);
	}
	else {
		gui_buffer_print_dsp16_signal(buffer,
			sb->width,
			sb->height,
			signal_buffer,
			signal_size);
	}
	if (zoom_view && zoom_view_id == box_id) {
		et024006_PutBitmap(buffer,
			GUI_ZOOM_BOX_WIDTH,
			GUI_ZOOM_BOX_X,
			GUI_ZOOM_BOX_Y,
			GUI_ZOOM_BOX_WIDTH,
			GUI_ZOOM_BOX_HEIGHT,
			sb->fg_color,
			sb->bg_color);
	}
	else {
		et024006_PutBitmap(buffer,
			sb->width,
			sb->x,
			sb->y,
			sb->width,
			sb->height,
			sb->fg_color,
			sb->bg_color);
	}
}

static void gui_draw_background(uint16_t x, uint16_t y, uint16_t width,
		uint16_t height)
{
  et024006_PutPixmap(background_image,
	ET024006_WIDTH,
	x,
	y,
	x,
	y,
	width,
	height);
}

static inline void gui_buffer_set_pixel(void *buffer, uint16_t width,
		uint16_t height, uint16_t x, uint16_t y)
{
  uint8_t *u8_buffer = (uint8_t *) buffer;
  if (x >= width || y >= height)
    return;
  u8_buffer += y * ET024006_BITMAP_WIDTH(width) + x / 8;
  *u8_buffer = *u8_buffer | (1 << (x & 7));
}

static void gui_buffer_print_dsp16_signal(void *buffer, uint16_t width,
		uint16_t height, dsp16_t *signal, uint16_t signal_size)
{
uint16_t t, cur_y;
uint32_t scale_x, scale_y;
uint32_t i_signal;
uint16_t i, last_y = 0;

	// Clean the graph
	memset(buffer, 0, ET024006_BITMAP_WIDTH(width)*height);
	// Compute scaling coefficients
	scale_y = (uint32_t) ((height/2) * 0x10000) / \
		((uint32_t) -DSP_FP_MIN(DSP16_QA, DSP16_QB));

	if (signal_size >= width) {
		// Speed drawing
		scale_x = (uint32_t) (signal_size * 0x10000) / width;
		for (t=0; t<width; t++) {
			i_signal = ((uint64_t) t * scale_x) / 0x10000;
			// Integer value
			cur_y = ((((uint64_t) signal[i_signal] * scale_y) / \
				0x10000) >> DSP16_QB);
			cur_y = height/2 - cur_y - 1;
			if (last_y <= cur_y && t) {
				for (i=last_y; i<=cur_y; i++)
					gui_buffer_set_pixel(buffer,
						width,
						height,
						t,
						i);
			}
			else if (t) {
				for (i=cur_y; i<=last_y; i++)
				  gui_buffer_set_pixel(buffer,
					width,
					height,
					t,
					i);
			}
			last_y = cur_y;
		}
	}
	else {
		uint16_t prev_t = 0, prev_y = 0;
		int32_t delta_y;

		// Fine drawing
		scale_x = (((uint32_t) (width * (signal_size + 1)) / \
			signal_size) * 0x10000) / signal_size;
		for (i_signal=0; i_signal<signal_size; i_signal++) {
			t = ((uint64_t) i_signal * scale_x) / 0x10000;
			cur_y = ((((uint64_t) signal[i_signal] * scale_y) / \
				0x10000) >> DSP16_QB);
			cur_y = height/2 - cur_y - 1;
			// Draw a line between points
			if (i_signal) {
				// Draw a line when its angle with the horizontal axis is
				// < 45 degrees
				if (Abs(prev_y - cur_y) < t - prev_t) {
					int16_t b;
					// delta_y = a * 0x10000
					delta_y = (int32_t) ((prev_y - cur_y) * 0x10000) / \
						(prev_t - t);
					b = cur_y - (delta_y * t) / 0x10000;
					for (i=prev_t; i<t; i++) {
						uint16_t temp_y;
						temp_y = (uint16_t) ((delta_y * i) / 0x10000 + b);
						gui_buffer_set_pixel(buffer,
							width,
							height,
							i,
							temp_y);
					}
				}
				// Draw a line when its angle with the horizontal axis is
				// > 45 degrees
				else {
					int16_t b;
					// delta_y = (1 / a) * 0x10000 < 0x10000
					delta_y = (int32_t) ((prev_t - t) * 0x10000) / \
						(prev_y - cur_y);
					b = cur_y - ((int32_t) 0x10000 * t) / delta_y;
					if (prev_y < cur_y) {
						for (i=prev_y; i<cur_y; i++) {
							uint16_t temp_x;
							temp_x = ((uint32_t) ((i - b) * delta_y)) / 0x10000;
							gui_buffer_set_pixel(buffer,
								width,
								height,
								temp_x,
								i);
						}
					}
					else
					{
						for (i=cur_y; i<prev_y; i++) {
							uint16_t temp_x;
							temp_x = ((uint32_t) ((i - b) * delta_y)) / 0x10000;
							gui_buffer_set_pixel(buffer,
								width,
								height,
								temp_x,
								i);
						}
					  }
				}
			}
			prev_t = t;
			prev_y = cur_y;
		}
	}
}
