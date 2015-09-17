/*
 * \file
 *
 * \brief ATSHA204 example module
 *
 * This file contains the main function and some helper functions.
 *
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
 * \mainpage ATSHA204 Example 1 for Security Xplained
 *
 * \par Purpose
 *
 * Basic communication example of an ATSHA204 component.
 *
 * \par Description
 *
 * In an infinite loop, sends a DevRev command to all four ATSHA204 devices
 * and indicates success or failure for one second using the main board LEDs.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdint.h>
#include <string.h>

#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "sysclk.h"
#include "sha204_example_led.h"
#include "sha204_timer.h"
#include "sha204_command_marshaling.h"
#include "sha204_lib_return_codes.h"
#include "security_xplained.h"


/** \brief This function is the entry function for an example application that
           uses the SHA204 ASF component.
 * \return result (0: success, otherwise failure)
 */
int main(void)
{
	uint8_t tx_buffer_command[SHA204_CMD_SIZE_MIN];
	uint8_t rx_buffer[DEVREV_RSP_SIZE];
	uint8_t sha204_lib_return;
	uint8_t i;
	uint8_t device_present_mask;
	uint8_t sha204_revision;

    sysclk_init();

	// Initialize the board.
	board_init();

	// Indicate entering main loop.
	led_display_number(0xFF);
	sha204h_delay_ms(1000);
	led_display_number(0x00);

	// The main loop wakes up a device, retrieves its revision, and puts it
	// back to sleep. It does this for all the SHA204 devices on the
	// Security Xplained extension board except the one that is used for identifying
	// an extension board.
	while (true) {
		device_present_mask = sha204_revision = 0;
		// Generate Wakeup pulse. All SHA204 devices that share SDA will wake up.
		sha204_lib_return = sha204p_wakeup();
	    if (sha204_lib_return != SHA204_SUCCESS) {
			// Indicate Wakeup failure.
			for (i = 0; i < 8; i++) {
				led_display_number(0xFF);
				sha204h_delay_ms(50);
				led_display_number(0x00);
				sha204h_delay_ms(50);
			}
			continue;		 
	    }

        // Read the revision from all devices and put them to sleep.
		struct sha204_dev_rev_parameters dev_rev = {
				.rx_buffer = rx_buffer, .tx_buffer = tx_buffer_command};
		for (i = 0; i < SHA204_DEVICE_COUNT; i++) {
			sha204p_set_device_id(sha204_i2c_address(i));
			memset(rx_buffer, 0, sizeof(rx_buffer));
	   
			// Send DevRev command and receive its response.
			sha204_lib_return = sha204m_dev_rev(&dev_rev);
			if (sha204_lib_return != SHA204_SUCCESS)
				continue;
			
			// Store result.
			device_present_mask |= (1 << i);
			sha204_revision = rx_buffer[SHA204_BUFFER_POS_DATA + 3];

			// Send Sleep command.
			sha204p_sleep();
		}

		display_status(device_present_mask, sha204_revision);
	}	

	return sha204_lib_return;
}
