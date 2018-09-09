/**
 * \file
 *
 * \brief AVR XMEGA Advanced Encryption Standard (AES) example
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref aes_group to encrypt and
 * decrypt 128-bit data.
 *
 * \section files Main Files
 * - aes.c AES driver implementation
 * - aes.h AES driver definitions
 * - aes_example1.c AES driver example application
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
 * operation results. Polling of interrupt flag is used in the example.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref aes_group
 * - \ref gpio_group for LED output
 * - \ref pmic_group for interrupt handling
 * - \ref sleepmgr_group for sleep management
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>

//! AES data, key block length.
#define BLOCK_LENGTH   	16

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

//! Variable used to check AES results.
bool success;

/**
 * \brief Generate AES sub key
 *
 * \note Get AES sub key by encryption of dummy data.
 *
 * \param key           Pointer to AES key input.
 * \param last_sub_key  Pointer to AES sub key output.
 *
 */
static bool aes_lastsubkey_generate(t_key key, t_key last_sub_key)
{
	bool keygen_ok;
	uint8_t i;

	/* Before using the AES it is recommended to do an AES software reset to
	 * put the module in known state, in case other parts of your code has
	 * accessed the AES module. */
	aes_software_reset();

	/* Set AES encryption of a single block in manual mode. */
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);

	/* Load key into AES key memory. */
	aes_set_key(key);

	/* Load dummy data into AES state memory. */
	for (i = 0; i < BLOCK_LENGTH; i++) {
		AES.STATE = 0x00;
	}

	/* Start encryption. */
	aes_start();

	do {
		/* Wait until AES is finished or an error occurs. */
	} while (aes_is_busy());

	/* If not error. */
	if (!aes_is_error()) {
		/* Store the last subkey. */
		aes_get_key(last_sub_key);
		aes_clear_interrupt_flag();
		keygen_ok = true;
	} else {
		aes_clear_error_flag();
		keygen_ok = false;
	}
	return keygen_ok;
}

int main( void )
{
	uint8_t i;

	board_init();
	sysclk_init();
	sleepmgr_init();

	/* Assume that everything is ok */
	success = true;

	/* Enable the AES clock. */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_AES);

	/* Do AES encryption and decryption of a single block. */
	//********************************************************
	//               CIPHER IN MANUAL MODE:
	//  - 128bit cryptographic key and data
	//  - ECB cipher mode
	//  - XOR disable
	//  - Polling AES State Ready Interrupt flag
	//********************************************************
	/* Before using the AES it is recommended to do an AES software reset to
	 * put the module in known state, in case other parts of your code has
	 * accessed the AES module. */
	aes_software_reset();

	/* Set AES encryption of a single block in manual mode. */
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);

	/* Disable the AES interrupt. */
	aes_isr_configure(AES_INTLVL_OFF);

	/* Load key into AES key memory. */
	aes_set_key(key);

	/* Load data into AES state memory. */
	aes_write_inputdata(plain_text);

	/* Start encryption. */
	aes_start();

	do {
		/* Wait until AES is finished or an error occurs. */
	} while (aes_is_busy());

	/* Store the result if not error. */
	if (!aes_is_error()) {
		aes_read_outputdata(single_ans);
	} else {
		success = false;
	}

	/* Check if encrypted answer is equal to cipher result. */
	for (i = 0; i < BLOCK_LENGTH ; i++ ) {
		if (cipher_text[i] != single_ans[i]) {
			success = false;
		}
	}

	//********************************************************
	//               DECIPHER IN AUTO MODE:
	//  - 128bit cryptographic key and data
	//  - ECB cipher mode
	//  - XOR disable
	//  - Polling AES State Ready Interrupt flag
	//********************************************************
	/* Generate last subkey. */
	if (!aes_lastsubkey_generate(key, lastsubkey)) {
		success = false;
	}

	/* Before using the AES it is recommended to do an AES software reset to
	 * put the module in known state, in case other parts of your code has
	 * accessed the AES module. */
	aes_software_reset();

	/* Set AES decryption of a single block in auto mode. */
	aes_configure(AES_DECRYPT, AES_AUTO, AES_XOR_OFF);

	/* Disable the AES interrupt. */
	aes_isr_configure(AES_INTLVL_OFF);

	/* Load key into AES key memory. */
	aes_set_key(lastsubkey);

	/* Load data into AES state memory. */
	aes_write_inputdata(cipher_text);
	// NOTE: since we're in auto mode, the ciphering process will start as soon
	// as the correct number of input data is written. In this case, the
	// process should start when we write the sixteenth byte.

	do {
		/* Wait until AES is finished or an error occurs. */
	} while (aes_is_busy());

	/* Store the result if not error. */
	if (!aes_is_error()) {
		aes_read_outputdata(single_ans);
	} else {
		success = false;
	}

	/* Check if decrypted answer is equal to plaintext. */
	for (i = 0; i < BLOCK_LENGTH ; i++ ) {
		if (plain_text[i] != single_ans[i]) {
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

