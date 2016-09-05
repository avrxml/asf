/**
 * \file
 *
 * \brief Blood Pressure Sensor Profile
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
#include "blood_pressure.h"
#include "blp_sensor.h"
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

/** @brief blood pressure measurement service handler **/
blp_gatt_service_handler_t blp_service_handler;

/** @brief callback functions pointers contains the address of application
 *functions **/
blp_notification_callback_t notification_cb;

blp_indication_callback_t	indication_cb;


/** @brief contains the connection handle functions **/
at_ble_handle_t connection_handle;


static const ble_event_callback_t blp_sensor_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	blp_sensor_connected_state_handler,
	blp_sensor_disconnect_event_handler,
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

static const ble_event_callback_t blp_sensor_gatt_server_handle[] = {
	blp_notification_confirmation_handler,
	blp_indication_confirmation_handler,
	blp_sensor_char_changed_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
/****************************************************************************************
*							        Implementations										*
****************************************************************************************/


/** @brief register_blp_notification_handler registers the notification handler
 * passed by the application
 *  @param[in] blp_notification_callback_t address of the notification handler
 *function to be called
 */
void register_blp_notification_handler(
		blp_notification_callback_t blp_notificaton_handler)
{
	notification_cb = blp_notificaton_handler;
}


/** @brief register_blp_indication_handler registers the indication handler
 * passed by the application
 *  @param[in] blp_indication_callback_t address of the indication handler
 *function to be called
 */
void register_blp_indication_handler(
		blp_indication_callback_t blp_indication_handler)
{
		indication_cb = blp_indication_handler;
}

/** @brief blp_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 *  @return AT_BLE_SUCCESS on success
 */	
at_ble_status_t blp_notification_confirmation_handler(void *params)
{
	at_ble_cmd_complete_event_t event_params;
	memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
	
	if (event_params.status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("Notification Successfully sent over the air");
	} else {
		DBG_LOG_DEV("Sending Notification over the air failed");
	}
	return AT_BLE_SUCCESS;
}

/** @brief blp_indication_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 *  @return AT_BLE_SUCCESS on success
 */	
at_ble_status_t blp_indication_confirmation_handler(void *params)
{
	at_ble_cmd_complete_event_t event_params;
	memcpy(&event_params,params,sizeof(at_ble_cmd_complete_event_t));
	
	if (event_params.status == AT_BLE_SUCCESS){
		DBG_LOG_DEV("Indication successfully sent over the air");
	} else {
		DBG_LOG_DEV("Sending indication over the air failed %d",event_params.status);
	}
	
	return AT_BLE_SUCCESS;
}


/** @brief blp_sensor_send_notification adds the new characteristic value and
 * sends the notification
 *  @param[in] blp_data the new hr characteristic value needs to be updated
 *  @param[in] length length of new characteristic value
 */
void blp_sensor_send_notification(uint8_t *blp_data, uint8_t length)
{
	at_ble_status_t status;

	/** Updating the new characteristic value */
	if ((status
				= at_ble_characteristic_value_set(
					blp_service_handler.serv_chars
					[1].char_val_handle, blp_data,
					length)) != AT_BLE_SUCCESS) {
		DBG_LOG("at_ble_characteristic_value_set for notification failed,reason %x",
				status);
				return;
	}

	/** Sending the notification for the updated characteristic */
	if ((status	= at_ble_notification_send(connection_handle,
					blp_service_handler.serv_chars[1]
					.char_val_handle))) {
		DBG_LOG("Send notification failed,reason %x", status);
	}
}

/** @brief blp_sensor_send_indication adds the new characteristic value and
 * sends the indication
 *  @param[in] blp the new blp characteristic value needs to be updated
 *  @param[in] length length of new characteristic value
 */
void blp_sensor_send_indication(uint8_t *blp_data, uint8_t length)
{
	at_ble_status_t status;

	/** Updating the new characteristic value */
	if ((status
				= at_ble_characteristic_value_set(
					blp_service_handler.serv_chars
					[0].char_val_handle, blp_data,
					length)) != AT_BLE_SUCCESS) {
		DBG_LOG("at_ble_characteristic_value_set for indication failed,reason %x",
				status);
				return;
	}

	/** Sending the indication for the updated characteristic */
	if ((status	= at_ble_indication_send(connection_handle,
						blp_service_handler.serv_chars[0]
						.char_val_handle))) {
			DBG_LOG("Send indication failed,reason %x", status);
	}
}


/** @brief blp_sensor_char_changed_handler called by the ble manager after a
 *  change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *  characteristic and new value
 *  @return AT_BLE_SUCCESS
 */
at_ble_status_t blp_sensor_char_changed_handler(
					void *char_handle)
{
	uint8_t action_event;
	at_ble_characteristic_changed_t change_params;
	memcpy((uint8_t *)&change_params, char_handle,
			sizeof(at_ble_characteristic_changed_t));

	action_event = blp_char_change_handler(&blp_service_handler,
			&change_params);

	if ((action_event == BLP_NOTIFICATION_ENABLE) ||
			(action_event == BLP_NOTIFICATION_DISABLE)) {
		if (action_event == BLP_NOTIFICATION_ENABLE) {
			notification_cb(true);	
		} else if (action_event == BLP_NOTIFICATION_DISABLE){
			notification_cb(false);
		}
	} else if ((action_event == BLP_INDICATION_ENABLE) ||
			(action_event == BLP_INDICATION_DISABLE)) {
		if (action_event == BLP_INDICATION_ENABLE) {
			indication_cb(true);
		} else if (action_event == BLP_INDICATION_DISABLE) {
			indication_cb(false);
		}
	}

	return AT_BLE_SUCCESS;
}

/** @brief blp_sensor_disconnect_event_handler called by ble manager after
 * disconnection event received
 *	@param[in] at_ble_disconnected_t	which has connection handle and
 *reason for disconnection
 */
at_ble_status_t blp_sensor_disconnect_event_handler(
					void *disconnect)
{
    ALL_UNUSED(disconnect);
	return AT_BLE_SUCCESS;
}

/** @brief blp_sensor_connected_state_handler called by ble manager after a
 * change in characteristic
 *  @param[in] at_ble_connected_t which has connection handle and the peer
 *device address
 */
at_ble_status_t blp_sensor_connected_state_handler(
		void *conn_params)
{
	connection_handle = (at_ble_handle_t)(((at_ble_connected_t *)conn_params)->handle);
		
	return AT_BLE_SUCCESS;
}

/** @brief blp_disconnection called by the application to disconnect
 *
 */
void blp_disconnection(void)
{
	at_ble_status_t status;
	if ((status = at_ble_disconnect(connection_handle,AT_BLE_TERMINATED_BY_USER)) != AT_BLE_SUCCESS) {
		DBG_LOG("Disconnection not successuful, reason %x",status);
	}
}

/** @brief blp_sensor_adv starts advertisement
 *
 */
void blp_sensor_adv(void)
{
	at_ble_status_t status;
	
	/* Start of advertisement */
	if ((status
				= at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED,
					AT_BLE_ADV_GEN_DISCOVERABLE, NULL,
					AT_BLE_ADV_FP_ANY,
					BLP_SENSOR_FAST_ADV,
					BLP_SENSOR_ADV_TIMEOUT,
					0)) == AT_BLE_SUCCESS) {
		DBG_LOG("Bluetooth device is in Advertising Mode");
	} else {
		DBG_LOG(
				"Bluetooth LE Advertisement failed start Failed,reason %d",
				status);
	}
}

