/**
 * \file
 *
 * \brief Blood Pressure Sensor Profile declarations
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
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
