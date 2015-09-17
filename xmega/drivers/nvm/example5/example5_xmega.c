/**
 * \file
 *
 * \brief NVM example for AVR XMEGA
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <conf_example.h>
#include <asf.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example shows how to use and tests the user signature access of the
 * \ref nvm_flash_group "NVM flash handling" of the \ref nvm_group.
 *
 * \section files Main Files
 * - example5_xmega.c example application
 * - conf_example.h example configuration
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used. This example have
 * been tested with the following setup:
 * - Xplain
 * - XMEGA A1 Xplained
 *
 * \section description Description of the example
 *
 * The example tests the user signature row functions and reports back the
 * status on the Xplain LEDs:
 * - LED0: Write with erase check works as expected.
 * - LED1: Write without erase check works as expected.
 *         Does not give reliable result if the first test fails.
 * - LED2: Read user signature row works as expected.
 *         Does not give reliable result if the first test fails.
 * - LED7: Test completed
 *
 * If one of the LEDs does not light up, this indicates a failure on the
 * test.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref gpio_group for LED output
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

/**
 * \defgroup nvm_example_group NVM driver example functions
 *
 * @{
 */

#define BUFFER_SIZE               16

#define FLASH_ERASED              0xff

#define TEST_ADDR                 0

/**
 * Memory buffer to use during testing
 */
uint8_t buffer[BUFFER_SIZE];


/**
 * Set all values of a memory buffer to a given value
 */
static void set_buffer(uint8_t *buffer, uint8_t value)
{
	uint8_t i;

	for (i = 0; i < BUFFER_SIZE; i++) {
		buffer[i] = value;
	}
}

/**
 * Check if an EEPROM page is equal to a memory buffer
 */
static bool is_user_sig_equal_to_buffer(flash_addr_t address, uint8_t *buffer, uint8_t len)
{
	uint8_t i;

	for (i = 0; i < len; i++) {
		if (nvm_read_user_signature_row(address + i) != buffer[i]) {
			return false;
		}
	}

	return true;
}


/**
 * Test nvm_user_sig_write_buffer().
 *
 * Test procedure:
 * - Write to user signature row with automatic erasing enabled
 * - Verify contents is equal to buffer
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_write_and_erase(void)
{
	// Clear memory buffer
	set_buffer(buffer, FLASH_ERASED);

	// Set some test values
	buffer[0] = 84;
	buffer[1] = 101;
	buffer[2] = 115;
	buffer[3] = 116;

	// Write test values to user signature row
	nvm_user_sig_write_buffer(TEST_ADDR, &buffer, BUFFER_SIZE, true);

	if (is_user_sig_equal_to_buffer(TEST_ADDR, buffer, BUFFER_SIZE)) {
		return STATUS_OK;
	}

	return ERR_BAD_DATA;
}

/**
 * Test nvm_user_sig_write_buffer().
 *
 * Test procedure:
 * - Erase and write to user signature row with some known values
 * - Write other values to user signature row with automatic erasing disabled
 * - Verify contents is NOT equal to buffer
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_write_no_erase(void)
{
	// Clear memory buffer
	set_buffer(buffer, FLASH_ERASED);

	// Set some test values
	buffer[0] = 0xAA;
	buffer[1] = 0xEE;
	buffer[2] = 0xAA;
	buffer[3] = 0xEE;

	// Erase and write data to user signature row:
	nvm_user_sig_write_buffer(TEST_ADDR, &buffer, BUFFER_SIZE, true);

	// Clear memory buffer
	set_buffer(buffer, FLASH_ERASED);

	// Set some other test values
	buffer[0] = 0xCA;
	buffer[1] = 0xFE;
	buffer[2] = 0xBA;
	buffer[3] = 0xBE;

	// Write test values to user signature row without erasing:
	nvm_user_sig_write_buffer(TEST_ADDR, &buffer, BUFFER_SIZE, false);

	// Verify that contents is not equal:
	if (is_user_sig_equal_to_buffer(TEST_ADDR, buffer, BUFFER_SIZE)) {
		return ERR_BAD_DATA;
	}

	return STATUS_OK;
}

/**
 * Test nvm_user_sig_read_buffer().
 *
 * Test procedure:
 * - Write to user signature row
 * - Read back with nvm_user_sig_read_buffer().
 * - Verify contents
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_read(void)
{
	uint8_t i;
	uint8_t mybuffer[BUFFER_SIZE];

	// Clear memory buffer
	set_buffer(buffer, FLASH_ERASED);

	// Set some test values
	buffer[0] = 0xB0;
	buffer[1] = 0x0B;
	buffer[2] = 0xB0;
	buffer[3] = 0x0B;
	buffer[4] = 0xB0;
	buffer[5] = 0x0B;

	// Erase and write test values to user signature row
	nvm_user_sig_write_buffer(TEST_ADDR, &buffer, BUFFER_SIZE, true);

	// Read back
	nvm_user_sig_read_buffer(TEST_ADDR, mybuffer, BUFFER_SIZE);

	// Verify
	for (i=0; i < BUFFER_SIZE; i++) {
		if (buffer[i] != mybuffer[i]) {
			return ERR_BAD_DATA;
		}
	}

	return STATUS_OK;
}


int main(void)
{
	board_init();

	if (test_write_and_erase() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_0);
	}

	if (test_write_no_erase() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_1);
	}

	if (test_read() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_2);
	}

	/* Toggle LED to indicate that we are done */
	gpio_toggle_pin(LED_PIN_7);

	while (true) {}

}

//! @}

