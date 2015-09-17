/**
 * \file
 *
 * \brief Resistive Touch driver.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "asf.h"
#include "rtouch.h"
#include "rtouch_calibrate.h"
#include "resistive_touch_board.h"

/** Size in pixels of calibration points. */
#define POINTS_SIZE         4

/** Calibration points. */
static rtouch_calibration_point_t gs_calibration_points[] = {
	/* Top-left corner calibration point */
	{
		{LCD_WIDTH / 10,LCD_HEIGHT / 10},
		{0, 0}
	},
	/* Top-right corner calibration point */
	{
		{LCD_WIDTH - LCD_WIDTH / 10, LCD_HEIGHT / 10},
		{0, 0}
	},
	/* Bottom-right corner calibration point */
	{
		{LCD_WIDTH - LCD_WIDTH / 10, LCD_HEIGHT - LCD_HEIGHT / 10},
		{0, 0}
	},
	/* Bottom-left corner calibration point */
	{
		{LCD_WIDTH / 10, LCD_HEIGHT - LCD_HEIGHT / 10},
		{0, 0}
	},
	/* Test calibration point */
	{
		{LCD_WIDTH / 2, LCD_HEIGHT / 2},
		{0, 0}
	}	
};

/**
 * \brief Display a calibration point on the given buffer.
 *
 * \param p_point  Calibration point to display.
 */
static void draw_calibration_point(const rtouch_point_t *p_point)
{
	lcd_set_foreground_color(UNI_COLOR_RED);
	lcd_draw_filled_rectangle(p_point->x - POINTS_SIZE / 2,
			p_point->y - POINTS_SIZE / 2,
			p_point->x - POINTS_SIZE / 2 + POINTS_SIZE,
			p_point->y - POINTS_SIZE / 2 + POINTS_SIZE
	);
}

/**
 * \brief Clears a calibration point from the given buffer.
 *
 * \param p_point  Calibration point to clear.
 */
static void clear_calibration_point(const rtouch_point_t *p_point)
{
	lcd_set_foreground_color(UNI_COLOR_WHITE);
	lcd_draw_filled_rectangle(p_point->x - POINTS_SIZE / 2,
			p_point->y - POINTS_SIZE / 2,
			p_point->x - POINTS_SIZE / 2 + POINTS_SIZE,
			p_point->y - POINTS_SIZE / 2 + POINTS_SIZE
	);
}

/**
 * \brief Starts the calibration routines and displays user instructions on screen.
 *
 * \return 0 if calibration is successful, else 1.
 */
uint32_t rtouch_calibrate(void)
{
	uint32_t i;
	uint32_t x, y;

	/* Print user instructions */
	lcd_fill(UNI_COLOR_WHITE);
	lcd_set_foreground_color(UNI_COLOR_BLACK);
	lcd_draw_string(30, 60, (uint8_t *)"LCD calibration");
	lcd_draw_string(1, 190, (uint8_t *)"Touch the dots to\ncalibrate the screen");

	/* Calibration points */
	for (i = 0; i < 5; i++) {
		draw_calibration_point(&gs_calibration_points[i].panel);

		/* Wait for touch & end of conversion */
		rtouch_wait_pressed();
		
		rtouch_get_raw_point(&x, &y);
		gs_calibration_points[i].raw.x = x;
		gs_calibration_points[i].raw.y = y;
		clear_calibration_point(&gs_calibration_points[i].panel);

		/* Wait for contact loss */
		rtouch_wait_released();
	}
	
	if (rtouch_compute_calibration((rtouch_calibration_point_t *) &gs_calibration_points) == 0) {
		lcd_fill(UNI_COLOR_WHITE);
		lcd_set_foreground_color(UNI_COLOR_BLACK);
		lcd_draw_string(20, 130, (uint8_t *)"Calibration done.");
		return 0;
	} else {
		lcd_fill(UNI_COLOR_WHITE);
		lcd_set_foreground_color(UNI_COLOR_BLACK);
		lcd_draw_string(10, 100, (uint8_t *)"Calibration failed!");
		lcd_draw_string(10, 190, (uint8_t *)"Please try again...");
		
		/* Wait for contact loss */
		rtouch_wait_released();
		return 1;
	}
}
