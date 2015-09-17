/**
 * \file
 *
 * \brief Unit tests for ADC driver using DAC
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
 * This is the unit test for the XMEGA ADC driver using the chip DAC as an
 * analog input voltage. The test requires the following external connection
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
 * Single ended tests (12-bit mode):
 * -# DACA0 successively set to minimum, half of maximum and maximum
 *  - Measure ADC0, ADC2, ADC4 and ADC6
 * -# DACA1 successively set to minimum, half of maximum and maximum
 *  - Measure ADC1, ADC3, ADC5 and ADC7
 *
 * Single ended tests (8-bit mode):
 * - Same as 12-bit mode, but range is modified to fit 8-bit range.
 *
 * Differential test (12-bit mode):
 * -# DACA0 and DACA1 set to half of maximum in order to get a differential zero
 *  - Measure differential pairs ADC0/ADC1, ADC2/ADC3, ADC4/ADC5 and ADC6/ADC7
 * -# DACA0 set to maximum, DACA1 set to minimum to get maximum positive value
 *  - Measure differential pairs ADC0/ADC1, ADC2/ADC3, ADC4/ADC5 and ADC6/ADC7
 * -# DACA0 set to minimum, DACA1 set to maximum to get maximum negative value
 *  - Measure differential pairs ADC0/ADC1, ADC2/ADC3, ADC4/ADC5 and ADC6/ADC7
 *
 * Differential with gain test (12-bit mode):
 * -# Set DACA0 and DACA1 to gain adjusted value that should give a ADC input
 *   value of half maximum positive value
 *  - Measure differential pairs ADC4/ADC5 and ADC6/ADC7
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
 * - \ref adc_group "ADC driver"
 * - \ref dac_group "DAC driver"
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
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

//! \name ADC unit test definitions
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

//! \name ADC unit test helper functions
//@{

/**
 * \internal
 * \brief Measure single-ended input MUX combinations on channel
 *
 * Measures a set of input MUX combinations on a single channel, using averaging
 * of the number of samples specified with \ref NUM_AVERAGE_SAMPLES.
 *
 * \pre This function does not configure the ADC, only the ADC channel, and
 * therefore the specified ADC needs to be configured before this function
 * is run.
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

	// Common configuration
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);

	for (input = 0; input < num_inputs; input++) {
		adcch_set_input(&adcch_conf, mux_inputs[input], ADCCH_NEG_NONE,
				1);
		adcch_write_configuration(adc, ch_mask, &adcch_conf);

		// Enable and do dummy conversion
		adc_enable(adc);
		adc_start_conversion(adc, ch_mask);
		adc_wait_for_interrupt_flag(adc, ch_mask);

		// Read an average
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
 * \brief Measure differential input MUX combinations on channel
 *
 * Measures a set of input MUX combinations on a single channel, using averaging
 * of the number of samples specified with \ref NUM_AVERAGE_SAMPLES.
 *
 * \pre This function does not configure the ADC, only the ADC channel, and
 * therefore the specified ADC needs to be configured before this function
 * is run.
 *
 * \param adc Pointer to ADC to measure with.
 * \param ch_mask Mask for channel to measure with.
 * \param mux_pos_inputs Pointer to array of positive input MUX settings.
 * \param mux_neg_inputs Pointer to array of negative input MUX settings.
 * \param num_inputs Number of input MUX setting pairs.
 * \param results Pointer to array to store results in.
 * \param gain Gain to use for all measurements.
 *
 * \note The array which \e results points to must have at least \e num_inputs
 * elements.
 */
