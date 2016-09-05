/**
 * \file
 *
 * \brief Blood Pressure Sensor Profile declarations
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

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Blood Pressure Profile Configuration
// =======================

#ifndef __BLP_SENSOR_H__
#define __BLP_SENSOR_H__

/****************************************************************************************
*							        Macro                                               *
****************************************************************************************/


/** @brief APP_BLP_SENSOR_FAST_ADV	 between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
//	<o> Fast Advertisement Interval <100-1000:50>
//	<i> Defines inteval of Fast advertisement in ms.
//	<i> Default: 100
//	<id> blp_sensor_fast_adv
#define BLP_SENSOR_FAST_ADV								(100) //100 ms

/** @brief BLP_SENSOR_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in 
 *seconds, 0x0000 disables time-out.*/
//	<o> Advertisement Timeout <1-655>
//	<i> Defines interval at which advertisement timeout in sec.
//	<i> Default: 655
//	<id> blp_sensor_adv_timeout
#define BLP_SENSOR_ADV_TIMEOUT							(655) // 10 min

/** @brief scan_resp_len is the length of the scan response data */
//	<o> Scan Response Buffer <1-20>
//	<i> Defines size of buffer for scan response.
//	<i> Default: 10
//	<id> blp_scan_resp_len
#define SCAN_RESP_LEN									(10)
	
/** @brief ADV_DATA_LEN */
#define ADV_DATA_LEN									(18)

/** @brief ADV_TYPE_LEN */
#define ADV_TYPE_LEN									(0x01)

#define UUID_16_BIT_LEN									(0x02)


/** @brief HR_SENSOR_ADV_DATA_UUID_TYPE is complete 16 bit uuid type*/
#define BLP_SENSOR_ADV_DATA_COMP_16_UUID_TYPE			(0x03)

/** @brief HR_SENSOR_ADV_DATA_UUID_LEN the total length for hr uuid and dis uuid */
#define BLP_SENSOR_ADV_DATA_UUID_LEN					(4)

/** @brief DEVICE_INFORMATION_SERVICE_UUID **/
#define DEVICE_INFORMATION_SERVICE_UUID					(0x180A)

/** @brief BLP_SENSOR_ADV_DATA_NAME_TYPE the gap ad data type */
#define BLP_SENSOR_ADV_DATA_NAME_TYPE					(0x09)

/** @brief BLP_SENSOR_ADV_DATA_NAME_LEN the  length of the device name */
#define BLP_SENSOR_ADV_DATA_NAME_LEN					(9)

/* @brief BLP_ADV_DATA_NAME_DATA the actual name of device */
//	<s.9>	Advertising String
//	<i>	String Descriptor describing in advertising packet.
//	<id> blp_sensor_adv_data_name_data
#define BLP_SENSOR_ADV_DATA_NAME_DATA					("ATMEL-BLP")

/* @brief call back handler type  */
typedef void (*blp_notification_callback_t)(bool);

/* @brief call back handler type  */
typedef void (*blp_indication_callback_t)(bool);


/****************************************************************************************
*							        Function Prototypes	                                *                                                        *
****************************************************************************************/

/** @brief blp_sensor_init initializes and defines the services of the blp profile
 *
 *  @param[in] params are unused.
 *
 */
void blp_sensor_init(void *param);

/** @brief blp_sensor_service_init initializes the services of the profile
 *
 */
void blp_sensor_service_init(void);

/** @brief blp_sensor_service_define defines the services of the profile
 *
 */
void blp_sensor_service_define(void);

/** @brief blp_sensor_adv adds the advertisement data of the profile and starts
 * advertisement
 *
 */
void blp_sensor_adv(void);

/** @brief blp_sensor_send_notification disconnects with the peer device called by the
 * application
 *
 */

void blp_sensor_send_notification(uint8_t *hr_data, uint8_t length);

/** @brief blp_sensor_send_indication adds the new characteristic value and
 * sends the indication
 *  @param[in] blp the new blp characteristic value needs to be updated
 *  @param[in] length length of new characteristic value
 */
void blp_sensor_send_indication(uint8_t *blp_data, uint8_t length);

/** @brief register_blp_indication_handler registers the indication handler
 * passed by the application
 *  @param[in] blp_indication_callback_t address of the indication handler
 *function to be called
 */
void register_blp_indication_handler(
		blp_indication_callback_t blp_indication_handler);
		
/** @brief register_blp_notification_handler registers the notification handler
 * passed by the application
 *  @param[in] blp_notification_callback_t address of the notification handler
 *function to be called
 */
void register_blp_notification_handler(
		blp_notification_callback_t blp_notificaton_handler);

/** @brief blp_sensor_char_changed_handler called by the ble manager after a
 *  change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *  characteristic and new value
 *  @return AT_BLE_SUCCESS
 */
at_ble_status_t blp_sensor_char_changed_handler(
					void *char_handle);

/** @brief blp_sensor_disconnect_event_handler called by ble manager after
 * disconnection event received
 *	@param[in] at_ble_disconnected_t	which has connection handle and
 *reason for disconnection
 */
at_ble_status_t blp_sensor_disconnect_event_handler(
					void *disconnect);

/** @brief blp_sensor_connected_state_handler called by ble manager after a
 * change in characteristic
 *  @param[in] at_ble_connected_t which has connection handle and the peer
 *device address
 */
at_ble_status_t blp_sensor_connected_state_handler(
		void *conn_params);

/** @brief blp_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 *  @return AT_BLE_SUCCESS on success
 */	
at_ble_status_t blp_notification_confirmation_handler(void *params);

/** @brief blp_indication_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 *  @return AT_BLE_SUCCESS on success
 */	
at_ble_status_t blp_indication_confirmation_handler(void *params);

/** @brief blp_disconnection called by the application to disconnect
 *
 */
void blp_disconnection(void);

#endif /*__BLP_SENSOR_H__ */
// </h>

// <<< end of configuration section >>>
