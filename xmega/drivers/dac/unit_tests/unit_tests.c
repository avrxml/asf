/**
 * \file
 *
 * \brief Unit tests for DAC driver using ADC
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the XMEGA DAC driver using the chip ADC.
 * The test requires the following external connection
 * on the board that runs the tests:
 * - (PA1, PA3, PA5, PA7) connected together (J2 on XMEGA-A1 Xplained board)
 * - (PA0, PA2, PA4, PA6) connected together (J2 on XMEGA-A1 Xplained board)
 *
 * The DAC outputs are available on PA2 and PA3, leaving two groups of four
 * ADC pins connected to the two DAC output pins.
 *
 * As there will be variations in the value output on the DAC output and the
 * ADC input, all test measurements will have a range of acceptable values that
 * lie above and below the expected value. The size of this range can be
 * configured in the conf_test.h header file and have separate ranges for 8-bit
 * and 12-bit results.
 *
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_adc.h
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup(s):
 * - XMEGA-A1 Xplained - should also work on the XMEGA-A3BU Xplained with the
 * same external connections
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref dac_group "DAC driver"
 * - \ref adc_group "ADC driver"
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

//! \name Unit test configuration
//@{

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
 * \def CONF_TEST_ACCEPT_DELTA_8BIT
 * \brief Maximum acceptable deviation for 8-bit measurements
 *
 * The ADC results are accepted, i.e., test passed, as long as they are within
 * the acceptable range for the sample resolution:
 * \f[
 * \left| R_\textnormal{measured,8bit} - R_\textnormal{expected,8bit} \right|
 * \le \Delta_\textnormal{accepted,8bit}
 * \f]
 */

/**
 * \def CONF_TEST_ACCEPT_DELTA_12BIT
 * \brief Maximum acceptable deviation for 12-bit measurements
 *
 * The ADC results are accepted, i.e., test passed, as long as they are within
 * the acceptable range for the sample resolution:
 * \f[
 * \left| R_\textnormal{measured,12bit} - R_\textnormal{expected,12bit} \right|
 * \le \Delta_\textnormal{accepted,12bit}
 * \f]
 */
//@}

//! \name DAC unit test definitions
//@{

/**
 * \internal
 * \brief Number of samples to average over
 */
#define NUM_AVERAGE_SAMPLES 4

/**
 * \internal
 * \brief Number of ADC channels to use
 */
#define NUM_CHANNELS  4

/**
 * \internal
 * \brief Minimum value for DAC
 */
#define DAC_MIN 0

/**
 * \internal
 * \brief Maximum value for DAC
 */
#define DAC_MAX 0xfff

/**
 * \internal
 * \brief Maximum value for DAC in 8-bit
 */
#define DAC_8BIT_MAX 0xff

/**
 * \internal
 * \brief ADC zero value
 */
#define ADC_ZERO               0

/**
 * \internal
 * \brief Maximum value for ADC in unsigned 8-bit
 */
#define ADC_UNSIGNED_8BIT_MAX  0xff

/**
 * \internal
 * \brief Maximum value for ADC in unsigned 12-bit
 */
#define ADC_UNSIGNED_12BIT_MAX 0xfff

/**
 * \internal
 * \brief Maximum value for ADC in signed 12-bit
 */
#define ADC_SIGNED_12BIT_MAX   0x7ff

/**
 * \internal
 * \brief Minimum value for ADC in signed 12-bit
 */
#define ADC_SIGNED_12BIT_MIN   -0x800

//@}

//! \name DAC unit test helper functions
//@{

/**
 * \internal
 * \brief Shift 8 bit value to be used in left adjusted mode.
 *
 ***\param value value to left adjust
 *
 ***\return value left adjusted 16 bit value to write to \ref
 * dac_set_channel_value
 */
static uint16_t left_adjust_8bit_value(uint16_t value)
{
	value &= 0x00FF;
	value <<= 8;
	return value;
}

/**
 * \internal
 * \brief Measure single-ended input MUX combinations on channel
 *
 * Measures a set of input MUX combinations on a single channel, using averaging
 * of the number of samples specified with \ref NUM_AVERAGE_SAMPLES.
 *
 * \param adc Pointer to ADC to measure with.
 * \param ch_mask Mask for channel to measure with.
 * \param mux_inputs Pointer to array of input MUX settings.
 * \param num_inputs Number of input MUX settings.
 * \param results Pointer to array to store results in.
 *
 * \note The array which \e results points to must have at least \e num_inputs
 * elements.
 */
