/**
 * \file
 *
 * \brief Sensor tag Service declarations
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef __SENSOR_TAG_H__
#define __SENSOR_TAG_H__

/*- Includes ---------------------------------------------------------------*/
#include "at_ble_api.h"
#include "ble_utils.h"
#include "ble_manager.h"

/* macros */

/** @brief count of included service in tx power service
  * 
  */
#define ST_INCLUDED_SERVICE_COUNT		(0)

/** @brief count of characteristics in tx power service
  * 
  */
#define ENVIRONMENT_CHARACTERISTIC_COUNT		(2)
#define DEVICE_ORIENTATION_CHARACTERISTIC_COUNT		(6)
#define BATTERY_LEVEL_CHARACTERISTIC_COUNT		(1)

/** @brief scan_resp_len is the length of the scan response data */
#define SCAN_RESP_LEN					(10)

/** @brief LL_ADV_DATA_UUID_LEN the size of LL service uuid */
#define ADV_DATA_UUID_LEN			(16)
/** @brief LL_ADV_DATA_UUID_TYPE the total sizeof LL service uuid*/
#define ADV_DATA_UUID_TYPE			(0x06)

/** @brief ADV_TYPE_LEN */
#define ADV_TYPE_LEN					(0x01)

/* @brief PXP_ADV_DATA_NAME_DATA the actual name of device */
#define WD_ADV_DATA_NAME_DATA			("AT-STAG")

/** @brief PXP_ADV_DATA_NAME_LEN the  length of the device name */
#define WD_ADV_DATA_NAME_LEN			(7)

/** @brief PXP_ADV_DATA_NAME_TYPE the gap ad data type */
#define WD_ADV_DATA_NAME_TYPE			(0x09)

/** @brief APP_PXP_FAST_ADV between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
#define APP_FAST_ADV				160 // 100ms //(0x20) //20 ms//(100) //100 ms

/** @brief APP_PXP_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x3FFF in seconds, 0x0000 disables time-out.*/
#define APP_ADV_TIMEOUT				(1000) // 100 Secs

/* smart tag service UUIDs */

/* Environment service */
#define ENVIRONMENT_SERVICE_UUID ("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc0\xba\x5a\xf0")

/* Environment service characteristics */
#define ENVIRONMENT_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd0\xba\x5a\xf0")
#define ENV_ODR_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd1\xba\x5a\xf0")

/* Device orientation service */
#define DEVICE_ORIENTATION_SERVICE_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc1\xba\x5a\xf0")

/* Device orientation service characteristics */
#define GYRO_POSITIONS_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd4\xba\x5a\xf0")
#define ACCELERO_POSITIONS_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd7\xba\x5a\xf0")
#define ROTATION_VECTOR_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd8\xba\x5a\xf0")
#define MOTION_ODR_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd9\xba\x5a\xf0")
#define ACCELERO_DROP_IND_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xda\xba\x5a\xf0")
#define ACCELERO_STEP_INC_IND_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xdb\xba\x5a\xf0")


/* Battery level service */
#define BATTERY_SERVICE_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc3\xba\x5a\xf0") 

/* Battery level service characteristics */
#define BATTERY_LEVEL_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xdc\xba\x5a\xf0")


/* type definitions */

/** @brief GATT services handles
*/
typedef struct gatt_environment_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars[2];
}gatt_environment_service_handler_t;

typedef struct gatt_device_orientation_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars[6];
}gatt_device_orientation_service_handler_t;

typedef struct gatt_battery_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars;
}gatt_battery_service_handler_t;



typedef struct gyro_positions
{
	int16_t x_pos;
	int16_t y_pos;
	int16_t z_pos;
}gyro_positions_t;

typedef struct accelero_positions
{
	int16_t x_pos;
	int16_t y_pos;
	int16_t z_pos;
}accelero_positions_t;

typedef struct device_rotation_vector
{
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t w;
	
}device_rotation_vector_t;

typedef struct environment_data
{
	int16_t temperature;
	uint16_t pressure;
	uint32_t uv;
	uint8_t humidity;
}environment_data_t;

/* static variables */
static volatile bool connection_established_flag;
static volatile bool env_char_notification_flag;
static volatile bool gyro_char_notification_flag;
static volatile bool accelero_char_notification_flag;
static volatile bool rotation_vector_char_notification_flag;
static volatile bool drop_char_notification_flag;
static volatile bool step_inc_char_notification_flag;
static volatile bool low_bat_char_notification_flag;

/* extern variables */
extern gatt_environment_service_handler_t environment_service_handle;
extern uint16_t advertisement_timer;
extern bool started_advertising;
extern uint8_t env_sensor_period;
extern uint8_t motion_sensor_period;

/* inline functions */

/* check ble connection established */
static inline bool is_ble_connection_established(void)
{
	return (connection_established_flag);
}

/* check environment characteristic notification enabled */
static inline bool is_ble_env_char_notification_enabled(void)
{
	return (env_char_notification_flag);
}

/* check gyro characteristic notification enabled */
static inline bool is_ble_gyro_char_notification_enabled(void)
{
	return (gyro_char_notification_flag);
}

/* check accelero characteristic notification enabled */
static inline bool is_ble_accelero_char_notification_enabled(void)
{
	return (accelero_char_notification_flag);
}

/* check rotation vector characteristic notification enabled */
static inline bool is_ble_rotation_vector_char_notification_enabled(void)
{
	return (rotation_vector_char_notification_flag);
}

/* check drop characteristic notification enabled */
static inline bool is_ble_drop_char_notification_enabled(void)
{
	return (drop_char_notification_flag);
}

/* check step-inc characteristic notification enabled */
static inline bool is_ble_step_inc_char_notification_enabled(void)
{
	return (step_inc_char_notification_flag);
}

/* check low battery characteristic notification enabled */
static inline bool is_ble_low_bat_char_notification_enabled(void)
{
	return (low_bat_char_notification_flag);
}

/**@brief Function used to update characteristic value
 *
 * @param[in] lock_command_serv gatt service information
 * @param[in] char_len length of the new characteristic value
 * @param[in] char_data new characteristic value information
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */

/**
* \smart tag service advertisement initialization and adv start
*/
void wbe_service_adv(void);

/**
* \Lock command Initialization which initializes service,defines and start adv
*/
void wbe_init(void *param);

/**
 * @brief Event for connection 
 *
 * @param[in] 
 *
 */
void st_connection_event(at_ble_handle_t ble_handle);

/* @brief timer call back for rssi update
* enable the flags to execute the application taskc
*
*/
void timer_callback_handler(void);

/**
 * @brief Event for connection 
 *
 * @param[in] 
 *
 */
void wbe_connection_event(at_ble_handle_t ble_handle);

/* Application interfaces */
at_ble_status_t ble_notify_environment_data(environment_data_t* env_data);
at_ble_status_t ble_notify_gyro_positions(gyro_positions_t* gyro_positions);
at_ble_status_t ble_notify_accelero_positions(accelero_positions_t* accelero_pos);
at_ble_status_t ble_notify_device_rotation_vector(device_rotation_vector_t* rotation_vector);
at_ble_status_t ble_notify_accelero_step_detection(void);
at_ble_status_t ble_notify_device_drop_detection(void);
at_ble_status_t ble_notify_low_battery(void);
at_ble_status_t wbe_char_changed_handler(void *params);
void wbe_profile_init(void);
at_ble_status_t wbe_start_advertisement(void);
void timer_callback_fn(void);

#endif /* __SENSOR_TAG_H__ */
