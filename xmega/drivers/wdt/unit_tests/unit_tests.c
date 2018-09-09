/**
 * \file
 *
 * \brief Unit tests for Watchdog timer module
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
 * visit <a href="http://www.microchip.com/">Microchip</a>.\n
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
