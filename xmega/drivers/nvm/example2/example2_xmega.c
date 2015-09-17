/**
 * \file
 *
 * \brief NVM example for AVR XMEGA
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
#include <conf_example.h>
#include <asf.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example shows how to use and tests the
 * \ref nvm_eeprom_group "NVM EEPROM handling" of the \ref nvm_group.
 *
 * \section files Main Files
 * - example2_xmega.c example application
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
 * The example tests the EEPROM and reports back the status on the Xplain LEDs:
 * - LED0: Erasing of all pages works as expected
 * - LED1: Write functions work as expected
 * - LED2: Atomic erase and write works as expected
 * - LED3: Write without an erase works as expected
 * - LED4: Erase of a few bytes in a page works as expected
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

#define EEPROM_ERASED             0xff

#define TEST_ERASE_PAGE           2
#define TEST_WRITE_PAGE           1
#define TEST_ATOMIC_WRITE_PAGE    2
#define TEST_SPLIT_WRITE_PAGE     3
#define TEST_ERASE_BYTES_PAGE_1   4
#define TEST_ERASE_BYTES_PAGE_2   5

/**
 * Memory buffer to use during testing
 */
uint8_t buffer[EEPROM_PAGE_SIZE];

/**
 * Check if an EEPROM page is equal to a memory buffer
 */
static bool is_eeprom_page_equal_to_buffer(uint8_t page_addr, uint8_t *buffer)
{
	uint8_t i;

	for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
		if (nvm_eeprom_read_byte(page_addr * EEPROM_PAGE_SIZE + i) != buffer[i]) {
			return false;
		}
	}

	return true;
}

/**
 * Set all values of a memory buffer to a given value
 */
static void set_buffer(uint8_t *buffer, uint8_t value)
{
	uint8_t i;

	for (i = 0; i < EEPROM_PAGE_SIZE; i++) {
		buffer[i] = value;
	}
}

/**
 * Test nvm_eeprom_erase_all().
 *
 * Test procedure:
 * - Write one byte to the TEST_ERASE_PAGE page.
 * - Erase all memory locations
 * - Verify that the EEPROM is erased by a spot test on the
 *   TEST_ERASE_PAGE page.
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_erase(void)
{
	nvm_eeprom_write_byte(TEST_ERASE_PAGE * EEPROM_PAGE_SIZE, 42);
	nvm_eeprom_erase_all();
	set_buffer(buffer, EEPROM_ERASED);

	if (is_eeprom_page_equal_to_buffer(TEST_ERASE_PAGE, buffer)) {
		return STATUS_OK;
	}

	return ERR_BAD_DATA;
}

/**
 * Test nvm_eeprom_erase_page() and nvm_eeprom_write_byte().
 *
 * Test procedure:
 * - Erase TEST_WRITE_PAGE
 * - Write one byte
 * - Verify that the byte is written and that all other bytes are erased.
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_write(void)
{
	// Test write
	nvm_eeprom_erase_page(TEST_WRITE_PAGE);

	set_buffer(buffer, EEPROM_ERASED);
	buffer[0] = 0xaa;
	nvm_eeprom_write_byte(TEST_WRITE_PAGE*EEPROM_PAGE_SIZE, buffer[0]);

	if (is_eeprom_page_equal_to_buffer(TEST_WRITE_PAGE, buffer)) {
		return STATUS_OK;
	}

	return ERR_BAD_DATA;
}

/**
 * Test nvm_eeprom_load_byte_to_buffer(), nvm_eeprom_flush_buffer() and
 * nvm_eeprom_atomic_write_page()
 *
 * Test procedure:
 * - Erase the TEST_ATOMIC_WRITE_PAGE page.
 * - Load byte and then flush the buffer at one location.
 * - Load two more bytes at a different location.
 * - Write the page.
 * - Verify that only two bytes are written and the rest of the page is erased.
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_atomic_write(void)
{
	nvm_eeprom_erase_page(TEST_ATOMIC_WRITE_PAGE);

	/* Load a dummy byte and then flush the buffer to remove the byte from
	 * the buffer. The byte should not be written to the EEPROM.
	 */
	nvm_eeprom_load_byte_to_buffer(0, 55);
	nvm_eeprom_flush_buffer();

	/* Load some additional bytes */
	set_buffer(buffer, EEPROM_ERASED);
	buffer[1] = 0xaa;
	buffer[2] = 0x19;
	nvm_eeprom_load_byte_to_buffer(1, buffer[1]);
	nvm_eeprom_load_byte_to_buffer(2, buffer[2]);
	/* Erase and then write page */
	nvm_eeprom_atomic_write_page(TEST_ATOMIC_WRITE_PAGE);

	if (is_eeprom_page_equal_to_buffer(TEST_ATOMIC_WRITE_PAGE, buffer)) {
		return STATUS_OK;
	}

	return ERR_BAD_DATA;
}

