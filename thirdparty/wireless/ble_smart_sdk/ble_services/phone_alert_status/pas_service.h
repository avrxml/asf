/**
* \file
*
* \brief Phone Alert Status Service files
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
