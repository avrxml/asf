/**
 * \file
 *
 * \brief Sensors Xplained Electronic Compass Calibration Application
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
 *
 * \mainpage
 *
 * \section intro Introduction
 * This is an application illustrating a few basic examples on how to use
 * the \ref mems_sensor_api and \ref gfx_mono_font facilities in the AVR
 * Software Framework.
 *
 * \section files Main Files
 * - inertial_sensor.c Main application module
 * - conf_board.h Evaluation board configuration
 * - conf_st7565r.h ST7565R display controller driver configuration
 * - conf_sysfont.h Monochrome font configuration
 * - conf_twim.h TWI master driver configuration
 * - conf_usart_spi.h USART in SPI master mode configuration
 *
 * \section device_info Device Info
 * This example has been developed and tested on the XMEGA-A3BU Xplained
 * evaluation kit with a Sensors Xplained Inertial 1 board (ATAVRSBIN1)
 * fitted to the J1 and J2 expansion blocks.
 *
 * \section description Description of the example application
 * Upon power up the example application will display brief text greeting
 * on the LCD before guiding the user through a 3-step process that will
 * calibrate the electronic compass on the ATAVRSBIN1 board to the local
 * geomagnetic field.
 *
 * The manual calibration process is simply a matter of orienting the
 * sensor in a particular way and then calling the \ref sensor_calibrate()
 * routine. At each step a message on the monochrome will show directions
 * on the required board orientation. The user moves to the next calibration
 * stop by pressing the \c SW1 button on the evaluation board.
 *
 * Once calibration is complete the calibration values are stored in
 * non-volatile memory on the evaluation board and the application enters
 * a loop that periodically updates a compass needle display along with
 * data on the current magnetic direction, inclination and strength.
 *
 * \section dependencies Dependencies
 * Relevant module dependencies for this application are:
 * - \ref mems_sensor_api
 * - \ref gfx_mono
 * - \ref gfx_mono_font
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "sensor_demo.h"

/**
 * \brief User prompt routine
 *
 * This routine prompts the user to press the button and then waits
 * for him to do so.
 *
 * \return  Nothing.
 */
static void prompt_user(char *prompt_string)
{
	gfx_mono_draw_string(prompt_string, 1, 5, &sysfont);
	gfx_mono_draw_string("and press SW1", 1, 13, &sysfont);

	LED_Off(ALL_LEDS);

	/* Wait for user to push button before continuing. */
	do {
		LED_Toggle(PROMPT_LED);
		delay_ms(500);
	} while (!switch_pressed(SW1));

	LED_Off(PROMPT_LED);

	/* Wait for button release before continuing. */
	while (switch_pressed(SW1)) {
	}

	clear_screen();
}

/**
 * \brief Main entry of example application
 */
int main(void)
{
	/* The sensor_platform_init() function will initialize the system
	 * clock and sensor bus support in addition to configuring the
	 * XMEGA-A3BU and Sensor Xplained boards.
	 *
	 * Use gfx_mono_init() to initialize the monochrome graphical system
	 * API then write a splash screen after enabling the LCD display
	 * backlight and setting the contrast.
	 */
	sensor_platform_init();
	gfx_mono_init();

	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	st7565r_set_contrast(ST7565R_DISPLAY_CONTRAST_MIN);

	gfx_mono_draw_string("Atmel\r\nSensors Xplained\r\nCompass Calibration",
			0, 0, &sysfont);

	delay_ms(2000);

	clear_screen();

	/* Attach descriptors to sensors on an Xplained add-on board. */
	sensor_t compass;
	sensor_attach(&compass, SENSOR_TYPE_COMPASS, 0, 0);

	/* Calibration Step 1. */
	prompt_user("Lay board flat");
	(void)sensor_calibrate(&compass, MANUAL_CALIBRATE, 1, NULL);

	/* Calibration Step 2. */
	prompt_user("Rotate 180");
	(void)sensor_calibrate(&compass, MANUAL_CALIBRATE, 2, NULL);

	/* Calibration Step 3. */
	prompt_user("Flip board");

	if (sensor_calibrate(&compass, MANUAL_CALIBRATE, 3, NULL) != true) {
		if (compass.err == SENSOR_ERR_IO) {
			gfx_mono_draw_string("Calibration failure", 1, 5,
					&sysfont);
			gfx_mono_draw_string("Write Error", 1, 13, &sysfont);
		} else {
			gfx_mono_draw_string("Unknown Error", 1, 5, &sysfont);
			gfx_mono_draw_string("Calibrating Device", 1, 13,
					&sysfont);
		}

		return -1;
	}

	/* Draw rotation units. */
	gfx_mono_draw_string("Hed     Deg        ", 1, 5, &sysfont);
	gfx_mono_draw_string("Inc     Deg        ", 1, 13, &sysfont);
	gfx_mono_draw_string("Fld     uT         ", 1, 21, &sysfont);

	static gfx_coord_t const ring_center_x = 112;
	static gfx_coord_t const ring_center_y = 16;
	static gfx_coord_t const ring_radius   = 15;

	/* Compass needle point x- and y-coordinate. */
	gfx_coord_t needle_x = ring_center_x;
	gfx_coord_t needle_y = ring_center_y;

	/* Draw the compass ring. */
	gfx_mono_draw_circle(ring_center_x, ring_center_y, ring_radius,
			GFX_PIXEL_SET, GFX_WHOLE);

	while (true) {
		static sensor_data_t data;
		sensor_get_heading(&compass, &data);

		/* Calculate compass needle coordinates on the display by using
		 * sin() & cos() to find the x- and y-coordinate of the needle
		 * point. Note that the 'direction' value is in degrees and must
		 * be converted to radians for the C-library math functions.
		 */
		int const needle_length = ring_radius - 3;
		double const direction = radians(data.heading.direction);

		/* Erase the old compass needle. */
		gfx_mono_draw_line(ring_center_x, ring_center_y,
				needle_x, needle_y, GFX_PIXEL_CLR);

		needle_x = ring_center_x +
				(needle_length * sin(direction + M_PI));
		needle_y = ring_center_y +
				(needle_length * cos(direction + M_PI));

		/* Draw the new compass needle. */
		gfx_mono_draw_line(ring_center_x, ring_center_y,
				needle_x, needle_y, GFX_PIXEL_SET);

		LED_Toggle(PROMPT_LED);

		static char format_buffer[3][15];

		sprintf(format_buffer[0], "%4ld", data.heading.direction);
		sprintf(format_buffer[1], "%4ld", data.heading.inclination);
		sprintf(format_buffer[2], "%4ld", data.heading.strength);

		gfx_mono_draw_string(format_buffer[0], 20, 5, &sysfont);
		gfx_mono_draw_string(format_buffer[1], 20, 13, &sysfont);
		gfx_mono_draw_string(format_buffer[2], 20, 21, &sysfont);

		delay_ms(hz_to_ms(REFRESH_PERIOD));
	}
}
