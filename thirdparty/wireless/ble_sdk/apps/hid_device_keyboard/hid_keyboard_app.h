/**
 * \file
 *
 * \brief HID Keyboard Device Profile Application declarations
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 * Support</a>
 */

#ifndef __HID_DEVICE_APP_H__
#define __HID_DEVICE_APP_H__
#include "stdio.h"

/****************************************************************************************
*							        Macros	                                     							*
****************************************************************************************/
/** @brief Key pad debounce time */
#define KEY_PAD_DEBOUNCE_TIME	(200)

/** @brief Maximum text length */
#define MAX_TEXT_LEN			(11)

/** @brief Represent zero position */
#define POSITION_ZERO			(0)

/** @brief Represent six position */
#define POSITION_SIX			(6)

/** @brief Enable caps */
#define CAPS_ON					(2)

/** @brief Disable caps */
#define CAPS_OFF				(0)

/** @brief Button event ID */
#define APP_BUTTON_EVENT_ID		(1)

/****************************************************************************************
*							        Function                                     		*
****************************************************************************************/

/** @brief Callback call during custom event */
static at_ble_status_t hid_custom_event(void *param);

/** @brief Callback call during connect event */
static at_ble_status_t hid_connect_cb(void *params);

/** @brief Callback call during disconnect event */
static at_ble_status_t hid_disconnect_cb(void *params);

/** @brief Callback call during notification confirmation */
static at_ble_status_t hid_notification_confirmed_cb(void *params);
#endif /*__HID_DEVICE_APP_H__*/
