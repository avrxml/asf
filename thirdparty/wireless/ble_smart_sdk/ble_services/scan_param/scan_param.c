/**
 * \file
 *
 * \brief Scan Parameters service
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

extern at_ble_connected_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];

bool volatile sps_notification_flag = false;
static at_ble_status_t sps_disconnected_event_handler(void *params);

static const ble_gap_event_cb_t sps_gap_cb = {
	.disconnected = sps_disconnected_event_handler,
};

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t sps_disconnected_event_handler(void *params)
{
	at_ble_disconnected_t *disconnected = (at_ble_disconnected_t *)params;
	sps_notification_flag = false;
	ALL_UNUSED(disconnected);
	return AT_BLE_SUCCESS;
}
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
		
		/* Register callbacks for gap related events */
		ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
										BLE_GAP_EVENT_TYPE,
										&sps_gap_cb);
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
at_ble_status_t sps_scan_refresh_char_update(sps_gatt_service_handler_t *sps_serv ,uint8_t scan_refresh_value)
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
			return AT_BLE_SUCCESS;
		}
	}
	
	return AT_BLE_FAILURE;
}

/**@brief function to check the client characteristic configuration value. 
 */
at_ble_status_t	sps_char_changed_event(sps_gatt_service_handler_t *sps_service_handler, at_ble_characteristic_changed_t *char_handle)
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
		if(change_params.char_new_value[0] == SCP_NOTIFICATION_ENABLE)
		{
			sps_notification_flag = true;

#ifndef SAMB11          /* stuck when try sending notification in here on SAMB11 */
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

#endif
		}
		else
		{
			sps_notification_flag = false;
		}
	}
	return AT_BLE_SUCCESS;	
}
