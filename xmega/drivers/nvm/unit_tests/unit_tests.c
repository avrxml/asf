/**
 * \file
 *
 * \brief Unit tests for Non Volatile Memory controller driver
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
#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <sysclk.h>
#include <nvm.h>
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref nvm_group.
 * It consists of test cases for the following functionality:
 * - device signature readout
 * - EEPROM read/write access
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
 * - Xplain
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions \ref unit_tests.c
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref nvm_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
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
//@}

//! \name Device signature readout unit test macros
//@{
#if defined(__GNUC__) || defined(__DOXYGEN__)
//! Macro for device ID byte 0 in device header file
#  define CHIP_DEVID0 SIGNATURE_0
//! Macro for device ID byte 1 in device header file
#  define CHIP_DEVID1 SIGNATURE_1
//! Macro for device ID byte 2 in device header file
#  define CHIP_DEVID2 SIGNATURE_2
#elif defined(__ICCAVR__)
#  define CHIP_DEVID0 SIGNATURE_000
#  define CHIP_DEVID1 SIGNATURE_001
#  define CHIP_DEVID2 SIGNATURE_002
#else
#  error Unknown compiler
#endif
//@}

//! \name EEPROM read/write access unit test macros
//@{
//! Content of an erased EEPROM byte
#define EEPROM_ERASED             0xff
//! EEPROM page to use in erase test
#define TEST_ERASE_PAGE           2
//! EEPROM page to use in write test
#define TEST_WRITE_PAGE           1
//! EEPROM page to use in atomic write test
#define TEST_ATOMIC_WRITE_PAGE    2
//! EEPROM page to use in split write test
#define TEST_SPLIT_WRITE_PAGE     3
//! EEPROM page 1 of 2 to use in erase byte test
#define TEST_ERASE_BYTES_PAGE_1   4
//! EEPROM page 2 of 2 to use in erase byte test
#define TEST_ERASE_BYTES_PAGE_2   5
//! EEPROM page to use in read buffer test
#define TEST_READ_BUFFER_PAGE     6
//! EEPROM page to use in write buffer test
#define TEST_WRITE_BUFFER_PAGE    7
//@}

//! \name Helper functions and macros
//@{
//! \brief Set all bytes in \a buffer to \a value.
#define set_buffer(buffer, value) memset(buffer, value, sizeof(buffer))
//! \brief Check if EEPROM \a page is equal to memory \a buffer
static bool is_eeprom_page_equal_to_buffer(uint8_t page, uint8_t *buffer)
{
	uint8_t i;

	for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
		if (nvm_eeprom_read_byte(page * EEPROM_PAGE_SIZE + i)
				!= buffer[i]) {
			return false;
		}
	}

	return true;
}
//@}

//! \name Device signature readout unit test functions
//@{
/**
 * \brief Test device ID readout
 *
 * This test reads out the \e DEVIDn registers and compares their value with
 * the expected device ID for the device for which the project was compiled.
 *
 * \param test Current test case.
 */
static void run_device_id_test(const struct test_case *test)
{
	struct nvm_device_id dev_id;

	nvm_read_device_id(&dev_id);

	test_assert_true(test, dev_id.devid0 == CHIP_DEVID0,
			"Read device DEVID0 as 0x%02x, expected 0x%02x",
			dev_id.devid0, CHIP_DEVID0);
	test_assert_true(test, dev_id.devid1 == CHIP_DEVID1,
			"Read device DEVID1 as 0x%02x, expected 0x%02x",
			dev_id.devid1, CHIP_DEVID1);
	test_assert_true(test, dev_id.devid2 == CHIP_DEVID2,
			"Read device DEVID2 as 0x%02x, expected 0x%02x",
			dev_id.devid2, CHIP_DEVID2);
}

/**
 * \brief Test device serial number readout
 *
 * This test reads out the \e LOTNUMn, \e WAFNUM, \e COORDXn and \e COORDYn
 * values from the device signature row and checks that they are not all 0xFF.
 *
 * \param test Current test case.
 */
