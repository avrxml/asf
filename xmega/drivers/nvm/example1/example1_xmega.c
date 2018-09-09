/**
 * \file
 *
 * \brief NVM example for AVR XMEGA
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
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
