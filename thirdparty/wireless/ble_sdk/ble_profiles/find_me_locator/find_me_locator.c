/**
  * \file
  *
  * \brief Find Me Locator Profile
  *
  * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
  * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
  * Support</a>
  */
 
 /****************************************************************************************
 *							        Includes	                                     	*
 ****************************************************************************************/
 #include <asf.h>
 #include "immediate_alert.h"
 #include "ble_manager.h"
 #include "find_me_locator.h"
 
 /****************************************************************************************
 *							        Globals	                                     		*
 ****************************************************************************************/
 
 #if defined IMMEDIATE_ALERT_SERVICE
 gatt_ias_char_handler_t ias_handle =
 {0, 0, 0, AT_BLE_INVALID_PARAM, AT_BLE_INVALID_PARAM, NULL};
 uint8_t ias_char_data[MAX_IAS_CHAR_SIZE];
 #endif
 
 static const ble_gap_event_cb_t fmp_gap_handle = {
 	.connected = fmp_locator_connected_state_handler,
 	.disconnected = fmp_locator_disconnect_event_handler,
	.pair_done =  fmp_locator_pair_done_handler,
	.encryption_status_changed = fmp_locator_pair_done_handler
 };
 
 static const ble_gatt_client_event_cb_t fmp_gatt_client_handle = {
 	.primary_service_found = fmp_locator_service_found_handler,
 	.characteristic_found = fmp_locator_characteristic_found_handler,
 	.discovery_complete = fmp_locator_discovery_complete_handler,
 };
 
 /** @brief Alert value used for immediate alert service */
 uint8_t immediate_alert_value = INVALID_IAS_PARAM;

 /****************************************************************************************
 *							        Implementation	                                    *
 ****************************************************************************************/
 
 /**@brief Discover all services
 *
 * @param[in] handle connection handle
 .
 * @return @ref AT_BLE_SUCCESS operation programmed successfully.
 * @return @ref AT_BLE_INVALID_PARAM incorrect parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t fmp_locator_service_discover(at_ble_handle_t handle)
{
	at_ble_status_t status;
	status = at_ble_primary_service_discover_all(
					handle,
					GATT_DISCOVERY_STARTING_HANDLE,
					GATT_DISCOVERY_ENDING_HANDLE);
	if (status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("GATT Discovery request started ");
	} else {
		DBG_LOG("GATT Discovery request failed");
	}
	
	return status;
}

/* Callback registered for AT_BLE_CONNECTED_DONE event from stack */
at_ble_status_t fmp_locator_pair_done_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_pair_done_t *pair_done_val;
	pair_done_val = (at_ble_pair_done_t *)params;

	if(ble_check_iscentral(pair_done_val->handle))
	{
		if (pair_done_val->status == AT_BLE_SUCCESS)
		{
			status = fmp_locator_service_discover(pair_done_val->handle);
		}
		else
		{
			return pair_done_val->status;
		}
	}
	return status;
}

/**
 * @brief Service found handler invoked by ble manager
 */
at_ble_status_t fmp_locator_service_found_handler(void *param) 
{
	at_ble_primary_service_found_t * primary_service_params;
	primary_service_params = (at_ble_primary_service_found_t *)param;
	at_ble_uuid_t *ias_service_uuid;
	ias_service_uuid = &primary_service_params->service_uuid;
	if (ias_service_uuid->type == AT_BLE_UUID_16) {
		uint16_t service_uuid;
		service_uuid = ((ias_service_uuid->uuid[1] << 8) | 
						ias_service_uuid->uuid[0]);
		switch (service_uuid) {
			/* for Current Time Service*/
			case IMMEDIATE_ALERT_SERVICE_UUID:
			{
				ias_handle.start_handle
				= primary_service_params->start_handle;
				ias_handle.end_handle
				= primary_service_params->end_handle;
				DBG_LOG("Immediate Alert service discovered");
				DBG_LOG_PTS("start_handle: %04X end_handle: %04X ",
				primary_service_params->start_handle,
				primary_service_params->end_handle);
				ias_handle.service_discovery=(at_ble_status_t)DISCOVER_SUCCESS;
			}
			break;
			
			default:
			break;
		}
	}
	return AT_BLE_SUCCESS;
}