static void single_ended_unsigned_average(ADC_t *adc, uint8_t ch_mask,
		const uint8_t *mux_inputs, uint8_t num_inputs,
		uint16_t *results)
{
	struct adc_channel_config adcch_conf;
	uint8_t input;
	uint8_t i;
	uint32_t sum;

	memset(&adcch_conf, 0, sizeof(struct adc_channel_config));

	/* Common configuration */
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);

	for (input = 0; input < num_inputs; input++) {
		adcch_set_input(&adcch_conf,
				mux_inputs[input],
				ADCCH_NEG_NONE, 1);
		adcch_write_configuration(adc, ch_mask, &adcch_conf);

		/* Enable and do dummy conversion */
		adc_enable(adc);
		adc_start_conversion(adc, ch_mask);
		adc_wait_for_interrupt_flag(adc, ch_mask);

		/* Read an average */
		sum = 0;

		for (i = 0; i < NUM_AVERAGE_SAMPLES; i++) {
			adc_start_conversion(adc, ch_mask);
			adc_wait_for_interrupt_flag(adc, ch_mask);
			sum += adc_get_result(adc, ch_mask);
		}

		adc_disable(adc);

		results[input] = sum / NUM_AVERAGE_SAMPLES;
	}
}

/**
 * \internal
 * \brief Verify that unsigned ADC result is in acceptable range
 *
 * Checks that measurement is within the expected range, and outputs a clear
 * error message to the unit test framework if it is not.
 *
 * \param test Pointer to current test case.
 * \param expected The expected ADC result.
 * \param actual The measured ADC result.
 * \param dac_ch The DAC channel used.
 * \param adc_ch The ADC channel used.
 * \param mux_input The ADC input MUX setting used.
 * \param upper_limit Upper limit of accepted range, can override the limit
 * implicitly given by the accepted deviation.
 * \param value_is_12bit Specify the resolution of the ADC result and thus the
 * accepted deviation:
 * \arg \c true for +/- CONF_TEST_ACCEPT_DELTA_12BIT
 * \arg \c false for +/- CONF_TEST_ACCEPT_DELTA_8BIT
 */
static void verify_unsigned_result(const struct test_case *test,
		uint16_t expected, uint16_t actual, uint8_t dac_ch,
		uint8_t adc_ch, uint8_t mux_input, uint16_t upper_limit,
		bool value_is_12bit)
{
	int16_t upper;
	int16_t lower;
	uint16_t delta;

	if (value_is_12bit) {
		upper = expected + CONF_TEST_ACCEPT_DELTA_12BIT;
		lower = expected - CONF_TEST_ACCEPT_DELTA_12BIT;
		delta = CONF_TEST_ACCEPT_DELTA_12BIT;
	} else {
		upper = expected + CONF_TEST_ACCEPT_DELTA_8BIT;
		lower = expected - CONF_TEST_ACCEPT_DELTA_8BIT;
		delta = CONF_TEST_ACCEPT_DELTA_8BIT;
	}

	/* Cap compare values */
	if (upper > upper_limit) {
		upper = upper_limit;
	}

	if (lower < 0) {
		lower = 0;
	}

	test_assert_true(test, ((actual <= upper) && (actual >= lower)),
			"ADC result (%i) is outside acceptable range (%d +-%d)"
			". DACA%d output, ADCA ch%d, pin%d input.",
			actual, expected, delta, dac_ch, adc_ch, mux_input);
}

//@}

//! \name DAC unit test functions
//@{

