/**
 * \file
 *
 * \brief Unit Tests of the SD/MMC SPI Component.
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

/*! \mainpage
 * \section intro Introduction
 * This is the unit test application for the \ref sd_mmc_spi.h
 * "SD/MMC SPI component".
 * It consists of test cases for the following:
 * - Check presence
 * - Memory check
 * - Read/write sector access and data integrity
 * - Read/write multiple sector access and data integrity
 * \warning Running this application might damage the data stored in the SD/MMC card.
 *
 * \section files Main Files
 * - \ref unit_tests.c: SD/MMC SPI unit tests application.
 * - \ref sd_mmc_spi.c: The SD/MMC SPI driver
 * - \ref sd_mmc_spi.h: The SD/MMC SPI driver header
 * - \ref conf_test.h: test configuration file
 * - \ref conf_board.h: board initialization process configuration
 * - \ref conf_clock.h: clock specific initialization
 * - \ref conf_sd_mmc_spi.h: SD/MMC SPI driver configuration
 * - \ref conf_usart_serial.h: Serial port configuration (\see conf_test.h)
 * - \ref conf_access.h: Used for compatibility only.
 *
 * \section deviceinfo Device Info
 * All 32-bit AVR devices can be used. This application has been tested
 * with the following setup:
 *   - Evaluation kits with a SD/MMC connected to an SPI interface.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for 32-bit AVRs.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include "sd_mmc_spi.h"
#include "conf_test.h"

/**
 * \brief A SD/MMC sector-sized buffer for processing data in ram memory
 */
static uint8_t sector_buf[MMC_SECTOR_SIZE];

/**
 * \brief Index of the current sector being processed.
 * Used by \ref run_multiple_sector_access_test
 */
static uint32_t cur_sector = 0;

/**
 * \brief Test case pointer used to refer to the current test case in the
 * function \ref sd_mmc_spi_read_multiple_sector_callback
 */
static struct test_case *cur_test;

/**
 * \name Unit test configuration
 * \def CONF_TEST_NB_SECTORS
 * \brief Defines the number of sectors to be read during the \ref run_multiple_sector_access_test test
 */

/**
 * \name Debug output configuration
 * \{
 */
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
/**
 * \}
 */

/**
 * \name Byte pattern generator
 * \{
 */
/**
 * \brief Generates byte pattern 1 for a given address
 */
#define BYTE_PATTERN1(x)                                                       \
	((uint8_t) (((x) * 5 + 1) & 0xff))
/**
 * \brief Generates byte pattern 2 for a given address
 */
#define BYTE_PATTERN2(x)                                                       \
	((uint8_t) (((x) * 3) & 0xff))
/**
 * \}
 */

/**
 * \brief Checks the presence of the SD/MMC card
 *
 * This function will simply test the output of the function
 * \ref sd_mmc_spi_check_presence and returns an error in case of failure.
 *
 * \param test Current test case.
 */
static void run_check_presence_test(const struct test_case *test)
{
	bool status;

	status = sd_mmc_spi_check_presence();
	test_assert_true(test, status == true, "SD/MMC card not present");
}

/**
 * \brief Performs a memory initialization on the SD/MMC card
 *
 * This function will simply test the output of the function
 * \ref sd_mmc_spi_mem_check and returns an error in case of failure.
 *
 * \param test Current test case.
 */
static void run_memory_check_test(const struct test_case *test)
{
	bool status;

	status = sd_mmc_spi_mem_check();
	test_assert_true(test, status == true, "Error memory initialization failed");
}

/**
 * \brief Test the read and write sector operations on the SD/MMC card
 *
 * This function will test the read and write functionalities of the SD/MMC SPI
 * component using sector access. It will first fill the last sector of the
 * device with a known pattern and read it back by testing each value read.
 *
 * \param test Current test case.
 */
static void run_sector_access_test(const struct test_case *test)
{
	uint32_t i;
	bool status;

	/* Fills a ram sector with a known pattern
	 */
	for (i=0; i<MMC_SECTOR_SIZE; i++) {
		sector_buf[i] = BYTE_PATTERN1(i);
	}

	/* Compute the SD card capacity to retrieve the last sector address
	 */
	sd_mmc_spi_get_capacity();

	/* Write the ram sector to the SD/MMC card
	 */
	status = sd_mmc_spi_write_open(sd_mmc_spi_last_block_address);
	test_assert_true(test, status == true,
			"Cannot open the SD/MMC memory for write access");
	status = sd_mmc_spi_write_sector_from_ram(sector_buf);
	test_assert_true(test, status == true,
			"Write sector operation error");
	sd_mmc_spi_write_close();

	/* Clear the ram sector buffer
	 */
	for (i=0; i<MMC_SECTOR_SIZE; i++) {
		sector_buf[i] = 0;
	}

	/* Read back the sector from the SD/MMC card
	 */
	status = sd_mmc_spi_read_open(sd_mmc_spi_last_block_address);
	test_assert_true(test, status == true,
			"Cannot open the SD/MMC memory for read access");
	status = sd_mmc_spi_read_sector_to_ram(sector_buf);
	test_assert_true(test, status == true,
			"Read sector operation error");
	sd_mmc_spi_read_close();

	/* Compare the values read from the SD/MMC card with the expected values
	 */
	for (i=0; i<MMC_SECTOR_SIZE; i++) {
		test_assert_true(test, sector_buf[i] == BYTE_PATTERN1(i),
				"Value not expected @ 0x%08x (read: 0x%02x,"
				" expected: 0x%02x)", i, sector_buf[i],
				BYTE_PATTERN1(i));
	}
}

