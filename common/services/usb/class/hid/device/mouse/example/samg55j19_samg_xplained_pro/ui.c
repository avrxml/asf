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
#define  MOUSE_MOVE_COUNT  50

#define  MOVE_UP     0
#define  MOVE_RIGHT  1
#define  MOVE_DOWN   2
#define  MOVE_LEFT   3

/* Wakeup pin is push button 1 (SW0) */
#define  WAKEUP_PIN      (GPIO_PUSH_BUTTON_1)
#define  WAKEUP_PIO      (PIN_PUSHBUTTON_1_PIO)
#define  WAKEUP_PIO_ID   (PIN_PUSHBUTTON_1_ID)
#define  WAKEUP_PIO_MASK (PIN_PUSHBUTTON_1_MASK)
#define  WAKEUP_PIO_ATTR \
	(PIO_INPUT | PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_LOW_LEVEL)

static uint8_t move_dir = MOVE_UP;
static int32_t move_count = MOUSE_MOVE_COUNT;

/* Interrupt on "pin change" from push button to do wakeup on USB
 * Note:
 * This interrupt is enable when the USB host enable remote wakeup feature
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
	sleepmgr_lock_mode(SLEEPMGR_WAIT_FAST);
	/* Set handler for push button */
	pio_handler_set(WAKEUP_PIO, WAKEUP_PIO_ID, WAKEUP_PIO_MASK,
		WAKEUP_PIO_ATTR, ui_wakeup_handler);
	/* Enable IRQ for button */
	NVIC_EnableIRQ((IRQn_Type) WAKEUP_PIO_ID);
	/* Initialize LED */
	LED_Off(LED0);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
}


void ui_wakeup_enable(void)
{
	/* Configure BP3 as PIO input */
	pio_configure_pin(WAKEUP_PIN, WAKEUP_PIO_ATTR);
	/* Enable interrupt for BP3 */
	pio_enable_pin_interrupt(WAKEUP_PIN);
	/* Enable fast wakeup for button pin (WKUP10 for PA20) */
	//pmc_set_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup_disable(void)
{
	/* Disable interrupt for button pin */
	pio_disable_pin_interrupt(WAKEUP_PIN);
	/* Disable fast wakeup for button pin (WKUP10 for BP3) */
	//pmc_clr_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup(void)
{
	LED_On(LED0);
}

void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED0);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED0);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if (cpt_sof < 2) {
		return;
	}
	cpt_sof = 0;

	// Uses buttons to move mouse
	if (!ioport_get_pin_level(WAKEUP_PIN)) {
		move_count --;
		switch(move_dir) {
		case MOVE_UP:
			udi_hid_mouse_moveY(-MOUSE_MOVE_RANGE);
			if (move_count < 0) {
				move_dir = MOVE_RIGHT;
				move_count = MOUSE_MOVE_COUNT;
			}
			break;
		case MOVE_RIGHT:
			udi_hid_mouse_moveX(+MOUSE_MOVE_RANGE);
			if (move_count < 0) {
				move_dir = MOVE_DOWN;
				move_count = MOUSE_MOVE_COUNT;
			}
			break;
		case MOVE_DOWN:
			udi_hid_mouse_moveY(+MOUSE_MOVE_RANGE);
			if (move_count < 0) {
				move_dir = MOVE_LEFT;
				move_count = MOUSE_MOVE_COUNT;
			}
			break;
		case MOVE_LEFT:
			udi_hid_mouse_moveX(-MOUSE_MOVE_RANGE);
			if (move_count < 0) {
				move_dir = MOVE_UP;
				move_count = MOUSE_MOVE_COUNT;
			}
			break;
		}
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAMG55-XPRO:
 * - LED0 blinks when USB host has checked and enabled HID Mouse interface
 * - No mouse buttons are linked
 * - SW0 is used to move mouse around
 * - Only a low level on SW0 will generate a wakeup to USB Host in remote wakeup mode.
 *
 */