static void differential_signed_average(ADC_t *adc, uint8_t ch_mask,
		const uint8_t *mux_pos_inputs, const uint8_t *mux_neg_inputs,
		uint8_t num_inputs, int16_t *results, uint8_t gain)
{
	struct adc_channel_config adcch_conf;
	uint8_t input;
	uint8_t i;
	int32_t sum;

	memset(&adcch_conf, 0, sizeof(struct adc_channel_config));

	// Common configuration
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);

	for (input = 0; input < num_inputs; input++) {
		adcch_set_input(&adcch_conf, mux_pos_inputs[input],
				mux_neg_inputs[input], gain);
		adcch_write_configuration(adc, ch_mask, &adcch_conf);

		// Enable and do dummy conversion
		adc_enable(adc);
		adc_start_conversion(adc, ch_mask);
		adc_wait_for_interrupt_flag(adc, ch_mask);

		// Read an average
		sum = 0;
		for (i = 0; i < NUM_AVERAGE_SAMPLES; i++) {
			adc_start_conversion(adc, ch_mask);
			adc_wait_for_interrupt_flag(adc, ch_mask);
			sum += (int16_t)adc_get_result(adc, ch_mask);
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

	// Cap compare values
	if(upper > upper_limit) {
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
 * \param adc_ch The ADC channel used.
 * \param mux_pos_input The positive ADC input MUX setting used.
 * \param mux_neg_input The negative ADC input MUX setting used.
 * \param lower_limit Lower limit of accepted range, can override the limit
 * implicitly given by the accepted deviation.
 * \param upper_limit Upper limit of accepted range, can override the limit
 * implicitly given by the accepted deviation.
 * \param gain ADC input gain setting used.
 * \param value_is_12bit Specify the resolution of the ADC result and thus the
 * accepted deviation:
 * \arg \c true for +/- CONF_TEST_ACCEPT_DELTA_12BIT
 * \arg \c false for +/- CONF_TEST_ACCEPT_DELTA_8BIT
 */
static void verify_signed_result(const struct test_case *test,
		int16_t expected, int16_t actual, uint8_t adc_ch,
		uint8_t mux_pos_input, uint8_t mux_neg_input,
		int16_t lower_limit, int16_t upper_limit, uint8_t gain,
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

	// Cap compare values
	if(upper > upper_limit) {
		upper = upper_limit;
	}
	if (lower < lower_limit) {
		lower = lower_limit;
	}

	test_assert_true(test, ((actual <= upper) && (actual >= lower)),
			"ADC result (%d) is outside acceptable range (%d +-%d)"
			". ADCA ch%d, pin pair %d-%d input, gain %d", actual,
			expected, delta, adc_ch, mux_pos_input, mux_neg_input,
			gain);
}

//@}

//! \name ADC unit test functions
//@{

/**
 * \internal
 * \brief Test single ended conversion in 12-bit mode using the DAC
 *
 * This tests output three different values on the two DAC channels:
 * - 0 (output analog value is greater than 0, as the DAC cannot go that low)
 * - 1/2 * \ref DAC_MAX Half of the maximum value of the DAC
 * - \ref DAC_MAX The maximum value (VREF) of the DAC.
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, and the results are compared and checked to see if they
 * are within the acceptable range of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_single_ended_12bit_conversion_test(
		const struct test_case *test)
{
	// Number of MUX inputs that are to be read
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

	// Configure ADC
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	// Configure DAC
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP,
			DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);
	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);


	// Set outputs as zero
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MIN);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MIN);

	for(dac_channel = 0; dac_channel < 2; dac_channel++) {
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

	// Set outputs as 1/2 * MAX_VALUE
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX / 2);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX / 2);

	for(dac_channel = 0; dac_channel < 2; dac_channel++) {
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

	// Set outputs as MAX_VALUE
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX);

	for(dac_channel = 0; dac_channel < 2; dac_channel++) {
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
 * \brief Test single ended conversion in 8-bit mode using the DAC
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
static void run_single_ended_8bit_conversion_test(
		const struct test_case *test)
{
	// Number of MUX inputs that are to be read
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

	// Configure ADC
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_8,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	// Configure DAC
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP, DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);

	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);

	// Set outputs as zero
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MIN);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MIN);

	for(dac_channel = 0; dac_channel < 2; dac_channel++) {
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
						ADC_UNSIGNED_8BIT_MAX, false);
			}
		}
	}

	// Set outputs as 1/2 * MAX_VALUE
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX / 2);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX / 2);

	for(dac_channel = 0; dac_channel < 2; dac_channel++) {
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
						ADC_UNSIGNED_8BIT_MAX / 2,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_8BIT_MAX, false);
			}
		}
	}

	// Set outputs as MAX_VALUE
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX);

	for(dac_channel = 0; dac_channel < 2; dac_channel++) {
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
						ADC_UNSIGNED_8BIT_MAX,
						results[mux_index],
						dac_channel,
						adc_channel,
						channelgroup[dac_channel]
						[mux_index],
						ADC_UNSIGNED_8BIT_MAX, false);
			}
		}
	}
}

/**
 * \internal
 * \brief Test differential conversion in 12-bit mode using the DAC
 *
 * This tests output three different values on the two DAC channels:
 * - 1/2 * \ref DAC_MAX on both outputs to get a differential zero
 * - \ref DAC_MAX on positive and \ref DAC_MIN on negative to get max positive
 * result
 * - \ref DAC_MIN on positive and \ref DAC_MAX on negative to get max negative
 * result
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, and the results are compared and checked to see if they
 * are within the acceptable range of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_differential_12bit_conversion_test(
		const struct test_case *test)
{
	// Number of MUX inputs that are to be read
	const uint8_t num_inputs = 4;

	/* Connection between DAC outputs and ADC MUX inputs
	 * input 0, 2, 4, 6 is connected to DACA0
	 * input 1, 3, 5, 7 is connected to DACA1.
	 */
	const uint8_t channel_pos[] = {0, 2, 4, 6};
	const uint8_t channel_neg[] = {1, 3, 5, 7};

	uint8_t adc_channel;
	uint8_t mux_index;
	int16_t results[4];
	struct dac_config dac_conf;
	struct adc_config adc_conf;

	// Configure ADC
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	// Configure DAC
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP,
			DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);
	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);


	// Set outputs to same (1/2 * MAX_VALUE) to get zero
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX / 2);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX / 2);

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS; adc_channel++) {
		differential_signed_average(&ADCA, 1 << adc_channel,
				channel_pos, channel_neg, num_inputs, results,
				1);
		for (mux_index = 0; mux_index < num_inputs; mux_index++) {
			verify_signed_result(test, ADC_ZERO,
					results[mux_index], adc_channel,
					channel_pos[mux_index],
					channel_neg[mux_index],
					ADC_SIGNED_12BIT_MIN,
					ADC_SIGNED_12BIT_MAX, 1, true);
		}
	}

	// Set output to max positive range for positive result
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MIN);

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS; adc_channel++) {
		differential_signed_average(&ADCA, 1 << adc_channel,
				channel_pos, channel_neg, num_inputs, results,
				1);
		for (mux_index = 0; mux_index < num_inputs; mux_index++) {
			verify_signed_result(test, ADC_SIGNED_12BIT_MAX,
					results[mux_index], adc_channel,
					channel_pos[mux_index],
					channel_neg[mux_index],
					ADC_SIGNED_12BIT_MIN,
					ADC_SIGNED_12BIT_MAX, 1, true);
		}
	}

	// Set output to max negative range for negative result
	dac_wait_for_channel_ready(&DACA, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH0, DAC_MIN);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MAX);

	/* Read all ADC pins connected to active DAC output.
	 * All channels are converted NUM_SAMPLES times and the
	 * final value used for the assert is an average.
	 */
	for (adc_channel = 0; adc_channel < NUM_CHANNELS; adc_channel++) {
		differential_signed_average(&ADCA, 1 << adc_channel,
				channel_pos, channel_neg, num_inputs, results,
				1);
		for (mux_index = 0; mux_index < num_inputs; mux_index++) {
			verify_signed_result(test, ADC_SIGNED_12BIT_MIN,
					results[mux_index], adc_channel,
					channel_pos[mux_index],
					channel_neg[mux_index],
					ADC_SIGNED_12BIT_MIN,
					ADC_SIGNED_12BIT_MAX, 1, true);
		}
	}
}

