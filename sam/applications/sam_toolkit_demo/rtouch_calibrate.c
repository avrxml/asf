/**
 * \file
 *
 * \brief Resistive touch calibration for SAM toolkit demo.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "rtouch.h"
#include "rtouch_calibrate.h"
#include <asf.h>
#include "demo_parameters.h"
#include "demo.h"

/** Size in pixels of calibration points. */
#define POINTS_SIZE         4

/** Number of touch points for the calibration */
#define NUM_TOUCH_POINTS         5

const char string_calib_name[] = "LCD calibration";
const char string_calib_instruction[] = "Touch the dots to\ncalibrate the screen";
const char string_calib_done[] ="Calibration done.";
const char string_calib_failed[] = "Calibration failed!";
const char string_calib_retry[] = "Please try again...";

/* White background color */
#define BGCOLOR GFX_COLOR_WHITE
#define DOTCOLOR GFX_COLOR_RED
#define TXTCOLOR GFX_COLOR_BLUE

/** Calibration points. */
static rtouch_calibration_point_t gs_calibration_points[] = {
	/* Top-left corner calibration point */
	{
		{LCD_WIDTH / 10, LCD_HEIGHT / 10},
		{0,0}
	},
	/* Top-right corner calibration point */
	{
		{LCD_WIDTH - LCD_WIDTH / 10, LCD_HEIGHT / 10},
		{0,0}
	},
	/* Bottom-right corner calibration point */
	{
		{LCD_WIDTH - LCD_WIDTH / 10, LCD_HEIGHT - LCD_HEIGHT / 10},
		{0,0}
	},
	/* Bottom-left corner calibration point */
	{
		{LCD_WIDTH / 10, LCD_HEIGHT - LCD_HEIGHT / 10},
		{0,0}
	},
	/* Test calibration point */
	{
		{LCD_WIDTH / 2, LCD_HEIGHT / 2},
		{0,0}
	}
};

/**
 * \brief Display a calibration point on the given buffer.
 *
 * \param p_point  Calibration point to display.
 */
static void draw_calibration_point(const rtouch_point_t *p_point)
{
	gfx_draw_filled_rect(p_point->x - POINTS_SIZE / 2,
			p_point->y - POINTS_SIZE / 2,
			POINTS_SIZE,
			POINTS_SIZE,
			DOTCOLOR
	);
}

/**
 * \brief Clears a calibration point from the given buffer.
 *
 * \param p_point  Calibration point to clear.
 */
static void clear_calibration_point(const rtouch_point_t *p_point)
{
	gfx_draw_filled_rect(p_point->x - POINTS_SIZE / 2,
			p_point->y - POINTS_SIZE / 2,
			POINTS_SIZE,
			POINTS_SIZE,
			BGCOLOR
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
	/* Fill the whole screen with the background color */
	gfx_draw_filled_rect(0, 0, gfx_get_width(), gfx_get_height(), BGCOLOR);
	/* Print user instructions */
	/* Write center-aligned text string to the top of the display */
	gfx_draw_string_aligned(string_calib_name,
			gfx_get_width() / 2, 60, &sysfont,
			GFX_COLOR_WHITE, TXTCOLOR,
			TEXT_POS_CENTER, TEXT_ALIGN_LEFT);

	gfx_draw_string_aligned(string_calib_instruction,
		gfx_get_width() / 2, 190, &sysfont,
		GFX_COLOR_WHITE, TXTCOLOR,
		TEXT_POS_CENTER, TEXT_ALIGN_LEFT);

	/* Calibration points */
	for (i = 0; i < NUM_TOUCH_POINTS; i++) {
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

	/* Check if the points acceptable */
	if (rtouch_compute_calibration((rtouch_calibration_point_t *) &gs_calibration_points) == 0) {

		for (i=0;i<NUM_TOUCH_POINTS;i++) {
			g_demo_parameters.calib_points[i].raw.x = gs_calibration_points[i].raw.x;
			g_demo_parameters.calib_points[i].raw.y = gs_calibration_points[i].raw.y;
		}

		/* Commit changes to the parameter area */
		demo_parameters_commit_changes();

		/* Display calibration done string */
		gfx_draw_string_aligned(string_calib_done,
				gfx_get_width() / 2, 130, &sysfont,
				GFX_COLOR_WHITE, TXTCOLOR,
				TEXT_POS_CENTER, TEXT_ALIGN_LEFT);

		return 0;

	} else {
		/* Clear instruction string first */
		gfx_draw_string_aligned(string_calib_instruction,
			gfx_get_width() / 2, 190, &sysfont,
			GFX_COLOR_WHITE, GFX_COLOR_WHITE,
			TEXT_POS_CENTER, TEXT_ALIGN_LEFT);

		/* Show up failed string */
		gfx_draw_string_aligned(string_calib_failed,
			gfx_get_width() / 2, 100, &sysfont,
			GFX_COLOR_WHITE, TXTCOLOR,
			TEXT_POS_CENTER, TEXT_ALIGN_LEFT);

		/* Display retry string */
		gfx_draw_string_aligned(string_calib_retry,
			gfx_get_width() / 2, 190, &sysfont,
			GFX_COLOR_WHITE, TXTCOLOR,
			TEXT_POS_CENTER, TEXT_ALIGN_LEFT);

		/* Wait for contact loss */
		rtouch_wait_released();
		return 1;
	}
}
