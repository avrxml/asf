
/**
* \file
*
* \brief Phone Alert Status Profile declarations
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
* 
* \section preface Preface
* This is the reference manual for the Time Information Profile
*/
// <<< Use Configuration Wizard in Context Menu >>>
// <h> Phone Alert Status Profile Configuration
// =======================
#ifndef __PAS_CLIENT_H__
#define __PAS_CLIENT_H__
/***********************************************************************************
 *									Macros			                               *
 **********************************************************************************/
/**@brief Advertisement Interval*/
//	<o> Fast Advertisement Interval <100-1000:50>
//	<i> Defines inteval of Fast advertisement in ms.
//	<i> Default: 100
//	<id> pas_fast_adv
#define APP_PAS_FAST_ADV						(1600) //1000 ms

/** @brief APP_PAS_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in 
 *seconds, 0x0000 disables time-out.*/
//	<o> Advertisement Timeout <1-655>
//	<i> Defines interval at which advertisement timeout in sec.
//	<i> Default: 655
//	<id> pas_adv_timeout
#define APP_PAS_ADV_TIMEOUT						(655) // 10 min

/**@brief Scan Response length*/
//	<o> Scan Response Buffer <1-20>
//	<i> Defines size of buffer for scan response.
//	<i> Default: 10
//	<id> pas_scan_resp_len
#define SCAN_RESP_LEN							(10)

/**@brief ADV type UUID Type & Length*/
#define AD_TYPE_16BIT_UUID_LEN					(2)
#define AD_TYPE_32BIT_UUID_LEN					(4)
#define AD_TYPE_128BIT_UUID_LEN					(16)

#define ADV_TYPE_LEN							(0x01)

#define PAS_SERVICE_UUID						(0x180E)
#define PAS_ADV_DATA_UUID_TYPE					(0x3)

/**@brief TIP Service Solicitation Info*/
#define TP_ADV_DATA_SERVSOLICITATION_16UUID_TYPE	(0x14)
#define TP_ADV_DATA_SERVSOLICITATION_128UUID_TYPE	(0x15)

/**@brief Advertisement Name Type Length & data */
#define PAS_ADV_DATA_NAME_LEN					(9)
#define PAS_ADV_DATA_NAME_TYPE					(0x09)


//	<s.9>	Advertising String
//	<i>	String Descriptor describing in advertising packet.
//	<id> pas_adv_data_name_data
#define PAS_ADV_DATA_NAME_DATA					("ATMEL-PAS")

/**@brief The length of 16bit uuid */
#define	PAS_ADV_DATA_UUID_LEN					(2)

/**@brief UUID Type & Length*/
#define UUID_16BIT_LEN							(2)
#define UUID_32BIT_LEN							(4)
#define UUID_128BIT_LEN							(16)

/**@brief No of Characteristics and No of descriptors */
#define PAS_MAX_CHARACTERISTIC					(3)
#define ANP_MAX_DESCRIPTOR						(2)

/**@brief True False*/
#define FALSE									(0)
#define TRUE									(1)

/**@brief start and end handle */
#define START_HANDLE							(0x0001)
#define END_HANDLE								(0xffff)

/**@brief characteristic uuids*/
#define ALERT_STATUS_UUID						(0x2a3f)
#define RINGER_SETTING_UUID						(0x2a41)
#define RINGER_CONTROL_POINT_UUID				(0x2a40)

#define CLIENT_CONF_CHAR_DESCRIPTOR_UUID		(0x2902)


/***********************************************************************************
 *									types			                               *
 **********************************************************************************/
typedef struct serv_info
{
	bool discovery;
	at_ble_handle_t start_handle;
	at_ble_handle_t end_handle;	
}serv_info_t;

typedef struct char_info
{
	bool discovery;
	at_ble_handle_t char_handle;
	at_ble_handle_t value_handle;
	uint8_t properties;
}char_info_t;

typedef struct desc_info 
{
	bool discovery;
	at_ble_handle_t desc_handle;	
}desc_info_t;

typedef struct pas_service_handler
{
	at_ble_handle_t conn_handle;
	
	/* Primary Service info */
	serv_info_t pas_service_info;
	
	/*characteristic info for alert status*/
	char_info_t alert_status_char;
	
	/*descriptor info for alert status*/
	desc_info_t alert_status_desc;
	
	/*characteristic info for ringer setting*/
	char_info_t ringer_setting_char;
	
	/*descriptor info for ringer setting*/
	desc_info_t ringer_setting_desc;
	
	/*characteristic info for ringer control point*/
	char_info_t ringer_control_point_char;
		
}pas_service_handler_t;

typedef void (*read_callback_t) (uint8_t *,uint8_t);
typedef void (*notification_callback_t) (uint8_t *,uint8_t);

