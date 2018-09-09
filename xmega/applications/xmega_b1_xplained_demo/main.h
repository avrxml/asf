/**
 * \file
 *
 * \brief XMEGA-B1 Xplained Demonstration
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
