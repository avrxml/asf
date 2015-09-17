/**
 * \file
 *
 * \brief USB Generic HID Communication Management
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

#include "asf.h"
#include "usb_hid_com.h"
#include "adc_sensors.h"

//! HID report IN to be sent
static struct hid_report_in report_to_send;
static bool hib_b_start_dfu = false;

void usb_hid_com_init(void)
{
	//Initialize information fields
	report_to_send.silicon_revision = nvm_read_device_rev()+'A';
	report_to_send.firmware_revision = FIRMWARE_REVISION;
#ifdef CONFIG_XMEGA_128B1_REVA
	report_to_send.firmware_si_target = 'A';
#else
	report_to_send.firmware_si_target = 'B';
#endif
}

void usb_hid_com_set_feature(uint8_t *data)
{
	// Check if magic sequence has been received to start DFU
	if (data[0] == 0x55 && data[1] == 0xAA
			&& data[2] == 0x55 && data[3]== 0xAA) {
	hib_b_start_dfu = true;
	}
}

void usb_hid_com_report_out_received(uint8_t *data)
{
	// Mapped received data to report out type
	struct hid_report_out *report_out=(struct hid_report_out*)data;

	// Update LEDs with new report value
	ui_led(report_out->leds);
	// Load contrast value
	ui_set_contrast(report_out->contrast);
	// Set Backlight level
	ui_set_backlight(report_out->backlight<<8);
	// Display test user message if requested
	if (report_out->lcd_notify & 0x01) {
		ui_display_text(report_out->lcd_user_text, LCD_TEXT_STRING_SIZE);
		main_notify_external_msg();
	}
	// Display numeric user message if requested
	if (report_out->lcd_notify & 0x02) {
		ui_display_sensor(report_out->lcd_user_num, UI_UNIT_NONE);
		main_notify_external_msg();
	}
}

void usb_hid_com_send_report_in(void)
{
	// Fill HID report fields
	report_to_send.light = adc_sensor_get_light();
	report_to_send.temperature = adc_sensor_get_temperature();
	report_to_send.ext_voltage = adc_sensor_get_ext_voltage();
	report_to_send.potentiometer = adc_sensor_get_potentiometer();
	report_to_send.buttons = ui_button();
	report_to_send.sensor_selected = main_get_sensor_selected();
	// Send HID report in background
	udi_hid_generic_send_report_in((uint8_t*)&report_to_send);
}

bool usb_hid_com_is_start_dfu(void)
{
	if (hib_b_start_dfu) {
		return true;
	} else {
		return false;
	}
}