/**
 * \internal
 * \brief Test differential conversion with gain in 12-bit mode using the DAC
 *
 * This test outputs a gain compensated level on DAC output that should result
 * in a value of half maximum positive value on the ADC.
 *
 * These values are then measured using the ADC on the pins that are connected
 * to the DAC channel, and the results are compared and checked to see if they
 * are within the acceptable range of values that passes the test.
 *
 * \param test Current test case.
 */
static void run_differential_12bit_with_gain_conversion_test(
		const struct test_case *test)
{
	// Number of MUX inputs that are to be read
	const uint8_t num_inputs = 2;

	/* Connection between DAC outputs and ADC MUX inputs.
	 * Only the high nibble on PORTA is possible for gain.
	 * input 4, 6 is connected to DACA0
	 * input 5, 7 is connected to DACA1.
	 */
	const uint8_t channel_pos[] = {4, 6};
	const uint8_t channel_neg[] = {5, 7};

	/*
	 * Go through gain level up to 8, since any higher gives too much
	 * propagated error for sensible unit test limits.
	 */
	const uint8_t gain[] = {1, 2, 4, 8};
	uint8_t gain_index;

	uint8_t adc_channel;
	uint8_t mux_index;
	int16_t results[2];
	struct dac_config dac_conf;
	struct adc_config adc_conf;

	// Configure ADC
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
			ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);

	// Configure DAC
	dac_read_configuration(&DACA, &dac_conf);
	dac_set_conversion_parameters(&dac_conf, DAC_REF_BANDGAP,
			DAC_ADJ_RIGHT);
	dac_set_active_channel(&dac_conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&dac_conf, 0, 0);
	dac_set_conversion_interval(&dac_conf, 10);
	dac_set_refresh_interval(&dac_conf, 20);
	dac_write_configuration(&DACA, &dac_conf);
	dac_enable(&DACA);


	// Set negative output to zero
	dac_wait_for_channel_ready(&DACA, DAC_CH1);
	dac_set_channel_value(&DACA, DAC_CH1, DAC_MIN);

	for (gain_index = 0; gain_index < sizeof(gain); gain_index++) {
		// Set positive output to half positive range adjusted to gain
		dac_wait_for_channel_ready(&DACA, DAC_CH0);
		dac_set_channel_value(&DACA, DAC_CH0, DAC_MAX /
				(gain[gain_index] * 2));

		/* Read all ADC pins connected to active DAC output.
		 * All channels are converted NUM_SAMPLES times and the
		 * final value used for the assert is an average.
		 */
		for (adc_channel = 0; adc_channel < NUM_CHANNELS;
				adc_channel++) {
			differential_signed_average(&ADCA, 1 << adc_channel,
					channel_pos, channel_neg, num_inputs,
					results, gain[gain_index]);
			for (mux_index = 0; mux_index < num_inputs;
					mux_index++) {
				verify_signed_result(test,
						ADC_SIGNED_12BIT_MAX / 2,
						results[mux_index],
						adc_channel,
						channel_pos[mux_index],
						channel_neg[mux_index],
						ADC_SIGNED_12BIT_MIN,
						ADC_SIGNED_12BIT_MAX,
						gain[gain_index], true);
			}
		}
	}
}

