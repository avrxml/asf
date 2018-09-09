/**
 * \file
 *
 * \brief CRC example3 for AVR XMEGA
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
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
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
