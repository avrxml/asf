/**
 * \file
 *
 * \brief Blood Pressure Sensor Application
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
/**
 * \mainpage BLOOD PRESSURE SENSOR Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The Blood Pressure example application bring-up the Blood Pressure profile defined by the Bluetooth SIG.
 * + The Blood Pressure Profile (BLP) enables a device to connect and interact with a Blood Pressure Sensor
 *   device that is used in consumer and professional health care applications.
 * + This example demonstrates a Blood Pressure Sensor application (GATT Server role).
 *- Blood Pressure Measurements -
 * + The Intermediate Cuff Pressure characteristic may be notified frequently during the course of a measurement so that 
 *   a receiving device can effectively update the display on its user interface during the measurement process.
 *   When the Client Characteristic Configuration descriptor is configured for indications and Blood Pressure measurement is available, 
 *   this characteristic shall be indicated while in a connection.
 *   The Blood Pressure Measurement Application, which is the GATT server, will hold the characteristics and send the measurement 
 *   values to the Blood Pressure Monitor.
 * + The Blood Pressure Measurement characteristic shall be used to send Blood Pressure measurements
 * + The Intermediate Cuff Pressure characteristic is used to send Current Cuff Pressure values to a device for display purposes while 
 *   the measurement is in progress
 * + The Blood Pressure Feature characteristic shall be used to describe the supported features of the Blood Pressure Sensor
 * + ATBTLC1000 together with the host MCU simulates a Blood Pressure Sensor (GATT Server Role) and sends simulated values to 
 *   the Blood Pressure Monitor (Smart Connect mobile application).
 *- Running the Demo -
 * + 1. Build and flash the binary into supported evaluation board.
 * + 2. Open the console using TeraTerm or any serial port monitor.
 * + 3. Press the Reset button.
 * + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 * + 5. The device is now in advertising mode.
 * + 6. On a BLE compatibl iPhone®/Android phone, enable Bluetooth in the Settings page. Start the Atmel Smart Connect App and scan for devices. 
        ATMEL-BLP will be appear among the devices scanned. Click on **ATMEL-BLP** to connect to supported platform.
 * + 7. Once connected, the client side will request for the pairing procedure . The console log provides a guidance for the user to 
        enter the pass-key.
 * + 8. Upon entering the Blood Pressure service page, the mobile application will enable the notifications and indications for interim 
 *      cuff pressure and blood pressure characteristics respectively. The blood pressure sensor device simulated by ATBTLC1000 will send the 
 *      current blood pressure values after receiving the indications enabling request.
 * + 9. The SW0 button can be used on XPRO Board to receive updated blood pressure measurements. The blood pressure sensor will first sends 
 *      the interim cuff pressure values as notifications and then sends the final blood pressure measurements as an indication. 
 *      The blood pressure measurements sent by blood pressure sensor are simulated values. 
 * + 10. The Atmel Smart Connect mobile App for Android also implements the same features and provides the same look and feel. 
 *       Henceforth incase if a BLE compatible Android phone is used, the demo instructions from the mobile app perspective remains the same as above.
 * \section Modules
 * ***************************** Modules **************************************
 *- BLE Manger -  
 *  + The Event Manager is responsible for handling the following:
 *    + Generic BLE Event Handling:-
 *       + BLE Event Manager handles the events triggered by BLE stack and also responsible 
 *  	 for invoking all registered callbacks for respective events. BLE Manager 
 *  	 handles all GAP related functionality. In addition to that handles multiple connection 
 *  	 instances, Pairing, Encryption, Scanning.
 *    + Handling Multi-role/multi-connection:-
 *  	  + BLE Event Manager is responsible for handling multiple connection instances 
 *  	  and stores bonding information and Keys to retain the bonded device. 
 *  	  BLE Manager is able to identify and remove the device information when pairing/encryption 
 *		  gets failed. In case of multi-role, it handles the state/event handling of both central and peripheral in multiple contexts.
 *    + Controlling the Advertisement data:-
 *  	  + BLE Event Manager is responsible for generating the advertisement and scan response data
 *  	  for BLE profiles/services that are attached with BLE Manager.
 *
 *- BLE Services -
 *  + This service exposes blood pressure and other data from a blood pressure monitor intended for healthcare applications.
 *  + The BLOOD PRESSURE Service exposes blood pressure and other data related to a blood pressure monitor.
 *  + Blood Pressure service has three characteristics:
 *    + **Blood Pressure Measurement**: 
 *      + The BLOOD PRESSURE MEASUREMENT characteristic is used to send a Blood Pressure measurement.
 *    + **Intermediate Cuff Pressure**: 
 *      + The INTERMEDIATE CUFF PRESSURE characteristic is used to send intermediate Cuff Pressure values to a device for display purposes while the measurement is in progress.
 *    + **Blood Pressure Feature**: 
 *      + The Blood Pressure Feature characteristic is used to describe the supported features of the Blood Pressure Sensor.
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/****************************************************************************************
*							        Includes											 *
****************************************************************************************/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "button.h"
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

volatile bool app_init_done = false; 

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_state_handler,
	.disconnected = app_disconnected_state_handler
};

static const ble_gatt_server_event_cb_t app_gatt_server_handle = {
	.notification_confirmed = app_notification_confirmation_handler,
	.indication_confirmed = app_indication_confirmation_handler
};

/* To keep the app in execution mode */
bool app_exec = true;

