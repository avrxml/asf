/**
 * \file
 *
 * \brief Unit tests for Watchdog timer module
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <conf_test.h>
#include <asf.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref wdt_group.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used.
 * This example has been tested with the following setup:
 * - XMEGA-A1 Xplained
 *
 * \section description Description of the unit tests
 * The application will set up a watchdog timeout, and the test will fail
 * if the watchdog reset doesn't occur.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref wdt_group
 * - \ref reset_cause_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other
 * compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information,
 * visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

//! \brief Flag to indicate if last reset is caused by watchdog timeout
bool wdt_rc;

/**
 * \brief Delay for \a ms milliseconds
 *
 * \param ms number of milliseconds to busy wait
 */
static void mdelay(uint16_t ms)
{
	uint32_t count;

	// Approximate the number of loop iterations needed.
	count = sysclk_get_cpu_hz() / 1000;
	count *= ms;
#if defined(__GNUC__)
	count /= 6;
#elif defined(__ICCAVR__)
	count /= 12;
#else
#error Unsupported compiler.
#endif

	do {
		asm("");
	} while (--count);
}

/**
 * \brief Run test to check last reset cause
 *
 * This test will pass if last reset was caused by a watchdog timeout.
 *
 * \param test Current test case.
 */
static void run_reset_cause_test(const struct test_case *test)
{
	test_assert_true(test, wdt_rc, "Watchdog reset failed");
}

//! \brief Set up and run test suite
int main(void)
{
	// Check and clear WDT reset cause
	wdt_rc = reset_cause_get_causes() & CHIP_RESET_CAUSE_WDT;
	reset_cause_clear_causes(CHIP_RESET_CAUSE_WDT);

	// USART init values
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};
	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Check if we are reset by WDT. If not, do a WDT reset
	if (!wdt_rc) {
		wdt_reset_mcu();
		// Wait for reset
		mdelay(1000);
	}

	// Define all the test cases
	DEFINE_TEST_CASE(rc_test, NULL, run_reset_cause_test, NULL,
			"Confirming watchdog reset");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(wdt_tests) = {
		&rc_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(wdt_suite, wdt_tests, "XMEGA WDT driver test suite");

	// Run all tests in the test suite
	test_suite_run(&wdt_suite);
}
