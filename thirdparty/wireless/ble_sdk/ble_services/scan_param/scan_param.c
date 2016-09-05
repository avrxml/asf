/**
 * \file
 *
 * \brief Scan Parameters service
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

 /**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Scan Param Service
 */
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/


#include "scan_param.h"


/**@brief Initialize the service with its included service, characteristics, and descriptors
 *
 * @param[in] scan_param gatt service information
 *
 * @return none
 */

extern at_ble_connected_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];

bool volatile sps_notification_flag = false;

/**@brief Initialize the service with its included service, characteristics, and descriptors
 */
void sps_init_service(sps_gatt_service_handler_t *sps_serv, uint16_t *scan_interval_window, uint8_t *scan_refresh)
{
		sps_serv->serv_handle= 0;
		sps_serv->serv_uuid.type= AT_BLE_UUID_16;
		sps_serv->serv_uuid.uuid[0]= (uint8_t)SPS_SERVICE_UUID;
		sps_serv->serv_uuid.uuid[1]= (SPS_SERVICE_UUID >> 8);
		
		sps_serv->serv_chars[0].char_val_handle = 0;          /* handle stored here */
		sps_serv->serv_chars[0].uuid.type = AT_BLE_UUID_16;
		sps_serv->serv_chars[0].uuid.uuid[0] = (uint8_t)SPS_CHAR_SCAN_INT_VALUE_UUID;          /* UUID : Scan Interval Value */
		sps_serv->serv_chars[0].uuid.uuid[1] = (SPS_CHAR_SCAN_INT_VALUE_UUID >> 8);          /* UUID : Scan Interval Value */
		sps_serv->serv_chars[0].properties = AT_BLE_CHAR_WRITE_WITHOUT_RESPONSE; /* Properties */
		sps_serv->serv_chars[0].init_value = (uint8_t *)scan_interval_window;             /* value */
		sps_serv->serv_chars[0].value_init_len = (sizeof(uint16_t) * 2);
		sps_serv->serv_chars[0].value_max_len =  (sizeof(uint16_t) * 2);
		sps_serv->serv_chars[0].value_permissions = AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;   /* permissions */											
		sps_serv->serv_chars[0].user_desc = NULL;           /* user defined name */
		sps_serv->serv_chars[0].user_desc_len = 0;
		sps_serv->serv_chars[0].user_desc_max_len = 0;
		sps_serv->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
		sps_serv->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
		sps_serv->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
		sps_serv->serv_chars[0].user_desc_handle = 0;             /*user desc handles*/
		sps_serv->serv_chars[0].client_config_handle = 0;         /*client config handles*/
		sps_serv->serv_chars[0].server_config_handle = 0;         /*server config handles*/
		sps_serv->serv_chars[0].presentation_format = NULL;       /* presentation format */
		
		sps_serv->serv_chars[1].char_val_handle = 0;          /* handle stored here */
		sps_serv->serv_chars[1].uuid.type = AT_BLE_UUID_16;
		sps_serv->serv_chars[1].uuid.uuid[0] = (uint8_t) SPS_CHAR_SCAN_REFRESH_UUID;          /* UUID : Tx Power Level */
		sps_serv->serv_chars[1].uuid.uuid[1] = (uint8_t) (SPS_CHAR_SCAN_REFRESH_UUID >> 8);          /* UUID : Tx Power Level */
		sps_serv->serv_chars[1].properties = AT_BLE_CHAR_NOTIFY; /* Properties */
		sps_serv->serv_chars[1].init_value = scan_refresh;             /* value */
		sps_serv->serv_chars[1].value_init_len = sizeof(uint8_t);
		sps_serv->serv_chars[1].value_max_len =  sizeof(uint8_t);
		sps_serv->serv_chars[1].value_permissions = AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR;   /* permissions */
		sps_serv->serv_chars[1].user_desc = NULL;           /* user defined name */
		sps_serv->serv_chars[1].user_desc_len = 0;
		sps_serv->serv_chars[1].user_desc_max_len = 0;
		sps_serv->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
#if BLE_PAIR_ENABLE
		sps_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR;         /*client config permissions*/
#else 
		sps_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
#endif
		sps_serv->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
		sps_serv->serv_chars[1].user_desc_handle = 0;             /*user desc handles*/
		sps_serv->serv_chars[1].client_config_handle = 0;         /*client config handles*/
		sps_serv->serv_chars[1].server_config_handle = 0;         /*server config handles*/
		sps_serv->serv_chars[1].presentation_format = NULL;       /* presentation format */
}