extern bool button_debounce;
bool isTimer = false;
bool isIndication = false;
uint8_t g_blp_data[BLP_DATA_LEN];
uint8_t g_idx = 0;

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
		DBG_LOG("\r\nPress the button to receive the blood pressure parameters");
		indication_sent = true;
		user_request_flag = false;
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
	
	if (value >= max_val) {
		operator[value_type] = -1;
	} else if (value <= min_val) {
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
		/* DBG_LOG("Flags are %d and length is %d",blp_data[0],idx); */

		isIndication = true;

		memcpy(g_blp_data, blp_data, sizeof(uint8_t) * BLP_DATA_LEN);
		g_idx = idx;
		send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);

		/* Sending the default notification for the first time */
		/* blp_sensor_send_indication(blp_data,idx); */
	} else {
		DBG_LOG("Disabled indication by the remote server for blood pressure");
	}
}
/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		if (user_request_flag) {
			timer_count++;
			notify = true;
		}

		update_time_stamp();

		hw_timer_start(TIMER_INTERVAL);
	}
	if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
	/* App connected state*/
	if (app_state) {
		if (user_request_flag == false) {
			if (indication_flag) {
				/** For changing the units for each button press*/
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

	button_debounce = true;
	}
	if (isIndication == true) {
	/* Sending the default notification for the first time */
	blp_sensor_send_indication(g_blp_data, g_idx);

	isIndication = false;
	}

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
				} else {
				DBG_LOG("Previous indication is failed and device is disconnecting");
				blp_disconnection();
				status = AT_BLE_FAILURE;
			}
		}
		timer_count = 0;
		user_request_flag = 0;
		}
	}
	return status;
}
/**
 * @brief Button Press Callback
 */
static void button_cb(void)
{
	if(!app_init_done)
	{
		//do nothing
	}
	else
	{
		DBG_LOG("button pressed");
		send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/**
 * \brief Timer callback handler called on timer expiry
 */
static void timer_callback_handler(void)
{
	if(!app_init_done)
	{
		//do nothing
	}
	else
	{	
		hw_timer_stop();
		//isTimer = true;
		send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t blp_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};
/**
 * \brief Heart Rate Sensor Application main function
 */
int main(void)
{
	indication_sent = true;
	notification_sent = true;
	units = APP_DEFAULT_VAL;
	notification_flag = APP_DEFAULT_VAL;
	indication_flag = APP_DEFAULT_VAL;
	user_request_flag =  APP_DEFAULT_VAL;
	timer_count = APP_DEFAULT_VAL;
	notify = 0;
	app_state = 0;
	memset(operator, 1, sizeof(int8_t) * 9);
	systolic_val_mmhg = SYSTOLIC_MIN_MMHG;
	diastolic_val_mmhg = DIASTOLIC_MIN_MMHG;
	map_val_mmhg = MAP_MIN_MMHG;
	systolic_val_kpa = SYSTOLIC_MIN_KPA;
	diastolic_val_kpa = DIASTOLIC_MIN_KPA;
	map_val_kpa = MAP_MIN_KPA;
	pulse_rate_val = PULSE_RATE_MIN;
	interim_diastolic_mmhg = DIASTOLIC_MIN_MMHG;
	interim_diastolic_kpa = DIASTOLIC_MIN_KPA;
	interim_systolic_mmhg = SYSTOLIC_MIN_MMHG;
	interim_systolic_kpa = SYSTOLIC_MIN_KPA;
	interim_map_mmhg = MAP_MIN_MMHG;
	interim_map_kpa = MAP_MIN_KPA;
	app_exec = true;
	isTimer = false;
	isIndication = false;
	memset(g_blp_data, 0, sizeof(uint8_t) * BLP_DATA_LEN);
	g_idx = 0;
/*
	app_gap_handle[0] = NULL;
	app_gap_handle[1] = NULL;
	app_gap_handle[2] = NULL;
	app_gap_handle[3] = NULL;
	app_gap_handle[4] = NULL;
	app_gap_handle[5] = app_connected_state_handler;
	app_gap_handle[6] = app_disconnected_state_handler;
	app_gap_handle[7] = NULL;
	app_gap_handle[8] = NULL;
	app_gap_handle[9] = NULL;
	app_gap_handle[10] = NULL;
	app_gap_handle[11] = NULL;
	app_gap_handle[12] = NULL;
	app_gap_handle[13] = NULL;
	app_gap_handle[14] = NULL;
	app_gap_handle[15] = NULL;
	app_gap_handle[16] = NULL;
	app_gap_handle[17] = NULL;
	app_gap_handle[18] = NULL;
	
	app_gatt_server_handle[0] = app_notification_confirmation_handler;
	app_gatt_server_handle[1] = app_indication_confirmation_handler;
	app_gatt_server_handle[2] = NULL;
	app_gatt_server_handle[3] = NULL;
	app_gatt_server_handle[4] = NULL;
	app_gatt_server_handle[5] = NULL;
	app_gatt_server_handle[6] = NULL;
	app_gatt_server_handle[7] = NULL;
	app_gatt_server_handle[8] = NULL;
	app_gatt_server_handle[9] = NULL;
*/
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* Initialize serial console */
	serial_console_init();

	/* Initialize the hardware timer */
	hw_timer_init();

	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);

	DBG_LOG("Initializing Blood Pressure Sensor Application");

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);

	/* Initialize the blood pressure sensor profile */
	blp_sensor_init(NULL);
	
	app_init_done =  true;
	/** Initializing the application time stamp*/
	time_stamp_init();

	/* Registering the app_notification_handler with the profile */
	register_blp_notification_handler(app_notification_handler);
	
	/* Registering the app_indication_handler with the profile */
	register_blp_indication_handler(app_indication_handler);
	
	/* Triggering advertisement */
	blp_sensor_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	&app_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	&app_gatt_server_handle);
	
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&blp_app_custom_event_handle);
	/** Start the timer */
	hw_timer_start(TIMER_INTERVAL);

	/* Capturing the events  */
	while (app_exec) {
		ble_event_task(0xFFFFFFFF);
	}
	return 0;
}

