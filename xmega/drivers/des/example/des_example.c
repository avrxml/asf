/**
 * \file
 *
 * \brief AVR XMEGA Data Encryption Standard (DES) driver
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
 * This simple example shows how to use the \ref des_group to encrypt and
 * decrypt 8-byte block data by single DES and 3DES, 3 x 8-byte data block by
 * DES CBC mode.
 *
 * \section files Main Files
 * - des.s DES driver implementation in GCC
 * - des.s90 DES driver implementation in IAR
 * - des.h DES driver definitions
 * - des_example.c DES driver example application
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with DES instruction in the AVR XMEGA core can be used.
 * This example have been tested with the following setup:
 * - Xplain
 * - XMEGA A1 Xplained
 * - XMEGA A3BU Xplained
 *
 * \section description Description of the example
 * The example will implement DES, 3DES and DES in CBC mode. It uses the xplain
 * LEDS to show the AES operation results.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref des_group
 * - \ref gpio_group for LED output
 * - \ref sleepmgr_group for sleep management
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
#include <asf.h>

//! AES data, key block length.
#define DES_BLOCK_LENGTH  8

//! AES data block count.
#define DES_BLOCK_COUNT   3

/**
 *  \brief Plaintext block used by DES and 3DES.
 *
 *  \note  The MSB in the block is byte 0, and LSB is byte 7.
 */
uint8_t data[DES_BLOCK_LENGTH] = {
		0xAB, 0xBA, 0x00, 0xBE, 0xEF, 0x00, 0xDE, 0xAD};

//! \brief Variable used to store DES and tripleDES results.
uint8_t single_ans[DES_BLOCK_LENGTH];

/**
 *  \brief Keys used by all DES operations.
 *  (single DES only uses the first 64-bit key).
 *
 *  \note  The MSB of the 3 keys is byte 0,8 and 16.
 *  The LSB of each key is byte 7, 15 and 23.
 */
uint8_t keys[DES_BLOCK_LENGTH * DES_BLOCK_COUNT] = {
		0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
		0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2,
		0xFE, 0x98, 0x6C, 0x38, 0x23, 0xFC, 0x2D, 0x23};

//! \brief Initial vector used during DES Cipher Block Chaining.
uint8_t init[DES_BLOCK_LENGTH] = {
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

//! \brief Plain text used during DES Cipher Block Chaining.
uint8_t data_block[DES_BLOCK_LENGTH * DES_BLOCK_COUNT]= {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00,
		0xAB, 0xBA, 0x00, 0x00, 0xDE, 0xAD, 0x00, 0x00};

//! \brief Variable used to store ciphertext from DES Cipher Block Chaining.
uint8_t cipher_block_ans[DES_BLOCK_LENGTH * DES_BLOCK_COUNT];

/**
 *  \brief Variable used to store decrypted plaintext from DES Cipher Block
 *  Chaining.
 *
 *  \note It is not possible to use the same variable to store the plaintext
 *  after a Cipher Block Chaining operation, as the previous cipher block is
 *  used to decode the current data block.
 *
 */
uint8_t block_ans[DES_BLOCK_LENGTH * DES_BLOCK_COUNT];

//! \brief Main example doing DES encryption/decryption.
int main( void )
{
	uint8_t i;

	board_init();
	sleepmgr_init();

	bool success = true;

	/* Example of how to use Single DES encryption and decryption functions. */
	des_encrypt(data, single_ans, keys);
	des_decrypt(single_ans, single_ans, keys);

	/* Check if decrypted answer is equal to plaintext. */
	for (i = 0; i < DES_BLOCK_LENGTH ; i++ ){
		if (data[i] != single_ans[i]){
			success = false;
			break;
		}
	}

	if (success){

		/* Example of how to use 3DES encryption and decryption functions. */
		des_3des_encrypt(data, single_ans, keys);
		des_3des_decrypt(single_ans, single_ans, keys);

		/* Check if decrypted answer is equal to plaintext. */
		for (i = 0; i < DES_BLOCK_LENGTH ; i++ ){
			if (data[i] != single_ans[i]){
				success = false;
				break;
		 	}
		}
	}

	if (success){
		/* Example of how to use DES Cipher Block Chaining encryption and
		 * decryption functions.
		 */
		des_cbc_encrypt(data_block, cipher_block_ans, keys, init, true, DES_BLOCK_COUNT);
		des_cbc_decrypt(cipher_block_ans, block_ans, keys, init, true, DES_BLOCK_COUNT);

		/* Check if decrypted answer is equal to plaintext. */
		for (i = 1; i < (DES_BLOCK_LENGTH * DES_BLOCK_COUNT); i++ ){
			if (data_block[i] != block_ans[i]){
				success = false;
				break;
			}
		}
	}

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
