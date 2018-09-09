/**
 * \file
 *
 * \brief Unit tests for MT48LC16m16a2tg7 SDRAM component.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include <conf_test.h>
#include <unit_tests.h>
/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the MT48LC16m16a2tg7 SDRAM component.
 * In essence, it tests that the EBI has been correctly set up by employing test
 * for data integrity issues.
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref common_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 *
 * \section device_info Device Info
 * All AVR UC3 devices can be used.
 * This example has been tested with the following setup:
 * - evk1100
 * - evk1104
 * - evk1105
 * - uc3c_ek
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */

//! \name Debug output configuration
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
//@}

/**
 * \fn static void sdramc_init(void)
 * \brief Initialize the EBI for on-board SDRAM
 *
 * This function initializes the EBI according to the parameters specific to the
 * on-board SDRAM chip(s). It is defined in \ref conf_test.h.
 */
//! Test setup
static struct ebi_test_params params = {
	.base = (volatile unsigned long *)SDRAM,
	.size = (uint32_t)SDRAM_SIZE,
};


/**
 * \brief Run unit tests for MT48LC16m16a2tg7 SDRAM
 * \return 0  which should never occur.
 */
int main (void)
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
	sdramc_init(sysclk_get_cpu_hz());

	// Define all the test cases
	DEFINE_TEST_CASE(data_integrity_test, NULL, run_data_integrity_test,
			NULL, "Data integrity test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(ebi_sdram_tests) = {
		&data_integrity_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(ebi_sdram_suite, ebi_sdram_tests,
			"UC3 EBI driver w/ SDRAM test suite");

	// Set up the test data pointer and run all tests in the suite
	test_set_data(&params);
	test_suite_run(&ebi_sdram_suite);

	while (true);

	return (0);
}
