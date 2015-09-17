/**
 * \file
 *
 * \brief Unit tests for Cyclic Redundancy Check (CRC) driver
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdio.h>
#include <stdbool.h>
#include <asf.h>
#include <sysclk.h>
#include <board.h>
#include <dma.h>
#include <unit_test/suite.h>
#include <crc.h>
#include <sleepmgr.h>
#include "conf_test.h"

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref crc_group
 * It consists of test cases for the following functionality:

 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_sleepmgr.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used.
 * This example has been tested with the following setup:
 * - XMEGA A3U STK600
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions
 * \ref unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref crc_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

//! \brief Get array length
#define LENGTH(x) (sizeof(x) / sizeof(*x))

//! \name Data declarations
//@{

const uint8_t data_8bit[] = {
		0xF0, 0x0F, 0xAB, 0x12, 0x21, 0x01, 0x10, 0x20,
		0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90
};

PROGMEM_DECLARE(uint8_t, data_8bit_flash[]) = {
		0xF0, 0x0F, 0xAB, 0x12, 0x21, 0x01, 0x10, 0x20,
		0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90
};

/**
 * \brief Pre-calculated checksums of data_8bit
 * and data_8bit_flash (without the trailing zeros)
 */
#define CRC_CHECKSUM_32BIT	0x26CB2533
#define CRC_CHECKSUM_16BIT	0x51D1

//@}

//! \name Helper functions
//@{

/**
 * \brief Setup a DMA channel for CRC calculation
 *
 * \param buf_length    Length of buffer to transfer
 * \param src_buffer    Pointer to transfer buffer
 * \param dst_buffer    Pointer to receive buffer
 */
static void setup_dma_channel(uint8_t buf_length,
		uint8_t *src_buffer, uint8_t *dst_buffer)
{
	struct dma_channel_config config;

	dma_enable();

	dma_channel_write_source(CONF_TEST_DMACH, (uint16_t)src_buffer);
	dma_channel_write_destination(CONF_TEST_DMACH, (uint16_t)dst_buffer);
	dma_channel_write_burst_length(CONF_TEST_DMACH, 1);
	dma_channel_write_transfer_count(CONF_TEST_DMACH, buf_length);
	dma_channel_write_repeats(CONF_TEST_DMACH, 1);

	dma_channel_read_config(CONF_TEST_DMACH, &config);
	dma_channel_set_src_dir_mode(&config, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&config, DMA_CH_DESTDIR_INC_gc);
	dma_channel_write_config(CONF_TEST_DMACH, &config);

	dma_channel_enable(CONF_TEST_DMACH);
}

/**
 * \brief Wait for DMA transfer to complete and check status
 *
 * \param test    Current test case
 *
 * \retval true if DMA transfer completed without error
 * \retval false if DMA transfer failed
 */
static bool wait_for_dma_transfer(const struct test_case *test)
{
	enum dma_channel_status status;

	while (1) {
		status = dma_get_channel_status(CONF_TEST_DMACH);

		if(status == DMA_CH_TRANSFER_COMPLETED) {
			return true;
		} else if(status == DMA_CH_FREE) {
			test_assert_true(test, false,
					"DMA channel did not trigger while doing CRC");
			return false;
		}
		else if(status == DMA_CH_TRANSFER_ERROR) {
			test_assert_true(test, false, "DMA Transfer error doing CRC");
			return false;
		}
	}
}

//@}

//! \name Unit test functions
//@{

/**
 * \brief 32-bit checksum of data block test
 *
 * This test does a checksum of a data block with a known checksum,
 * and verifies that they are equal. It then appends the 32-bit CRC
 * to the end of the data, and does another checksum operation, which
 * should result in the zero flag being set.
 *
 * \param test    Current test case
 */
