/**
 * \file
 *
 * \brief Unit tests for Enhanced Direct Memory Access controller driver
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
#include <edma.h>
#include <pmic.h>
#include <tc45.h>
#include <sleepmgr.h>
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>
#include <conf_board.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref edma_group.
 * It consists of test cases for the following functionality:
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with a EDMA controller can be used. This example have
 * been tested on the following board(s):
 * - XMEGA-E5-XPLAINED
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions \ref
 * unit_tests.c
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref edma_group
 * - \ref tc_group
 * - \ref pmic_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

/** \name Unit test configuration */
/** @{ */

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
 * \brief Stop bit configuration of USART
 */

/**
 * \def MEMORY_BLOCK_SIZE
 * Size of memory blocks used in copy tests
 */

/**
 * \def DEST_BLOCK_TC45_SIZE
 * Size of destination block used in trigger/callback test
 */

/**
 * \def TIMER45
 * Timer used in EDMA fixed read/trigger/callback test
 */

/**
 * \def TIMER45_PERIOD
 * Period of timer
 */

/**
 * \def TIMER45_RESOLUTION
 * Resolution of timer
 */

/** @} */

/** \brief Texts often used in "test_assert_true()" function */
uint8_t text_s[] = "StdCH_";
uint8_t text_p[] = "PerCH_";
uint8_t text_1[] = "does not match configuration";
uint8_t text_2[] = "not set correctly";
uint8_t text_3[] = "not unset correctly";

/** \name Variables */
/** @{ */
/** \brief Memory block to read from */
uint8_t memory_block_src[MEMORY_BLOCK_SIZE];
/** \brief Memory block to write to */
uint8_t memory_block_dest[MEMORY_BLOCK_SIZE];

/**
 * \brief Memory block to write to, used by
 * run_edma_triggered_with_callback
 */
uint8_t dest_block_tc45[DEST_BLOCK_TC45_SIZE];

/** \brief Expected result from trigger/interrupt copy */
uint8_t expected_result_tc[DEST_BLOCK_TC45_SIZE] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

/**
 * \brief Whether EDMA has finished copying,
 * used by run_edma_triggered_with_callback
 */
volatile bool edma_has_completed = false;

/** \brief Counter incremented by timer overflow */
volatile uint8_t timer45_overflow_counter = 0;

/** @} */

/** \name Helper functions */
/** @{ */

/** \brief Set all bytes in \a buffer to \a value. */
#define set_buffer(buffer, value) memset(buffer, value, sizeof(buffer))

/**
 * \brief Fill a block with a certain pattern
 *
 *
 * \param block_1    Pointer to block.
 * \param block_size Size of block to fill.
 *
 */
static void block_fill(uint8_t *block, uint16_t size)
{
	uint16_t i;

	for (i = 0; i < size; i++) {
		block[i] = (uint8_t)(i ^ 0xff00);
	}
}

/**
 * \brief Compare two 8-bit data blocks of given size
 *
 * \note Compares two blocks of equal size to see if they are equal.
 *
 * \param block_1    Pointer to first block.
 * \param block_2    Pointer to second block.
 * \param block_size Size of blocks to compare.
 *
 */
static bool block_compare
	(const uint8_t *block_1, const uint8_t *block_2, uint16_t block_size)
{
	uint16_t byte_index;

	for (byte_index = 0; byte_index < block_size; byte_index++) {
		if (block_1[byte_index] != block_2[byte_index]) {
			return false;
		}
	}

	return true;
}

/**
 * \brief Trigger and wait for a successful transfer
 *
 * \param channel_num             Channel number
 */
static void ut_edma_transfer_block(edma_channel_num_t channel_num)
{
	/* Trigger a EDMA copy */
	edma_channel_trigger_block_transfer(channel_num);

	/* Wait for it to finish */
	while (edma_get_channel_status(channel_num) !=
			EDMA_CH_TRANSFER_COMPLETED) {
		/* Intentionally left empty */
	}
}

/** @} */

/** \name Callback functions */
/** @{ */

/**
 * \brief EDMA transfer complete callback
 *
 * \param status             Status of transfer
 */
static void ut_edma_transfer_is_complete(edma_channel_status_t status)
{
	if (status == EDMA_CH_TRANSFER_COMPLETED) {
		/* The transfer is complete, tell the waiting test function */
		edma_has_completed = true;
		/* Disable the timer */
		tc45_disable(&TIMER45);
	}
}

