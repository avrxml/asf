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

static struct{
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
	// Display "AVR "
	{false,true,HID_CAPS_LOCK}, // Enable caps lock
	{false,false,HID_CAPS_LOCK},
	{false,true,HID_A},
	{false,false,HID_A},
	{false,true,HID_V},
	{false,false,HID_V},
	{false,true,HID_R},
	{false,false,HID_R},
	{false,true,HID_CAPS_LOCK}, // Disable caps lock
	{false,false,HID_CAPS_LOCK},
};


void ui_init(void)
{
	LED_On(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
	LED_Off(LED3_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
	LED_Off(LED3_GPIO);
}

void ui_wakeup_enable(void)
{
	PORT_t *port;
	// Configure pin change interrupt for asynch. wake-up on button pin.
	ioport_configure_pin(GPIO_PUSH_BUTTON_0,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INT0MASK = 0xFF;
	port->INTCTRL = PORT_INT0LVL_LO_gc;
}

void ui_wakeup_disable(void)
{
	PORT_t *port;
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INT0MASK = 0x00;
}


/*! \brief Interrupt on "pin change" from switch to do wakeup on USB
 * Callback running when USB Host enable mouse interface
 *
 * Note:
 * This interrupt is enable when the USB host enable remotewakeup feature
 * This interrupt wakeup the CPU if this one is in idle mode
 */
ISR(PORTF_INT0_vect)
{
	PORT_t *port;
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INTFLAGS = 0x01; // Ack interrupt
	// It is a wakeup then send wakeup USB
	udc_remotewakeup();
}

void ui_wakeup(void)
{
	LED_On(LED0_GPIO);
}

void ui_process(uint16_t framenumber)
{
	bool b_btn_state, sucess;
	static bool btn_last_state = false;
	static bool sequence_running = false;
	static uint8_t u8_sequence_pos = 0;
	uint8_t u8_value;
	static uint16_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1_GPIO);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if ((cpt_sof%2)==0) {
		return;
	}

	// Scan buttons on switch 0 to send keys sequence
	b_btn_state = (gpio_pin_is_low(GPIO_PUSH_BUTTON_0)) ? true : false;
	if (b_btn_state != btn_last_state) {
		btn_last_state = b_btn_state;
		sequence_running = true;
	}

	// Sequence process running each 200ms
	if (200 > cpt_sof) {
		return;
	}
	cpt_sof = 0;

	if (sequence_running) {
		// Send next key
		u8_value = ui_sequence[u8_sequence_pos].u8_value;
		if (ui_sequence[u8_sequence_pos].b_modifier) {
			if (ui_sequence[u8_sequence_pos].b_down) {
				sucess = udi_hid_kbd_modifier_down(u8_value);
			} else {
				sucess = udi_hid_kbd_modifier_up(u8_value);
			}
		} else {
			if (ui_sequence[u8_sequence_pos].b_down) {
				sucess = udi_hid_kbd_down(u8_value);
			} else {
				sucess = udi_hid_kbd_up(u8_value);
			}
		}
		if (!sucess) {
			return; // Retry it on next schedule
		}
		// Valid sequence position
		u8_sequence_pos++;
		if (u8_sequence_pos>=sizeof(ui_sequence)/sizeof(ui_sequence[0])) {
			u8_sequence_pos=0;
			sequence_running=false;
		}
	}

}

void ui_kbd_led(uint8_t value)
{
	if (value&HID_LED_NUM_LOCK) {
		LED_On(LED2_GPIO);
	} else {
		LED_Off(LED2_GPIO);
	}
	if (value&HID_LED_CAPS_LOCK) {
		LED_On(LED3_GPIO);
	} else {
		LED_Off(LED3_GPIO);
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on STK600:
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled HID Keyboard interface
 * - Led 2 displays numeric lock status.
 * - Led 3 displays caps lock status.
 * - The switch 0 open a note pad application on Windows O.S.
 * and send key sequence "Atmel AVR USB Keyboard"
 * - The switch 0 can be used to wakeup USB Host in remote wakeup mode.
 *
 * Setup for STK600:
 * - LEDS connector is connected to PORTE
 * - SWITCHES are connected to PORTF
 */
