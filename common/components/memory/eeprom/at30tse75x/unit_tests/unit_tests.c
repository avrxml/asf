/**
 * \file
 *
 * \brief AT30TS(E)75x Unit Tests.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the AT30TS(E)75x driver.
 * It consists of test cases for the following functionality:
 * - AT30TS(E)75x data write (if has EEPROM)
 * - AT30TS(E)75x data read (if has EEPROM)
 * - AT30TS(E)75x temperature read
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * This example has been tested with the following setup:
 * - sam4n16c_sam4n_xplained_pro with I/O1 XPRO board connected to EXT1
 * - sam4c16c_sam4c_ek
 * - sam4cmp_db
 * - sam4cms_db
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
//@}

#include "asf.h"
#include <stdio.h>
#include <string.h>
#include "conf_test.h"

#define NB_PAGE 16
#define NB_BYTE 16

double temp;
uint8_t rx[NB_BYTE], tx[NB_BYTE];

#if BOARD_USING_AT30TSE != AT30TS75
/**
 * \brief AT30TS(E)75x write EEPROM test.
 *
 * \param test Current test case.
 */
static void run_test_write_data(const struct test_case *test)
{
	memset(tx, 0xFF, NB_BYTE);
	test_assert_true(test,
			at30tse_eeprom_write(tx, NB_BYTE, 0,
			(int)temp % NB_PAGE) == TWI_SUCCESS,
			"Error: AT30TS(E)75x write failed\n\r");
	delay_ms(5);
}

/**
 * \brief AT30TS(E)75x read and comparison EEPROM test.
 *
 * \param test Current test case.
 */
static void run_test_read_compare_data(const struct test_case *test)
{
	memset(rx, 0, NB_BYTE);
	test_assert_true(test,
			at30tse_eeprom_read(rx, NB_BYTE, 0,
			(int)temp % NB_PAGE) == TWI_SUCCESS,
			"Error: AT30TS(E)75x read failed\n\r");
	test_assert_true(test,
			!memcmp(tx, rx, NB_BYTE),
			"Error: AT30TS(E)75x comparison failed\n\r");
}
#endif

/**
 * \brief AT30TS(E)75x read temperature test.
 *
 * \param test Current test case.
 */
static void run_test_read_temperature(const struct test_case *test)
{
	test_assert_true(test,
			(at30tse_read_temperature(&temp) == TWI_SUCCESS),
			"Error: AT30TS(E)75x read temperature failed\n\r");
}

/**
 * \brief Application entry point for AT30TS(E)75x unit tests.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS
	};

	sysclk_init();
	board_init();
	/* Initialize AT30TS(E)75x */
	at30tse_init();

	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define all the test cases */
	DEFINE_TEST_CASE(at30tse_test_read_temperature,
			NULL, run_test_read_temperature, NULL,
			"at30tse read temperature test");
#if BOARD_USING_AT30TSE != AT30TS75
	DEFINE_TEST_CASE(at30tse_test_write_data,
			NULL, run_test_write_data, NULL,
			"at30tse write data test");
	DEFINE_TEST_CASE(at30tse_test_read_data,
			NULL, run_test_read_compare_data, NULL,
			"at30tse read and compare data test");
#endif
	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(at30tse_test_array) = {
			&at30tse_test_read_temperature,
#if BOARD_USING_AT30TSE != AT30TS75
			&at30tse_test_write_data,
			&at30tse_test_read_data,
#endif
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(at30tse_suite, at30tse_test_array,
			"at30tse driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&at30tse_suite);

	while (1) {
		/* Busy-wait forever */
	}
}
