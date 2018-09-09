/**
 * \file
 *
 * \brief NVM example for AVR XMEGA
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
#include <board.h>
#include <compiler.h>
#include <gpio.h>
#include <sysclk.h>
#include <nvm.h>
#include <status_codes.h>
#include <conf_example.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example shows how to use and tests the
 * \ref nvm_flash_group "NVM Flash handling" of the \ref nvm_group.
 *
 * \section files Main Files
 * - example3_xmega.c example application
 * - conf_example.h example configuration
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used. This example have
 * been tested with the following setup:
 * - Xplain * - XMEGA A1 Xplained
 * - XMEGA-A3BU Xplained
 *
 * \section description Description of the example
 *
 * The example tests the flash and reports back the status on
 * the Xplain/A1 Xplained LEDs:
 * - LED0: Atomic erase and write of application table section works as expected
 * - LED1: Write without erase of application table section works as expected
 * - LED2: Atomic erase and write of boot section works as expected
 * - LED3: Write without erase of boot section works as expected
 * - LED7: Test completed
 *
 * In the case of the A3BU Xplained:
 * - LED0: Atomic erase and write of application table section works as expected
 * - LED1: Write without erase of application table section works as expected
 * - LED2 (Red): Atomic erase and write of boot section works as expected
 * - LED3 (Green): Write without erase of boot section works as expected
 * - LCD Backlight: Test completed
 * (Successful tests on LED2 and LED3 will appear as an orange color)
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
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */

/**
 * \defgroup nvm_example_group NVM driver example functions
 *
 * @{
 */

#define FLASH_ERASED 0xffff

/**
 * Memory buffer to use during testing
 */
uint16_t buffer[FLASH_PAGE_SIZE / 2];

/**
 * Check if an flash page is equal to a flash page buffer in SRAM
 */
static bool is_flash_page_equal_to_buffer(flash_addr_t page_addr,
		uint16_t *buffer)
{
	uint16_t i;

	for (i = 0; i < FLASH_PAGE_SIZE / 2; i++) {
		if (nvm_flash_read_word(page_addr) != buffer[i]) {
			return false;
		}
		page_addr += 2;
	}

	return true;
}

/**
 * Set all values of a flash page buffer in SRAM to a given value
 */
static void set_buffer(uint16_t *buffer, uint16_t value)
{
	uint16_t i;

	for (i = 0; i < FLASH_PAGE_SIZE / 2; i++) {
		buffer[i] = value;
	}
}

/**
 * \brief Fill page buffer with a sequence of numbers
 *
 * The function fills the flash page buffer with a sequence of number,
 * and stores the same numbers in a buffer in SRAM. The buffer is used
 * to verify that the flash has been programmed correctly. A seed number
 * is used to ensure that different pages always will be different.
 *
 * \param buffer SRAM buffer to store values written to the flash page buffer
 * \param seed Offset number to use on stored values.
 */
static void fill_flash_page_buffer(uint16_t *buffer,
		uint16_t seed)
{
	uint16_t addr;
	uint16_t value;

	for (addr = 0; addr < FLASH_PAGE_SIZE / 2; addr++) {
		value = seed - addr;
		buffer[addr] = value;
		nvm_flash_load_word_to_buffer(addr * 2, value);
	}
}

