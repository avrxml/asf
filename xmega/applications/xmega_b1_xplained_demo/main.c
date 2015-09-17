/**
 * \file
 *
 * \brief Main functions for XMEGA-B1 Xplained demonstration
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

/**
 * \mainpage XMEGA-B1 Xplained Demonstration
 *
 * This is the application that is shipped with the XMEGA-B1 Xplained
 * kits.
 * <B>
 * The application note AVR1619 provides more detailed information about this
 * firmware. >/B>
 * This application demonstrates the main features of the ATxmega128B1 and its
 * XMEGA-B1 Xplained:
 * - USB device: The application implements a USB composite device
 *   with generic HID communication and Mass Storage interfaces. When connected
 *   to a USB host, the application appears as:
 *   - A mass storage device (using the on-board 8MBytes DataFlash memory)
 *   - A Generic HID device that can exchange data with a dedicated host
 *     application.
 * - LCD module controller: The ATxmega128B1 is connected to an LCD glass, and is
 *   able to write text messages, display numeric values or blink pixel on the
 *   LCD glass.
 * - Measure on-board analog sensors values (NTC, light sensor, external voltage
 *   ,and potentiometer and display those values to the on-board LCD.
 * - Touch Buttons: The CS0..CS3 touch buttons allow to change the displayed
 *   sensor.
 *
 * \note
 * The application is build by default for ATxmega128B1 revision B and higher.
 * At startup time, the application checks if the silicon revision match with the
 * firmware build. If the application detects an issue, it will be reported with
 * a "REV ERR" message on the LCD and the application will be locked.
 * However pressing CS0,CS1,CS2,CS3 in this sequence can force application execution,
 * but the LCD display behavior will be incorrect. To rebuild the application
 * for ATxmega128B1 revision A, the CONFIG_XMEGA_128B1_REVA define should
 * be set in the project.
 *
 * \section files Main files:
 * - main.c  Main application
 * - adc_sensors.c On-board sensor measurements
 * - ui.c Manages the user interface (LCD, LEDs, Touch Sensor)
 * - usb_hid_com.c Manages the USB generic communication messages with the host
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "preprocessor.h"
#include "at45dbx.h"
#include "main.h"
#include "adc_sensors.h"
#include "usb_hid_com.h"
#include "asf.h"

// USB Device serial number (built from user signature raw)
uint8_t xplain_b1_serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH + 1];

//! Timeout to display user message received with USB
uint16_t main_timeout_user_message = 0;

//! Sensor selector to be displayed on LCD
enum sensor_display_t main_sensor_selected;

//! Flag to know if the HID generic interface is enabled
static bool main_b_generic_enable = false;
//! Flag to know if the Mass Storage interface is enabled
static volatile bool main_b_msc_enable = false;

static void main_build_usb_serial_number(void);

//! Internal variable to know for which Si revision the firmware was built
#ifdef CONFIG_XMEGA_128B1_REVA
static bool firmware_rev_a = true;
#else
static bool firmware_rev_a = false;
#endif

void (*start_bootloader) (void) = (void (*)(void))(BOOT_SECTION_START/2+0x1FC/2);

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	//Initialize interrupt controller
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize sleep manager
	sleepmgr_init();
	// Initialize clock tree
	sysclk_init();
	// Initialize hardware board resources
	board_init();

	// Initialize user interface
	ui_init();
	ui_powerdown();

	// Sanity check about Silicon revision Vs Firmware build
	// for Silicon revision A, firmware should be specific
	if ((!firmware_rev_a) && (nvm_read_device_rev()==0)) {
		ui_si_revision_error();
		while(ui_button()!=1);
		while(ui_button()!=2);
		while(ui_button()!=4);
		while(ui_button()!=8);
	}

	// Initialize DATA Flash
	at45dbx_init();

	// Initialize ADC for on-board sensors
	adc_sensors_init();

	// Initialize USB HID report protocol
	usb_hid_com_init();

	// Start USB stack
	main_build_usb_serial_number();
	udc_start();

	// The main loop manages only the power mode
	// because everything else is managed by interrupt.
	// The USB Start of Frame event manages internal tick events for
	// on-board sensor updates as well as LCD display update.
	while (true) {
		if (main_b_msc_enable) {
			if (!udi_msc_process_trans()) {
				sleepmgr_enter_sleep();
			}
		} else {
			sleepmgr_enter_sleep();
		}
		if (usb_hid_com_is_start_dfu()) {
			main_start_dfu_session();
		}
	}
}

void main_sof_action(void)
{
	// LCD strings for on-board sensors
	static uint8_t lcd_txt_light[LCD_TEXT_STRING_SIZE + 2 * LCD_TEST_DISPLAY_SIZE]
			= "LIGHT SENSOR     ";
	static uint8_t lcd_txt_ntc[LCD_TEXT_STRING_SIZE + 2 * LCD_TEST_DISPLAY_SIZE]
			= "TEMPERATURE SENSOR     ";
	static uint8_t lcd_txt_pot[LCD_TEXT_STRING_SIZE + 2 * LCD_TEST_DISPLAY_SIZE]
			= "POTENTIOMETER INPUT     ";
	static uint8_t lcd_txt_ext_v[LCD_TEXT_STRING_SIZE + 2 * LCD_TEST_DISPLAY_SIZE]
			= "EXTERNAL VOLTAGE    ";

	static uint8_t old_button;

	if (main_timeout_user_message) {
		// Currently LCD is busy displaying user message from USB
		// Wait end of timeout to display again sensor message
		if (--main_timeout_user_message == 0) {
			switch (main_sensor_selected) {
			case LIGHT_SENSOR:
				ui_display_text(lcd_txt_light,
					sizeof(lcd_txt_light));
				break;
			case TEMPERATURE_SENSOR:
				ui_display_text(lcd_txt_ntc,
					sizeof(lcd_txt_ntc));
				break;
			case POTENTIOMETER_SENSOR:
				ui_display_text(lcd_txt_pot,
					sizeof(lcd_txt_pot));
				break;
			case EXT_VOLTAGE_INPUT:
				ui_display_text(lcd_txt_ext_v,
					sizeof(lcd_txt_ext_v));
			default:
				break;
			}
		}
	}

	// Change LOCAL text display when a button is pressed...
	if (old_button!=ui_button()) {
		old_button=ui_button();
		if (ui_pusb_button_0()) {
			ui_display_text(lcd_txt_light, sizeof(lcd_txt_light));
			main_sensor_selected = LIGHT_SENSOR;
		}
		if (ui_pusb_button_1()) {
			ui_display_text(lcd_txt_ntc, sizeof(lcd_txt_ntc));
			main_sensor_selected = TEMPERATURE_SENSOR;
		}
		if (ui_pusb_button_2()) {
			ui_display_text(lcd_txt_pot, sizeof(lcd_txt_pot));
			main_sensor_selected = POTENTIOMETER_SENSOR;
		}
		if (ui_pusb_button_3()) {
			ui_display_text(lcd_txt_ext_v, sizeof(lcd_txt_ext_v));
			main_sensor_selected = EXT_VOLTAGE_INPUT;
		}
		// If hid generic interface is not up and running, do nothing
		if (!main_b_generic_enable) {
			return;
		}
	}

	// Prepare new report each HID_SEND_REPORT_MS ms
	if ((udd_get_frame_number() % HID_SEND_REPORT_MS) == 0) {
		usb_hid_com_send_report_in();
	}

	// Prepare new ADC samples each ADC_NEW_SAMPLE_MS ms
	if ((udd_get_frame_number() % ADC_NEW_SAMPLE_MS) == 0) {
		adc_sensors_start_conversions();
	}

	// Each LCD_SCROLLING_MS ms Performs LCD scrolling
	if ((udd_get_frame_number() % LCD_SCROLLING_MS) == 0) {
		ui_scroll_text();
	}

	// Refresh on-board sensors LCD display
	// each LCD_SENSOR_MS and if no user message is displayed
	if (((udd_get_frame_number() % LCD_SENSOR_MS) == 0)
		&& (main_timeout_user_message == 0)) {
		switch (main_sensor_selected) {
		case LIGHT_SENSOR:
			ui_display_sensor(adc_sensor_get_light(),
					UI_UNIT_NONE);
			break;
		case TEMPERATURE_SENSOR:
			ui_display_sensor(adc_sensor_get_temperature(),
					UI_UNIT_DEGREE_C);
			break;
		case POTENTIOMETER_SENSOR:
			ui_display_sensor(adc_sensor_get_potentiometer(),
					UI_UNIT_MILIVOLT);
			break;
		case EXT_VOLTAGE_INPUT:
			ui_display_sensor(adc_sensor_get_ext_voltage(),
					UI_UNIT_MILIVOLT);
		default:
			break;
		}
	}
}

bool main_hid_generic_enable(void)
{
	main_b_generic_enable = true;
	backlight_on();
	return true;
}

void main_hid_generic_disable(void)
{
	backlight_off();
	main_b_generic_enable = false;
}

bool main_msc_enable(void)
{
	main_b_msc_enable = true;
	return true;
}

void main_msc_disable(void)
{
	main_b_msc_enable = false;
}

enum sensor_display_t main_get_sensor_selected(void)
{
	return main_sensor_selected;
}

void main_notify_external_msg(void)
{
	main_timeout_user_message = LCD_USER_MSG_MS;
}

static void main_build_usb_serial_number(void)
{
	uint8_t i;
	uint8_t digit;
	uint8_t nibble_to_ascii[16] = "0123456789ABCDEF";

	/* Copy serial number from user signature row and convert to ASCII
	 * The 6 byte id results in a 12 byte string (1 character per nibble)
	 */
	for (i = 0; i < USER_SIGNATURE_USBID_SIZE / 2; i++) {
		digit = nvm_read_user_signature_row(i + USER_SIGNATURE_USBID_POS);
		// Upper nibble
		xplain_b1_serial_number[i * 2] =
				nibble_to_ascii[(digit & 0xF0)>>4];
		// Lower nibble
		xplain_b1_serial_number[(i * 2) + 1] =
				nibble_to_ascii[(digit & 0x0F)];
	}
}

void main_start_dfu_session(void)
{
	udc_stop();
	adc_sensors_stop();
	ui_dfu_session();
	EIND = BOOT_SECTION_START>>17;
	start_bootloader();
}
