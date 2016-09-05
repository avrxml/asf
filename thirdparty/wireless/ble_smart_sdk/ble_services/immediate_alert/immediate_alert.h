/**
 * \file
 *
 * \brief Immediate Alert Service declarations
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Immediate Alert Service Configuration
// =======================

#ifndef __IMMEDIATE_ALERT_H__
#define __IMMEDIATE_ALERT_H__

#include "ble_manager.h"

/** @brief count of included service in immediate alert service
  * 
  */
#define IAS_INCLUDED_SERVICE_COUNT	(0)

/** @brief count of characteristics in immediate alert service
  * 
  */
#define IAS_CHARACTERISTIC_COUNT	(1)

/** @brief Invalid parameter written
  * 
  */
#define INVALID_IAS_PARAM			(0xff)

/* Immediate alert levels */
typedef enum immediate_alert_level {
	IAS_NO_ALERT,
	IAS_MID_ALERT,
	IAS_HIGH_ALERT
} immediate_alert_level_t;


/****************************************************************************************
*							        Function Prototypes	                                *
****************************************************************************************/

//  <q> Enable Immediate Alert Service GATT Client Role
//  <i> Defines to support GATT Client Role
//  <id> ias_client
#define IAS_CLIENT 0

#if defined IAS_GATT_CLIENT

/* Maximum character data size to support */
#define MAX_IAS_CHAR_SIZE                       (1)

/* Character pointer to start data write */
#define IAS_WRITE_OFFSET                        (0)

/* Immediate Alert write length */
#define IAS_WRITE_LENGTH                        (1)

/* Immediate alert write without response */
#define IAS_WRITE_WITH_RESPONSE					        (false)

/* Perform the signed write  */
#define IAS_NO_SIGNED_WRITE                     (false)

typedef struct gatt_ias_char_handler
{
	at_ble_handle_t start_handle;
	at_ble_handle_t end_handle;
	at_ble_handle_t char_handle;
	at_ble_status_t char_discovery;
	uint8_t *char_data;
}gatt_ias_char_handler_t;


/**@brief write a characteristic  to Immediate Alert Characteristics
 *
 * if with_response is True, write completion will be reported via @ref
 *AT_BLE_CHARACTERISTIC_WRITE_RESPONSE event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @param[in] alert level need to write
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t ias_alert_level_write(at_ble_handle_t conn_handle,
		at_ble_handle_t char_handle,
		immediate_alert_level_t alert_level);

#endif /*__IAS_GATT_CLIENT__*/

//  <q> Enable Immediate Alert Service GATT Server Role
//  <i> Defines to support GATT Server Role
//  <id> ias_server
#define IAS_SERVER 0

#if defined IAS_GATT_SERVER

/** @brief Initialize the Immediate alert service with default values
  * 
  *
  * @param[in] gatt_service_handler_t  the service info which has handle range,uuid and characteristic array fields
  *
  * @pre Must be called before @ref ias_primary_service_define
  *
  * @return void
  */
void init_immediate_alert_service(gatt_service_handler_t *immediate_alert_serv);

/** @brief Defining Immediate alert service to the attribute data base
  * 
  *
  * @param[in] gatt_service_handler_t  the service info which has handle range,uuid and characteristic array fields
  *
  * @pre Must be called after @ref init_immediate_alert_service
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */

at_ble_status_t ias_primary_service_define(gatt_service_handler_t *ias_primary_service);

/** @brief sets the alert value of the immediate alert service
  * @param[in] gatt_service_handler_t  the service info which has handle range,uuid and characteristic array fields
  * @param[in] at_ble_characteristic_changed_t  the information of the characteristic changed.
  * @return @ref ias alert value
  */
uint8_t ias_set_alert_value(at_ble_characteristic_changed_t *change_params, gatt_service_handler_t *ias_handler);
#endif //IAS_GATT_SERVER

#endif /* __IMMEDIATE_ALERT_H__ */
// </h>

// <<< end of configuration section >>>
