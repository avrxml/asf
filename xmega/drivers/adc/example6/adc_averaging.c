/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 6
 * Example demonstration of hardware averaging feature of XMEGA E ADC.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example demonstrates how to use XMEGA E ADC hardware averaging feature
 * to perform the average of a given number of ADC samples.
 *
 * The application continuously performs ADC conversion and send ADC values
 * on serial COM.
 *
 * At startup the hardware averaging is disabled.
 * To enable hardware averaging press key 'a', and key 'd' to disable it.
 *
 * The hardware averaging is enabled to average 1024 samples.
 *
 * ADC configuration used:
 * - 12-bit unsigned results
 * - 1x gain
 * - voltage reference = VCC /2
 * - single-ended measurement
 * - input is the internal temperature sensor
 * - freerun triggering
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - uses the RC032X routine board with TQFP32 socket
 * - PC2 & PC3 pins should be respectively connected to RXD & TXD pins
 *   of RS232 SPARE header
 * - STK600 RS2232 port should be connected to a computer and
 *   the communication terminal configuration should be 38400 baud, no parity.
 *
 * For XMEGA-E5 Xplained board:
 * - Connect USB (Virtual port COM) to a computer and
 *   the communication terminal configuration should be 38400 baud, no parity.
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
#include <conf_example.h>

/* Internal ADC funtions */
static void main_adc_init(void);
static void main_adc_averaging_start(void);
static void main_adc_averaging_stop(void);

int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	printf("\x0C\n\r-- ADC Averaging Example --\n\r");
	printf("-- Compiled: %s %s --\n\r\n\r", __DATE__, __TIME__);

	printf("Commands:\n\r");
	printf("- key 'a' to enable averaging\n\r");
	printf("- key 'd' to disable averaging\n\r");

	/* ADC initialization */
	main_adc_init();
	main_adc_averaging_stop();

	while (1) {
		if (usart_rx_is_complete(CONF_TEST_USART)) {
			char key = getchar();
			if (key == 'a') {
				main_adc_averaging_start();
			}

			if (key == 'd') {
				main_adc_averaging_stop();
			}
		}

		/* Wait sample with or without average */
		uint16_t sample;
		adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
		sample = adc_get_unsigned_result(&ADCA, ADC_CH0);
		printf("ADC Value: %4u\r", sample);
	}
}

/**
 * \brief Initialize ADC
 *
 * Here the averaging feature is disabled.
 */
static void main_adc_init(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	/* Configure the ADC module:
	 * - unsigned, more than 12-bit results
	 * - VCC /2 voltage reference
	 * - 200 kHz maximum clock rate
	 * - freerun conversion triggering
	 * - enabled internal temperature sensor
	 */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_VCCDIV2);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN, 1, 0);
	adc_enable_internal_input(&adc_conf, ADC_INT_TEMPSENSE);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure ADC channel 0:
	 * - single-ended measurement from temperature sensor
	 * - interrupt flag set on completed conversion
	 */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_set_input(&adcch_conf, ADCCH_POS_TEMPSENSE, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Enable ADC which starts the freerun conversion.*/
	adc_enable(&ADCA);
}

/**
 * \brief Enables averaging
 */
static void main_adc_averaging_start(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	adc_disable(&ADCA);

	/* Change resolution parameter to accept averaging */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_MT12,
			ADC_REF_VCCDIV2);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Enable averaging */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_averaging(&adcch_conf, ADC_SAMPNUM_1024X);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	adc_enable(&ADCA);
	printf("\n\r* ADC averaging enabled\n\r");
}

/**
 * \brief Disables averaging
 */
static void main_adc_averaging_stop(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	adc_disable(&ADCA);

	/* Set default resolution parameter */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_VCCDIV2);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Disable averaging */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_disable_averaging(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	adc_enable(&ADCA);
	printf("\n\r* ADC averaging disabled\n\r");
}
