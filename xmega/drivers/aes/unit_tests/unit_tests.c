/**
 * \file
 *
 * \brief Unit tests for Advanced Encryption Standard crypto module
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

#include <stdint.h>
#include <stdbool.h>
#include <board.h>
#include <sysclk.h>
#include <aes.h>
#include <string.h>
#include <sleepmgr.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>
#include <pmic.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref aes_group.
 * It consists of test cases for the following functionality:
 * - Get and set functions
 * - Status functions
 * - Encryption
 * - Decryption
 * - Auto/manual start with XOR
 * - Interrupt handling
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 * - \ref conf_sleepmgr.h
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used.
 * This example has been tested with the following setup:
 * - XMEGA-A1 Xplained
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test
 * functions \ref unit_tests.c
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref aes_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other
 * compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information,
 * visit <a href="http://www.atmel.com/">Atmel</a>.\n
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

//! \name Variables and result data
//@{

//! \brief Key used for encryption and generation of last subkey
t_key encryption_key = {
	0x30, 0x70, 0x97, 0x1A, 0xB7, 0xCE, 0x45, 0x06,
	0x3F, 0xD2, 0x57, 0x3F, 0x49, 0xF5, 0x42, 0x0D
};

//! \brief Data which will be encrypted
t_data encryption_data = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

//! \brief Data which has been pre-encrypted with above key for comparison
t_data pre_encrypted_data = {
	0x59, 0x1D, 0xA5, 0xBF, 0xEA, 0x0E, 0xD7, 0x61,
	0x24, 0x4E, 0x81, 0xBA, 0x1E, 0xF6, 0x24, 0xB5
};

//! \brief Dummy data used to fill AES state memory
t_data dummy_data = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
 * \brief Last subkey used in decryption of above encrypted data, generated
 * from above encryption key.
 */
t_key lastsubkey;

//! \brief Callback read finished variable
volatile bool aes_is_finished = false;

//! \brief Output data for enc/dec-test
t_data output_data;

//@}

//! \name Helper functions and macros
//@{
//! \brief Set all bytes in \a buffer to \a value.
#define set_buffer(buffer, value) memset(buffer, value, sizeof(buffer))

/**
 * \brief Compare two data blocks
 *
 * Compares two blocks of equal length to see if they are equal.
 *
 * \param block_1    Pointer to first block.
 * \param block_2    Pointer to second block.
 *
 */
static bool compare_data_block(t_data block_1, t_data block_2)
{
	uint8_t byte_index;

	for (byte_index = 0; byte_index < AES_DATA_SIZE; byte_index++) {
		if (block_1[byte_index] != block_2[byte_index]) {
			return false;
		}
	}

	return true;
}

/**
 * \brief Generate AES sub key
 *
 * Get AES sub key by encryption of dummy data.
 *
 * \param key           Pointer to AES key input.
 * \param last_sub_key  Pointer to AES sub key output.
 *
 */
static bool aes_lastsubkey_generate(t_key key, t_key last_sub_key)
{
	bool keygen_ok;

	aes_software_reset();

	/* Set AES encryption of a single block in manual mode. */
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);

	/* Load key into AES key memory. */
	aes_set_key(key);

	/* Load dummy data into AES state memory. It isn't important what is
	 * written, just that a write cycle occurs. */
	aes_write_inputdata(dummy_data);

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
//@}

//! \name AES interface test functions
//@{
/**
 * \brief Test AES get and set key value functions.
 *
 * This test sets a key in the AES module, and reads it back with
 * the get function.
 *
 * \param test Current test case.
 */
static void run_aes_set_and_get_key_test(const struct test_case *test)
{
	t_key read_key;
	bool  success;

	set_buffer(read_key, 0x00);

	/* Reset the module */
	aes_software_reset();

	aes_set_key(encryption_key);
	aes_get_key(read_key);

	/* Check if read_key is the same as what was set */
	success = compare_data_block(encryption_key, read_key);

	test_assert_true(test, success,
			"Key read from AES module is not as set");
}

