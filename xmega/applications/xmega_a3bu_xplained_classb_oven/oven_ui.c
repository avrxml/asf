/**
 * \file
 *
 * \brief Class B Oven User Interface
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include "oven.h"
#include "oven_ui.h"
#include "oven_bitmaps.h"

/**
 * \brief Update user interface elements
 *
 * Draw bitmap representing state of Q-Touch button as a pot on a plate, and a
 * vertical bar representing the selected power level and whether the plate
 * element is being actuated.
 *
 * \param potstate Whether a pot is detected
 * \param wattage Power level
 * \param power Whether power is applied
 */
void oven_ui_update_graphics(enum pot_t potstate, uint8_t wattage, bool power)
{
	int16_t barheight;

	/* Draw pot on/off graphic */
	if (potstate == POT_ON) {
		gfx_mono_put_bitmap(&bitmap_pot_on, 114, 16);
	} else {
		gfx_mono_put_bitmap(&bitmap_pot_off, 114, 16);
	}

	/* Draw power bar */
	barheight = (int)wattage * 31 / MAX_WATTAGE;
	gfx_mono_draw_filled_rect(123, 0, 6, 32, GFX_PIXEL_CLR);
	gfx_mono_draw_filled_rect(123, 31 - barheight, 6, barheight,
			GFX_PIXEL_SET);

	/* Hollow power bar if power is off. */
	if (!power) {
		gfx_mono_draw_filled_rect(124, min(31, 31 - barheight + 1), 3,
				max(0, barheight - 2), GFX_PIXEL_CLR);
	}
}

/**
 * \brief Plot a simulation time-step of the cooking plate and pot contents.
 *
 * The input values must be between 0 and 32 to fit inside the display.
 *
 * \param y1 Temperature of pot
 * \param y2 Temperature of plate
 */
void oven_ui_draw_plot_step(const uint8_t y1, const uint8_t y2)
{
	static uint8_t xpos = OVEN_PLOT_OFFSET_X;
	static uint8_t last_ypos = OVEN_DISP_MAX_Y / 2;
	static uint8_t last_ypos2 = OVEN_DISP_MAX_Y / 2;
	double temp = y1;
	double temp_plate = y2;

	/* Clear existing pixels at current position */
	gfx_mono_draw_filled_rect(xpos, OVEN_PLOT_MIN_Y, 2, OVEN_PLOT_MAX_Y,
			GFX_PIXEL_CLR);

	/* Draw dotted line of pot temperature */
	if (!(xpos % 3)) {
		gfx_mono_generic_draw_line(xpos > OVEN_PLOT_OFFSET_X
				? (xpos - 1) : OVEN_PLOT_OFFSET_X, last_ypos,
				xpos, OVEN_DISP_MAX_Y - temp, GFX_PIXEL_SET);
	}

	/* Draw solid line of plate temperature */
	gfx_mono_generic_draw_line(xpos > OVEN_PLOT_OFFSET_X
			? (xpos - 1) : OVEN_PLOT_OFFSET_X, last_ypos2, xpos,
			OVEN_DISP_MAX_Y - temp_plate, GFX_PIXEL_SET);

	/* Save current y-position to be able to draw a line at the next
	 * simulation step
	 */
	last_ypos = OVEN_DISP_MAX_Y - temp;
	last_ypos2 = OVEN_DISP_MAX_Y - temp_plate;

	/* Redraw axis at beginning */
	if (xpos < (OVEN_PLOT_OFFSET_X + 2)) {
		oven_ui_draw_axis();
	}

	/* Wrap drawing window between OVEN_PLOT_OFFSET_X and OVEN_PLOT_MAX_X */
	if ((++xpos + OVEN_PLOT_OFFSET_X)
			> (OVEN_PLOT_MAX_X - OVEN_PLOT_OFFSET_X)) {
		xpos = OVEN_PLOT_OFFSET_X;
	}
}

/**
 * \brief Draw vertical and horizontal axis
 */
void oven_ui_draw_axis(void)
{
	/* Vertical */
	gfx_mono_generic_draw_line(3, 0, 3, 29, GFX_PIXEL_SET);
	gfx_mono_generic_draw_line(3, 0, 0, 3, GFX_PIXEL_SET);
	gfx_mono_generic_draw_line(3, 0, 6, 3, GFX_PIXEL_SET);

	/* Horizontal */
	gfx_mono_generic_draw_line(3, 29, 112, 29, GFX_PIXEL_SET);
	gfx_mono_generic_draw_line(112, 29, 109, 32, GFX_PIXEL_SET);
	gfx_mono_generic_draw_line(112, 29, 109, 26, GFX_PIXEL_SET);
}

/**
 * \brief Report whether the power button has been pressed and released
 *
 * \return true if the power button has been pressed and released
 */
bool oven_ui_power_button_is_pressed(void)
{
	static bool down = false;

	/* Set state to down if pressed down */
	if (gpio_pin_is_low(GPIO_PUSH_BUTTON_1)) {
		down = true;
	}

	/* Return true if the button was pressed and is now not pressed. */
	if (gpio_pin_is_high(GPIO_PUSH_BUTTON_1) && down) {
		down = false;
		return true;
	}

	return false;
}

/**
 * \brief Report whether the back button has been pressed and released
 *
 * \return true if the back button has been pressed and released
 */
bool oven_ui_back_button_is_pressed(void)
{
	static bool bdown = false;

	/* Set state to down if pressed down */
	if (gpio_pin_is_low(GPIO_PUSH_BUTTON_0)) {
		bdown = true;
	}

	/* Return true if the button was pressed and is now not pressed. */
	if (gpio_pin_is_high(GPIO_PUSH_BUTTON_0) && bdown) {
		bdown = false;
		return true;
	}

	return false;
}

/**
 * \brief Set the status LED to specified color
 */
void oven_ui_set_status_leds(enum status_led s)
{
	PORTD.OUTCLR = 0x03 << PIN4_bp;
	PORTD.OUTSET = s << PIN4_bp;
}
