/**
 * \file
 *
 * \brief Unit tests for FREQM driver.
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
 * This is the unit test application for the FREQM driver.
 * It consists of test cases for the following functionality:
 * - Enable FREQM and start measurement, when measurement done will generate interrupt
 *
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 * - \ref conf_sleepmgr.h
 *
 * \section device_info Device Info
 * The SAM4L devices can be used.
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
 * \def CONF_TEST_FREQM
 * \brief Test the functions provided by the FREQM driver.
 */
//@}

volatile uint8_t intflag = 0;

/** FREQM instance */
struct freqm_dev_inst g_freqm_inst;

/** FREQM configuration */
struct freqm_config  g_freqm_cfg;

/**
 * \brief Interrupt handler for FREQM interrupt.
 */
static void set_int_flag(void)
{
	if (freqm_get_interrupt_status(&g_freqm_inst) & FREQM_ISR_DONE) {
		freqm_disable_interrupt(&g_freqm_inst,
				FREQM_INTERRUPT_MEASURMENT_READY);
		intflag = 1;
	}
}

/**
 * \brief Test FREQM measurement done.
 *
 * \param test Current test case.
 */
static void run_freqm_test(const struct test_case *test)
{
	/* Measure default clock. */
	freqm_get_config_defaults(&g_freqm_cfg);
	freqm_init(&g_freqm_inst, FREQM, &g_freqm_cfg);
	freqm_set_callback(&g_freqm_inst, FREQM_INTERRUPT_MEASURMENT_READY,
			set_int_flag, 1);
	freqm_start_measure(&g_freqm_inst);

	/* Delay more than the measurement time to wait for interrupt. */
	delay_ms(20);

	test_assert_true(test, intflag == 1, "FREQM test failed");
}

/**
 * \brief Run FREQM driver unit tests.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS
	};

	/* Initialize the system clock and board */
	sysclk_init();
	board_init();

	/* Enable the debug uart */
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

#if defined(__GNUC__)
	setbuf(stdout, NULL);
#endif

	/* Define all the test cases */
	DEFINE_TEST_CASE(freqm_test, NULL, run_freqm_test, NULL,
			"FREQM test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(freqm_tests) = {
		&freqm_test
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(freqm_suite, freqm_tests,
			"SAM FREQM driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&freqm_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
