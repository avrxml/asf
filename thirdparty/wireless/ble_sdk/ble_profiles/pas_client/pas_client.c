/**
* \file
*
* \brief Phone Alert Status Profile
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
* This is the reference manual for the Time Information Profile
*/
/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#include <string.h>
#include "at_ble_api.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "pas_client.h"
#include "pas_service.h"

/***********************************************************************************
 *									Globals			                               *
 **********************************************************************************/
pas_service_handler_t pas_service_data;

/* Application call backs */
read_callback_t alert_status_read_cb;
read_callback_t ringer_setting_read_cb;

notification_callback_t alert_status_notification_cb;
notification_callback_t ringer_setting_notification_cb;

static const ble_event_callback_t pas_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	pas_client_service_discovery,
	pas_client_disconnected_event_handler,
	NULL,
	NULL,
	pas_client_write_notifications,
	NULL,
	NULL,
	NULL,
	NULL,
	pas_client_write_notifications,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t pas_gatt_client_handle[] = {
	pas_client_service_found_handler,
	NULL,
	pas_client_characteristic_found_handler,
	pas_client_descriptor_found_handler,
	pas_client_discovery_complete_handler,
	pas_client_char_read_response_handler,
	NULL,
	pas_client_char_write_response_handler,
	pas_client_notification_handler,
	NULL
};

/***********************************************************************************
 *									Implementation	                               *
 **********************************************************************************/
/*
 *@brief initializes the profile data to the default values
 *@param[in] none
 *@return	none
 */
void pas_data_init(void)
{
	memset(&pas_service_data,0,sizeof(pas_service_data));
}

/**
 * @brief register the call back for reading alert status
 * @param[in] read_callback_t type application callback
 * @return none
 */
void register_alert_status_read_callback(read_callback_t app_read_cb)
{
	alert_status_read_cb = app_read_cb ;
}

/**
 * @brief register the call back for reading ringer setting 
 * @param[in] read_callback_t type application callback
 * @return none
 */
void register_ringer_setting_read_callback(read_callback_t app_read_cb)
{
	ringer_setting_read_cb = app_read_cb;
}

/**
 * @brief register the call back for notification of alert status  
 * @param[in] notification_callback_t type application callback
 * @return none
 */
void register_alert_status_notification_callback(notification_callback_t app_notify_cb)
{
	alert_status_notification_cb = app_notify_cb ;
}

/**
 * @brief register the call back for notification of ringer setting
 * @param[in] notification_callback_t type application callback
 * @return none
 */
void register_ringer_setting_notification_callback(notification_callback_t app_notify_cb)
{
	ringer_setting_notification_cb = app_notify_cb ;
}

/**
 * @brief sets the advertisement data and triggers advertisement
 * @param[in] none
 * @return none
 */
void pas_client_adv(void)
{
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
	APP_PAS_FAST_ADV, 0, 0) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Failed to start advertisement");
	} else {
		DBG_LOG("Device is in Advertising Mode");
	}
}

/**
 * @brief starts the service discovery
 * @return AT_BLE_SUCCESS for success and AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_client_start_service_discovery(void )
{
	at_ble_uuid_t pas_uuid;	
	pas_uuid.type = AT_BLE_UUID_16;
	pas_uuid.uuid[0] =  (uint8_t) PAS_SERVICE_UUID;
	pas_uuid.uuid[1] = (uint8_t ) (PAS_SERVICE_UUID >> 8 );
	
	if(at_ble_primary_service_discover_by_uuid(pas_service_data.conn_handle,
												GATT_DISCOVERY_STARTING_HANDLE,
												GATT_DISCOVERY_ENDING_HANDLE,
												&pas_uuid) == AT_BLE_SUCCESS) {
			DBG_LOG_DEV("GATT Discovery request started ");
			return AT_BLE_SUCCESS;
	} else {
			DBG_LOG("GATT Discovery request failed");
	}
	
	return AT_BLE_FAILURE;
}

/**
 * @brief to perform the service discovery
 * @param[in] connection parameters 
 * @return AT_BLE_SUCCESS for success and AT_BLE_FAILURE for failure
 * @pre Called after connection by the ble manager
 */
