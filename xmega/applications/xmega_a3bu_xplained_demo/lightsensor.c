/**
 * \file
 *
 * \brief Lightsensor application
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

#include "stdio.h"
#include "adc_sensors.h"
#include "sysfont.h"
#include "bitmaps.h"
#include "keyboard.h"
#include "lightsensor.h"

//! Number of light sensor samples to average
#define LIGHTSENSOR_NUM_SAMPLES 200

/**
 * \brief Lightsensor application
 *
 * This application will output the lightsensor value read by the ADC both as
 * a raw value and by drawing a bar that represent the amount of light hitting
 * the sensor.
 * The raw value is sampled 200 times and the average value is used.
 */
void lightsensor_application(void)
{
	struct keyboard_event input_key;
	char string_buf[10];
	uint32_t lightsensor_val = 0;
	uint8_t scaled;
	uint8_t iterations = 0;

	// Clear screen
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);

	// Put icons at the bottom of the screen indicating light intensity
	gfx_mono_put_bitmap(&bitmap_moon, 1, 24);    // ~1 lux
	gfx_mono_put_bitmap(&bitmap_cloud, 30, 24);  // ~200 lux
	gfx_mono_put_bitmap(&bitmap_indoor, 60, 24); // ~400 lux
	gfx_mono_put_bitmap(&bitmap_sun, 118, 24);   // ~850 lux

	// Draw static strings outside the loop
	gfx_mono_draw_string("Lightsensor", 0, 0, &sysfont);
	gfx_mono_draw_string("Raw value:", 0, 8, &sysfont);

	while (true) {
		// Start an ADC conversion of the lightsensor
		lightsensor_measure();

		while (!lightsensor_data_is_ready()) {
			// Wait until the conversion is complete
		}

		lightsensor_val += lightsensor_get_raw_value();

		// Let's average some samples to be outputted
		if (iterations++ >= LIGHTSENSOR_NUM_SAMPLES) {
			iterations = 0;
			lightsensor_val /= LIGHTSENSOR_NUM_SAMPLES;
			snprintf(string_buf, sizeof(string_buf), "%4ld", lightsensor_val);
			gfx_mono_draw_string(string_buf, 70, 8, &sysfont);
			// Scale down to LCD width for drawing bar
			scaled = lightsensor_val >> 4;

			// Graphic bar representing the light level
			gfx_mono_draw_filled_rect(0, 17,scaled, 7, GFX_PIXEL_SET);
			gfx_mono_draw_filled_rect(scaled, 17,128 - scaled, 7,
					GFX_PIXEL_CLR);

			lightsensor_val = 0;
		}

		// exit if the "back" button has been pressed and released
		keyboard_get_key_state(&input_key);
		if ((input_key.keycode == KEYBOARD_BACK) &&
				(input_key.type == KEYBOARD_RELEASE)) {
			break;
		}
	}
}