/**
 * Test nvm_flash_load_word_to_buffer() and nvm_flash_atomic_write_app_page().
 *
 * Test procedure:
 * - Fill page buffer.
 * - Erase and write page to application table section
 * - Verify each byte written correctly by comparing each word in flash
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_atomic_write_app_table(void)
{
	flash_addr_t page_addr;

	page_addr = APPTABLE_SECTION_START + 2 * FLASH_PAGE_SIZE;

	set_buffer(buffer, FLASH_ERASED);
	fill_flash_page_buffer(buffer, 0xA37A);

	nvm_flash_atomic_write_app_page(page_addr);
	if (!is_flash_page_equal_to_buffer(page_addr, buffer)) {
		return ERR_BAD_DATA;
	}

	return STATUS_OK;
}

/**
 * Test nvm_flash_load_word_to_buffer(), nvm_flash_erase_app_page() and
 * nvm_flash_split_write_app_page().
 *
 * Test procedure:
 * - Erase page in application table section.
 * - Verify the page is erased by comparing each word in flash.
 * - Write page to application table section
 * - Verify each byte written correctly by comparing each word in flash
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_split_write_app_table(void)
{
	flash_addr_t page_addr;

	page_addr = APPTABLE_SECTION_START + 3 * FLASH_PAGE_SIZE;

	set_buffer(buffer, FLASH_ERASED);
	nvm_flash_erase_app_page(page_addr);

	if (!is_flash_page_equal_to_buffer(page_addr, buffer)) {
		return ERR_BAD_DATA;
	}

	fill_flash_page_buffer(buffer, 0x317A);

	nvm_flash_split_write_app_page(page_addr);
	if (!is_flash_page_equal_to_buffer(page_addr, buffer)) {
		return ERR_BAD_DATA;
	}

	return STATUS_OK;
}

/**
 * Test nvm_flash_load_word_to_buffer() and nvm_flash_atomic_write_boot_page().
 *
 * Test procedure:
 * - Fill page buffer.
 * - Erase and write page to bootloader section
 * - Verify each byte written correctly by comparing each word in flash
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_atomic_write_boot(void)
{
	flash_addr_t page_addr;

	page_addr = BOOT_SECTION_START + 1 * FLASH_PAGE_SIZE;

	set_buffer(buffer, FLASH_ERASED);
	fill_flash_page_buffer(buffer, 0xC1BA);

	nvm_flash_atomic_write_boot_page(page_addr);
	if (!is_flash_page_equal_to_buffer(page_addr, buffer)) {
		return ERR_BAD_DATA;
	}

	return STATUS_OK;
}

/**
 * Test nvm_flash_load_word_to_buffer(), nvm_flash_erase_boot_page() and
 * nvm_flash_split_write_boot_page().
 *
 * Test procedure:
 * - Erase page in boot section.
 * - Verify the page is erased by comparing each word in flash.
 * - Write page to boot section
 * - Verify each byte written correctly by comparing each word in flash
 *
 * \return STATUS_OK if test succeeded, otherwise ERR_BAD_DATA
 */
static status_code_t test_split_write_boot(void)
{
	flash_addr_t page_addr;

	page_addr = BOOT_SECTION_START + 2 * FLASH_PAGE_SIZE;

	set_buffer(buffer, FLASH_ERASED);
	nvm_flash_erase_boot_page(page_addr);

	if (!is_flash_page_equal_to_buffer(page_addr, buffer)) {
		return ERR_BAD_DATA;
	}

	fill_flash_page_buffer(buffer, 0x7CAF);

	nvm_flash_split_write_boot_page(page_addr);
	if (!is_flash_page_equal_to_buffer(page_addr, buffer)) {
		return ERR_BAD_DATA;
	}

	return STATUS_OK;
}

int main(void)
{
	board_init();

#if (BOARD == XMEGA_A3BU_XPLAINED)
	/* Turn off status LED and back light,
	 * as it is used to show success of tests. */
	ioport_set_pin_high(LED3_GPIO);

#endif

	if (test_atomic_write_app_table() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_0);
	}

	if (test_split_write_app_table() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_1);
	}

	if (test_atomic_write_boot() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_2);
	}

	if (test_split_write_boot() == STATUS_OK) {
		/* Toggle LED to indicate success */
		gpio_toggle_pin(LED_PIN_3);
	}

#if (BOARD == XMEGA_A3BU_XPLAINED)
	/* Turn on the LCD back light to show that we are done */
	ioport_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
#else
	/* Turn on LED 7 to show that we are done */
	gpio_toggle_pin(LED_PIN_7);
#endif

	while (true) {}

}

//! @}
