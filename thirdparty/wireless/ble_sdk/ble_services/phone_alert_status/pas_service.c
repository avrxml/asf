/**
* \file
*
* \brief Phone Alert Status Service
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
* \mainpage
* \section preface Preface
* This is the reference manual for the Current Time Service
*/
/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/

#include <string.h>
#include "at_ble_api.h"
#include "ble_utils.h"
#include "pas_service.h"

/***********************************************************************************
 *									Implementations	                               *
 **********************************************************************************/

/**
 * @brief triggers the read of alert status characteristic
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @return AT_BLE_SUCCESS for success AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_read_alert_status_char(at_ble_handle_t conn_handle,at_ble_handle_t char_handle)
{
	return (at_ble_characteristic_read(conn_handle,char_handle,0,ALERT_STATUS_CHAR_LEN));
}

/**
 * @brief triggers the read of ringer setting characteristic
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @return AT_BLE_SUCCESS for success AT_BLE_FAILURE for failure
 */
at_ble_status_t pas_read_ringer_setting_char(at_ble_handle_t conn_handle,at_ble_handle_t char_handle)
{
	return (at_ble_characteristic_read(conn_handle,char_handle,0,RINGER_SETTING_CHAR_LEN));
}

/**
 * @brief Enable/disable the notification
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @param[in] set/clear the notification
 * @return AT_BLE_SUCCESS for success or refer at_ble_err_status_t
 */
at_ble_status_t pas_char_write_notification(at_ble_handle_t conn_handle,at_ble_handle_t char_handle,bool enable)
{
	uint16_t data = enable;
	return (at_ble_characteristic_write(conn_handle,char_handle,0,CLIENT_CONF_CHAR_DESCRIPTOR_LEN
	,(uint8_t *)&data,false,true));
}

/**
 * @brief sets ringer control point
 * @param[in] at_ble_handle_t connection handle
 * @param[in] at_ble_handle-t characteristic handle
 * @param[in] set/clear the notification
 * @return AT_BLE_SUCCESS for success or refer at_ble_err_status_t
 */

at_ble_status_t pas_char_set_ringer_control_point(at_ble_handle_t conn_handle,at_ble_handle_t char_handle,uint8_t data)
{
	uint8_t ringer_data = data;
	return (at_ble_characteristic_write(conn_handle,char_handle,0,RINGER_SETTING_CHAR_LEN
	,&ringer_data,false,false));
}

