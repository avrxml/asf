/**
 * \file
 *
 * \brief Blood Pressure Sensor Application
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
*							        Includes											 *
****************************************************************************************/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "blp_sensor_app.h"
#include "blp_sensor.h"
#include "profiles.h"
/****************************************************************************************
*							        Globals												*
****************************************************************************************/
/** flag to check if indication has been sent successfully over the air*/
volatile bool indication_sent = true;

/** flag to check if notification has been sent successfully over the air*/
volatile bool notification_sent = true;

/** Flag to change the events from mmgh to kpa and vice versa*/
volatile bool units = APP_DEFAULT_VAL;

/** flag to send notifications */
 volatile bool notification_flag = APP_DEFAULT_VAL;

/** flag to send indication */
volatile bool indication_flag = APP_DEFAULT_VAL;

/** Flag to identify user request for indication and notification*/
volatile bool user_request_flag =  APP_DEFAULT_VAL;

/** Counter to maintain interval of indication*/
volatile uint8_t timer_count = APP_DEFAULT_VAL;

/** flag to send one notification for one second*/
volatile bool notify = 0;

/** flag to check the app state*/
volatile bool app_state;

/** flags for reversing the direction of characteristic*
	 change for indication*/
volatile int8_t operator[9] = {1,1,1,1,1,1,1,1,1};

/** Current systolic value  in mmhg*/
uint16_t systolic_val_mmhg = SYSTOLIC_MIN_MMHG;

/** Current diastolic value in mmhg*/
uint16_t diastolic_val_mmhg = DIASTOLIC_MIN_MMHG;

/** Current map value in mmhg*/
uint16_t map_val_mmhg = MAP_MIN_MMHG;

/** Current systolic in kpa*/
uint16_t systolic_val_kpa = SYSTOLIC_MIN_KPA;

/** current diastolic value in kpa*/
uint16_t diastolic_val_kpa = DIASTOLIC_MIN_KPA;

/** current map value in kpa*/
uint16_t map_val_kpa = MAP_MIN_KPA;

/** Current pulse rate value in kpa*/
uint16_t pulse_rate_val = PULSE_RATE_MIN;

/** Current time stamp */
prf_date_time_t time_stamp;

/* Intermediate Cuff Pressure Values for notification */
uint16_t interim_diastolic_mmhg = DIASTOLIC_MIN_MMHG;

uint16_t interim_diastolic_kpa = DIASTOLIC_MIN_KPA; 

uint16_t interim_systolic_mmhg = SYSTOLIC_MIN_MMHG;

uint16_t interim_systolic_kpa = SYSTOLIC_MIN_KPA;

uint16_t interim_map_mmhg = MAP_MIN_MMHG;

uint16_t interim_map_kpa = MAP_MIN_KPA;


static const ble_event_callback_t app_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	app_connected_state_handler,
	app_disconnected_state_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t app_gatt_server_handle[] = {
	app_notification_confirmation_handler,
	app_indication_confirmation_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/****************************************************************************************
*							        Functions											*
****************************************************************************************/
/**
 * @brief app_connected_state profile notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_connected_state_handler(void *params)
{
	app_state = true;
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t app_disconnected_state_handler(void *param)
{
	app_state = false;
	//Resetting all the simulated values
		
		interim_diastolic_mmhg = DIASTOLIC_MIN_MMHG;
		interim_diastolic_kpa = DIASTOLIC_MIN_KPA;
		interim_systolic_mmhg = SYSTOLIC_MIN_MMHG;
		interim_systolic_kpa = SYSTOLIC_MIN_KPA;
		interim_map_mmhg = MAP_MIN_MMHG;
		interim_map_kpa = MAP_MIN_KPA;
		systolic_val_mmhg = SYSTOLIC_MIN_MMHG;
		diastolic_val_mmhg = DIASTOLIC_MIN_MMHG;
		map_val_mmhg = MAP_MIN_MMHG;
		systolic_val_kpa = SYSTOLIC_MIN_KPA;
		diastolic_val_kpa = DIASTOLIC_MIN_KPA;
		map_val_kpa = MAP_MIN_KPA;
		pulse_rate_val = PULSE_RATE_MIN;
		units = !units;
		indication_sent = true;
		notification_sent = true;
		notify = false;
		timer_count = APP_DEFAULT_VAL;
		user_request_flag =  APP_DEFAULT_VAL;
		indication_flag = APP_DEFAULT_VAL;
		notification_flag = APP_DEFAULT_VAL;
		
		/* Starting advertisement */
		blp_sensor_adv();
		ALL_UNUSED(param);
		return AT_BLE_SUCCESS;
}