/**
 * \brief Test AES state interface functions
 *
 * This test verifies that the functions used to check the state of the module
 * are working, and that the clear functions work correctly.
 *
 * \param test Current test case
 */
static void run_aes_state_interface_test(const struct test_case *test)
{
	/* We first reset the module */
	aes_software_reset();

	/* The module should not be busy or in error */
	test_assert_true(test, aes_is_busy(), "Module should not be busy!");
	test_assert_true(test, !aes_is_error(), "Module should not be in error!");

	/* Get the module into error by trying to start the module before loading
	 * values into state memory */
	aes_start();

	/* Module should now be in error */
	test_assert_true(test, aes_is_error(), "Module should be in error!");

	/* Clear the flag and check that it has been cleared */
	aes_clear_error_flag();

	test_assert_true(test, !aes_is_error(),
			"Module error flag should have been cleared!");

	/* Load up dummy data into the module and do a cycle, then check that
	 * the aes_is_busy() function actually works */
	aes_software_reset();

	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);
	aes_set_key(encryption_key);
	aes_write_inputdata(dummy_data);

	aes_start();

	/* Check that the module is busy, the module uses more CPU cycles to
	 * complete than it takes to see if it is busy */
	test_assert_true(test, aes_is_busy(), "AES module should be busy!");

	do {
		/* Wait until the module is finished */
	} while(aes_is_busy());

	/* The module now has data in the status register. We want to clear the
	 * flag without reading the data. First verify that the flag is set */
	test_assert_true(test, AES.STATUS & AES_SRIF_bm,
			"State ready interrupt flag is not set!");

	aes_clear_interrupt_flag();

	/* Now, check that we have cleared the flag */
	test_assert_true(test, !(AES.STATUS & AES_SRIF_bm),
			"State ready interrupt flag should not be set!");
}

/**
 * \brief Test AES encryption function
 *
 * This test generates an encrypted byte string from a key and input,
 * and compares it to a pre-encrypted value.
 *
 * \param test Current test case.
 */
static void run_aes_encryption_test(const struct test_case *test)
{
	t_data encrypted_data;
	bool   success;

	/* Zero out the output data storage */
	set_buffer(encrypted_data, 0x00);

	/* Reset the module */
	aes_software_reset();

	/*
	 * Configure AES module to encrypt, triggered by software,
	 * with no XOR
	 */
	aes_configure(AES_ENCRYPT, AES_MANUAL, AES_XOR_OFF);
	aes_set_key(encryption_key);
	aes_write_inputdata(encryption_data);

	aes_start();

	do {
		/* Wait until AES is finished or an error occurs. */
	} while (aes_is_busy());

	aes_read_outputdata(encrypted_data);

	/* Verify that this data is correct by comparing it
	 * to our pre-encrypted value
	 */
	success = compare_data_block(encrypted_data, pre_encrypted_data);

	test_assert_true(test, success,
			"Encrypted values do not match pre-encrypted"
			" values");
}

/**
 * \brief Test AES decryption function.
 *
 * This test decrypts the pre-encrypted data and checks
 * whether it is correct.
 *
 * \param test Current test case.
 */
static void run_aes_decryption_test(const struct test_case *test)
{
	t_key decrypted_data;
	bool  success;

	set_buffer(decrypted_data, 0x00);
	set_buffer(lastsubkey, 0x00);

	/* Call helper function to generate a last subkey for decryption */
	if (!aes_lastsubkey_generate(encryption_key, lastsubkey)) {
		success = false;
		test_assert_true(test, !success,
				"Could not generate last subkey");
	} else {
		/* Configure module for manual decryption */
		aes_software_reset();
		aes_set_key(lastsubkey);
		aes_configure(AES_DECRYPT, AES_MANUAL, AES_XOR_OFF);
		aes_write_inputdata(pre_encrypted_data);

		aes_start();

		do {
			/* Wait until AES is finished or an error occurs. */
		} while (aes_is_busy());

		aes_read_outputdata(decrypted_data);

		/* Verify that the decrypted data is correct */
		success = compare_data_block(decrypted_data, encryption_data);

		test_assert_true(test, success,
				"Decrypted values do not match excepted values");
	}
}

