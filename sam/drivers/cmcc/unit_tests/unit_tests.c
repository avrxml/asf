/**
 * \file
 *
 * \brief Unit tests for CMCC driver.
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
 * This is the unit test application for the CMCC driver.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_uart_serial.h
 *
 * \section device_info Device Info
 * All SAM devices with an CMCC module can be used.
 * This example has been tested with the following setup:
 * - sam4e16e_sam4e_ek
 * - sam4c16c_sam4c_ek
 * - sam4cmp16c_sam4cmp_db
 * - sam4cms16c_sam4cms_db
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for ARM. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */

/** Define CMCC Base */
#if SAM4C || SAM4CM
#define CMCC_BASE    CMCC0
#else
#define CMCC_BASE    CMCC
#endif

#define FIBONACCI_NUM    30

/** CMCC configuration */
struct cmcc_config g_cmcc_cfg;

volatile bool flag = false;

/**
 *  \brief Recursively calculate the nth Fibonacci number.
 *
 * \param n Indicates which (positive) Fibonacci number to compute.
 *
 * \return The nth Fibonacci number.
 */
static uint32_t recfibo(uint32_t n)
{
	if (n == 0 || n == 1) {
		return n;
	}

	return recfibo(n - 2) + recfibo(n - 1);
}

/**
 * \brief Test Cache Data Hit Counter Mode.
 *
 * \param Null.
 */
static void run_cache_data_hit_test(const struct test_case *test)
{
	recfibo(FIBONACCI_NUM);
	if (0 == cmcc_get_monitor_cnt(CMCC_BASE)) {
		flag = false;
	} else {
		flag = true;
	}

	test_assert_true(test, flag == true, "CMCC mode monitor not work!");
}

/**
 * \brief Run CMCC driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate = CONF_TEST_BAUDRATE,
		.paritytype = CONF_TEST_PARITY,
	};

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Enable the CMCC module. */
	cmcc_get_config_defaults(&g_cmcc_cfg);
	cmcc_init(CMCC_BASE, &g_cmcc_cfg);
	cmcc_enable(CMCC_BASE);

	/* Define all the test cases. */
	DEFINE_TEST_CASE(dhit_mode_test, NULL, run_cache_data_hit_test, NULL,
			"SAM CMCC Data Hit Mode test.");

	/* Put test case addresses in an array. */
	DEFINE_TEST_ARRAY(cmcc_tests) = {
		&dhit_mode_test,
	};

	/* Define the test suite. */
	DEFINE_TEST_SUITE(cmcc_suite, cmcc_tests, "SAM CMCC driver test suite");

	/* Run all tests in the test suite. */
	test_suite_run(&cmcc_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
