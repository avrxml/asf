/**
 * \file
 *
 * \brief Heart Rate Sensor Application Declarations
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
