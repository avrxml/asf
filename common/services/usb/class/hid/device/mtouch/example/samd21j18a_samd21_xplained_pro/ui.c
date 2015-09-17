/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

static uint8_t ui_hid_report[UDI_HID_REPORT_IN_SIZE] = {0};
static uint8_t ui_step = 0;

void ui_multitouch_down(void);

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
	if ((framenumber % 1000) == 0) {
		LED_On(LED_0_PIN);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED_0_PIN);
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
		ui_multitouch_down();
		ui_step++;
		btn0_last_state = b_btn_state;
	}
}
void ui_multitouch_down()
{
	// Report ID
	ui_hid_report[0] = UDI_HID_REPORT_ID_MTOUCH;
 
	// 1st touch
	ui_hid_report[1] = 3;
	ui_hid_report[2] = 1;
	ui_hid_report[3] = (uint8_t)((1365 + 10 * ui_step) & 0x00FF);
	ui_hid_report[4] = (uint8_t)(((1365 + 10 * ui_step) & 0xFF00) >> 8);
	ui_hid_report[5] = (uint8_t)(1365 & 0x00FF);
	ui_hid_report[6] = (uint8_t)((1365 & 0xFF00) >> 8);
 
	// 2nd touch
	ui_hid_report[7] = 3;
	ui_hid_report[8] = 2;
	ui_hid_report[9] = (uint8_t)(1365 & 0x00FF);
	ui_hid_report[10] = (uint8_t)((1365 & 0xFF00) >> 8);
	ui_hid_report[11] = (uint8_t)((1365 + 10 * ui_step) & 0x00FF);
	ui_hid_report[12] = (uint8_t)(((1365 + 10 * ui_step) & 0xFF00) >> 8);
 
	// Number of touches   
	ui_hid_report[13] = 2;

	udi_hid_multi_touch_send_report_in(ui_hid_report);
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAMD21-XPlain:
 * - LED0 blinks when USB host has checked and enabled HID multi-touch interface
 * - Push button 0 (SW0) is used to draw line in paint application on Windows O.S
 *  - Run the application and connect usb to he PC first.
 *  - Open paint and push button 0 continuously, then two lines painted.
 *
 */