static void run_device_serial_test(const struct test_case *test)
{
	struct nvm_device_serial dev_sn;
	bool serial_is_set;
	uint8_t byte_index;

	nvm_read_device_serial(&dev_sn);

	serial_is_set = false;
	for (byte_index = 0; byte_index < sizeof(dev_sn.byte); byte_index++) {
		if (dev_sn.byte[byte_index] != 0xff) {
			serial_is_set = true;
			break;
		}
	}
	test_assert_true(test, serial_is_set, "Read device serial as all 0xFF");
}
//@}

//! \name EEPROM read/write access unit test functions
//@{
/**
 * \brief Test EEPROM erase
 *
 * This test writes a single byte to test page \ref TEST_ERASE_PAGE, then erases
 * _all_ pages and checks that the test page contains only \ref EEPROM_ERASED.
 *
 * \param test Current test case.
 */
static void run_eeprom_erase_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	bool    success;

	nvm_eeprom_write_byte(TEST_ERASE_PAGE * EEPROM_PAGE_SIZE, 42);
	nvm_eeprom_erase_all();
	set_buffer(buffer, EEPROM_ERASED);

	success = is_eeprom_page_equal_to_buffer(TEST_ERASE_PAGE, buffer);
	test_assert_true(test, success, "Page was not erased");
}

/**
 * \brief Test EEPROM write
 *
 * This test erases test page \ref TEST_WRITE_PAGE, then writes a single byte to
 * it and verifies that the write succeeded.
 *
 * \param test Current test case.
 */
static void run_eeprom_write_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	bool    success;

	nvm_eeprom_erase_page(TEST_WRITE_PAGE);
	set_buffer(buffer, EEPROM_ERASED);
	buffer[0] = 0xaa;
	nvm_eeprom_write_byte(TEST_WRITE_PAGE * EEPROM_PAGE_SIZE, buffer[0]);

	success = is_eeprom_page_equal_to_buffer(TEST_WRITE_PAGE, buffer);
	test_assert_true(test, success, "Byte write failed");
}

/**
 * \brief Test EEPROM atomic write
 *
 * This test erases test page \ref TEST_ATOMIC_WRITE_PAGE, then writes the first
 * byte in the page buffer before flushing it -- this byte should then not be
 * written to the EEPROM -- and writing different values to the two successive
 * bytes. An atomic write is then executed before the write of the latter two
 * bytes is verified.
 *
 * \param test Current test case.
 */
static void run_eeprom_atomic_write_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	bool    success;

	nvm_eeprom_erase_page(TEST_ATOMIC_WRITE_PAGE);

	/* Load a dummy byte and then flush the buffer to remove the byte from
	 * the buffer. The byte should not be written to the EEPROM.
	 */
	nvm_eeprom_load_byte_to_buffer(0, 55);
	nvm_eeprom_flush_buffer();

	/* Load some additional bytes */
	set_buffer(buffer, EEPROM_ERASED);
	buffer[1] = 0xaa;
	buffer[2] = 0x19;
	nvm_eeprom_load_byte_to_buffer(1, buffer[1]);
	nvm_eeprom_load_byte_to_buffer(2, buffer[2]);
	/* Erase and then write page */
	nvm_eeprom_atomic_write_page(TEST_ATOMIC_WRITE_PAGE);

	success = is_eeprom_page_equal_to_buffer(TEST_ATOMIC_WRITE_PAGE, buffer);
	test_assert_true(test, success,
			"Page buffer flush, byte load or atomic write failed");
}

/**
 * \brief Test EEPROM split write
 *
 * This test writes two bytes to test page \ref TEST_SPLIT_WRITE_PAGE with an
 * atomic write operation, then writes a third byte to the same page with a
 * split write operation before verifying that the page contains all three
 * bytes.
 *
 * \param test Current test case.
 */
