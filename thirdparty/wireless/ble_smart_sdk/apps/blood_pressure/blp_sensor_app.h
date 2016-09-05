/**
 * \file
 *
 * \brief Blood Pressure Sensor Application Declarations
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
