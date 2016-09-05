
/**
* \file
*
* \brief Alert Notification Profile declarations
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
// <h> Alert Notification Center service Configuration
// =======================
#ifndef __ANCS_CLIENT_H__
#define __ANCS_CLIENT_H__
/***********************************************************************************
 *									Macros			                               *
 **********************************************************************************/
/**@brief Advertisement Interval*/
//	<o> Fast Advertisement Interval <100-1000:50>
//	<i> Defines inteval of Fast advertisement in ms.
//	<i> Default: 100
//	<id> ancs_app_anp_fast_adv
#define APP_ANP_FAST_ADV						(1600) //1000 ms

/** @brief APP_ANP_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in 
 *seconds, 0x0000 disables time-out.*/
//	<o> Advertisement Timeout <1-655>
//	<i> Defines interval at which advertisement timeout in sec.
//	<i> Default: 655
//	<id> ancs_app_anp_adv_timeout
#define APP_ANP_ADV_TIMEOUT						(655) // 10 min

/**@brief Scan Response length*/
//	<o> Scan Response Buffer <1-20>
//	<i> Defines size of buffer for scan response.
//	<i> Default: 10
//	<id> ancs_scan_resp_len
#define SCAN_RESP_LEN							(10)

/**@brief ADV type UUID Type & Length*/
#define AD_TYPE_16BIT_UUID_LEN					(2)
#define AD_TYPE_32BIT_UUID_LEN					(4)
#define AD_TYPE_128BIT_UUID_LEN					(16)
#define ADV_TYPE_LEN							(0x1)

/**@brief Appearance type & Length */
#define ANP_ADV_DATA_APPEARANCE_LEN				(2)
#define ANP_ADV_DATA_APPEARANCE_TYPE			(0x19)
#define ANP_ADV_DATA_APPEARANCE_DATA			("\x00\x40")

/**@brief Advertisement Name Type Length & data */
#define ANP_ADV_DATA_NAME_LEN					(6)
#define ANP_ADV_DATA_NAME_TYPE					(0x9)
//	<s.9>	Advertising String
//	<i>	String Descriptor describing in advertising packet.
//	<id> ancs_anp_adv_data_name_data
#define ANP_ADV_DATA_NAME_DATA					("AT-ANS")

/**@brief ANCS Service Solicitation Info*/
#define ANP_SOLICITATION_LEN					   (16)
#define ANP_ADV_DATA_SERVSOLICITATION_16UUID_TYPE  (0x14)
#define ANP_ADV_DATA_SERVSOLICITATION_128UUID_TYPE (0x15)

#define ANCS_CHAR_NOTIFICATION_SOURCE_UUID "\xBD\x1D\xA2\x99\xE6\x25\x58\x8C\xD9\x42\x01\x63\x0D\x12\xBF\x9F"
#define ANCS_CHAR_CONTROL_POINT "\xD9\xD9\xAA\xFD\xBD\x9B\x21\x98\xA8\x49\xE1\x45\xF3\xD8\xD1\x69"
#define ANCS_CHAR_DATA_SOURCE "\xFB\x7B\x7C\xCE\x6A\xB3\x44\xBE\xB5\x4B\xD6\x24\xE9\xC6\xEA\x22"

/**@brief No of Characteristics and No of decriptors */
#define ANP_MAX_CHARACTERISTIC					(3)
#define ANP_MAX_DESCRIPTOR						(3)

/**@brief True False*/
#define FALSE									(0)
#define TRUE									(1)

/**@brief Notification status*/
#define NOTIFICATION_ADDED						(0)
#define NOTIFICATION_REMOVED					(2)

/**@brief Alert Types*/
#define CATEGORY_ID_INCOMINGCALL				(1)
#define CATEGORY_ID_MISSEDCALL					(2)

/**@brief start and end handle */
#define START_HANDLE							(0x1)
#define END_HANDLE								(0xFFFF)

#define AT_DISCOVER_SUCCESS						(10)

/***********************************************************************************
 *									types			                               *
 **********************************************************************************/
/* Typedef for alert notification profile */