/***********************************************************************************
 *									Function Prototype                             *
 **********************************************************************************/


/**
 * @brief invoked by ble manager for initializing the profile
 * @param[in] none
 * @return none
 */
void pas_client_init(void *params);

/*
 *@brief initializes the profile data to the default values
 *@param[in] none
 *@return	none
 */
void pas_data_init(void);

/**
 * @brief sets the advertisement data and triggers advertisement
 * @param[in] none
 * @return none
 */
void pas_client_adv(void);

/**
 * @brief to perform the service discovery
 * @param[in] connection parameters 
 * @return AT_BLE_SUCCESS for success and AT_BLE_FAILURE for failure
 * @pre Called after connection by the ble manager
 */
at_ble_status_t pas_client_service_discovery(void *params);

/**
 * @brief Discovery Complete handler invoked by ble manager
 * @param[in] at_ble_discovery_complete_t disconnected handler 
 */
at_ble_status_t pas_client_discovery_complete_handler(void *params);

/**
 * @brief Service found handler invoked by ble manager
 * @param[in] at_ble_primary_service_found_t invoked when a primary service is found in peer device
 */
 at_ble_status_t pas_client_service_found_handler(void * params);

/**
 * @brief characteristic found handler invoked by ble manager
 * @param[in] at_ble_characteristic_found_t when a characteristic is found in peer device
 */
at_ble_status_t pas_client_characteristic_found_handler(void *params);

/**
 * @brief client descriptor found handler invoked by ble manager
 * @param[in] at_ble_descriptor_found_t invoked when a descriptor is found in peer device
 */
at_ble_status_t pas_client_descriptor_found_handler(void *param);

/**
 * @brief disconnected event handler invoked by ble manager
 * @param[in] at_ble_disconnected_t contains the disconnection info
 */
at_ble_status_t pas_client_disconnected_event_handler(void *params);

/**
 * @brief invoked by ble manager on receiving notification
 * @param[in] at_ble_notification_recieved_t notification information containing handle data
 */
at_ble_status_t pas_client_notification_handler(void *params);

/**
 * @brief invoked by ble manager for setting the write response handler 
 * @param[in] at_ble_characteristic_write_response_t response data contains status and handle
 */
at_ble_status_t pas_client_char_write_response_handler(void *event_params);

/**
 * @brief invoked by ble manager to read the alert status characteristic
 * @return AT_BLE_SUCCESS if success or AT_BLE_FAILURE
 */
at_ble_status_t pas_client_read_alert_status_char(void);

/**
 * @brief invoked by ble manager to read the write status characteristic
 * @return AT_BLE_SUCCESS if success or AT_BLE_FAILURE
 */
at_ble_status_t pas_client_read_ringer_setting_char(void);

/**
 * @brief invoked by ble manager to read the write ringer control point characteristic
 * @return AT_BLE_SUCCESS if success or AT_BLE_FAILURE
 */
at_ble_status_t pas_client_write_ringer_control_point(uint8_t ringer);

/**
 * @brief invoked by ble manager for setting the notification of characteristics
 * @param[in] char_id represents the characteristic,0 for alert status,1 for ringer setting
 * @param[in] enable weather notifications has to enabled or disabled.1 for enable 0 for disable
 * @return	 AT_BLE_SUCCESS if success or AT_BLE_FAILURE
 */
at_ble_status_t pas_client_enable_char_notification(bool char_id,bool enable);

/**
 * @brief invoked by ble manager for setting the notification 
 */
at_ble_status_t pas_client_write_notifications(void *params);

/**
 * @brief register the call back for reading alert status
 * @param[in] read_callback_t type application callback
 * @return none
 */
void register_alert_status_read_callback(read_callback_t app_read_cb);

/**
 * @brief register the call back for reading ringer setting 
 * @param[in] read_callback_t type application callback
 * @return none
 */
void register_ringer_setting_read_callback(read_callback_t app_read_cb);

/**
 * @brief register the call back for notification of alert status  
 * @param[in] notification_callback_t type application callback
 * @return none
 */
void register_alert_status_notification_callback(notification_callback_t app_notify_cb);

/**
 * @brief register the call back for notification of ringer setting
 * @param[in] notification_callback_t type application callback
 * @return none
 */
void register_ringer_setting_notification_callback(notification_callback_t app_notify_cb);

/**
 * @brief called by the ble manager after receiving the read response event 
 * @param[in] characteristic read response parameters
 * @return none
 */
at_ble_status_t pas_client_char_read_response_handler(void *event_params);

/**
 * @brief starts the service discovery
 * @return AT_BLE_SUCCESS for success and AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_client_start_service_discovery(void );
#endif /* __PAS_CLIENT_H__*/
// </h>

// <<< end of configuration section >>>
