/**
 * \file
 *
 * \brief Battery service
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
 * This is the reference manual for the Battery Service
 */
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/

#include "battery.h"

/** Initial value of the battery level characteristic value */
uint8_t battery_init_value = 10;

/** Presentation format of the battery level */
at_ble_char_presentation_t presentation_format;

bool volatile bat_notification_flag = false;

extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];

/**@brief Initialize the service with its included service, characteristics, and descriptors
 */
void bat_init_service(bat_gatt_service_handler_t *battery_serv, uint8_t *battery_value)
{
	battery_serv->serv_handle = 0;
	battery_serv->serv_uuid.type = AT_BLE_UUID_16;
	battery_serv->serv_uuid.uuid[0] = (uint8_t) BAT_SERVICE_UUID;
	battery_serv->serv_uuid.uuid[1] = (uint8_t) (BAT_SERVICE_UUID >> 8);
	
	//Battery service characteristic
	battery_serv->serv_chars.char_val_handle = 0;          /* handle stored here */
	battery_serv->serv_chars.uuid.type = AT_BLE_UUID_16;
	battery_serv->serv_chars.uuid.uuid[0] = (uint8_t) BAT_CHAR_BAT_LEVEL_UUID;          /* UUID : Manufacturer Name String */
	battery_serv->serv_chars.uuid.uuid[1] = (uint8_t) (BAT_CHAR_BAT_LEVEL_UUID >> 8);          /* UUID : Manufacturer Name String */
	battery_serv->serv_chars.properties = (AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY); /* Properties */
	battery_serv->serv_chars.init_value = &battery_init_value;             /* value */
	battery_serv->serv_chars.value_init_len = sizeof(uint8_t);
	battery_serv->serv_chars.value_max_len = sizeof(uint8_t);
#if BLE_PAIR_ENABLE
	battery_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_REQ_AUTHN_NO_AUTHR |
												 AT_BLE_ATTR_WRITABLE_REQ_AUTHN_NO_AUTHR);   /* permissions */
#else
	battery_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR |
												 AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);   /* permissions */
#endif
	battery_serv->serv_chars.user_desc = NULL;           /* user defined name */
	battery_serv->serv_chars.user_desc_len = 0;
	battery_serv->serv_chars.user_desc_max_len = 0;
	battery_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;             /*user description permissions*/
	battery_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*client config permissions*/
	battery_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;         /*server config permissions*/
	battery_serv->serv_chars.user_desc_handle = 0;             /*user desc handles*/
	battery_serv->serv_chars.client_config_handle = 0;         /*client config handles*/
	battery_serv->serv_chars.server_config_handle = 0;         /*server config handles*/
	
	presentation_format.format = AT_BLE_PRES_FORMAT_UINT8;
	presentation_format.exponent = BAT_CHAR_PRESENTATION_FORMAT_EXPONENT;
	presentation_format.unit = (uint8_t) BAT_CHAR_PRESENTATION_FORMAT_UNIT;
	presentation_format.unit = (uint8_t) (BAT_CHAR_PRESENTATION_FORMAT_UNIT >> 8);
	presentation_format.name_space = BAT_CHAR_PRESENTATION_FORMAT_NAMESPACE;
	presentation_format.description = (uint8_t) BAT_CHAR_PRESENTATION_FORMAT_DESCRIPTOR;
	presentation_format.description = (uint8_t) (BAT_CHAR_PRESENTATION_FORMAT_DESCRIPTOR >> 8);
	
	battery_serv->serv_chars.presentation_format = &presentation_format;       /* presentation format */
	ALL_UNUSED(battery_value);
}

/**@brief defining a initialized service 
 */
at_ble_status_t bat_primary_service_define(bat_gatt_service_handler_t *battery_service)
{
	return(at_ble_primary_service_define(&battery_service->serv_uuid,
	&battery_service->serv_handle,
	NULL, 0,
	&battery_service->serv_chars, 1));
}

/**@brief Function used to update characteristic value
 */
at_ble_status_t bat_update_char_value (at_ble_handle_t conn_handle, bat_gatt_service_handler_t *battery_serv , uint8_t char_data,bool volatile *flag)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	/* Updating the att data base */
	if ((status = at_ble_characteristic_value_set(battery_serv->serv_chars.char_val_handle, &char_data, sizeof(uint8_t))) != AT_BLE_SUCCESS){
		DBG_LOG("updating the characteristic failed%d",status);
		return status;
	} else {
		DBG_LOG_DEV("updating the characteristic value is successful");
	}

	if(bat_notification_flag){
		/* sending notification to the peer about change in the battery level */ 
		if((status = at_ble_notification_send(conn_handle, battery_serv->serv_chars.char_val_handle)) != AT_BLE_SUCCESS) {
			DBG_LOG("sending notification failed%d",status);
			return status;
		}
		else {
			DBG_LOG_DEV("sending notification successful");
			*flag = false;
			return status;
		}
	}
	return status;
}
/**@brief function to check the client characteristic configuration value. 
 */
at_ble_status_t bat_char_changed_event(at_ble_handle_t conn_handle, bat_gatt_service_handler_t *battery_service, at_ble_characteristic_changed_t *char_handle, bool volatile *flag)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, char_handle, sizeof(at_ble_characteristic_changed_t));
	
	if(battery_service->serv_chars.client_config_handle == change_params.char_handle)
	{
		if(change_params.char_new_value[0])
		{
			bat_notification_flag = true;
			/* sending notification to the peer about change in the battery level */
			if((status = at_ble_notification_send(conn_handle, battery_service->serv_chars.char_val_handle)) != AT_BLE_SUCCESS) {
				DBG_LOG("sending notification failed%d",status);
				return status;
			}
			else {
				DBG_LOG_DEV("sending notification successful");
				*flag = false;
				return status;
			}			
		}
		else
		{
			bat_notification_flag = false;			
		}
	}
	return status;
}
