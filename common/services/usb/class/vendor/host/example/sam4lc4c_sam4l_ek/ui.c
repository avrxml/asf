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

/* LED backlight control */
#define backlight_on() ioport_set_pin_level(LCD_BL_GPIO,LCD_BL_ACTIVE_LEVEL)
#define backlight_off() ioport_set_pin_level(LCD_BL_GPIO,LCD_BL_INACTIVE_LEVEL)
#define backlight_toggle() ioport_toggle_pin_level(LCD_BL_GPIO)

/**
 * \name Main user interface functions
 * @{
 */
void ui_init(void)
{
	/* Initialize LEDs */
	LED_Off(LED0);
	/* Initialize backlight */
	backlight_off();
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
/*! Status of the Vendor test */
static bool ui_test_done;
/*! Result of the Vendor test */
static bool ui_test_result;

void ui_usb_vbus_change(bool b_vbus_present)
{
	UNUSED(b_vbus_present);
}

void ui_usb_vbus_error(void)
{
}

void ui_usb_connection_event(uhc_device_t *dev, bool b_present)
{
	UNUSED(dev);
	if (!b_present) {
		LED_On(LED0);
		backlight_off();
		ui_enum_status = UHC_ENUM_DISCONNECT;
	}
}

void ui_usb_enum_event(uhc_device_t *dev, uhc_enum_status_t status)
{
	UNUSED(dev);
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
		ui_test_done = false;
	}
}


void ui_usb_sof_event(void)
{
	static uint16_t counter_sof = 0;

	if (ui_enum_status == UHC_ENUM_SUCCESS) {
		/* Display device enumerated and in active mode */
		if (++counter_sof > ui_device_speed_blink) {
			counter_sof = 0;
			LED_Toggle(LED0);
			if (ui_test_done && !ui_test_result) {
				/* Test fail then blink backlight */
				backlight_toggle();
			}
		}
	}
}

void ui_test_finish(bool b_success)
{
	ui_test_done = true;
	ui_test_result = b_success;
	if (b_success) {
		backlight_on();
	}
}

/*! @} */


/**
 * \defgroup UI User Interface
 *
 * Jumper setup on SAM4L_EK :
 * - Connect PA06 and usb (VBus detect)
 * - Connect PB05 and usb (ID detect)
 * - Connect PC07 and usb (VBus error detect)
 * - Connect PC08 and usb (VBus control)
 *
 * Human interface on SAM4L_EK :
 * - Led 0 is on when it's host and there is no device connected
 * - Led 0 blinks when a device is enumerated and Vendor interface enabled
 *   - The blink is slow (1s) with low speed device
 *   - The blink is normal (0.5s) with full speed device
 *   - The blink is fast (0.25s) with high speed device
 * - LED backlight is on when Vendor test is success
 * - LED backlight blinks when Vendor test is unsuccess
 */
