/**
* \file
*
* \brief Time Information Profile
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
#include "at_ble_errno.h"
#include "at_ble_api.h"
#include "current_time.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "time_info.h"

#if defined CURRENT_TIME_SERVICE
#include "current_time.h"
#endif

#if defined NEXT_DST_SERVICE
#include "next_dst.h"
#endif

#if defined REFERENCE_TIME_SERVICE
#include "reference_time.h"
#endif


/***********************************************************************************
 *									Globals			                               *
 **********************************************************************************/
bonding_complete_t bonding_cb = NULL;

#if defined CURRENT_TIME_SERVICE
/**@brief CTS Characteristic Value array*/
uint8_t curr_char_data[CTS_READ_LENGTH];
/**@brief CTS Characteristic Value array*/
uint8_t lti_char_data[CTS_READ_LENGTH];
/**@brief CTS Characteristic Value array*/
uint8_t rti_char_data[CTS_READ_LENGTH];
/**@brief CTS Service Handle*/
gatt_cts_handler_t cts_handle = {0, 0, AT_BLE_INVALID_PARAM, AT_BLE_INVALID_PARAM, 
								0, NULL, 0, 0, NULL, 0, NULL};
#endif

#if defined NEXT_DST_SERVICE
/**@brief Time with DST Characteristic Value array*/
uint8_t dst_char_data[DST_READ_LENGTH];
/**@brief Next DST Change Service Handle*/
gatt_dst_handler_t dst_handle = {0, 0, 0, AT_BLE_INVALID_PARAM, NULL};
#endif

#if defined REFERENCE_TIME_SERVICE
/**@brief Time Update Control Point Characteristic Value array*/
uint8_t tp_control_char_data[RTU_TP_CP_READ_LENGTH];

/**@brief Time Update State Characteristic Value array*/
uint8_t tp_state_char_data[RTU_TP_CP_READ_LENGTH];

/**@brief Reference Time Update Service Handle*/
gatt_rtu_handler_t rtu_handle = {0, 0, AT_BLE_INVALID_PARAM, 0, NULL, 0, NULL};
#endif

/**@breif Peer Connected device info*/
extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];

volatile bool current_time_char_found = false;
volatile bool local_time_char_found = false;
volatile bool ref_time_char_found = false;
volatile bool time_with_dst_char_found = false;
volatile bool time_update_cp_char_found = false;
volatile bool time_update_state_char_found = false;
volatile bool Desc_found = false;

read_response_callback_t read_response_callback = NULL;

static const ble_event_callback_t time_info_gatt_client_handle[] = {
	time_info_service_found_handler,
	NULL,
	time_info_characteristic_found_handler,
	time_info_descriptor_found_handler,
	time_info_discovery_complete_handler,
	time_info_characteristic_read_response,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t ble_mgr_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	time_info_service_discover,
	time_info_disconnected_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
static at_ble_handle_t time_info_conn_handle = 0;
/***********************************************************************************
 *									Implementations	                               *
 **********************************************************************************/
/**
 * @brief time info advertisement data and start of advertisement data
 */
void time_info_adv()
{	
	at_ble_status_t status;
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return;
	}
	
	if ((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, 
						NULL, AT_BLE_ADV_FP_ANY, APP_TP_FAST_ADV, 
						APP_TP_ADV_TIMEOUT, 0)) != AT_BLE_SUCCESS) {
		DBG_LOG("BLE Adv start Failed :%d",status);
		return;
	} else {
		DBG_LOG("Device is in Advertising Mode");
	}
}

/**
 * @brief Time Information profile initializations and setting of device in advertising mode
 */
void time_info_init(void)
{
	#if defined CURRENT_TIME_SERVICE
	cts_handle.curr_char_data = curr_char_data;
	cts_handle.lti_char_data = lti_char_data;
	cts_handle.rti_char_data = rti_char_data;
	#endif
	
	#if defined NEXT_DST_SERVICE
	dst_handle.dst_char_data = dst_char_data;
	#endif
	
	#if defined REFERENCE_TIME_SERVICE
	rtu_handle.tp_control_char_data = tp_control_char_data;
	rtu_handle.tp_state_char_data = tp_state_char_data;
	#endif
	
	time_info_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									ble_mgr_gap_handle);
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_CLIENT_EVENT_TYPE,
									time_info_gatt_client_handle);
}

