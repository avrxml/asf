/**
 * \file
 *
 * \brief Proximity Monitor Profile declarations
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Proximity Monitor Profile Configuration
// =======================
#ifndef __PXP_MONITOR_H__
#define __PXP_MONITOR_H__

#include "ble_manager.h"

typedef enum {
	AD_TYPE_FLAGS = 01,
	AD_TYPE_COMPLETE_LIST_UUID = 0x03,
	AD_TYPE_COMPLETE_LOCAL_NAME = 0x09
} AD_TYPE;

typedef enum {
	PXP_DEV_UNCONNECTED,
	PXP_DEV_CONNECTING,
	PXP_DEV_CONNECTED,
	PXP_DEV_PAIRED,
	PXP_DEV_SERVICE_FOUND
} PXP_DEV;

//   <o> Rssi Prameter Update Interval <1-10>
//   <i> Defines inteval at which rssi value get updated.
//   <i> Default: 1
#define PXP_RSSI_UPDATE_INTERVAL        (0x01)


//   <o> Low Alert Range Value 
//   <i> Defines inteval at which Low Alert get triggers.
//   <i> Default: -70
#define PXP_LOW_ALERT_RANGE             (-70)

//   <o> High Alert Range Value 
//   <i> Defines inteval at which High Alert get triggers.
//   <i> Default: -90
#define PXP_HIGH_ALERT_RANGE            (-90)

#define PXP_ASCII_TO_DECIMAL_VALUE      ('0')

#define PXP_CONNECT_REQ_INTERVAL        (20)

#define DISCOVER_SUCCESS				(10)
#ifdef ENABLE_PTS
#define DBG_LOG_PTS 					DBG_LOG
#else
#define DBG_LOG_PTS						ALL_UNUSED
#endif

typedef void (*hw_timer_start_func_cb_t)(uint32_t);
typedef void (*hw_timer_stop_func_cb_t)(void);
typedef ble_peripheral_state_t (*peripheral_state_cb_t)(void);

/* *@brief Initializes Proximity profile
 * handler Pointer reference to respective variables
 *
 */
void pxp_monitor_init(void *param);

/**@brief Search for a given AD type in a buffer, received from advertising
 * packets
 *
 * starts search form the buffer, need to provide required search params
 *
 * @param[in] scan_buffer where all received advertising packet are stored
 * @param[in] scanned_dev_count elements in scan_buffer
 *
 * @return @ref AT_BLE_SUCCESS operation programmed successfully
 * @return @ref AT_BLE_INVALID_PARAM incorrect parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t pxp_monitor_scan_data_handler(void *params);

/**@brief peer device connection terminated
 *
 * handler for disconnect notification
 * try to send connect request for previously connect device.
 *
 * @param[in] available disconnect handler of peer and
 * reason for disconnection
 *
 * @return @ref AT_BLE_SUCCESS Reconnect request sent to previously connected
 *device
 * @return @ref AT_BLE_FAILURE Reconnection fails.
 */
at_ble_status_t pxp_disconnect_event_handler(void *params);

/**@brief Connected event state handle after connection request to peer device
 *
 * After connecting to the peer device start the GATT primary discovery
 *
 * @param[in] at_ble_connected_t parameters of the established connection
 *
 * @return @ref AT_BLE_SUCCESS operation programmed successfully.
 * @return @ref AT_BLE_INVALID_PARAM if GATT discovery parameter are incorrect
 *parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t pxp_monitor_connected_state_handler(void *params);

at_ble_status_t pxp_monitor_pair_done_handler(void *params);

at_ble_status_t pxp_monitor_encryption_change_handler(void *params);

/**@brief Discover all Characteristics supported for Proximity Service of a
 * connected device
 *  and handles discovery complete
 * Search will go from start_handle to end_handle, whenever a characteristic is
 *found
 * After search and discovery completes will initialize the alert level and read
 *the tx power value as defined
 * @ref AT_BLE_CHARACTERISTIC_FOUND event is sent and @ref
 *AT_BLE_DISCOVERY_COMPLETE is sent at end of discover operation.
 *
 * @param[in] discover_status discovery status on each
 * @param[in] start_handle start of the searched range
 * @param[in] end_handle   end of the searched range
 *
 */
at_ble_status_t pxp_monitor_discovery_complete_handler(void *params);

/**@brief Handles the read response from the peer/connected device
 *
 * if any read request send, response back event is handle.
 * compare the read response characteristics with available service.
 * and data is handle to the respective service.
 */
at_ble_status_t pxp_monitor_characteristic_read_response(void *params);

/**@brief Handles all Discovered characteristics of a given handler in a
 * connected device
 *
 * Compare the characteristics UUID with proximity services whenever a
 *characteristics is found
 * if compare stores the characteristics handler of respective service
 *
 * @param[in] characteristic_found Discovered characteristics params of a
 *connected device
 *
 */
at_ble_status_t pxp_monitor_characteristic_found_handler(void *params);

/**@brief Discover the Proximity services
 *
 * Search will go from start_handle to end_handle, whenever a service is found
 *and
 * compare with proximity services and stores the respective handlers
 * @ref PXP_MONITOR_CONNECTED_STATE_HANDLER event i.
 *
 * @param[in] at_ble_primary_service_found_t  Primary service parameter
 *
 * @return @ref AT_BLE_SUCCESS operation programmed successfully
 * @return @ref AT_BLE_INVALID_PARAM incorrect parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t pxp_monitor_service_found_handler(
		void *params);

/**@brief Connect to a peer device
 *
 * Connecting to a peer device, implicitly starting the necessary scan operation
 *then
 * connecting if a device in the peers list is found.
 *
 * @param[in] scan_buffer a list of peers that the device will connect to one of
 *them
 * @param[in] index index of elements in peers, to initiate the connection
 *
 * @return @ref AT_BLE_SUCCESS operation programmed successfully
 * @return @ref AT_BLE_INVALID_PARAM incorrect parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t pxp_monitor_connect_request(at_ble_scan_info_t *scan_buffer,
		uint8_t index);

/**@brief Discover all services
 *
 * @param[in] connection handle.
 * @return @ref AT_BLE_SUCCESS operation programmed successfully
 * @return @ref AT_BLE_INVALID_PARAM incorrect parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t pxp_monitor_service_discover(at_ble_handle_t);

at_ble_status_t pxp_monitor_start_scan(void);
void register_hw_timer_start_func_cb(hw_timer_start_func_cb_t timer_start_fn);
void register_hw_timer_stop_func_cb(hw_timer_stop_func_cb_t timer_stop_fn);
void register_peripheral_state_cb(peripheral_state_cb_t peripheral_state_cb);
#endif /*__PXP_MONITOR_H__*/
// </h>

// <<< end of configuration section >>>
