/**
* \file
*
* \brief Battery Information Service - Application
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
 * \mainpage BATTERY INFO Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The Battery Application is an example application that bring-up Bluetooth Battery Service. 
 * + Battery service provides the capability to report battery level of the device using the battery characteristics. 
 * + Any application discovering the database can access the Battery service instance during discovery services. 
 * + For the purpose of demonstration, the example simulates battery level of the device. The battery level will be 
 *   increased by 1% for every 100ms and decreased by 1% every 1 second. The range of battery level is from 0% to 100%.
 *- Supported Evolution Kit -
 *	+ ATSAML21-XPRO-B + ATBTLC1000 XPRO
 *	+ ATSAMD21-XPRO + ATBTLC1000 XPRO
 *	+ ATSAMG55-XPRO + ATBTLC1000 XPRO
 *	+ ATSAM4S-XPRO + ATBTLC1000 XPRO
 *- Running the Demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode. 
 *  + 6. Enable Bluetooth® from the Settings page on a BLE compatible Android device or iPhone®. Use the Atmel Smart Connect 
 *       application to scan for peripheral devices. A device with the name ‘ATMEL-BAS’ will appear in the list of scanned devices. 
 *  + 7. Click on the “ATMEL-BAS” device. A pop-up will appear requesting pass-key. Enter “123456” and click on ‘Pair’.
 *  + 8. Once the “Battery Service” is clicked user gets notifications for battery level characteristic. User can stop receiving the notifications by disabling notifications.
 * \section Modules
 * ***************************** Modules **************************************
 *- BLE Manger -  Refer more info to \ref ble_manager
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
 *  + The Battery Service exposes the state of a battery within a device.
 *  + The Battery Service exposes the Battery State and Battery Level of a single battery or set of batteries in a device.
 *  + Battery service has one characteristic:
 *    + Battery Level: 
 *      + The Battery Level characteristic is read using the GATT Read Characteristic Value sub-procedure and returns the current battery level as a percentage from 0% to 100%; 
 *        0% represents a battery that is fully discharged, 100% represents a battery that is fully charged.
 *
 *- BLE Platform Services -
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
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 *- Links for BluSDK -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=documents
 *- Links for ATBTLC1000 -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000
 *- Development Kit -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=tools
 *- SAM L21 + BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xstk
 *- BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xpro
 *- Applications -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=applications
 *- Support and FAQ - visit 
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/


#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "battery.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "battery_info.h"
#include "ble_manager.h"
#include "conf_extint.h"

/* === GLOBALS ============================================================ */

#define BATTERY_UPDATE_INTERVAL	(1) //1 second
#define BATTERY_MAX_LEVEL		(100)
#define BATTERY_MIN_LEVEL		(0)

#define APP_TIMER_EVENT_ID		(1)
	
bat_gatt_service_handler_t bas_service_handler;

bool volatile timer_cb_done = false;
bool volatile notification_flag = true;
bool volatile battery_flag = true;
at_ble_handle_t bat_connection_handle;
uint8_t battery_level = BATTERY_MIN_LEVEL;
user_custom_event_t app_timer_event = 
{
.id = APP_TIMER_EVENT_ID,
.bptr = NULL
};

/**
* \Timer callback handler called on timer expiry
*/
static void timer_callback_handler(void)
{
	//Timer call back
	timer_cb_done = true;
	at_ble_event_user_defined_post(&app_timer_event);
}

