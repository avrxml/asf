/**
 * \file
 *
 * \brief XMEGA-B1 Xplained Demonstration
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

#ifndef _MAIN_H_
#define _MAIN_H_

/*! \brief On-Board sensor list available for display
 *
 * This enumeration type stores the available list of sensors.
 */
enum sensor_display_t {
	//! Light Sensor
	LIGHT_SENSOR = 1,
	//! External Temperature Sensor
	TEMPERATURE_SENSOR = 2,
	//! Potentiometer Sensor
	POTENTIOMETER_SENSOR = 3,
	//! External Voltage Input
	EXT_VOLTAGE_INPUT = 4,
};

/*! \brief Called by HID interface
 * Callback running when USB Host enable generic interface
 *
 * \retval true if generic startup is ok
 */
bool main_hid_generic_enable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host disable generic interface
 */
void main_hid_generic_disable(void);

/*! \brief Called when a start of frame is received on USB line
 */
void main_sof_action(void);

/*! \brief Function executed when a report OUT from HID is received
 *
 * \param data Pointer to the received data buffer in SRAM
 */
void main_report_out(uint8_t *data);

/*! \brief Called by MSC interface
 *
 * Callback executed when USB Host enable MSC interface
 *
 * \retval true if MSC startup is ok
 */
bool main_msc_enable(void);

/*! \brief Called by MSC interface
 * Callback running when USB Host disable MSC interface
 */
void main_msc_disable(void);

/*! \brief Return the current sensor selected
 *
 * \retval Sensor selected
 */
enum sensor_display_t main_get_sensor_selected(void);

/*! \brief Notify that a external message is display
 * This function must look the display update during a specific timing
 */
void main_notify_external_msg(void);

/*! \brief Start the USB DFU bootloader execution from user application
 */
void main_start_dfu_session(void);

extern uint8_t xplain_b1_serial_number[];

//! The LCD scrolling delay in ms
#define LCD_SCROLLING_MS          300

//! The allowed delay to display user message on the LCD
#define LCD_USER_MSG_MS           5000

//! On-board sensors ADC refresh delay in ms
#define ADC_NEW_SAMPLE_MS         5

//! The interval to send new HID OUT report in ms
#define HID_SEND_REPORT_MS        10

//! The interval to send new HID OUT report in ms
#define LCD_SENSOR_MS             100

//! Firmware revision
#define FIRMWARE_REVISION         200

//! USBID position in user signature row
#define USER_SIGNATURE_USBID_POS  4

//! USBID size in user signature row
#define USER_SIGNATURE_USBID_SIZE 12

#endif // _MAIN_H_