/**
 * \brief Callback for AES interrupt
 */
static void int_callback_aes(void)
{
	aes_read_outputdata(output_data);
	aes_is_finished = true;
}

/**
 * \brief Run encryption, then decryption. Announce end by interrupt.
 *
 * This test encrypts a data block, decrypts it, and checks if it's
 * correct with respect to the input data block. Utilizes interrupts
 * to let the software know when a encryption/decryption is done.
 * AUTO mode will be used in this test.
 *
 * \param test Current test case.
 */
static void run_aes_encrypt_and_decrypt_test(const struct test_case *test)
{
	bool success = true;

	set_buffer(output_data, 0x00);
	set_buffer(lastsubkey, 0x00);

	aes_software_reset();

	/* Configure AES for automatic mode, with XOR, and interrupt. */
	aes_configure(AES_ENCRYPT, AES_AUTO, AES_XOR_ON);
	aes_isr_configure(AES_INTLVL_LO);
	aes_set_key(encryption_key);
	aes_write_inputdata(encryption_data);

	/*
	 * Wait for it to end, since we actually don't have
	 * anything better to do
	 */
	while (!aes_is_finished) {
		/* Intentionally left empty */
	}

	/* output_data should now contain encrypted data */

	/* Configure AES for automatic mode, XOR off. Last subkey
	 * is already in the module, so we don't need to load it
	 */
	aes_is_finished = false;
	aes_configure(AES_DECRYPT, AES_AUTO, AES_XOR_OFF);
	aes_write_inputdata(output_data);

	while (!aes_is_finished) {
		/* Intentionally left empty */
	}

	/* The AES module should be finished now, and output_data
	 * should contain our initial data. */
	success = compare_data_block(output_data, encryption_data);

	test_assert_true(test, success,
			"End values do not match start values");
}
//@}

//! \brief Set up and run test suite
int main(void)
{
	/* USART init values */
	const usart_serial_options_t usart_serial_options =
	{
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	/* Start services */
	pmic_init();
	sysclk_init();
	board_init();
	sleepmgr_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Enable the clock for the AES module */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_AES);

	/* Enable global interrupts */
	cpu_irq_enable();

	// Set callback for AES module
	aes_set_callback(&int_callback_aes);

	/* Define test cases */
	DEFINE_TEST_CASE(aes_get_set_test, NULL, run_aes_set_and_get_key_test,
			NULL, "Get and set functions");
	DEFINE_TEST_CASE(aes_state_interface_test, NULL,
			run_aes_state_interface_test, NULL, "Test of AES state functions");
	DEFINE_TEST_CASE(aes_encryption_test, NULL, run_aes_encryption_test,
			NULL, "Encryption with known result");
	DEFINE_TEST_CASE(aes_decryption_test, NULL, run_aes_decryption_test,
			NULL, "Decryption function known result");
	DEFINE_TEST_CASE(aes_enc_dec_test, NULL,
			run_aes_encrypt_and_decrypt_test, NULL,
			"Encryption and decryption with interrupt and auto mode");

	DEFINE_TEST_ARRAY(aes_tests) = {
		&aes_get_set_test,
		&aes_state_interface_test,
		&aes_encryption_test,
		&aes_decryption_test,
		&aes_enc_dec_test
	};

	DEFINE_TEST_SUITE(aes_suite, aes_tests, "XMEGA AES driver test suite");

	/* Run all test in the suite */
	test_suite_run(&aes_suite);

	while (1) {
		/* Intentionally left blank */
	}
}
