/**
* \file
*
* \brief Proximity Reporter Profile declarations
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
// <h> Proximity Reporter Profile Configuration
// =======================

#ifndef __PXP_REPORTER_H__
#define __PXP_REPORTER_H__

#include "ble_manager.h"


/****************************************************************************************
*							        Includes	                                     							*
****************************************************************************************/


/****************************************************************************************
*							        Macros	                                     							*
****************************************************************************************/
/** @brief PATHLOSS which defines if the user wants pathloss functionality*/
//   <q> Enable PathLoss
//   <i> Defines whether to enable the pathlosss functionality or not.
#define PATHLOSS_VALUE 1

#if (PATHLOSS_VALUE == 1)
#define PATHLOSS
#endif


/** @brief APP_PXP_FAST_ADV between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
//  <o> Fast Advertisement Interval <100-1000:50>
//  <i> Defines inteval of Fast advertisement in ms.
//  <i> Default: 100
//  <id> pxp_reporter_fast_adv
#define APP_PXP_FAST_ADV				(1600) //1000 ms

/** @brief APP_PXP_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in 
 *seconds, 0x0000 disables time-out.*/
//	<o> Advertisement Timeout <1-655>
//	<i> Defines interval at which advertisement timeout in sec.
//	<i> Default: 655
//  <id> pxp_reporter_adv_timeout
#define APP_PXP_ADV_TIMEOUT				(655) // 10 min

/** @brief scan_resp_len is the length of the scan response data */
//  <o> Scan Response Buffer <1-20>
//  <i> Defines size of buffer for scan response.
//  <i> Default: 10
//  <id> pxp_reporter_scan_resp_len
#define SCAN_RESP_LEN					(10)

/** @brief ADV_DATA_LEN */
#define ADV_DATA_LEN					(18)

/** @brief ADV_TYPE_LEN */
#define ADV_TYPE_LEN					(0x1)

/** @brief LL_ADV_DATA_UUID_LEN the size of LL service uuid */
#define LL_ADV_DATA_UUID_LEN			(2)

/** @brief LL_ADV_DATA_UUID_TYPE the total sizeof LL service uuid*/
#define LL_ADV_DATA_UUID_TYPE			(0x3)

/** @brief TXP_ADV_DATA_UUID_LEN the size of TXP service uuid */
#define TXP_ADV_DATA_UUID_LEN			(2)

/** @brief TXP_ADV_DATA_UUID_TYPE the total sizeof TXP service uuid*/
#define TXP_ADV_DATA_UUID_TYPE			(0x3)

/** @brief IAL_ADV_DATA_UUID_LEN the size of IAL service uuid */
#define IAL_ADV_DATA_UUID_LEN			(2)

/** @brief IAL_ADV_DATA_UUID_TYPE the total sizeof IAL service uuid*/
#define IAL_ADV_DATA_UUID_TYPE			(0x3)

/** @brief PXP_ADV_DATA_NAME_LEN the  length of the device name */
#define PXP_ADV_DATA_NAME_LEN			(9)

/** @brief PXP_ADV_DATA_NAME_TYPE the gap ad data type */
#define PXP_ADV_DATA_NAME_TYPE			(0x9)

/* @brief PXP_ADV_DATA_NAME_DATA the actual name of device */

//  <s.9> Advertising String
//  <i> String Descriptor describing in advertising packet.
//  <id> pxp_reporter_adv_data_name_data
#define PXP_ADV_DATA_NAME_DATA			("ATMEL-PXP")

/* @brief call back handler type  */
typedef void (*reporter_callback_t)(uint8_t);

typedef void (*reporter_state_callback_t)(bool);
/****************************************************************************************
*							        Function Prototypes	                                     							*
****************************************************************************************/


/** @brief Initialize the profile, includes all initializations and start up routines of the profiles
  * 
  *
  * @param[in] void
  *
  * @pre Must be called after @ref ble_init
  *
  * @return void
  */
void pxp_reporter_init(void *param);

/** @brief Initialize the services of the profile
  * 
  * @param[in] void
  *
  * @pre Must be called before @ref pxp_service_define
  *
  * @return void
  */
void pxp_service_init(void);


/** @brief profile services definition to the attribute data base
  * 
  * @param[in] void
  *
  * @pre Must be called after ref pxp_service_init
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
at_ble_status_t pxp_service_define (void);


/** @brief sets the advertisement data and starts advertisement
  * 
  * @param[in] void
  *
  * @pre Must be called after ref pxp_service_init
  *
  * @return void
  */
void pxp_reporter_adv(void);

/** @brief connection state handler
  * 
  * @param[in] at_ble_connected_t which includes the connection parameters like handle
  *
  * @pre Must be called when the device is connected
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
at_ble_status_t pxp_reporter_connected_state_handler (void *params);


/** @brief disconnection event handler
  * 
  * @param[in] at_ble_disconencted_t which includes the connection parameters like handle,reason for disconnection
  *
  * @pre Must be called when the device is disconnected
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
at_ble_status_t pxp_reporter_disconnect_event_handler(void *params);


/** @brief character changed handler
  * 
  * @param[in] at_ble_characteristic_changed_t which includes handle,new value
  *
  * @pre Must be called when character change event occurred
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
at_ble_status_t pxp_reporter_char_changed_handler(void *params);

/**
* @brief function which register the application alerting function for pathloss to the profile
*
* @param[in] call back function of the application for pathloss
*
*/
void register_pathloss_handler(reporter_callback_t pathloss_fn);

/**
* @brief function which register the application alerting function for linkloss to the profile
*
* @param[in] call back function of the application for linkloss
*
*/
void register_linkloss_handler(reporter_callback_t linkloss_fn);

/**
* @brief function which register the application state call back handler
*
* @param[in] call back function of the application for connection state
*
*/
void register_state_handler(reporter_state_callback_t state_fn);

#endif
// </h>

// <<< end of configuration section >>>

