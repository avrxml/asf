/**
 * \file
 *
 * \brief Time Information Profile Application
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
 * \mainpage Time information profile
 * \section Introduction
 * ************************* Introduction ************************************
 *
 * The BLE Time Information Profile is an example profile application 
 * implementing BLE Time Service. 
 * When the BLE device connected with a compatible iPhone device
 * supporting time service reads the current time, date and day information 
 * from the iPhone and displays it on the console. 
 * This example application also works with BLE compatible Android devices running 
 *  Atmel Smart Connect mobile application.
 *
 * 
 * - Running the demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *	+ 5. The device is now in advertising mode.
 *  + 6. On the iPhone, enable Bluetooth in the Settings page. The phone will 
 *		  start to scan for devices. ATMEL-TIP will be appear among the devices 
 *        scanned.Click on ATMEL-TIP to connect.
 *	+ 7. Once connected, on the client side will request for pairing procedure 
 *        with iPhone.The console log provides guidance to the user to enter 
 *		  the pass-key on iPhone.
 *	+ 8. On iPhone side, a pop-up screen prompting the user to enter the pass-key.
 *		  Enter ‘123456’ in the text box and click on ‘Pair’.
 *	+ 9. Press the SW0 button on supported platform to read the internally 
 *        supported characteristic values from iPhone. 
 *
 * \section Modules
 * ***************************** MODULES ***************************************** 
 * - BLE Manger -  
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
 * - BLE Services-
 *	The Time profile enables the device to get the date, time, time zone, 
 *	and DST information and control the functions related to time.
 *	TIME-INFO PROFILE supports three services:
 *  + Current Time service -
 *    + This service is used to obtain the date and time,
 *	    and related information such as time zone as exposed by the Current 
 *	    Time service in the peer device.
 *	+ Next DST Change service -  
 *    + This service is used to obtain Information of 
 *      when next change of daylight savings time (DST) will occur can be 
 *      retrieved from the peer exposed by the Next DST Change service.
 *	+ Reference Time Update service -  
 *    + This service enables the device to request updating the time 
 *      on the peer device as exposed by the Reference Time Update service.
 *
 * - Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 * \section BluSDK Package
 * ***************************** BluSDK Package *****************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
/***********************************************************************************
 *									Includes		                               *
 **********************************************************************************/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "tip_app.h"
#include "time_info.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "current_time.h"
#include "next_dst.h"
#include "reference_time.h"
#include "led.h"
#include "button.h"
#include "samb11_delay.h"



/***********************************************************************************
 *									Types			                               *
 **********************************************************************************/
extern gatt_cts_handler_t cts_handle;
extern gatt_dst_handler_t dst_handle;
extern gatt_rtu_handler_t rtu_handle;
extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];
extern bool button_debounce;
extern volatile bool current_time_char_found;
extern volatile bool local_time_char_found;
extern volatile bool ref_time_char_found;
extern volatile bool time_with_dst_char_found;
extern volatile bool time_update_cp_char_found;
extern volatile bool time_update_state_char_found;
static bool completed_prev_read = true;
/** To keep the app executing continuously*/
bool app_exec_flag = false;
bool discovery_flag = false;
static at_ble_status_t tip_pair_done_cb(void *params);
static at_ble_status_t tip_encryption_status_changed_cb(void *params);
static at_ble_status_t tip_disconnect_cb(void *params);
static at_ble_status_t tip_discovery_complete_cb(void *params);
static at_ble_status_t app_read_response_cb(void *params);
volatile bool app_init_done = false;
/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/
static const ble_gap_event_cb_t tip_app_gap_handle = {
	.pair_done = tip_pair_done_cb,
	.encryption_status_changed = tip_encryption_status_changed_cb,
	.disconnected = tip_disconnect_cb
};

static const ble_gatt_client_event_cb_t tip_app_gatt_client_handle = {
	.discovery_complete = tip_discovery_complete_cb,
	.characteristic_read_by_uuid_response = app_read_response_cb
};

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if((plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX)) && completed_prev_read && discovery_flag && app_exec_flag )
	{
		//delay_ms(200);
		/* code for pts */
		#ifdef ENABLE_PTS
		DBG_LOG("Press 1 for Service discovery");
		DBG_LOG("Press 2 for Writing to time update control point");
		DBG_LOG("Press 3 for Enable/Disable notification");
		DBG_LOG("Press 4 for Read current time characteristics value");
		DBG_LOG("And press enter");
		uint8_t option = 0;
		option = getchar_b11();
		option = option - 48;
		DBG_LOG("Received %d",option);
		switch (option) {
			case 1 :
			time_info_service_discovery();
			break;
			case 2 :
			DBG_LOG("Enter time update control point value [1 - 2] and press enter");
			option = getchar_b11();
			if (!(tis_rtu_update_write(ble_dev_info[0].conn_info.handle,
			rtu_handle.tp_control_char_handle,
			(uint8_t)option) == AT_BLE_SUCCESS)) {
				DBG_LOG("Fail to write Time Update control point");
			}
			break;
			case 3 :
			DBG_LOG("Enter 0 to disable both notification & indication");
			DBG_LOG("Enter 1 to enable notification");
			DBG_LOG("Enter 2 to enable indication");
			DBG_LOG("Enter 3 to enable both notification and indication");
			DBG_LOG("And press Enter");
			option = getchar_b11();
			option = option - 48;
			time_info_write_notification_handler((uint16_t)option);
			break;
			case 4 :
			if (current_time_char_found) {
				if (tis_current_time_read( ble_dev_info[0].conn_info.handle,
				cts_handle.curr_char_handle)
				== AT_BLE_SUCCESS) {
					LED_Toggle(LED0);
					DBG_LOG_DEV("CurrentTime info request success");
					completed_prev_read = false;
				}
			}
			break;
		}
		#else /* code for pts */
		if (current_time_char_found) {
			if (tis_current_time_read( ble_dev_info[0].conn_info.handle,
			cts_handle.curr_char_handle)
			== AT_BLE_SUCCESS) {
				LED_Toggle(LED0);
				DBG_LOG_DEV("CurrentTime info request success");
				completed_prev_read = false;
			}
		}
		#endif
		delay_ms(200);
		button_debounce = true;
	}
	return status;
}
/**
 * @brief Button Press Callback 
 */
