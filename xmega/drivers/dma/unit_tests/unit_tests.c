/**
 * \file
 *
 * \brief Unit tests for Direct Memory Access controller driver
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
#include <dma.h>
#include <pmic.h>
#include <tc.h>
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
 * This is the unit test application for the \ref dma_group.
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
 * All AVR XMEGA devices can be used.
 * This example has been tested with the following setup:
 * - Xplained
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions \ref
 *unit_tests.c
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref dma_group
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
 * \def DEST_BLOCK_TC_SIZE
 * Size of destination block used in trigger/callback test
 */

/**
 * \def DOUBLE_BUFFER_REPEATS
 * Number of times to repeat copy in double buffer test
 */

/**
 * \def TIMER
 * Timer used in DMA fixed read/trigger/callback test
 */

/**
 * \def TIMER_PERIOD
 * Period of timer
 */

/**
 * \def TIMER_RESOLUTION
 * Resolution of timer
 */

/** @} */

/** \name Variables */
/** @{ */
/** \brief Memory block to read from */
uint8_t memory_block_src[MEMORY_BLOCK_SIZE];
/** \brief Memory block to write to */
uint8_t memory_block_dest[MEMORY_BLOCK_SIZE];

/**
 * \brief Memory block to write to, used by
 * run_dma_triggered_with_callback
 */
uint8_t dest_block_tc[DEST_BLOCK_TC_SIZE];

/** \brief Expected result from trigger/interrupt copy */
uint8_t expected_result_tc[DEST_BLOCK_TC_SIZE] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

/**
 * \brief Whether DMA has finished copying,
 * used by run_dma_triggered_with_callback
 */
volatile bool dma_has_completed = false;

/** \brief Counter incremented by timer overflow */
volatile uint8_t timer_overflow_counter = 0;

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
 * \brief Compare two 8-bit data blocks of given size in reverse
 *
 * \note Compares two blocks of equal size in reverse to see if they are equal.
 *
 * \param block_1    Pointer to first block.
 * \param block_2    Pointer to second block.
 * \param block_size Size of blocks to compare.
 *
 */
