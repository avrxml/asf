/**
 * \file
 *
 * \brief Common User Interface for USB host Android Open Accessory Protocol
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

#ifndef _UI_H_
#define _UI_H_

#include "uhc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Initializes the user interface */
void ui_init(void);

/*! \brief Notify that the USB mode is switched automatically.
 * This is only possible when the ID pin is available.
 *
 * \param b_host_mode true, if the host mode has been selected
 */
void ui_usb_mode_change(bool b_host_mode);

/*! \brief Notify that a Vbus is changed
 * Available only in USB hardware with Vbus monitoring.
 *
 * \param b_vbus_present true, if Vbus is high.
 */
void ui_usb_vbus_change(bool b_vbus_present);

/*! \brief Notify that a Vbus error has occurred
 * Available only in USB hardware with Vbus monitoring.
 */
void ui_usb_vbus_error(void);

/*! \brief Notify that a USB device has been connected or disconnected.
 *
 * \param dev         Pointer on USB device information
 * \param b_present   true, if the device has been connected
 */
void ui_usb_connection_event(uhc_device_t *dev, bool b_present);

/** \brief Notify that a USB device or the host woke up the USB line */
void ui_usb_wakeup_event(void);

/** \brief Notify that a SOF has been sent (each 1 ms) */
void ui_usb_sof_event(void);

/*! \brief Notify the end of a USB device enumeration process
 *
 * \param dev         Pointer on USB device information
 * \param status      Status of the USB enumeration
 */
void ui_usb_enum_event(uhc_device_t *dev, uhc_enum_status_t status);

void ui_uhi_aoa_change(uhc_device_t * dev, bool b_plug);

/** \brief Processing of a Message from the Android device */
void ui_usb_message_reception(void);

/** \brief Called when Bulk IN Transfer is finished */
void read_complete(usb_add_t add, usb_ep_t ep, uhd_trans_status_t, iram_size_t);

/**
 * \name Commands from accessory to Android device
 * @{
 */
#define MESSAGE_ATD_SIMPLE_SWITCH            0x01

#define MESSAGE_ATD_SENSOR_TEMPERATURE       0x10
#define MESSAGE_ATD_SENSOR_LIGHT             0x11
/* @} */

/**
 * \name Commands from Android device to accessory
 * @{
 */
#define MESSAGE_DTA_SIMPLE_LED               0x02
#define MESSAGE_DTA_SIMPLE_DISPLAY_BACKLIGHT 0x03

#define MESSAGE_DTA_STRING_BASIC             0x20
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* _UI_H_ */
