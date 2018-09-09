/**
 * \file
 *
 * \brief HID Mouse Device Profile Application declarations
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
/** @brief Mouse movement position */
#define MOUSE_MOVEMENT_POS 10

/** @brief Keypad debounce time */
#define KEY_PAD_DEBOUNCE_TIME	(200)

/** @brief Mouse change direction after number of attempt */
#define MOUSE_CHANGE_DIRECTION	(5)

/** @brief Button event ID */
#define APP_BUTTON_EVENT_ID		(1)

/****************************************************************************************
*							        Enumerations	                                   	*
****************************************************************************************/
/**@brief Mouse movement
*/
typedef enum
{
	MOUSE_RIGHT_MOVEMENT    = 1,
	MOUSE_LEFT_MOVEMENT,
	MOUSE_UP_MOVEMENT,
	MOUSE_DOWN_MOVEMENT,	
}mouse_move;

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