static bool block_compare_reverse
	(const uint8_t *block_1, const uint8_t *block_2, uint16_t block_size)
{
	uint16_t byte_index;

	for (byte_index = 0; byte_index < block_size; byte_index++) {
		if (block_1[block_size - byte_index - 1] !=
				block_2[byte_index]) {
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
static void dma_transfer_block(dma_channel_num_t channel_num)
{
	/* Trigger a DMA copy */
	dma_channel_trigger_block_transfer(channel_num);

	/* Wait for it to finish */
	while (dma_get_channel_status(channel_num) !=
			DMA_CH_TRANSFER_COMPLETED) {
		/* Intentionally left empty */
	}
}

/** @} */

/** \name Callback functions */
/** @{ */

/**
 * \brief DMA transfer complete callback
 *
 * \param status             Status of transfer
 */
static void dma_transfer_is_complete(enum dma_channel_status status)
{
	if (status == DMA_CH_TRANSFER_COMPLETED) {
		/* The transfer is complete, tell the waiting test function */
		dma_has_completed = true;
		/* Disable the timer */
		tc_disable(&TIMER);
	}
}

/**
 *
 * \brief Timer overflow callback
 *
 * \note The callback increments the overflow counter
 */
static void timer_overflow_callback(void)
{
	timer_overflow_counter++;
}

/** @} */

/** \name DMA module test functions */
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
static void run_dma_memory_copy_burst_length_test(const struct test_case *test)
{
	struct dma_channel_config config_params;
	uint8_t channel_index;
	bool success = true; /* Assume everything goes well */

	/* Fill the source block with our known pattern */
	set_buffer(memory_block_src, 0x00);
	block_fill(memory_block_src, MEMORY_BLOCK_SIZE);

	/* Null out the config params */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable DMA */
	dma_enable();

	/*
	 * - No reload on source and destination
	 * - Increment source and destination
	 */
	dma_channel_set_src_reload_mode(&config_params,
			DMA_CH_SRCRELOAD_NONE_gc);
	dma_channel_set_src_dir_mode(&config_params,
			DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_reload_mode(&config_params,
			DMA_CH_DESTRELOAD_NONE_gc);
	dma_channel_set_dest_dir_mode(&config_params,
			DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_transfer_count(&config_params,
			MEMORY_BLOCK_SIZE);
	dma_channel_set_source_address(&config_params,
			(uint16_t)(uintptr_t)memory_block_src);
	dma_channel_set_destination_address(&config_params,
			(uint16_t)(uintptr_t)memory_block_dest);

	/* Test burst lengths on all channels */
	for (channel_index = 0; channel_index < DMA_NUMBER_OF_CHANNELS;
			channel_index++) {
		dma_channel_set_burst_length(&config_params,
				DMA_CH_BURSTLEN_1BYTE_gc);
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write 2 byte burst length */
		dma_channel_reset(channel_index);
		dma_channel_set_burst_length(&config_params,
				DMA_CH_BURSTLEN_2BYTE_gc);
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write 4 byte burst length */
		dma_channel_reset(channel_index);
		dma_channel_set_burst_length(&config_params,
				DMA_CH_BURSTLEN_4BYTE_gc);
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write 8 byte burst length */
		dma_channel_reset(channel_index);
		dma_channel_set_burst_length(&config_params,
				DMA_CH_BURSTLEN_8BYTE_gc);
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}
	}

	/* Disable DMA */
	dma_disable();

	test_assert_true(test, success,
			"DMA burst mode memory copy tests failed on channel %d",
			channel_index);
}

/**
 * \brief Test different directions on all channels
 *
 * \note This test copies the source memory block into the destination block
 * in different ways.
 *
 * \param test              Current test
 */
static void run_dma_direction_test(const struct test_case *test)
{
	struct dma_channel_config config_params;
	uint8_t channel_index;
	bool success = true; /* Assume everything goes well */

	/* Fill the source block with our known pattern */
	set_buffer(memory_block_src, 0x00);
	block_fill(memory_block_src, MEMORY_BLOCK_SIZE);

	/* Null out the config params */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable DMA */
	dma_enable();

	/* No reload on source and destination */
	dma_channel_set_src_reload_mode(&config_params,
			DMA_CH_SRCRELOAD_NONE_gc);
	dma_channel_set_dest_reload_mode(&config_params,
			DMA_CH_DESTRELOAD_NONE_gc);
	dma_channel_set_transfer_count(&config_params,
			MEMORY_BLOCK_SIZE);
	dma_channel_set_burst_length(&config_params,
			DMA_CH_BURSTLEN_1BYTE_gc);

	/* Test a memory transfer on all channels */
	for (channel_index = 0; channel_index < DMA_NUMBER_OF_CHANNELS;
			channel_index++) {
		/* Reset channel and write the configuration */
		dma_channel_reset(channel_index);
		/* Increment source, increment destination */
		dma_channel_set_src_dir_mode(&config_params,
				DMA_CH_SRCDIR_INC_gc);
		dma_channel_set_dest_dir_mode(&config_params,
				DMA_CH_DESTDIR_INC_gc);
		/* Data starts from the first byte */
		dma_channel_set_source_address(&config_params,
				(uint16_t)(uintptr_t)memory_block_src);
		dma_channel_set_destination_address(&config_params,
				(uint16_t)(uintptr_t)memory_block_dest);

		/* Write the config */
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write the configuration */
		dma_channel_reset(channel_index);
		/* Decrement source, increment destination */
		dma_channel_set_src_dir_mode(&config_params,
				DMA_CH_SRCDIR_DEC_gc);
		dma_channel_set_dest_dir_mode(&config_params,
				DMA_CH_DESTDIR_INC_gc);
		/* Data starts from the first byte */
		dma_channel_set_source_address(&config_params,
				(uint16_t)(uintptr_t)
				(memory_block_src + MEMORY_BLOCK_SIZE - 1));
		dma_channel_set_destination_address(&config_params,
				(uint16_t)(uintptr_t)memory_block_dest);

		/* Write the config */
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that destination is the reverse of source */
		success = block_compare_reverse(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write the configuration */
		dma_channel_reset(channel_index);
		/* Decrement source, increment destination */
		dma_channel_set_src_dir_mode(&config_params,
				DMA_CH_SRCDIR_INC_gc);
		dma_channel_set_dest_dir_mode(&config_params,
				DMA_CH_DESTDIR_DEC_gc);
		/* Data starts from the first byte */
		dma_channel_set_source_address(&config_params,
				(uint16_t)(uintptr_t)memory_block_src);
		dma_channel_set_destination_address(&config_params,
				(uint16_t)(uintptr_t)
				(memory_block_dest + MEMORY_BLOCK_SIZE - 1));

		/* Write the config */
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that destination is the reverse of source */
		success = block_compare_reverse(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}

		/* Reset channel and write the configuration */
		dma_channel_reset(channel_index);
		/* Decrement source, Decrement destination */
		dma_channel_set_src_dir_mode(&config_params,
				DMA_CH_SRCDIR_DEC_gc);
		dma_channel_set_dest_dir_mode(&config_params,
				DMA_CH_DESTDIR_DEC_gc);
		/* Data starts from the first byte */
		dma_channel_set_source_address(&config_params,
				(uint16_t)(uintptr_t)
				(memory_block_src + MEMORY_BLOCK_SIZE - 1));
		dma_channel_set_destination_address(&config_params,
				(uint16_t)(uintptr_t)
				(memory_block_dest + MEMORY_BLOCK_SIZE - 1));

		/* Write the config */
		dma_channel_write_config(channel_index, &config_params);

		/* Clear destination */
		set_buffer(memory_block_dest, 0x00);

		/* Enable channel, transfer, and disable it */
		dma_channel_enable(channel_index);
		dma_transfer_block(channel_index);
		dma_channel_disable(channel_index);

		/* Check that source and destination are equal */
		success = block_compare(memory_block_src,
				memory_block_dest, MEMORY_BLOCK_SIZE);

		if (!success) {
			break;
		}
	}

	/* Disable DMA */
	dma_disable();

	test_assert_true(test, success,
			"DMA direction copy test failed on channel %d",
			channel_index);
}

/**
 *
 * \brief Set DMA configuration, and read it back
 *
 * \note This function writes a configuration to the DMA
 * controller, and reads it back to verify settings have
 * been correctly set.
 *
 * \param test   Current test case
 */
static void run_dma_config_interface_test(const struct test_case *test)
{
	struct dma_channel_config config_params;
	struct dma_channel_config read_config;

	const uint16_t transfer_count = 1024;
	const uint8_t repeats         = 64;
	uint8_t channel_index;

#ifdef CONFIG_HAVE_HUGEMEM
	hugemem_ptr_t dest_huge_addr  = HUGEMEM_NULL;
	hugemem_ptr_t src_huge_addr   = HUGEMEM_NULL;

	hugemem_write32(dest_huge_addr, 0xABCD1234);
	hugemem_write32(src_huge_addr, 0xAAAABBBB);
#else
	const uint16_t dest_addr      = 0xBEEF;
	const uint16_t src_addr       = 0xABCD;
#endif

	memset(&config_params, 0, sizeof(config_params));

	dma_enable();

	/* Apply some parameters */
	dma_channel_set_burst_length(&config_params,
			DMA_CH_BURSTLEN_4BYTE_gc);
	dma_channel_set_single_shot(&config_params);
	dma_channel_set_interrupt_level(&config_params,
			PMIC_LVL_HIGH);
	dma_channel_set_src_reload_mode(&config_params,
			DMA_CH_SRCRELOAD_BLOCK_gc);
	dma_channel_set_dest_reload_mode(&config_params,
			DMA_CH_DESTRELOAD_BURST_gc);
	dma_channel_set_src_dir_mode(&config_params,
			DMA_CH_SRCDIR_DEC_gc);
	dma_channel_set_dest_dir_mode(&config_params,
			DMA_CH_DESTDIR_DEC_gc);
	dma_channel_set_trigger_source(&config_params,
			DMA_CH_TRIGSRC_TCC0_CCA_gc);
	dma_channel_set_transfer_count(&config_params,
			transfer_count);
	dma_channel_set_repeats(&config_params,
			repeats);

#ifdef CONFIG_HAVE_HUGEMEM
	dma_channel_set_destination_hugemem(&config_params, dest_huge_addr);
	dma_channel_set_source_hugemem(&config_params, src_huge_addr);
#else
	dma_channel_set_destination_address(&config_params, dest_addr);
	dma_channel_set_source_address(&config_params, src_addr);
#endif

	/* Loop through all channels, read back config from them, and verify */
	for (channel_index = 0; channel_index < DMA_NUMBER_OF_CHANNELS; channel_index++) {
		dma_channel_write_config(channel_index, &config_params);

		/* Null out the read_config struct */
		memset(&read_config, 0, sizeof(read_config));

		/* Read the config back from the module */
		dma_channel_read_config(channel_index, &read_config);

		test_assert_true(test,
				read_config.addrctrl == config_params.addrctrl,
				"CH %d: Address control register does not match configuration",
				channel_index);

		test_assert_true(test, read_config.ctrla == config_params.ctrla,
				"CH %d: Control register A does not match configuration",
				channel_index);

		test_assert_true(test,
				read_config.repcnt == config_params.repcnt,
				"CH %d: Repeat counter register does not match configuration",
				channel_index);

		test_assert_true(test,
				read_config.trfcnt == config_params.trfcnt,
				"CH %d: Transfer counter register does not"
				" match configuration", channel_index);

		test_assert_true(test,
				read_config.trigsrc == config_params.trigsrc,
				"CH %d: Trigger source register does not match configuration",
				channel_index);

#ifdef CONFIG_HAVE_HUGEMEM
		test_assert_true(test,
				read_config.destaddr == config_params.destaddr,
				"CH %d: Destination address register does not"
				" match configuration", channel_index);

		test_assert_true(test,
				read_config.srcaddr == config_params.srcaddr,
				"CH %d: Source address register does not match configuration",
				channel_index);
#else
		test_assert_true(test,
				read_config.destaddr16 == config_params.destaddr16,
				"CH %d: DESTADDR16 does not match configuration",
				channel_index);

		test_assert_true(test,
				read_config.srcaddr16 == config_params.srcaddr16,
				"CH %d: SRCADDR16 does not match configuration");
#endif
	}

	/* Reset the channel */
	dma_channel_reset(DMA_CHANNEL_0);

	/* Check set and unset single shot */
	memset(&config_params, 0, sizeof(config_params));
	memset(&read_config, 0, sizeof(read_config));

	dma_channel_set_single_shot(&config_params);

	dma_channel_write_config(DMA_CHANNEL_0, &config_params);
	dma_channel_read_config(DMA_CHANNEL_0, &read_config);

	test_assert_true(test, read_config.ctrla == config_params.ctrla,
			"Single shot mode not set correctly");

	memset(&config_params, 0, sizeof(config_params));
	dma_channel_unset_single_shot(&config_params);

	dma_channel_write_config(DMA_CHANNEL_0, &config_params);
	dma_channel_read_config(DMA_CHANNEL_0, &read_config);

	test_assert_true(test, read_config.ctrla == config_params.ctrla,
			"Single shot mode not unset correctly");

	/* Reset it again, and test the direct configuration functions */
	memset(&read_config, 0, sizeof(read_config));

	dma_channel_write_burst_length(DMA_CHANNEL_0, DMA_CH_BURSTLEN_4BYTE_gc);
	dma_channel_write_transfer_count(DMA_CHANNEL_0, transfer_count);
	dma_channel_write_repeats(DMA_CHANNEL_0, repeats);

#ifdef CONFIG_HAVE_HUGEMEM
	dma_channel_write_source_hugemem(DMA_CHANNEL_0, src_huge_addr);
	dma_channel_write_destination_hugemem(DMA_CHANNEL_0, dest_huge_addr);
#else
	dma_channel_write_source(DMA_CHANNEL_0, src_addr);
	dma_channel_write_destination(DMA_CHANNEL_0, dest_addr);
#endif

	/* Verify that settings have been set correctly */
	dma_channel_read_config(DMA_CHANNEL_0, &read_config);

	test_assert_true(test,
			(read_config.ctrla & DMA_CH_BURSTLEN_gm)
			== DMA_CH_BURSTLEN_4BYTE_gc,
			"Read burst length does not match configuration");

	test_assert_true(test, read_config.trfcnt == transfer_count,
			"Read transfer count does not match configuration");

	test_assert_true(test, read_config.repcnt == repeats,
			"Read repeat value does not match configuration");

#ifdef CONFIG_HAVE_HUGEMEM
	test_assert_true(test, read_config.srcaddr == src_huge_addr,
			"Read source address does not match configuration");

	test_assert_true(test, read_config.destaddr == dest_huge_addr,
			"Read destination address does not match configuration");
#else
	test_assert_true(test, read_config.srcaddr16 == src_addr,
			"Read source address does not match configuration");

	test_assert_true(test, read_config.destaddr16 == dest_addr,
			"Read destination address does not match configuration");
#endif

	dma_disable();
}

/**
 * \brief Test read from fixed location, trigger from timer and callback
 *
 * \note This test sets up a timer to trigger the DMA module,
 * which in turn reads the timer_overflow_counter variable and writes
 * it to memory sequentially. It then checks to see that the memory block
 * written is sequential according to the overflow count.
 *
 * \param test              Current test
 */
static void run_dma_triggered_with_callback(const struct test_case *test)
{
	struct dma_channel_config config_params;
	bool success;

	/* Null the buffer */
	set_buffer(dest_block_tc, 0x0000);

	/* Null out the config parameter struct */
	memset(&config_params, 0, sizeof(config_params));

	/*
	 * Enable the timer, and set it to count up.
	 * When it overflows, it triggers the DMA to
	 * read timer_overflow_counter. */
	tc_enable(&TIMER);

	tc_set_direction(&TIMER, TC_UP);
	tc_write_period(&TIMER, TIMER_PERIOD);

	tc_set_resolution(&TIMER, TIMER_RESOLUTION);

	tc_set_overflow_interrupt_level(&TIMER, PMIC_LVL_LOW);
	tc_set_overflow_interrupt_callback(&TIMER, timer_overflow_callback);

	/* Enable the DMA module */
	dma_enable();

	/* Set callback for transfer done */
	dma_set_callback(DMA_CHANNEL_0, dma_transfer_is_complete);

	/* Set low interrupt level */
	dma_channel_set_interrupt_level(&config_params, PMIC_LVL_LOW);

	/* Set up the DMA to read the timer value
	 *
	 * - Single shot transfer mode
	 * - Two byte (16-bit) burst length
	 * - Increment on source and destination
	 * - Reload on burst for source
	 * - No reload for destination
	 */
	dma_channel_set_single_shot(&config_params);
	dma_channel_set_burst_length(&config_params,
			DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_src_reload_mode(&config_params,
			DMA_CH_SRCRELOAD_BURST_gc);
	dma_channel_set_src_dir_mode(&config_params,
			DMA_CH_SRCDIR_FIXED_gc);
	dma_channel_set_dest_reload_mode(&config_params,
			DMA_CH_DESTRELOAD_NONE_gc);
	dma_channel_set_dest_dir_mode(&config_params,
			DMA_CH_DESTDIR_INC_gc);

	/* Set trigger source to TCC0's overflow */
	dma_channel_set_trigger_source(&config_params,
			DMA_CH_TRIGSRC_TCC0_OVF_gc);

	/* Transfer DEST_BLOCK_TC_SIZE bytes */
	dma_channel_set_transfer_count(&config_params,
			DEST_BLOCK_TC_SIZE);

	/* Set address */
	dma_channel_set_source_address(&config_params,
			(uint16_t)(uintptr_t)&timer_overflow_counter);
	dma_channel_set_destination_address(&config_params,
			(uint16_t)(uintptr_t)dest_block_tc);

	/* Reset the channel */
	dma_channel_reset(DMA_CHANNEL_0);

	/* Write the config */
	dma_channel_write_config(DMA_CHANNEL_0, &config_params);

	/* Enable the channel */
	dma_channel_enable(DMA_CHANNEL_0);

	/* Wait for transfer to finish */
	while (!dma_has_completed) {
		/* Intentionally left empty */
	}

	/* Disable DMA */
	dma_disable();

	/* Verify that the result is as expected */
	success = block_compare(dest_block_tc,
			expected_result_tc, DEST_BLOCK_TC_SIZE);

	test_assert_true(test, success, "Result is not as expected");
}

/**
 * \brief Test the error handling of the module
 *
 * \note Test error handling by disabling a channel which is in use
 *
 * \param test              Current test
 */
static void run_dma_error_handling_test(const struct test_case *test)
{
	/* Enable DMA */
	dma_enable();

	/* Reset the channel */
	dma_channel_reset(DMA_CHANNEL_0);

	/* Set up channel 0 to do some work, check that is it busy,
	 * change some settings and verify a transfer error */
	dma_channel_write_burst_length(DMA_CHANNEL_0,
			DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_write_transfer_count(DMA_CHANNEL_0,
			MEMORY_BLOCK_SIZE);
	dma_channel_write_source(DMA_CHANNEL_0,
			(uint16_t)(uintptr_t)memory_block_src);
	dma_channel_write_destination(DMA_CHANNEL_0,
			(uint16_t)(uintptr_t)memory_block_dest);

	/* Enable the channel */
	dma_channel_enable(DMA_CHANNEL_0);

	/* Start a block transfer */
	dma_channel_trigger_block_transfer(DMA_CHANNEL_0);

	/* Wait for the channel to become busy */
	while (!dma_channel_is_busy(DMA_CHANNEL_0)) {
		/* Intentionally left empty */
	}

	/* Disable the channel while it is busy */
	if (dma_channel_is_busy(DMA_CHANNEL_0)) {
		dma_channel_disable(DMA_CHANNEL_0);
	}

	/* Test whether the channel is in error */
	test_assert_true(test,
			dma_get_channel_status(
			DMA_CHANNEL_0) == DMA_CH_TRANSFER_ERROR,
			"DMA channel not in error after disabling during transfer"
			" write");

	dma_disable();
}

/**
 *
 * \brief Test double buffering mode
 *
 * \note This function tests the double buffering feature of the DMA
 * controller by configuring channel 0 and 1 to do the same copy,
 * and verify that the channels enable each other according to
 * the double buffering process.
 *
 * \param test   Current test case
 */
static void run_dma_double_buffering_test(const struct test_case *test)
{
	struct dma_channel_config config_params;
	bool success = true; /* Assume everything goes well */

	/* Fill source block with pattern data */
	set_buffer(memory_block_src, 0x00);
	block_fill(memory_block_src, MEMORY_BLOCK_SIZE);

	/* Null out the destination block */
	set_buffer(memory_block_dest, 0x00);

	/* Null out the config params */
	memset(&config_params, 0, sizeof(config_params));

	/* Enable DMA */
	dma_enable();

	/* Enable double buffering mode on channel 0 and 1 */
	dma_set_double_buffer_mode(DMA_DBUFMODE_CH01_gc);

	/* Set channel 1 to copy from memory_block_src to memory_block_dest */
	dma_channel_set_src_reload_mode(&config_params,
			DMA_CH_SRCRELOAD_NONE_gc);
	dma_channel_set_src_dir_mode(&config_params,
			DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_reload_mode(&config_params,
			DMA_CH_DESTRELOAD_NONE_gc);
	dma_channel_set_dest_dir_mode(&config_params,
			DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_burst_length(&config_params,
			DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&config_params,
			MEMORY_BLOCK_SIZE);
	dma_channel_set_source_address(&config_params,
			(uint16_t)(uintptr_t)memory_block_src);
	dma_channel_set_destination_address(&config_params,
			(uint16_t)(uintptr_t)memory_block_dest);
	dma_channel_set_repeats(&config_params,
			DOUBLE_BUFFER_REPEATS);

	/* Write config and enable */
	dma_channel_write_config(DMA_CHANNEL_0, &config_params);
	dma_channel_write_config(DMA_CHANNEL_1, &config_params);

	/* Enable only channel 0 */
	dma_channel_enable(DMA_CHANNEL_0);

	/* Transfer block and wait for it to finish */
	dma_channel_trigger_block_transfer(DMA_CHANNEL_0);

	while (dma_get_channel_status(DMA_CHANNEL_0) !=
			DMA_CH_TRANSFER_COMPLETED) {
		/* Intentionally left empty */
	}

	/*
	 * If double buffering is working, channel 1
	 * will be enabled now by the controller
	 */
	if (!(dma_channel_is_enabled(DMA_CHANNEL_1))) {
		success = false;
	}

	/*
	 * Disable channel 0, transfer channel 1,
	 * and verify that channel 0 is enabled again by the controller
	 */
	dma_channel_disable(DMA_CHANNEL_0);

	/* Transfer block and wait for it to finish */
	dma_channel_trigger_block_transfer(DMA_CHANNEL_1);

	while (dma_get_channel_status(DMA_CHANNEL_1) !=
			DMA_CH_TRANSFER_COMPLETED) {
		/* Intentionally left empty */
	}

	/* Verify that channel 0 is enabled again */
	if (!(dma_channel_is_enabled(DMA_CHANNEL_0))) {
		success = false;
	}

	test_assert_true(test, success,
			"Double buffering mode did not function properly");
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

	DEFINE_TEST_CASE(memory_copy_burst_length_test, NULL,
			run_dma_memory_copy_burst_length_test,
			NULL, "Memory copy and burst lengths");

	DEFINE_TEST_CASE(direction_test, NULL, run_dma_direction_test,
			NULL, "Copy directions");

	DEFINE_TEST_CASE(config_interface_test, NULL,
			run_dma_config_interface_test, NULL,
			"Write and read configuration parameters");

	DEFINE_TEST_CASE(trigger_callback_test, NULL,
			run_dma_triggered_with_callback, NULL,
			"External trigger and DMA callback");

	DEFINE_TEST_CASE(error_handling_test, NULL,
			run_dma_error_handling_test, NULL,
			"Error handling by the module");

	DEFINE_TEST_CASE(double_buffering_test, NULL,
			run_dma_double_buffering_test,
			NULL, "Double buffering");

	DEFINE_TEST_ARRAY(dma_tests) = {
		&memory_copy_burst_length_test,
		&direction_test,
		&config_interface_test,
		&trigger_callback_test,
		&error_handling_test,
		&double_buffering_test
	};

	DEFINE_TEST_SUITE(dma_suite, dma_tests, "XMEGA DMA driver test suite");

	test_suite_run(&dma_suite);

	while (1) {
		/* Intentionally left empty */
	}
}
