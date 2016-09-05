/**
 * \file
 *
 * \brief Heart Rate Sensor Application Declarations
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
 *    Atmel micro controller product.
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

#ifndef __HR_SENSOR_APP_H__
#define __HR_SENSOR_APP_H__

/**@brief Notification interval to determine the frequency of notifications */
#define NOTIFICATION_INTERVAL									            (1)

/**@brief Activities of user */
#define ACTIVITY_NORMAL										                 (0)
#define ACTIVITY_WALKING								                     (1)
#define ACTIVITY_BRISK_WALKING						                         (2)
#define ACTIVITY_RUNNING						                             (3)
#define ACTIVITY_FAST_RUNNING				                                 (4)
#define DEFAULT_ACTIVITY													 (0xFF)

/**@brief Time counter for change of activities by user*/
#define TIME_NORMAL_LIMIT													(40)
#define TIME_WALKING_LIMIT											        (80)
#define TIME_BRISK_WALK_LIMIT	                                            (120)
#define TIME_RUNNING_LIMIT			                                        (160)
#define TIME_FAST_RUNNING_LIMIT			                                    (200)

/**@brief Heart Rate maximum and minimum values for various activities*/
#define HEART_RATE_MIN_NORM													(50)
#define HEART_RATE_MAX_NORM													(70)

#define HEART_RATE_MIN_WALKING												(71)
#define HEART_RATE_MAX_WALKING												(90)

#define HEART_RATE_MIN_BRISK_WALK											(91)
#define HEART_RATE_MAX_BRISK_WALK											(110)

#define HEART_RATE_MIN_RUNNING												(111)
#define HEART_RATE_MAX_RUNNING												(130)

#define HEART_RATE_MIN_FAST_RUNNING											(130)
#define HEART_RATE_MAX_FAST_RUNNING											(151)

/**@brief RR interval  maximum and minimum values for simulation of rr values */
#define RR_VALUE_MAX                                                            (1000)
#define RR_VALUE_MIN                                                            (100)

#define ENERGY_RESET                                                            (0)

/**@brief energy expended for various activities*/
#define ENERGY_EXP_NORMAL                                                       (3)
#define ENERGY_EXP_WALKING                                                      (10)
#define ENERGY_EXP_BRISK_WALKING												(20)
#define ENERGY_EXP_RUNNING                                                      (40)
#define ENERGY_EXP_FAST_RUNNING													(60)

/**@brief time limit of last activity*/
#define START_OF_FIRST_ACTIVITY													(0)
#define END_OF_LAST_ACTIVITY													(200)

/**@brief The maximum length of the hr measurement characteristic data*/
#define HR_CHAR_VALUE_LEN                                                       (10)

/**@brief HR_VALUE_FORMAT_UINT16 the bit to represent hr value sent is 16 bit*/
#define HR_VALUE_FORMAT_UINT16												(0x1 << 0)

/**@brief SENSOR_CONTACT_FTR_NOT_SPRTD_NOT_CONTACT to represent the sensor
 *contact info*/
#define SENSOR_CONTACT_FTR_NOT_SPRTD_NOT_CONTACT							(0x1 << 2)
/**@brief SENSOR_CONTACT_FTR_NOT_SPRTD_NOT_CONTACT to represent the sensor
 *contact info*/
#define SENSOR_CONTACT_FTR_NOT_SPRTD_CONTACT_DTD							(0x3 << 1)

/**@brief ENERGY_EXPENDED_FIELD_PRESENT represents energy expended inclusion*/
#define ENERGY_EXPENDED_FIELD_PRESENT										(0x1 << 3)

/**@brief RR_INTERVAL_VALUE_PRESENT represents the rr value inclusion*/
#define RR_INTERVAL_VALUE_PRESENT											(0x1 <<	4)

/** @brief app_notification_cfm_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
at_ble_status_t app_notification_cfm_handler(void *params);

/** @brief connected state handler
 *  @param[in] status of the application
 */
static at_ble_status_t app_connected_event_handler(void *params);

/** @brief disconnected state handler
 *  @param[in] status of the application
 */
static at_ble_status_t app_disconnected_event_handler(void *params);

#endif /* __HR_SENSOR_APP_H__ */