static void run_eeprom_split_write_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	bool    success;

	/* Erase and then program two bytes into the page */
	set_buffer(buffer, EEPROM_ERASED);
	buffer[1] = 0xaa;
	buffer[2] = 0x19;
	nvm_eeprom_load_byte_to_buffer(1, buffer[1]);
	nvm_eeprom_load_byte_to_buffer(2, buffer[2]);
	nvm_eeprom_atomic_write_page(TEST_SPLIT_WRITE_PAGE);

	/*
	 * Write another value to the EEPROM and then verify that all three bytes
	 * are present, ie. the split write does not delete the first two bytes.
	 */
	buffer[3] = 0xa7;
	nvm_eeprom_load_byte_to_buffer(3, buffer[3]);
	nvm_eeprom_split_write_page(TEST_SPLIT_WRITE_PAGE);

	success = is_eeprom_page_equal_to_buffer(TEST_SPLIT_WRITE_PAGE, buffer);
	test_assert_true(test, success, "Split write failed");
}

/**
 * \brief Test EEPROM byte erase
 *
 * This test writes two bytes to test pages \ref TEST_ERASE_BYTES_PAGE_1 and
 * \ref TEST_ERASE_BYTES_PAGE_2, then tries to erase one of them in the first
 * page before verifying the erase, then in all pages before verifying that the
 * second page was also erased.
 *
 * \param test Current test case.
 */
static void run_eeprom_erase_byte_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	bool    success;

	set_buffer(buffer, EEPROM_ERASED);
	buffer[0] = 0xaa;
	buffer[1] = 0xaf;

	/* Write two bytes into first page */
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_1 * EEPROM_PAGE_SIZE + 0,
			buffer[0]);
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_1 * EEPROM_PAGE_SIZE + 1,
			buffer[1]);

	/* Write two bytes into second page */
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_2 * EEPROM_PAGE_SIZE + 0,
			buffer[0]);
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_2 * EEPROM_PAGE_SIZE + 1,
			buffer[1]);

	/* Erase first byte of the first page */
	nvm_eeprom_load_byte_to_buffer(0, 0xff);
	nvm_eeprom_erase_bytes_in_page(TEST_ERASE_BYTES_PAGE_1);
	buffer[0] = EEPROM_ERASED;

	success = is_eeprom_page_equal_to_buffer(TEST_ERASE_BYTES_PAGE_1, buffer);
	test_assert_true(test, success, "Byte erase in single page failed");

	/* Erase first byte of all pages */
	nvm_eeprom_load_byte_to_buffer(0, 0xff);
	nvm_eeprom_erase_bytes_in_all_pages();

	success = is_eeprom_page_equal_to_buffer(TEST_ERASE_BYTES_PAGE_2, buffer);
	test_assert_true(test, success, "Byte erase in all pages failed");
}

/**
 * \brief Test EEPROM read buffer
 *
 * This test erases test page \ref TEST_READ_BUFFER_PAGE then writes four bytes
 * to the test page and tries to read the entire page using
 * nvm_eeprom_read_buffer.
 * It then compares the read page with what's actually in the EEPROM page.
 *
 * \param test Current test case.
 */
static void run_eeprom_read_buffer_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	uint8_t buffer_read[EEPROM_PAGE_SIZE];
	bool    success;

	set_buffer(buffer, EEPROM_ERASED);
	buffer[0] = 0x11;
	buffer[1] = 0xaa;
	buffer[2] = 0xee;
	buffer[3] = 0x22;

	// Erase page
	nvm_eeprom_erase_bytes_in_page(TEST_READ_BUFFER_PAGE);

	// Write 4 bytes into page:
	nvm_eeprom_write_byte(TEST_READ_BUFFER_PAGE * EEPROM_PAGE_SIZE + 0,
			buffer[0]);
	nvm_eeprom_write_byte(TEST_READ_BUFFER_PAGE * EEPROM_PAGE_SIZE + 1,
			buffer[1]);
	nvm_eeprom_write_byte(TEST_READ_BUFFER_PAGE * EEPROM_PAGE_SIZE + 2,
			buffer[2]);
	nvm_eeprom_write_byte(TEST_READ_BUFFER_PAGE * EEPROM_PAGE_SIZE + 3,
			buffer[3]);

	nvm_eeprom_read_buffer(TEST_READ_BUFFER_PAGE * EEPROM_PAGE_SIZE,
			&buffer_read, EEPROM_PAGE_SIZE);

	// Check that what we have read is actually what's in EEPROM.
	success = is_eeprom_page_equal_to_buffer(TEST_READ_BUFFER_PAGE,
			buffer_read);

	test_assert_true(test, success, "Read buffer failed");
}

