/**
 * \file
 *
 * \brief AVR XMEGA Advanced Encryption Standard (AES) example
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
 * This simple example shows how to use the \ref aes_group to encrypt 128-bit
 * data with DMA support.
 *
 * \section files Main Files
 * - aes.c AES driver implementation
 * - aes.h AES driver definitions
 * - aes_example3.c AES driver example application
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with AES crypto module can be used. This example have
 * been tested with the following setup:
 * - Xplain
 * - XMEGA A1 Xplained
 * - XMEGA A3BU Xplained
 *
 * \section description Description of the example
 * The example will initialize the AES and use the xplain LEDS to show the AES
 * operation results. DMA support is used in the example.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref aes_group
 * - \ref gpio_group for LED output
 * - \ref pmic_group for interrupt handling
 * - \ref sleepmgr_group for sleep management
 * - \ref dma_group for DMA support
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <string.h>
#include <asf.h>

//! AES data, key block length.
#define BLOCK_LENGTH	16

//! DMA channel used.
#define DMA_CHANNEL_N	0

//! Key from FIPS-197 = 00010203 04050607 08090A0B 0C0D0E0.
uint8_t key[BLOCK_LENGTH] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

//! Plaintext from FIPS-197 = 00112233 44556677 8899AABB CCDDEEFF.
uint8_t plain_text[BLOCK_LENGTH] = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55,	0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

//! Cipher result from FIPS-197 = 69c4e0d8 6a7b0430 d8cdb780 70b4c55a.
uint8_t cipher_text[BLOCK_LENGTH] = {
		0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
		0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};

//! Variable used to store the result from a single AES encryption/decryption.
uint8_t single_ans[BLOCK_LENGTH];

/**
 * \brief Key used when the AES shall decrypt.
 *
 * This last subkey is a modified version of the key.
 */
uint8_t lastsubkey[BLOCK_LENGTH];