/** @brief Updating the time stamp
 *
 */
static void update_time_stamp(void)
{
	if (time_stamp.sec < SECOND_MAX)
	{
		time_stamp.sec++;
	}
	else
	{
		time_stamp.sec = 0;	
		if (time_stamp.min < MINUTE_MAX)
		{
			time_stamp.min++;
		}
		else
		{
			time_stamp.min = 0;
			if (time_stamp.hour < HOUR_MAX)
			{
				time_stamp.hour++;
			}
			else
			{
				time_stamp.hour = 0;
				if (time_stamp.day < DAY_MAX)
				{
					time_stamp.day++;
				}
				else
				{
					time_stamp.day = 1;
					if (time_stamp.month < MONTH_MAX)
					{
						time_stamp.month++;
					}
					else
					{
						time_stamp.month = 1;
						if (time_stamp.year < YEAR_MAX)
						{
							time_stamp.year++;
						} 
						else
						{
							time_stamp.year = 2015;
						}
					}
				}
			}
		}			
	}	
}

/** @brief initializes the time stamp with default time stamp
 *
 */
static void time_stamp_init(void)
{
	memset((uint8_t *)&time_stamp, 0, sizeof(prf_date_time_t));
	time_stamp.year = 2015;
	time_stamp.day = 1;
	time_stamp.month = 1;
}

/** @brief app_notification_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
static at_ble_status_t app_notification_confirmation_handler(void *params)
{

	if (((at_ble_cmd_complete_event_t *)params)->status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("App Notification Successfully sent over the air");
		notification_sent = true;
	} else {
		DBG_LOG_DEV("Sending Notification over the air failed");
		notification_sent = false;
	}
	return AT_BLE_SUCCESS;
}



/** @brief app_indication_confirmation_handler called by ble manager 
 *	to give the status of notification sent
 *  @param[in] at_ble_cmd_complete_event_t address of the cmd completion
 */	
static at_ble_status_t app_indication_confirmation_handler(void *params)
{
	if (((at_ble_cmd_complete_event_t * )params)->status == AT_BLE_SUCCESS) {
		DBG_LOG_DEV("App Indication successfully sent over the air");
		indication_sent = true;
		user_request_flag = false;
		timer_count = 0;
		DBG_LOG("\r\nPress the button to receive the blood pressure parameters");
	} else {
		DBG_LOG_DEV("Sending indication over the air failed reason %x ",
								((at_ble_cmd_complete_event_t * )params)->status);
		indication_sent = false;
	}
	return AT_BLE_SUCCESS;
}

/** @brief blp_value_update which will change the blood pressure measurement operations
 *  @param[in] data to which the blood pressure parameter value to be appended
 *  @param[in] idx index where the value has to be updated
 *  @param[in] value_type which will determine which blood pressure parameter 
 */
static void blp_value_update(uint8_t *data, uint8_t idx, uint16_t value, uint8_t value_type)
{
	uint8_t min_val = 0, max_val = 0;
	
	switch(value_type) {
	case SYSTOLIC_MMHG:	
		min_val = SYSTOLIC_MIN_MMHG;
		max_val = SYSTOLIC_MAX_MMHG;
	break;
	
	case DIASTOLIC_MMHG:
		min_val = DIASTOLIC_MIN_MMHG;
		max_val = DIASTOLIC_MAX_MMHG;
	break;
	
	case MAP_MMHG:
		min_val = MAP_MIN_MMHG;
		max_val = MAP_MAX_MMHG;
	break;
	
	case PULSE_RATE:
		min_val = PULSE_RATE_MIN;
		max_val = PULSE_RATE_MAX;
	break;
	
	case SYSTOLIC_KPA:
		min_val = SYSTOLIC_MIN_KPA;
		max_val = SYSTOLIC_MAX_KPA;
	break;
	
	case DIASTOLIC_KPA:
		min_val = DIASTOLIC_MIN_KPA;
		max_val = DIASTOLIC_MAX_KPA;
	break;
	
	case MAP_KPA:
		min_val = MAP_MIN_KPA;
		max_val = MAP_MAX_KPA;
	break;
	
	case INTERIM_SYS_MMHG:
		min_val = SYSTOLIC_MIN_MMHG;
		max_val = SYSTOLIC_MAX_MMHG;
	break;
	
	case INTERIM_SYS_KPA:
		min_val = SYSTOLIC_MIN_KPA;
		max_val = SYSTOLIC_MAX_KPA;
	break;
	}
	
	if (value == max_val) {
		operator[value_type] = -1;
	} else if (value == min_val) {
		operator[value_type] = 1;
	}
	memcpy((data + idx),&value,2);
}
/** @brief sends the characteristic data for the profile to send indication
 *
 */
