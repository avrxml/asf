/**
 * \file
 *
 * \brief Battery Service declarations
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
// <h> Battery Service Configuration
// =======================

#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "at_ble_api.h"
#include "ble_manager.h"

/** characteristic presentation format value */
#define BAT_CHAR_PRESENTATION_FORMAT_VALUE 0x04

/** @brief Characteristic presentation format exponent */
#define BAT_CHAR_PRESENTATION_FORMAT_EXPONENT 0x00

/** @brief Characteristic presentation format unit */
#define BAT_CHAR_PRESENTATION_FORMAT_UNIT BAT_SERVICE_UUID

/** @brief Characteristic presentation format namespace */
#define BAT_CHAR_PRESENTATION_FORMAT_NAMESPACE 0x01

/**  @brief Characteristic presentation format descriptor */
#define BAT_CHAR_PRESENTATION_FORMAT_DESCRIPTOR 0x1000

typedef struct bat_gatt_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars;
}bat_gatt_service_handler_t;

/**@brief Update the battery characteristic value after defining the services using bat_primary_service_define
 *
 * @param[in] conn_handle connection handle
 * @param[in] battery_serv battery service instance
 * @param[in] char_data New battery level
 * @param[in] notify flag to send the notification 
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t bat_update_char_value (at_ble_handle_t conn_handle, bat_gatt_service_handler_t *battery_serv , uint8_t char_data, bool notify);

/**@brief Battery service and characteristic initialization(Called only once by user).
 *
 * @param[in] battery_serv battery service instance
 *
 * @return none
 */
void bat_init_service(bat_gatt_service_handler_t *battery_serv, uint8_t *battery_value);

/**@brief Register a battery service instance inside stack. 
 *
 * @param[in] battery_service battery service instance
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t bat_primary_service_define(bat_gatt_service_handler_t *battery_service);

/**@brief function to check the client characteristic configuration value. 
 *
 * @param[in] battery_service battery service instance
 * @param[in] characteristic_changed_param characteristic changed @ref at_ble_characteristic_changed_t
 *
 * @return @ref AT_BLE_SUCCESS when matches with BAS CCCD 
 * @return @ref AT_BLE_FAILURE when doesn't matches with BAS CCCD 
 */
at_ble_status_t bat_char_changed_event(bat_gatt_service_handler_t *battery_service, at_ble_characteristic_changed_t *characteristic_changed_param);

#endif /* __BATTERY_H__ */
// </h>

// <<< end of configuration section >>>
