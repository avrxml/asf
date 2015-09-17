/**
 * \file
 *
 * \brief Unit tests for QDec driver
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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <asf.h>
#include <conf_test.h>

#if XMEGA_E
#  define tc_enable               tc45_enable
#  define tc_get_resolution       tc45_get_resolution
#  define tc_set_resolution       tc45_set_resolution
#  define tc_read_count           tc45_read_count
#  define tc_write_count          tc45_write_count
#  define tc_write_period         tc45_write_period
#  define tc_set_wgm              tc45_set_wgm
#  define TC_WG_NORMAL            TC45_WG_NORMAL
#endif

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the XMEGA QDec module
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.*
 *
 * \section board_setup Board setup
 * For XMEGA-E5 Xplained board:
 * - Connect USB (Virtual port COM) to a computer and
 *   the communication terminal configuration should be 38400 baud, no parity.
 * For XMEGA-B1 Xplained board:
 * - Connect the UART RXD (PE2) and TXD (PE3) to a RS232 port.
 *   the communication terminal configuration should be 38400 baud, no parity.
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
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
/* @} */

/* ! \name QDEC unit test functions */
/* @{ */

/* ! Definition of Quadrature Encoder steps */
#define QENC_PH0_PIN             (0x01)
#define QENC_PH90_PIN            (0x02)
#define QENC_INDEX_PIN           (0x04)
#define QENC_PH0_PH90_INDEX_PINS (0x07)
#define QENC_STEP_1              (0)
#define QENC_STEP_2              (QENC_PH90_PIN)
#define QENC_STEP_3              (QENC_PH90_PIN | QENC_PH0_PIN)
#define QENC_STEP_4              (QENC_PH0_PIN)

/* ! Structure to test one step of QDec */
struct qenc_step {
	uint8_t encoder;
	uint8_t qdec_pos;
};

/**
 * \internal
 * \brief Initializes the timer counter used to generate a delay
 */
static void main_delay_init(void)
{
	tc_enable(CONF_DELAY_TC);
	tc_set_wgm(CONF_DELAY_TC, TC_WG_NORMAL);
	tc_set_resolution(CONF_DELAY_TC, 1);
	tc_write_period(CONF_DELAY_TC, 0xFFFF);
}

/**
 * \internal
 * \brief Generate a delay in ms
 *
 * \param delay_ms  Number of ms to wait
 */
static void main_delay_ms(uint16_t delay_ms)
{
	uint16_t ticks_delay;

	tc_write_count(CONF_DELAY_TC, 0);
	ticks_delay = (uint64_t)tc_get_resolution(CONF_DELAY_TC) * delay_ms
			/ 1000;
	while (ticks_delay > tc_read_count(CONF_DELAY_TC)) {
	}
}

/**
 * \internal
 * \brief Reset pins used to simulate Quadrature encoder
 */
static void generator_qenc_reset(void)
{
	CONF_QDEC_PORT->DIRSET = QENC_PH0_PH90_INDEX_PINS
			<< CONF_QDEC_PINS_BASE;
	CONF_QDEC_PORT->OUT
			= (CONF_QDEC_PORT->OUT & ~(QENC_PH0_PH90_INDEX_PINS
			<< CONF_QDEC_PINS_BASE));
	main_delay_ms(10);
}

/**
 * \internal
 * \brief Set Quadrature Encoder pins to a specific value
 *
 * \param value  Output pins states to generate
 */
static void generator_qenc(uint8_t value)
{
	CONF_QDEC_PORT->OUT
			= (CONF_QDEC_PORT->OUT & ~(QENC_PH0_PH90_INDEX_PINS
			<< CONF_QDEC_PINS_BASE))
			| (value << CONF_QDEC_PINS_BASE);
}

/**
 * \internal
 * \brief QDEC Unit Test Clean up for XMEGA PORT D and QDEC
 *
 * \param test       Current test case
 * \param config     Configuration of QDec
 * \param sequence   Test sequence to execute
 * \param seq_size   size of test sequence
 */
