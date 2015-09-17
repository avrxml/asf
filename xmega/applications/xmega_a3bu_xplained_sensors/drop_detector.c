/**
 * \file
 *
 * \brief Sensors Xplained Accelerometer Drop Detection Demo
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
 * the \ref mems_sensor_api along with sensor events and the
 * \ref gfx_mono_font facilities in the ASF.
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
 * Upon power up the example will initialize the monochrome display, system
 * clocks, GPIO mapping, and interfaces to sensors on a Sensors Xplained
 * board attached to the XMEGA-A3BU evaluation board.
 *
 * The acceleration sensor and evaluation board are both put into low-power
 * modes after an accelerometer low-g event is enabled to detect free fall.
 * When the board is dropped, thereby generating a low-g state in Earth's
 * gravitational field, the accelerometer generates an interrupt waking the
 * MCU from sleep state.
 *
 * Upon wake the application will collect a finite set of acceleration vector
 * measurements and plot the results in a graph on the monochrome display
 * after the user pushes the \c SW1 button on the evaluation board. The demo
 * will be reset and ready to detect another low-g event when the user pushes
 * the \c SW1 button a second time.
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
#include <sleepmgr.h>

#define SLEEP_MODE          (SLEEPMGR_PDOWN)       /* sleep mode */
#define LOW_G_THRESHOLD     (250 /* milli-g*/)     /* free fall threshold */
#define LOW_G_SATURATION    (13000 /* milli-g */)  /* low-g saturation val */
#define BANDWIDTH           (1500 /* Hz */)        /* accelerometer bandwidth */
#define RANGE               (8000 /* milli-g */)   /* accelerometer range */
#define ACCEL_LED           LED2                   /* activity indicator */

#define DATA_SAMPLE_COUNT   (128)  /* waveform graph sample set count */
#define SAMPLE_AVG_COUNT    (10)   /* waveform graph sample average count */

/**
 * \brief Example application entry routine
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
	 *
	 * The MCU is going to be put in a sleep mode, so initialize the
	 * sleep manager API with a call to the sleepmgr_init() routine.
	 */
	sensor_platform_init();
	gfx_mono_init();
	sleepmgr_init();

	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	st7565r_set_contrast(ST7565R_DISPLAY_CONTRAST_MIN);

	/* Attach an accelerometer on a Sensors Xplained board. */
	sensor_t accelerometer;
	sensor_attach(&accelerometer, SENSOR_TYPE_ACCELEROMETER, 0, 0);

	/* Enable the accelerometer low-g (free fall) event. */
	sensor_enable_event(&accelerometer, SENSOR_EVENT_LOW_G);

	/* Set the free fall threshold (low-g event), bandwidth and range. */
	sensor_set_threshold(&accelerometer, SENSOR_THRESHOLD_LOW_G,
			LOW_G_THRESHOLD);
	sensor_set_bandwidth(&accelerometer, BANDWIDTH);
	sensor_set_range(&accelerometer, RANGE);

	while (true) {
		/* Put the accelerometer into a low-power mode (if available). */
		sensor_set_state(&accelerometer, SENSOR_STATE_LOW_POWER);

		LED_Off(ACCEL_LED);

		clear_screen();

		/* Display the "armed" message and put the MCU in sleep mode. */
		gfx_mono_draw_string("ATMEL Drop Demo\r\nXMEGA Powered Down\r\n"
				"g Sensor Armed", 1, 5, &sysfont);

		sleepmgr_lock_mode(SLEEP_MODE);
		sleepmgr_enter_sleep();

		/* The following runs after the MCU has been woken by an
		 * external low-g interrupt from the accelerometer.
		 *
		 * Turn on the red LED while falling and put the accelerometer
		 * into a high-power mode (if available) to sample date points.
		 */
		LED_On(ACCEL_LED);

		sensor_set_state(&accelerometer, SENSOR_STATE_HIGHEST_POWER);

		static scalar_t acceleration_waveform[DATA_SAMPLE_COUNT];
		scalar_t acceleration_max = 0;

		for (int data_count = 0; data_count < DATA_SAMPLE_COUNT;
				++data_count) {
			acceleration_waveform[data_count] = 0;

			for (int i = 0; i < SAMPLE_AVG_COUNT; ++i) {
				/* Calculate the gravity vector magnitude. */
				vector3_t gvec;
				sensor_get_vector(&accelerometer, &gvec);

				scalar_t const acceleration_magnitude
					= vector3_magnitude(&gvec);

				/* Store the maximum g magnitude for this
				 * sub-group. */
				if (acceleration_magnitude >
						acceleration_waveform[data_count]) {
					acceleration_waveform[data_count]
						= acceleration_magnitude;
				}

				/* Store the maximum g magnitude for the whole
				 * data set. */
				if (acceleration_magnitude > acceleration_max) {
					acceleration_max = acceleration_magnitude;
				}
			}
		}

		clear_screen();

		/* Turn the max acceleration into a string and convert to g. */
		static char max_g_string[20];

		if (acceleration_max > LOW_G_SATURATION) {
			sprintf(max_g_string, "g Sensor Saturated");
		} else {
			sprintf(max_g_string, "Peak = %02.2f g",
					acceleration_max / 1000);
		}

		/* Print the max g on the monochrome display. */
		gfx_mono_draw_string("Drop Detected", 1, 5, &sysfont);
		gfx_mono_draw_string(max_g_string, 1, 13, &sysfont);
		gfx_mono_draw_string("Press SW1 for chart", 1, 21, &sysfont);

		do {
			LED_Toggle(ACCEL_LED);
			delay_ms(100);
		} while (!switch_pressed(SW1));

		/* Plot the collected data points to create the waveform chart. */
		clear_screen();
		screen_border();

		for (int data_count = 0; data_count < DATA_SAMPLE_COUNT; ++data_count) {
			gfx_mono_draw_filled_circle(data_count,
					32 -
					(acceleration_waveform[data_count] / 500), 1,
					GFX_PIXEL_SET, GFX_WHOLE);
		}

		do {
			LED_Toggle(PROMPT_LED);
			delay_ms(100);
		} while (!switch_pressed(SW1));

		LED_Off(PROMPT_LED);
	}
}
