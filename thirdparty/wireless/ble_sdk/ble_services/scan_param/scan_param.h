/**
 * \file
 *
 * \brief Scan Parameters Service declarations
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
// <h> Scan Parameter Service Configuration
// =======================

#ifndef __SCAN_PARAM_H__
#define __SCAN_PARAM_H__

#include "at_ble_api.h"
#include "ble_manager.h"

#define SCP_NOTIFICATION_ENABLE 0x01

typedef struct sps_gatt_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars[2];
}sps_gatt_service_handler_t;

/**@brief Function used to update the scan refresh characteristic value during connection
 *
 * @param[in] sps_serv gatt service information
 * @param[in] info_data @ref sps_info_data, holds the new data information
 *
 * @return none
 */
at_ble_status_t sps_scan_refresh_char_update(sps_gatt_service_handler_t *sps_serv, uint8_t scan_refresh_value);

/**@brief Initialize the service with its included service, characteristics, and descriptors
 *
 * @param[in] sps_serv gatt service information
 * @param[in] scan_interval_window used to know the gatt client scan interval window
 * @param[in] scan_refresh application uses to get the updated scan interval window of the gatt client
 *
 * @return none
 */
void sps_init_service(sps_gatt_service_handler_t *sps_serv, uint16_t *scan_interval_window, uint8_t *scan_refresh);

/**@brief defining a initialized service 
 *
 * @param[in] sps_service gatt service information
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t sps_primary_service_define(sps_gatt_service_handler_t *sps_service);

/**@brief function to check the client characteristic configuration value. 
 *
 * @param[in] sps_service_handler scan parameters service instance
 * @param[in] char_handle characteristic changed @ref at_ble_characteristic_changed_t
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t	sps_char_changed_event(sps_gatt_service_handler_t *sps_service_handler, at_ble_characteristic_changed_t *char_handle);

#endif /* __SCAN_PARAM_H__ */
// </h>

// <<< end of configuration section >>>

