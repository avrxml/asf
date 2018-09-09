/**
 * \file
 *
 * \brief Battery service
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
	battery_serv->serv_chars.init_value = battery_value;             /* value */
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
at_ble_status_t bat_update_char_value (at_ble_handle_t conn_handle, bat_gatt_service_handler_t *battery_serv , uint8_t char_data, bool notify)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	/* Updating the ATT data base */
	if ((status = at_ble_characteristic_value_set(battery_serv->serv_chars.char_val_handle, &char_data, sizeof(uint8_t))) != AT_BLE_SUCCESS)
	{
		DBG_LOG("updating the characteristic failed%d",status);
		return status;
	} 
	else
	{
		DBG_LOG_DEV("updating the characteristic value is successful");
		
		if (notify)
		{
			/* sending notification to the peer about change in the battery level */
			if((status = at_ble_notification_send(conn_handle, battery_serv->serv_chars.char_val_handle)) != AT_BLE_SUCCESS) {
				DBG_LOG("sending notification failed%d",status);
				return status;
			}
			else {
				DBG_LOG_DEV("sending notification successful");
			}
		}
		
	}

	return status;
}
/**@brief function to check the client characteristic configuration value. 
 */
at_ble_status_t bat_char_changed_event(bat_gatt_service_handler_t *battery_service, at_ble_characteristic_changed_t *characteristic_changed_param)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	if((battery_service->serv_chars.client_config_handle == characteristic_changed_param->char_handle) && (characteristic_changed_param->char_new_value[0] == 0x01))
	{
		status = AT_BLE_SUCCESS;
	}
	else if((battery_service->serv_chars.client_config_handle == characteristic_changed_param->char_handle) && (characteristic_changed_param->char_new_value[0] == 0x00))
	{
		status = AT_BLE_PRF_NTF_DISABLED;
	}
	return status;
}
