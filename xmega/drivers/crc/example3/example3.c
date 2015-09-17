/**
 * \file
 *
 * \brief CRC example3 for AVR XMEGA
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
#include <asf.h>
#include <led.h>
#include <compiler.h>
#include <status_codes.h>
#include <sleepmgr.h>

 /**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref crc_group on a DMA
 * transaction.
 *
 * \section files Main Files
 * - crc.c CRC driver implementation
 * - crc.h CRC driver definitions
 * - example3.c example application
 *
 * \section device_info Device Info
 * All AVR XMEGA AU devices with a CRC module can be used. This example have
 * been tested with the following setup:
 * - XMEGA-A3BU Xplained
 *
 * \section description Description of the example
 * The example will calculate the checksum for a data buffer and append it
 * to the buffer. Then it will setup the CRC module for a DMA transaction,
 * and then start a DMA transfer between two internal memory areas, and upon
 * completion heck that the checksum generated for the transfer is zero.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref crc_group
 * - \ref dma_group
 * - \ref gpio_group for LED output
 * - \ref pmic_group for interrupt handling
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

 //! DMA channel to use in example
#define DMA_CHANNEL 0

//! Size of data buffer to transfer
#define DMA_BUFFER_SIZE 8

//! Source buffer to use for DMA transfer
static uint8_t          source[DMA_BUFFER_SIZE];

//! Destination buffer to use for DMA transfer
static uint8_t          destination[DMA_BUFFER_SIZE];

/**
 * \internal
 * \brief Callback for DMA transfer complete
 *
 * \param status Status of a completed (or failed) DMA transfer
 */
static void example_crc_dma_transfer_done(enum dma_channel_status status)
{
	uint32_t checksum;
	checksum = crc_dma_checksum_stop();
	if (checksum == 0) {
		// Light the second LED to indicate that checksum is zero
		LED_On(LED1_GPIO);
	}
}

/**
 * \brief main function
 */
int main(void)
{
	struct dma_channel_config config;
	uint32_t                  checksum;

	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();

	// Randomly selected data
	source[0] = 0xAA;
	source[1] = 0xBB;
	source[2] = 0xCC;
	source[3] = 0xDD;
	source[4] = 0xEE;
	source[5] = 0xFF;

	// Calculate checksum for the data
	checksum = crc_io_checksum((void*)source, 6, CRC_16BIT);
	// Append the checksum to the data, big endian
	crc16_append_value(checksum, source+6);

	//Enable the CRC module for DMA
	crc_dma_checksum_start(DMA_CHANNEL, CRC_16BIT);

	// Enable DMA
	dma_enable();

	// Set callback function for DMA completion
	dma_set_callback(DMA_CHANNEL, example_crc_dma_transfer_done);

	// Make sure config is all zeroed out so we don't get any stray bits
	memset(&config, 0, sizeof(config));

	/**
	 * This example will configure a DMA channel with the following
	 * settings:
	 *  - Low interrupt priority
	 *  - 1 byte burst length
	 *  - DMA_BUFFER_SIZE bytes for each transfer
	 *  - Reload source and destination address at end of each transfer
	 *  - Increment source and destination address during transfer
	 *  - Source address is set to \ref source
	 *  - Destination address is set to \ref destination
	 */
	dma_channel_set_interrupt_level(&config, PMIC_LVL_LOW);
	dma_channel_set_burst_length(&config, DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&config, DMA_BUFFER_SIZE);
	dma_channel_set_src_reload_mode(&config,
			DMA_CH_SRCRELOAD_TRANSACTION_gc);
	dma_channel_set_dest_reload_mode(&config,
			DMA_CH_DESTRELOAD_TRANSACTION_gc);
	dma_channel_set_src_dir_mode(&config, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&config, DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_source_address(&config, (uint16_t)(uintptr_t)source);
	dma_channel_set_destination_address(&config,
			(uint16_t)(uintptr_t)destination);
	dma_channel_write_config(DMA_CHANNEL, &config);

	// Use the configuration above by enabling the DMA channel in use.
	dma_channel_enable(DMA_CHANNEL);

	// Enable interrupts
	cpu_irq_enable();

	// Trigger the DMA transfer
	dma_channel_trigger_block_transfer(DMA_CHANNEL);

	// Light the first LED to indicate that the DMA has started.
	gpio_set_pin_low(LED0_GPIO);

	while (true) {
		/*
		 * Force a NOP instruction for an eventual placement of a debug
		 * session breakpoint.
		 */
		asm("nop\n");
	}
}
