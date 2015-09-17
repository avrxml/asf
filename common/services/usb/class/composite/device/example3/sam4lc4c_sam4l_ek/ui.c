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

//! Sequence process running each \c SEQUENCE_PERIOD ms
#define SEQUENCE_PERIOD 150

//! Wakeup, ignore button change until button is back to default state
static bool btn_wakeup = false;

static struct {
	bool b_modifier;
	bool b_down;
	uint8_t u8_value;
} ui_sequence[] = {
	// Display windows menu
	{true,true,HID_MODIFIER_LEFT_UI},
	// Launch Windows Command line
	{false,true,HID_R},
	{false,false,HID_R},
	// Clear modifier
	{true,false,HID_MODIFIER_LEFT_UI},
	// Tape sequence "notepad" + return
	{false,true,HID_N},
	{false,false,HID_N},
	{false,true,HID_O},
	{false,false,HID_O},
	{false,true,HID_T},
	{false,false,HID_T},
	{false,true,HID_E},
	{false,false,HID_E},
	{false,true,HID_P},
	{false,false,HID_P},
	{false,true,HID_A},
	{false,false,HID_A},
	{false,true,HID_D},
	{false,false,HID_D},
	{false,true,HID_ENTER},
	{false,false,HID_ENTER},
	// Delay to wait "notepad" focus
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	// Display "Atmel "
	{true,true,HID_MODIFIER_RIGHT_SHIFT}, // Enable Maj
	{false,true,HID_A},
	{false,false,HID_A},
	{true,false,HID_MODIFIER_RIGHT_SHIFT}, // Disable Maj
	{false,true,HID_T},
	{false,false,HID_T},
	{false,true,HID_M},
	{false,false,HID_M},
	{false,true,HID_E},
	{false,false,HID_E},
	{false,true,HID_L},
	{false,false,HID_L},
	{false,true,HID_SPACEBAR},
	{false,false,HID_SPACEBAR},
	// Display "ARM "
	{false,true,HID_CAPS_LOCK}, // Enable caps lock
	{false,false,HID_CAPS_LOCK},
	{false,true,HID_A},
	{false,false,HID_A},
	{false,true,HID_R},
	{false,false,HID_R},
	{false,true,HID_M},
	{false,false,HID_M},
	{false,true,HID_CAPS_LOCK}, // Disable caps lock
	{false,false,HID_CAPS_LOCK},
};

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
		// Wakeup, ignore button change until button is back to default state
		btn_wakeup = true;
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

void ui_start_read(void)
{
}

void ui_stop_read(void)
{
}

void ui_start_write(void)
{
}

void ui_stop_write(void)
{
}

void ui_process(uint16_t framenumber)
{
	bool b_btn_state, success;
	static bool btn_last_state = false;
	static bool sequence_running = false;
	static uint8_t u8_sequence_pos = 0;
	uint8_t u8_value;
	static uint16_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED0);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED0);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if ((cpt_sof % 2) == 0) {
		return;
	}

	// Buttons down to send keys sequence
	b_btn_state = (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0));
	if (b_btn_state != btn_last_state) {
		btn_last_state = b_btn_state;
		if (btn_wakeup) {
			if (!b_btn_state) {
				btn_wakeup = false;
			}
		} else {
			sequence_running = true;
		}
	}

	// Sequence process running each period
	if (SEQUENCE_PERIOD > cpt_sof) {
		return;
	}
	cpt_sof = 0;

	if (sequence_running) {
		// Send next key
		u8_value = ui_sequence[u8_sequence_pos].u8_value;
		if (u8_value!=0) {
			if (ui_sequence[u8_sequence_pos].b_modifier) {
				if (ui_sequence[u8_sequence_pos].b_down) {
					success = udi_hid_kbd_modifier_down(u8_value);
				} else {
					success = udi_hid_kbd_modifier_up(u8_value);
				}
			} else {
				if (ui_sequence[u8_sequence_pos].b_down) {
					success = udi_hid_kbd_down(u8_value);
				} else {
					success = udi_hid_kbd_up(u8_value);
				}
			}
			if (!success) {
				return; // Retry it on next schedule
			}
		}
		// Valid sequence position
		u8_sequence_pos++;
		if (u8_sequence_pos >=
			sizeof(ui_sequence) / sizeof(ui_sequence[0])) {
			u8_sequence_pos = 0;
			sequence_running = false;
		}
	}
}

void ui_kbd_led(uint8_t value)
{
	UNUSED(value);
	/*
	if (value & HID_LED_NUM_LOCK) {
	} else {
	}
	if (value & HID_LED_CAPS_LOCK) {
	} else {
	}
	*/
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM4L-EK
 * - LED0 blinks when USB host has checked and enabled Mouse and MSC interface
 * - PB0 down opens a notepad application on Windows O.S.
 *   and sends key sequence "Atmel ARM"
 * - Only PB0 down will generate a wakeup to USB Host in remote wakeup mode.
 *
 */
