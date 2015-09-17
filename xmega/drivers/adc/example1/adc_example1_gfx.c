/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 1
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
 * This simple example shows how to use the \ref adc_group for temperature
 * measurement with the internal sensor.
 *
 * \section files Main files:
 * - adc_example1_gfx.c: ADC driver example application
 * - conf_adc.h: configuration of the ADC driver
 * - conf_example.h: configuration of the example
 *
 * \section apiinfo ADC driver API
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an ADC can be used.
 *
 * \section exampledescription Description of the example
 * ADC A is configured for unsigned, 12-bit conversions using the VCC as
 * voltage reference. The conversions are configured for manual triggering.
 * Channel 0 of the ADC is configured for single-ended measurements from the
 * internal temperature sensor.
 *
 * An ADC interrupt callback function is used to handle the conversion results,
 * from which the current temperature, in Celsius, is computed. The computation
 * utilizes a linearization of the temperature data to calculate temperature
 * in Celsius.
 *
 * The computed temperature is stored in the global variable
 * \ref last_temperature, which is printed to the display,
 * and can be watched during debugging.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <conf_example.h>

//! The latest, computed temperature.
static volatile int16_t last_temperature;

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask ADC channel mask.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	#ifdef CONF_BOARD_OLED_UG_2832HSWEG04
		gfx_mono_draw_filled_rect(0,0,128,32,GFX_PIXEL_CLR);
	#endif
	int32_t temperature;
	char out_str[OUTPUT_STR_SIZE];

	/* Compute current temperature in Celsius, based on linearization
	 * of the temperature sensor adc data.
	 */

	if (result > 697) {
		temperature = (int8_t)((-0.0295 * result) + 40.5);
	} if (result > 420) {
		temperature = (int8_t)((-0.0474 * result) + 53.3);
	} else {
		temperature = (int8_t)((-0.0777 * result) + 65.1);
	}

	last_temperature = temperature;

	// Write temperature to display
	snprintf(out_str, OUTPUT_STR_SIZE, "Temperature: %4d C", last_temperature);
	gfx_mono_draw_string(out_str, 0, 0, &sysfont);

	// Start next conversion.
	adc_start_conversion(adc, ch_mask);
}

int main(void)
{
	struct adc_config         adc_conf;
	struct adc_channel_config adcch_conf;

	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	gfx_mono_init();

	// Enable backlight if display type is not OLED
	#ifndef CONF_BOARD_OLED_UG_2832HSWEG04
		ioport_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	#endif

	// Initialize configuration structures.
	adc_read_configuration(&ADCA, &adc_conf);
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Configure the ADC module:
	 * - unsigned, 12-bit results
	 * - VCC voltage reference
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 * - temperature sensor enabled
	 * - callback function
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_enable_internal_input(&adc_conf, ADC_INT_TEMPSENSE);

	adc_write_configuration(&ADCA, &adc_conf);
	adc_set_callback(&ADCA, &adc_handler);

	/* Configure ADC channel 0:
	 * - single-ended measurement from temperature sensor
	 * - interrupt flag set on completed conversion
	 * - interrupts disabled
	 */
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_NONE,
			1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);

	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	// Enable the ADC and start the first conversion.
	adc_enable(&ADCA);
	adc_start_conversion(&ADCA, ADC_CH0);

	do {
		// Sleep until ADC interrupt triggers.
		sleepmgr_enter_sleep();
	} while (1);
}