static void run_32bit_io_test(const struct test_case *test)
{
	uint8_t tmp_buffer[LENGTH(data_8bit) + sizeof(uint32_t)];
	int i;
	uint32_t checksum;

	crc_io_checksum_byte_start(CRC_32BIT);

	for(i = 0; i < LENGTH(data_8bit); ++i)
		crc_io_checksum_byte_add(data_8bit[i]);

	checksum = crc_io_checksum_byte_stop();

	test_assert_true(test, checksum == CRC_CHECKSUM_32BIT,
			"Checksum mismatch on IO CRC-32 test");

	memcpy(tmp_buffer, data_8bit, LENGTH(data_8bit));
	crc32_append_value(checksum,
			&tmp_buffer[LENGTH(tmp_buffer) - sizeof(uint32_t)]);

	checksum = crc_io_checksum(tmp_buffer,LENGTH(tmp_buffer), CRC_32BIT);

	test_assert_true(test, checksum == 0,
			"Checksum fail check failed on IO CRC-32 test");
}

/**
 * \brief 16-bit checksum of data block test
 *
 * This test does a checksum of a data block with a known checksum,
 * and verifies that they are equal. It then appends the 16-bit CRC
 * to the end of the data, and does another checksum operation, which
 * should result in the zero flag being set.
 *
 * \param test    Current test case
 */
static void run_16bit_io_test(const struct test_case *test)
{
	uint8_t tmp_buffer[LENGTH(data_8bit) + sizeof(uint16_t)];
	int i;
	uint16_t checksum;

	crc_io_checksum_byte_start(CRC_16BIT);

	for(i = 0;i < LENGTH(data_8bit); ++i)
		crc_io_checksum_byte_add(data_8bit[i]);

	checksum = crc_io_checksum_byte_stop();

	test_assert_true(test, checksum == CRC_CHECKSUM_16BIT,
			"Checksum mismatch on IO CRC-16 test");

	memcpy(tmp_buffer, data_8bit, LENGTH(data_8bit));
	crc16_append_value(checksum,
			&tmp_buffer[LENGTH(tmp_buffer) - sizeof(uint16_t)]);

	checksum = crc_io_checksum(tmp_buffer, LENGTH(tmp_buffer), CRC_16BIT);

	test_assert_true(test, checksum == 0,
			"Checksum fail check failed on IO CRC-16 test");
}

/**
 * \brief 32-bit checksum of DMA transfer data
 *
 * This test sets up DMA to do a data block transfer, and sets up
 * the CRC module to do a 32 bit checksum on the data. The checksum
 * will then be added to another buffer, the same procedure is setup
 * with this new buffer which should result in a checksum = 0.
 *
 * \param test    Current test case
 */
static void run_32bit_dma_test(const struct test_case *test)
{
	uint32_t checksum;
	bool success;
	uint8_t data_buf_8bit[LENGTH(data_8bit) + sizeof(uint32_t)];
	uint8_t data_8bit_cpy[LENGTH(data_8bit) + sizeof(uint32_t)];

	setup_dma_channel(LENGTH(data_8bit),
			(uint8_t *)data_8bit, data_buf_8bit);
	crc_dma_checksum_start(CONF_TEST_DMACH, CRC_32BIT);
	dma_channel_trigger_block_transfer(CONF_TEST_DMACH);

	success = wait_for_dma_transfer(test);

	dma_channel_disable(CONF_TEST_DMACH);
	dma_disable();

	checksum = crc_dma_checksum_stop();

	if (!success) {
		return;
	}

	test_assert_true(test, checksum == CRC_CHECKSUM_32BIT,
			"Checksum mismatch on DMA CRC-32 test");

	memcpy(data_8bit_cpy, data_8bit, LENGTH(data_8bit));
	crc32_append_value(checksum,
			&data_8bit_cpy[LENGTH(data_8bit_cpy) - sizeof(uint32_t)]);

	setup_dma_channel(LENGTH(data_8bit_cpy),
			(uint8_t *)data_8bit_cpy, data_buf_8bit);

	crc_dma_checksum_start(CONF_TEST_DMACH, CRC_32BIT);
	dma_channel_trigger_block_transfer(CONF_TEST_DMACH);

	success = wait_for_dma_transfer(test);

	dma_channel_disable(CONF_TEST_DMACH);
	dma_disable();

	checksum = crc_dma_checksum_stop();

	if (!success) {
		return;
	}

	test_assert_true(test, checksum == 0,
			"Checksum fail check failed on DMA CRC-16 test");
}

