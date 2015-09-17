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
#include "touch.h"
#include "ui.h"

static uint8_t ui_hid_report[UDI_HID_REPORT_OUT_SIZE];


void ui_init(void)
{

	touch_init();

	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_On(LED2_GPIO);
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
	ioport_configure_pin(GPIO_PUSH_BUTTON_1,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_2,
			IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	port = ioport_pin_to_port(GPIO_PUSH_BUTTON_0);
	port->INT0MASK = 0x0F;
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
ISR(PORTB_INT0_vect)
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
	bool b_btn_state;
	static bool btn0_last_state = false;
	static bool btn1_last_state = false;
	static bool btn2_last_state = false;
	static bool btnq_last_state = false;
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1_GPIO);
	}
	// Scan process running each 40ms
	cpt_sof++;
	if (40 > cpt_sof)
		return;
	cpt_sof = 0;

	// Scan buttons on switch 0, 1 and 2
	b_btn_state = (gpio_pin_is_low(GPIO_PUSH_BUTTON_0)) ? true : false;
	if (b_btn_state != btn0_last_state) {
		ui_hid_report[0]=b_btn_state;
		udi_hid_generic_send_report_in(ui_hid_report);
		btn0_last_state = b_btn_state;
	}

	b_btn_state = (gpio_pin_is_low(GPIO_PUSH_BUTTON_1)) ? true : false;
	if (b_btn_state != btn1_last_state) {
		ui_hid_report[0]=b_btn_state;
		udi_hid_generic_send_report_in(ui_hid_report);
		btn1_last_state = b_btn_state;
	}

	b_btn_state = (gpio_pin_is_low(GPIO_PUSH_BUTTON_2)) ? true : false;
	if (b_btn_state != btn2_last_state) {
		ui_hid_report[0]=b_btn_state;
		udi_hid_generic_send_report_in(ui_hid_report);
		btn2_last_state = b_btn_state;
	}

	// Scan QTouch button
	b_btn_state = (!check_touch_key_pressed()) ? true : false;
	if (b_btn_state != btnq_last_state) {
		ui_hid_report[0]=b_btn_state;
		udi_hid_generic_send_report_in(ui_hid_report);
		btnq_last_state = b_btn_state;
	}
}

void ui_led_change(uint8_t *report)
{
	if (report[0]=='1') {
		// A led must be on
		switch(report[1]) {
			case '2':
			LED_On(LED2_GPIO);
			break;
			case '3':
			LED_On(LED3_GPIO);
			break;
		}
	} else {
		// A led must be off
		switch(report[1]) {
			case '2':
			LED_Off(LED2_GPIO);
			break;
			case '3':
			LED_Off(LED3_GPIO);
			break;
		}
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on XMEGA-A3BU Xplained:
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB Host have checked and enabled HID generic interface
 * - Green LED close to USB connector is on linked on HID events LED 2
 * - Red LED close to USB connector is linked on HID events LED 3
 * - Event buttons are linked to switches CS0, CS1, CS2 and CS3
 * (CS3 = QTouch button managed by XMEGA QTouch library)
 * - All switches can be used to wakeup USB Host in remote wakeup mode.
 */
