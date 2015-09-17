/**
 * \file
 *
 * \brief User Interface
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "ui.h"

#define  MOUSE_MOVE_RANGE  3

/* Wakeup pin is push button 2 (BP3, fast wakeup 10) */
#define  WAKEUP_PMC_FSTT (PUSHBUTTON_2_WKUP_FSTT)
#define  WAKEUP_PIN      (GPIO_PUSH_BUTTON_2)
#define  WAKEUP_PIO      (PIN_PUSHBUTTON_2_PIO)
#define  WAKEUP_PIO_ID   (PIN_PUSHBUTTON_2_ID)
#define  WAKEUP_PIO_MASK (PIN_PUSHBUTTON_2_MASK)
#define  WAKEUP_PIO_ATTR \
	(PIO_INPUT | PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_LOW_LEVEL)

/* Interrupt on "pin change" from push button to do wakeup on USB
 * Note:
 * This interrupt is enable when the USB host enable remotewakeup feature
 * This interrupt wakeup the CPU if this one is in idle mode
 */
static void ui_wakeup_handler(uint32_t id, uint32_t mask)
{
	if (WAKEUP_PIO_ID == id && WAKEUP_PIO_MASK == mask) {
		/* It is a wakeup then send wakeup USB */
		udc_remotewakeup();
		LED_On(LED0);
	}
}

void ui_init(void)
{
	/* Set handler for push button */
	pio_handler_set(WAKEUP_PIO, WAKEUP_PIO_ID, WAKEUP_PIO_MASK,
		WAKEUP_PIO_ATTR, ui_wakeup_handler);
	/* Enable IRQ for button (PIOA) */
	NVIC_EnableIRQ((IRQn_Type) WAKEUP_PIO_ID);
	/* Initialize LEDs */
	LED_Off(LED0);
	LED_Off(LED1);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
}


void ui_wakeup_enable(void)
{
	/* Configure BP3 as PIO input */
	pio_configure_pin(WAKEUP_PIN, WAKEUP_PIO_ATTR);
	/* Enable interrupt for BP3 */
	pio_enable_pin_interrupt(WAKEUP_PIN);
	/* Enable fast wakeup for button pin (WKUP10 for PA20) */
	pmc_set_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup_disable(void)
{
	/* Disable interrupt for button pin */
	pio_disable_pin_interrupt(WAKEUP_PIN);
	/* Disable fast wakeup for button pin (WKUP10 for BP3) */
	pmc_clr_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup(void)
{
	LED_On(LED0);
}

void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;
	static bool btn_left = false, btn_right = false;
	bool btn_pressed;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1);
	}
	/* Scan process running each 2ms */
	cpt_sof++;
	if (cpt_sof < 2) {
		return;
	}
	cpt_sof = 0;

	/* Uses buttons to move mouse */
	if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_3)) {
		udi_hid_mouse_moveY(-MOUSE_MOVE_RANGE);
	}
	if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_4)) {
		udi_hid_mouse_moveY( MOUSE_MOVE_RANGE);
	}
	/* Check button click */
	btn_pressed = !ioport_get_pin_level(GPIO_PUSH_BUTTON_2);
	if (btn_pressed != btn_left) {
		btn_left = btn_pressed;
		udi_hid_mouse_btnleft(btn_left);
	}
	btn_pressed = !ioport_get_pin_level(GPIO_PUSH_BUTTON_1);
	if (btn_pressed != btn_right) {
		btn_right = btn_pressed;
		udi_hid_mouse_btnright(btn_right);
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM4E-EK:
 * - Led 0 (D2) is on when USB is wakeup
 * - Led 1 (D3) blinks when USB host has checked and enabled HID Mouse interface
 * - Push button 2 (BP3) and push button 1 (BP2) are linked to mouse button
 *   left and right
 * - Push button 3 (BP4) and push button 4 (BP5) are used to move mouse up
 *   and down
 * - Only a low level on push button 2 (BP3) will generate a wakeup to USB Host
 *   in remote wakeup mode.
 *
 */
