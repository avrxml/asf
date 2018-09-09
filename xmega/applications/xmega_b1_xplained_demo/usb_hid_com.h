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
#ifndef USB_HID_COM_H_INCLUDED
#define USB_HID_COM_H_INCLUDED

#include "asf.h"
#include "main.h"
#include "ui.h"

/**
 * \brief Structure containing the HID report OUT
 *
 * This structure is used to store the on-board  leds and lcd state
 * received from the USB host using generic HID report OUT.
 */
struct hid_report_out {
	union {
		struct {
			uint8_t leds;
			uint8_t lcd_notify;
			uint8_t lcd_user_text[LCD_TEXT_STRING_SIZE];
			uint16_t lcd_user_num;
			uint8_t contrast;
			uint8_t backlight;
		};
		uint8_t byte[UDI_HID_REPORT_OUT_SIZE];
	};
};

/**
 * \brief Structure containing the HID report IN
 *
 * This structure is used to store the on-board sensors measurements and status
 * sent to the USB host using generic HID report IN.
 */
struct hid_report_in {
	union {
		struct {
			uint8_t buttons;
			int8_t temperature;
			uint16_t light;
			uint16_t potentiometer;
			uint16_t ext_voltage;
			uint8_t sensor_selected;
			uint8_t silicon_revision;
			uint8_t firmware_revision;
			uint8_t firmware_si_target;
		};
		uint8_t byte[UDI_HID_REPORT_IN_SIZE];
	};
};

/*! \brief Function executed when a HID OUT report has been received
 *
 * \param feature Pointer to the received data buffer in SRAM
 */
void usb_hid_com_report_out_received(uint8_t *data);

/*! \brief Allows to send a HID IN report to the host
 *
 * This function is used to send a HID IN report the host
 */
void usb_hid_com_send_report_in(void);

/*! \brief Initialize the HID IN report
 *
 */
void usb_hid_com_init(void);

/*! \brief Function executed when an HID SET FEATURE request is received
 *
 * \param feature Pointer to the received data buffer in SRAM
 */
void usb_hid_com_set_feature(uint8_t *feature);

/**
 * \brief Test is a start DFU session is requested.
 *
 * \return true when a start DFU request has been received.
 */
bool usb_hid_com_is_start_dfu(void);

#endif /* USB_HID_COM_H_INCLUDED */
