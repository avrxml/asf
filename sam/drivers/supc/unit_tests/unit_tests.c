/**
 * \file
 *
 * \brief Unit tests for SUPC driver.
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
#include <board.h>
#include <sysclk.h>
#include <supc.h>
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>
#include <conf_board.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the SUPC driver.
 * It consists of test cases for the following functionality:
 * - Slow clock source switching
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All SAM devices can be used.
 * This example has been tested with the following setup:
 * - sam3n4c_sam3n_ek
 * - sam3s4c_sam3s_ek
 * - sam3sd8c_sam3s_ek2
 * - sam3u4e_sam3u_ek
 * - sam3x8h_sam3x_ek
 * - sam4s16c_sam4s_ek
 * - sam4sd32c_sam4s_ek2
 * - sam4sd32c_atpl230amb
 * - sam4n16c_sam4n_xplained_pro
 * - sam4c16c_sam4c_ek
 * - sam4cp16b_sam4cp16bmb
 * - sam4cmp16c_sam4cmp_db
 * - sam4cms16c_sam4cms_db
 * - samv71q21_samv71_xplained_ultra
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

//! \name Unit test configuration
//@{
/**
 * \def CONF_TEST_SUPC
 * \brief Switch slow clock source to the crystal oscillator output.
 */
//@}

#define SLOW_CLK_TIMEOUT 0xFFFFFFFF

/* Systick Counter */
static volatile uint32_t gs_ul_ms_ticks = 0U;

/**
 * \brief SysTick handler.
 */
void SysTick_Handler(void)
{
	/* Increment counter necessary in delay(). */
	gs_ul_ms_ticks++;
}

/**
 * \brief Test slow clock source switching.
 *
 * This test switches the slow clock on the crystal oscillator output.
 *
 * \param test Current test case.
 */
static void run_supc_test(const struct test_case *test)
{
	uint32_t status = 0;
	uint32_t timeout = 0;

	/* Test: Switch the slow clock on the crystal oscillator output */
	supc_switch_sclk_to_32kxtal(SUPC, 0);
	do {
		status = supc_get_status(SUPC);
		if (status & SUPC_SR_OSCSEL_CRYST) break;
	} while (timeout++ < SLOW_CLK_TIMEOUT);
	test_assert_true(test, (status & SUPC_SR_OSCSEL_CRYST) == SUPC_SR_OSCSEL_CRYST, "Test: switching slow clock source failed!");
}

/**
 * \brief Run SUPC driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
#ifdef CONF_TEST_CHAR_LENGTH
		.charlength = CONF_TEST_CHAR_LENGTH,
#endif
		.paritytype = CONF_TEST_PARITY,
#ifdef CONF_TEST_STOP_BITS
		.stopbits = CONF_TEST_STOP_BITS,
#endif
	};

	sysclk_init();
	board_init();

	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Set up SysTick Timer for 1 msec interrupts */
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		/* Capture error */
		while (1) {
		}
	}

	/* Define all the test cases */
	DEFINE_TEST_CASE(supc_test, NULL, run_supc_test, NULL,
		"SUPC slow clock source switching");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(supc_tests) = {
		&supc_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(supc_suite, supc_tests, "SAM SUPC driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&supc_suite);

	while (1) {
		/* Busy-wait forever */
	}
}
