/**
 * \file
 *
 * \brief User Interface
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

#include <asf.h>
#include "ui.h"

void ui_init(void)
{
	LED_On(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
	LED_Off(LED4_GPIO);
	LED_Off(LED5_GPIO);
	LED_Off(LED6_GPIO);
	LED_Off(LED7_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
	LED_Off(LED4_GPIO);
	LED_Off(LED5_GPIO);
	LED_Off(LED6_GPIO);
	LED_Off(LED7_GPIO);
}

void ui_wakeup(void)
{
	LED_On(LED0_GPIO);
}

void ui_com_open(uint8_t port)
{
	LED_On(LED2_GPIO);
}

void ui_com_close(uint8_t port)
{
	LED_Off(LED2_GPIO);
	LED_Off(LED4_GPIO);
	LED_Off(LED5_GPIO);
	LED_Off(LED6_GPIO);
	LED_Off(LED7_GPIO);
}

void ui_com_rx_start(void)
{
	LED_On(LED4_GPIO);
}

void ui_com_rx_stop(void)
{
	LED_Off(LED4_GPIO);
}

void ui_com_tx_start(void)
{
	LED_On(LED5_GPIO);
}

void ui_com_tx_stop(void)
{
	LED_Off(LED5_GPIO);
}

void ui_com_error(void)
{
	LED_On(LED6_GPIO);
}

void ui_com_overflow(void)
{
	LED_On(LED7_GPIO);
}

void ui_process(uint16_t framenumber)
{
	if (0 == framenumber) {
		LED_On(LED1_GPIO);
	}
	if (1000 == framenumber) {
		LED_Off(LED1_GPIO);
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on STK600:
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled CDC interface
 * - Led 2 is on when CDC port is open
 * - Led 4 is on during data transfer from CDC to UART
 * - Led 5 is on during data transfer from UART to CDC
 * - Led 6 signals an frame error on UART
 * - Led 7 signals an overflow
 *
 * Setup of STK600:
 * - LEDS connector is connected to PORTE
 * - SWITCHES are connected to PORTB
 * - PC2 (RXC0) is connected to RS232 SPARE RXD
 * - PC3 (TXC0) is connected to RS232 SPARE TXD
 * - Warning! The AREF1 jumper must be removed
 * because AREF1 connected on PORTB0 overrides switch 0.
 */
