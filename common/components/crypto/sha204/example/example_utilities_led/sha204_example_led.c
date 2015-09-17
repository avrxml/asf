/*
 * \file
 *
 * \brief LED module for ATSHA204 examples
 *
 * This file contains helper functions that use Xplained board LED's to indicate
 * success or failure and byte values (device version for instance).
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdio.h>
#include "asf.h"
#include "compiler.h"
#include "preprocessor.h"
#include "led.h"
#include "sha204_example_led.h"
#include "sha204_timer.h"


/** \brief This function displays a byte value using the Xplained LEDs.
    \param[in] value byte to display
*/
void led_display_number(uint8_t value)
{
   int i;
   int shifter;
   int led[] = {LED0_GPIO, LED1_GPIO, LED2_GPIO, LED3_GPIO, LED4_GPIO, LED5_GPIO, LED6_GPIO, LED7_GPIO};
   
   for (i = 0, shifter = 1; i < 8; i++, shifter <<= 1) {
	   if (shifter & value)
	      LED_On(led[i]);
	   else
	      LED_Off(led[i]);
   }
}


/** 
 * \brief This function displays information using LEDs about the SHA204 devices 
 *        on the Security Xplained board.
 * \param[in] device_present_mask The high nibble represents the four SHA204 devices.
 * \param[in] sha204_revision revision of the SHA204 devices
 */
void display_status(uint8_t device_present_mask, uint8_t sha204_revision)
{
	uint8_t i;
	uint8_t shifter = 1;

	// Find out how many devices are present.
	for (i = 0; shifter < 0x10; shifter <<= 1)
		if (shifter & device_present_mask)
			i++;

	if (i < SHA204_DEVICE_COUNT) {
		// Indicate communication failure.
		// Display the devices found using LED4..7 and error using LED0..3.
		uint8_t i;
		for (i = 0; i < 4; i++) {
			led_display_number(~device_present_mask);
			sha204h_delay_ms(125);
			led_display_number(0);
			sha204h_delay_ms(125);
		}
	}
	else {
		// Indicate success.
		// Display the devices found using LED4..7 and revision of last device using LED0..3.
		led_display_number((device_present_mask << 4) | sha204_revision);
		sha204h_delay_ms(1000);
		led_display_number(0);
		sha204h_delay_ms(1000);
	}	  
}
