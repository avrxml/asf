/**
 * \file
 *
 * \brief Unit tests for Non Volatile Memory controller driver
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
#include <string.h>
#include <unit_test/suite.h>
#include <stdio_serial.h>
#include <conf_test.h>
#include <sleepmgr.h>
#include <des.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the \ref nvm_group.
 * It consists of test cases for the following functionality:
 * - Encryption/decryption round test with DES/3DES
 * - Pre-calculated value tests for DES/3DES encryption/decryption
 * - Pre-calculated value tests for DES/3DES CBC encryption/decryption
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
 * - Xplained
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions \ref unit_tests.c
 * "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 * - \ref des_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
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
 * \brief Stop bit configuration of USART
 */
/**
 * \def DES_BLOCK_LENGTH
 * \brief Length of a single DES block
 */
/**
 * \def DES_CHAIN_BLOCK_COUNT
 * \brief Number of DES blocks used in chained encrypt
 */
/**
 * \def DES_KEY_COUNT
 * \brief Number of keys used in 3DES encrypt/decrypt
 */
//@}

//! \name Variables and expected results
//@{
//! \brief Data which will be encrypted
uint8_t data[DES_BLOCK_LENGTH] = {
	0x41, 0x56, 0x52, 0x78, 0x6D, 0x65, 0x67, 0x61};

//! \brief Data which will be encrypted with CBC
uint8_t data_cbc[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT] = {
	0x41, 0x56, 0x52, 0x78, 0x6D, 0x65, 0x67, 0x61,
	0x25, 0x52, 0x11, 0x94, 0x00, 0x10, 0x99, 0xAC,
	0xB2, 0xF0, 0xE3, 0xAE, 0xC7, 0x08, 0x43, 0x10};

//! \brief Pre-DES-encrypted result of the original \a data
uint8_t pre_des_encrypted_data[DES_BLOCK_LENGTH] = {
	0x3F, 0x9F, 0x66, 0xDA, 0xB7, 0x55, 0xF6, 0x6E};

//! \brief Pre-3DES-encrypted result of the original \a data
uint8_t pre_3des_encrypted_data[DES_BLOCK_LENGTH] = {
	0x89, 0x24, 0x41, 0x15, 0x1C, 0x68, 0x27, 0x2B};

//! \brief Pre CBC-DES-encrypted result of \a data_cbc
uint8_t pre_des_cbc_encrypted_data
	[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT] = {
	0xA2, 0x4C, 0xB5, 0x45, 0xE2, 0x23, 0x8F, 0xC3,
	0xF3, 0xD0, 0x8D, 0xC1, 0x5A, 0x46, 0x86, 0xC0,
	0xEC, 0x6E, 0x66, 0x28, 0x9D, 0xBD, 0xD8, 0xC9};

//! \brief Pre CBC-3DES-encrypted result of \a data_cbc
uint8_t pre_3des_cbc_encrypted_data
	[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT] = {
	0x97, 0xAF, 0x72, 0x58, 0xAE, 0xB5, 0xA4, 0x14,
	0x97, 0x52, 0x2B, 0xD2, 0x78, 0x22, 0x5C, 0x1C,
	0x06, 0x21, 0x59, 0x14, 0x29, 0x50, 0x4B, 0x41};

