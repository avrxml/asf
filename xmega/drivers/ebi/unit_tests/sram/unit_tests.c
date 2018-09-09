/**
 * \file
 *
 * \brief Unit tests for External Bus Interface driver with SRAM
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
#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include <sleepmgr.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref ebi_group and
 * \ref ebi_port_group. In essence, it tests that the EBI has been correctly set
 * up by employing test cases for:
 * - data bus faults
 * - address bus faults
 * - data integrity issues
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref common_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 *
 * \section device_info Device Info
 * All AVR XMEGA A1 devices can be used.
 * This example has been tested with the following setup:
 * - STK600 w/ CY7C1019DV-3310VXI (128 kB SRAM)
 *
 * \section description Description of the unit tests
 * See the documentation for the individual test functions \ref ebi_tests_group
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * Relevant module dependencies for this application are:
 * - \ref test_suite_group
 * - \ref ebi_group and \ref ebi_port_group
 * - \ref ebi_tests_group
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

//! \name Test configuration
//@{
/**
 * \def CONF_TEST_RAM_BASE
 * \brief Base address of RAM to test
 */
/**
 * \def CONF_TEST_RAM_SIZE
 * \brief Size of RAM to test
 */
//@}

/**
 * \fn static void ebi_sram_init(void)
 * \brief Initialize the EBI for on-board SRAM
 *
 * This function initializes the EBI according to the parameters specific to the
 * on-board SRAM chip(s). It is defined in \ref conf_test.h.
 */

//! Test setup
static struct ebi_test_params params = {
	.base = (hugemem_ptr_t)CONF_TEST_RAM_BASE,
	.size = (uint32_t)CONF_TEST_RAM_SIZE,
};

/**
 * \brief Run EBI driver unit tests
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
	sleepmgr_init();

	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);
	ebi_sram_init();

	// Define all the test cases
	DEFINE_TEST_CASE(data_bus_test, NULL, run_data_bus_test, NULL,
			"Data bus test");
	DEFINE_TEST_CASE(address_bus_test, NULL, run_address_bus_test, NULL,
			"Address bus test");
	DEFINE_TEST_CASE(data_integrity_test, NULL, run_data_integrity_test,
			NULL, "Data integrity test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(ebi_sram_tests) = {
		&data_bus_test,
		&address_bus_test,
		&data_integrity_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(ebi_sram_suite, ebi_sram_tests,
			"XMEGA EBI driver w/ SRAM test suite");

	// Set up the test data pointer and run all tests in the suite
	test_set_data(&params);
	test_suite_run(&ebi_sram_suite);

	while (1) {
		/* Intentionally left empty. */
	}
}