/**
 * \internal
 * \brief Test dual channel 12 bit conversion
 *
 * This tests output three different values on the two DAC channels:
 * - 0 (output analog value is greater than 0, as the DAC cannot go that low)
 * - 1/2 * \ref DAC_MAX Half of the maximum value of the DAC
 * - \ref DAC_MAX The maximum value (VREF) of the DAC.
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channels, and the results are compared and checked to see if they
 * are within the acceptable range of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_dual_channel_12bit_conversion_test(
		const struct test_case *test)
{
	/* Number of MUX inputs that are to be read */
	const uint8_t num_inputs = 4;

	/* Connection between DAC outputs and ADC MUX inputs
	 * input 0, 2, 4, 6 is connected to DACA0
	 * input 1, 3, 5, 7 is connected to DACA1.
	 */
	const uint8_t channelgroup[2][4] = {{0, 2, 4, 6}, {1, 3, 5, 7}};

	uint8_t dac_channel;
	uint8_t adc_channel;
	uint8_t mux_index;
	uint16_t results[4];
	struct dac_config dac_conf;
	struct adc_config adc_conf;

	/* Configure ADC */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure DAC */
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP,
			DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);
	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);

	/* Set outputs as zero */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MIN);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MIN);

	for (dac_channel = 0; dac_channel < 2; dac_channel++) {
		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			single_ended_unsigned_average(&ADCA, 1 << adc_channel,
					(uint8_t *)&channelgroup[dac_channel],
					num_inputs, results);

			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_unsigned_result(test,
						ADC_ZERO,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_12BIT_MAX, true);
			}
		}
	}

	/* Set outputs as 1/2 * MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX / 2);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX / 2);

	for (dac_channel = 0; dac_channel < 2; dac_channel++) {
		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			single_ended_unsigned_average(&ADCA, 1 << adc_channel,
					(uint8_t *)&channelgroup[dac_channel],
					num_inputs, results);

			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_unsigned_result(test,
						ADC_UNSIGNED_12BIT_MAX / 2,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_12BIT_MAX, true);
			}
		}
	}

	/* Set outputs as MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX);

	for (dac_channel = 0; dac_channel < 2; dac_channel++) {
		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			single_ended_unsigned_average(&ADCA, 1 << adc_channel,
					(uint8_t *)&channelgroup[dac_channel],
					num_inputs, results);

			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_unsigned_result(test,
						ADC_UNSIGNED_12BIT_MAX,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_12BIT_MAX, true);
			}
		}
	}
}

/**
 * \internal
 * \brief Test dual channel conversion in 8-bit mode using the DAC
 *
 * This tests output three different values on the two DAC channels:
 * - 0 (output analog value is greater than 0, as the DAC cannot go that low)
 * - 1/2 * \ref DAC_8BIT_MAX Half of the maximum value of the DAC in 8 bit mode
 * - \ref DAC_8BIT_MAX The maximum value (VREF) of the DAC.
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channels, using all available ADC channels and the results are
 * compared and checked to see if they are within the acceptable range of
 * values that passes the test.
 *
 * \param test Current test case.
 */
static void run_dual_channel_8bit_conversion_test(
		const struct test_case *test)
{
	/* Number of MUX inputs that are to be read */
	const uint8_t num_inputs = 4;

	/* Connection between DAC outputs and ADC MUX inputs
	 * input 0, 2, 4, 6 is connected to DACA0
	 * input 1, 3, 5, 7 is connected to DACA1.
	 */
	const uint8_t channelgroup[2][4] = {{0, 2, 4, 6}, {1, 3, 5, 7}};

	uint8_t dac_channel;
	uint8_t adc_channel;
	uint8_t mux_index;
	uint16_t results[4];
	struct dac_config dac_conf;
	struct adc_config adc_conf;

	/* Configure ADC */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure DAC */
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP, DAC_ADJ_LEFT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);

	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);

	/* Set outputs as zero */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, left_adjust_8bit_value(DAC_MIN));
	dac_set_channel_value(&DACA, DAC_CH1, left_adjust_8bit_value(DAC_MIN));

	for (dac_channel = 0; dac_channel < 2; dac_channel++) {
		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			single_ended_unsigned_average(&ADCA, 1 << adc_channel,
					(uint8_t *)&channelgroup[dac_channel],
					num_inputs, results);

			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_unsigned_result(test,
						ADC_ZERO,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_12BIT_MAX, true);
			}
		}
	}

	/* Set outputs as 1/2 * MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0,
			left_adjust_8bit_value(DAC_8BIT_MAX / 2));
	dac_set_channel_value(&DACA, DAC_CH1,
			left_adjust_8bit_value(DAC_8BIT_MAX / 2));

	for (dac_channel = 0; dac_channel < 2; dac_channel++) {
		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			single_ended_unsigned_average(&ADCA, 1 << adc_channel,
					(uint8_t *)&channelgroup[dac_channel],
					num_inputs, results);

			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_unsigned_result(test,
						ADC_UNSIGNED_12BIT_MAX / 2,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_12BIT_MAX, true);
			}
		}
	}

	/* Set outputs as MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0,
			left_adjust_8bit_value(DAC_8BIT_MAX));
	dac_set_channel_value(&DACA, DAC_CH0, 0xFF00);
	dac_set_channel_value(&DACA, DAC_CH1,
			left_adjust_8bit_value(DAC_8BIT_MAX));
	dac_set_channel_value(&DACA, DAC_CH1, 0xFF00);

	for (dac_channel = 0; dac_channel < 2; dac_channel++) {
		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			single_ended_unsigned_average(&ADCA, 1 << adc_channel,
					(uint8_t *)&channelgroup[dac_channel],
					num_inputs, results);

			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_unsigned_result(test,
						ADC_UNSIGNED_12BIT_MAX,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_12BIT_MAX, true);
			}
		}
	}
}

/**
 * \internal
 * \brief Test single channel conversion in 12-bit mode using the DAC
 *
 * This tests output three different values on the two DAC channels:
 * - 0 (output analog value is greater than 0, as the DAC cannot go that low)
 * - 1/2 * \ref DAC_MAX Half of the maximum value of the DAC
 * - \ref DAC_MAX The maximum value (VREF) of the DAC.
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, using all available ADC channels and the results are
 * compared and checked to see if they are within the acceptable range of
 * values that passes the test.
 *
 * \param test Current test case.
 */
