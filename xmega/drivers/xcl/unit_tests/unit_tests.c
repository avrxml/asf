/**
 * \file
 *
 * \brief Unit tests for XCL driver
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test for the XMEGA E XCL module
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_xcl.h
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup(s):
 * - STK600 with RC032X routing card
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref xcl_group "XCL driver"
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

/* ! \name XCL unit test functions */
/* @{ */

/**
 * \internal
 * \brief XCL Unit Test Clean up for XMEGA PORT D and XCL
 *
 * This function cleans up the XCL module configuration and PORT D set up.
 *
 * \param test Current test case.
 */
static void clean_xcl_io_portd(const struct test_case *test)
{
	XCL.CTRLA = 0;
	XCL.CTRLB = 0;
	XCL.CTRLC = 0;
	XCL.CTRLD = 0;
	PORTD.DIRCLR = 0x1F;
	PORTD.OUTCLR = 0x1F;
}

/**
 * \internal
 * \brief Test XCL Glue Logic module with 2 Input XOR
 *
 * This tests check the capabilty of the XCL Glue Logic driver to hangle a 2
 * inputs XOR operation.
 *
 * \param test Current test case.
 */
static void run_xcl_glue_logic_2inputs_xor_test(
		const struct test_case *test)
{
	port_pin_t in0, in1, out0;
	bool out;

	in0 = IOPORT_CREATE_PIN(PORTD, 2);
	in1 = IOPORT_CREATE_PIN(PORTD, 0);
	out0 = IOPORT_CREATE_PIN(PORTD, 4);

	ioport_configure_pin(in0, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(in1, IOPORT_DIR_OUTPUT);

	/* Configure the XCL module:
	 * - Asynchronous mode usage (locks Power Down sleep mode)
	 * - Configure XCL to use XMEGA port D
	 * - Configure LUT in 2 separe LUTs with 2 differents inputs
	 * - LUT IN0 input on pin 2 or port D
	 * - LUT IN1 input on pin 0 or port D
	 * - LUT OUT0 output on pin 4 or port D
	 * - No time Delay for both LUT0 and LUT1
	 * - LUT0 performs XOR operation
	 */
	xcl_enable(XCL_ASYNCHRONOUS);
	xcl_port(PD);
	xcl_lut_type(LUT_2LUT2IN);
	xcl_lut_in0(LUT_IN_PINL);
	xcl_lut_in1(LUT_IN_PINL);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	xcl_lut0_output(LUT0_OUT_PIN4);
	xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE);
	xcl_lut0_truth(XOR);

	gpio_set_pin_low(in0);
	gpio_set_pin_low(in1);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == false, "XOR2 failure");

	gpio_set_pin_high(in0);
	gpio_set_pin_low(in1);
	out = ioport_get_pin_level(out0);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	test_assert_true(test, out == true, "XOR2 failure");

	gpio_set_pin_low(in0);
	gpio_set_pin_high(in1);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == true, "XOR2 failure");

	gpio_set_pin_high(in0);
	gpio_set_pin_high(in1);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == false, "XOR2 failure");
}

/**
 * \internal
 * \brief Test XCL Glue Logic module with 3 Inputs XOR
 *
 * This tests check the capabilty of the XCL Glue Logic Drivers to hangle
 * a 3 inputs XOR operation.
 *
 * \param test Current test case.
 */
static void run_xcl_glue_logic_3inputs_xor_test(
		const struct test_case *test)
{
	port_pin_t in0, in2, in3, out0;
	bool out;

	in0 = IOPORT_CREATE_PIN(PORTD, 2);
	in2 = IOPORT_CREATE_PIN(PORTD, 1);
	in3 = IOPORT_CREATE_PIN(PORTD, 3);
	out0 = IOPORT_CREATE_PIN(PORTD, 4);

	ioport_configure_pin(in0, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(in2, IOPORT_DIR_OUTPUT);
	ioport_configure_pin(in3, IOPORT_DIR_OUTPUT);

	/* Configure the XCL module:
	 * - Asynchronous mode usage (locks Power Down sleep mode)
	 * - Configure XCL to use ATxmega32E5 port D
	 * - Configure LUT in 1 LUTs with 3 differents inputs
	 * - LUT IN0 input on pin 2 of port D
	 * - LUT IN1 input on XCL LUT OUT1
	 * - LUT OUT0 output on pin 4 of port D
	 * - LUT IN2 input on pin 1 of port D
	 * - LUT IN3 input on pin 3 of port D
	 * - LUT OUT1 output on LUT IN1
	 * - No time Delay for both LUT0 and LUT1
	 * - LUT0 performs XOR operation
	 * - LUT1 performs XOR operation
	 */
	xcl_enable(XCL_ASYNCHRONOUS);
	xcl_port(PD);
	xcl_lut_type(LUT_1LUT3IN);
	xcl_lut_in0(LUT_IN_PINL);
	xcl_lut_in1(LUT_IN_XCL);
	xcl_lut_in2(LUT_IN_PINL);
	xcl_lut_in3(LUT_IN_PINL);
	xcl_lut0_output(LUT0_OUT_PIN4);
	xcl_lut_config_delay(DLY11, LUT_DLY_DISABLE, LUT_DLY_DISABLE);
	xcl_lut0_truth(XOR);
	xcl_lut1_truth(XOR);

	gpio_set_pin_low(in0);
	gpio_set_pin_low(in2);
	gpio_set_pin_low(in3);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == false, " XCL XOR failure");

	gpio_set_pin_high(in0);
	gpio_set_pin_low(in2);
	gpio_set_pin_low(in3);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == true, " XCL XOR failure");

	gpio_set_pin_low(in0);
	gpio_set_pin_high(in2);
	gpio_set_pin_low(in3);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == true, " XCL XOR failure");

	gpio_set_pin_low(in0);
	gpio_set_pin_low(in2);
	gpio_set_pin_high(in3);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == true, " XCL XOR failure");

	gpio_set_pin_low(in0);
	gpio_set_pin_high(in2);
	gpio_set_pin_high(in3);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == false, " XCL XOR failure");

	gpio_set_pin_high(in0);
	gpio_set_pin_high(in2);
	gpio_set_pin_high(in3);
	asm("nop");
	asm("nop"); /* give two cycles propagation delay to read the port pin */
	out = ioport_get_pin_level(out0);
	test_assert_true(test, out == true, " XCL XOR failure");
}

/* @} */

/**
 * \brief Run XCL unit tests
 *
 * Initializes the clock system, board and serial output, then sets up the
 * XCL unit test suite and runs it.
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
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define 2 inputs XOR test */
	DEFINE_TEST_CASE(xcl_glue_logic_2inputs_xor_test, NULL,
			run_xcl_glue_logic_2inputs_xor_test, clean_xcl_io_portd,
			"XCL 2 Inputs LUT XOR Test");

	/* Define 3 inputs XOR test */
	DEFINE_TEST_CASE(xcl_glue_logic_3inputs_xor_test, NULL,
			run_xcl_glue_logic_3inputs_xor_test, clean_xcl_io_portd,
			"XCL 3 Inputs Chained LUTs XOR Test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(xcl_tests) = {
		&xcl_glue_logic_2inputs_xor_test,
		&xcl_glue_logic_3inputs_xor_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(xcl_suite, xcl_tests,
			"XMEGA E XCL driver test suite");

	/* Run all tests in the suite */
	test_suite_run(&xcl_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
