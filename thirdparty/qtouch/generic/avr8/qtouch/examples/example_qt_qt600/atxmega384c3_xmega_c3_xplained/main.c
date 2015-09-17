/**
 * \file
 *
 * \brief Main functions
 *
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

#include <asf.h>
#include "app_touch.h"

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	static bool qtb0_last_state = false;
	static bool qtb1_last_state = false;
	bool qtb_state;

	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize ASF services
	sleepmgr_init();
	sysclk_init();
	board_init();

	// Initialize sub task
	app_touch_init();

	// The main loop
	while (true) {
		// Enter in sleep mode
		sleepmgr_enter_sleep();

		// Check state of QTouch buttons
		qtb_state = app_touch_check_key_pressed(0);
		if (qtb_state != qtb0_last_state) {
			// QTouch button have changed
			qtb0_last_state = qtb_state;
			if (!qtb_state) {
				LED_On(LED0_GPIO);
			} else {
				LED_Off(LED0_GPIO);
			}
		}
		qtb_state = app_touch_check_key_pressed(1);
		if (qtb_state != qtb1_last_state) {
			// QTouch button have changed
			qtb1_last_state = qtb_state;
			if (!qtb_state) {
				LED_On(LED1_GPIO);
			} else {
				LED_Off(LED1_GPIO);
			}
		}
	}
}

/**
 * \mainpage ASF Qtouch example on XMEGA-C3 Xplained
 *
 * \section intro Introduction
 * This example shows how to implement the Qtouch library and Qtouch Debug
 * on XMEGA-C3 Xplained board. The Qtouch is avtivated on button QTB0 and QTB0.
 *
 * \section QtouchDebug QTouch Debug
 * The Qtouch Debug is available through BitBangSPI interface on PORTC (4,5,6,7).
 * The QT600 Bridge must be used to connect XMEGA-C3 Xplained board with the
 * QTouch Analyser tool.
 *
 * \section QT600 QT600 Setting
 *
 * Device Fuse Settings
 * The default fuse settings for the device will be correct for the
 * QT600 Example Project. However it is adviceable to verify
 * the fuse settings before trying to run the application.
 * 
 * WDWP:       8 cycles (8ms @ 3.3V)
 * WDP:        8 cycles (8ms @ 3.3V)
 * BOOTRST:	Application Reset
 * BODACT:     BOD Disabled
 * BODPD:      BOD Disabled
 * JTAGEN:     Programmed
 * 
 * Fusebyte0: 0xFF
 * Fusebyte1: 0x00
 * Fusebyte2: 0xFF
 * Fusebyte4: 0xFE
 * Fusebyte5: 0xFF
 */

