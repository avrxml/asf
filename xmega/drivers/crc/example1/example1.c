/**
 * \file
 *
 * \brief CRC example1 for AVR XMEGA
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

 /**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref crc_group by using the I/O
 * interface by using the CPU and writing the data to the DATAIN register.
 *
 * \section files Main Files
 * - crc.c CRC driver implementation
 * - crc.h CRC driver definitions
 * - example1.c example application
 *
 * \section device_info Device Info
 * All AVR XMEGA AU/B/C devices with a CRC module can be used. This example has
 * been tested with the following setup:
 * - XMEGA-B1 Xplained
 * - XMEGA-A3BU Xplained
 *
 * \section description Description of the example
 * The example will generate both CRC-32 and CRC-16 for a data buffer, append
 * the checksums to the data, generate new checksums and check that these are
 * zero. Leds will light up if this is the case.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref crc_group
 * - \ref gpio_group for LED output
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <led.h>

/**
 * \brief Example 1 main application routine
 */
int main(void)
{
	board_init();
	sysclk_init();

	// Initialize the platform LED's.
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);

	uint32_t checksum;
	uint8_t data[15];

	// Randomly selected data
	data[0] = 0xAA;
	data[1] = 0xBB;
	data[2] = 0xCC;
	data[3] = 0xDD;
	data[4] = 0xEE;
	data[5] = 0xFF;

	// Calculate checksum for the data
	checksum = crc_io_checksum((void*)data, 6, CRC_32BIT);
	// Append complemented CHECKSUM registers value to the data, little endian
	crc32_append_value(checksum, data+6);
	// Calculate the new checksum
	checksum = crc_io_checksum((void*)data, 10, CRC_32BIT);

	// Check that the data has not been corrupted, and checksum is zero
	if (checksum == 0) {
		// Turn on LED0
		LED_On(LED0_GPIO);
	}

	// Calculate CRC 16 checksum for the data
	checksum = crc_io_checksum((void*)data, 6, CRC_16BIT);
	// Append the checksum to the data, big endian
	crc16_append_value(checksum, data+6);
	// Calculate the new checksum
	checksum = crc_io_checksum((void*)data, 8, CRC_16BIT);

	// Check that the data has not been corrupted, and checksum is zero
	if (checksum == 0) {
		// Turn on LED1
		LED_On(LED1_GPIO);
	}

	// End of application, loop forever
	while (true) {
		// Intentionally left empty
	}

}