static void generator_qenc_seq(const struct test_case *test,
		qdec_config_t *config, struct qenc_step *sequence, uint8_t seq_size)
{
	uint16_t pos;

	for (uint8_t i = 0; i < seq_size; i++) {
		generator_qenc(sequence[i].encoder);
		main_delay_ms(10);
		pos = qdec_get_position(config);
		test_assert_true(test, sequence[i].qdec_pos == pos,
				"QDec sequence failure: expected %u, catched %u\r\n",
				sequence[i].qdec_pos, pos);
	}
}

/**
 * \internal
 * \brief Test QDEC in common mode
 *
 * \param test Current test case.
 */
static void run_qdec_common_mode(
		const struct test_case *test)
{
	qdec_config_t config;
	struct qenc_step seq[] = {
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_3, 2},
		{QENC_STEP_4, 3},
		{QENC_STEP_1, 4},
		{QENC_STEP_2, 5},
		{QENC_STEP_3, 6},
		{QENC_STEP_4, 7},
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_1, 0},
		{QENC_STEP_4, 7},
		{QENC_STEP_3, 6},
	};

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_revolution(&config, 8);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc_seq(test, &config, seq, sizeof(seq) / sizeof(seq[0]));
}

#if XMEGA_E

/**
 * \internal
 * \brief Test QDEC in rotary mode
 *
 * \param test Current test case.
 */
static void run_qdec_rotary_mode(
		const struct test_case *test)
{
	qdec_config_t config;
	struct qenc_step seq[] = {
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_3, 2},
		{QENC_STEP_4, 3},
		{QENC_STEP_1, 4},
		{QENC_STEP_4, 4},
		{QENC_STEP_1, 4},
		{QENC_STEP_4, 4},
		{QENC_STEP_3, 2},
		{QENC_STEP_2, 1},
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 0},
		{QENC_STEP_3, 2},
	};

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_rotary(&config);
	qdec_config_revolution(&config, 8);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc_seq(test, &config, seq, sizeof(seq) / sizeof(seq[0]));
}

#endif

/**
 * \internal
 * \brief Test QDEC in index mode on step QENC_STEP_1
 *
 * \param test Current test case.
 */
static void run_qdec_index1_mode(
		const struct test_case *test)
{
	qdec_config_t config;
	struct qenc_step seq[] = {
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_3, 2},
		{QENC_STEP_4 | QENC_INDEX_PIN, 3},
		{QENC_STEP_1 | QENC_INDEX_PIN, 0},
		{QENC_STEP_2 | QENC_INDEX_PIN, 1},
		{QENC_STEP_3 | QENC_INDEX_PIN, 2},
		{QENC_STEP_4, 3},
		{QENC_STEP_1, 4},
		{QENC_STEP_2, 5},
		{QENC_STEP_3, 6},
		{QENC_STEP_4 | QENC_INDEX_PIN, 7},
		{QENC_STEP_1 | QENC_INDEX_PIN, 0},
	};

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_index_pin(&config, false, QDEC_STATE_NO_PH0_NO_PH90);
	qdec_config_revolution(&config, 8);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc_seq(test, &config, seq, sizeof(seq) / sizeof(seq[0]));
}

/**
 * \internal
 * \brief Test QDEC in index mode on step QENC_STEP_2
 *
 * \param test Current test case.
 */
static void run_qdec_index2_mode(
		const struct test_case *test)
{
	qdec_config_t config;
	struct qenc_step seq[] = {
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_3, 2},
		{QENC_STEP_4, 3},
		{QENC_STEP_1 | QENC_INDEX_PIN, 4},
		{QENC_STEP_2 | QENC_INDEX_PIN, 0},
		{QENC_STEP_3 | QENC_INDEX_PIN, 1},
		{QENC_STEP_4 | QENC_INDEX_PIN, 2},
		{QENC_STEP_1, 3},
		{QENC_STEP_2, 4},
		{QENC_STEP_3, 5},
		{QENC_STEP_4, 6},
		{QENC_STEP_1 | QENC_INDEX_PIN, 7},
		{QENC_STEP_2 | QENC_INDEX_PIN, 0},
		{QENC_STEP_3 | QENC_INDEX_PIN, 1},
		{QENC_STEP_4 | QENC_INDEX_PIN, 2},
	};

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_index_pin(&config, false, QDEC_STATE_PH0_NO_PH90);
	qdec_config_revolution(&config, 8);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc_seq(test, &config, seq, sizeof(seq) / sizeof(seq[0]));
}

