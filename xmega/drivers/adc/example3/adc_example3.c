/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 3
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
 * This simple example shows how to use the new scan feature found in the
 * XMEGA-...U devices.
 *
 * \section files Main files:
 * - adc_example3.c: ADC driver example application
 * - conf_adc.h: configuration of the ADC driver
 *
 * \section apiinfo ADC driver API
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA U devices with an ADC can be used or devices that have the
 * scan register on ADCA channel 0.
 *
 * \section exampledescription Description of the example
 * ADC A is configured for signed, 12-bit conversions using the internal 1 V
 * voltage reference. The conversions are configured to be triggered from the
 * event system. Channel 0 of the ADC is configured for differential measurement
 * of all ADCA pins (PA0-PA7) with internal ground.
 *
 * An ADC interrupt callback function is used to handle the conversion results,
 * each result is stored into an array at the position corresponding to the pin
 * number, e.g. PA0 result is stored in adc_scan_results[0].
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
#include <string.h>

static volatile int16_t adc_scan_results[8];

//! The last measured voltage on the board
static volatile double vcc_voltage;

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param channel ADC channel number.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	static uint8_t current_scan_channel = 0;
	// Store the ADC results from the scan in the result array
	if (ch_mask & ADC_CH0) {
		adc_scan_results[current_scan_channel] = result;
		current_scan_channel++;
		// When 8 pins have been scanned the SCAN OFFSET wraps to zero
		if (current_scan_channel == 8) {
			current_scan_channel = 0;
		}
	}
}

/**
 * \brief ADC Example 3 main application function
 */
int main(void)
{
	// ADC configuration structs
	struct adc_config         adc_conf;
	struct adc_channel_config adcch_conf;

	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	// Clear the configuration structures.
	memset(&adc_conf, 0, sizeof(struct adc_config));
	memset(&adcch_conf, 0, sizeof(struct adc_channel_config));

	/* Configure the ADC module:
	 * - signed, 12-bit results
	 * - bandgap (1 V) voltage reference
	 * - 200 kHz lock rate
	 * - channel0 triggered by event
	 * - use the ADC current limiter in high setting mode
	 * - enable the internal bandgap reference
	 * - callback function
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_EVENT_SWEEP, 1, 0);
	adc_set_current_limit(&adc_conf, ADC_CURRENT_LIMIT_HIGH);
	adc_set_gain_impedance_mode(&adc_conf, ADC_GAIN_HIGHIMPEDANCE);
	adc_enable_internal_input(&adc_conf, ADC_INT_BANDGAP);

	adc_write_configuration(&ADCA, &adc_conf);
	adc_set_callback(&ADCA, &adc_handler);

	/* Configure ADC channel 0:
	 * - differential measurement between POS: ADC0-8 and NEG: internal gnd
	 * - interrupt at completed conversion
	 * - use the CH0 pin scan function
	 */
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN0, ADCCH_NEG_INTERNAL_GND, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);
	adcch_set_pin_scan(&adcch_conf, 0, 7);

	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	// Enable the Event System to use as trigger source
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	// Conversion 62500 times a second
	EVSYS.CH0MUX = EVSYS_CHMUX_PRESCALER_512_gc;

	// Enable the ADC
	adc_enable(&ADCA);

	do {
		// Sleep until ADC interrupt triggers.
		sleepmgr_enter_sleep();
	} while (1);
}
