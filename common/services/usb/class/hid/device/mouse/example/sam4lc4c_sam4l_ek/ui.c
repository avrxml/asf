/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

static uint8_t move_dir = MOVE_UP;
static int32_t move_count = MOUSE_MOVE_COUNT;

// Wakeup pin is PB0 (PC3, EIC5)
#define UI_WAKEUP_IRQN         EIC_5_IRQn
#define UI_WAKEUP_IRQ_LEVEL    5
#define UI_WAKEUP_EIC_LINE     GPIO_PUSH_BUTTON_EIC_LINE // 5
#define UI_WAKEUP_HANDLER      button_handler
#define UI_WAKEUP_BPM_SRC      BPM_BKUPWEN_EIC

/*!
 * \name Internal routines to manage asynchronous interrupt pin change
 * This interrupt is connected to a switch and allows to wakeup CPU in low sleep
 * mode.
 * This wakeup the USB devices connected via a downstream resume.
 * @{
 */
static void ui_enable_asynchronous_interrupt(void);
static void ui_disable_asynchronous_interrupt(void);

// Interrupt on "pin change" from PB0 to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
static void UI_WAKEUP_HANDLER(void)
{
	sysclk_enable_peripheral_clock(EIC);
	if(eic_line_interrupt_is_pending(EIC, UI_WAKEUP_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, UI_WAKEUP_EIC_LINE);
		ui_disable_asynchronous_interrupt();
		// It is a wakeup then send wakeup USB
		udc_remotewakeup();
	}
	sysclk_disable_peripheral_clock(EIC);
}

/**
 * \brief Initializes and enables interrupt pin change
 */
static void ui_enable_asynchronous_interrupt(void)
{
	/* Initialize EIC for button wakeup */
	sysclk_enable_peripheral_clock(EIC);
	struct eic_line_config eic_opt ={
		.eic_mode = EIC_MODE_EDGE_TRIGGERED,
		.eic_edge = EIC_EDGE_FALLING_EDGE,
		.eic_level = EIC_LEVEL_LOW_LEVEL,
		.eic_filter = EIC_FILTER_DISABLED,
		.eic_async = EIC_ASYNCH_MODE
	};
	eic_enable(EIC);
	eic_line_set_config(EIC, UI_WAKEUP_EIC_LINE, &eic_opt);
	eic_line_set_callback(EIC, UI_WAKEUP_EIC_LINE, UI_WAKEUP_HANDLER,
		UI_WAKEUP_IRQN, UI_WAKEUP_IRQ_LEVEL);
	eic_line_enable(EIC, UI_WAKEUP_EIC_LINE);
	eic_line_enable_interrupt(EIC, UI_WAKEUP_EIC_LINE);
}

/**
 * \brief Disables interrupt pin change
 */
static void ui_disable_asynchronous_interrupt(void)
{
	eic_line_disable_interrupt(EIC, UI_WAKEUP_EIC_LINE);
	sysclk_disable_peripheral_clock(EIC);
}

//! @}

void ui_init(void)
{
	// Initialize LEDs
	LED_On(LED0);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
}


void ui_wakeup_enable(void)
{
	ui_enable_asynchronous_interrupt();
}

void ui_wakeup_disable(void)
{
	ui_disable_asynchronous_interrupt();
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
	if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) {
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
 * Human interface on SAM4L-EK:
 * - LED0 blinks when USB host has checked and enabled HID Mouse interface
 * - No mouse buttons are linked
 * - PB0 is used to move mouse around
 * - Only a low level on PB0 will generate a wakeup to USB Host in remote wakeup mode.
 *
 */
