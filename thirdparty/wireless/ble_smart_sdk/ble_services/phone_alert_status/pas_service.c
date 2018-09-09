/**
* \file
*
* \brief Phone Alert Status Service
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
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

