/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter Driver Example 9
 *
 * Example which shows two-point calibration of the internal temperature sensor
 * of AVR XMEGA E.
 *
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
 * This example shows the usage of two calibration points available for
 * the internal temperature sensor in AVR XMEGA E devices.
 * Two-point calibration of the AVR XMEGA E internal temperature sensor helps
 * to keep the error in temperature readings to the lowest levels.
 *
 * The production signature row of AVR XMEGA E device contains calibration data
 * for the internal temperature sensor, stored during device manufacturing.
 * These two calibration points can be used for two-point calibration
 *
 * ADC configuration used:
 * - Signed, 12-bit results
 * - 1x gain
 * - Voltage reference = Internal 1V
 * - Single ended
 * - Internal temperature sensor as source
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The ADC driver API can be found \ref adc_group "here".
 *
 * \section board_setup Board setup
 * For STK600 board:
 * - Use the RC032X routine board with TQFP32 socket
 * - PC2 & PC3 pins should respectively be connected to RXD & TXD pins
 *   of RS232 SPARE header
 * - STK600 RS232 port should be connected to a computer and
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
#include <asf.h>
#include <conf_example.h>

/* Use 'y = mx + c' to find out the Temperature reading
 * corresponding to the ADC reading
 */
float m = 0.0;
float c = 0.0;
volatile bool result_ready = false;
volatile int16_t temperature = 0;

/* Internal ADC functions */
static void main_adc_init(void);
static void main_adc_averaging(void);

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask ADC channel mask.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	temperature = (int16_t)((m * result) + c);
	result_ready = true;
}

int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	/* To load data from the production signature row */
	int16_t calib_hottemp = 0;
	int16_t calib_roomtemp = 0;
	int16_t adc_val_hottemp = 0;
	int16_t adc_val_roomtemp = 0;

	/* Initializations */
	board_init();
	sysclk_init();
	sleepmgr_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	printf("\x0C\n\r-- AVR XMEGA E Internal Temperature Sensor \
			Two Point Calibration");
	printf("  (Compiled: %s %s)\n\r", __DATE__, __TIME__);

	/* Get the calibration point data from production signature row into the
	 * device
	 */
	calib_hottemp = adc_get_calibration_data(ADC_CAL_HOTTEMP);
	calib_roomtemp = adc_get_calibration_data(ADC_CAL_ROOMTEMP);
	adc_val_hottemp = adc_get_calibration_data(ADC_CAL_TEMPSENSE);
	adc_val_roomtemp = adc_get_calibration_data(ADC_CAL_TEMPSENSE2);

	/* Find the slope of the line m = (y2-y1)/(x2-x1)*/
	m = (float)(calib_hottemp -
			calib_roomtemp) / (adc_val_hottemp - adc_val_roomtemp);
	/* Find the y intercept c = y - mx*/
	c = (float)calib_hottemp - (m * adc_val_hottemp);

	/* ADC initialization */
	main_adc_init();
	/* Enable averaging */
	main_adc_averaging();

	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Enable ADC */
	adc_enable(&ADCA);
	adc_start_conversion(&ADCA, ADC_CH0);

	uint16_t i, j;
	while (1) {
		/* Delay between two readings */
		i = 0xFFFF;
		j = 0x9;
		while (j--) {
			while (i--) {
			}
		}
		if (result_ready) {
			/* Start next conversion. */
			adc_start_conversion(&ADCA, ADC_CH0);
			/* Display the temperature reading */
			printf("The temperature is now %u Deg C\n\r", temperature);
			result_ready = false;
		}
	}
}

/**
 * \brief Initialize ADC
 */
static void main_adc_init(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	/* Configure the ADC module:
	 * ADC Voltage reference => 1.00V from internal bandgap
	 * ADC Clock speed => 125KHz
	 * ADC Mode => Internal Signed
	 * Manual conversion triggering
	 */
	adc_read_configuration(&ADCA, &adc_conf); /* Initialize structures. */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_MT12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 125000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_enable_internal_input(&adc_conf, ADC_INT_TEMPSENSE);
	adc_write_configuration(&ADCA, &adc_conf);
	adc_set_callback(&ADCA, &adc_handler);

	/* Configure ADC channel:
	 * ADC Input => Temperature Sensor
	 * Interrupt flag set on conversion complete
	 * Interrupts Disabled
	 */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_set_input(&adcch_conf, ADCCH_POS_TEMPSENSE, ADCCH_NEG_NONE, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Enable ADC */
	adc_enable(&ADCA);
}

/**
 * \brief Enables averaging
 */
static void main_adc_averaging(void)
{
	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	adc_disable(&ADCA);

	/* Change resolution parameter to accept averaging */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_MT12,
			ADC_REF_BANDGAP);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Enable averaging */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_averaging(&adcch_conf, ADC_SAMPNUM_512X);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	adc_enable(&ADCA);
}
