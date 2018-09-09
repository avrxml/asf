/**
 * \file
 *
 * \brief CRC example1 for AVR XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
