/**
 * \file
 *
 * \brief Main functions
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

