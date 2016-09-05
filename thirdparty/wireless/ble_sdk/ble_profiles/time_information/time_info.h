/**
 * \file
 *
 * \brief Time Information Profile declarations
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Time Profile Configuration
// =======================
#ifndef __TIME_INFO_H__
#define __TIME_INFO_H__

/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#define TIME_INFO

/***********************************************************************************
 *									Macros			                               *
 **********************************************************************************/
/**@brief Advertisement Interval*/
//  <o> Fast Advertisement Interval <100-1000:50>
//  <i> Defines inteval of Fast advertisement in ms.
//  <i> Default: 100
//  <id> tip_fast_adv
#define APP_TP_FAST_ADV								(1600)		//1000 ms

/** @brief APP_TP_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in 
 *seconds, 0x0000 disables time-out.*/
//	<o> Advertisement Timeout <1-655>
//	<i> Defines interval at which advertisement timeout in sec.
//	<i> Default: 655
//  <id> tip_adv_timeout
#define APP_TP_ADV_TIMEOUT							(655)		// 10 min

/**@brief Scan Response length*/
//  <o> Scan Response Buffer <1-20>
//  <i> Defines size of buffer for scan response.
//  <i> Default: 10
//  <id> tip_scan_resp_len
#define SCAN_RESP_LEN								(10)

/**@brief Time Profile type, UUid & Length */
#define ADV_TYPE_LEN								(0x01)
#define TP_ADV_DATA_UUID_LEN						(2)
#define TP_ADV_DATA_16BIT_UUID_LEN					(2)

#define CT_ADV_DATA_UUID_LEN						(0x02)
#define NEXT_DST_ADV_DATA_UUID_LEN					(0x02)
#define REF_TIM_ADV_DATA_UUID_LEN					(0x02)

/**@brief TIP Service Solicitation Info*/
#define TP_ADV_DATA_SERVSOLICITATION_16UUID_TYPE	(0x14)
#define TP_ADV_DATA_SERVSOLICITATION_128UUID_TYPE	(0x15)

//	<q> TIP Profile for Andriod
//	<i> Defines whether to support for Andriod or iOS
//  <id> tip_andriod
#define TIP_ANDRIOD 1

#if defined TP_ANDROID
/**@brief Current Time Service UUId for Android */
#define TP_ADV_DATA_UUID_TYPE						(0x03)
#else
/**@brief Current Time Service UUId for iOS  */
#define TP_ADV_DATA_UUID_TYPE						(0x14)
#endif

#define TP_ADV_DATA_UUID_CTS_DATA					("\x05\x18")

/**@brief Appearance type & Length */
#define TP_ADV_DATA_APPEARANCE_LEN					(2)
#define TP_ADV_DATA_APPEARANCE_TYPE					(0x19)
#define TP_ADV_DATA_APPEARANCE_DATA					("\x00\x01")

/**@brief Advertisement Name Type Length & data */
#define TP_ADV_DATA_NAME_LEN						(9)
#define TP_ADV_DATA_NAME_TYPE						(0x09)
//  <s.9> Advertising String
//  <i> String Descriptor describing in advertising packet.
//  <id> tip_adv_data_name_data
#define TP_ADV_DATA_NAME_DATA						("ATMEL-TIP")

#define AT_BLE_DISCOVER_SUCCESS						(10)
#ifdef ENABLE_PTS
#define DBG_LOG_PTS					DBG_LOG 
#else
#define DBG_LOG_PTS					ALL_UNUSED
#endif

/***********************************************************************************
 *									Types			                               *
 **********************************************************************************/


/***********************************************************************************
 *									Functions		                               *
 **********************************************************************************/
typedef void (*bonding_complete_t)(bool flag);
typedef void (* read_response_callback_t) (at_ble_characteristic_read_response_t *char_read_resp);

/**
 * @brief Initialize the Time information Profile
 * 
 */
void time_info_init(void);

/**
 * @brief Discovering the services of time server used by applications
 */
at_ble_status_t time_info_service_discovery(void);

/**
 * @brief Discovering the services of time server
 * @param[in] connection parameter which includes the connection handle device address
 */
at_ble_status_t time_info_service_discover(void *param);

/**
 * @brief Handler for scan info data
 * @param[in] scan info data data buffer containing scan data of all the scanned device 
 * @param[in] Device count
 */
at_ble_status_t time_info_scan_data_handler(at_ble_scan_info_t *scan_buffer,
uint8_t scanned_dev_count);

/**
 * @brief Handler for connection event 
 * @param[in] connected event parameter containing details like handle
 * \note Called by the ble_manager after receiving connection event
 */
at_ble_status_t time_info_connected_state_handler(void *param);

/**
 * @brief Handler for discovery complete event
 * @param[in] discovery complete event which contains result of discovery event
 * \note Called by the ble_manager after receiving discovery complete event
 */
at_ble_status_t time_info_discovery_complete_handler(void *param);

/**
 * @brief Handler for characteristic read
 * @param[in] read response param which gives the status of read and value
 * \note Called by the ble_manager after receiving read response event
 */
at_ble_status_t time_info_characteristic_read_response(void *param);

/**
 * @brief Handler for characteristic found event
 * @param[in] characteristic found event parameter containing details like characteristic handle,uuid
 * \note Called by the ble_manager after receiving characteristic found event
 */
at_ble_status_t time_info_characteristic_found_handler(void *param);

/**
 * @brief Handler for service found event
 * @param[in] service found event parameter containing details like service handle,uuid
 * \note Called by the ble_manager after receiving service found event
 */
at_ble_status_t time_info_service_found_handler(void *param);
/**
 * @brief Handler for disconnection event
 * @param[in] disconnected event parameter containing details like handle
 * \note Called by the ble_manager after receiving disconnection event
 */
at_ble_status_t time_info_disconnected_event_handler(void *param);

/**
 * @brief setting time profile advertisement data and triggering of advertisement
 */
void time_info_adv(void);

/**
 * @brief Handler for descriptor found event
 * @param[in] descriptor found event parameter containing details like characteristic handle,uuid
 * \note Called by the ble_manager after receiving characteristic found event
 */
at_ble_status_t time_info_descriptor_found_handler(void *param);

/**
 * @brief Handler for write notification found event
 * @param[in] value to be set 
 */
void time_info_write_notification_handler(uint16_t value);

/**
 * @brief Handler for notification received found event
 * @param[in] descriptor found event parameter containing details like characteristic handle,uuid
 * \note Called by the ble_manager after receiving characteristic found event
 */
at_ble_status_t time_info_notification_handler(void *param);
/**
 * @brief Application registering callback for characteristic read response received 
 * @param[in] read_response_cb callback of application @ref read_response_callback_t
 * @return None
 */
void time_info_register_read_response_callback(read_response_callback_t read_response_cb);

/**
 * @brief Handler for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack 
 * @param[in] param @ref at_ble_encryption_status_changed_t
 * @return None
 */
void time_info_encryption_status_changed_handler(at_ble_encryption_status_changed_t *param);
/**
 * @brief Handler for AT_BLE_PAIR_DONE event from stack 
 * @param[in] pair_done_param @ref at_ble_pair_done_t
 * @return @ref at_ble_status_t
 */
at_ble_status_t time_info_pair_done_handler(void *param);

/**
 * @brief Application registering callback for both paired done event 
 * \and encryption status changed event
 * @param[in] read_response_cb callback of application @ref read_response_callback_t
 * @return None
 */
void time_info_register_bonding_callback(bonding_complete_t bonding_complete_cb);

#endif /* __TIME_INFO_H__ */
// </h>

// <<< end of configuration section >>>
