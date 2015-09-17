/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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


/**
 * \name Main user interface functions
 * @{
 */
void ui_init(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
	LED_Off(LED2);
	LED_Off(LED3);
}

void ui_usb_mode_change(bool b_host_mode)
{
	ui_init();
}
//! @}

/**
 * \name Host mode user interface functions
 * @{
 */

//! Status of device enumeration
static uhc_enum_status_t ui_enum_status=UHC_ENUM_DISCONNECT;
//! Blink frequency depending on device speed
static uint16_t ui_device_speed_blink;
//! Status of the MSC test
static bool ui_test_done;
//! Result of the MSC test
static bool ui_test_result;

void ui_usb_vbus_change(bool b_vbus_present)
{
	if (b_vbus_present) {
		LED_On(LED0);
	} else {
		LED_Off(LED0);
	}
}

void ui_usb_vbus_error(void)
{
}

void ui_usb_connection_event(uhc_device_t *dev, bool b_present)
{
	if (b_present) {
		LED_On(LED1);
	} else {
		ui_enum_status = UHC_ENUM_DISCONNECT;
		LED_Off(LED1);
	}
	LED_Off(LED2);
	LED_Off(LED3);
}

void ui_usb_enum_event(uhc_device_t *dev, uhc_enum_status_t status)
{
	ui_enum_status = status;
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

void ui_usb_wakeup_event(void)
{
}

void ui_usb_sof_event(void)
{
	static uint16_t counter_sof = 0;

	if (ui_enum_status == UHC_ENUM_SUCCESS) {

		// Display device enumerated and in active mode
		if (++counter_sof > ui_device_speed_blink) {
			counter_sof = 0;
			LED_Toggle(LED1);
			if (ui_test_done && !ui_test_result) {
				// Test fail then blink led
				LED_Toggle(LED3);
			}
		}
	}
}

void ui_test_flag_reset(void)
{
	ui_test_done = false;
	LED_Off(LED3);
}

void ui_test_finish(bool b_success)
{
	ui_test_done = true;
	ui_test_result = b_success;
	if (b_success) {
		LED_On(LED3);
	}
}
//! @}

//! \name Callback to show the MSC read and write access
//! @{
void ui_start_read(void)
{
	LED_On(LED2);
}

void ui_stop_read(void)
{
	LED_Off(LED2);
}

void ui_start_write(void)
{
	LED_On(LED2);
}

void ui_stop_write(void)
{
	LED_Off(LED2);
}
//! @}


/**
 * \defgroup UI User Interface
 *
 * Human interface on EVK1101 :
 * - PWR led is on when power present
 * - Led 0 is on when USB OTG cable is plugged and Vbus is present
 * - Led 1 is continuously on when a device is connected
 * - Led 1 blinks when the device is enumerated and USB in idle mode
 *   - The blink is slow (1s) with low speed device
 *   - The blink is normal (0.5s) with full speed device
 *   - The blink is fast (0.25s) with high speed device
 * - Led 2 is on when a read or write access is on going
 * - Led 3 is on when a LUN test is success
 * - Led 3 blinks when a LUN test is unsuccessful
 */
