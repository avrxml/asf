/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 7
 * Example demonstration of hardware over-sampling feature of XMEGA E ADC.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example demonstrates a method to increase the resolution of the
 * ADC using the hardware over-sampling feature of the XMEGA E ADC.
 *
 * The application continuously perfomrs an ADC conversion and send ADC values
 * on serial COM.
 *
 * At startup the hardware over-sampling is disabled.
 * To enable hardware over-sampling press key 'o', and key 'd' to disable it.
 *
 * When the hardware over-sampling is enabled an average of 1024 samples is done
 * and a 16bits resolution is requested.
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <conf_example.h>

/* Internal ADC funtions */
static void main_adc_init(void);
static void main_adc_oversampling_start(void);
static void main_adc_oversampling_stop(void);

int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};
	char binary[16 + 1];
	uint8_t i;
	bool oversampling_is_enabled;

	/* Usual initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	printf("\x0C\n\r-- ADC Over-sampling Example --\n\r");
	printf("-- Compiled: %s %s --\n\r\n\r", __DATE__, __TIME__);

	printf("Commands:\n\r");
	printf("- key 'o' to enable over-sampling\n\r");
	printf("- key 'd' to disable over-sampling\n\r");

	/* ADC initializations */
	main_adc_init();
	main_adc_oversampling_stop();
	oversampling_is_enabled = false;

	while (1) {
		if (usart_rx_is_complete(CONF_TEST_USART)) {
			char key = getchar();
			if (key == 'o') {
				main_adc_oversampling_start();
				oversampling_is_enabled = true;
			}

			if (key == 'd') {
				main_adc_oversampling_stop();
				oversampling_is_enabled = false;
			}
		}

		/* Wait sample with or without over-sampling */
		uint16_t sample;
		adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
		sample = adc_get_unsigned_result(&ADCA, ADC_CH0);
		if (!oversampling_is_enabled) {
			sample <<= 4;
		}

		i = 15;
		do {
			binary[i] = '0' + (sample & 1);
			sample >>= 1;
		} while (i--);
		binary[16] = 0;
		printf("ADC Value: %sb\r", binary);
	}
}

/**
 * \brief Initialize ADC
 *
 * Here the over-sampling feature is disabled.
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
 * \brief Enables over-sampling
 */
static void main_adc_oversampling_start(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	adc_disable(&ADCA);

	/* Change resolution parameter to accept over-sampling */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_MT12,
			ADC_REF_VCCDIV2);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Enable over-sampling */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_oversampling(&adcch_conf, ADC_SAMPNUM_1024X, 16);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	adc_enable(&ADCA);
	printf("\n\r* ADC over-sampling enabled\n\r");
}

/**
 * \brief Disables over-sampling
 */
static void main_adc_oversampling_stop(void)
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

	/* Disable over-sampling */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_disable_oversampling(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	adc_enable(&ADCA);
	printf("\n\r* ADC over-sampling disabled\n\r");
}
