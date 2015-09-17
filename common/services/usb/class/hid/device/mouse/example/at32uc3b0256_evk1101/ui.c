/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#define  MOUSE_MOVE_RANGE  (3*4)

// Interrupt on "pin change" from switch to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
ISR(ui_wakeup_isr, AVR32_GPIO_IRQ_GROUP, 0)
{
	// Clear GPIO interrupts.
	gpio_clear_pin_interrupt_flag(GPIO_PUSH_BUTTON_0);
	gpio_clear_pin_interrupt_flag(GPIO_PUSH_BUTTON_1);
	// Clear External Interrupt Line else Wakeup event always enabled
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT6);
	eic_clear_interrupt_line(&AVR32_EIC, EXT_INT7);
	// It is a wakeup then send wakeup USB
	udc_remotewakeup();
}


void ui_init(void)
{
	// Initialize accelerometer driver
	sysclk_enable_pba_module(SYSCLK_ADC);
	acc_init();

	/* register joystick handler on level 0 */
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(ui_wakeup_isr,
			AVR32_GPIO_IRQ_0 + (GPIO_PUSH_BUTTON_0 / 8), 0);
	cpu_irq_restore(flags);

	LED_On(LED0);
	LED_Off(LED1);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
}


//! Structure holding the configuration parameters of the EIC low level driver.
static eic_options_t eic_options = {
	// Enable level-triggered interrupt.
	.eic_mode = EIC_MODE_LEVEL_TRIGGERED,
	// Don't care value because the chosen eic mode is level-triggered.
	.eic_edge = 0,
	// Interrupt will trigger on low-level.
	.eic_level = EIC_LEVEL_LOW_LEVEL,
	// Enable filter.
	.eic_filter = EIC_FILTER_ENABLED,
	// For Wake Up mode, initialize in asynchronous mode
	.eic_async = EIC_ASYNCH_MODE
};


void ui_wakeup_enable(void)
{
	/* configure buttons to produce IT on all state change */
	gpio_enable_pin_interrupt(GPIO_PUSH_BUTTON_0, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(GPIO_PUSH_BUTTON_1, GPIO_PIN_CHANGE);
	/*
	 * Configure pin change interrupt for asynchronous wake-up (required to
	 * wake up from the STATIC sleep mode).
	 *
	 * First, map the interrupt line to the GPIO pin with the right
	 * peripheral function.
	 */
	gpio_enable_module_pin(GPIO_PUSH_BUTTON_0, AVR32_EIC_EXTINT_6_FUNCTION);
	gpio_enable_module_pin(GPIO_PUSH_BUTTON_1, AVR32_EIC_EXTINT_7_FUNCTION);

	/*
	 * Enable the internal pull-up resistor on that pin (because the EIC is
	 * configured such that the interrupt will trigger on low-level, see
	 * eic_options.eic_level).
	 */
	gpio_enable_pin_pull_up(GPIO_PUSH_BUTTON_0);
	gpio_enable_pin_pull_up(GPIO_PUSH_BUTTON_1);

	// Init the EIC controller with the set options.
	eic_options.eic_line = EXT_INT6;
	eic_init(&AVR32_EIC, &eic_options,
			sizeof(eic_options) / sizeof(eic_options_t));
	eic_options.eic_line = EXT_INT7;
	eic_init(&AVR32_EIC, &eic_options,
			sizeof(eic_options) / sizeof(eic_options_t));
	// Enable External Interrupt Controller Line
	eic_enable_line(&AVR32_EIC, EXT_INT6);
	eic_enable_line(&AVR32_EIC, EXT_INT7);
}

void ui_wakeup_disable(void)
{
	eic_disable_line(&AVR32_EIC, EXT_INT6);
	eic_disable_line(&AVR32_EIC, EXT_INT7);
	/* Disable joystick input change ITs. */
	gpio_disable_pin_interrupt(GPIO_PUSH_BUTTON_0);
	gpio_disable_pin_interrupt(GPIO_PUSH_BUTTON_1);
}

void ui_wakeup(void)
{
	LED_On(LED0);
}

void ui_process(uint16_t framenumber)
{
	bool b_btn_state;
	static bool btn_left_last_state = HID_MOUSE_BTN_UP;
	static bool btn_right_last_state = HID_MOUSE_BTN_UP;
	static bool btn_middle_last_state = HID_MOUSE_BTN_UP;
	uint8_t i;
	signed int val;
	struct cal_t {
		uint8_t angle;
		int8_t move;
	} calibration_acc[5] = {
		{
		40, 10 * MOUSE_MOVE_RANGE}, {
		30, 6 * MOUSE_MOVE_RANGE}, {
		20, 4 * MOUSE_MOVE_RANGE}, {
		15, 2 * MOUSE_MOVE_RANGE}, {
	10, 1 * MOUSE_MOVE_RANGE}};
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1);
	}
	// Scan process running each 40ms
	cpt_sof++;
	if (40 > cpt_sof)
		return;
	cpt_sof = 0;

	// Scan buttons on switch 0 (left), 1 (right), joystick_press (middle),
	b_btn_state = (!gpio_get_pin_value(GPIO_PUSH_BUTTON_0)) ?
			HID_MOUSE_BTN_DOWN : HID_MOUSE_BTN_UP;
	if (b_btn_state != btn_left_last_state) {
		udi_hid_mouse_btnleft(b_btn_state);
		btn_left_last_state = b_btn_state;
	}
	b_btn_state = (is_joystick_pressed())? HID_MOUSE_BTN_DOWN :
			HID_MOUSE_BTN_UP;
	if (b_btn_state != btn_middle_last_state) {
		udi_hid_mouse_btnmiddle(b_btn_state);
		btn_middle_last_state = b_btn_state;
	}
	b_btn_state = (!gpio_get_pin_value(GPIO_PUSH_BUTTON_1)) ?
			HID_MOUSE_BTN_DOWN : HID_MOUSE_BTN_UP;
	if (b_btn_state != btn_right_last_state) {
		udi_hid_mouse_btnright(b_btn_state);
		btn_right_last_state = b_btn_state;
	}
	// Look joystick activity for the Wheel events
	if (is_joystick_up())
		udi_hid_mouse_moveScroll(MOUSE_MOVE_RANGE);
	if (is_joystick_down())
		udi_hid_mouse_moveScroll(-MOUSE_MOVE_RANGE);

	// Look accelerometer activity for the X and Y events
	acc_update();	// Get accelerometer acquisition and process data
	for (i = 0; i < (sizeof(calibration_acc) / sizeof(struct cal_t)); i++) {
		val = is_acc_abs_angle_x(calibration_acc[i].angle);
		if (0 == val)
			continue;
		if (0 < val)
			udi_hid_mouse_moveX(0 - calibration_acc[i].move);
		else
			udi_hid_mouse_moveX(calibration_acc[i].move);
		break;
	}
	for (i = 0; i < (sizeof(calibration_acc) / sizeof(struct cal_t)); i++) {
		val = is_acc_abs_angle_y(calibration_acc[i].angle);
		if (0 == val)
			continue;
		if (0 < val)
			udi_hid_mouse_moveY(0 - calibration_acc[i].move);
		else
			udi_hid_mouse_moveY(calibration_acc[i].move);
		break;
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on EVK1101 :
 * - PWR led is on when power present
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled HID mouse interface
 * - Mouse buttons are linked to switch PB0 (left), PB1 (right), joystick press (middle)
 * - Look accelerometer activity for the X and Y mouse events
 * - Look joystick activity for the Wheel events
 * - The PB0 and PB1 can be used to wakeup USB Host in remote wakeup mode.
 */