/**@brief Handles all Discovered characteristics of a given handler in a
* connected device
*
* @param[in] characteristic_found Discovered characteristics params of a
*connected device
*
*/
at_ble_status_t fmp_locator_characteristic_found_handler(void *params)
{
	uint16_t charac_16_uuid;
	at_ble_characteristic_found_t *characteristic_found;
	characteristic_found = (at_ble_characteristic_found_t *)params;
	
	if(!ble_check_iscentral(characteristic_found->conn_handle))
	{
		return AT_BLE_FAILURE;
	}

	charac_16_uuid = (uint16_t)((characteristic_found->char_uuid.uuid[0]) |	\
	(characteristic_found->char_uuid.uuid[1] << 8));

	if (charac_16_uuid == ALERT_LEVEL_CHAR_UUID) {
		ias_handle.char_handle = characteristic_found->value_handle;
		ias_handle.char_discovery = (at_ble_status_t)DISCOVER_SUCCESS;
		DBG_LOG_PTS("Immediate alert characteristics: Attrib handle %x property %x handle: %x uuid : %x",
		characteristic_found->char_handle, characteristic_found->properties,
		ias_handle.char_handle, charac_16_uuid);
	}
	return AT_BLE_SUCCESS;
}

/**@brief Discover all Characteristics supported for Immediate alert Service of a
* connected device
*
* @param[in] discover_status discovery status of each handle
*
*/
at_ble_status_t fmp_locator_discovery_complete_handler(void *params)
{
	at_ble_discovery_complete_t *discover_status;
	discover_status = (at_ble_discovery_complete_t *)params;
	
	if(!ble_check_iscentral(discover_status->conn_handle))
	{
		return AT_BLE_FAILURE;
	}
	
	DBG_LOG_DEV("discover complete operation %d and %d",discover_status->operation,discover_status->status);
	
	if ((discover_status->status == DISCOVER_SUCCESS) || (discover_status->status == AT_BLE_SUCCESS))
	{
		if(discover_status->operation == AT_BLE_DISC_ALL_SVC)
		{
			#if defined IMMEDIATE_ALERT_SERVICE
			if (ias_handle.service_discovery == DISCOVER_SUCCESS)
			{
				if (at_ble_characteristic_discover_all(
				discover_status->conn_handle,
				ias_handle.start_handle,
				ias_handle.end_handle) ==
				AT_BLE_SUCCESS)
				{
					DBG_LOG_DEV("Immediate Characteristic Discovery Started");
				}
				else
				{
					ias_handle.service_discovery = AT_BLE_FAILURE;
					DBG_LOG("Immediate Characteristic Discovery Failed");
				}
			}
			else if(ias_handle.service_discovery==AT_BLE_INVALID_PARAM)
			{
				DBG_LOG("Immediate Alert Service not Available");
				DBG_LOG("FIND ME PROFILE NOT SUPPORTED");
				at_ble_disconnect(discover_status->conn_handle, AT_BLE_TERMINATED_BY_USER);
			}
			#endif
		}
		else if(discover_status->operation == AT_BLE_DISC_ALL_CHAR)
		{
			if(ias_handle.char_discovery == AT_BLE_INVALID_PARAM)
			{
				DBG_LOG("Alert level characteristic not Available");
				DBG_LOG("FIND ME PROFILE NOT SUPPORTED");
				at_ble_disconnect(discover_status->conn_handle, AT_BLE_TERMINATED_BY_USER);
			}
		}
	}
	return AT_BLE_SUCCESS;
}


 /**
  * \Find me profile connected state handler function
  */
 at_ble_status_t fmp_locator_connected_state_handler(void * params)
 {
 	at_ble_status_t status;
 	at_ble_connected_t *conn_params;
 	conn_params = (at_ble_connected_t *)params;

#if BLE_PAIR_ENABLE == false
 	if(ble_check_iscentral(pair_done_val->handle))
 	{
	 	if (pair_done_val->status == AT_BLE_SUCCESS)
	 	{
		 	status = fmp_locator_service_discover(pair_done_val->handle);
	 	}
	 	else
	 	{
		 	return pair_done_val->status;
	 	}
 	}
#endif
	 
	ALL_UNUSED(conn_params);
	ALL_UNUSED(status);
 	return AT_BLE_SUCCESS;
 }
 
 /**
  * \Find me profile disconnected state handler function
  */
 at_ble_status_t fmp_locator_disconnect_event_handler(void * params)
 {
 	at_ble_disconnected_t *disconnect;
 	disconnect = (at_ble_disconnected_t *)params;
 	
	 
	 
	ALL_UNUSED(disconnect);
 	return AT_BLE_SUCCESS;
 }
 
 
 /**
  * \Find me Profile service initialization, declarations and advertisement
  */
 void fmp_locator_init(void)
 {
 	
 	/* Callback registering for BLE-GAP Role */
 	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, &fmp_gap_handle);
 	
 	/* Callback registering for BLE-GATT-Server Role */
 	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GATT_CLIENT_EVENT_TYPE, &fmp_gatt_client_handle);
 	
 }
