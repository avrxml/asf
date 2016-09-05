/**
 * \file
 *
 * \brief Custom Serial Chat Service
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Custom Serial Chat Service
 */
/*- Includes ---------------------------------------------------------------*/
#include "stddef.h"
#include "stdio.h"
#include "string.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "cscs.h"

csc_serv_t csc_inst;

/**
* \CSC service Configuration function
*/
at_ble_status_t csc_serv_init(uint8_t *buf, uint16_t len)
{
	/* Configure SPOG Service UUID info*/
	csc_inst.serv_uuid.type =	 AT_BLE_UUID_128;
	memcpy(&csc_inst.serv_uuid.uuid[0], CSC_SERVICE_UUID, CSC_UUID_128_LEN);
	
	/* Configure SPOG Service Handle*/
	csc_inst.serv_handle = 0;
	
	/*Configure SPOG Endpoint Characteristic*/
	csc_inst.endpoint_chars.uuid.type = AT_BLE_UUID_128;
	memcpy(&csc_inst.endpoint_chars.uuid.uuid[0], CSC_ENDPOINT_CHAR_UUID, CSC_UUID_128_LEN);
	csc_inst.endpoint_chars.properties = AT_BLE_CHAR_NOTIFY;	
	csc_inst.endpoint_chars.init_value = buf;
	csc_inst.endpoint_chars.value_init_len = len;
	csc_inst.endpoint_chars.value_max_len = len;
	/* Configure the CSC characteristic permission */
	if(BLE_PAIR_ENABLE){
		csc_inst.endpoint_chars.value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);
		}else{
		csc_inst.endpoint_chars.value_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	}
	return(at_ble_primary_service_define(&csc_inst.serv_uuid, &csc_inst.serv_handle, NULL, 0, &csc_inst.endpoint_chars, 1));
}

/**
* \CSC service send data function
*/
at_ble_status_t csc_serv_send_data(uint16_t connhandle, uint8_t *databuf, uint16_t datalen)
{
	at_ble_status_t status;
	uint16_t value = 0;
	uint16_t length;
	length = sizeof(uint16_t);
	status = at_ble_characteristic_value_get(csc_inst.endpoint_chars.client_config_handle, (uint8_t *)&value, &length);
	if (status != AT_BLE_SUCCESS){
		DBG_LOG("at_ble_characteristic_value_get value get failed");
		return status;
	}
	if(value == 1){
		status = at_ble_characteristic_value_set(csc_inst.endpoint_chars.char_val_handle, databuf, datalen);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG("at_ble_characteristic_value_set value set failed");
			return status;
		}
		status = at_ble_notification_send(connhandle, csc_inst.endpoint_chars.char_val_handle);
		if (status != AT_BLE_SUCCESS){
			DBG_LOG("at_ble_notification_send  failed");
			return status;
		}
	}
	return status;
}
