/**
* \file
*
* \brief Next DST Change Service
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
* This is the reference manual for the Next DST Change Service
*/
/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/

#include <string.h>
#include "at_ble_api.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "next_dst.h"

/***********************************************************************************
 *									Implementations	                               *
 **********************************************************************************/
/**@brief Send the Read request to the DST change characteristic
 * Read value will be reported via @ref AT_BLE_CHARACTERISTIC_READ_RESPONSE
 *event
 */
at_ble_status_t tis_dst_change_read(at_ble_handle_t conn_handle,
		at_ble_handle_t char_handle)
{
	if (char_handle == DST_INVALID_CHAR_HANDLE) {
		return (AT_BLE_INVALID_STATE);
	}
	return (at_ble_characteristic_read(conn_handle, char_handle, 
										DST_READ_OFFSET, DST_READ_LENGTH));
}

/**@brief Read response handler for read response for dst change characteristic
 */
int8_t tis_dst_change_read_response(at_ble_characteristic_read_response_t *read_resp,
		gatt_dst_handler_t *dst_handler)
{
	if (read_resp->status != AT_BLE_SUCCESS) {
		return read_resp->status;
	}
	if (read_resp->char_handle == dst_handler->dst_char_handle) {
		DBG_LOG("DST Time is Time:");		
		DBG_LOG_CONT("[DD:MM:YYYY]: %02d-%02d-%02d [HH:MM:SS]: %02d:%02d:%02d  DST Offset is :%02d",
		read_resp->char_value[3],
		read_resp->char_value[2],
		((uint16_t)read_resp->char_value[0] | (read_resp->char_value[1] <<8)),
		read_resp->char_value[4],
		read_resp->char_value[5],
		read_resp->char_value[6],
		read_resp->char_value[7]);
	}
	return AT_BLE_SUCCESS;
}