at_ble_status_t pas_client_service_discovery(void *params)
{
	at_ble_connected_t conn_params;
	memcpy(&conn_params,params,sizeof(at_ble_connected_t));
	
	if (conn_params.conn_status != AT_BLE_SUCCESS) {
		return conn_params.conn_status;
	}
	
	pas_service_data.conn_handle = conn_params.handle;
	
	return pas_client_start_service_discovery();
}

/**
 * @brief Discovery Complete handler invoked by ble manager
 * @param[in] at_ble_discovery_complete_t disconnected handler 
 */
at_ble_status_t pas_client_discovery_complete_handler(void *params)
{
		at_ble_status_t status;
		at_ble_discovery_complete_t discover_status;
		memcpy((uint8_t *)&discover_status, params, sizeof(at_ble_discovery_complete_t));
		
		DBG_LOG_DEV("The discovery complete operation %d status %x",discover_status.operation,discover_status.status);
		
		if ((discover_status.status == AT_BLE_SUCCESS) || (discover_status.status == AT_BLE_ATT_ATTRIBUTE_NOT_FOUND)) {
			if(discover_status.operation == AT_BLE_DISC_BY_UUID_SVC) {
				if (pas_service_data.pas_service_info.discovery) {
					if((status = at_ble_characteristic_discover_all(pas_service_data.conn_handle,
					pas_service_data.pas_service_info.start_handle,
					pas_service_data.pas_service_info.end_handle)) != AT_BLE_SUCCESS) {
						DBG_LOG("Fail to start characteristic discovery,reason %x",status);
					}
					pas_service_data.pas_service_info.discovery = 0;
					} else  {
					DBG_LOG("Phone alert status not found initiating disconnection");
					at_ble_disconnect(pas_service_data.conn_handle,AT_BLE_TERMINATED_BY_USER);
				}
			} else if(discover_status.operation == AT_BLE_DISC_ALL_CHAR) {
				if (pas_service_data.ringer_control_point_char.discovery) {
					if((status = at_ble_descriptor_discover_all(pas_service_data.conn_handle,
					pas_service_data.pas_service_info.start_handle,
					pas_service_data.pas_service_info.end_handle)) != AT_BLE_SUCCESS) {
						DBG_LOG("Fail to start Descriptor Discovery Failed,reason %x",status);
					}
					pas_service_data.ringer_control_point_char.discovery = 0;
				}
			} else if(discover_status.operation == AT_BLE_DISC_DESC_CHAR) {
				DBG_LOG_DEV("Descriptor discovery successfull");
				#if !BLE_PAIR_ENABLE
					pas_client_write_notifications(NULL);
				#endif
			}	
		} else {
			DBG_LOG("discovery operation not successfully");
		}
			return AT_BLE_SUCCESS;
}
		
/**
 * @brief Service found handler invoked by ble manager
 * @param[in] at_ble_primary_service_found_t invoked when a primary service is found in peer device
 */
 at_ble_status_t pas_client_service_found_handler(void * params)
{
	at_ble_uuid_t *pas_service_uuid;
	at_ble_primary_service_found_t * primary_service_params = (at_ble_primary_service_found_t * )params;
	uint16_t uuid;
	pas_service_uuid = &primary_service_params->service_uuid;
	memcpy(&uuid,&primary_service_params->service_uuid.uuid,2);
	DBG_LOG_PTS("service found with uuidtype %d %x",primary_service_params->service_uuid.type,uuid);
	if	(pas_service_uuid->type == AT_BLE_UUID_16) {
		uint16_t service_uuid = (uint16_t)((primary_service_params->service_uuid.uuid[0]) |	\
		(primary_service_params->service_uuid.uuid[1] << 8));
		
		/* for Phone Alert Status Service*/
		if(service_uuid == PAS_SERVICE_UUID) {
			pas_service_data.pas_service_info.start_handle = primary_service_params->start_handle;
			pas_service_data.pas_service_info.end_handle   = primary_service_params->end_handle;
		
			DBG_LOG_PTS("Phone Alert Status Service Discovered  %04X %04X", pas_service_data.pas_service_info.start_handle,
			 pas_service_data.pas_service_info.end_handle);
			DBG_LOG("Phone Alert Status Service discovered in the remote device");
			pas_service_data.pas_service_info.discovery = true;
		}
	}
		return AT_BLE_SUCCESS;
}