/**
 * \internal
 * \brief Test QDEC in index mode on step QENC_STEP_3
 *
 * \param test Current test case.
 */
static void run_qdec_index3_mode(
		const struct test_case *test)
{
	qdec_config_t config;
	struct qenc_step seq[] = {
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_3, 2},
		{QENC_STEP_4, 3},
		{QENC_STEP_1, 4},
		{QENC_STEP_2 | QENC_INDEX_PIN, 5},
		{QENC_STEP_3 | QENC_INDEX_PIN, 0},
		{QENC_STEP_4 | QENC_INDEX_PIN, 1},
		{QENC_STEP_1 | QENC_INDEX_PIN, 2},
		{QENC_STEP_2, 3},
		{QENC_STEP_3, 4},
		{QENC_STEP_4, 5},
		{QENC_STEP_1, 6},
		{QENC_STEP_2 | QENC_INDEX_PIN, 7},
		{QENC_STEP_3 | QENC_INDEX_PIN, 0},
		{QENC_STEP_4 | QENC_INDEX_PIN, 1},
		{QENC_STEP_1 | QENC_INDEX_PIN, 2},
	};

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_index_pin(&config, false, QDEC_STATE_PH0_PH90);
	qdec_config_revolution(&config, 8);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc_seq(test, &config, seq, sizeof(seq) / sizeof(seq[0]));
}

/**
 * \internal
 * \brief Test QDEC in index mode on step QENC_STEP_4
 *
 * \param test Current test case.
 */
static void run_qdec_index4_mode(
		const struct test_case *test)
{
	qdec_config_t config;
	struct qenc_step seq[] = {
		{QENC_STEP_1, 0},
		{QENC_STEP_2, 1},
		{QENC_STEP_3, 2},
		{QENC_STEP_4, 3},
		{QENC_STEP_1, 4},
		{QENC_STEP_2, 5},
		{QENC_STEP_3 | QENC_INDEX_PIN, 6},
		{QENC_STEP_4 | QENC_INDEX_PIN, 0},
		{QENC_STEP_1 | QENC_INDEX_PIN, 1},
		{QENC_STEP_2 | QENC_INDEX_PIN, 2},
		{QENC_STEP_3, 3},
		{QENC_STEP_4, 4},
		{QENC_STEP_1, 5},
		{QENC_STEP_2, 6},
		{QENC_STEP_3 | QENC_INDEX_PIN, 7},
		{QENC_STEP_4 | QENC_INDEX_PIN, 0},
		{QENC_STEP_1 | QENC_INDEX_PIN, 1},
		{QENC_STEP_2 | QENC_INDEX_PIN, 2},
	};

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_index_pin(&config, false, QDEC_STATE_NO_PH0_PH90);
	qdec_config_revolution(&config, 8);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc_seq(test, &config, seq, sizeof(seq) / sizeof(seq[0]));
}

/**
 * \internal
 * \brief Common function to test frequency computation from QDEC
 *
 * \param test       Current test case
 * \param res        QDec resolution for this test
 * \param delay      Delay between each quadrature steps
 * \param freq_unit  QDec frequency unit used for this test
 * \param freq       Expected frequency for this test
 */
