/**
* \file
*
* \brief Reference Time Update Service
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
*    Atmel micro controller product.
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

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the Reference Time Update Service
*/
/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/

#include <string.h>
#include "at_ble_api.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "reference_time.h"

/***********************************************************************************
 *									Implementations	                               *
 **********************************************************************************/
/**@brief write Characteristic value giving its handle
 */
at_ble_status_t tis_rtu_update_write(at_ble_handle_t conn_handle, at_ble_handle_t char_value_handle, uint8_t char_value)
{
	if (char_value_handle == RTU_INVALID_CHAR_HANDLE) {
		return (AT_BLE_INVALID_STATE);
	}
	return(at_ble_characteristic_write(conn_handle, char_value_handle, 0, 1, 
									   &char_value, false, false));
}
/**@brief Send the Read request to the current time characteristic
 * Read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE
 *event
 */
at_ble_status_t tis_rtu_update_read(at_ble_handle_t conn_handle,
		at_ble_handle_t char_handle, uint16_t length)
{
	if (char_handle == RTU_INVALID_CHAR_HANDLE) {
		return (AT_BLE_INVALID_STATE); 
	}
	return (at_ble_characteristic_read(conn_handle,char_handle,RTU_READ_OFFSET,length));
}

/**@brief Read response handler for read response for time characteristic
 */
int8_t tis_rtu_update_read_response(at_ble_characteristic_read_response_t *read_resp,
		gatt_rtu_handler_t *rtu_handler)
{
	if (read_resp->status != AT_BLE_SUCCESS)
		return read_resp->status;
	else if (read_resp->char_handle == rtu_handler->tp_state_char_handle) {
			DBG_LOG("Source = %02d",read_resp->char_value[0]);
			DBG_LOG("Result = %02d",read_resp->char_value[1]);
	}
	return 0;
}