/**
 * @brief characteristic found handler invoked by ble manager
 * @param[in] at_ble_characteristic_found_t when a characteristic is found in peer device
 */
at_ble_status_t pas_client_characteristic_found_handler(void *params)
{
	uint16_t charac_16_uuid;
	at_ble_characteristic_found_t *characteristic_found = (at_ble_characteristic_found_t *)params;
	DBG_LOG_DEV("Characteristic found %d",characteristic_found->char_uuid.type);
	
	charac_16_uuid = (uint16_t)((characteristic_found->char_uuid.uuid[0]) | \
	(characteristic_found->char_uuid.uuid[1] << 8));
	
	
	switch(charac_16_uuid) {
		/* Alert status characteristic */
		case ALERT_STATUS_UUID :
		{
			pas_service_data.alert_status_char.char_handle = characteristic_found->char_handle;
			pas_service_data.alert_status_char.value_handle = characteristic_found->value_handle;
			pas_service_data.alert_status_char.properties = characteristic_found->properties;
			
			DBG_LOG_PTS("Supported Alert Status characteristics %04X",pas_service_data.alert_status_char.char_handle);
			DBG_LOG_PTS("Characteristic Info ConnHandle 0x%02x : Char handle 0x%02x : Value handle : 0x%02x : Properties : 0x%02x",
			characteristic_found->conn_handle,
			characteristic_found->char_handle,
			characteristic_found->value_handle,
			characteristic_found->properties);
			DBG_LOG_PTS("UUID : 0x%02x%02x",characteristic_found->char_uuid.uuid[1],characteristic_found->char_uuid.uuid[0]);
			
			pas_service_data.alert_status_char.discovery = true;
		}
		break;
		
		/* Ringer Setting characteristic */
		case RINGER_SETTING_UUID:
		{
			pas_service_data.ringer_setting_char.char_handle = characteristic_found->char_handle;
			pas_service_data.ringer_setting_char.value_handle = characteristic_found->value_handle;
			pas_service_data.ringer_setting_char.properties = characteristic_found->properties;
			
			DBG_LOG_PTS("Ringer Setting Characteristics %04X",pas_service_data.ringer_setting_char.char_handle);
			DBG_LOG_PTS("Characteristic Info ConnHandle 0x%02x : Char handle 0x%02x : Value handle : 0x%02x : Properties : 0x%02x",
			characteristic_found->conn_handle,
			characteristic_found->char_handle,
			characteristic_found->value_handle,
			characteristic_found->properties);
			DBG_LOG_PTS("UUID : 0x%02x%02x",characteristic_found->char_uuid.uuid[1],characteristic_found->char_uuid.uuid[0]);
			
			pas_service_data.ringer_setting_char.discovery = true;
		}
		break;
		
		/* Ringer Control Point Characteristic */
		case RINGER_CONTROL_POINT_UUID:
		{
			pas_service_data.ringer_control_point_char.char_handle = characteristic_found->char_handle;
			pas_service_data.ringer_control_point_char.value_handle = characteristic_found->value_handle;
			pas_service_data.ringer_control_point_char.properties = characteristic_found->properties;
			
			DBG_LOG_PTS("Ringer Control Point characteristics %04X",pas_service_data.ringer_control_point_char.char_handle);
			DBG_LOG_PTS("Characteristic Info ConnHandle 0x%02x : Char handle 0x%02x : Value handle : 0x%02x : Properties : 0x%02x",
			characteristic_found->conn_handle,
			characteristic_found->char_handle,
			characteristic_found->value_handle,
			characteristic_found->properties);
			DBG_LOG_PTS("UUID : 0x%02x%02x",characteristic_found->char_uuid.uuid[1],characteristic_found->char_uuid.uuid[0]);
			pas_service_data.ringer_control_point_char.discovery = true;
		}
		break;	
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief client descriptor found handler invoked by ble manager
 * @param[in] at_ble_descriptor_found_t invoked when a descriptor is found in peer device
 */
at_ble_status_t pas_client_descriptor_found_handler(void *param)
{
		uint16_t desc_uuid;
		DBG_LOG_DEV("Descriptor discovered");
		at_ble_descriptor_found_t *params = (at_ble_descriptor_found_t *)param;
		if (params->desc_uuid.type == AT_BLE_UUID_16) {
			desc_uuid = (uint16_t)((params->desc_uuid.uuid[0]) | \
			(params->desc_uuid.uuid[1] << 8));
			
			if (desc_uuid == CLIENT_CONF_CHAR_DESCRIPTOR_UUID) {
				DBG_LOG_DEV("Descriptor found for ***");
				if (params->desc_handle > pas_service_data.alert_status_char.char_handle &&
					params->desc_handle < pas_service_data.ringer_setting_char.char_handle) {
					pas_service_data.alert_status_desc.desc_handle = params->desc_handle;
					pas_service_data.alert_status_desc.discovery = true;		
					DBG_LOG_PTS("The alert status desc handle is %x",pas_service_data.alert_status_desc.desc_handle);
					DBG_LOG_PTS("The descriptor uuid is %x",desc_uuid);
				} else if (params->desc_handle > pas_service_data.ringer_setting_char.char_handle &&
							params->desc_handle < pas_service_data.ringer_control_point_char.char_handle) {
					pas_service_data.ringer_setting_desc.desc_handle = params->desc_handle;
					pas_service_data.ringer_setting_desc.discovery = true;
					DBG_LOG_PTS("The ringer setting desc handle is %x",pas_service_data.ringer_setting_desc.desc_handle);
					DBG_LOG_PTS("The descriptor uuid is %x",desc_uuid);
				}
			}
		}		
		return AT_BLE_SUCCESS;	
}


/**
 * @brief disconnected event handler invoked by ble manager
 * @param[in] at_ble_disconnected_t contains the disconnection info
 */
at_ble_status_t pas_client_disconnected_event_handler(void *params)
{
	at_ble_disconnected_t disconnect;
	memcpy((uint8_t *)&disconnect, params, sizeof(at_ble_disconnected_t));
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}


/**
 * @brief invoked by ble manager on receiving notification
 * @param[in] at_ble_notification_recieved_t notification information containing handle data
 */
at_ble_status_t pas_client_notification_handler(void *params)
{
	 at_ble_notification_recieved_t notification;
	 memcpy((uint8_t *)&notification, params, sizeof(at_ble_notification_recieved_t));
	 DBG_LOG_DEV("Notification received handle %x  ",notification.char_handle);
	 if (notification.char_handle == (pas_service_data.alert_status_char.value_handle)) {
		 //Calling application notification handler for alert status characteristic
		 alert_status_notification_cb(notification.char_value,notification.char_len);
		 
	 } else if(notification.char_handle == (pas_service_data.ringer_setting_char.value_handle)) {
		 
		 //Calling application notification handler for ringer status characteristic
		 ringer_setting_notification_cb(notification.char_value,notification.char_len);
	 }
	 return AT_BLE_SUCCESS;
}

/**
 * @brief invoked by ble manager for setting the write response handler 
 * @param[in] at_ble_characteristic_write_response_t response data contains status and handle
 */
at_ble_status_t pas_client_char_write_response_handler(void *event_params)
{
	at_ble_characteristic_write_response_t *params = (at_ble_characteristic_write_response_t *)event_params;
	DBG_LOG("Write Response received");
	if (params -> status == AT_BLE_SUCCESS) {
		if (params->char_handle == pas_service_data.alert_status_desc.desc_handle) {
			DBG_LOG_DEV("Setting the notification for alert status characteristic is successfully");
		} else if (params->char_handle == pas_service_data.ringer_setting_desc.desc_handle) {
			DBG_LOG_DEV("Setting the notification for ringer setting is successful");
		}
	} else {
		DBG_LOG("Setting Failed %d",params->status);
		DBG_LOG("Char handle %d",params->char_handle);
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief called by the ble manager after receiving the read response event 
 * @param[in] characteristic read response parameters
 * @return none
 */
at_ble_status_t pas_client_char_read_response_handler(void *event_params)
{
	at_ble_characteristic_read_response_t *params = (at_ble_characteristic_read_response_t *)event_params;
	if (params ->status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("offset is %d",params->char_offset);
		if (params ->char_handle == pas_service_data.alert_status_char.value_handle) {
			// call application for read alert status char
			alert_status_read_cb(params->char_value,params->char_len);
		} else if (params ->char_handle == pas_service_data.ringer_setting_char.value_handle) {
			
			// call application for read of ringer setting char
			ringer_setting_read_cb(params->char_value,params->char_len);
		}
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief invoked by app to read the alert status characteristic
 * @return AT_BLE_SUCCESS for success or refer at_ble_err_status_t
 */
at_ble_status_t pas_client_read_alert_status_char(void)
{
	//invoke service function for read
	return (pas_read_alert_status_char(pas_service_data.conn_handle,
	pas_service_data.alert_status_char.value_handle));
}

/**
 * @brief invoked by app to read the write status characteristic
 * @return AT_BLE_SUCCESS for success or refer at_ble_err_status_t
 */
at_ble_status_t pas_client_read_ringer_setting_char(void)
{
	//invoke service function for read
	return (pas_read_ringer_setting_char(pas_service_data.conn_handle,
	pas_service_data.ringer_setting_char.value_handle));
}

/**
 * @brief invoked by app to read the write ringer control point characteristic
 * @return AT_BLE_SUCCESS for success or refer at_ble_err_status_t*/
at_ble_status_t pas_client_write_ringer_control_point(uint8_t ringer)
{
	// invoke service function for write	
	return (pas_char_set_ringer_control_point(pas_service_data.conn_handle,
	(pas_service_data.ringer_control_point_char.value_handle ),ringer));
}

/**
 * @brief invoked by ble manager for setting the notification of characteristics
 * @param[in] char_id represents the characteristic,1 for alert status,0 for ringer setting
 * @param[in] enable weather notifications has to enabled or disabled.1 for enable 0 for disable
 * @return	 AT_BLE_SUCCESS for success or refer at_ble_err_status_t
 */
at_ble_status_t pas_client_enable_char_notification(bool char_id,bool enable)
{
	if (char_id)
	{
		return (pas_char_write_notification(pas_service_data.conn_handle,
		pas_service_data.alert_status_desc.desc_handle,enable));
	} else {
		return (pas_char_write_notification(pas_service_data.conn_handle,
		pas_service_data.ringer_setting_desc.desc_handle,enable));
	}
}

/**
 * @brief invoked by ble manager for setting the notification 
 */
at_ble_status_t pas_client_write_notifications(void *params)
{
	at_ble_status_t status;
	DBG_LOG("Enabling notifications");
	if ((status = pas_client_enable_char_notification(0,1)) != AT_BLE_SUCCESS) {
		DBG_LOG("notification enabling failed");
	}
	
	if ((status = pas_client_enable_char_notification(1,1)) != AT_BLE_SUCCESS) {
		DBG_LOG("notification enabling failed");
	}
        ALL_UNUSED(status);
        ALL_UNUSED(params);
		return AT_BLE_SUCCESS;
}

/**
 * @brief invoked by ble manager for initializing the profile
 */
void pas_client_init( void *params)
{
	at_ble_status_t status;
	
	pas_data_init();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	pas_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_CLIENT_EVENT_TYPE,
	pas_gatt_client_handle);
	
	status = ble_advertisement_data_set();
	if (status != AT_BLE_SUCCESS) {
		DBG_LOG("Advertisement data set failed reason %d",status);
	}
		
	UNUSED(params);
}
