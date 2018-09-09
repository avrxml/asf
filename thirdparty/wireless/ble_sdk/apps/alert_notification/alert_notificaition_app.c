/**
* \file
*
* \brief Alert Notification Application
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
 * \mainpage Alert Notification Profile Example
 * \section Introduction
 * **************************** Introduction *********************************
 *	+ The Alert Notification example application bring-up the Alert Notification
 *     profile defined by the Bluetooth SIG.
 *
 *	+ The Alert Notification profile allows a device like a watch to obtain 
 *     information from a cellphone about incoming calls, missed calls, 
 *     and SMS/MMS messages. 
 *
 *	+ The information may include the caller ID for an incoming call or the 
 *     sender's ID for email/SMS/MMS but not the message. This profile also enables the 
 *     client device to get information about the number of unread messages on 
 *     the server device.
 *
 *	+ The Alert Notification example application supports the following Features:
 *	+ Device Discovery and Disconnection
 *	+ Pairing / Bonding
 *	+ Alert Notification Service
 *	+ Alert on incoming call
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
 *  + 6. On a BLE compatible Android phone, enable Bluetooth in the Settings page. Start the 'Atmel Smart Connect' App and scan for devices. 
 *    **ATMEL-ANS** will be appear among the devices scanned. Click on **ATMEL-ANS** to connect to supported platform.
 *  + 7. Once connected, the client side will request for the pairing procedure . The console log provides a guidance for the user to 
 *    enter the pass-key.
 *  + 8. After connection gets established, 'Alert Notification Service' page appears on foreground and then click on the same page.
 *  + 9. Enable the notifications by using the SW0 button as described in Chapter 4. The mobile app should reflect the status 
 *  + 10. The user can trigger a missed call/send an SMS to Android phone. The corresponding notification gets displayed at ATBTLC1000 side console logs.
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
 *  + The Alert Notification service exposes alert information in a device. 
 *  + This information includes the following:
 *	    + Type of alert occurring in a device
 *        + Additional text information such as caller ID or sender ID
 *        + Count of new alerts
 *        + Count of unread alert items
 *  + Alert Notification service has five characteristics
 *    + Supported New Alert Category
 *    + This characteristic exposes what categories of new alert is supported in the server.
 *  + New Alert:
 *    + This characteristic exposes information about the count of new alerts.
 *    + Supported Unread Alert Category.
 *    + This characteristic exposes what categories of unread alerts are supported in the server
 *  + Unread Alert Status:
 *    + This characteristic exposes the count of unread alert events existing in the server
 *    + Alert Notification Control Point
 *    + This characteristic allows the peer device to enable/disable the alert 
 *	  notification of new alert and unread event more selectively 
 *	  done by setting or clearing the notification bit in the Client Characteristic 
 *	  Configuration for each alert characteristic.
 *
 *- BLE Platform Services -
 *  +  Interface Settings -
 *	  + Connect ATBTLC1000 XPRO to SAML21-XPRO-B -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMD21-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMG55-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAM4S-XPRO  -> EXT1
 *  +  Serial Console COM port settings -
 *	  + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *	+  6-Wire Mode Connection Setup -
 *	  + Pins are 1:1 match with SAML21/D21 Xpro EXT1 Header to BTLC1000 XPro Header
 *	  + UART(No Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin13, Tx-Pin14)
 *	  + UART(With Flow Control)-SAM G55 Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6, Rx-Pin16, Tx-Pin17)
 *	  + BTLC1000 Wakeup Pin-SAM G55 XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55 XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55 Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55 Xpro Pins(Pin19)
 *  +  4-Wire Mode Connection setup -
 *	  + UART(With Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin15, Tx-Pin17, RTS-Pin16, CTS-Pin18)
 *	  + BTLC1000 Wakeup Pin-SAM L21/D21 XPro Pins (Rx-Pin6)
 *	  + BTLC1000 Chip Enable Pin-SAM L21/D21 XPro Pins (Rx-Pin4)
 *	  + UART(With Flow Control)-SAM G55/4S Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6)
 *	  + BTLC1000 Wakeup Pin-SAM G55/4S XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55/4S XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55/4S Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55/4S Xpro Pins(Pin19)
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
#include "at_ble_errno.h"
#include "at_ble_trace.h"
#include "platform.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_manager.h"
#include "alert_notification.h"
#include "alert_notification_profile.h"

#define APP_BUTTON_EVENT_ID		(1)

extern gatt_anp_handler_t anp_handle;

extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];

volatile bool user_request = false;

volatile bool notification_enable = false;

volatile bool app_state = false;

/* Flag to avoid spurious interrupt posting  during/before reset and initialization completed */
volatile bool app_init_done = false;

