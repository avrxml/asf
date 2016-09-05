/**
 * \file
 *
 * \brief TX Service declarations
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
// <h> Tx Power Service Configuration
// =======================

#ifndef __TX_POWER_H__
#define __TX_POWER_H__

#include "ble_manager.h"

//  <q> Enable Tx Power Service GATT Server Role
//  <i> Defines to support GATT Server Role
//  <id> txps_server
#define TXPS_SERVER 0

 #if defined TXPS_GATT_SERVER

/****************************************************************************************
*							        Macros	                                     		*
****************************************************************************************/

/** @brief  DEFAULT_TX_PWR_VALUE the tx power value*/
#define DEFAULT_TX_PWR_VALUE			(0)


/** @brief count of included service in tx power service
  * 
  */
#define TXPS_INCLUDED_SERVICE_COUNT		(0)

/** @brief count of characteristics in tx power service
  * 
  */
#define TXPS_CHARACTERISTIC_COUNT		(1)

typedef struct gatt_txps_char_handler
{
	at_ble_handle_t start_handle;
	at_ble_handle_t end_handle;
	at_ble_handle_t char_handle;
	at_ble_status_t char_discovery;
	uint8_t *char_data;
}gatt_txps_char_handler_t;


/****************************************************************************************
*							        Function Declarations	                            *
****************************************************************************************/

/** @brief Initialize the tx power service with default values
  * 
  *
  * @param[in] gatt_service_handler_t  the service info which has handle range,uuid and characteristic array fields
  *
  * @pre Must be called before @ref txps_primary_service_define
  *
  * @return void
  */

void init_tx_power_service(gatt_service_handler_t *tx_power_serv);



/** @brief Defining the tx power service to the attribute data base
  * 
  *
  * @param[in] gatt_service_handler_t  the service info which has handle range,uuid and characteristic array fields
  *
  * @pre Must be called after @ref init_tx_power_service
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
at_ble_status_t txps_primary_service_define(gatt_service_handler_t *txps_primary_service);
#endif//TXPS_GATT_SERVER

//  <q> Enable Tx Power Service GATT Client Role
//  <i> Defines to support GATT Client Role
//  <id> txps_client
#define TXPS_CLIENT 0

#if defined TXPS_GATT_CLIENT
/* Tx Power Maximum Character byte support */
#define MAX_TX_POWER_CHAR_SIZE                  (1)

/* Tx Power offset at where to start reading */
#define TXPS_POWER_READ_OFFSET                  (0)

/* Tx Power length of data to read */
#define TXPS_POWER_READ_LENGTH                  (1)

/* Tx Power invalid Characteristics handler */
#define TXPS_INVALID_CHAR_HANDLE                (0)

/* Tx Power invalid read Power value */
#define TXPS_INVALID_POWER_VALUE                (0xFF)

typedef struct gatt_txps_char_handler
{
	at_ble_handle_t start_handle;
	at_ble_handle_t end_handle;
	at_ble_handle_t char_handle;
	at_ble_status_t char_discovery;
	uint8_t *char_data;
}gatt_txps_char_handler_t;

/**@brief Send the Read Request to Tx Power service
 *
 * Read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE
 *event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_INVALID_PARAM Invalid arguments.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t txps_power_read(at_ble_handle_t conn_handle,
    at_ble_handle_t char_handle);

/**@brief Read a Tx Power
 *
 * @param[in] read_value read response data available form
 *at_ble_characteristic_read_response_t
 * @return TX power in dBm .
 * @return @ref TXPS_INVALID_POWER_VALUE if is not valid result
 */
int8_t txps_power_read_response(
    at_ble_characteristic_read_response_t *char_read_resp,
    gatt_txps_char_handler_t *txps_handler);
#endif //TXPS_GATT_CLIENT

#endif /* __TX_POWER_H__ */
// </h>

// <<< end of configuration section >>>
