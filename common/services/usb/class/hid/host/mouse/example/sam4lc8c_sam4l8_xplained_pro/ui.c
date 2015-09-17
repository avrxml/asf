/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

/* Wakeup pin is SW0 (PC24, EIC1) */
#define UI_WAKEUP_IRQN         EIC_1_IRQn
#define UI_WAKEUP_IRQ_LEVEL    5
#define UI_WAKEUP_EIC_LINE     SW0_EIC_LINE
#define UI_WAKEUP_HANDLER      button_handler
#define UI_WAKEUP_BPM_SRC      BPM_BKUPWEN_EIC

/**
 * \name Internal routines to manage asynchronous interrupt pin change
 * This interrupt is connected to a switch and allows to wakeup CPU in low sleep
 * mode.
 * This wakeup the USB devices connected via a downstream resume.
 * @{
 */
static void ui_enable_asynchronous_interrupt(void);
static void ui_disable_asynchronous_interrupt(void);

/**
 * \brief Interrupt handler for interrupt pin change
 */
static void UI_WAKEUP_HANDLER(void)
{
	sysclk_enable_peripheral_clock(EIC);
	if (eic_line_interrupt_is_pending(EIC, UI_WAKEUP_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, UI_WAKEUP_EIC_LINE);
		if (uhc_is_suspend()) {
			ui_disable_asynchronous_interrupt();

			/* Wakeup host and device */
			uhc_resume();
		}
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
	struct eic_line_config eic_opt = {
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

/*! @} */

/**
 * \name Main user interface functions
 * @{
 */
void ui_init(void)
{
	/* Initialize LEDs */
	LED_Off(LED0);
}

void ui_usb_mode_change(bool b_host_mode)
{
	if (b_host_mode) {
		LED_On(LED0);
	} else {
		LED_Off(LED0);
	}
}

/*! @} */

/**
 * \name Host mode user interface functions
 * @{
 */

/*! Status of device enumeration */
static uhc_enum_status_t ui_enum_status = UHC_ENUM_DISCONNECT;
/*! Blink frequency depending on device speed */
static uint16_t ui_device_speed_blink;
/*! Manages device mouse button down */
static uint8_t ui_nb_down = 0;

void ui_usb_vbus_change(bool b_vbus_present)
{
	UNUSED(b_vbus_present);
}

void ui_usb_vbus_error(void)
{
	/* Not used for 4L */
}

void ui_usb_connection_event(uhc_device_t *dev, bool b_present)
{
	UNUSED(dev);
	if (!b_present) {
		LED_On(LED0);
		ui_enum_status = UHC_ENUM_DISCONNECT;
	}
}

void ui_usb_enum_event(uhc_device_t *dev, uhc_enum_status_t status)
{
	ui_enum_status = status;
	if (ui_enum_status == UHC_ENUM_SUCCESS) {
		switch (dev->speed) {
		case UHD_SPEED_HIGH:
			ui_device_speed_blink = 250;
			break;

		case UHD_SPEED_FULL:
			ui_device_speed_blink = 500;
			break;

		case UHD_SPEED_LOW:
		default:
			ui_device_speed_blink = 1000;
			break;
		}
	}
}

void ui_usb_wakeup_event(void)
{
	ui_disable_asynchronous_interrupt();
}

void ui_usb_sof_event(void)
{
	bool b_btn_state;
	static bool btn_suspend_and_remotewakeup = false;
	static uint16_t counter_sof = 0;

	if (ui_enum_status == UHC_ENUM_SUCCESS) {
		/* Display device enumerated and in active mode */
		if (++counter_sof > ui_device_speed_blink) {
			counter_sof = 0;
			LED_Toggle(LED0);
		}

		/* Scan button to enter in suspend mode and remote wakeup */
		b_btn_state = !ioport_get_pin_level(GPIO_PUSH_BUTTON_0);
		if (b_btn_state != btn_suspend_and_remotewakeup) {
			/* Button have changed */
			btn_suspend_and_remotewakeup = b_btn_state;
			if (b_btn_state) {
				/* Button has been pressed */
				ui_enable_asynchronous_interrupt();
				LED_Off(LED0);
				uhc_suspend(true);
				return;
			}
		}

		/* Power on a LED when the mouse button down */
		if (ui_nb_down) {
			LED_On(LED0);
		}
	}
}

static void ui_uhi_hid_mouse_btn(bool b_state)
{
	if (b_state) {
		ui_nb_down++;
	} else {
		ui_nb_down--;
	}
}

void ui_uhi_hid_mouse_btn_left(bool b_state)
{
	ui_uhi_hid_mouse_btn(b_state);
}

void ui_uhi_hid_mouse_btn_right(bool b_state)
{
	ui_uhi_hid_mouse_btn(b_state);
}

void ui_uhi_hid_mouse_btn_middle(bool b_state)
{
	ui_uhi_hid_mouse_btn(b_state);
}

void ui_uhi_hid_mouse_move(int8_t x, int8_t y, int8_t scroll)
{
	UNUSED(x);
	UNUSED(y);
	UNUSED(scroll);
}

/*! @} */

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM4L8 Xplained Pro:
 * - Led 0 is on when it's host and there is no device connected
 * - Led 0 blinks when a HID mouse is enumerated and USB in idle mode
 *   - The blink is slow (1s) with low speed device
 *   - The blink is normal (0.5s) with full speed device
 *   - The blink is fast (0.25s) with high speed device
 * - Led 0 is on when a HID mouse button is pressed
 * - Button SW0 allows to enter the device in suspend mode with remote wakeup
 *   feature authorized
 * - Only SW0 button can be used to wakeup USB device in suspend mode
 */
