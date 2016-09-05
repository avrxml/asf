/**
 * \file
 *
 * \brief Heart Rate Sensor Profile
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

/****************************************************************************************
*							        Includes	
*                                       *
****************************************************************************************/
#include <asf.h>
#include <string.h>
#include "timer_hw.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "device_info.h"
#include "heart_rate.h"
#include "hr_sensor.h"
#include "ble_utils.h"

/****************************************************************************************
*							        Globals		
*                                       *
****************************************************************************************/
/** @brief Scan response data*/
uint8_t scan_rsp_data[SCAN_RESP_LEN]
	= {0x09, 0xff, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};

/** @brief device information service handler **/
dis_gatt_service_handler_t dis_service_handler;

/** @brief heart rate measurement service handler **/
hr_gatt_service_handler_t hr_service_handler;

/** @brief callback functions pointers contains the address of application
 *functions **/
hr_notification_callback_t notification_cb;
hr_reset_callback_t reset_cb;

bool notification_confirm = true;
/** @brief contains the connection handle functions **/
at_ble_handle_t connection_handle;

static const ble_event_callback_t hr_sensor_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	hr_sensor_connected_state_handler,
	hr_sensor_disconnect_event_handler,
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

static const ble_event_callback_t hr_sensor_gatt_server_handle[] = {
	hr_sensor_notification_cfm_handler,
	NULL,
	hr_sensor_char_changed_handler,
	NULL,
	NULL,
	hr_sensor_char_write_request,
	NULL,
	NULL,
	NULL,
	NULL
};

/****************************************************************************************
*							        Implementations
*                                       *
****************************************************************************************/
/** @brief register_hr_notification_handler registers the notification handler
 *	passed by the application
 *  param[in] hr_notification_callback_t address of the notification handler
 *	function to be called
 */
void register_hr_notification_handler(
		hr_notification_callback_t hr_notificaton_handler)
{
	notification_cb = hr_notificaton_handler;
}

/** @brief register_hr_reset_handler registers the reset handler passed by the
 *  application
 *  @param[in]	hr_reset_callback_t address of the handler function to be called
 */
void register_hr_reset_handler(hr_reset_callback_t hr_reset_handler)
{
	reset_cb = hr_reset_handler;
}

/** @brief hr_sensor_char_write_request handles the write request for heart rate 
 *  control point characteristic.
 *  @param[in]	at_ble_characteristic_write_request_t parameters containing the value written
 */
at_ble_status_t hr_sensor_char_write_request(void * params)
{
	at_ble_characteristic_write_request_t request;
	at_ble_status_t status;
	
	memcpy(&request,params,sizeof(at_ble_characteristic_write_request_t));
	if (request.char_handle == hr_service_handler.serv_chars[2].char_val_handle) {
			if (request.value[0] == HR_CONTROL_POINT_RESET_VAL) {
				status = at_ble_write_authorize_reply(&request,AT_BLE_SUCCESS);
				if (status != AT_BLE_SUCCESS) {
					DBG_LOG("at_ble_write_authorize_reply failed");
					} else  {
					reset_cb();
				}
				
			} else {
				DBG_LOG_DEV("Sending an error code");
				at_ble_write_authorize_reply(&request,AT_BLE_ATT_APP_ERROR);
			}
	}

	return AT_BLE_SUCCESS;
}

/** @brief hr_notification_confirmation_handler called on notification confirmation
 *  event by the ble manager
 *	@param[in] at_ble_status_t AT_BLE_SUCCESS on success AT_BLE_FAILURE on failure
 *  called
 */
at_ble_status_t hr_sensor_notification_cfm_handler(void * params)
{
	at_ble_cmd_complete_event_t param;
	
	memcpy(&param,params,sizeof(at_ble_cmd_complete_event_t));
	if ( param.status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("Notification confirmation successful");
	} else { 
		DBG_LOG_DEV("Notification confirmation failure");
	}
	return AT_BLE_SUCCESS;
}

/** @brief hr_sensor_send_notification adds the new characteristic value and
 * sends the notification
 *  @param[in] hr_data the new hr characteristic value needs to be updated
 *  @param[in] length length of new characteristic value
 */
bool hr_sensor_send_notification(uint8_t *hr_data, uint8_t length)
{
	at_ble_status_t status;
	
	/** Updating the new characteristic value */
	if ((status = at_ble_characteristic_value_set(
							hr_service_handler.serv_chars[0].char_val_handle,
							hr_data, length)) != AT_BLE_SUCCESS) {
		DBG_LOG("Write value for notification failed,reason %d",
				status);
		return false;
	}

	/** Sending the notification for the updated characteristic */
	if ((status	= at_ble_notification_send(connection_handle,
										hr_service_handler.serv_chars[0]
										.char_val_handle))) {
		DBG_LOG("Send notification failed,reason %d", status);
		return false;	
	}
	
	return true;
}

/** @brief hr_sensor_char_changed_handler called by the ble manager after a
 *  change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *  characteristic and new value
 *  @return AT_BLE_SUCCESS on success and AT_BLE_FAILURE on failure
 */
at_ble_status_t hr_sensor_char_changed_handler(
		void *params)
{
	uint8_t action_event;

	at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, params,
			sizeof(at_ble_characteristic_changed_t));

	action_event = hr_write_value_handler(&hr_service_handler,
			&change_params);

	if ((action_event == HR_NOTIFICATION_DISABLE) ||
			(action_event == HR_NOTIFICATION_ENABLE)) {
		notification_cb(action_event);
	} else if (action_event == HR_CONTROL_POINT_RESET) {
		reset_cb();
	}

	return AT_BLE_SUCCESS;
}