//! Initialization vector used during Cipher Block Chaining (CBC).
uint8_t init[BLOCK_LENGTH] = {
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

//! Variable used to check AES results.
bool success;

//! DMA channel configuration.
struct dma_channel_config config;

/**
 * \brief Access AES module with DMA support
 *
 * \note Set KEY and write STATE using DMA channel 0.
 * Check DMA driver example for more information about DMA usage.
 *
 */
static void aes_dma_input(void)
{
	// Make sure config is all zeroed out so we don't get any stray bits
	memset(&config, 0, sizeof(config));

	dma_enable();
	dma_channel_set_burst_length(&config, DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&config, BLOCK_LENGTH);
	dma_channel_set_src_reload_mode(&config, DMA_CH_SRCRELOAD_NONE_gc);
	dma_channel_set_dest_reload_mode(&config, DMA_CH_DESTRELOAD_NONE_gc);
	dma_channel_set_src_dir_mode(&config, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&config, DMA_CH_DESTDIR_FIXED_gc);
	dma_channel_set_source_address(&config, (uint16_t)(uintptr_t)key);
	dma_channel_set_destination_address(&config,
		(uint16_t)(uintptr_t)&AES_KEY);
	dma_channel_write_config(DMA_CHANNEL_N, &config);

	// Use the configuration above by enabling the DMA channel in use.
	dma_channel_enable(DMA_CHANNEL_N);

	/*
	 * Trigger a manual start since there is no trigger sources used in
	 * this example.
	 */
	dma_channel_trigger_block_transfer(DMA_CHANNEL_N);

	while (!(DMA_CH0_CTRLB & DMA_CH_TRNIF_bm));
	DMA_CH0_CTRLB |= DMA_CH_TRNIF_bm;
	dma_channel_set_source_address(&config, (uint16_t)(uintptr_t)plain_text);
	dma_channel_set_destination_address(&config,
		(uint16_t)(uintptr_t)&AES_STATE);
	dma_channel_write_config(DMA_CHANNEL_N, &config);

	// Use the configuration above by enabling the DMA channel in use.
	dma_channel_enable(DMA_CHANNEL_N);

	/*
	 * Trigger a manual start since there is no trigger sources used in
	 * this example.
	 */
	dma_channel_trigger_block_transfer(DMA_CHANNEL_N);
	while (!(DMA_CH0_CTRLB & DMA_CH_TRNIF_bm));
	DMA_CH0_CTRLB |= DMA_CH_TRNIF_bm;
}

/**
 * \brief Access AES module with DMA support
 *
 * \note Read STATE using DMA channel 0.
 * Check DMA driver example for more information about DMA usage.
 *
 */
static void aes_dma_output(void)
{
	// Make sure config is all zeroed out so we don't get any stray bits
	memset(&config, 0, sizeof(config));

	dma_enable();
	dma_channel_set_burst_length(&config, DMA_CH_BURSTLEN_1BYTE_gc);
	dma_channel_set_transfer_count(&config, BLOCK_LENGTH);
	dma_channel_set_src_reload_mode(&config, DMA_CH_SRCRELOAD_NONE_gc);
	dma_channel_set_dest_reload_mode(&config, DMA_CH_DESTRELOAD_NONE_gc);
	dma_channel_set_src_dir_mode(&config, DMA_CH_SRCDIR_FIXED_gc);
	dma_channel_set_dest_dir_mode(&config, DMA_CH_DESTDIR_INC_gc);
	dma_channel_set_source_address(&config, (uint16_t)(uintptr_t)&AES_STATE);
	dma_channel_set_destination_address(&config,
		(uint16_t)(uintptr_t)single_ans);
	dma_channel_write_config(DMA_CHANNEL_N, &config);

	// Use the configuration above by enabling the DMA channel in use.
	dma_channel_enable(DMA_CHANNEL_N);

	/*
	 * Trigger a manual start since there is no trigger sources used in
	 * this example.
	 */
	dma_channel_trigger_block_transfer(DMA_CHANNEL_N);
	while (!(DMA_CH0_CTRLB & DMA_CH_TRNIF_bm));
	DMA_CH0_CTRLB |= DMA_CH_TRNIF_bm;

	dma_disable();
}

int main( void )
{
	uint8_t i;

	board_init();
	sysclk_init();
	sleepmgr_init();

	/* Assume that everything is ok*/
	success = true;

	/* Enable the AES clock. */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_AES);

	/* Do AES encryption of a single block with DMA support. */
	//********************************************************
	//               CIPHER IN MANUAL MODE:
	//  - 128bit cryptographic key and data
	//  - ECB cipher mode
	//  - XOR disable
	//  - DMA support for AES input and output
	//********************************************************
	/* Before using the AES it is recommended to do an AES software reset to
	 * put the module in known state, in case other parts of your code has
	 * accessed the AES module. */
	aes_software_reset();

	/* Set AES encryption of a single block in manual mode. */
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);

	/* Disable the AES interrupt. */
	aes_isr_configure(AES_INTLVL_OFF);

	/* Set KEY and write STATE using DMA channel 0. */
	aes_dma_input();

	/* Start encryption. */
	aes_start();

	do {
		/* Wait until AES is finished or an error occurs. */
	} while (aes_is_busy());

	/* Store the result if not error. */
	if (!aes_is_error()){
		/* Read STATE using DMA channel 0. */
		aes_dma_output();
	} else {
		success = false;
	}

	/* Check if encrypted answer is equal to cipher result. */
	for (i = 0; i < BLOCK_LENGTH ; i++ ) {
		if (cipher_text[i] != single_ans[i]) {
			success = false;
		}
	}

	/* Disable the AES clock. */
	sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_AES);

	/* Indicate final result by lighting LED. */
	if (success) {
		/* If the example ends up here every thing is ok. */
		ioport_set_pin_low(LED0_GPIO);
	} else {
		/* If the example ends up here something is wrong. */
		ioport_set_pin_low(LED1_GPIO);
	}

	while (true) {
		/* Go to sleep. */
		sleepmgr_enter_sleep();
	}
}

