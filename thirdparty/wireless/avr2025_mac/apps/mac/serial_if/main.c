/**
 * \file main.c
 *
 * \brief MAC Serial Interface Application
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 * 2)  <a href="https://www.microchip.com/support/">Atmel Design
 * Support</a>.\n
 *
 * \section contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.microchip.com/design-centers/8-bit">www.atmel.com</A>.\n
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