/** @brief hr_sensor_disconnect_event_handler called by ble manager after
 *  disconnection event recieved
 *	@param[in] at_ble_disconnected_t	which has connection handle and
 *  reason for disconnection
 */
at_ble_status_t hr_sensor_disconnect_event_handler(
		void *disconnect)
{
	ALL_UNUSED(disconnect);   
	return AT_BLE_SUCCESS;
}

/** @brief hr_sensor_connected_state_handler called by ble manager after a
 * change in characteristic
 *  @param[in] at_ble_connected_t which has connection handle and the peer
 *device address
 */
at_ble_status_t hr_sensor_connected_state_handler(
							void *params)
{
	at_ble_connected_t conn_params;
	memcpy(&conn_params,params,sizeof(at_ble_connected_t));
	connection_handle = (at_ble_handle_t)conn_params.handle;
	return AT_BLE_SUCCESS;
}

/** @brief hr_sensor_disconnect disconnects with the peer device called by the
 * application
 *
 */
void hr_sensor_disconnect(void)
{
	at_ble_status_t status;

	if ((status
				= at_ble_disconnect(connection_handle,
					AT_BLE_TERMINATED_BY_USER)) !=
			AT_BLE_SUCCESS) {
		DBG_LOG("Failed to disconnect , reason %x", status);
	} else {
		DBG_LOG("Disconnection request successfull");
	}
}

/** @brief hr_sensor_adv adds the advertisement data of the profile and starts
 * advertisement
 *
 */
void hr_sensor_adv(void)
{
	at_ble_status_t status;
	/* Start of advertisement */
	if ((status
				= at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
					AT_BLE_ADV_GEN_DISCOVERABLE, NULL,
					AT_BLE_ADV_FP_ANY,
					HR_SENSOR_FAST_ADV,
					0,
					0)) == AT_BLE_SUCCESS) {
		DBG_LOG("Bluetooth device is in Advertising Mode");
	} else {
		DBG_LOG(
				"Bluetooth LE Advertisement failed start Failed,reason %d",
				status);
	}
}

/** @brief hr_sensor_service_define defines the services of the profile
 *
 */
void hr_sensor_service_define(void)
{
	at_ble_status_t status;

	if ((status = hr_primary_service_define(&hr_service_handler)) !=
			AT_BLE_SUCCESS) {
		DBG_LOG("Heart Rate Service definition Failed,reason: %x",
				status);
	} else {
		DBG_LOG_DEV("hr service defined succesfully");
	} 
	
	if ((status = dis_primary_service_define(&dis_service_handler)) !=
			AT_BLE_SUCCESS) {
		DBG_LOG("Dis Service definition failed,reason: %x", status);
	} else {
		DBG_LOG_DEV("dis service defined succesfully");
	}
}

/**
 * \brief Initialization of profile services
 */
void hr_sensor_service_init(void)
{
	hr_init_service(&hr_service_handler);

	dis_init_service(&dis_service_handler);
}

/** @brief hr_sensor_init initializes and defines the services of the hr profile
 *
 *  @param[in] params are unused.
 *
 */
void hr_sensor_init(void *param)
{
	at_ble_status_t status;

	hr_sensor_service_init();
	hr_sensor_service_define();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	hr_sensor_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	hr_sensor_gatt_server_handle);

	status = ble_advertisement_data_set();
	if (status != AT_BLE_SUCCESS) {
		DBG_LOG("Advertisement data set failed reason %d",status);
	}	
	
	/* Handles received */
	DBG_LOG_PTS("\n\nThe handle for heart rate service 0x%04x",
							hr_service_handler.serv_handle);
	DBG_LOG_PTS("The characteristic handle for heart rate mm is 0x%04x",
							hr_service_handler.serv_chars[0].char_val_handle - 1);
	DBG_LOG_PTS("The characteristic value handle for heart rate mm is 0x%04x",
							hr_service_handler.serv_chars[0].char_val_handle);
	DBG_LOG_PTS("The characteristic handle for body sensor location is 0x%04x",
							hr_service_handler.serv_chars[1].char_val_handle - 1);
	DBG_LOG_PTS("The characteristic value handle body sensor location is 0x%04x",
							hr_service_handler.serv_chars[1].char_val_handle);
	DBG_LOG_PTS("The characteristic handle for heart rate control point is 0x%04x",
							hr_service_handler.serv_chars[2].char_val_handle - 1);
	DBG_LOG_PTS("The characteristic value handle for heart rate control point is 0x%04x",
							hr_service_handler.serv_chars[2].char_val_handle);
	DBG_LOG_PTS("The descriptor handle for heart rate mm is 0x%04x",
							hr_service_handler.serv_chars[0].client_config_handle);
	
	/* The handles received for Device information */
	DBG_LOG_PTS("\r\nThe service handle for Device information service is 0x%04x",
						dis_service_handler.serv_handle);
	DBG_LOG_PTS("The Handles for the characteristics of DIS are given below\n");
	DBG_LOG_PTS("Characteristic 1 - 0x%04x",
						dis_service_handler.serv_chars[0].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 2 - 0x%04x",
						dis_service_handler.serv_chars[1].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 3 - 0x%04x",
						dis_service_handler.serv_chars[2].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 4 - 0x%04x",
						dis_service_handler.serv_chars[3].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 5 - 0x%04x",
						dis_service_handler.serv_chars[4].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 6 - 0x%04x",
						dis_service_handler.serv_chars[5].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 7 - 0x%04x",
						dis_service_handler.serv_chars[6].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 8 - 0x%04x",
						dis_service_handler.serv_chars[7].char_val_handle - 1);
	DBG_LOG_PTS("Characteristic 9 - 0x%04x",
						dis_service_handler.serv_chars[8].char_val_handle - 1);
											
    ALL_UNUSED(param);
}
