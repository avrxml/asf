/**
* \file
*
* \brief Scan parameters Service - Application
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
 * \mainpage Scan Parameter
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 * The Scan Parameter service application bring-up the Scan Parameter service 
 * defined by the Bluetooth SIG.
 * This example demonstrates retrieving scan interval window information from 
 * a peer device implementing this service. The application implements a GATT 
 * server role. For the purpose of demonstration, the example also shows how 
 * to get the updated scan interval window value, by configuring the scan 
 * refresh characteristic for notification
 *
 * - Running the demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *  + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings 
 *    page. Start the Atmel Smart Connect App and scan for devices. ATMEL-SCP will 
 *    be appear among the devices scanned. Click on **ATMEL-SCP** to connect to 
 *    supported platform.
 *  + 7. Once connected, the client side will request for the pairing procedure . 
 *    The console log provides a guidance for the user to enter the pass-key.
 *  + 8. Once paired, the application displays the scan parameter service.
 *  + 9. On clicking the Scan Parameters service, user keeps getting notifications 
 *    for scan refresh characteristic value, user can disable notifications. 
 *  + 10. User can write the new value for Scan Interval Window characteristic on 
 *    mobile app. On the device side, the updated new value for scan interval 
 *    and scan window will be displayed in the console log.
 *
 * \section Modules
 * ***************************** MODULES ***************************************** 
 * - BLE Manager - 
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
 *  + The Scan Parameters Service enables a GATT Client to store the LE scan 
 *    parameters it is using on a GATT Server device so that the GATT Server
 *    can utilize the information to adjust behaviour to optimize power 
 *    consumption and/or reconnection latency
 *  + The Scan Parameters Service enables a GATT Server device to expose a 
 *    characteristic for the GATT Client to write its scan interval and 
 *    scan window on the GATT Server device.
 *
 *    Scan Parameters service has two characteristics:
 *  + Scan Interval Window -
 *    The Scan Interval Window characteristic is used to store the scan 
 *    parameters of the Client. Included in this characteristic are the 
 *    Scan Interval and Scan Window of the Client.
 *  + Scan Refresh -
 *    The Scan Refresh characteristic is used to notify the Client that 
 *    the Server requires the Scan Interval Window characteristic to be 
 *    written with the latest values upon notification
 *
 * - BLE Platform Services -
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
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/
#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "scan_param.h"
#include "ble_manager.h"
#include "scan_parameter_app.h"
#include "button.h"
#include "led.h"

bool app_exec  = true;
/* === GLOBALS ============================================================ */

#define SCAN_PRAM_UPDATE_INTERVAL	(5) //5 second

/* === GLOBALS ============================================================ */

sps_gatt_service_handler_t sps_service_handler;
uint16_t scan_interval_window[2];
uint8_t scan_refresh;

bool volatile timer_cb_done = false;
bool volatile notification_cfm_flag = true;

uint8_t scan_refresh_value = 0;

/**
* \Timer callback handler called on timer expiry
*/
void timer_callback_handler(void)
{
	//Timer call back
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* Advertisement data set and Advertisement start */
static at_ble_status_t sps_service_advertise(void)
{
	at_ble_status_t status;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return status;
	}
	
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE,
						 NULL, AT_BLE_ADV_FP_ANY, APP_SCP_FAST_ADV, 
						 APP_SCP_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS) { 
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else {
		DBG_LOG("BLE Adv start Failed");
	}
	return status;
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	at_ble_pair_done_t *pair_done = (at_ble_pair_done_t *)param;
	if(pair_done->status == AT_BLE_SUCCESS)
	{
		timer_cb_done = true;
		hw_timer_start(SCAN_PRAM_UPDATE_INTERVAL);
		LED_On(LED0);
		ALL_UNUSED(param);
	}
	return pair_done->status;
}

/* Callback registered for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack */
static at_ble_status_t ble_encryption_app_event(void *param)
{
	at_ble_encryption_status_changed_t *encryp_done = (at_ble_encryption_status_changed_t *)param;
	if(encryp_done->status == AT_BLE_SUCCESS)
	{
		timer_cb_done = true;
		hw_timer_start(SCAN_PRAM_UPDATE_INTERVAL);
		LED_On(LED0);
		ALL_UNUSED(param);
	}
	return encryp_done->status;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	timer_cb_done = false;
	hw_timer_stop();
	sps_service_advertise();
	LED_Off(LED0);
    ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CONNECTED event from stack */
static at_ble_status_t ble_connected_app_event(void *param)
{
	at_ble_connected_t *connected = (at_ble_connected_t *)param;
	if(connected->conn_status == AT_BLE_SUCCESS)
	{
		#if !BLE_PAIR_ENABLE
		ble_paired_app_event(param);
		#else
		ALL_UNUSED(param);
		#endif
	}
	return connected->conn_status;
}

/**
* \Service Characteristic change handler function
*/
static at_ble_status_t sps_char_changed_cb(void *param)
{
	at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;
	return sps_char_changed_event(&sps_service_handler, char_handle);
}

/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t sps_notification_confirmed_cb(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(!notification_status->status)
	{
		notification_cfm_flag = true;
		DBG_LOG_DEV("sending notification to the peer success");
	}
	return AT_BLE_SUCCESS;
}

static void button_cb(void)
{
	/* For user usage */
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		LED_Toggle(LED0);
		/* send the notification and Update the scan parameter */	
		
		if(sps_scan_refresh_char_update(&sps_service_handler, scan_refresh_value) == AT_BLE_SUCCESS)
		{
			DBG_LOG("Scan Refresh Characteristic Value: %d", scan_refresh_value);
			notification_cfm_flag = false;
		}
	}
}

static const ble_gap_event_cb_t ble_scan_param_app_gap_cb = {
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
	.pair_done = ble_paired_app_event,
	.encryption_status_changed = ble_encryption_app_event
};

static const ble_gatt_server_event_cb_t ble_scan_param_app_gatt_server_cb = {
	.notification_confirmed = sps_notification_confirmed_cb,
	.characteristic_changed = sps_char_changed_cb
};

static const ble_custom_event_cb_t ble_scan_param_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event 
};

/**
* \Scan Parameter Application main function
*/

int main(void)
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* Initialize the LED */
	led_init();

	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Scan Parameter Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initialize the scan parameter service */
	sps_init_service(&sps_service_handler, scan_interval_window, &scan_refresh);
	
	/* Define the primary service in the GATT server database */
	sps_primary_service_define(&sps_service_handler);
	
	sps_service_advertise();
	
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&ble_scan_param_app_gap_cb);
	
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									&ble_scan_param_app_gatt_server_cb);
									
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&ble_scan_param_app_custom_event_handle);
	
	/* Capturing the events  */ 
	while (1) {
		/* BLE Event Task */
		ble_event_task(0xFFFFFFFF);
	}	
	return 0;
}


