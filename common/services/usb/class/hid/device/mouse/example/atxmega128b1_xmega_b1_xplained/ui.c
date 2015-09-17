/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

void ui_init(void)
{
	LED_On(LED0_GPIO);
	LED_Off(LED1_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	led_power_off();
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
	ioport_configure_pin(GPIO_PUSH_BUTTON_3,
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
ISR(PORTE_INT0_vect)
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
	led_power_on();
}

void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1_GPIO);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if (2 > cpt_sof) {
		return;
	}
	cpt_sof = 0;

	// Scan move on switch 0 (right), 1 (left), 2 (down), 3 (up)
	if (gpio_pin_is_low(GPIO_PUSH_BUTTON_0))
		udi_hid_mouse_moveX(MOUSE_MOVE_RANGE);
	if (gpio_pin_is_low(GPIO_PUSH_BUTTON_1))
		udi_hid_mouse_moveX(-MOUSE_MOVE_RANGE);
	if (gpio_pin_is_low(GPIO_PUSH_BUTTON_2))
		udi_hid_mouse_moveY(MOUSE_MOVE_RANGE);
	if (gpio_pin_is_low(GPIO_PUSH_BUTTON_3))
		udi_hid_mouse_moveY(-MOUSE_MOVE_RANGE);
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on XMEGA-B1 Xplained:
 * - Led 0 and the RED led close to USB connector are on
 *   when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled HID mouse interface
 * - Mouse move are linked at switches 0 (right), 1 (left), 2 (down), 3 (up)
 * - All switches can be used to wakeup USB Host in remote wakeup mode.
 */
