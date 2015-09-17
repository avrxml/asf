/**
 * \file
 *
 * \brief Unit tests for XMEGA E ADC driver using DAC
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the XMEGA ADC driver using the on-chip DAC as an
 * analog input voltage.
 *
 * The example captures and check sample values (1.8V to -1.8V)
 * in three modes:
 * - wihout averaging and without offset/gain correction
 * - with averaging and without offset/gain correction
 * - with averaging and with offset/gain correction
 * The DAC is used to generate ADC inputs voltages.
 *
 * ADC configuration used:
 * - signed, 12-bit results
 * - 1x gain
 * - voltage reference = VCC / 1.6 = 3.3V / 1.6
 * - differential measurement mode :
 *   - Input voltage V+ is ADC2 pin (PA2 pin)
 *   - Input voltage V- is ADC3 pin (PA3 pin)
 *
 * DAC configuration used:
 * - AVCC as reference
 * - right adjusted channel value
 * - both DAC channels active on :
 *   - DAC0 (PA2 pin) for ADC V+
 *   - DAC1 (PA3 pin) for ADC V-
 *
 * \note
 * All AVR XMEGA E devices can be used.
 * The "Calibration and correction" method is described in the datasheet
 * "XMEGA E Manual".
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

/* ! \name Unit test configuration */
/* @{ */

/**
 * \def CONF_TEST_USART
 * \brief USART to redirect STDIO to
 */

/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of USART
 */

/**
 * \def CONF_TEST_CHARLENGTH
 * \brief Character length (bits) of USART
 */

/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of USART
 */

/**
 * \def CONF_TEST_STOPBITS
 * \brief Stopbit configuration of USART
 */

/**
 * \def CONF_TEST_ACCEPT_DELTA
 * \brief Maximum acceptable deviation for measurements in standard mode
 * (unit mV)
 *
 * \def CONF_TEST_ACCEPT_DELTA_AVERAGING
 * \brief Maximum acceptable delta for measurements between
 * several values in averaging mode (unit mV)
 *
 * \def CONF_TEST_ACCEPT_DELTA_CORRECTION
 * \brief Maximum acceptable deviation for measurements in averaging
 * and offset/gain corrections mode (unit mV)
 *
 * The ADC results are accepted, i.e., test passed, as long as they are within
 * the acceptable range for the sample resolution.
 */
/* @} */

#define CONV_MAX_VOLTAGE   1800
#define CONV_VOLTAGE_STEP  600
#define NB_VALUE           ((CONV_MAX_VOLTAGE / CONV_VOLTAGE_STEP) * 2 + 1)

/* Internal ADC funtions */
static void main_adc_init(void);
static int16_t main_adc_input(void);

/* Internal DAC funtions */
static void main_dac_init(void);
static void main_dac_output(int16_t volt);

/* ! \name ADC unit test functions */
/* @{ */

/**
 * \internal
 * \brief Test standard conversion in 12-bit mode using the DAC
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, and the results are compared and checked to see if they
 * are within the acceptable range of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_standard_conversion_test(
		const struct test_case *test)
{
	int16_t volt_output;
	int16_t volt_input;
	uint16_t error;

	for (volt_output = -CONV_MAX_VOLTAGE;
			volt_output <= CONV_MAX_VOLTAGE;
			volt_output += CONV_VOLTAGE_STEP) {
		main_dac_output(volt_output);
		volt_input = main_adc_input();
		if (volt_output > volt_input) {
			error = volt_output - volt_input;
		} else {
			error = volt_input - volt_output;
		}

		test_assert_true(test, error < CONF_TEST_ACCEPT_DELTA,
				"ADC result is outside acceptable range (expected %d, captured %d)",
				volt_output, volt_input);
	}
}

/**
 * \internal
 * \brief Test averaging conversion in 12-bit mode using the DAC
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, and the results are compared and checked to see if they
 * are within the acceptable average of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_averaging_conversion_test(
		const struct test_case *test)
{
	int16_t volt_output;
	int16_t volt_min, volt_max, volt_input;

	/* ADC module configuration structure */
	struct adc_config adc_conf;
	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;

	adc_disable(&ADCA);

	/* Change resolution parameter to accept averaging */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_MT12,
			ADC_REF_VCC);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Enable averaging */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_averaging(&adcch_conf, ADC_SAMPNUM_1024X);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	adc_enable(&ADCA);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);

	/* Check values */
	for (volt_output = -CONV_MAX_VOLTAGE;
			volt_output <= CONV_MAX_VOLTAGE;
			volt_output += CONV_VOLTAGE_STEP) {
		main_dac_output(volt_output);
		/* first capture */
		volt_input = main_adc_input();
		volt_min = volt_max = volt_input;
		/* several capture */
		for (uint8_t i = 0; i < 5; i++) {
			volt_input = main_adc_input();
			if (volt_min > volt_input) {
				volt_min = volt_input;
			}

			if (volt_max < volt_input) {
				volt_max = volt_input;
			}
		}

		test_assert_true(test,
				(volt_max - volt_min) < CONF_TEST_ACCEPT_DELTA_AVERAGING,
				"ADC result is not in acceptable stable range (Min %dmV, Max %dmV)",
				volt_min, volt_max);
	}
}