/**
 * \brief Test the read and write multiple sector operations on the SD/MMC card
 *
 * This function will test the read and write functionalities of the SD/MMC SPI
 * component using multiple sector access. It will first fill the \ref CONF_TEST_NB_SECTORS
 * last sectors with a known pattern and read it back to test each value.
 *
 * \param test Current test case.
 */
static void run_multiple_sector_access_test(const struct test_case *test)
{
	bool status;

	/* Compute the SD card capacity to retrieve the last sector address
	 */
	sd_mmc_spi_get_capacity();

	/* Write the ram sector to the SD/MMC card
	 */
	status = sd_mmc_spi_write_open(sd_mmc_spi_last_block_address - CONF_TEST_NB_SECTORS);
	test_assert_true(test, status == true,
			"Cannot open the SD/MMC memory for write access");
	cur_sector = 0;
	status = sd_mmc_spi_write_multiple_sector(CONF_TEST_NB_SECTORS);
	test_assert_true(test, status == true,
			"Error while writing sector # %i", cur_sector);
	sd_mmc_spi_write_close();

	/* Read back the sector from the SD/MMC card
	 */
	status = sd_mmc_spi_read_open(sd_mmc_spi_last_block_address - CONF_TEST_NB_SECTORS);
	test_assert_true(test, status == true,
			"Cannot open the SD/MMC memory for read access");
	cur_sector = 0;
	cur_test = (struct test_case *) test;
	status = sd_mmc_spi_read_multiple_sector(CONF_TEST_NB_SECTORS);
	test_assert_true(test, status == true,
			"Error while reading sector # %i", cur_sector);
	sd_mmc_spi_read_close();
}

/**
 * \copybrief sd_mmc_spi_write_multiple_sector_callback
 * \note Used by \ref run_multiple_sector_access_test
 */
void sd_mmc_spi_write_multiple_sector_callback(void *psector)
{
	uint32_t i;
	uint8_t value;

	/* This reflect the index of the current sector being accessed
	 */
	cur_sector++;

	/* Fills a ram sector with a known pattern
	 */
	for (i=0; i<MMC_SECTOR_SIZE; i++) {
		value = BYTE_PATTERN2(cur_sector * MMC_SECTOR_SIZE + i);
		((uint8_t *) psector)[i] = value;
	}
}

/**
 *  \copybrief sd_mmc_spi_read_multiple_sector_callback
 * \note Used by \ref run_multiple_sector_access_test
 */
void sd_mmc_spi_read_multiple_sector_callback(const void *psector)
{
	uint32_t i;
	uint8_t expected;

	/* This reflect the index of the current sector being accessed
	 */
	cur_sector++;

	/* Test each values of the current sector
	 */
	for (i=0; i<MMC_SECTOR_SIZE; i++) {
		expected = BYTE_PATTERN2(cur_sector * MMC_SECTOR_SIZE + i);
		test_assert_true(cur_test, ((uint8_t *) psector)[i] == expected,
				"Value not expected @ 0x%08x in sector # %i"
				" (read: 0x%02x, expected: 0x%02x)", i,
				cur_sector, ((uint8_t *) psector)[i],
				expected);
	}
}

/**
 * \brief Run SD/MMC SPI component unit tests
 *
 * Initializes the clock system, board, serial output and SD/MMC card, then sets
 * up the SD/MMC SPI unit test suite and runs it.
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	// Initialize the board and all the peripheral required
	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Initialize SD/MMC driver resources: SPI and SD/MMC.
	sd_mmc_spi_board_init();

	// Define all the test cases
	DEFINE_TEST_CASE(check_presence_test, NULL, run_check_presence_test,
			NULL, "Check presence");
	DEFINE_TEST_CASE(memory_check_test, NULL, run_memory_check_test, NULL,
			"Memory check");
	DEFINE_TEST_CASE(sector_access_test, NULL, run_sector_access_test, NULL,
			"Read/write sector access");
	DEFINE_TEST_CASE(multiple_sector_access_test, NULL,
			run_multiple_sector_access_test, NULL,
			"Read/write multiple sector access");

	// Put test case addresses in an array.
	DEFINE_TEST_ARRAY(memory_tests) = {
		&check_presence_test,
		&memory_check_test,
		&sector_access_test,
		&multiple_sector_access_test
	};

	// Define the test suite.
	DEFINE_TEST_SUITE(memory_suite, memory_tests,
			"SD/MMC SPI component unit tests suite");

	// Run all tests in the test suite.
	test_suite_run(&memory_suite);

	while (1) {
		// Intentionally left empty.
	};
}