/**
 *
 * \brief Timer overflow callback
 *
 * \note The callback increments the overflow counter
 */
static void timer45_overflow_callback(void)
{
	timer45_overflow_counter++;
	TIMER45.INTFLAGS = TC4_OVFIF_bm;
}

/** @} */

/** \name EDMA module test functions */
/** @{ */

/**
 * \brief Test the different burst lengths by copying on all channels
 *
 * \note This test copies a memory block using different burst lengths,
 * and verifies that they have been copied correctly, thereby testing both
 * burst lengths and memory transfer.
 *
 * \param test              Current test
 */
static void run_edma_memory_copy_burst_length_test(const struct test_case *test)
{
	struct edma_channel_config config_params;
	edma_channel_num_t channel_index;
	bool success = true; /* Assume everything goes well */

	/* Fill the source block with our known pattern */
	set_buffer(memory_block_src, 0x00);
	block_fill(memory_block_src, MEMORY_BLOCK_SIZE);

	/* Null out the config params */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable EDMA, channels 0 and 2 in standard conf. */
	edma_enable(EDMA_CHMODE_STD02_gc);

	/*
	 * - No reload on source and destination
	 * - Increment source and destination
	 */
	edma_channel_set_src_reload_mode(&config_params,
			EDMA_CH_RELOAD_NONE_gc);
	edma_channel_set_src_dir_mode(&config_params, EDMA_CH_DIR_INC_gc);
	edma_channel_set_dest_reload_mode(&config_params,
			EDMA_CH_RELOAD_NONE_gc);
	edma_channel_set_dest_dir_mode(&config_params, EDMA_CH_DESTDIR_INC_gc);
	edma_channel_set_transfer_count16(&config_params, MEMORY_BLOCK_SIZE);
	edma_channel_set_source_address(&config_params,
			(uint16_t)(uintptr_t)memory_block_src);
	edma_channel_set_destination_address(&config_params,
			(uint16_t)(uintptr_t)memory_block_dest);

	/* Test burst lengths on all standard channels */
	for (channel_index = EDMA_CH_0;
			channel_index < EDMA_NUMBER_OF_STANDARD_CHANNELS;
			channel_index += 2) {
		edma_channel_set_burst_length(&config_params,
				EDMA_CH_BURSTLEN_1BYTE_gc);
		edma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		edma_channel_enable(channel_index);
		ut_edma_transfer_block(channel_index);
		edma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src, memory_block_dest,
				MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write 2 byte burst length */
		edma_channel_reset(channel_index);
		edma_channel_set_burst_length(&config_params,
				EDMA_CH_BURSTLEN_2BYTE_gc);
		edma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		edma_channel_enable(channel_index);
		ut_edma_transfer_block(channel_index);
		edma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src, memory_block_dest,
				MEMORY_BLOCK_SIZE);

		/* test_assert_true(test, false, "Test-1 "); */

		if (!success) {
			break;
		}
	}
	/* Disable EDMA */
	edma_disable();

	test_assert_true(test,
			success,
			"EDMA burst mode memory copy tests failed on StdCH_%d",
			channel_index);
}

/**
 *
 * \brief Set EDMA configuration for standard channels, and read it back
 *
 * \note This function writes a configuration to the EDMA controller, and reads
 * it back to verify settings have been correctly set.
 *
 * \param test   Current test case
 */