/**
 * \internal
 * \brief Test correction conversion in 12-bit mode using the DAC
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, and the results are compared and checked to see if they
 * are within the acceptable range of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_correction_conversion_test(
		const struct test_case *test)
{
	int16_t volt_output;
	int16_t volt_input;
	uint16_t error;

	/** Measures and enables offset and gain corrections */

	/* ADC channel configuration structure */
	struct adc_channel_config adcch_conf;
	uint16_t offset_correction;

	/* Expected value for gain correction at 1.9V
	 * expected_value = Max. range * 1.9V / (VCC / 1.6)
	 * Max. range = 12 bits signed = 11 bits unsigned
	 */
	const uint16_t expected_value
		= ((1 << 11) * 1900UL) / (3300L * 1000L / 1600L);
	/* Captured value for gain correction */
	uint16_t captured_value;

	/* DAC Output 0 Volt */
	main_dac_output(0);

	/* Capture value for 0 Volt */
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	offset_correction = adc_get_unsigned_result(&ADCA, ADC_CH0);

	/* Enable offset correction */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_enable_correction(&adcch_conf, offset_correction, 1, 1);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* DAC Output 1.9 Volts */
	main_dac_output(1900);

	/* Capture value for 1.9 Volts */
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	captured_value = adc_get_unsigned_result(&ADCA, ADC_CH0);

	/* Enable offset & gain correction */
	adcch_enable_correction(&adcch_conf, offset_correction, expected_value,
			captured_value);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Check values */
	for (volt_output = -CONV_MAX_VOLTAGE;
			volt_output <= CONV_MAX_VOLTAGE;
			volt_output += CONV_VOLTAGE_STEP) {
		main_dac_output(volt_output);
		volt_input = main_adc_input();
		if (volt_output > volt_input) {
			error = volt_output - volt_input;
		} else {
			error = volt_input - volt_output;
		}

		test_assert_true(test,
				error < CONF_TEST_ACCEPT_DELTA_CORRECTION,
				"ADC result is outside acceptable range (expected %d, captured %d)",
				volt_output, volt_input);
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
	 * - signed, 12-bit results
	 * - voltage reference = VCC / 1.6 = 3.3V / 1.6
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 */
	adc_read_configuration(&ADCA, &adc_conf); /* Initialize structures. */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_VCC);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure ADC channel:
	 * - differential measurement mode
	 * - Input voltage V+ is ADC2 pin (PA2 pin)
	 * - Input voltage V- is ADC3 pin (PA3 pin)
	 * - 1x gain
	 * - interrupt flag set on completed conversion
	 */
	adcch_read_configuration(&ADCA, ADC_CH0, &adcch_conf);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN2, ADCCH_NEG_PIN3, 1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);
	adcch_write_configuration(&ADCA, ADC_CH0, &adcch_conf);

	/* Enable ADC */
	adc_enable(&ADCA);

	/* Ignore first conversion */
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
}

/**
 * \brief Captures a values on ADC
 *
 * \return value on ADC pins (mV)
 */
static int16_t main_adc_input(void)
{
	int16_t sample;

	/* Ignore two averaging conversions
	 * which can include two differents voltage
	 */
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);

	/* Capture averaging */
	adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
	sample = adc_get_signed_result(&ADCA, ADC_CH0);

	/* Conversion sample to mV :
	 * mV = sample * (VCC / 1.6) / Max. range
	 */
	return ((int32_t)sample * (3300L * 1000L / 1600L)) / (1 << 11);
}

/**
 * \brief Initializes the DAC
 */
static void main_dac_init(void)
{
	/* DAC module configuration structure */
	struct dac_config dac_conf;

	/* Create configuration:
	 * - AVCC as reference
	 * - right adjusted channel value
	 * - both DAC channels active on :
	 *   - DAC0 (PA2 pin) for ADC V+
	 *   - DAC1 (PA3 pin) for ADC V-
	 * - manually triggered conversions on both channels
	 */
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_AVCC, DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);
}

/**
 * \brief Changes the DAC ouput value
 *
 * \param volt Value to output on DAC pins
 */
static void main_dac_output(int16_t volt)
{
	/* Samples of values used:
	 * | ADC0 |    V+   | ADC1 |    V-   | V+ - V- |
	 * ---------------------------------------------
	 * | 4096 |   Vcc   |    0 |    0V   |   Vcc   |
	 * | 3072 | Vcc 3/4 | 1024 | Vcc 1/4 |  Vcc/2  |
	 * | 2048 | Vcc 1/2 | 2048 | Vcc 1/2 |    0V   |
	 * | 1024 | Vcc 1/4 | 3072 | Vcc 3/4 | -Vcc/2  |
	 * |    0 |    0V   | 4096 |   Vcc   |  -Vcc   |
	 */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0,
			(1 << 11) + (((1 << 11) * (int32_t)volt) / 3300));
	dac_set_channel_value(&DACA, DAC_CH1,
			(1 << 11) - (((1 << 11) * (int32_t)volt) / 3300));
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
}

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
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* ADC and DAC initializations */
	main_dac_init();
	main_adc_init();

	DEFINE_TEST_CASE(standard_conversion_test, NULL,
			run_standard_conversion_test, NULL,
			"Standard conversion with 12-bit result");
	DEFINE_TEST_CASE(averaging_conversion_test, NULL,
			run_averaging_conversion_test, NULL,
			"Averaging conversion with 12-bit result");
	DEFINE_TEST_CASE(correction_conversion_test, NULL,
			run_correction_conversion_test, NULL,
			"Correction conversion with 12-bit result");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(adc_tests) = {
		&standard_conversion_test,
		&averaging_conversion_test,
		&correction_conversion_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(adc_suite, adc_tests,
			"XMEGA E ADC driver test suite");

	/* Run all tests in the suite */
	test_suite_run(&adc_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
