/**
 * \file
 *
 * \brief Sensors Xplained Inertial Sensor Data Display Example
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
 
/**
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
 * on the LCD before sampling and printing formatted data obtained from
 * the sensors on the ATAVRSBIN1 board.
 *
 * Data from only one particular sensor is shown. Use the \c SW1 button
 * on the evaluation kit board to cycle through outputs from different
 * sensors. The 3-axis accelerometer values are initially displayed when
 * the board is powered up. Pressing \c SW1 will cycle through displays
 * showing data for the gyroscope, electronic compass and finally the
 * temperature sensor before returning again to the accelerometer output
 * display
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
#include <asf.h>
#include "sensor_demo.h"

enum format_string_select {
	acceleration_format = 0,
	rotation_format     = 3,
	heading_format      = 6,
	temperature_format  = 9
};

static void draw_formatted_data(enum format_string_select sel,
		const sensor_data_t *data)
{
	static char format_buffer[3][17];

	static const char *format[] = {
		/* Acceleration */
		"X %5ld Milli-G ",
		"Y %5ld Milli-G ",
		"Z %5ld Milli-G ",

		/* Angular Rate */
		"X %5ld Deg/sec ",
		"Y %5ld Deg/sec ",
		"Z %5ld Deg/sec ",

		/* Magnetic Heading */
		"Hed %4ld Deg    ",
		"Inc %4ld Deg    ",
		"Fld %4ld uT     ",

		/* Temperature */
		"%4ld Fahrenheit ",
		"%4ld Celsius    ",
		"                "
	};

	LED_Toggle(PROMPT_LED);

	sprintf(format_buffer[0], format[sel + 0], data->value[0]);
	sprintf(format_buffer[1], format[sel + 1], data->value[1]);
	sprintf(format_buffer[2], format[sel + 2], data->value[2]);

	gfx_mono_draw_string(format_buffer[0], 1, 5, &sysfont);
	gfx_mono_draw_string(format_buffer[1], 1, 13, &sysfont);
	gfx_mono_draw_string(format_buffer[2], 1, 21, &sysfont);

	delay_ms(hz_to_ms(REFRESH_PERIOD));
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

	gfx_mono_draw_string(
			"Atmel\r\nSensors Xplained\r\nInertial Sensor Demo",
			0, 0, &sysfont);

	delay_ms(2000);

	clear_screen();

	gfx_mono_draw_string("Press button SW1", 1, 5, &sysfont);
	gfx_mono_draw_string("to change sensor", 1, 13, &sysfont);

	delay_ms(1000);

	screen_border();

	/* Attach descriptors to sensors on an Xplained add-on board. */
	sensor_t accelerometer;
	sensor_attach(&accelerometer, SENSOR_TYPE_ACCELEROMETER, 0, 0);

	sensor_t gyroscope;
	sensor_attach(&gyroscope, SENSOR_TYPE_GYROSCOPE, 0, 0);

	sensor_t compass;
	sensor_attach(&compass, SENSOR_TYPE_COMPASS, 0, 0);

	/* Configure operational parameters for select sensors. */
	sensor_set_range(&accelerometer, 2000 /* milli-g */);
	sensor_set_bandwidth(&accelerometer, 25 /* Hertz */);

	while (true) {
		/* Initialize a sensor data descriptor for scaled data. */
		static sensor_data_t sensor_data = {.scaled = true};

		do {
			/* Measure & show acceleration until button SW1 is
			 * pushed. */
			sensor_read(&accelerometer, SENSOR_READ_ACCELERATION, &sensor_data);
			draw_formatted_data(acceleration_format, &sensor_data);
		} while (!switch_pressed(SW1));

		do {
			/* Measure & show rotation until button SW1 is pushed. */
			sensor_read(&gyroscope, SENSOR_READ_ROTATION, &sensor_data);
			draw_formatted_data(rotation_format, &sensor_data);
		} while (!switch_pressed(SW1));

		static gfx_coord_t const ring_center_x = 112;
		static gfx_coord_t const ring_center_y = 16;
		static gfx_coord_t const ring_radius   = 15;

		/* Compass needle point x- and y-coordinate. */
		gfx_coord_t needle_x = ring_center_x;
		gfx_coord_t needle_y = ring_center_y;

		/* Draw the compass ring. */
		gfx_mono_draw_circle(ring_center_x, ring_center_y, ring_radius,
				GFX_PIXEL_SET, GFX_WHOLE);

		do {
			/* Measure & show magnetic heading until button SW1 is
			 * pushed.
			 */
			sensor_read(&compass, SENSOR_READ_HEADING, &sensor_data);

			/* Calculate compass needle coordinates on the display
			 * by using sin() & cos() to find the x- and y-coordinate of the
			 * needle point. Note that the 'direction' value is in degrees
			 * and must be converted to radians for the C-library math
			 * functions.
			 */
			int const needle_length = ring_radius - 3;
			double const direction  = radians(sensor_data.heading.direction);

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

			draw_formatted_data(heading_format, &sensor_data);
		} while (!switch_pressed(SW1));

		/* Clear last compass needle and compass ring. */
		gfx_mono_draw_line(ring_center_x, ring_center_y,
				needle_x, needle_y, GFX_PIXEL_CLR);

		gfx_mono_draw_circle(ring_center_x, ring_center_y, ring_radius,
				GFX_PIXEL_CLR, GFX_WHOLE);

		screen_border();

		do {
			/* Measure & show temperature until button SW1 is pushed. */
			sensor_read(&gyroscope, SENSOR_READ_TEMPERATURE, &sensor_data);

			/* Arrange temperature data values in array with
			 * Fahrenheit and Celsius formats.
			 */
			int32_t const temp_deg_c = sensor_data.temperature.value;
			int32_t const temp_deg_f = CELSIUS_TO_FAHRENHEIT(temp_deg_c);

			sensor_data.value[0] = temp_deg_f;
			sensor_data.value[1] = temp_deg_c;

			draw_formatted_data(temperature_format, &sensor_data);
		} while (!switch_pressed(SW1));
	}
}
