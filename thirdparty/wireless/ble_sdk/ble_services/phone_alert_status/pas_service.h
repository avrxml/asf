/**
* \file
*
* \brief Phone Alert Status Service files
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
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Phone Alert Service Configuration
// =======================
#ifndef __PAS_SERVICE_H__
#define __PAS_SERVICE_H__

/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#include "at_ble_api.h"
#include "ble_utils.h"

/***********************************************************************************
 *									Types			                               *
 **********************************************************************************/
#define ALERT_STATUS_CHAR_LEN			(1)
#define RINGER_SETTING_CHAR_LEN			(1)
#define CLIENT_CONF_CHAR_DESCRIPTOR_LEN	(2)

/***********************************************************************************
 *									Function Prototypes                            *
 **********************************************************************************/

/**
 * @brief triggers the read of alert status characteritic
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @return AT_BLE_SUCCESS for success AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_read_alert_status_char(at_ble_handle_t conn_handle,at_ble_handle_t char_handle);

/**
 * @brief triggers the read of ringer setting characteristic
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @return AT_BLE_SUCCESS for success AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_read_ringer_setting_char(at_ble_handle_t conn_handle,at_ble_handle_t char_handle);

/**
 * @brief Enable/disable the notification
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @param[in] set/clear the notification
 * @return AT_BLE_SUCCESS for success AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_char_write_notification(at_ble_handle_t conn_handle,at_ble_handle_t char_handle,bool enable);


/**
 * @brief sets ringer control point
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @param[in] set/clear the notification
 * @return AT_BLE_SUCCESS for success AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_char_set_ringer_control_point(at_ble_handle_t conn_handle,at_ble_handle_t char_handle,uint8_t data);


#endif /*__CURRENT_TIME_H__*/
// </h>

// <<< end of configuration section >>>
