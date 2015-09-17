/**
 * \file
 *
 * \brief AT25DFx SerialFlash Component Example.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
 * \mainpage AT25 SerialFlash example
 *
 * \section Purpose
 *
 * This example demonstrates how to use the Atmel SerialFlash AT25DFx component.
 * The SerialFlash requires an SPI bus as a low level communication interface. This
 * interface can be implemented using SPI or USART in SPI mode for SAM devices.
 *
 * \section Requirements
 *
 * This package can be used with SAM3N, SAM4L, SAM4E and SAM4C evaluation kits.
 *
 * \section Description
 *
 * This example will do the following operations.
 *   - Send "Read Status" command to the SerialFlash.
 *   - Read back the status of the SerialFlash.
 *   - Write two sectors to the SerialFlash.
 *   - Read back these sectors and check correct content.
 *   - If all operations are correct, \ref DATA_FLASH_LED_EXAMPLE_0 is 'on' and
 *     \ref DATA_FLASH_LED_EXAMPLE_1 is 'on'(If have).
 *     The other states of  LEDs mean that the SerialFlash access fails somewhere.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board. Please
 *    refer to the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6224.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *    GNU-Based Software Development</a> application note or the
 *    <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">
 *    IAR EWARM User Guide</a>, depending on the solutions that users choose.
 * -# Some texts, images and basic shapes should be displayed on the LCD.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_board.h"

/** Test size */
#define AT25DFX_TEST_DATA_SIZE   (1024)

/** Test block start address */
#define AT25DFX_TEST_BLOCK_ADDR  (0)

/** RAM buffer used in this example */
static uint8_t ram_buff[AT25DFX_TEST_DATA_SIZE];

/**
 * \brief Entry point when test is failed.
 */
void test_ko(void);

/**
 * \brief Application entry point for AT25DFx example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint16_t i;

	sysclk_init();
	board_init();

	/* Initialize the SerialFlash */
	at25dfx_initialize();

	/* Set the SerialFlash active */
	at25dfx_set_mem_active(AT25DFX_MEM_ID);

	/* Unprotect the chip */
	if (at25dfx_protect_chip(AT25_TYPE_UNPROTECT) == AT25_SUCCESS) {
		LED_On(DATA_FLASH_LED_EXAMPLE_0);
	} else {
		test_ko();
	}

	/* Check if the SerialFlash is valid */
	if (at25dfx_mem_check() == AT25_SUCCESS) {
		LED_On(DATA_FLASH_LED_EXAMPLE_0);
	} else {
		test_ko();
	}

	/* Prepare half of the SerialFlash sector as 0xAA */
	for (i = 0; i < AT25DFX_TEST_DATA_SIZE / 2; i++) {
		ram_buff[i] = 0xAA;
	}

	/* And the remaining half as 0x55 */
	for (; i < AT25DFX_TEST_DATA_SIZE; i++) {
		ram_buff[i] = 0x55;
	}

	/* Erase the block before write */
	at25dfx_erase_block(AT25DFX_TEST_BLOCK_ADDR);

	/* Write the data to the SerialFlash */
	at25dfx_write(ram_buff, AT25DFX_TEST_DATA_SIZE, AT25DFX_TEST_BLOCK_ADDR);

	/* Read back this sector and compare them with the expected values */
	at25dfx_read(ram_buff, AT25DFX_TEST_DATA_SIZE, AT25DFX_TEST_BLOCK_ADDR);

	for (i = 0; i < AT25DFX_TEST_DATA_SIZE / 2; i++) {
		if (ram_buff[i] != 0xAA) {
			test_ko();
		}
	}
	for (; i < AT25DFX_TEST_DATA_SIZE; i++) {
		if (ram_buff[i] != 0x55) {
			test_ko();
		}
	}

	/* Write one SerialFlash sector as 0x00, 0x01 .... */
	for (i = 0; i < AT25DFX_TEST_DATA_SIZE; i++) {
		ram_buff[i] = i;
	}

	/* Erase the block before write */
	at25dfx_erase_block(AT25DFX_TEST_BLOCK_ADDR);

	/* Write the data to the SerialFlash */
	at25dfx_write(ram_buff, AT25DFX_TEST_DATA_SIZE, AT25DFX_TEST_BLOCK_ADDR);

	/* Read back this sector and compare them with the expected values */
	at25dfx_read(ram_buff, AT25DFX_TEST_DATA_SIZE, AT25DFX_TEST_BLOCK_ADDR);

	for (i = 0; i < AT25DFX_TEST_DATA_SIZE; i++) {
		if (ram_buff[i] != (i % 0x100)) {
			test_ko();
		}
	}

	LED_On(DATA_FLASH_LED_EXAMPLE_1);
	while (1);
}

void test_ko(void)
{
	LED_Off(DATA_FLASH_LED_EXAMPLE_1);
	while (1);
}
