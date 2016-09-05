/**
 * \file
 *
 * \brief Custom Serial Chat Profile
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
 * This is the reference manual for the Custom Serial Chat Profile
 */
/*- Includes ---------------------------------------------------------------*/
#include <asf.h>

#include "string.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "cscp.h"
#include "cscs.h"

///* Scan response data */
//uint8_t scan_rsp_data[SCAN_RESP_LEN] = {0x09,0xff, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};
	
static const ble_event_callback_t csc_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	csc_prf_connected_state_handler,
	csc_prf_disconnect_event_handler,
	NULL,
	NULL,
	csc_prf_write_notification_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	csc_prf_write_notification_handler,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t csc_gatt_client_handle[] = {
	csc_prf_service_found_handler,
	NULL,
	csc_prf_characteristic_found_handler,
	csc_prf_descriptor_found_handler,
	csc_prf_discovery_complete_handler,
	NULL,
	NULL,
	NULL,
	csc_prf_notification_handler,
	NULL
};


static const ble_event_callback_t csc_gatt_server_handle[] = {
	csc_notification_confirmation_handler,
	NULL,
	csc_prf_char_changed_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

	
/*Profile Information*/
app_csc_data_t app_csc_info;	

/* Notification callback function pointer */
recv_ntf_callback_t recv_ntf_cb;

/**
* \CSC buffer initialization function
*/
void csc_prf_buf_init(uint8_t *databuf, uint16_t datalen)
{
	app_csc_info.buff_ptr = databuf;
	app_csc_info.buff_len = datalen;
}

/**
* \CSC profile initialization function
*/
void csc_prf_init(void *param)
{ 
	at_ble_status_t status;
	csc_serv_init(app_csc_info.buff_ptr, app_csc_info.buff_len);
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	csc_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_CLIENT_EVENT_TYPE,
	csc_gatt_client_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	csc_gatt_server_handle);
	
	status = ble_advertisement_data_set();
	if (status != AT_BLE_SUCCESS) {
		DBG_LOG("Advertisement data set failed reason %d",status);
	}
}

/**
* \CSC profile send data function
*/
void csc_prf_send_data(uint8_t *databuf, uint16_t datalen)
{
	if(datalen <= app_csc_info.buff_len){
		csc_serv_send_data(app_csc_info.conn_params.handle, databuf, datalen);
	}else{
		csc_serv_send_data(app_csc_info.conn_params.handle, databuf, app_csc_info.buff_len);
	}
}

