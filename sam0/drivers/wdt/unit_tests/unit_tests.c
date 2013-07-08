/**
 * \file
 *
 * \brief SAM D20 Watchdog Unit test
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
#include <asf.h>
#include <stdio_serial.h>
#include <string.h>
#include "conf_test.h"

/* Flag that indicates whether Watchdog was Reset cause */
static volatile bool wdr_flag = false;

/**
 * \brief Test WDT initialization, enabling and disabling
 *
 * If last reset cause was not by Watchdog module, this function will check the
 * initialization of Watchdog module with specified configurations and enabling
 * the module. If last reset was by Watchdog module, this function will check
 * the disabling of Watchdog module
 *
 * \param test Current test case.
 */
static void run_wdt_enable_disable_test(const struct test_case *test)
{
	struct wdt_conf config;
	enum status_code status;

	/* Check if last reset was by Watchdog module */
	if (wdr_flag == false) {
		/* Initialize delay service*/
		delay_init();

		/* Get the Watchdog default configuration */
		wdt_get_config_defaults(&config);

		/* Set the desired configuration */
		config.clock_source         = CONF_WDT_GCLK_GEN;
		config.timeout_period       = CONF_WDT_TIMEOUT_PERIOD;
		config.early_warning_period = WDT_PERIOD_1024CLK;

		/* Initialize the Watchdog module */
		status = wdt_init(&config);

		/* Check if the initialization was successful */
		test_assert_true(test, status == STATUS_OK,
				"WDT initialization failed");

		/* Enable Watchdog module */
		status = wdt_enable();

		/* Check if Watchdog module was successfully enabled */
		test_assert_true(test, status == STATUS_OK,
				"WDT Enabling failed");
	} else {
		/* Disable the Watchdog module */
		status = wdt_disable();

		/* Check if disabling the Watchdog module was successful */
		test_assert_true(test, status == STATUS_OK,
				"WDT disabling failed");
	}
}

/**
 * \brief Test Early Warning of Watchdog module
 *
 * If last reset cause was not Watchdog, following the Watchdog initialization
 * and enabling in the previous test, this function will wait for
 * CONF_WDT_EARLY_WARNING_WAIT_MS and will check if early warning flag is set.
 * Consequently, clear the early warning flag.
 *
 * \param test Current test case.
 */
static void run_wdt_early_warning_test(const struct test_case *test)
{
	/* Check if last reset was by Watchdog module */
	if (wdr_flag == false) {
		/* Wait for Early Warning flag to be set */
		delay_ms(CONF_WDT_EARLY_WARNING_WAIT_MS);

		/* Check if the Early Warning flag is set */
		test_assert_true(test, wdt_is_early_warning() == true,
				"Early Warning failed \n");

		/* Clear the Early Warning flag */
		wdt_clear_early_warning();
	}
}

/**
 * \brief Wait for Watchdog module to reset
 *
 *  Wait for CONF_WDT_RESET_WAIT_MS for Watchdog to reset the device
 *
 * \param test Current test case.
 */
static void wait_for_wdt_reset(const struct test_case *test)
{
	/* Check if last reset was by Watchdog module */
	if (wdr_flag == false) {
		/* Wait for Watchdog module to reset the device */
		delay_ms(CONF_WDT_RESET_WAIT_MS);
	}
}

/**
 * \brief Test whether last reset was by Watchdog module
 *
 * This function tests whether the last reset was caused by timeout
 * of Watchdog module
 *
 * \param test Current test case.
 */
static void run_reset_cause_test(const struct test_case *test)
{
	test_assert_true(test, wdr_flag == true,
			"Watchdog reset failed: %x", wdr_flag);
}

/**
 * \brief Initialize USART for unit tests
 *
 * Initializes the USART used by the unit test. The USART connected to
 * embedded debugger is used for outputting the results.
 *
 */
static void configure_stdio_serial(void)
{
	struct usart_config usart_conf;
	struct usart_module unit_test_output;

	/* Configure USART for unit test output */
	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&unit_test_output, CONF_STDIO_USART, &usart_conf);
	usart_enable(&unit_test_output);
	/* Enable transceivers */
	usart_enable_transceiver(&unit_test_output, USART_TRANSCEIVER_TX);
	usart_enable_transceiver(&unit_test_output, USART_TRANSCEIVER_RX);
}

/**
 * \brief Run WDT unit tests
 *
 * Initializes the system and serial output, then sets up the
 * WDT unit test suite and runs it.
 */
int main(void)
{
	/* Check whether reset cause was Watchdog */
	wdr_flag = (system_get_reset_cause() & PM_RCAUSE_WDT);
	system_init();

	/* Reset the Watchdog count */
	wdt_reset_count();
	configure_stdio_serial();

	/* Define Test Cases */
	DEFINE_TEST_CASE(wdt_enable_disable_test, NULL,
			run_wdt_enable_disable_test, NULL,
			"WDT Initialization, Enabling/Disabling");

	DEFINE_TEST_CASE(wdt_early_warning_test, NULL,
			run_wdt_early_warning_test, wait_for_wdt_reset,
			"WDT Early Warning Test");

	DEFINE_TEST_CASE(reset_cause_test, NULL,
			run_reset_cause_test, NULL,
			"Confirming Watchdog Reset");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(wdt_tests) = {
			&wdt_enable_disable_test,
			&wdt_early_warning_test,
			&reset_cause_test,
			};

	/* Define the test suite */
	DEFINE_TEST_SUITE(wdt_suite, wdt_tests,
			"SAM D20 WDT driver test suite");

	/* Run all tests in the suite*/
	test_suite_run(&wdt_suite);

	while (1) {
		/* Intentionally left empty */
	}

}