/**
 * @brief Discovering the services of time server used by applications
 */
at_ble_status_t time_info_service_discovery(void)
{
	if (at_ble_primary_service_discover_all(time_info_conn_handle, 
			GATT_DISCOVERY_STARTING_HANDLE, GATT_DISCOVERY_ENDING_HANDLE)
			== AT_BLE_SUCCESS) {
		DBG_LOG_DEV("GATT Discovery request started ");
		return AT_BLE_SUCCESS;
	} else {
		DBG_LOG("GATT Discovery request failed");
		return AT_BLE_FAILURE;
	}
}

/**
 * @brief Discovering the services of time server
 */
at_ble_status_t time_info_service_discover(void *param)
{	
	at_ble_connected_t *conn_params = (at_ble_connected_t *)param;
	if (conn_params->conn_status != AT_BLE_SUCCESS) {
		return conn_params->conn_status;
	}
	time_info_conn_handle = conn_params->handle;
	return time_info_service_discovery();
}

/**
 * @brief Handler for connection event 
 */
at_ble_status_t time_info_connected_state_handler(void *param)
{
	at_ble_connected_t *conn_params;
	conn_params = (at_ble_connected_t *)param;
	at_ble_status_t discovery_status = AT_BLE_FAILURE;
	at_ble_status_t status;
	if (conn_params->conn_status==AT_BLE_SUCCESS) {
		DBG_LOG_DEV("BLE: Security Procedure");
		/*Start the security procedure*/
		status = ble_send_slave_sec_request(conn_params->handle);
		if (status != AT_BLE_SUCCESS) {
			DBG_LOG("BLE: Fail to start security procedure. status = %d", 
					status);
		}
	}
	return discovery_status;
}

/**
 * @brief Service found handler invoked by ble manager
 */