/**
* \CSC device advertisement handler function
*/
void csc_prf_dev_adv(void)
{
	/* Start of advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_CSC_FAST_ADV, APP_CSC_ADV_TIMEOUT, 0) == AT_BLE_SUCCESS){
		DBG_LOG("Device Started Advertisement");
	}else{
		DBG_LOG("Device Advertisement Failed");
	}
}

/**
 * @brief Connection handler invoked by ble manager
 */
at_ble_status_t csc_prf_connected_state_handler(void *params)
{
	at_ble_status_t status;
	memcpy((uint8_t *)&app_csc_info.conn_params, params, sizeof(at_ble_connected_t));
	if(!app_csc_info.devicedb){		
		app_csc_info.discover_role = DISCOVER_SERVICE;	
		app_csc_info.csc_serv.service_uuid.type = AT_BLE_UUID_128;
		memcpy(&app_csc_info.csc_serv.service_uuid.uuid[0], CSC_SERVICE_UUID, CSC_UUID_128_LEN);
		/* Discover Remote Service by service UUID */
		status = at_ble_primary_service_discover_by_uuid(app_csc_info.conn_params.handle,START_HANDLE, END_HANDLE, &app_csc_info.csc_serv.service_uuid);
		if(status != AT_BLE_SUCCESS){
			DBG_LOG("Failed to start service discovery. status = %d", status);
		} else {
			DBG_LOG_DEV("Started service discovery");
		}		
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief Discovery Complete handler invoked by ble manager
 */
at_ble_status_t csc_prf_discovery_complete_handler(void *params)
{
		at_ble_discovery_complete_t discover_status;
		memcpy((uint8_t *)&discover_status, params, sizeof(at_ble_discovery_complete_t));
		if(discover_status.status == AT_DISCOVER_SUCCESS){
			if(discover_status.operation == AT_BLE_DISC_BY_UUID_SVC){
				DBG_LOG_DEV("Discover Service Info:\r\n -->ConnHandle 0x%02x\r\n -->start handle 0x%02x\r\n -->End handle : 0x%02x",
				app_csc_info.csc_serv.conn_handle,
				app_csc_info.csc_serv.start_handle,
				app_csc_info.csc_serv.end_handle);				
				if(at_ble_characteristic_discover_all(app_csc_info.conn_params.handle, app_csc_info.csc_serv.start_handle, app_csc_info.csc_serv.end_handle) != AT_BLE_SUCCESS){
					DBG_LOG("Fail to start discover characteristic");
				}
			}
			else if(discover_status.operation == AT_BLE_DISC_ALL_CHAR){
				if(at_ble_descriptor_discover_all(app_csc_info.csc_char.conn_handle,(app_csc_info.csc_char.value_handle+1), (app_csc_info.csc_serv.end_handle)) != AT_BLE_SUCCESS){
					DBG_LOG("Descriptor Discovery Failed");
				}
			}
			else if(discover_status.operation == AT_BLE_DISC_DESC_CHAR){
				app_csc_info.devicedb = true;
			}
		}
		return AT_BLE_SUCCESS;
}

/**
 * @brief Service found handler invoked by ble manager
 */
at_ble_status_t csc_prf_service_found_handler(void * params)
{
	DBG_LOG_DEV("Service Found handler");
	memcpy((uint8_t *)&app_csc_info.csc_serv, params, sizeof(at_ble_primary_service_found_t));
	return AT_BLE_SUCCESS;
}

/**
 * @brief characteristic found handler invoked by ble manager
 */
at_ble_status_t csc_prf_characteristic_found_handler(void *params)
{
	memcpy((uint8_t *)&app_csc_info.csc_char, params, sizeof(at_ble_characteristic_found_t));
	return AT_BLE_SUCCESS;
}

/**
 * @brief client descriptor found handler invoked by ble manager
 */
at_ble_status_t csc_prf_descriptor_found_handler(void *params)
{
		memcpy((uint8_t *)&app_csc_info.csc_desc, params, sizeof(at_ble_descriptor_found_t));
		return AT_BLE_SUCCESS;
}

/**
 * @brief invoked by ble manager on receiving notification
 */
at_ble_status_t csc_prf_notification_handler(void *params)
{
	 at_ble_notification_recieved_t notif;
	 csc_report_ntf_t ntf_info;
	 memcpy((uint8_t *)&notif, params, sizeof(at_ble_notification_recieved_t));
	 ntf_info.conn_handle = notif.conn_handle;
	 ntf_info.recv_buff_len = notif.char_len;
	 ntf_info.recv_buff = &notif.char_value[0];
	 recv_ntf_cb(&ntf_info);
	 return AT_BLE_SUCCESS;
}

/**
 * @brief invoked by ble manager for setting the notification 
 */
at_ble_status_t csc_prf_write_notification_handler(void *params)
{
	uint8_t data[2] = {1, 0};
	DBG_LOG("\r\n");
	if(at_ble_characteristic_write(app_csc_info.csc_desc.conn_handle, app_csc_info.csc_desc.desc_handle, 0, 2, data, false, true) == AT_BLE_FAILURE){
		DBG_LOG("\r\nFailed to send characteristic Write Request");
	}

	return AT_BLE_SUCCESS;
}

/**
* \CSC device disconnected handler function
*/

at_ble_status_t csc_prf_disconnect_event_handler(void *params)
{
	app_csc_info.devicedb = false;

	return AT_BLE_SUCCESS;
}

/**
* \Service characteristic change handler function
*/
at_ble_status_t csc_prf_char_changed_handler(void *params)
{
	at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, params, sizeof(at_ble_characteristic_changed_t));
	return AT_BLE_SUCCESS;
}

/**
* \Notify to user about the new data received
*/
void notify_recv_ntf_handler(recv_ntf_callback_t recv_ntf_fn)
{
	recv_ntf_cb = recv_ntf_fn;
}

/** @brief csc_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
at_ble_status_t csc_notification_confirmation_handler(void *params)
{
	at_ble_cmd_complete_event_t *event_param = (at_ble_cmd_complete_event_t *)params;
	if (event_param->status == AT_BLE_SUCCESS){
		DBG_LOG_DEV("Notification Successfully sent over the air");
		DBG_LOG_DEV("\r\n");
	} else {
		DBG_LOG_DEV("Sending Notification over the air failed");
		DBG_LOG_DEV("\r\n");
	}
	return AT_BLE_SUCCESS;
}


