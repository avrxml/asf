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

#define  MOUSE_MOVE_RANGE  3

// Interrupt on "pin change" from switch to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
ISR(ui_wakeup_isr, AVR32_GPIO_IRQ_GROUP, 0)
{
	// Clear GPIO interrupt.
	gpio_clear_pin_interrupt_flag(AVR32_PIN_PA20);
	// Clear External Interrupt Line else Wakeup event always enabled
	eic_clear_interrupt_line(&AVR32_EIC, EXT_NMI);
	// It is a wakeup then send wakeup USB
	udc_remotewakeup();
}


void ui_init(void)
{
	/* register joystick handler on level 0 */
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(ui_wakeup_isr,
			AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA20 / 8), 0);
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
static const eic_options_t eic_options = {
	// Choose External Interrupt Controller Line
	.eic_line = EXT_NMI,
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
	// With the UC3A3 datasheet, in section "Peripheral Multiplexing on I/O lines"
	// in the table "GPIO Controller Function Multiplexing", we see that the NMI is
	// mapped on GPIO 20 (i.e. PA20). Using the EVK1104 schematics, we see that the
	// J17.7 pin is connected to PA20. Thus, a low level on J17.7 will generate an
	// NMI. The External Interrupt number 8 is the NMI.

	/* configure joystick to produce IT on all state change */
	gpio_enable_pin_interrupt(AVR32_PIN_PA20, GPIO_PIN_CHANGE);
	/*
	 * Configure pin change interrupt for asynchronous wake-up (required to
	 * wake up from the STATIC sleep mode).
	 *
	 * First, map the interrupt line to the GPIO pin with the right
	 * peripheral function.
	 */
	gpio_enable_module_pin(AVR32_PIN_PA20, AVR32_EIC_EXTINT_8_FUNCTION);

	/*
	 * Enable the internal pull-up resistor on that pin (because the EIC is
	 * configured such that the interrupt will trigger on low-level, see
	 * eic_options.eic_level).
	 */
	gpio_enable_pin_pull_up(AVR32_PIN_PA20);

	// Init the EIC controller with the set options.
	eic_init(&AVR32_EIC, &eic_options, sizeof(eic_options) /
			sizeof(eic_options_t));

	// Enable External Interrupt Controller Line
	eic_enable_line(&AVR32_EIC, EXT_NMI);
}

void ui_wakeup_disable(void)
{
	eic_disable_line(&AVR32_EIC, EXT_NMI);
	/* Disable joystick input change ITs. */
	gpio_disable_pin_interrupt(AVR32_PIN_PA20);
}


void ui_wakeup(void)
{
	LED_On(LED0);
}


void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED1);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if (2 > cpt_sof)
		return;
	cpt_sof = 0;

	// Scan move on SW2 (right)
	if (gpio_get_pin_value(GPIO_PUSH_BUTTON_SW2) ==
			GPIO_PUSH_BUTTON_SW2_PRESSED)
		udi_hid_mouse_moveX(MOUSE_MOVE_RANGE);
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on EVK1104 :
 * - PWR led is on when power present
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled HID mouse interface
 * - Mouse buttons are not linked.
 * - Only SW2 allows to move mouse on X axe.
 * - Only a low level on J17.7 will generate a wakeup to USB Host in remote wakeup mode.
 */