at_ble_status_t time_info_service_found_handler(void *param)
{
	at_ble_primary_service_found_t * primary_service_params;
	primary_service_params = (at_ble_primary_service_found_t *)param;
	at_ble_uuid_t *ctx_service_uuid;
	ctx_service_uuid = &primary_service_params->service_uuid;
	if (ctx_service_uuid->type == AT_BLE_UUID_16) {
		uint16_t service_uuid;
		service_uuid = ((ctx_service_uuid->uuid[1] << 8) | 
						ctx_service_uuid->uuid[0]);
		switch (service_uuid) {
			/* for Current Time Service*/
			case CURRENT_TIME_SERVICE_UUID:
			{
				cts_handle.start_handle = primary_service_params->start_handle;
				cts_handle.end_handle = primary_service_params->end_handle;
				DBG_LOG_PTS("current time service discovered(service uuid: %x) start %x end %x",
							service_uuid, cts_handle.start_handle,
							cts_handle.end_handle);
				cts_handle.char_discovery = AT_BLE_SUCCESS;
			}
			break;
			
			/* for Next DST Change service*/
			case NEXT_DST_SERVICE_UUID:
			{
				dst_handle.start_handle = primary_service_params->start_handle;
				dst_handle.end_handle = primary_service_params->end_handle;
				DBG_LOG_PTS("Next DST Change Service discovered(service uuid: %x)\
							start %x end %x",
							service_uuid, dst_handle.start_handle,
							dst_handle.end_handle);
				dst_handle.char_discovery = AT_BLE_SUCCESS;
			}
			break;
			
			/* for Reference Time Update Service*/
			case REFERENCE_TIME_SERVICE_UUID:
			{
				rtu_handle.start_handle = primary_service_params->start_handle;
				rtu_handle.end_handle = primary_service_params->end_handle;
				DBG_LOG_PTS("Reference time update service discovered(service uuid: %x) start %x end %x",
							service_uuid, rtu_handle.start_handle,
							rtu_handle.end_handle);
				rtu_handle.char_discovery = AT_BLE_SUCCESS;
			}
			break;
			
			default:
			break;
		}
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief Discovery Complete handler invoked by ble manager
 */
at_ble_status_t time_info_discovery_complete_handler(void *param)
{
	at_ble_discovery_complete_t *discover_status = (at_ble_discovery_complete_t *)param;
	bool discover_char_flag = true;
	if (discover_status->status == AT_BLE_DISCOVER_SUCCESS || 
		discover_status->status == AT_BLE_SUCCESS) {
		#if defined CURRENT_TIME_SERVICE
		if ((cts_handle.char_discovery == AT_BLE_SUCCESS) && 
			(discover_char_flag)) {
			if (at_ble_characteristic_discover_all(
			discover_status->conn_handle,
			cts_handle.start_handle,
			cts_handle.end_handle) == AT_BLE_SUCCESS) {
				discover_char_flag = false;
				DBG_LOG_DEV("CTS Characteristic Discovery Started");
			} else {
				DBG_LOG_DEV("CTS Characteristic Discovery Failed");
			}
			cts_handle.char_discovery = AT_BLE_FAILURE;
		} else if (cts_handle.char_discovery == AT_BLE_INVALID_PARAM) {
			DBG_LOG("Current time Service not Found");
			cts_handle.char_discovery = AT_BLE_INVALID_STATE;
		}
		
		if ((cts_handle.desc_discovery == AT_BLE_SUCCESS) && 
			(discover_char_flag)) {
			if (at_ble_descriptor_discover_all(
			discover_status->conn_handle,
			cts_handle.start_handle,
			cts_handle.end_handle) == AT_BLE_SUCCESS) {
				discover_char_flag = false;
				DBG_LOG_DEV("CTS Descriptors Discovery Started");
			} else {
				DBG_LOG_DEV("CTS Descriptors Discovery Failed");
			}
			cts_handle.desc_discovery = AT_BLE_FAILURE;
		}
		#endif
		
		#if defined NEXT_DST_SERVICE
		if ((dst_handle.char_discovery == AT_BLE_SUCCESS) && 
			(discover_char_flag)) {
			if (at_ble_characteristic_discover_all(
			discover_status->conn_handle,
			dst_handle.start_handle,
			dst_handle.end_handle) == AT_BLE_SUCCESS) {
				discover_char_flag = false;
				DBG_LOG_DEV("DST Characteristic Discovery Started");
			} else {
				DBG_LOG_DEV("DST Characteristic Discovery Failed");
			}
			dst_handle.char_discovery = AT_BLE_FAILURE;
		} else if (dst_handle.char_discovery == AT_BLE_INVALID_PARAM) {
			DBG_LOG("Next DST Change Service not Found");
			dst_handle.char_discovery = AT_BLE_INVALID_STATE;
		}
		#endif
		
		#if defined REFERENCE_TIME_SERVICE
		if ((rtu_handle.char_discovery == AT_BLE_SUCCESS) && 
			(discover_char_flag)) {
			if (at_ble_characteristic_discover_all(
			discover_status->conn_handle,
			rtu_handle.start_handle,
			rtu_handle.end_handle) == AT_BLE_SUCCESS) {
				discover_char_flag = false;
				DBG_LOG_DEV("RTU Characteristic Discovery Started");
			} else {
				DBG_LOG_DEV("RTU Characteristic Discovery Failed");
			}
			rtu_handle.char_discovery = AT_BLE_FAILURE;
		} else if (rtu_handle.char_discovery == AT_BLE_INVALID_PARAM) {
			DBG_LOG("Reference Time Update Service not Found");
			rtu_handle.char_discovery = AT_BLE_INVALID_STATE;
		}
		#endif
		
		if((cts_handle.char_discovery == AT_BLE_INVALID_STATE) && 
			(discover_char_flag)) {
			DBG_LOG("TIME INFOMATION PROFILE NOT SUPPORTED");
			discover_char_flag = false;
			if(at_ble_disconnect(discover_status->conn_handle, 
								AT_BLE_TERMINATED_BY_USER) != AT_BLE_SUCCESS) {
				DBG_LOG("disconnection failed");
			}
		}
		
		if (discover_char_flag) {
			DBG_LOG("GATT characteristic discovery completed");
			if(ble_send_slave_sec_request(discover_status->conn_handle) 
										  == AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Successfully send Slave Security Request");
			} else {
				DBG_LOG("Fail to send Slave Security Request");
			}
		}
	} else {
		DBG_LOG("discover complete failure %d",discover_status->status);
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief characteristic found handler invoked by ble manager
 */
at_ble_status_t time_info_descriptor_found_handler(void *param)
{
	at_ble_descriptor_found_t *descriptor_found = (at_ble_descriptor_found_t *)param;
	uint16_t desc_16_uuid ;
	
	desc_16_uuid = (uint16_t)((descriptor_found->desc_uuid.uuid[0]) | \
	(descriptor_found->desc_uuid.uuid[1] << 8));
	 if (desc_16_uuid == CTS_CLIENT_CHAR_DESCRIPTOR) {
		 cts_handle.curr_desc_handle = descriptor_found->desc_handle;
		 Desc_found = true;
		 DBG_LOG_PTS("Current Time Descriptor uuid : %x handle: %x", desc_16_uuid,
						cts_handle.curr_desc_handle);
	 }
	return AT_BLE_SUCCESS;
}
/**
 * @brief characteristic found handler invoked by ble manager
 */
at_ble_status_t time_info_characteristic_found_handler(void *param)
{
	at_ble_characteristic_found_t *characteristic_found = (at_ble_characteristic_found_t *)param;
	uint16_t charac_16_uuid ;
	
	charac_16_uuid = (uint16_t)((characteristic_found->char_uuid.uuid[0]) | \
	(characteristic_found->char_uuid.uuid[1] << 8));
	
	if (charac_16_uuid == CURRENT_TIME_CHAR_UUID) {
		current_time_char_found = true;
		cts_handle.curr_char_handle = characteristic_found->value_handle;
		DBG_LOG_PTS("current time characteristics: Attrib handle %x property %x handle: %x uuid : %x",
					characteristic_found->char_handle, characteristic_found->properties,
					cts_handle.curr_char_handle, charac_16_uuid);
		cts_handle.desc_discovery = AT_BLE_SUCCESS;
	}
	
	if (charac_16_uuid == LOCAL_TIME_CHAR_UUID) {
		local_time_char_found = true;
		cts_handle.lti_char_handle = characteristic_found->value_handle;
		DBG_LOG_PTS("Local time characteristics: Attrib handle %x property %x handle: %x uuid : %x",
					characteristic_found->char_handle, characteristic_found->properties,
					cts_handle.lti_char_handle, charac_16_uuid);
	}
	
	if (charac_16_uuid == REF_TIME_CHAR_UUID) {
		ref_time_char_found = true;
		cts_handle.rti_char_handle = characteristic_found->value_handle;
		DBG_LOG_PTS("Reference time characteristics: Attrib handle %x property %x handle: %x uuid : %x",
					characteristic_found->char_handle, characteristic_found->properties,
					cts_handle.rti_char_handle, charac_16_uuid);
	}
	
	if (charac_16_uuid == TIME_WITH_DST_CHAR_UUID) {
		time_with_dst_char_found = true;
		dst_handle.dst_char_handle = characteristic_found->value_handle;
		DBG_LOG_PTS("Time with DST characteristics: Attrib handle %x property %x handle: %x uuid : %x",
					characteristic_found->char_handle, characteristic_found->properties,
					dst_handle.dst_char_handle, charac_16_uuid);
	}

	if (charac_16_uuid == TIME_UPDATE_CP_CHAR_UUID) {
		time_update_cp_char_found = true;
		rtu_handle.tp_control_char_handle = characteristic_found->value_handle;
		DBG_LOG_PTS("Time Update Control Point characteristics: Attrib handle %x property %x handle: %x uuid : %x",
					characteristic_found->char_handle, characteristic_found->properties,
					rtu_handle.tp_control_char_handle, charac_16_uuid);
	}
	
	if (charac_16_uuid == TIME_UPDATE_STATE_CHAR_UUID) {
		time_update_state_char_found = true;
		rtu_handle.tp_state_char_handle = characteristic_found->value_handle;
		DBG_LOG_PTS("Time Update State characteristics: Attrib handle %x property %x handle: %x uuid : %x",
					characteristic_found->char_handle, characteristic_found->properties,
					rtu_handle.tp_state_char_handle, charac_16_uuid);
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief time read response handler invoked by ble manager
 */
at_ble_status_t time_info_characteristic_read_response(void *param)
{
	at_ble_characteristic_read_response_t *char_read_resp = (at_ble_characteristic_read_response_t *)param;
	if (char_read_resp->status == AT_BLE_GAP_TIMEOUT) {
		DBG_LOG("Characteristic read response failed %d(timeout)", 
				char_read_resp->status);
		at_ble_disconnect(char_read_resp->conn_handle, 
						AT_BLE_TERMINATED_BY_USER);
		return char_read_resp->status;
	}
	#if defined CURRENT_TIME_SERVICE
		tis_current_time_read_response(char_read_resp, &cts_handle);
	#endif
	
	#if defined NEXT_DST_SERVICE
		tis_dst_change_read_response(char_read_resp, &dst_handle);
	#endif
	
	#if defined REFERENCE_TIME_SERVICE
		tis_rtu_update_read_response(char_read_resp, &rtu_handle);
	#endif
	if (read_response_callback) {
		read_response_callback(char_read_resp);
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief char notification received handler invoked by ble manager
 */
at_ble_status_t time_info_notification_handler(void *param)
{
	at_ble_notification_recieved_t *noti_read_resp = (at_ble_notification_recieved_t *)param;
	at_ble_characteristic_read_response_t char_read_resp;
	
	char_read_resp.char_handle = noti_read_resp->char_handle;
	char_read_resp.char_len = noti_read_resp->char_len;
	memcpy( &char_read_resp.char_value, noti_read_resp->char_value, 
			char_read_resp.char_len);
	
	#if defined CURRENT_TIME_SERVICE
	tis_current_time_read_response(&char_read_resp, &cts_handle);
	#endif
	
	#if defined NEXT_DST_SERVICE
	tis_dst_change_read_response(&char_read_resp, &dst_handle);
	#endif
	
	#if defined REFERENCE_TIME_SERVICE
	tis_rtu_update_read_response(&char_read_resp, &rtu_handle);
	#endif
	return AT_BLE_SUCCESS;
}


/**
 * @brief disconnected event handler invoked by ble manager
 */
at_ble_status_t time_info_disconnected_event_handler(void *param)
{
	at_ble_disconnected_t *disconnect = (at_ble_disconnected_t *)param;
	cts_handle.char_discovery = AT_BLE_INVALID_PARAM;
	cts_handle.desc_discovery = AT_BLE_INVALID_PARAM;
	dst_handle.char_discovery = AT_BLE_INVALID_PARAM;
	rtu_handle.char_discovery = AT_BLE_INVALID_PARAM;
	current_time_char_found = false;
	local_time_char_found = false;
	ref_time_char_found = false;
	time_with_dst_char_found = false;
	time_update_cp_char_found = false;
	time_update_state_char_found = false;
	Desc_found = false;
	
	if (at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, 
						NULL, AT_BLE_ADV_FP_ANY, APP_TP_FAST_ADV, 
						APP_TP_ADV_TIMEOUT, 0) != AT_BLE_SUCCESS) {
		DBG_LOG("BLE Adv start Failed");
	} else {
		DBG_LOG("BLE Started Adv");
	}	
        UNUSED(disconnect);
		return AT_BLE_SUCCESS;
}

/**
 * @brief write notification handler invoked by ble manager
 */
 void time_info_write_notification_handler(uint16_t value)
 {
 	/* To enable notification */
 	if(Desc_found) {
		if (!(tis_current_time_noti(ble_dev_info[0].conn_info.handle, 
 				cts_handle.curr_desc_handle, (uint8_t *)&value) == AT_BLE_SUCCESS)) {
 			DBG_LOG("Fail to set Current Time descriptor 0");
 		}
 	}
 }

/**
 * @brief Application registering callback for both paired done event 
 * \and encryption status changed event
 */
void time_info_register_bonding_callback(bonding_complete_t bonding_complete_cb)
{
	bonding_cb = bonding_complete_cb;
}

/**
 * @brief Handler for AT_BLE_PAIR_DONE event from stack
 */
at_ble_status_t time_info_pair_done_handler(void *param)
{
	if (bonding_cb) {
		bonding_cb(true);		
	}
    UNUSED(param);
	return AT_BLE_SUCCESS;
}

/**
 * @brief Application registering callback for characteristic read response received 
 */
void time_info_register_read_response_callback(read_response_callback_t read_response_cb)
{
	if (read_response_cb) {
		read_response_callback = read_response_cb;
	}
}

/**
 * @brief Handler for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack
 */ 
void time_info_encryption_status_changed_handler(at_ble_encryption_status_changed_t *param)
{
	if (bonding_cb) {
		bonding_cb(true);
	}
    UNUSED(param);
}
