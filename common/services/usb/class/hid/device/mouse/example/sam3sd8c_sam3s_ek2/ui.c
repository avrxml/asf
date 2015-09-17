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

// Wakeup pin is PA15 (fast wakeup 14)
#define  WAKEUP_PMC_FSTT (PMC_FSMR_FSTT14)
#define  WAKEUP_PIN      (PIO_PA15_IDX)
#define  WAKEUP_PIO      (PIOA)
#define  WAKEUP_PIO_ID   (ID_PIOA)
#define  WAKEUP_PIO_MASK (PIO_PA15)
#define  WAKEUP_PIO_ATTR (PIO_INPUT | PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_LOW_LEVEL)

// Interrupt on "pin change" from PA15 to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
static void ui_wakeup_handler(uint32_t id, uint32_t mask)
{
	if (WAKEUP_PIO_ID == id && WAKEUP_PIO_MASK == mask) {
		// It is a wakeup then send wakeup USB
		udc_remotewakeup();
	}
}

void ui_init(void)
{
	// Enable PIO clock for button inputs
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);
	pmc_enable_periph_clk(ID_PIOC);
	// Set handler for PA15
	pio_handler_set(WAKEUP_PIO, WAKEUP_PIO_ID, WAKEUP_PIO_MASK, WAKEUP_PIO_ATTR, ui_wakeup_handler);
	// Enable IRQ for button (PIOA)
	NVIC_EnableIRQ((IRQn_Type) WAKEUP_PIO_ID);
	// Initialize LEDs
	LED_On(LED0_GPIO);
	LED_Off(LED1_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
}


void ui_wakeup_enable(void)
{
	// Configure PA15 as PIO input
	pio_configure_pin(WAKEUP_PIN, WAKEUP_PIO_ATTR);
	// Enable interrupt for PA15
	pio_enable_pin_interrupt(WAKEUP_PIN);
	// Enable fast wakeup for button pin (WKUP14 for PA15)
	pmc_set_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup_disable(void)
{
	// Disable interrupt for button pin
	pio_disable_pin_interrupt(WAKEUP_PIN);
	// Disable fast wakeup for button pin (WKUP14 for PA15)
	pmc_clr_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup(void)
{
	LED_On(LED0_GPIO);
}

void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED0_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED0_GPIO);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if (cpt_sof < 2) {
		return;
	}
	cpt_sof = 0;

	// Uses buttons to move mouse
	if (!gpio_pin_is_high(GPIO_PUSH_BUTTON_1)) {
		udi_hid_mouse_moveX(-MOUSE_MOVE_RANGE);
	}
	if (!gpio_pin_is_high(GPIO_PUSH_BUTTON_2)) {
		udi_hid_mouse_moveX( MOUSE_MOVE_RANGE);
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM3S-EK2 :
 * - Led 0 (D2) blinks when USB host has checked and enabled HID Mouse interface
 * - No mouse buttons are linked
 * - USRPB1 (BP2) and USRPB2 (BP3) are used to move mouse left and right
 * - Only a low level on J13.PA15 will generate a wakeup to USB Host in remote wakeup mode.
 *
 */
