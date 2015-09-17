/**
 * \file main.c
 *
 * \brief  Serial Bridge Application
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
 * \page license License
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
