/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
	LED_On(LED0);
	LED_Off(LED1);
	LED_Off(LED2);
	LED_Off(LED_BI0_GREEN);
	LED_Off(LED_BI0_RED);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
	LED_Off(LED2);
	LED_Off(LED_BI0_GREEN);
	LED_Off(LED_BI0_RED);
	LED_Off(LED_BI1_GREEN);
	LED_Off(LED_BI1_RED);
}

void ui_wakeup(void)
{
	LED_On(LED0);
}

void ui_com_open(uint8_t port)
{
	LED_On(LED2);
}

void ui_com_close(uint8_t port)
{
	LED_Off(LED2);
	LED_Off(LED_BI0_GREEN);
	LED_Off(LED_BI0_RED);
	LED_Off(LED_BI1_GREEN);
	LED_Off(LED_BI1_RED);
}

void ui_com_rx_start(void)
{
	LED_On(LED_BI0_GREEN);
}

void ui_com_rx_stop(void)
{
	LED_Off(LED_BI0_GREEN);
}

void ui_com_tx_start(void)
{
	LED_On(LED_BI1_GREEN);
}

void ui_com_tx_stop(void)
{
	LED_Off(LED_BI1_GREEN);
}

void ui_com_error(void)
{
	LED_On(LED_BI0_RED);
}

void ui_com_overflow(void)
{
	LED_On(LED_BI1_RED);
}

void ui_process(uint16_t framenumber)
{
	if ((framenumber % 1000) == 0) {
		LED_On(LED1);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1);
	}
}


/**
 * \defgroup UI User Interface
 *
 * Human interface on EVK1100:
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled CDC interface
 * - Led 2 is on when the COM is open by USB Host
 * - Led 4 green is on during data transfer from CDC to UART
 * - Led 5 green is on during data transfer from UART to CDC
 * - Led 4 red means an frame error on UART
 * - Led 5 red means an overflow
 */
