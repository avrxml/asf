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

static bool ui_b_led_blink = true;
static uint8_t ui_hid_report[UDI_HID_REPORT_OUT_SIZE];

void ui_init(void)
{
	/* Initialize LEDs */
	LED_On(LED_0_PIN);
}

void ui_powerdown(void)
{
	LED_Off(LED_0_PIN);
}


void ui_wakeup_enable(void)
{

}

void ui_wakeup_disable(void)
{

}

void ui_wakeup(void)
{
	LED_On(LED_0_PIN);
}

void ui_process(uint16_t framenumber)
{
	bool b_btn_state;
	static bool btn0_last_state = false;
	static uint8_t cpt_sof = 0;

	// Blink LED
	if (ui_b_led_blink) {
		if ((framenumber % 1000) == 0) {
			LED_On(LED_0_PIN);
		}
		if ((framenumber % 1000) == 500) {
			LED_Off(LED_0_PIN);
		}
	}

	// Scan process running each 40ms
	cpt_sof++;
	if (cpt_sof < 40) {
		return;
	}
	cpt_sof = 0;

	// Scan buttons
	b_btn_state = !port_pin_get_input_level(BUTTON_0_PIN);
	if (b_btn_state != btn0_last_state) {
		ui_hid_report[0] = b_btn_state;
		udi_hid_generic_send_report_in(ui_hid_report);
		btn0_last_state = b_btn_state;
	}
}

void ui_led_change(uint8_t *report)
{
	if (report[0]=='1') {
		// A led must be on
		switch(report[1]) {
			case '1':
			ui_b_led_blink = false;
			LED_On(LED_0_PIN);
			break;
		}
	} else {
		// A led can blink
		switch(report[1]) {
			case '1':
			ui_b_led_blink = true;
			break;
		}
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAMD21-XPlain:
 * - LED0 blinks when USB host has checked and enabled HID generic interface
 * - HID events LED1 can make LED0 always on or blinking
 * - Event buttons are linked to SW0.
 *
 */
