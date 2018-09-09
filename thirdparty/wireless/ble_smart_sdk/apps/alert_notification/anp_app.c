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
 *- Running the Demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *  + 6. On a BLE compatible Android phone, enable Bluetooth in the Settings page. Start the 'Atmel Smart Connect' App and scan for devices. 
 *    **ATMEL-ANP** will be appear among the devices scanned. Click on **ATMEL-ANP** to connect to supported platform.
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
 *  +  Serial Console COM port settings -
 *	  + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 **\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 *- Links for SAMB11-
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
**/
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/
#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "alert_notification.h"
#include "alert_notification_profile.h"
#include "button.h"
#include "samb11_delay.h"

bool notification_enable = false;
bool app_state = false;
extern bool button_debounce;


/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/
/**
 * @brief Button Press Callback 
 */
static void button_cb(void)
{
    send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	
}

/**
 * @brief Timer Callback
 */
static void timer_callback_handler(void)
{
	/* Free to use for User Application */
}

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

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	
	if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
#ifdef ENABLE_PTS
		DBG_LOG("Press 1 for service discovery");
		DBG_LOG("Press 2 for write notification");
		DBG_LOG("Press 3 for disable notification");
		DBG_LOG("Press 4 for write to alert notification control point");
		DBG_LOG("And press Enter");
		uint8_t ncp_data[2] = {0};
		uint8_t option = 0;
		option = getchar_b11();
		option= option - 48;
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
			DBG_LOG("Enter alert category[email/news/..] and press Enter");
			option = getchar_b11();
			option = option - 48;
			if (option > 255) {
				DBG_LOG("Entered a wrong value[0-255]");
				break;
				} else {
				ncp_data[1] = (uint8_t) option;
				DBG_LOG("Alert category is %d",ncp_data[1]);
			}
			DBG_LOG("Enter the command ID[0 - 5] and press Enter");
			option = getchar_b11();
			option = option - 48;
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
		delay_ms(200);
		button_debounce = true;
	}
	else if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		/* Free to use for Timer platform event */
	}
	return status;
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t anp_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

int main(void)
{
	notification_enable = false;
	app_state = false;

	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();

	/* Initializing the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);
	
	/* Initializing the hardware timer */
	hw_timer_init();
	
	/*Registration of timer callback*/
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Alert Notification Profile Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initializing the anp profile */
	anp_client_init(NULL);

	/* Register the connected call back with the profile */
	register_connected_callback(app_connected_state);
	
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&anp_app_custom_event_handle);
	
	/* Starting the advertisement */
	anp_client_adv();
	
   /* Capturing the events  */
	while(1) {
		/* BLE Event Task */
		ble_event_task(20);
		}
}
