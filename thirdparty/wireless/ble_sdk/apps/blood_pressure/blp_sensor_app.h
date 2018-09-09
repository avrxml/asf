/**
 * \file
 *
 * \brief Blood Pressure Sensor Application Declarations
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
 *Support</a>
 */

/****************************************************************************************
*							        Macros												 *
****************************************************************************************/

/**
 * @brief Timer Interval which is 1 second
 */
#define TIMER_INTERVAL									(1)

/**
 * @brief Indication timer the time taken to send an indication
 */
#define INDICATION_TIMER_VAL							(10)

/**
 * @brief APP_DEFAULT_VAL default value for data in application
 */
#define APP_DEFAULT_VAL									(0)

/**
 * @brief BLP_DATA_LEN the blp measurment characteristic data length
 */
#define BLP_DATA_LEN									(19)

/**
 * @brief Mask for flags field in the blp measurement value
 */
#define BLOOD_PRESSURE_UNITS_FLAG_MASK					(0x1 << 0)

#define BLOOD_PRESSURE_TIME_FLAG_MASK					(0x1 << 1)

#define BLOOD_PRESSURE_PULSE_FLAG_MASK					(0x1 << 2)

#define BLOOD_PRESSURE_USERID_FLAG_MASK					(0x1 << 3)

#define BLOOD_PRESSURE_MMT_STATUS_FLAG_MASK				(0x1 << 4)

/**
 * @brief min and max fields for blp mm in both mmhg and kpa
 */

#define SYSTOLIC_MIN_MMHG									(80)
#define SYSTOLIC_MAX_MMHG									(130)

#define DIASTOLIC_MIN_MMHG									(60)
#define DIASTOLIC_MAX_MMHG									(90)

#define MAP_MIN_MMHG										(70)
#define MAP_MAX_MMHG										(100)

#define SYSTOLIC_MIN_KPA									(10)
#define SYSTOLIC_MAX_KPA									(17)

#define DIASTOLIC_MIN_KPA									(7)
#define DIASTOLIC_MAX_KPA									(11)

#define MAP_MIN_KPA											(8)
#define MAP_MAX_KPA											(12)

#define PULSE_RATE_MIN										(60)
#define PULSE_RATE_MAX										(120)

#define USERID_1											(1)
#define USERID_2											(2)

/**
 * @brief Default Time stamp Values
 */
							

/**
 * @brief Max Time stamp Values for time stamp calculation
 */
#define SECOND_MAX											(59)
#define MINUTE_MAX											(59)
#define HOUR_MAX											(23)
#define DAY_MAX												(31)
#define MONTH_MAX											(12)
#define YEAR_MAX											(9999)

/**
 * @brief Blood pressure parameters
 */
#define SYSTOLIC_MMHG										(0)
#define DIASTOLIC_MMHG										(1)
#define MAP_MMHG											(2)
#define SYSTOLIC_KPA										(3)
#define DIASTOLIC_KPA										(4)
#define MAP_KPA												(5)
#define PULSE_RATE											(6)
#define INTERIM_SYS_MMHG									(7)
#define INTERIM_SYS_KPA										(8)

/**
 * @brief app_connected_state ble manger notifies the application about state
 * @param[in] connected parameters
 */
static at_ble_status_t app_connected_state_handler(void *params);

/**
 * @brief app_connected_state ble manger notifies the application about state
 * @param[in] disconnection parameters
 */
static at_ble_status_t app_disconnected_state_handler(void *param);

/** @brief app_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
static at_ble_status_t app_notification_confirmation_handler(void *params);

/** @brief app_indication_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
static at_ble_status_t app_indication_confirmation_handler(void *params);
