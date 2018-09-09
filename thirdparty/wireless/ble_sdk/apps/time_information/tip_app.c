/**
 * \file
 *
 * \brief Time Information Profile Application
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
 * - Supported Evolution Kit -
 *	+ ATSAML21-XPRO-B + ATBTLC1000 XPRO
 *	+ ATSAMD21-XPRO + ATBTLC1000 XPRO
 *	+ ATSAMG55-XPRO + ATBTLC1000 XPRO
 *	+ ATSAM4S-XPRO + ATBTLC1000 XPRO
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
 *  +  Interface Settings -
 *	  + Connect ATBTLC1000 XPRO to SAML21-XPRO-B -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMD21-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMG55-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAM4S-XPRO  -> EXT1
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *	+  6-Wire Mode Connection Setup -
 *    + Pins are 1:1 match with SAML21/D21 Xpro EXT1 Header to BTLC1000 XPro Header
 *	  + UART(No Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin13, Tx-Pin14)
 *	  + UART(With Flow Control)-SAM G55 Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6, Rx-Pin16, Tx-Pin17)
 *	  + BTLC1000 Wakeup Pin-SAM G55 XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55 XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55 Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55 Xpro Pins(Pin19)
 *  +  4-Wire Mode Connection setup -
 * 	  + UART(With Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin15, Tx-Pin17, RTS-Pin16, CTS-Pin18)
 * 	  + BTLC1000 Wakeup Pin-SAM L21/D21 XPro Pins (Rx-Pin6)
 * 	  + BTLC1000 Chip Enable Pin-SAM L21/D21 XPro Pins (Rx-Pin4)
 * 	  + UART(With Flow Control)-SAM G55/4S Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6)
 * 	  + BTLC1000 Wakeup Pin-SAM G55/4S XPro Pins(Pin4)
 * 	  + BTLC1000 Chip Enable Pin-SAM G55/4S XPro Pins(Pin10)
 * 	  + BTLC1000 Vcc Pin-SAM L21/D21/G55/4S Xpro Pins(Pin20)
 * 	  + BTLC1000 GND Pin-SAM L21/D21/G55/4S Xpro Pins(Pin19)
 *
 * \section BluSDK Package
 * ***************************** BluSDK Package *****************************************
 * - BluSDK -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=documents
 * - ATBTLC1000 - 
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000 
 * - Developement kit -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=tools
 *		+ SAM L21 + BTLC1000 XPro -
 *			+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xstk
 *		+ BTLC1000 XPro -
 *				+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xpro
 * - Applications - 
 *		+ http://www.microchip.com/devices/ATBTLC1000.aspx?tab=applications
 * - Support and FAQ visit - 
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
#include "profiles.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "current_time.h"
#include "next_dst.h"
#include "reference_time.h"

/***********************************************************************************
 *									Types			                               *
 **********************************************************************************/
#define APP_INVALID_EVENT_ID	(0)
#define APP_TIMER_EVENT_ID		(1)
#define APP_BUTTON_EVENT_ID		(2)

extern gatt_cts_handler_t cts_handle;
extern gatt_dst_handler_t dst_handle;
extern gatt_rtu_handler_t rtu_handle;
extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];
volatile bool button_pressed = false;
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

user_custom_event_t app_custom_event[2] = {
	{
		.id = APP_TIMER_EVENT_ID,
		.bptr = NULL,
	},
	{
		.id = APP_BUTTON_EVENT_ID,
		.bptr = NULL
	}
};

/* Flag to avoid spurious interrupt posting  during/before reset and initialization completed */
volatile bool app_init_done = false;

static at_ble_status_t tip_pair_done_cb(void *params);
static at_ble_status_t tip_encryption_status_changed_cb(void *params);
static at_ble_status_t tip_disconnect_cb(void *params);
static at_ble_status_t tip_discovery_complete_cb(void *params);
static at_ble_status_t app_read_response_cb(void *params);
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

/**
 * @brief Button Press Callback 
 */
void button_cb(void)
{
	if(app_init_done)
	{
		button_pressed = true;
		/* Post the custom event */
		at_ble_event_user_defined_post(&app_custom_event[1]);
	}
}

/**
 * @brief Timer Callback
 */
void timer_callback_handler(void)
{
	/* Free to use for User Application */
}

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

static at_ble_status_t tip_app_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	user_custom_event_t **tip_app_custom_event = (user_custom_event_t **)param;
	if ((*tip_app_custom_event)->id != APP_BUTTON_EVENT_ID)
	{
		status = AT_BLE_FAILURE;
	}
	else if ((button_pressed) && (completed_prev_read) && \
		 (discovery_flag) && (app_exec_flag)) {
		delay_ms(200);
			/* code for pts */
		#ifdef ENABLE_PTS
		DBG_LOG("Press 1 for Service discovery");
		DBG_LOG("Press 2 for Writing to time update control point");
		DBG_LOG("Press 3 for Enable/Disable notification");
		DBG_LOG("Press 4 for Read current time characteristics value");
		DBG_LOG("And press enter");
		int option = 0;
		scanf("%d", &option);
		DBG_LOG("Received %d",option);
		switch (option) {
		case 1 :
			time_info_service_discovery();
			break;
		case 2 :
			DBG_LOG("Enter time update control point value [1 - 2] and press enter");
			scanf("%d", &option);
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
			scanf("%d", &option);
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
		button_pressed = false;
	}
	
	return status;
}

/* All TIP Custom Event callback */
static const ble_custom_event_cb_t tip_custom_event_cb = {
	.custom_event = tip_app_custom_event
};


/**
 * @brief Main Function for Time Information Callback
 */
int main (void)
{
#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
#elif SAM0
	system_init();
#endif
	/* Initializing the console  */
	serial_console_init();
	
	/* Initializing the button */
	button_init();
	
	/* Initializing the hardware timer */
	hw_timer_init();
	
	/*Registration of timer callback*/
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Time Profile Application");
	
	/* initialize the BLE chip  and Set the device mac address */
	ble_device_init(NULL);
	app_init_done = true;
	
	time_info_init();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_CLIENT_EVENT_TYPE,
									&tip_app_gatt_client_handle);
									
	/* Callback registering for BLE-GAP Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, BLE_GAP_EVENT_TYPE, &tip_app_gap_handle);
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&tip_custom_event_cb);
	
	while(1) {
		ble_event_task();
	}
}
	