/* Typedef for alert notification profile */
typedef struct app_anp_data{
	/** To check the service discovery status */
	uint8_t devicedb;
	///** To check the type of discovery */
	uint8_t discover_role;
	/** Connection related information*/
	at_ble_connected_t conn_params;
	/** information of characteristics found */
	at_ble_characteristic_found_t char_info;
	/** information of descriptor found */
	at_ble_descriptor_found_t ancs_desc;
}app_anp_data_t;

/**@brief type of attribute discovery */
typedef enum
{
	/// Discovery Disable
	DISCOVER_IDLE = 0,
	
	/// Discover Service
	DISCOVER_SERVICE,
	
	/// Discover Include Service
	DISCOVER_INCLUDE_SERVICE,
	
	/// Discover Characteristic
	DISCOVER_CHARACTERISTIC,
	
	/// Discover Descriptor
	DISCOVER_DESCRIPTOR,
	
}ble_discovery_role;

/**@brief time data structure */
typedef struct time_val
{
	uint8_t timeval[8];
	uint8_t dateval[10];
	uint8_t daysval[2];
	uint16_t year;
}time_val_t;

/***********************************************************************************
 *									Funtions		                               *
 **********************************************************************************/
/**
 * @brief Initialize the profile specific information
 * 
 */
void anp_info_init(void);

/**
 * @brief Function used for profile initialization and also enable the advertisement
 * \note Called by the ble_manager
 * 
 */
void anp_client_init( void *params);

/**
 * @brief does all the profile initialization and starts the advertisement
 * \note Called by the ble_manager
 */
void anp_client_adv(void);

/**
 * @brief Handler for connection event 
 * @param[in] connected event parameter containing details like handle
 * \note Called by the ble_manager after receiving connection event
 */
at_ble_status_t anp_client_connected_state_handler(void *params);

/**
 * @brief Handler for disconnection event
 * @param[in] disconnected event parameter containing details like handle
 * \note Called by the ble_manager after receiving disconnection event
 */
at_ble_status_t anp_client_disconnected_event_handler(void *params);

/**
 * @brief Handler for service found event
 * @param[in] service found event parameter containing details like service handle,uuid
 * \note Called by the ble_manager after receiving service found event
 */
at_ble_status_t anp_client_service_found_handler(void * params);

/**
 * @brief Handler for discovery complete event
 * @param[in] discovery complete event which contains result of discovery event
 * \note Called by the ble_manager after receiving discovery complete event
 */
at_ble_status_t anp_client_discovery_complete_handler(void *params);

/**
 * @brief Handler for characteristic found event
 * @param[in] characteristic found event parameter containing details like characteristic handle,uuid
 * \note Called by the ble_manager after receiving characteristic found event
 */
at_ble_status_t anp_client_characteristic_found_handler(void *params);

/**
 * @brief Handler for descriptor found event
 * @param[in] descriptor found event parameter containing details like descriptor handle,uuid
 * \note Called by the ble_manager after receiving descriptor found event
 */
at_ble_status_t anp_client_descriptor_found_handler(void *params);

/**
 * @brief Handler for char changed handler 
 * @param[in] characteristic changed event parameter containing details like characteristic handle,value
 * \note Called by the ble_manager after receiving characteristic change event
 */
void anp_client_char_changed_handler(at_ble_characteristic_changed_t *params);

/**
 * @brief Handler for write response 
 * @param[in] write response parameter contating the result of write request
 * \note Called by the ble_manager after receiving write response event
 */
at_ble_status_t anp_client_write_response_handler(void *params);

/**
 * @brief Handler for notification event 
 * @param[in] notification received parameter containing the notification value
 * \note Called by the ble_manager after receiving the notification
 */
at_ble_status_t anp_client_notification_handler(void *params);

/**
 * @brief Handler for enabling the notification 
 * \note Called by the ble_manager for enabling the notification in the gatt server
 */
at_ble_status_t anp_client_write_notification_handler(void *param);
#endif /* __ANCS_CLIENT_H__*/
// </h>

// <<< end of configuration section >>>