/**@brief defining a initialized service 
 */
at_ble_status_t sps_primary_service_define(sps_gatt_service_handler_t *sps_service)
{
	return(at_ble_primary_service_define(&sps_service->serv_uuid,
	&sps_service->serv_handle,
	NULL, 0,
	sps_service->serv_chars, 2));
}

/**@brief Function used to update the scan refresh characteristic value during connection
 */
at_ble_status_t sps_scan_refresh_char_update(sps_gatt_service_handler_t *sps_serv ,uint8_t scan_refresh_value, bool volatile *flag)
{
	//updating application data
	sps_serv->serv_chars[1].init_value = &scan_refresh_value;
	
	//updating the att data base
	if ((at_ble_characteristic_value_set(sps_serv->serv_chars[1].char_val_handle, &scan_refresh_value, sizeof(uint8_t))) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Updating scan refresh characteristic failed\r\n");
		return AT_BLE_FAILURE;
	} 
	
	//sending notification to the peer about change in the scan parameters
	if(sps_notification_flag){
		if((at_ble_notification_send(ble_dev_info[0].handle, sps_serv->serv_chars[1].char_val_handle)) == AT_BLE_FAILURE) {
			DBG_LOG("sending notification failed");
			return AT_BLE_FAILURE;
		}
		else 
		{
			DBG_LOG_DEV("sending notification successful");
			*flag = false;
			return AT_BLE_SUCCESS;
		}
	}
	
	return AT_BLE_FAILURE;
}

/**@brief function to check the client characteristic configuration value. 
 */
at_ble_status_t	sps_char_changed_event(sps_gatt_service_handler_t *sps_service_handler, at_ble_characteristic_changed_t *char_handle, bool volatile *flag)
{
	at_ble_characteristic_changed_t change_params;
	uint16_t scan_interval_window[2];
	memcpy((uint8_t *)&change_params, char_handle, sizeof(at_ble_characteristic_changed_t));
	
	if(sps_service_handler->serv_chars[0].char_val_handle == change_params.char_handle)
	{
		memcpy(sps_service_handler->serv_chars[0].init_value, change_params.char_new_value, change_params.char_len);
		memcpy(&scan_interval_window[0], &change_params.char_new_value[0], sizeof(uint16_t));
		memcpy(&scan_interval_window[1], &change_params.char_new_value[2], sizeof(uint16_t));		
		DBG_LOG("New scan interval window parameter");
		DBG_LOG("Scan Interval %d ms",scan_interval_window[0]);
		DBG_LOG("Scan Window   %d ms",scan_interval_window[1]);
	}
	
	if(sps_service_handler->serv_chars[1].client_config_handle == change_params.char_handle)
	{
		if(change_params.char_new_value[0])
		{
			sps_notification_flag = true;
			if((at_ble_notification_send(change_params.conn_handle, sps_service_handler->serv_chars[1].char_val_handle)) == AT_BLE_FAILURE) {
				DBG_LOG("sending notification failed");
				return AT_BLE_FAILURE;
			}
			else
			{
				DBG_LOG_DEV("sending notification successful");
				*flag = false;
				return AT_BLE_SUCCESS;
			}
		}
		else
		{
			sps_notification_flag = false;
		}
	}
	return AT_BLE_SUCCESS;	
}