static void run_edma_standard_config_interface_test(const struct test_case *test)
{
	struct edma_channel_config config_params;
	struct edma_channel_config read_config;

	const uint16_t transfer_count16 = 0x5A69;
	const uint16_t dest_addr = 0xBEEF;
	const uint16_t src_addr = 0xDEAD;
	const bool repeat_mode = true;

	edma_channel_num_t channel_index;
	uint8_t channel_status;

	/* Null out the config params */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable EDMA, channels 0 and 2 in standard conf. */
	edma_enable(EDMA_CHMODE_STD02_gc);

	/* Apply some parameters */
	edma_channel_set_burst_length(&config_params,
			EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_interrupt_level(&config_params, EDMA_INT_LVL_HI);
	edma_channel_set_src_reload_mode(&config_params,
			EDMA_CH_RELOAD_BLOCK_gc);
	edma_channel_set_dest_reload_mode(&config_params,
			EDMA_CH_RELOAD_BURST_gc);
	edma_channel_set_src_dir_mode(&config_params, EDMA_CH_DIR_INC_gc);
	edma_channel_set_dest_dir_mode(&config_params, EDMA_CH_DESTDIR_INC_gc);
	edma_channel_set_trigger_source(&config_params,
			EDMA_CH_TRIGSRC_TCD5_CCB_gc);
	edma_channel_set_transfer_count16(&config_params, transfer_count16);
	edma_channel_set_source_address(&config_params, src_addr);
	edma_channel_set_destination_address(&config_params, dest_addr);

	/* Loop through all channels, read back config from them, and verify */
	for (channel_index = EDMA_CH_0;
			channel_index < EDMA_NUMBER_OF_STANDARD_CHANNELS;
			channel_index += 2) {
		edma_channel_write_config(channel_index, &config_params);

		/* Null out the read_config struct */
		memset(&read_config, 0, sizeof(read_config));

		/* Read the config back from the module */
		channel_status = edma_channel_read_config(channel_index,
				&read_config);

		/* Verify */
		test_assert_true(test,
				channel_status == EDMA_CH_FREE,
				"%s%d: Unavailable CH", text_s, channel_index);
		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: CTRLA %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.ctrlb == config_params.ctrlb,
				"%s%d: CTRLB %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.addrctrl == config_params.addrctrl,
				"%s%d: ADDCTRL %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.addr == config_params.addr,
				"%s%d: ADD %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.destaddrctrl == config_params.destaddrctrl,
				"%s%d: DESTADDCTRL %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.destaddr == config_params.destaddr,
				"%s%d: DESTADD %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.trigsrc == config_params.trigsrc,
				"%s%d: TRIGSRC %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.trfcnt16 == config_params.trfcnt16,
				"%s%d: TRFCNT16 %s", text_s, channel_index,
				text_1);

		/* Reset the channel */
		edma_channel_reset(channel_index);

		/* Check set and unset single shot */
		memset(&config_params, 0, sizeof(config_params));
		memset(&read_config, 0, sizeof(read_config));

		edma_channel_set_single_shot(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: SINGLE %s", text_s, channel_index,
				text_2);

		memset(&config_params, 0, sizeof(config_params));
		edma_channel_unset_single_shot(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: SINGLE  %s", text_s, channel_index,
				text_3);

		/* Check set and unset repeat mode */
		memset(&config_params, 0, sizeof(config_params));
		memset(&read_config, 0, sizeof(read_config));

		edma_channel_set_repeat(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: REPEAT %s", text_s, channel_index,
				text_2);

		memset(&config_params, 0, sizeof(config_params));
		edma_channel_unset_repeat(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: REPEAT %s", text_s, channel_index,
				text_3);

		/* Test the direct configuration functions */
		memset(&read_config, 0, sizeof(read_config));

		edma_channel_write_burst_length(channel_index,
				EDMA_CH_BURSTLEN_2BYTE_gc);
		edma_channel_write_transfer_count16(channel_index,
				transfer_count16);
		edma_channel_write_repeat(channel_index, repeat_mode);
		edma_channel_write_src_address(channel_index, src_addr);
		edma_channel_write_dest_address(channel_index, dest_addr);

		/* Read the config back from the module */
		channel_status = edma_channel_read_config(channel_index,
				&read_config);

		/* Verify */
		test_assert_true(test,
				channel_status == EDMA_CH_FREE,
				"%s%d: Unavailable CH", text_s, channel_index);
		test_assert_true(test,
				((read_config.ctrla & EDMA_CH_BURSTLEN_bm) ==
						EDMA_CH_BURSTLEN_2BYTE_gc),
				"%s%d: BURSTLEN %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.trfcnt16 == transfer_count16,
				"%s%d: TRFCNT16 %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				(read_config.ctrla & EDMA_CH_REPEAT_bm) ==
				((uint8_t)repeat_mode) << EDMA_CH_REPEAT_bp,
				"%s%d: REPEAT %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.addr == src_addr,
				"%s%d: ADD %s", text_s, channel_index,
				text_1);
		test_assert_true(test,
				read_config.destaddr == dest_addr,
				"%s%d: DESTADD %s", text_s, channel_index,
				text_1);
	}
	edma_disable();
}

/**
 *
 * \brief Set EDMA configuration for peripheral channels, and read it back
 *
 * \note This function writes a configuration to the EDMA controller, and reads
 * it back to verify settings have been correctly set.
 *
 * \param test   Current test case
 */