//! \brief Initialization vector used for starting CBC
uint8_t init[DES_BLOCK_LENGTH] = {
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

//! \brief Keys used for the 3DES-encryption. Only first key for DES.
uint8_t keys[DES_BLOCK_LENGTH * DES_KEY_COUNT] = {
	0x94, 0x74, 0xB8, 0xE8, 0xC7, 0x3B, 0xCA, 0x7D,
	0x28, 0x34, 0x76, 0xAB, 0x38, 0xCF, 0x37, 0xC2,
	0xFE, 0x98, 0x6C, 0x38, 0x23, 0xFC, 0x2D, 0x23};
//@}

//! \name Helper functions and macros
//@{
//! \brief Set all bytes in \a buffer to \a value.
#define set_buffer(buffer, value) memset(buffer, value, sizeof(buffer))

/**
 * \brief Compare two data blocks of given size
 *
 * \note Compares two blocks of equal size to see if they are equal.
 *
 * \param block_1    Pointer to first block.
 * \param block_2    Pointer to second block.
 * \param block_size Size of blocks to compare.
 *
 */
static bool compare_data_block
		(const uint8_t *block_1, const uint8_t *block_2, uint8_t block_size)
{
	uint8_t byte_index;

	for (byte_index = 0; byte_index < block_size; byte_index++) {
		if (block_1[byte_index] != block_2[byte_index]) {
			return false;
		}
	}

	return true;
}
//@}

//! \name Unit test functions
//@{

/**
 * \brief Run DES encryption and decryption in series
 *
 * \note This test encrypts a known data block with a known key,
 * then decrypts it with the same key, and compares the result
 * with the original data.
 *
 * \param test Current test case.
 */
static void run_des_encryption_and_decryption_test
		(const struct test_case *test)
{
	uint8_t enc_result[DES_BLOCK_LENGTH];
	uint8_t dec_result[DES_BLOCK_LENGTH];
	bool    success;

	// Null out the two result blocks
	set_buffer(enc_result, 0x00);
	set_buffer(dec_result, 0x00);

	// Encrypt using only the first key with DES
	des_encrypt(data, enc_result, keys);
	// Decrypt using only the first key with DES
	des_decrypt(enc_result, dec_result, keys);

	// Compare the result with the initial data
	success = compare_data_block(dec_result, data, DES_BLOCK_LENGTH);

	test_assert_true(test, success,
			"DES decrypted value differs from original data");
}

/**
 * \brief Run 3DES encryption and decryption in series
 *
 * \note This test encrypts a known data block with known keys,
 * then decrypts it with the same keys, and compares the result
 * with the original data.
 *
 * \param test Current test case.
 */
static void run_3des_encryption_and_decryption_test
		(const struct test_case *test)
{
	uint8_t enc_result[DES_BLOCK_LENGTH];
	uint8_t dec_result[DES_BLOCK_LENGTH];
	bool    success;

	// Null out the two result blocks
	set_buffer(enc_result, 0x00);
	set_buffer(dec_result, 0x00);

	// Encrypt using all three keys by 3DES
	des_3des_encrypt(data, enc_result, keys);

	// Decrypt using all three keys by 3DES
	des_3des_decrypt(enc_result, dec_result, keys);

	// Compare the end result with the initial data
	success = compare_data_block(dec_result, data, DES_BLOCK_LENGTH);

	test_assert_true(test, success,
			"3DES decrypted values differ from original data");
}

/**
 * \brief Run DES encryption with known result
 *
 * \note This test encrypts a data block, and compares
 * it with a known pre-encrypted block from the same data,
 * and compares them.
 *
 * \param test Current test case.
 */
static void run_des_encryption(const struct test_case *test)
{
	uint8_t enc_result[DES_BLOCK_LENGTH];
	bool success;

	set_buffer(enc_result, 0x00);

	// Uses first key only
	des_encrypt(data, enc_result, keys);

	success = compare_data_block(enc_result,
			pre_des_encrypted_data, DES_BLOCK_LENGTH);

	test_assert_true(test, success,
			"DES encryption result does not match pre-calculated values");
}

/**
 * \brief Run DES decryption with known result
 *
 * \note This test decrypts the known pre-encrypted data
 * block and compares it to the data which it was created
 * from.
 *
 * \param test Current test case.
 */
static void run_des_decryption(const struct test_case *test)
{
	uint8_t dec_result[DES_BLOCK_LENGTH];
	bool success;

	set_buffer(dec_result, 0x00);

	des_decrypt(pre_des_encrypted_data, dec_result, keys);

	success = compare_data_block(data, dec_result, DES_BLOCK_LENGTH);

	test_assert_true(test, success,
			"DES decryption result does not match pre-encrypted data");
}

/**
 * \brief Run 3DES encryption with known result
 *
 * \note This test encrypts a data block, and compares
 * it with a known pre-encrypted block from the same data,
 * and compares them.
 *
 * \param test Current test case.
 */
static void run_3des_encryption(const struct test_case *test)
{
	uint8_t enc_result[DES_BLOCK_LENGTH];
	bool success;

	set_buffer(enc_result, 0x00);

	// Uses all keys
	des_3des_encrypt(data, enc_result, keys);

	success = compare_data_block(enc_result,
			pre_3des_encrypted_data, DES_BLOCK_LENGTH);

	test_assert_true(test, success,
			"3DES encryption result does not match pre-calculated values");
}

/**
 * \brief Run 3DES decryption with known result
 *
 * \note This test decrypts the known pre-encrypted data
 * block and compares it to the data which it was created
 * from.
 *
 * \param test Current test case.
 */
static void run_3des_decryption(const struct test_case *test)
{
	uint8_t dec_result[DES_BLOCK_LENGTH];
	bool success;

	set_buffer(dec_result, 0x00);

	des_3des_decrypt(pre_3des_encrypted_data, dec_result, keys);

	success = compare_data_block(data, dec_result, DES_BLOCK_LENGTH);

	test_assert_true(test, success,
		"3DES decryption result does not match pre-encrypted data");
}

/**
 * \brief Run DES CBC encryption on three data blocks, with known result
 *
 * \note This test runs DES CBC encryption on three blocks of data,
 * using a known init vector. It then compares with pre-calculated
 * data to verify the encryption.
 *
 * \param test Current test case.
 */
static void run_des_cbc_encryption_test
		(const struct test_case *test)
{
	uint8_t enc_result[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT];
	bool success;

	set_buffer(enc_result, 0x00);

	// Encrypt using first key, with DES and chaining, and initial value
	des_cbc_encrypt(data_cbc, enc_result, keys,
			init, false, DES_CHAIN_BLOCK_COUNT);

	success = compare_data_block(enc_result, pre_des_cbc_encrypted_data,
			DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT);

	test_assert_true(test, success,
			"DES chained encrypted values differ from expected result");
}

/**
 * \brief Run DES CBC decryption on three data blocks, with known result
 *
 * \note This test runs DES CBC decryption on the pre-encrypted data,
 * using a known init vector. It then compares with the original
 * data to verify the decryption.
 *
 * \param test Current test case.
 */
static void run_des_cbc_decryption_test
		(const struct test_case *test)
{
	uint8_t dec_result[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT];
	bool success;

	set_buffer(dec_result, 0x00);

	// Encrypt using first key, with DES and chaining, and initial value
	des_cbc_decrypt(pre_des_cbc_encrypted_data, dec_result,
			keys, init, false, DES_CHAIN_BLOCK_COUNT);

	success = compare_data_block(dec_result, data_cbc,
			DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT);

	test_assert_true(test, success,
			"DES chained decryption values differ from original data");
}

/**
 * \brief Run 3DES CBC encryption on three data blocks, with known result
 *
 * \note This test runs 3DES CBC encryption on three blocks of data,
 * using a known init vector. It then compares with pre-calculated
 * data to verify the encryption.
 *
 * \param test Current test case.
 */
static void run_3des_cbc_encryption_test
		(const struct test_case *test)
{
	uint8_t enc_result[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT];
	bool success;

	set_buffer(enc_result, 0x00);

	// Encrypt using all three keys, with 3DES and chaining, with initial value
	des_cbc_encrypt(data_cbc, enc_result, keys,
			init, true, DES_CHAIN_BLOCK_COUNT);

	success = compare_data_block(enc_result, pre_3des_cbc_encrypted_data,
			DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT);

	test_assert_true(test, success,
			"3DES chained encrypted values differ from expected result");
}

/**
 * \brief Run 3DES CBC decryption on three data blocks, with known result
 *
 * \note This test runs 3DES CBC decryption on the pre-encrypted data,
 * using a known init vector. It then compares with the original
 * data to verify the decryption.
 *
 * \param test Current test case.
 */
static void run_3des_cbc_decryption_test
		(const struct test_case *test)
{
	uint8_t dec_result[DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT];
	bool success;

	set_buffer(dec_result, 0x00);

	// Decrypt using all three keys, 3DES chaining with initial value
	des_cbc_decrypt(pre_3des_cbc_encrypted_data, dec_result,
			keys, init, true, DES_CHAIN_BLOCK_COUNT);

	success = compare_data_block(dec_result, data_cbc,
			DES_BLOCK_LENGTH * DES_CHAIN_BLOCK_COUNT);

	test_assert_true(test, success,
			"3DES chained decrypted values differ from original");
}


//@}

/**
 * \brief Run DES driver unit tests
 */
int main (void)
{
	const usart_serial_options_t usart_serial_options =
	{
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	sleepmgr_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	// Define all the test cases
	DEFINE_TEST_CASE(enc_dec_des_test, NULL,
			run_des_encryption_and_decryption_test,
			NULL, "DES encryption/decryption test");

	DEFINE_TEST_CASE(enc_dec_3des_test, NULL,
			run_3des_encryption_and_decryption_test,
			NULL, "3DES encryption/decryption test");

	DEFINE_TEST_CASE(enc_des_pre_test, NULL, run_des_encryption,
			NULL, "Pre-calculated values DES encryption test");

	DEFINE_TEST_CASE(dec_des_pre_test, NULL, run_des_decryption,
			NULL, "Pre-calculated values DES decryption test");

	DEFINE_TEST_CASE(enc_3des_pre_test, NULL, run_3des_encryption,
			NULL, "Pre-calculated values 3DES encryption test");

	DEFINE_TEST_CASE(dec_3des_pre_test, NULL, run_3des_decryption,
			NULL, "Pre-calculated values 3DES decryption test");

	DEFINE_TEST_CASE(enc_des_cbc_pre_test, NULL,
			run_des_cbc_encryption_test, NULL,
			"Pre-calculated values DES CBC encryption test");

	DEFINE_TEST_CASE(dec_des_cbc_pre_test, NULL,
			run_des_cbc_decryption_test, NULL,
			"Pre-calculated values DES CBC decryption test");

	DEFINE_TEST_CASE(enc_3des_cbc_pre_test, NULL,
			run_3des_cbc_encryption_test, NULL,
			"Pre-calculated values 3DES CBC encryption test");

	DEFINE_TEST_CASE(dec_3des_cbc_pre_test, NULL,
			run_3des_cbc_decryption_test, NULL,
			"Pre-calculated values 3DES CBC decryption test");

	// Put test case addresses in an array
	DEFINE_TEST_ARRAY(des_tests) = {
		&enc_dec_des_test,
		&enc_dec_3des_test,
		&enc_des_pre_test,
		&dec_des_pre_test,
		&enc_3des_pre_test,
		&dec_3des_pre_test,
		&enc_des_cbc_pre_test,
		&dec_des_cbc_pre_test,
		&enc_3des_cbc_pre_test,
		&dec_3des_cbc_pre_test
	};

	// Define the test suite
	DEFINE_TEST_SUITE(des_suite, des_tests, "XMEGA DES driver test suite");

	// Run all tests in the test suite
	test_suite_run(&des_suite);

	while (1);
}
