/**
* \file
*
* \brief Reference Time Update Service
*
* Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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