static void blp_char_indication(void)
{
	uint8_t blp_data[BLP_DATA_LEN];
	uint8_t idx = 0;
	
	DBG_LOG("\n\n");
	
	DBG_LOG("The Blood Pressure Values are:");
	/* initializing flags to zero*/
	blp_data[0] = 0;
	/** Blood pressure measurement flags */
	if (units) {
		/** Units in mmhg*/
			blp_data[idx] |= (0x1)  & ~(BLOOD_PRESSURE_UNITS_FLAG_MASK);
	} else {
		/** Units in kpa*/
		    blp_data[idx] |= (0x1)  | BLOOD_PRESSURE_UNITS_FLAG_MASK ;
	} 
	
	/** Appending the flags for time stamp , pulse rate, user id , mm status */
	blp_data[idx]	|= BLOOD_PRESSURE_TIME_FLAG_MASK;
	blp_data[idx]	|= BLOOD_PRESSURE_PULSE_FLAG_MASK;
	blp_data[idx]	|= BLOOD_PRESSURE_USERID_FLAG_MASK;
	blp_data[idx++] |= BLOOD_PRESSURE_MMT_STATUS_FLAG_MASK;
	
	if (units) {
		systolic_val_mmhg = systolic_val_mmhg + (operator[SYSTOLIC_MMHG]);
		blp_value_update(blp_data,idx,systolic_val_mmhg,SYSTOLIC_MMHG);
		idx += 2;
		DBG_LOG("%-12s","Systolic");
		DBG_LOG_CONT("   %d mmhg",systolic_val_mmhg);
		
		diastolic_val_mmhg = diastolic_val_mmhg + (operator[DIASTOLIC_MMHG]);
		blp_value_update(blp_data,idx,diastolic_val_mmhg,DIASTOLIC_MMHG);
		idx += 2;
		DBG_LOG("%-12s","Diastolic");
		DBG_LOG_CONT("   %d mmhg",diastolic_val_mmhg);
		
		map_val_mmhg = map_val_mmhg + (operator[MAP_MMHG]);
		blp_value_update(blp_data,idx,map_val_mmhg,MAP_MMHG);
		idx += 2;
		DBG_LOG("%-12s","Map");
		DBG_LOG_CONT("   %d mmhg",map_val_mmhg);
	} else {
		systolic_val_kpa = systolic_val_kpa + (operator[SYSTOLIC_KPA]);
		blp_value_update(blp_data,idx,systolic_val_kpa,SYSTOLIC_KPA);
		idx += 2;
		DBG_LOG("%-12s","Systolic");
		DBG_LOG_CONT("   %02d kpa",systolic_val_kpa);
		diastolic_val_kpa = diastolic_val_kpa + (operator[DIASTOLIC_KPA]);
		blp_value_update(blp_data,idx,diastolic_val_kpa,DIASTOLIC_KPA);
		idx += 2;
		DBG_LOG("%-12s","Diastolic");
		DBG_LOG_CONT("   %02d kpa",diastolic_val_kpa);
		map_val_kpa = map_val_kpa + (operator[MAP_KPA]);
		blp_value_update(blp_data,idx,map_val_kpa,MAP_KPA);
		idx += 2;
		DBG_LOG("%-12s","Map");
		DBG_LOG_CONT("   %02d kpa",map_val_kpa);	
	}
		memcpy(&blp_data[idx],(uint8_t*)&(time_stamp.year),2);
		idx += 2;
		memcpy(&blp_data[idx],(uint8_t*)&(time_stamp.month),1);
		idx += 1;
		memcpy(&blp_data[idx],(uint8_t*)&(time_stamp.day),1);
		idx += 1;
		memcpy(&blp_data[idx],(uint8_t*)&(time_stamp.hour),1);
		idx += 1;
		memcpy(&blp_data[idx],(uint8_t*)&(time_stamp.min),1);
		idx += 1;
		memcpy(&blp_data[idx],(uint8_t*)&(time_stamp.sec),1);
		idx += 1;
		
	pulse_rate_val = pulse_rate_val + (operator[PULSE_RATE]);
	blp_value_update(blp_data,idx,pulse_rate_val,PULSE_RATE);
	idx += 2;
	DBG_LOG("%-12s","Pulserate");
	DBG_LOG_CONT("   %d bpm",pulse_rate_val);
	
	
	/** Appending User id */
	if (units) {
		blp_data[idx++] = USERID_1;
	} else {
		blp_data[idx++] = USERID_2;
	}
	
	/** Appending Measurement status field */
	blp_data[idx++] = 0xf;
	blp_data[idx++] = 0x0;
	
	blp_sensor_send_indication(blp_data,idx);	
	
}

