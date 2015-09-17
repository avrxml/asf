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
