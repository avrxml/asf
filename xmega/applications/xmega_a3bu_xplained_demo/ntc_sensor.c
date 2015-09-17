/**
 * \file
 *
 * \brief NTC application
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
#include "keyboard.h"
#include "ntc_sensor.h"

//! The X position of the temperature image on screen
#define X_POS 10

//! Flash text for the heading
PROGMEM_DECLARE(char const, header[]) = "Temperature";

//! The thermometer image
uint8_t tempscale_data[] = {
		0x01, 0xf9, 0xfd, 0xfd, 0xf9, 0x01,
		0x41, 0xff, 0xff, 0xff, 0xff, 0x41,
		0x10, 0xff, 0xff, 0xff, 0xff, 0x10,
		0x9e, 0xbf, 0xbf, 0xbf, 0xbf, 0x9e
};

/**
 * \brief The NTC Temperature application
 */
void ntc_sensor_application(void)
{
	// Struct to hold the keyboard event
	struct keyboard_event key;
	// Bitmap to hold the actual thermometer image
	struct gfx_mono_bitmap tempscale;
	// String to hold the converted temperature reading
	char temperature_string[15];
	// Variable to hold the image thermometer scale
	uint8_t temp_scale;
	// Variable for holding the actual temperature in Celsius
	int16_t temperature;

	// Initiate a temperature sensor reading
	ntc_measure();

	// Struct for holding the temperature scale background
	tempscale.type = GFX_MONO_BITMAP_RAM;
	tempscale.width = 6;
	tempscale.height = 32;
	tempscale.data.pixmap = tempscale_data;

	// Screen border
	gfx_mono_draw_rect(0, 0, 128, 32, GFX_PIXEL_SET);
	// Clear screen
	gfx_mono_draw_filled_rect(1, 1, 126, 30, GFX_PIXEL_CLR);

	// Draw the header
	gfx_mono_draw_progmem_string((char PROGMEM_PTR_T)header, 27, 2, &sysfont);

	// The NTC temperature application loop
	while (1) {
		// Draw the thermometer
		gfx_mono_put_bitmap(&tempscale, X_POS, 0);

		// wait for NTC data to ready
		while (!ntc_data_is_ready());
		// Read the temperature once the ADC reading is done
		temperature = ntc_get_temperature();

		// Call a new reading while doing the rest of the calculations
		ntc_measure();

		// Convert the temperature into the thermometer scale
		temp_scale = -0.36 * temperature + 20.25;
		if (temp_scale <= 0) {
			temp_scale = 0;
		}

		// Draw the scale element on top of the background temperature image
		gfx_mono_draw_filled_rect(X_POS + 2, 3, 2, temp_scale,
				GFX_PIXEL_CLR);

		snprintf(temperature_string, sizeof(temperature_string), "%3i Celsius",
				temperature);

		// Draw the Celsius string
		gfx_mono_draw_string(temperature_string, X_POS + 12, 13, &sysfont);

		// Convert the temperature to Fahrenheit
		temperature = temperature * 1.8 + 32;
		snprintf(temperature_string, sizeof(temperature_string),
				"%3i Fahrenheit", temperature);

		// Draw the Fahrenheit temperature string
		gfx_mono_draw_string(temperature_string, X_POS + 12, 21, &sysfont);

		// Check if there has been any keyboard interactions
		keyboard_get_key_state(&key);
		if ((key.keycode == KEYBOARD_BACK) &&
				(key.type == KEYBOARD_RELEASE)) {
			// End the application
			break;
		}
	}
}