static void button_cb(void)
{
	if(app_init_done)
	{
		send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/**
 * @brief Timer Callback
 */
void timer_callback_handler(void)
{
	/* Free to use for User Application */
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t tip_sensor_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};
/* Callback called during paring is done */
static at_ble_status_t tip_pair_done_cb(void *params)
{
	at_ble_pair_done_t *pairing_params;
	pairing_params = (at_ble_pair_done_t *)params;
	if(pairing_params->status == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("pair_done_cb : app_exec = true");
		app_exec_flag = true;
	}
	return AT_BLE_SUCCESS;
}

/* Callback called during encryption status changed */
static at_ble_status_t tip_encryption_status_changed_cb(void *params)
{
	at_ble_encryption_status_changed_t *enc_status;
	enc_status = (at_ble_encryption_status_changed_t *)params;
	if(enc_status->status == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("encryption_status_changed_cb : app_exec = true");
		app_exec_flag = true;
	}
	return AT_BLE_SUCCESS;
}

/* Callback called during discovery_complete */
static at_ble_status_t tip_discovery_complete_cb(void *params)
{
	at_ble_discovery_complete_t *discover_status = (at_ble_discovery_complete_t *)params;
	if (discover_status->status == AT_BLE_DISCOVER_SUCCESS || discover_status->status == AT_BLE_SUCCESS) 
	{
		DBG_LOG_DEV("discovery_complete_cb : discovery_flag = true");
		discovery_flag = true;
	}
	return AT_BLE_SUCCESS;
}

/* Callback called during disconnect */
static at_ble_status_t tip_disconnect_cb(void *params)
{
	DBG_LOG_DEV("disconnect_cb : app_exec = false, discovery_flag = false");
	discovery_flag = false;
	app_exec_flag = false;
	completed_prev_read = true;
	return AT_BLE_SUCCESS;
}

/**
 * @brief Callback registered for characteristic read response 
 * @param[in] char_read_resp @ref at_ble_characteristic_read_response_t
 * @return None
 *
 */
static at_ble_status_t app_read_response_cb(void *param)
{
	at_ble_characteristic_read_response_t *char_read_resp;
	char_read_resp = (at_ble_characteristic_read_response_t *)param;

	if(char_read_resp ->status != AT_BLE_SUCCESS) {
		completed_prev_read = true;
		return AT_BLE_FAILURE;
	}
	if (char_read_resp->char_handle == cts_handle.curr_char_handle) {
		if (local_time_char_found) {
			if (tis_current_time_read( char_read_resp->conn_handle,
			cts_handle.lti_char_handle )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Local Time info request success");
			}
		}else {
			completed_prev_read = true;
		}
	} else if (char_read_resp->char_handle == cts_handle.lti_char_handle) {
		if (ref_time_char_found) {
			if (tis_current_time_read( char_read_resp->conn_handle,
			cts_handle.rti_char_handle )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Reference Time info request success");
			}
		}else {
			completed_prev_read = true;
		}
	} else if (char_read_resp->char_handle == cts_handle.rti_char_handle) {
		if (time_with_dst_char_found) {
			if (tis_dst_change_read( char_read_resp->conn_handle,
			dst_handle.dst_char_handle )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Time with DST read request success");
			}
		}else {
			completed_prev_read = true;
		}
	} else if (char_read_resp->char_handle == dst_handle.dst_char_handle) {
		if (time_update_state_char_found) {
			if (tis_rtu_update_read( char_read_resp->conn_handle,
			rtu_handle.tp_state_char_handle, 20 )
			== AT_BLE_SUCCESS) {
				DBG_LOG_DEV("Time update state request success");
			}
		}
		completed_prev_read = true;
	}
	return AT_BLE_SUCCESS;
}

/**
 * @brief Main Function for Time Information Callback
 */
int main (void)
{

	platform_driver_init();
	acquire_sleep_lock();

	/* Initializing the console  */
	serial_console_init();
	
	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

 	/* Initialize the LED */
	led_init();
	
	/* Initializing the hardware timer */
	hw_timer_init();
	
	/*Registration of timer callback*/
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Time Profile Application");
	
	/* initialize the BLE chip  and Set the device mac address */
	ble_device_init(NULL);
	
	time_info_init();
	
	app_init_done = true;
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_CLIENT_EVENT_TYPE,
									&tip_app_gatt_client_handle);
									
	/* Callback registering for BLE-GAP Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, &tip_app_gap_handle);
	
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&tip_sensor_app_custom_event_handle);
	
	while(1) {
		ble_event_task(0xFFFFFFFF);
	}
}
	
