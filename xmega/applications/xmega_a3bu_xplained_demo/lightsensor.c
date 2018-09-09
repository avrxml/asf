/**
 * \file
 *
 * \brief Lightsensor application
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