user_custom_event_t app_button_event =
{
	.id = APP_BUTTON_EVENT_ID,
	.bptr = NULL
};

/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/
/**
 * @brief Button Press Callback 
 */
void button_cb(void)
{
	if(app_init_done)
	{
		at_ble_event_user_defined_post(&app_button_event);	
	}
}

/**
 * @brief Timer Callback
 */
static void timer_callback_handler(void)
{
	/* Free to use for User Application */
}

static at_ble_status_t anp_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	user_custom_event_t **app_custom_event = (user_custom_event_t **)param;
	
	/* timer interrupt event for battery update */
	if ((((*app_custom_event)->id) == APP_BUTTON_EVENT_ID))
	{
		/* Button debounce delay*/
		delay_ms(350);
#ifdef ENABLE_PTS
			DBG_LOG("Press 1 for service discovery");
			DBG_LOG("Press 2 for write notification");
			DBG_LOG("Press 3 for disable notification");
			DBG_LOG("Press 4 for write to alert notification control point");
			DBG_LOG("And press Enter");
			uint8_t ncp_data[2] = {0};
			int option = 0;
			scanf("%d", &option);
			DBG_LOG("Received %d",option);
		switch (option) {
			case 1 :
			alert_service_discovery();
			break;
			case 2 :
			anp_client_write_notification_handler();
			break;
			case 3 :
			anp_client_disable_notification();
			break;
			case 4 :
			DBG_LOG("Enter alert catagory[email/news/..] and press Enter");
			scanf("%d",&option);
			if (option > 255) {
				DBG_LOG("Entered a wrong value[0-255]");
				break;
				} else {
				ncp_data[1] = (uint8_t) option;
				DBG_LOG("Alert catogory is %d",ncp_data[1]);
			}
			DBG_LOG("Enter the command ID[0 - 5] and press Enter");
			scanf("%d", &option);
			if (option > 255) {
				DBG_LOG("Entered a wrong value[0-255]");
				break;
				} else {
				ncp_data[0] = (uint8_t) option;
				DBG_LOG("Command ID is %d",ncp_data[0]);
			}
			anp_write_to_ncp(ncp_data);
			break;
		}
#else
		if (notification_enable) {
			anp_client_write_notification_handler();
			notification_enable = false;
			} else {
			anp_client_disable_notification();
			notification_enable = true;
		}
#endif
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;
}

/* All BLE Manager Custom Event callback */
static const ble_custom_event_cb_t anp_custom_event_cb = {
	.custom_event = anp_custom_event
};

/**
 * @brief app_connected_state profile notifies the application about state
 * @param[in] connected
 */
static void app_connected_state(bool connected)
{
	app_state = connected;
	if (connected) {
		DBG_LOG_DEV("App connected");	
	} else {
		/* Starting advertisement on disconnection */
		anp_client_adv();
	}		
}

int main(void)
{
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif
	
	/* Initialize serial console */
	serial_console_init();
	
	/* Initializing the button */
	button_init();
	
	/* Initializing the hardware timer */
	hw_timer_init();
	
	/*Registration of timer callback*/
	hw_timer_register_callback(timer_callback_handler);
	
	/* Register the connected call back with the profile */
	register_connected_callback(app_connected_state);
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&anp_custom_event_cb);
	
	DBG_LOG("Alert Notification Profile Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	app_init_done = true;
	
	/* Initializing the anp profile */
	anp_client_init(NULL);
	
	/* Starting the advertisement */
	anp_client_adv();
	
	/* Capturing the events  */
	while(1) {

		/* BLE Event Task */
		ble_event_task();
		
	}
}