//@}

/**
 * \brief Run ADC unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * ADC unit test suite and runs it.
 */
int main(void)
{

	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	board_init();
	sysclk_init();
	sleepmgr_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Define single ended conversion test cases
	DEFINE_TEST_CASE(single_ended_12bit_conversion_test, NULL,
			run_single_ended_12bit_conversion_test, NULL,
			"Single ended conversion with 12-bit result");
	DEFINE_TEST_CASE(single_ended_8bit_conversion_test, NULL,
			run_single_ended_8bit_conversion_test, NULL,
			"Single ended conversion with 8-bit result");

	// Define differential conversion test cases
	DEFINE_TEST_CASE(differential_conversion_test, NULL,
			run_differential_12bit_conversion_test, NULL,
			"Differential conversion with 12-bit result");
	DEFINE_TEST_CASE(differential_conversion_with_gain_test, NULL,
			run_differential_12bit_with_gain_conversion_test, NULL,
			"Differential conversion with 12-bit result and gain");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(adc_tests) = {
		&single_ended_12bit_conversion_test,
		&single_ended_8bit_conversion_test,
		&differential_conversion_test,
		&differential_conversion_with_gain_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(adc_suite, adc_tests,
			"XMEGA ADC driver test suite");

	// Run all tests in the suite
	test_suite_run(&adc_suite);

	while (1) {
		// Intentionally left empty.
	}
}