/**
 * \brief 16-bit checksum of DMA transfer data
 *
 * This test sets up DMA to do a data block transfer, and sets up
 * the CRC module to do a 16 bit checksum on the data. The checksum
 * will then be added to another buffer, the same procedure is setup
 * with this new buffer which should result in a checksum = 0.
 *
 * \param test    Current test case
 */
static void run_16bit_dma_test(const struct test_case *test)
{
	uint16_t checksum;
	bool success;
	uint8_t data_buf_8bit[LENGTH(data_8bit) + sizeof(uint16_t)];
	uint8_t data_8bit_cpy[LENGTH(data_8bit) + sizeof(uint16_t)];

	setup_dma_channel(LENGTH(data_8bit), (uint8_t*)data_8bit, data_buf_8bit);
	crc_dma_checksum_start(CONF_TEST_DMACH, CRC_16BIT);
	dma_channel_trigger_block_transfer(CONF_TEST_DMACH);

	success = wait_for_dma_transfer(test);

	dma_channel_disable(CONF_TEST_DMACH);
	dma_disable();

	checksum = crc_dma_checksum_stop();

	if (!success) {
		return;
	}

	test_assert_true(test, checksum == CRC_CHECKSUM_16BIT,
			"Checksum mismatch on DMA CRC-16 test");

	memcpy(data_8bit_cpy, data_8bit, LENGTH(data_8bit));
	crc16_append_value(checksum,
			&data_8bit_cpy[LENGTH(data_8bit_cpy) - sizeof(uint16_t)]);

	setup_dma_channel(LENGTH(data_8bit_cpy),
			(uint8_t *)data_8bit_cpy, data_buf_8bit);

	crc_dma_checksum_start(CONF_TEST_DMACH, CRC_16BIT);
	dma_channel_trigger_block_transfer(CONF_TEST_DMACH);

	success = wait_for_dma_transfer(test);

	dma_channel_disable(CONF_TEST_DMACH);
	dma_disable();

	checksum = crc_dma_checksum_stop();

	if (!success) {
		return;
	}

	test_assert_true(test, checksum == 0,
			"Checksum fail check failed on DMA CRC-16 test");
}

/**
 * \brief 32-bit CRC of flash memory test
 *
 * This test does a 32-bit checksum on the flash memory and verifies the
 * checksum against a pre-calculated 32-bit CRC checksum.
 *
 * \param test    Current test case
 */
static void run_flash_test(const struct test_case *test)
{
	uint32_t checksum;

	checksum = crc_flash_checksum(CRC_FLASH_RANGE,
			(flash_addr_t)data_8bit_flash, LENGTH(data_8bit_flash));
	test_assert_true(test, checksum == CRC_CHECKSUM_32BIT,
			"Checksum mismatch on flash range CRC-32 test");
}

//@}

/**
 * \brief Run CRC driver unit tests
 */
int main (void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	sleepmgr_init();

	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	DEFINE_TEST_CASE(crc_32bit_io_test, NULL,
		run_32bit_io_test,
		NULL, "32bit CRC on simulated IO data test");

	DEFINE_TEST_CASE(crc_16bit_io_test, NULL,
		run_16bit_io_test,
		NULL, "16bit CRC on simulated IO data test");

	DEFINE_TEST_CASE(crc_32bit_dma_test, NULL, run_32bit_dma_test,
		NULL, "32bit CRC DMA data test");

	DEFINE_TEST_CASE(crc_16bit_dma_test, NULL, run_16bit_dma_test,
		NULL, "16bit CRC DMA data test");

	DEFINE_TEST_CASE(crc_32bit_flash_range_test, NULL, run_flash_test,
		NULL, "32bit CRC flash range test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(crc_tests) = {
		&crc_32bit_io_test,
		&crc_16bit_io_test,
		&crc_32bit_dma_test,
		&crc_16bit_dma_test,
		&crc_32bit_flash_range_test,
	};

	// Define the test suite
	DEFINE_TEST_SUITE(crc_suite, crc_tests, "XMEGA CRC driver test suite");

	test_suite_run(&crc_suite);

	while (1) {
		// Intentionally left blank
	}
}