static void run_edma_peripheral_config_interface_test(
		const struct test_case *test)
{
	struct edma_channel_config config_params;
	struct edma_channel_config read_config;

	const uint16_t mem_addr = 0xBEEF;
	const uint8_t transfer_count8 = 0xA5;
	const bool repeat_mode = true;

	edma_channel_num_t channel_index;
	uint8_t channel_status;

	/* Null out the config params */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable EDMA, channels 0, 1, 2 and 3 in peripheral conf. */
	edma_enable(EDMA_CHMODE_PER0123_gc);

	/* Apply some parameters */
	edma_channel_set_burst_length(&config_params,
			EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_interrupt_level(&config_params, EDMA_INT_LVL_HI);
	edma_channel_set_memory_reload_mode(&config_params,
			EDMA_CH_RELOAD_BLOCK_gc);
	edma_channel_set_memory_dir_mode(&config_params, EDMA_CH_DIR_INC_gc);
	edma_channel_set_trigger_source(&config_params,
			EDMA_CH_TRIGSRC_SPIC_DRE_gc);
	edma_channel_set_transfer_count8(&config_params, transfer_count8);
	edma_channel_set_memory_address(&config_params, mem_addr);

	/* Loop through all channels, read back config from them, and verify */
	for (channel_index = EDMA_CH_0;
			channel_index < EDMA_NUMBER_OF_PERIPHERAL_CHANNELS;
			channel_index++) {
		edma_channel_write_config(channel_index, &config_params);

		/* Null out the read_config struct */
		memset(&read_config, 0, sizeof(read_config));

		/* Read the config back from the module */
		channel_status = edma_channel_read_config(channel_index,
				&read_config);

		/* Verify */
		test_assert_true(test,
				channel_status == EDMA_CH_FREE,
				"%s%d: Unavailable CH", text_p, channel_index);
		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: CTRLA %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.ctrlb == config_params.ctrlb,
				"%s%d: CTRLB %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.addrctrl == config_params.addrctrl,
				"%s%d: ADDCTRL %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.addr == config_params.addr,
				"%s%d: ADD %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.trigsrc == config_params.trigsrc,
				"%s%d: TRIGSRC %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.trfcnt8 == config_params.trfcnt8,
				"%s%d: TRFCNT8 %s", text_p, channel_index,
				text_1);

		/* Reset the channel */
		edma_channel_reset(channel_index);

		/* Check set and unset single shot */
		memset(&config_params, 0, sizeof(config_params));
		memset(&read_config, 0, sizeof(read_config));

		edma_channel_set_single_shot(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: SINGLE %s", text_p, channel_index,
				text_2);

		memset(&config_params, 0, sizeof(config_params));
		edma_channel_unset_single_shot(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: SINGLE %s", text_p, channel_index,
				text_3);

		/* Check set and unset repeat mode */
		memset(&config_params, 0, sizeof(config_params));
		memset(&read_config, 0, sizeof(read_config));

		edma_channel_set_repeat(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: REPEAT %s", text_p, channel_index,
				text_2);

		memset(&config_params, 0, sizeof(config_params));
		edma_channel_unset_repeat(&config_params);

		edma_channel_write_config(channel_index, &config_params);
		edma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.ctrla == config_params.ctrla,
				"%s%d: REPEAT %s", text_p, channel_index,
				text_3);

		/* Test the direct configuration functions */
		memset(&read_config, 0, sizeof(read_config));

		edma_channel_write_burst_length(channel_index,
				EDMA_CH_BURSTLEN_2BYTE_gc);
		edma_channel_write_transfer_count8(channel_index,
				transfer_count8);
		edma_channel_write_repeat(channel_index, repeat_mode);
		edma_channel_write_memory_address(channel_index, mem_addr);

		/* Read the config back from the module */
		channel_status = edma_channel_read_config(channel_index,
				&read_config);

		/* Verify */
		test_assert_true(test,
				channel_status == EDMA_CH_FREE,
				"%s%d: Unavailable CH", text_p, channel_index);
		test_assert_true(test,
				((read_config.ctrla & EDMA_CH_BURSTLEN_bm) ==
						EDMA_CH_BURSTLEN_2BYTE_gc),
				"%s%d: BURSTLEN %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.trfcnt8 == transfer_count8,
				"%s%d: TRFCNT8 %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				(read_config.ctrla & EDMA_CH_REPEAT_bm) ==
				((uint8_t)repeat_mode) << EDMA_CH_REPEAT_bp,
				"%s%d: REPEAT %s", text_p, channel_index,
				text_1);
		test_assert_true(test,
				read_config.addr == mem_addr,
				"%s%d: ADD  %s", text_p, channel_index,
				text_1);
	}
	edma_disable();
}

