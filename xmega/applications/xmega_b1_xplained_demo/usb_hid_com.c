/**
 * \file
 *
 * \brief USB Generic HID Communication Management
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
