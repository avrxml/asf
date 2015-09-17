/**
 * \file
 *
 * \brief Unit tests for GLOC driver.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 * This is the unit test application for the GLOC driver.
 * It contains one test case for the GLOC module:
 * - Test 4 inputs XOR.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_uart_serial.h
 * - \ref conf_gloc.h
 *
 * \section device_info Device Info
 * This example has been tested on a sam4lc4c_sam4l_user_board with the 
 * following setup:
 * - A 12MHz oscillator on OSC0,
 * - A USART connected on PIN_PC11 (RxD) and PIN_PC12 (TxD) pins,
 * - The selected GLOC input/output pins are PIN_PA20 (GLOC_IN0), PIN_PA21 
 *   (GLOC_IN1), PIN_PA22 (GLOC_IN2), PIN_PA07 (GLOC_IN3) and PIN_PA24 
 *   (GLOC_OUT0).
 * - GPIO pins used to set/clear GLOC input pins are PIN_PC01 (connected to 
 *   GLOC_IN0), PIN_PC07 (connected to GLOC_IN1), PIN_PC08 (connected to 
 *   GLOC_IN2) and PIN_PC13 (connected to GLOC_IN3).
 * - GPIO pin used to read GLOC output pin is PA24 (connected to GLOC_OUT0).
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

/** GLOC instance */
struct gloc_dev_inst   dev_inst;
/** GLOC LUT configuration */
struct gloc_lut_config lut_config;

/** XOR truth table for four input */
#define XOR_TRUTH_TABLE_FOUR_INPUT     0x6996u

/**
 * \brief Test GLOC Logic module with 4 Inputs XOR.
 *
 * This tests check the capabilty of the Glue Logic driver to handle 4
 * inputs XOR operation.
 *
 * \param test Current test case.
 */
static void run_gloc_4inputs_xor_test(const struct test_case *test)
{
	bool out;

	/* Configure pins for changing GLOC input */
	ioport_set_pin_dir(CONF_TEST_GLOC_IN0, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CONF_TEST_GLOC_IN1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CONF_TEST_GLOC_IN2, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CONF_TEST_GLOC_IN3, IOPORT_DIR_OUTPUT);

	/* Set the 4 inputs XOR truth table value in LUT0. */
	gloc_lut_get_config_defaults(&lut_config);
	lut_config.truth_table_value = XOR_TRUTH_TABLE_FOUR_INPUT;
	gloc_lut_set_config(&dev_inst, 0, &lut_config);

	/* Test GLOC with different input */
	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/** Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 1");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 2");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 3");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 4");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 5");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 6");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 7");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, LOW);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 8");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 9");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 10");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 11");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 12");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 13");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 14");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, LOW);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == true, "4 inputs XOR failed at 15");

	ioport_set_pin_level(CONF_TEST_GLOC_IN0, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN1, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN2, HIGH);
	ioport_set_pin_level(CONF_TEST_GLOC_IN3, HIGH);
	/* Ensure a minimum propagation delay to read the port pin. */
	delay_us(50);
	out = ioport_get_pin_level(CONF_TEST_GLOC_OUT0);
	test_assert_true(test, out == false, "4 inputs XOR failed at 16");
}

/**
 * \brief Run GLOC driver unit tests.
 */
int main(void)
{
	sysclk_init();
	board_init();

	const usart_serial_options_t usart_serial_options = {
		.baudrate = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits = CONF_TEST_STOPBITS
	};

	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Initialize the GLOC module */
	gloc_init(&dev_inst, GLOC);
	/* Enable the GLOC module. */
	gloc_enable(&dev_inst);

	/* Define all the test cases. */
	DEFINE_TEST_CASE(gloc_4inputs_xor_test, NULL, run_gloc_4inputs_xor_test,
			NULL, "SAM GLOC LUT 4 Inputs XOR Test");

	/* Put test case addresses in an array. */
	DEFINE_TEST_ARRAY(gloc_tests) = {
		&gloc_4inputs_xor_test,
	};

	/* Define the test suite. */
	DEFINE_TEST_SUITE(gloc_suite, gloc_tests, "SAM GLOC driver test suite");

	/* Run all tests in the test suite. */
	test_suite_run(&gloc_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