/**
 * \brief Test read from fixed location, trigger from timer and callback
 *
 * \note This test sets up a timer to trigger the EDMA module,
 * which in turn reads the timer45_overflow_counter variable and writes
 * it to memory sequentially. It then checks to see that the memory block
 * written is sequential according to the overflow count. This test uses the
 * event system wired on timer45 overflow as trigger.
 *
 * \param test              Current test
 */
static void run_edma_triggered_with_callback(const struct test_case *test)
{
	struct edma_channel_config config_params;
	bool success;

	/* Null the buffer */
	set_buffer(dest_block_tc45, 0x0000);

	/* Null out the config parameter struct */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable the Event System on EVSYS_CH_2 */
	EVSYS.CH2MUX = EVSYS_CHMUX_TCC4_OVF_gc;

	/*
	 * Enable the timer, and set it to count up.
	 * When it overflows, it triggers the EDMA to
	 * read timer45_overflow_counter.
	 */
	tc45_set_overflow_interrupt_callback(&TIMER45,
			timer45_overflow_callback);
	tc45_enable(&TIMER45);
	tc45_set_overflow_interrupt_level(&TIMER45, TC45_INT_LVL_LO);
	tc45_set_direction(&TIMER45, TC45_UP);
	tc45_write_period(&TIMER45, TIMER45_PERIOD);
	tc45_set_resolution(&TIMER45, TIMER45_RESOLUTION);

	/* 
	 * Enable the EDMA module, standard channel 0,
	 * peripheral channels 2 and 3
	*/
	edma_enable(EDMA_CHMODE_STD0_gc);

	/* Set callback for transfer done */
	edma_set_callback(EDMA_CH_0, ut_edma_transfer_is_complete);

	/* Set low interrupt level */
	edma_channel_set_interrupt_level(&config_params, EDMA_INT_LVL_LO);

	/* Set up the EDMA to read the timer value
	 *
	 * - Single shot transfer mode
	 * - 1 byte (16-bit) burst length
	 * - Increment on source and destination
	 * - Reload on burst for source
	 * - No reload for destination
	 */
	edma_channel_set_single_shot(&config_params);
	edma_channel_set_burst_length(&config_params,
			EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_src_reload_mode(&config_params,
			EDMA_CH_RELOAD_BURST_gc);
	edma_channel_set_src_dir_mode(&config_params, EDMA_CH_DIR_FIXED_gc);
	edma_channel_set_dest_reload_mode(&config_params,
			EDMA_CH_RELOAD_NONE_gc);
	edma_channel_set_dest_dir_mode(&config_params, EDMA_CH_DESTDIR_INC_gc);

	/* Set trigger source to Event Channel 2 (set to TCC4's overflow) */
	edma_channel_set_trigger_source(&config_params,
			EDMA_CH_TRIGSRC_EVSYS_CH2_gc);

	/* Transfer DEST_BLOCK_TC45_SIZE bytes */
	edma_channel_set_transfer_count16(&config_params, DEST_BLOCK_TC45_SIZE);

	/* Set address */
	edma_channel_set_source_address(&config_params,
			(uint16_t)(uintptr_t)&timer45_overflow_counter);
	edma_channel_set_destination_address(&config_params,
			(uint16_t)(uintptr_t)dest_block_tc45);

	/* Reset the channel */
	edma_channel_reset(EDMA_CH_0);

	/* Write the config */
	edma_channel_write_config(EDMA_CH_0, &config_params);

	/* Enable the channel */
	edma_channel_enable(EDMA_CH_0);

	/* Wait for transfer to finish */
	while (!edma_has_completed) {
		/* Intentionally left empty */
	}

	/* Disable EDMA and stop TC45 interrupts */
	tc45_set_overflow_interrupt_level(&TIMER45, TC45_INT_LVL_OFF);
	edma_disable();

	/* Verify that the result is as expected */
	success = block_compare(dest_block_tc45, expected_result_tc,
			DEST_BLOCK_TC45_SIZE);

	test_assert_true(test, success, "Result is not as expected");

	/* 
	 * Stop TIMER45:
	 *      Already done in EDMA callback "ut_edma_transfer_is_complete()"
	 */
}

/**
 * \brief Test the error handling of the module
 *
 * \note Test error handling by disabling a channel which is in use
 *
 * \param test              Current test
 */
static void run_edma_error_handling_test(const struct test_case *test)
{
	/* 
	 * Enable EDMA module, peripheral channels 0 and 1,
	 * standard channel 2
	 */
	edma_enable(EDMA_CHMODE_STD2_gc);

	/* Reset the channel */
	edma_channel_reset(EDMA_CH_2);

	/* Set up channel 0 to do some work, check that is it busy,
	 * change some settings and verify a transfer error */
	edma_channel_write_burst_length(EDMA_CH_2, EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_write_transfer_count16(EDMA_CH_2, MEMORY_BLOCK_SIZE);
	edma_channel_write_src_address(EDMA_CH_2,
			(uint16_t)(uintptr_t)memory_block_src);
	edma_channel_write_dest_address(EDMA_CH_2,
			(uint16_t)(uintptr_t)memory_block_dest);

	/* Enable the channel */
	edma_channel_enable(EDMA_CH_2);

	/* Start a block transfer */
	edma_channel_trigger_block_transfer(EDMA_CH_2);

	/* Wait for the channel to become busy */
	while (!edma_channel_is_busy(EDMA_CH_2)) {
		/* Intentionally left empty */
	}

	/* Disable the channel while it is busy */
	if (edma_channel_is_busy(EDMA_CH_2)) {
		edma_channel_disable(EDMA_CH_2);
	}

	/* Test whether the channel is in error */
	test_assert_true(test,
			edma_get_channel_status(
			EDMA_CH_2) == EDMA_CH_TRANSFER_ERROR,
			"EDMA channel not in ERROR after disabling during transfer write");

	edma_disable();
}

/**
 *
 * \brief Test search and double buffering (link) modes
 *
 * \note This function tests search and double buffering features of the EDMA
 * controller by configuring channel 2 searching data. In case of success, the
 * channel 0 is automatically (double buffering) enables (if it is
 * successfull) and copies a memory block. The copy is verified.
 *
 * \param test   Current test case
 */
static void run_edma_search_dbuf_test(const struct test_case *test)
{
	struct edma_channel_config ch0_config_params;
	struct edma_channel_config ch2_config_params;

	bool success;

	/* Fill source block with pattern data */
	set_buffer(memory_block_src, 0x00);
	block_fill(memory_block_src, MEMORY_BLOCK_SIZE);

	/* Null out the destination block */
	set_buffer(memory_block_dest, 0x00);

	/* Null out the config params */
	memset(&ch0_config_params, 0, sizeof(ch0_config_params));
	memset(&ch2_config_params, 0, sizeof(ch2_config_params));

	/* Enable EDMA module, Channels 0 and 2 in standard configuration */
	edma_enable(EDMA_CHMODE_STD02_gc);

	/* Enable double buffering mode on channel 0 and 1 */
	edma_set_double_buffer_mode(EDMA_DBUFMODE_BUF0123_gc);

	/* Set channel 2 to search data (last byte of memory_block_src[]) */
	edma_channel_set_src_reload_mode(&ch2_config_params,
			EDMA_CH_RELOAD_NONE_gc);
	edma_channel_set_src_dir_mode(&ch2_config_params, EDMA_CH_DIR_INC_gc);
	edma_channel_set_dest_reload_mode(&ch2_config_params,
			EDMA_CH_RELOAD_NONE_gc);
	edma_channel_set_search_mode(&ch2_config_params,
			EDMA_CH_DESTDIR_MP1_gc);
	edma_channel_set_burst_length(&ch2_config_params,
			EDMA_CH_BURSTLEN_1BYTE_gc);
	edma_channel_set_trigger_source(&ch2_config_params,
			EDMA_CH_TRIGSRC_OFF_gc);
	edma_channel_set_transfer_count16(&ch2_config_params,
			MEMORY_BLOCK_SIZE);
	edma_channel_set_source_address(&ch2_config_params,
			(uint16_t)(uintptr_t)memory_block_src);
	edma_channel_set_search_data(&ch2_config_params,
			memory_block_src[MEMORY_BLOCK_SIZE - 1], 0xFF);

	/* Write config channel 2 */
	edma_channel_write_config(EDMA_CH_2, &ch2_config_params);

	/*
	 * Set channel 0 to copy from memory_block_src[] to memory_block_dest[]
	 * once a match will occur on channel 2. There will be no transfer if no
	 * match occurs.
	 */
	edma_channel_set_repeat(&ch0_config_params);
	edma_channel_set_burst_length(&ch0_config_params,
			EDMA_CH_BURSTLEN_2BYTE_gc);
	edma_channel_set_src_reload_mode(&ch0_config_params,
			EDMA_CH_RELOAD_TRANSACTION_gc);
	edma_channel_set_dest_reload_mode(&ch0_config_params,
			EDMA_CH_RELOAD_TRANSACTION_gc);
	edma_channel_set_src_dir_mode(&ch0_config_params, EDMA_CH_DIR_INC_gc);
	edma_channel_set_dest_dir_mode(&ch0_config_params,
			EDMA_CH_DESTDIR_INC_gc);
	edma_channel_set_trigger_source(&ch0_config_params,
			EDMA_CH_TRIGSRC_OFF_gc);
	edma_channel_set_transfer_count16(&ch0_config_params,
			MEMORY_BLOCK_SIZE);
	edma_channel_set_source_address(&ch0_config_params,
			(uint16_t)(uintptr_t)memory_block_src);
	edma_channel_set_destination_address(&ch0_config_params,
			(uint16_t)(uintptr_t)memory_block_dest);

	/* Write config channel 0 */
	edma_channel_write_config(EDMA_CH_0, &ch0_config_params);

	/* Enable only channel 2 */
	edma_channel_enable(EDMA_CH_2);

	/* Transfer block and wait for it to finish */
	edma_channel_trigger_block_transfer(EDMA_CH_2);
	edma_channel_trigger_block_transfer(EDMA_CH_0);

	/* Wait for search completion */
	while (edma_get_channel_status(EDMA_CH_2) !=
			EDMA_CH_TRANSFER_COMPLETED) {
		/* Intentionally left empty */
	}

	/* Wait for transfer completion */
	while (edma_get_channel_status(EDMA_CH_0) !=
			EDMA_CH_TRANSFER_COMPLETED) {
		/* Intentionally left empty */
	}

	/* Disable EDMA */
	edma_disable();

	/* Verify that the search result is as expected */
	success = block_compare(
			edma_channel_get_search_pointer(EDMA_CH_2),
			&memory_block_src[MEMORY_BLOCK_SIZE - 1], 1);

	test_assert_true(test, success,
			"SEARCH mode did not function properly");

	/* Verify that the transfer result is as expected */
	success = block_compare(memory_block_src, memory_block_dest,
			MEMORY_BLOCK_SIZE);

	test_assert_true(test, success,
			"DOUBLE BUFFER mode did not function properly");
}

/* @} */

/* ! \brief Set up and run the test suite */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	pmic_init();
	sleepmgr_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Enable low level interrupts */
	pmic_enable_level(PMIC_LVL_LOW);

	/* Enable interrupt requests */
	cpu_irq_enable();

	DEFINE_TEST_CASE(memory_copy_burst_length_test,
			NULL, run_edma_memory_copy_burst_length_test, NULL,
			"Memory copy and burst lengths");

	DEFINE_TEST_CASE(standard_config_interface_test,
			NULL, run_edma_standard_config_interface_test, NULL,
			"Write and read conf param for stdCH");

	DEFINE_TEST_CASE(peripheral_config_interface_test,
			NULL, run_edma_peripheral_config_interface_test, NULL,
			"Write and read conf param for perCH");

	DEFINE_TEST_CASE(trigger_callback_test,
			NULL, run_edma_triggered_with_callback, NULL,
			"External trigger and EDMA callback");

	DEFINE_TEST_CASE(error_handling_test,
			NULL, run_edma_error_handling_test, NULL,
			"Error handling by the module");

	DEFINE_TEST_CASE(search_and_double_buffer_test,
			NULL, run_edma_search_dbuf_test, NULL,
			"SEARCH and DOUBLE BUFFER modes");

	DEFINE_TEST_ARRAY(edma_tests) = {
		&memory_copy_burst_length_test,
		&standard_config_interface_test,
		&peripheral_config_interface_test,
		&trigger_callback_test,
		&error_handling_test,
		&search_and_double_buffer_test
	};

	DEFINE_TEST_SUITE(edma_suite,
			edma_tests,
			"XMEGA EDMA driver test suite");

	test_suite_run(&edma_suite);

	while (1) {
		/* Intentionally left empty */
	}
}