/**
 * \brief Test EEPROM write buffer
 *
 * This test erases test page \ref TEST_WRITE_BUFFER_PAGE then writes a buffer
 * to the test page, and then overwrites this with the another buffer and checks
 * that the last buffer written is what the page is containing.
 *
 * \param test Current test case.
 */
static void run_eeprom_write_buffer_test(const struct test_case *test)
{
	uint8_t buffer[EEPROM_PAGE_SIZE];
	bool    success;

	set_buffer(buffer, EEPROM_ERASED);
	buffer[0] = 0x11;
	buffer[1] = 0xaa;
	buffer[2] = 0xee;
	buffer[3] = 0x22;

	// Erase page
	nvm_eeprom_erase_bytes_in_page(TEST_WRITE_BUFFER_PAGE);
	// Write buffer to EEPROM
	nvm_eeprom_erase_and_write_buffer(TEST_WRITE_BUFFER_PAGE *
			EEPROM_PAGE_SIZE, &buffer, EEPROM_PAGE_SIZE);
	/* Write another buffer to EEPROM to make sure EEPROM is deleted before
	 * written to.
	*/
	buffer[0] = 0x33;
	buffer[1] = 0x11;
	buffer[2] = 0xee;
	buffer[3] = 0xaa;
	nvm_eeprom_erase_and_write_buffer(TEST_WRITE_BUFFER_PAGE *
			EEPROM_PAGE_SIZE, &buffer, EEPROM_PAGE_SIZE);

	// Check that what we have written is in EEPROM.
	success = is_eeprom_page_equal_to_buffer(TEST_WRITE_BUFFER_PAGE, buffer);

	test_assert_true(test, success, "Write buffer failed");
}
//@}

/**
 * \brief Run NVM driver unit tests
 */
int main (void)
{
	const usart_serial_options_t usart_serial_options =
	{
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Define all the test cases
	DEFINE_TEST_CASE(dev_id_test, NULL, run_device_id_test, NULL,
			"Device ID readout");
	DEFINE_TEST_CASE(dev_sn_test, NULL, run_device_serial_test, NULL,
			"Device serial number readout");
	DEFINE_TEST_CASE(eeprom_erase_test, NULL, run_eeprom_erase_test, NULL,
			"EEPROM erase");
	DEFINE_TEST_CASE(eeprom_write_test, NULL, run_eeprom_write_test, NULL,
			"EEPROM write");
	DEFINE_TEST_CASE(eeprom_atomic_write_test, NULL,
			run_eeprom_atomic_write_test, NULL, "EEPROM atomic write");
	DEFINE_TEST_CASE(eeprom_split_write_test, NULL,
			run_eeprom_split_write_test, NULL, "EEPROM split write");
	DEFINE_TEST_CASE(eeprom_erase_byte_test, NULL, run_eeprom_erase_byte_test,
			NULL, "EEPROM erase byte");
	DEFINE_TEST_CASE(eeprom_read_buffer_test, NULL,
			run_eeprom_read_buffer_test, NULL, "EEPROM read buffer");
	DEFINE_TEST_CASE(eeprom_write_buffer_test, NULL,
			run_eeprom_write_buffer_test, NULL, "EEPROM write buffer");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(nvm_tests) = {
		&dev_id_test,
		&dev_sn_test,
		&eeprom_erase_test,
		&eeprom_write_test,
		&eeprom_atomic_write_test,
		&eeprom_split_write_test,
		&eeprom_erase_byte_test,
		&eeprom_read_buffer_test,
		&eeprom_write_buffer_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(nvm_suite, nvm_tests, "XMEGA NVM driver test suite");

	// Run all tests in the test suite
	test_suite_run(&nvm_suite);

	while (1);
}
