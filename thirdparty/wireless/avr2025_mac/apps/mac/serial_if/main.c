/**
 * \file main.c
 *
 * \brief MAC Serial Interface Application
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the MAC Serial Interface Application
 * \section main_files Application Files
 *      - main.c                 Application main file.
 * \section intro Application Introduction
 * The Serial Interface Application can be used for two purposes .
 *	 1. For Performing Serial based Interop and Compliance Tests for the MAC
 * Stack and by selecting any of the four configurations
 *              - Beacon FFD
 *              - Beacon RFD
 *              - No-Beacon FFD
 *              - No-Beacon RFD
 *	2. On the other hand it can be used for Running MAC Applications eg.MAC
 * Beacon/No Beacon Application for Two-Processor Boards.
 *      Following are the Four approaches by which a MAC Application can be run.
 *		- Approach-1:Running Beacon/No beacon/ No beacon Sleep
 * application on a single processor platform(Eg.Xmega-a3bu-xplained or
 * uc3-rz600
 *,
 *       Xmega Zigit-usb,RFR2 Xplained Pro)
 *		- Approach 2: Running Beacon/No beacon application for
 * 2p-approach ,with ncp image(Serial-if(Beacon FFD for Beacon App and NoBeacon
 * FFD for NoBeacon App)(MAC Stack)) flashed in NCP board(Eg.Xmega-Zigbit-Ext or
 * RFR2-Zigbit)
 *       and host image(actual application with api-parser support) flashed in
 * Host board(Eg.SAM4L-Xplained Pro).
 *		- Approach3: Running serial-if application in Single processor
 * for  boards  menitioned in Approach 1.This is used for performing
 *       compliance and interop tests for MAC stack.
 *		- Approach 4:Running serial-if application for 2p-approach for
 * boards mentioned in approach 2 used for performing  compliance and
 *       interop tests for MAC stack, ,with ncp image(Serial-if application(MAC
 * Stack)) flashed in NCP board(Eg.Xmega-Zigbit-Ext or RFR2-Zigbit)
 *       and host image(Serial-if application with only api-parser support(MAC
 * Stack not included)) flashed in Host board(Eg.SAM4L-Xplained Pro).
 * \section api_modules Application Dependent Modules
 * - \subpage api
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR .
 * Other compilers may or may not work.
 *
 * \section references References
 * 1)  IEEE Std 802.15.4-2006 Part 15.4: Wireless Medium Access Control (MAC)
 *     and Physical Layer (PHY) Specifications for Low-Rate Wireless Personal
 * Area
 *     Networks (WPANs).\n\n
 * 2)  <a href="http://www.atmel.com/design-support/">Atmel Design
 * Support</a>.\n
 *
 * \section contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.atmel.com/avr">www.atmel.com</A>.\n
 */

#include "conf_board.h"
#include "avr2025_mac.h"
#include "serial_interface.h"
#include "common_sw_timer.h"
#include <asf.h>
#if SAMD || SAMR21
#include "system.h"
#endif

/** Alert to indicate something has gone wrong in the application */
static void app_alert(void);

#define LED_POWER     (LED0)

/*! \brief Main function.
 */
int main(void)
{
	irq_initialize_vectors();
#if SAMD || SAMR21
	system_init();
	delay_init();
#else
	sysclk_init();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
#endif

	sw_timer_init();
	serial_interface_init();

	if (MAC_SUCCESS != wpan_init()) {
		app_alert();
	}

	LED_On(LED_POWER);
	cpu_irq_enable();

	while (1) {
		wpan_task();
		serial_data_handler();
	}
}

/* Alert to indicate something has gone wrong in the application */
static void app_alert(void)
{
	while (1) {
		#if LED_COUNT > 0
		LED_Toggle(LED0);
		#endif

		#if LED_COUNT > 1
		LED_Toggle(LED1);
		#endif

		#if LED_COUNT > 2
		LED_Toggle(LED2);
		#endif

		#if LED_COUNT > 3
		LED_Toggle(LED3);
		#endif

		#if LED_COUNT > 4
		LED_Toggle(LED4);
		#endif

		#if LED_COUNT > 5
		LED_Toggle(LED5);
		#endif

		#if LED_COUNT > 6
		LED_Toggle(LED6);
		#endif

		#if LED_COUNT > 7
		LED_Toggle(LED7);
		#endif
		delay_us(0xFFFF);
	}
}
