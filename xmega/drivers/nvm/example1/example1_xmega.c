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
 * This example shows how to use the
 * \ref nvm_signature_group "NVM signature handling" of the \ref nvm_group.
 *
 * \section files Main Files
 * - example1_xmega.c example application
 * - conf_example.h example configuration
 *
 * \section device_info Device Info
 * All AVR XMEGA devices can be used. This example have
 * been tested with the following setup:
 * - Xplain * - XMEGA A1 Xplained
 * - XMEGA A3BU Xplained
 *
 * \section description Description of the example
 *
 * The example uses the \ref nvm_signature_group "NVM signature driver" to
 * read the device signature and the production signature and outputs the
 * status on the Xplain LEDs:
 * - LED0: Signature byte read from device matches the compiled test program
 * - LED1: Production signature row has been read, and contains data
 * - LED7: Test completed
 * On the A3BU Xplained, test completed is shown by the status LED being green.
 *
 * If either LED0 or LED1 does not light up, this indicates a failure on these
 * tests.
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

#if defined(__GNUC__)
#  define CHIP_DEVID0 SIGNATURE_0
#  define CHIP_DEVID1 SIGNATURE_1
#  define CHIP_DEVID2 SIGNATURE_2
#elif defined(__ICCAVR__)
#  define CHIP_DEVID0 SIGNATURE_000
#  define CHIP_DEVID1 SIGNATURE_001
#  define CHIP_DEVID2 SIGNATURE_002
#else
#  error Unknown compiler
#endif

//! Storage for device id
struct nvm_device_id device_id;
//! Storage for serial number
struct nvm_device_serial serial;

int main(void)
{
	uint8_t counter;

	board_init();

	nvm_read_device_id(&device_id);

#if (BOARD == XMEGA_A3BU_XPLAINED)
	/* We use the status led to show that the test is
	 * complete, so turn it off now.
	 */
	ioport_set_pin_high(LED3_GPIO);
#endif

	// Check if actual device matches what the program was compiled for
	if ((device_id.devid0 == CHIP_DEVID0 ) &&
			(device_id.devid1 == CHIP_DEVID1 ) &&
			(device_id.devid2 == CHIP_DEVID2)) {

		// Set LED to indicate that signature matches.
		gpio_toggle_pin(LED_PIN_0);
	}

	nvm_read_device_serial(&serial);

	// Check that at least one of the bytes in the signature is not FF
	for (counter = 0; counter < sizeof(serial.byte); counter++) {
		if (serial.byte[counter] != 0xff) {
			// Set LED to indicate that we have found a byte that is not FF
			gpio_toggle_pin(LED_PIN_1);
			// Abort searching
			break;
		}
	}

	// Indicate that we are done
#if (BOARD == XMEGA_A3BU_XPLAINED)
	gpio_toggle_pin(LED_PIN_3);
#else
	gpio_toggle_pin(LED_PIN_7);
#endif

	while (true) {}

}