/** @brief sends the characteristic data for profile to send notification
 *
 */
static void blp_char_notification(void)
{
	uint8_t blp_data[BLP_DATA_LEN];	
	uint8_t idx = 0;
	blp_data[0] = 0;
	
	if (units) {
		/** Units in mmhg*/
		blp_data[idx++] |= (0x1)  & ~(BLOOD_PRESSURE_UNITS_FLAG_MASK);
		blp_data[0] = blp_data[0] & 1;	
		DBG_LOG("Cuff pressure  %d mmhg",interim_systolic_mmhg);
		blp_value_update(blp_data,idx,interim_systolic_mmhg,INTERIM_SYS_MMHG);
		idx += 2;
		interim_systolic_mmhg = interim_systolic_mmhg + (operator[7]);
	} else {
		/** Units in kpa*/
		blp_data[idx++] |=  (0x1)  | BLOOD_PRESSURE_UNITS_FLAG_MASK;
		blp_data[0] = blp_data[0] & 1;
		DBG_LOG("Cuff pressure  %02d kpa",interim_systolic_kpa);
		blp_value_update(blp_data,idx,interim_systolic_kpa,INTERIM_SYS_KPA);
		idx += 2;
		interim_systolic_kpa = interim_systolic_kpa + (operator[8]);
	}

	/** Appending diastolic in kpa*/
	blp_data[idx++] = 0;
	blp_data[idx++] = 0;
	
	/** Appending map in kpa */
	blp_data[idx++] = 0;
	blp_data[idx++] = 0;
	
	blp_data[0]	|= BLOOD_PRESSURE_USERID_FLAG_MASK;
	
	/** Appending User id */
	if (units) {
		blp_data[idx++] = USERID_1;
		} else {
		blp_data[idx++] = USERID_2;
	}
	
	blp_sensor_send_notification(blp_data,idx);
}
/** @brief notification handler function called by the profile
 *	@param[in] enable will give weather notification has to enabled
 *  or disabled.
 */
static void app_notification_handler(bool enable)
{
	notification_flag = enable;
	
	if (notification_flag) {
		DBG_LOG("Notifications enabled by the remote device for interim cuff pressure");
	} else{
		DBG_LOG("Disabled notifications by the remote device for interim cuff pressure");
		timer_count = INDICATION_TIMER_VAL;
	}
}

/** @brief indication handler function called by the profile
 *	@param[in] enable will give weather indication has to enabled
 *  or disabled.
 */
