/**
* \file
*
* \brief Link Loss Service Application declarations
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
// <h> Reference Time Update Service Configuration
// =======================
#ifndef __REFERENCE_TIME_H__
#define __REFERENCE_TIME_H__

/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#include "ble_manager.h"

/***********************************************************************************
 *									Macros			                               *
 **********************************************************************************/
//  <o> Characteristics read Lenght <0-100>
//  <i> Defines number of bytes to characteristics read length .
//  <i> Default: 20
//  <id> rtu_tp_cp_read_length
#define RTU_TP_CP_READ_LENGTH							(20)

#define RTU_TP_STATE_READ_LENGTH						(2)

/* Link Loss read offset length*/
//  <o> Characteristics read Offset <0-100>
//  <i> Defines offset value to read characteristics.
//  <i> Default: 0
//  <id> rtu_read_offset
#define RTU_READ_OFFSET									(0)

/* Reference Time Update invalid character handler*/
#define RTU_INVALID_CHAR_HANDLE							(0)

/***********************************************************************************
 *									Types		                               *
 **********************************************************************************/
typedef struct gatt_rtu_handler
{
	at_ble_handle_t start_handle;
	at_ble_handle_t end_handle;
	at_ble_status_t char_discovery;
	/* Time Update Control Point */
	at_ble_handle_t tp_control_char_handle;
	uint8_t *tp_control_char_data;
	/* Time Update State */
	at_ble_handle_t tp_state_char_handle;
	uint8_t *tp_state_char_data;
}gatt_rtu_handler_t;


/***********************************************************************************
 *									Functions		                               *
 **********************************************************************************/
/**@brief Send the Read request to the current time characteristic
 * Read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE
 *event
 *
 * @param[in] conn_handle handle of the connection
 * @param[in] char_handle handle of the characteristic
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_INVALID_PARAM Invalid arguments.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t tis_rtu_update_read(at_ble_handle_t conn_handle,
		at_ble_handle_t char_handle, uint16_t length);

/**@brief Read response handler for read response
 *
 * @param[in] read_value read response data available form
 *at_ble_characteristic_read_response_t
 * @return Current time value.
 * @return LLS_READ_RESP_INVALID if value are other than alert levels
 */
int8_t tis_rtu_update_read_response(at_ble_characteristic_read_response_t *read_resp,
		gatt_rtu_handler_t *rtu_handler);

/**@brief write Characteristic value giving its handle
 *
 * @param[in] conn_handle connection handle
 * @param[in] char_value_handle characteristic value handle
 * @param[in] char_value characteristic new value
 * @return Current time value.
 * @return LLS_READ_RESP_INVALID if value are other than alert levels
 */
at_ble_status_t tis_rtu_update_write(at_ble_handle_t conn_handle, at_ble_handle_t char_value_handle, uint8_t char_value);

#endif /* __REFERENCE_TIME_H__ */
// </h>

// <<< end of configuration section >>>