static void run_single_channel_12bit_conversion_test(
		const struct test_case *test)
{
	const uint8_t num_inputs = 4;

	/* Connection between DAC outputs and ADC MUX inputs
	 * input 0, 2, 4, 6 is connected to DACA0
	 * input 1, 3, 5, 7 is connected to DACA1.
	 */
	const uint8_t channelgroup[2][4] = {{0, 2, 4, 6}, {1, 3, 5, 7}};

	uint8_t dac_channel;
	uint8_t adc_channel;
	uint8_t mux_index;
	uint16_t results[4];
	struct dac_config dac_conf;
	struct adc_config adc_conf;

	/* Configure ADC */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/*  Configure DAC */
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP,
			DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);

	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);

	/* Set outputs as zero */
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MIN);

	dac_channel = 0;

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS;
			adc_channel++) {
		single_ended_unsigned_average(&ADCA, 1 << adc_channel,
				(uint8_t *)&channelgroup[dac_channel],
				num_inputs, results);

		for (mux_index = 0; mux_index < num_inputs;
				mux_index++) {
			verify_unsigned_result(test,
					ADC_ZERO,
					results[mux_index],
					dac_channel,
					adc_channel,
					channelgroup[dac_channel]
					[mux_index],
					ADC_UNSIGNED_12BIT_MAX, true);
		}
	}

	/* Set outputs as 1/2 * MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX / 2);

	dac_channel = 0;

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS;
			adc_channel++) {
		single_ended_unsigned_average(&ADCA, 1 << adc_channel,
				(uint8_t *)&channelgroup[dac_channel],
				num_inputs, results);

		for (mux_index = 0; mux_index < num_inputs;
				mux_index++) {
			verify_unsigned_result(test,
					ADC_UNSIGNED_12BIT_MAX / 2,
					results[mux_index],
					dac_channel,
					adc_channel,
					channelgroup[dac_channel]
					[mux_index],
					ADC_UNSIGNED_12BIT_MAX, true);
		}
	}

	/* Set outputs as MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX);

	dac_channel = 0;

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS;
			adc_channel++) {
		single_ended_unsigned_average(&ADCA, 1 << adc_channel,
				(uint8_t *)&channelgroup[dac_channel],
				num_inputs, results);

		for (mux_index = 0; mux_index < num_inputs;
				mux_index++) {
			verify_unsigned_result(test,
					ADC_UNSIGNED_12BIT_MAX,
					results[mux_index],
					dac_channel,
					adc_channel,
					channelgroup[dac_channel]
					[mux_index],
					ADC_UNSIGNED_12BIT_MAX, true);
		}
	}
}

/**
 * \internal
 * \brief Test single channel conversion in 8-bit mode using the DAC
 *
 * This tests output three different values on the two DAC channels:
 * - 0 (output analog value is greater than 0, as the DAC cannot go that low)
 * - 1/2 * \ref DAC_8BIT_MAX Half of the maximum value of the DAC in 8 bit mode
 * - \ref DAC_8BIT_MAX The maximum value (VREF) of the DAC.
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, using all available ADC channels and the results are
 * compared and checked to see if they are within the acceptable range of
 * values that passes the test.
 *
 * \param test Current test case.
 */