static void app_indication_handler(bool enable)
{
	uint8_t blp_data[BLP_DATA_LEN];
	uint8_t idx = 0;	 
	indication_flag = enable;

	if (indication_flag) {
		DBG_LOG("Indications enabled by the remote device for blood pressure\n ");
		if (units) {
			blp_data[idx++] = 0;
			DBG_LOG("Systolic       %02d mmhg",systolic_val_mmhg);
			memcpy(&blp_data[idx],&systolic_val_mmhg,2);
			idx += 2;
			DBG_LOG("Diastolic      %02d mmhg",diastolic_val_mmhg);
			memcpy(&blp_data[idx],&diastolic_val_mmhg,2);
			idx += 2;
			DBG_LOG("Map            %02d mmhg",map_val_mmhg);
			memcpy(&blp_data[idx],&map_val_mmhg,2);
			idx += 2;
		} else {
			blp_data[idx++] = 0x1;
			memcpy(&blp_data[idx],&systolic_val_kpa,2);
			idx += 2;
			DBG_LOG("Systolic       %02d kpa",systolic_val_kpa);
			memcpy(&blp_data[idx],&diastolic_val_kpa,2);
			idx += 2;
			DBG_LOG("Diastolic      %02d kpa",diastolic_val_kpa);
			memcpy(&blp_data[idx],&map_val_kpa,2);
			idx += 2;
			DBG_LOG("Map            %02d kpa",map_val_kpa);
		}
		
		blp_data[0]	|= BLOOD_PRESSURE_PULSE_FLAG_MASK;
			DBG_LOG("Pulse rate     %d bpm",pulse_rate_val);
		memcpy(&blp_data[idx],&pulse_rate_val,2);
		idx += 2;
		
		/* Sending the default notification for the first time */
		blp_sensor_send_indication(blp_data,idx);	
	} else {
		DBG_LOG("Disabled indication by the remote server for blood pressure");
	}
}


/**
 * @brief Button Press Callback
 */
void button_cb(void)
{
	/* App connected state*/
	if (app_state) {
		if (user_request_flag == false) {
				/** For changing the units for each button press*/
				if (indication_flag) {
					units = !units;
				}
				
				if (indication_flag || notification_flag) {
					
					/** To trigger the blood pressure indication */
					user_request_flag = true;
					timer_count = 0;	
				}
				
				if (notification_flag) {
					DBG_LOG("\r\nStarted sending Interim Cuff Pressure Values");
				}	
		}
	}
	
}

/**
 * \brief Timer callback handler called on timer expiry
 */
static void timer_callback_handler(void)
{
	if (user_request_flag) {
		timer_count++;	
		notify = true;
	} 
	update_time_stamp();
}

/* To keep the app in execution mode */
bool app_exec = true;
/**
 * \brief Heart Rate Sensor Application main function
 */
int main(void)
{
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif
        
	/* Initialize the button */
	button_init();

	/* Initialize serial console */
	serial_console_init();

	/* Initialize the hardware timer */
	hw_timer_init();

	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	/** Initializing the application time stamp*/
	time_stamp_init();
	
	/** Start the timer */
	hw_timer_start(TIMER_INTERVAL);
	
	DBG_LOG("Initializing Blood Pressure Sensor Application");

	/* Registering the app_notification_handler with the profile */
	register_blp_notification_handler(app_notification_handler);
	
	/* Registering the app_indication_handler with the profile */
	register_blp_indication_handler(app_indication_handler);
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);

	/* Initialize the blood pressure sensor profile */
	blp_sensor_init(NULL);
	
	/* Triggering advertisement */
	blp_sensor_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	app_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	app_gatt_server_handle);
	
	
	/* Capturing the events  */
	while (app_exec) {
		ble_event_task();
		
		/* Checking for button press */
		if (user_request_flag ) {
			
				/*Sending notifications of interim cuff pressure*/
				
				if (timer_count < INDICATION_TIMER_VAL ) {
				
				/* checking for notification enabled */
					if (notification_flag) {
					
					/* Sending one notification per second */
						if (notify) {
					
							/* Checking for previous notification sent over the air */
							if (notification_sent) {
								blp_char_notification();
							}
							notify = 0;
						}	
					}
				}
				
				if (timer_count == INDICATION_TIMER_VAL) {
					if (indication_flag) {
						/*Checking for previous indication sent over the  air */
						if (indication_sent) {
						
							/* Send a indication */
							blp_char_indication();
							timer_count++;
						} else {
							DBG_LOG("Previous indication is failed and device is disconnecting");
							blp_disconnection();
						}
					} 
						//timer_count = 0;
						//user_request_flag = 0;
				}
		}
	}
	return 0;
}