static void run_qdec_freq_mode(const struct test_case *test, uint16_t res,
		uint8_t delay, uint16_t freq_unit, uint16_t freq)
{
	qdec_config_t config;
	uint32_t freq_computed;

	/* Set to 0 the pins */
	generator_qenc_reset();

	/* QDec configuration */
	qdec_get_config_defaults(&config);
	qdec_config_phase_pins(&config, CONF_QDEC_PORT, CONF_QDEC_PINS_BASE,
			false, 1);
	qdec_config_tc(&config, CONF_QDEC_TC);
	qdec_config_enable_freq(&config, freq_unit);
	qdec_config_freq_tc(&config, CONF_QDEC_FREQ_TC);
	qdec_config_revolution(&config, res);
	/* QDec enable */
	qdec_enabled(&config);

	/* Reenable dir out on pins */
	generator_qenc_reset();

	/* Launch test */
	generator_qenc(QENC_STEP_1);
	main_delay_ms(delay);
	generator_qenc(QENC_STEP_2);
	main_delay_ms(delay);
	generator_qenc(QENC_STEP_3);
	main_delay_ms(delay);
	generator_qenc(QENC_STEP_4);
	main_delay_ms(delay);
	generator_qenc(QENC_STEP_1);
	main_delay_ms(delay);
	generator_qenc(QENC_STEP_2);
	main_delay_ms(delay);
	generator_qenc(QENC_STEP_3);
	freq_computed = qdec_get_frequency(&config);
	/* 5% tolerance on Frequency */
	if (freq_unit == 1) {
		test_assert_true(test,
				(freq_computed >= (freq - ((freq * 5) / 100))) &&
				(freq_computed <= (freq + ((freq * 5) / 100))),
				"QDec frequency failure: expected %umHz, catched %umHz\r\n", freq,
				freq_computed);
	} else {
		test_assert_true(test,
				(freq_computed >= (freq - ((freq * 5) / 100))) &&
				(freq_computed <= (freq + ((freq * 5) / 100))),
				"QDec frequency failure: expected %uHz, catched %uHz\r\n", freq,
				freq_computed);
	}
}

/**
 * \internal
 * \brief Test frequency computation from QDEC with 100mHz
 *
 * \param test Current test case.
 */
static void run_qdec_freq1_mode(
		const struct test_case *test)
{
	run_qdec_freq_mode(test, 100, 100, 1, 100);
}

/**
 * \internal
 * \brief Test frequency computation from QDEC with 1000mHz
 *
 * \param test Current test case.
 */
static void run_qdec_freq2_mode(
		const struct test_case *test)
{
	run_qdec_freq_mode(test, 100, 10, 1, 1000);
}

/**
 * \internal
 * \brief Test frequency computation from QDEC with 10Hz
 *
 * \param test Current test case.
 */
static void run_qdec_freq3_mode(
		const struct test_case *test)
{
	run_qdec_freq_mode(test, 16, 10, 1000, 6);
}

/* @} */

/**
 * \brief Run QDEC unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * QDEC unit test suite and runs it.
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
	sleepmgr_init();

	main_delay_init();

	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	DEFINE_TEST_CASE(qdec_common_mode, NULL,
			run_qdec_common_mode, NULL,
			"QDEC Common mode Test");
#if XMEGA_E
	DEFINE_TEST_CASE(qdec_rotary_mode, NULL,
			run_qdec_rotary_mode, NULL,
			"QDEC Rotary mode Test");
#endif
	DEFINE_TEST_CASE(qdec_index1_mode, NULL,
			run_qdec_index1_mode, NULL,
			"QDEC Index (step 1) mode Test");
	DEFINE_TEST_CASE(qdec_index2_mode, NULL,
			run_qdec_index2_mode, NULL,
			"QDEC Index (step 2) mode Test");
	DEFINE_TEST_CASE(qdec_index3_mode, NULL,
			run_qdec_index3_mode, NULL,
			"QDEC Index (step 3) mode Test");
	DEFINE_TEST_CASE(qdec_index4_mode, NULL,
			run_qdec_index4_mode, NULL,
			"QDEC Index (step 4) mode Test");
	DEFINE_TEST_CASE(qdec_freq1_mode, NULL,
			run_qdec_freq1_mode, NULL,
			"QDEC Frequency mode Test1");
	DEFINE_TEST_CASE(qdec_freq2_mode, NULL,
			run_qdec_freq2_mode, NULL,
			"QDEC Frequency mode Test2");
	DEFINE_TEST_CASE(qdec_freq3_mode, NULL,
			run_qdec_freq3_mode, NULL,
			"QDEC Frequency mode Test3");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(qdec_tests) = {
		&qdec_common_mode,
#if XMEGA_E
		&qdec_rotary_mode,
#endif
		&qdec_index1_mode,
		&qdec_index2_mode,
		&qdec_index3_mode,
		&qdec_index4_mode,
		&qdec_freq1_mode,
		&qdec_freq2_mode,
		&qdec_freq3_mode,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(qdec_suite, qdec_tests,
			"XMEGA QDEC driver test suite");

	/* Run all tests in the suite */
	test_suite_run(&qdec_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