/** @brief blp_sensor_service_define defines the services of the profile
 *
 */
void blp_sensor_service_define(void)
{
	at_ble_status_t status;

	if ((status = blp_primary_service_define(&blp_service_handler)) !=
			AT_BLE_SUCCESS) {
		DBG_LOG("Blood Pressure Service definition Failed,reason: %x",
				status);
	}
	
	if ((status = dis_primary_service_define(&dis_service_handler)) !=
			AT_BLE_SUCCESS) {
		DBG_LOG("Dis Service definition failed,reason: %x", status);
	}
	
}

/**
 * \brief Initialization of profile services
 */
void blp_sensor_service_init(void)
{
	blp_init_service(&blp_service_handler);

	dis_init_service(&dis_service_handler);
}

/** @brief blp_sensor_init initializes and defines the services of the hr profile
 *
 *  @param[in] params are unused.
 *
 */
void blp_sensor_init(void *param)
{
	at_ble_status_t status;
	
	blp_sensor_service_init();
	blp_sensor_service_define();
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	blp_sensor_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	blp_sensor_gatt_server_handle);
							
	status = ble_advertisement_data_set();
	
	if (status != AT_BLE_SUCCESS) {
		DBG_LOG("Advertisement set failed reason %d",status);
	}
				
	/* Handles for the Blood pressure service */
	DBG_LOG_PTS("\n\nThe service handle for blp is 0x%04x",
	blp_service_handler.serv_handle);
	DBG_LOG_PTS("The characteristic handle for blp measurement is 0x%04x",
	blp_service_handler.serv_chars[0].char_val_handle - 1);
	DBG_LOG_PTS("The characteristic value handle for blp measurement is 0x%04x",
	blp_service_handler.serv_chars[0].char_val_handle);
	DBG_LOG_PTS("The characteristic handle for intermediate cuff pressure is "
	"0x%04x",blp_service_handler.serv_chars[1].char_val_handle - 1);
	DBG_LOG_PTS("The characteristic value handle for intermediate cuff pressure is "
	"0x%04x",blp_service_handler.serv_chars[1].char_val_handle);
	DBG_LOG_PTS("The characteristic handle for blood pressure feature is 0x%04x",
	blp_service_handler.serv_chars[2].char_val_handle - 1);
	DBG_LOG_PTS("The characteristic value handle for blood pressure feature is 0x%04x",
	blp_service_handler.serv_chars[2].char_val_handle);
	DBG_LOG_PTS("The descriptor handle for blp measurement is 0x%04x",
	blp_service_handler.serv_chars[0].client_config_handle);
	DBG_LOG_PTS("The descriptor handle for intermediate cuff press is 0x%04x",
	blp_service_handler.serv_chars[1].client_config_handle);
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
							
	DBG_LOG_PTS("The default value of blood pressure feature is 0x%02x",
							*(blp_service_handler.serv_chars[2].init_value));					
							
							
    ALL_UNUSED(param);
}
