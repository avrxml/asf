/**
 * \file
 *
 * \brief Heart Rate Service declarations
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
 * Support</a>
 */

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Heart Rate Sensor Service Configuration
// =======================
#ifndef __HEART_RATE_H__
#define __HEART_RATE_H__

/************************************************************************/
/*                              Includes                                */
/************************************************************************/


/************************************************************************/
/*								MACROS					
 *               */
/************************************************************************/

/**
 * @brief Heart rate service UUID
 */
#define HEART_RATE_SERVICE_UUID                                                 (0x180D)

/**
 * @brief Heart rate measurement characteristic UUID
 */
#define HEART_RATE_MEASUREMENT_CHAR_UUID										(0x2A37)

/**
 * @brief Body Sensor location characteristic UUID
 */
#define BODY_SENSOR_LOCATION_CHAR_UUID											(0x2A38)

/**
 * @brief Heart Rate Control Point characteristic UUID
 */
#define HEART_RATE_CONTROL_POINT_CHAR_UUID										(0x2A39)

/**
 * @brief Total no of characteristic in heart rate service
 */
#define HR_TOTAL_CHARATERISTIC_NUM													(3)

/**
 * @brief number for Enabling the notification
 */
#define HR_NOTIFICATION_DISABLE														(0)

/**
 * @brief number for disabling the notification
 */
#define HR_NOTIFICATION_ENABLE														(1)

/**
 * @brief number for resetting energy expended
 */
#define HR_CONTROL_POINT_RESET														(2)

/**
 * @brief maximum size of flags field of hr mm characteristic in bytes
 */
#define HR_MM_FLAGS_SIZE                                                            (1)

/**
 * @brief maximum size of heart rate  field of hr mm characteristic in bytes
 */
#define HR_MM_VAL_SIZE                                                              (2)

/**
 * @brief maximum size of heart rate  field of hr mm characteristic in bytes
 */
#define HR_MM_EX_SIZE                                                               (2)

/**
 * @brief maximum size of rr value field of hr mm characteristic in bytes
 */
#define HR_MM_RR_SIZE                                                               (2 * 2)

/************************************************************************/
/*								enum					
 *               */
/************************************************************************/

typedef enum {
	OTHERS = 0,
	CHEST,
	WRIST,
	FINGER,
	HAND,
	EAR_LOBE,
	FOOT,
} body_sensor_location_val;

/************************************************************************/
/*							Types
 *                                      */
/************************************************************************/

/**
 * @brief hr_gatt_service_handler is the service handler function
 */
typedef struct hr_gatt_service_handler {
	/** service uuid */
	at_ble_uuid_t serv_uuid;
	/** service handle */
	at_ble_handle_t serv_handle;
	/** characteristic handle */
	at_ble_characteristic_t serv_chars[HR_TOTAL_CHARATERISTIC_NUM];
} hr_gatt_service_handler_t;

/************************************************************************/
/*						Functions
 *                                      */
/************************************************************************/

/**@brief Heart Rate service and characteristic initialization(Called only once
 * by user).
 *
 * @param[in] Heart rate service instance
 *
 * @return none
 */
void hr_init_service(hr_gatt_service_handler_t *heart_rate_serv);

/**@brief Register a dis service instance inside stack.
 *
 * @param[in] dis_primary_service dis service instance
 *
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t hr_primary_service_define(
		hr_gatt_service_handler_t *dis_primary_service);

/**@brief handler for characteristic write called by profile
 *
 * @param[in] hr_gatt_service_handler_t servcie handler
 *
 * @return number representing the changed characteristic
 */
uint8_t hr_write_value_handler(hr_gatt_service_handler_t *hr_primary_service,
		at_ble_characteristic_changed_t *params);

#endif /* __HEART_RATE_H__ */
// </h>

// <<< end of configuration section >>>
