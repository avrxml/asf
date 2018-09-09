/**
  * \file
  *
  * \brief Find Me Locator Profile declarations
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
  * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
  * Support</a>
  */
 
 // <<< Use Configuration Wizard in Context Menu >>>
 // <h> Find Me Target Profile Configuration
 // =======================
 
 #ifndef __FIND_ME_TARGET_H__
 #define __FIND_ME_TARGET_H__
 
 
 /****************************************************************************************
 *							        Includes	
 *                                                                                       *
 ****************************************************************************************/
 
 
 /****************************************************************************************
 *							        Macros	
 *                                                                                       *
 ****************************************************************************************/
#define DISCOVER_SUCCESS				(10)

//   <o> Low Alert Range Value
//   <i> Defines inteval at which Low Alert get triggers.
//   <i> Default: -70
#define FMP_LOW_ALERT_RANGE             (-70)

//   <o> High Alert Range Value
//   <i> Defines inteval at which High Alert get triggers.
//   <i> Default: -90
#define FMP_HIGH_ALERT_RANGE            (-90)

typedef enum fmp_locator_alert_level {
	FMP_NO_ALERT = 0,
	FMP_MID_ALERT,
	FMP_HIGH_ALERT
} fmp_locator_alert_level_t;
 
 /****************************************************************************************
 *							        Function
 *									Prototypes	                                        *
 ****************************************************************************************/
 
 /** @brief Initialize the profile, includes all initializations and start up
  * routines of the profiles
  *
  *
  * @param[in] void
  *
  * @pre Must be called after @ref ble_init
  *
  * @return void
  */
void fmp_locator_init(void);
 
 /** @brief connection state handler
  * called when device goes on connected state
  * @param[in] at_ble_connected_t which includes the connection parameters
  *
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
 /**
  * \Find me profile connected state handler function
  */
 at_ble_status_t fmp_locator_connected_state_handler(void * params);

 
 /** @brief disconnection event handler
  *
  * @param[in] at_ble_disconencted_t which includes the connection parameters
  *like handle,reason for disconnection
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  */
 at_ble_status_t fmp_locator_disconnect_event_handler(void * params);
 
 at_ble_status_t fmp_locator_pair_done_handler(void *params);
 
 /**
 * @brief Service found handler invoked by ble manager
 */
at_ble_status_t fmp_locator_service_found_handler(void *param);

/**@brief Handles all Discovered characteristics of a given handler in a
* connected device
*
* @param[in] characteristic_found Discovered characteristics params of a
*connected device
*
*/
at_ble_status_t fmp_locator_characteristic_found_handler(void *params);

/**@brief Discover all Characteristics supported for Immediate alert Service of a
* connected device
*
* @param[in] discover_status discovery status of each handle
*
*/
at_ble_status_t fmp_locator_discovery_complete_handler(void *params);

 /**@brief Discover all services
 *
 * @param[in] connection handle.
 * @return @ref AT_BLE_SUCCESS operation programmed successfully.
 * @return @ref AT_BLE_INVALID_PARAM incorrect parameter.
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t fmp_locator_service_discover(at_ble_handle_t handle);
 
 #endif /* __FIND_ME_TARGET_H__ */
 // </h>
 
 // <<< end of configuration section >>>