/**
 * Test nvm_eeprom_split_write_page()
 *
 * Test procedure:
 * - Erase TEST_SPLIT_WRITE_PAGE page and then write 2 bytes
 *   to TEST_SPLIT_WRITE_PAGE by using an atomic write
 * - Write 1 additional byte at another location using a split write
 * - Verify that all 3 bytes written correctly and that the rest of the page
 *   is erased.
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_split_write(void)
{
	/* Erase and then program two bytes into the page */
	set_buffer(buffer, EEPROM_ERASED);
	buffer[1] = 0xaa;
	buffer[2] = 0x19;
	nvm_eeprom_load_byte_to_buffer(1, buffer[1]);
	nvm_eeprom_load_byte_to_buffer(2, buffer[2]);
	nvm_eeprom_atomic_write_page(TEST_SPLIT_WRITE_PAGE);

	/*
	 * Write another value to the EEPROM and then verify that all three bytes
	 * are present, ie. the split write does not delete the first two bytes.
	 */
	buffer[3] = 0xa7;
	nvm_eeprom_load_byte_to_buffer(3, buffer[3]);
	nvm_eeprom_split_write_page(TEST_SPLIT_WRITE_PAGE);

	if (is_eeprom_page_equal_to_buffer(TEST_SPLIT_WRITE_PAGE, buffer)) {
		return STATUS_OK;
	}

	return ERR_BAD_DATA;
}

/**
 * Test nvm_eeprom_erase_bytes_in_page() and
 * nvm_eeprom_erase_bytes_in_all_pages()
 *
 * Test procedure:
 * - Write two bytes into page TEST_ERASE_BYTES_PAGE_1 and
 *   TEST_ERASE_BYTES_PAGE_2
 * - Load value to page buffer in the address of the first byte
 * - Erase first byte of TEST_ERASE_BYTES_PAGE_1
 * - Verify that first byte is deleted in TEST_ERASE_BYTES_PAGE_1
 * - Load value to page buffer in the address of the first byte
 * - Erase first byte of all pages
 * - Verify that first byte is deleted in TEST_ERASE_BYTES_PAGE_2
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_erase_bytes(void)
{
	set_buffer(buffer, EEPROM_ERASED);
	buffer[0] = 0xaa;
	buffer[1] = 0xaf;

	/* Write two bytes into first page */
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_1 * EEPROM_PAGE_SIZE + 0, buffer[0]);
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_1 * EEPROM_PAGE_SIZE + 1, buffer[1]);

	/* Write two bytes into second page */
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_2 * EEPROM_PAGE_SIZE + 0, buffer[0]);
	nvm_eeprom_write_byte(TEST_ERASE_BYTES_PAGE_2 * EEPROM_PAGE_SIZE + 1, buffer[1]);

	/* Erase first byte of the first page */
	nvm_eeprom_load_byte_to_buffer(0, 0xff);
	nvm_eeprom_erase_bytes_in_page(TEST_ERASE_BYTES_PAGE_1);
	buffer[0] = EEPROM_ERASED;

	if (is_eeprom_page_equal_to_buffer(TEST_ERASE_BYTES_PAGE_1, buffer)) {
		/* Erase first byte of all pages */
		nvm_eeprom_load_byte_to_buffer(0, 0xff);
		nvm_eeprom_erase_bytes_in_all_pages();

		if (is_eeprom_page_equal_to_buffer(TEST_ERASE_BYTES_PAGE_2, buffer)) {
			return STATUS_OK;
		}
	}

	return ERR_BAD_DATA;
}

int main(void)
{
	board_init();

	if (test_erase() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_0);
	}

	if (test_write() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_1);
	}

	if (test_atomic_write() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_2);
	}

	if (test_split_write() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_3);
	}

	if (test_erase_bytes() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_4);
	}

	/* Toggle LED to indicate that we are done */
	gpio_toggle_pin(LED_PIN_7);

	while (true) {}

}

//! @}

