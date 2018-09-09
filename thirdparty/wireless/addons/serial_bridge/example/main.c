/**
 * \file main.c
 *
 * \brief  Serial Bridge Application
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
 * \page license License
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/**
 * \mainpage Serial Bridge Application
 * \section preface Preface
 * This is the reference manual for Serial Bridge application
 * \section main_files Application Files
 * - main.c      Application main file.
 * \section intro Application Introduction
 * The serial Bridge Application is used in the host which acts as a bridge
 * between the Pc and the NCP device.
 * The serial Bridge application is used by Performance Analyzer application in
 * 2p approach
 * and for flashing image using Bootloader application,where it transfers data
 * from the Pc to the NCP and vice-versa.
 */

/* === INCLUDES ============================================================ */

#include <stdlib.h>
#include "serial_bridge.h"
#include "asf.h"
#if SAMD || SAMR21
#include "system.h"
#endif

/* === PROTOTYPES
 *=============================================================== */

void app_alert(void);

/* === GLOBALS ============================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * \brief Main function of the Serial Bridge application
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

	cpu_irq_enable();

	serial_bridge_init();

	while (1) {
		serial_bridge_handler();
	}
}

void app_alert()
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

/* EOF */
