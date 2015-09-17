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

#include <stdarg.h>
#include "board.h"
#include "et024006dhu.h"
#include "conf_demo.h"
#include "gpio.h"
#include "gui.h"
#include "background_image.h"
#include "delay.h"
#include "dsp.h"
#include "compiler.h"
#include "sysclk.h"


//_____ D E F I N I T I O N S ______________________________________________
//! Local structure definition for box
struct gui_box
{
	uint16_t x, y;
	uint16_t width, height;
	et024006_color_t fg_color, bg_color;
};
//! Local structure for GUI timing definition
static struct
{
	t_cpu_time cpu_time;
	uint32_t time_ms;
} gui_fs;


static const struct gui_box box[] = {
	GUI_BOXES
};
//_____ D E C L A R A T I O N S ____________________________________________
//! Local buffer to store GUI content
static uint8_t buffer[ET024006_BITMAP_WIDTH(GUI_MAX_BUFFER_WIDTH)*
	GUI_MAX_BUFFER_HEIGHT];
//! Local buffer to store signal content
static dsp16_t signal_buffer[GUI_BUFFER_LENGTH];

//! Local definition for GUI Update Refresh Screen function
static void gui_change_update_fs(uint32_t time_ms);
//! Local definition to clear GUI screen function
static void gui_clear_view(void);
//! Local definition for GUI Draw Background function
static void gui_draw_background(uint16_t x, uint16_t y, uint16_t width,
	uint16_t height);
//! Local definition to print signal in 16-bit format function
static void gui_buffer_print_dsp16_signal(void *buffer, uint16_t width,
	uint16_t height, dsp16_t *signal, uint16_t signal_size);
//! Local definition to set pixel
static inline void gui_buffer_set_pixel(void *buffer, uint16_t width,
	uint16_t height, uint16_t x, uint16_t y);
//! Local definition to print box
static void gui_print_signal_box(int32_t box_id, dsp16_t *signal,
	uint16_t signal_size, et024006_color_t color);


// External Reference to Sine In buffer in 16-bit fixed point Format
// scaled in GUI format the GUI task
extern dsp16_t signalin_gui[GUI_BUFFER_LENGTH];
// Reference to Noise Signal Buff 16-bit fixed point Format
// scaled in GUI format for the GUI task
extern dsp16_t signal_noise_gui[GUI_BUFFER_LENGTH];
// Reference to Signal In + Noise Signal Buff 16-bit fixed point Format
// scaled in GUI format the GUI task
extern dsp16_t signalin_noise_gui[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through 16-bit fixed point Format
//! scaled in GUI format for the GUI task
extern dsp16_t signalout_fp_gui[GUI_BUFFER_LENGTH];
//! Reference to Filtered Signal through floating point Format
//! scaled in GUI format for the GUI task
extern dsp16_t signalout_fpu_gui[GUI_BUFFER_LENGTH];

static void gui_change_update_fs(uint32_t time_ms)
{
	gui_fs.time_ms = time_ms;
	cpu_set_timeout(cpu_ms_2_cy(gui_fs.time_ms, sysclk_get_main_hz()),
		&gui_fs.cpu_time);
}
static void gui_clear_view(void)
{
	gui_draw_background(0, 0, ET024006_WIDTH, ET024006_HEIGHT);
}

static void gui_print_signal_box(int32_t box_id, dsp16_t *signal,
	uint16_t signal_size, et024006_color_t color)
{
const struct gui_box *sb = &box[box_id];
	signal_size = GUI_BUFFER_LENGTH-25;
	memcpy(signal_buffer, signal, sizeof(dsp16_t)*signal_size);
	gui_buffer_print_dsp16_signal(buffer, sb->width, sb->height,
		signal_buffer, signal_size);
	et024006_PutBitmap(buffer, sb->width, sb->x, sb->y, sb->width, sb->height,
		color, sb->bg_color);
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

/*! \brief Gui Task Init:
*      - Initialize the LCD Display
*/
void gui_task_init(void)
{
	// Init the delay function
	delay_init(sysclk_get_main_hz());
	// Init the display
	et024006_Init(sysclk_get_main_hz(), sysclk_get_main_hz());
	gpio_set_gpio_pin(ET024006DHU_BL_PIN);
	// Draw the background
	gui_clear_view();
	// Set the frequency rate of the update of the display
	gui_change_update_fs(GUI_DEFAULT_SCREEN_UPDATE_FS_MS);
}

/*! \brief Gui Task:
*      - Refresh the content of the screen
*/
void gui_task(void)
{
	extern volatile int16_t potentiometer_value;
	extern volatile uint8_t signal_selected_display;
	static int16_t cursor_pos = 0;
	int16_t new_cursor_pos;

	// Check the last time the printing has been done
	if (cpu_is_timeout(&gui_fs.cpu_time)) {
		if (signal_selected_display == 1) {
			gui_print_signal_box(GUI_SOURCE1_ID, signalin_gui,
				sizeof(signalin_gui)/sizeof(signalin_gui[0]), BLACK);
		}
		if (signal_selected_display==2) {
			gui_print_signal_box(GUI_SOURCE1_ID, signal_noise_gui,
				sizeof(signal_noise_gui)/sizeof(signal_noise_gui[0]), GREEN);
		}
		if (signal_selected_display==3) {
		gui_print_signal_box(GUI_SOURCE1_ID, signalin_noise_gui,
			sizeof(signalin_noise_gui)/sizeof(signalin_noise_gui[0]), RED);
		}
		if (signal_selected_display==4) {
		gui_print_signal_box(GUI_SOURCE1_ID, signalout_fp_gui,
			sizeof(signalout_fp_gui)/sizeof(signalout_fp_gui[0]), RED);
		}
		if (signal_selected_display==5) {
		gui_print_signal_box(GUI_SOURCE1_ID, signalout_fpu_gui,
			sizeof(signalout_fpu_gui)/sizeof(signalout_fpu_gui[0]), BLUE);
		}
		gui_print_signal_box(GUI_OUTPUT1_ID, signal_noise_gui,
			sizeof(signal_noise_gui)/sizeof(signal_noise_gui[0]), GREEN);
		cpu_set_timeout(cpu_ms_2_cy(gui_fs.time_ms, sysclk_get_main_hz()),
			&gui_fs.cpu_time);
	}
	// normalize the cursor range
	new_cursor_pos = (int16_t)((potentiometer_value *
		(int16_t)GUI_SOURCE1_HEIGHT) / (int16_t)512);// 10-bit differential conv.
	if (new_cursor_pos != cursor_pos) {
		// trigger changed, just draw a simple cursor  rectangle on the left
		// side of the display clear the old position
		et024006_DrawFilledRect(GUI_SOURCE1_X - 7, GUI_SOURCE1_HEIGHT -
			cursor_pos, 5, 3, BLACK);
		// display new position
		et024006_DrawFilledRect(GUI_SOURCE1_X - 7, GUI_SOURCE1_HEIGHT -
			new_cursor_pos, 5, 3, GUI_CURSOR_COLOR);
		// save new position
		cursor_pos = new_cursor_pos;
	}
}