/* Advertisement data set and Advertisement start */
static at_ble_status_t battery_service_advertise(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return status;
	}
	
	/* Start of advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_BAS_FAST_ADV, APP_BAS_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE Adv start Failed reason :%d",status);
	}
	return status;
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	at_ble_pair_done_t *pair_done = (at_ble_pair_done_t *)param;
	if(pair_done->status == AT_BLE_SUCCESS)
	{
		hw_timer_start(BATTERY_UPDATE_INTERVAL);
		ALL_UNUSED(param);
		return pair_done->status;
	}
	return pair_done->status;
}

/* Callback registered for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack */
static at_ble_status_t ble_enc_status_changed_app_event(void *param)
{
	at_ble_encryption_status_changed_t *encryption_status_changed = (at_ble_encryption_status_changed_t *)param;
	if(encryption_status_changed->status == AT_BLE_SUCCESS)
	{
		hw_timer_start(BATTERY_UPDATE_INTERVAL);
		ALL_UNUSED(param);
		return encryption_status_changed->status;
	}
	return encryption_status_changed->status;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	timer_cb_done = false;
	notification_flag = true;
	hw_timer_stop();

#ifdef BTLC_REINIT_SUPPORT /* BTLC1000 Re-Initialization Test */
	at_ble_status_t status;
	DBG_LOG("\r\n<<<<<<<<<<<<<<<<<<<<<<<< BTLC1000 Initializing Again >>>>>>>>>>>>>>>>>>>>>>>\r\n");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initialize the battery service */
	bat_init_service(&bas_service_handler, &battery_level);
	
	/* Define the primary service in the GATT server database */
	if((status = bat_primary_service_define(&bas_service_handler))!= AT_BLE_SUCCESS)
	{
		DBG_LOG("defining battery service failed %d", status);
	}
#endif

	battery_service_advertise();
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

static at_ble_status_t ble_connected_app_event(void *param)
{
	at_ble_connected_t *connected = (at_ble_connected_t *)param;
	bat_connection_handle = connected->handle;
	#if !BLE_PAIR_ENABLE
		ble_paired_app_event(param);
	#else
		ALL_UNUSED(param);
	#endif
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t ble_notification_confirmed_app_event(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(!notification_status->status)
	{
		notification_flag = true;
		DBG_LOG_DEV("sending notification to the peer success");				
	}
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;
	at_ble_status_t status = bat_char_changed_event(&bas_service_handler, char_handle);
	if(status == AT_BLE_SUCCESS)
	{
		notification_flag = true;	
	}
	else if (status == AT_BLE_PRF_NTF_DISABLED)
	{
		notification_flag = false;
	}

	return status;
}

void button_cb(void)
{
	/* For user usage */
}

static at_ble_status_t bat_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	user_custom_event_t **app_timer_custom_event = (user_custom_event_t **)param;
	
	/* timer interrupt event for battery update */
	if ( (timer_cb_done) && ( ((*app_timer_custom_event)->id) == APP_TIMER_EVENT_ID) )
	{
		timer_cb_done = false;
		/* send the notification and Update the battery level  */
		if(notification_flag){
			if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, notification_flag) == AT_BLE_SUCCESS)
			{
				DBG_LOG("Battery Level:%d%%", battery_level);
				notification_flag = false;
			}
			if(battery_level == BATTERY_MAX_LEVEL)
			{
				battery_flag = false;
			}
			else if(battery_level == BATTERY_MIN_LEVEL)
			{
				battery_flag = true;
			}
			if(battery_flag)
			{
				battery_level++;
			}
			else
			{
				battery_level--;
			}
			hw_timer_start(BATTERY_UPDATE_INTERVAL);
		}
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;
}

/* All BLE Manager Custom Event callback */
static const ble_custom_event_cb_t battery_custom_event_cb = {
	.custom_event = bat_custom_event
};

static const ble_gap_event_cb_t battery_app_gap_cb = {
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
	.pair_done = ble_paired_app_event,
	.encryption_status_changed = ble_enc_status_changed_app_event
};


static const ble_gatt_server_event_cb_t battery_app_gatt_server_cb = {
	.notification_confirmed = ble_notification_confirmed_app_event,
	.characteristic_changed = ble_char_changed_app_event
};

/**
* \Battery Service Application main function
*/
int main(void)
{
	at_ble_status_t status;		
	
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
	
	#if defined PDS_SERVICE
	/* Initialize the PDS Service */
		if(pds_module_init() == AT_BLE_SUCCESS){
			/* If button SW0 is pressed  during startup then the bonding information will be deleted */
			if(!button_0_input_level()){
				ble_remove_bonding_info();
			}else{
				DBG_LOG("Info: To Delete Bonding Information follow the Steps: ");
				DBG_LOG("\t1) Press and hold the reset button and SW0 button");
				DBG_LOG("\t2) Release Reset Button and wait for 5 seconds");
				DBG_LOG("\t3) Release SW0 Button");
			}
		}
	#endif /* defined PDS_SERVICE */
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Battery Service Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initialize the battery service */
	bat_init_service(&bas_service_handler, &battery_level);
	
	/* Define the primary service in the GATT server database */
	if((status = bat_primary_service_define(&bas_service_handler))!= AT_BLE_SUCCESS)
	{
		DBG_LOG("defining battery service failed %d", status);
	}
	
	battery_service_advertise();
	
	/* Register callbacks for gap related events */ 
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&battery_app_gap_cb);
									
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									&battery_app_gatt_server_cb);
									
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&battery_custom_event_cb);
	
	/* Capturing the events  */ 
	while (1) {
		/* BLE Event Task */
		ble_event_task();
	}
}