static void run_single_channel_8bit_conversion_test(
		const struct test_case *test)
{
	const uint8_t num_inputs = 4;

	/* Connection between DAC outputs and ADC MUX inputs
	 * input 0, 2, 4, 6 is connected to DACA0
	 * input 1, 3, 5, 7 is connected to DACA1.
	 */
	const uint8_t channelgroup[2][4] = {{0, 2, 4, 6}, {1, 3, 5, 7}};

	uint8_t dac_channel;
	uint8_t adc_channel;
	uint8_t mux_index;
	uint16_t results[4];
	struct dac_config dac_conf;
	struct adc_config adc_conf;

	/* Configure ADC */
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	/* Configure DAC */
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP, DAC_ADJ_LEFT);
	dac_set_active_channel(&dac_conf, DAC_CH0, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);

	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);

	/* Set outputs as zero */
	dac_set_channel_value(&DACA, DAC_CH0, 0x0000);

	dac_channel = 0;

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS;
			adc_channel++) {
		single_ended_unsigned_average(&ADCA, 1 << adc_channel,
				(uint8_t *)&channelgroup[dac_channel],
				num_inputs, results);

		for (mux_index = 0; mux_index < num_inputs;
				mux_index++) {
			verify_unsigned_result(test,
					ADC_ZERO,
					results[mux_index],
					dac_channel,
					adc_channel,
					channelgroup[dac_channel]
					[mux_index],
					ADC_UNSIGNED_12BIT_MAX, true);
		}
	}

	/* Set outputs as 1/2 * MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0);
	dac_set_channel_value(&DACA, DAC_CH0,
			left_adjust_8bit_value(DAC_8BIT_MAX / 2));

	dac_channel = 0;

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS;
			adc_channel++) {
		single_ended_unsigned_average(&ADCA, 1 << adc_channel,
				(uint8_t *)&channelgroup[dac_channel],
				num_inputs, results);

		for (mux_index = 0; mux_index < num_inputs;
				mux_index++) {
			verify_unsigned_result(test,
					ADC_UNSIGNED_12BIT_MAX / 2,
					results[mux_index],
					dac_channel,
					adc_channel,
					channelgroup[dac_channel]
					[mux_index],
					ADC_UNSIGNED_12BIT_MAX, true);
		}
	}

	/* Set outputs as MAX_VALUE */
	dac_wait_for_channel_ready(&DACA, DAC_CH0);
	dac_set_channel_value(&DACA, DAC_CH0,
			left_adjust_8bit_value(DAC_8BIT_MAX));

	dac_channel = 0;

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS;
			adc_channel++) {
		single_ended_unsigned_average(&ADCA, 1 << adc_channel,
				(uint8_t *)&channelgroup[dac_channel],
				num_inputs, results);

		for (mux_index = 0; mux_index < num_inputs;
				mux_index++) {
			verify_unsigned_result(test,
					ADC_UNSIGNED_12BIT_MAX,
					results[mux_index],
					dac_channel,
					adc_channel,
					channelgroup[dac_channel]
					[mux_index],
					ADC_UNSIGNED_12BIT_MAX, true);
		}
	}
}

//@}

/**
 * \brief Run DAC unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * DAC unit test suite and runs it.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};
	sysclk_init();
	board_init();
	sysclk_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define test cases */
	DEFINE_TEST_CASE(dual_channel_12bit_conversion_test, NULL,
			run_dual_channel_12bit_conversion_test, NULL,
			"Dual channel 12 bit conversion test");
	DEFINE_TEST_CASE(dual_channel_8bit_conversion_test, NULL,
			run_dual_channel_8bit_conversion_test, NULL,
			"Dual channel 8 bit conversion test");
	DEFINE_TEST_CASE(single_channel_12bit_conversion_test, NULL,
			run_single_channel_12bit_conversion_test, NULL,
			"Single channel 12 bit conversion test");
	DEFINE_TEST_CASE(single_channel_8bit_conversion_test, NULL,
			run_single_channel_8bit_conversion_test, NULL,
			"Single channel 8 bit conversion test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(dac_tests) = {
		&dual_channel_12bit_conversion_test,
		&dual_channel_8bit_conversion_test,
		&single_channel_12bit_conversion_test,
		&single_channel_8bit_conversion_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(dac_suite, dac_tests,
			"XMEGA DAC driver test suite");

	/* Run all tests in the suite */
	test_suite_run(&dac_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
