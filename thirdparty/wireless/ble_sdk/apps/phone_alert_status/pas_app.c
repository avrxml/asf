/**
* \file
*
* \brief Phone Alert Status Application
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
 * \mainpage phone alert status
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 * The Phone Alert Status example application bring-up the Phone alert profile 
 * defined by the Bluetooth SIG.
 * The Phone Alert Status profile (PAS) is used to obtain the Phone Alert Status
 * exposed by the Phone Alert Status service on a mobile device. Alert Status 
 * and Ringer Setting information of a mobile phone can be received and modified 
 * by the Phone Alert Status service. The device can also use this profile to 
 * configure ringer status on the mobile device. 
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
 *  + 5. The device is now in advertising mode.
 *  + 6. On a BLE compatible Android phone enable Bluetooth in the Settings page.
 *       Use the Atmel Smart Connect App and scan for devices. ATMEL-PAS 
 *       will be appear amongst the devices scanned. Click on ATMEL-PAS to connect.
 *  + 7. Once connected, the client side will request for the pairing procedure . 
 *       The console log provides a guidance for the user to enter the pass-key.
 *  + 8. Once the device is connected, notifications are automatically enabled and 
 *       the application reads the values of ‘Alert Status’, ‘Ringer Settings’, and 
 *      ‘Ringer Control Point’ characteristics, which are updated on the mobile app as shown.
 *  + 9. Now, when the SW0 button is pressed on supported platform, the device is 
 *       set to different modes by using the notifications.
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
 * - BLE Profiles -
 *   + The Phone Alert Status profile is used to obtain the Phone Alert Status 
 *     exposed by the Phone Alert Status service in the peer device. The 
 *     information of Alert Status and Ringer Setting of a phone can be received
 *     and changed by the Phone Alert Status service. This profile also enables 
 *     the device to configure ringer status on the peer device.
 *     Phone alert Profile supports one service:
 *     + Phone Alert Status Service -
 *       + The Phone Alert Status service uses the Alert Status characteristic, 
 *         Ringer Setting characteristic to expose the phone alert status and 
 *         Ringer Control Point characteristic to control phone into mute or enable.
 * - BLE Services -
 *  + The Phone Alert Status service uses the Alert Status characteristic, 
 *    Ringer Setting characteristic to expose the phone alert status and 
 *    Ringer Control Point characteristic to control phone into mute or enable.
 *
 *  + Phone alert Profile supports three characteristics:
 *    + Alert Status - 
 *      The Alert Status Characteristic includes three fields of information to express the alerting status as follows.
 *      + Ringer State 
 *      + Vibrator State
 *      + Display Alert State
 *    + Ringer Setting - 
 *      + The Ringer Setting characteristic returns the current value of the Ringer Setting when read.
 *    + Ringer Control point - 
 *      + When the Ringer Control Point characteristic is written, the server device performs an action based on the value (command).
 *        + The server device implements a state machine with two states, “Ringer Silent” and “Ringer Normal”
 *        + The command “Set Silent Mode” sets the state to “Ringer Silent”.
 *        + The command “Cancel Silent Mode” sets the state to “Ringer Normal”.
 *        + The command “Mute Once” silences the server device’s ringer.
 *        + The server device’s ringer is silenced when the state is “Ringer Silent” and act according to phone settings when the state is “Ringer Normal”.
 *
 * - BLE Platform Services -
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
 
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/
#include <asf.h>
#include "console_serial.h"
#include "stdio.h"
#include "at_ble_api.h"
#include "at_ble_trace.h"
#include "at_ble_errno.h"
#include "platform.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "pas_client.h"
#include "pas_app.h"

volatile uint8_t press_count = DEVICE_SILENT;		/*!< button press count*/

volatile bool flag = false;					/*!< To send values once per button press*/

volatile bool app_state;			/*!< state of the app,true for connected false for disconnected*/

/* Flag to avoid spurious interrupt posting  during/before reset and initialization completed */
volatile bool app_init_done = false;

user_custom_event_t app_button_event =
{
	.id = APP_BUTTON_EVENT_ID,
	.bptr = NULL
};

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_event_handler,
	.disconnected = app_disconnected_event_handler
};

/* All BLE Manager Custom Event callback */
static const ble_custom_event_cb_t pas_custom_event_cb = {
	.custom_event = pas_custom_event
};

static at_ble_status_t pas_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	user_custom_event_t **app_timer_custom_event = (user_custom_event_t **)param;
	
	if ( (app_state && !flag) && ( ((*app_timer_custom_event)->id) == APP_BUTTON_EVENT_ID) )
	{
		flag = true;
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;
}



/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/

/**
 * @brief display alert status info notifies the application about state
 * @param[in] data
 */
static void display_alert_status_info(uint8_t *data)
{
	if (data[0] & BIT0_MASK) {
		DBG_LOG("Ringer State Active");
	} else {
		DBG_LOG("Ringer State Inactive");
	}
	
	if (data[0] & BIT1_MASK) {
		DBG_LOG("Vibration State Active");
	} else {
		DBG_LOG("Vibration State Inactive");
	}
	
	if (data[0] & BIT2_MASK) {
		DBG_LOG("Display State Active");
	} else {
		DBG_LOG("Display State Inactive");
	}
}

/**
 * @brief display alert status info notifies the application about state
 * @param[in] data
 */
static void display_ringer_setting_info(uint8_t *data)
{
	if (data[0] == RINGER_SILENT) {
		DBG_LOG_CONT("Ringer Silent");
	} else if (data[0] == RINGER_NORMAL) {
		DBG_LOG_CONT("Ringer Normal");
	}	
}

/**
 * @brief app_connected_state blemanager notifies the application about state
 * @param[in] at_ble_connected_t
 */
static at_ble_status_t app_connected_event_handler(void *params)
{
	app_state = true;
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

/**
 * @brief app_connected_state ble manager notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_disconnected_event_handler(void *params)
{
		app_state = false;
		press_count = DEVICE_SILENT;
		pas_client_adv();
		ALL_UNUSED(params);
		return AT_BLE_SUCCESS;
}

/**
 * @brief displays alerts status value came in read response
 * @param[in] data
 * @param[in] len  
 */
static void app_alert_status_read(uint8_t *data, uint8_t len)
{
	DBG_LOG("\r\nAlert Status read:");
	DBG_LOG_DEV("Length of the data is %d",len);
	display_alert_status_info(data);
}

/**
 * @brief displays ringer setting value came in read response
 * @param[in] data
 * @param[in] len  
 */
static void app_ringer_setting_read(uint8_t *data, uint8_t len)
{
	DBG_LOG("\r\nRinger setting read :");
	DBG_LOG_DEV("Length of the data is %d",len);
	display_ringer_setting_info(data);
}

/**
 * @brief displays alert status value came in notification
 * @param[in] data
 * @param[in] len  
 */
static void app_alert_status_notify(uint8_t *data, uint8_t len)
{
	DBG_LOG("\r\nNotified Alert Status :");
	DBG_LOG_DEV("length of the data is %d",len);
	display_alert_status_info(data);
}

/**
 * @brief displays ringer setting value came in notification
 * @param[in] data
 * @param[in] len  
 */
static void app_ringer_setting_notify(uint8_t *data, uint8_t len)
{
	DBG_LOG("\r\nNotified Ringer setting :");
	DBG_LOG_DEV("length of the data is %d",len);
	display_ringer_setting_info(data);
}

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

/* To keep the app executing */
bool appp_exec = true;

int main(void)
{	
	at_ble_status_t status;
	#ifdef ENABLE_PTS
	uint8_t function_selector;
	#endif
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
	
	DBG_LOG("Initializing Phone Alert Status Profile Application");
	
	register_alert_status_read_callback(app_alert_status_read);
	
	register_ringer_setting_read_callback(app_ringer_setting_read);
	
	register_alert_status_notification_callback(app_alert_status_notify);
	
	register_ringer_setting_notification_callback(app_ringer_setting_notify);
		
		
		
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	app_init_done = true;
	
	/* Initializing the profile */
	pas_client_init(NULL);
	
	/* Starting the advertisement */		
	pas_client_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	&app_gap_handle);
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&pas_custom_event_cb);
	
	/* Capturing the events  */
	while(appp_exec) {
		/* BLE Event Task */
		ble_event_task();
		if (flag) {
			/* button debounce delay */
			delay_ms(350);
			flag = false;
			#ifdef ENABLE_PTS
			DBG_LOG_PTS("To choose the functionality enter the index of the functionality displayed");
			DBG_LOG_PTS("\t 1.Set Device to Silent");
			DBG_LOG_PTS("\t 2.Set Device to Mute Once");
			DBG_LOG_PTS("\t 3.Set Device to Cancel Mute");
			DBG_LOG_PTS("\t 4.Read Alert Status");
			DBG_LOG_PTS("\t 5.Read Ringer Setting");
			DBG_LOG_PTS("\t 6.Start Service Discovery");
			function_selector = getchar();
			function_selector = function_selector - 48;
			DBG_LOG("The option chosen is %d",function_selector);
			switch(function_selector) {
			
			case DEVICE_SILENT:
				pas_client_write_ringer_control_point(DEVICE_SILENT);
			break;
			
			case DEVICE_MUTE:
				pas_client_write_ringer_control_point(DEVICE_MUTE);	
			break;
			
			case DEVICE_NORMAL:
				pas_client_write_ringer_control_point(DEVICE_NORMAL);
			break;
			
			case READ_ALERT_STATUS:
				DBG_LOG("reading the alert status ");
				if ((status = pas_client_read_alert_status_char()) != AT_BLE_SUCCESS) {
					DBG_LOG("reading alert status invocation failed");
				}
				
			break;
			
			case READ_RINGER_SETTING:
				DBG_LOG("reading the ringer setting ");				
				if ((status = pas_client_read_ringer_setting_char()) != AT_BLE_SUCCESS) {
					DBG_LOG("reading ringer control point invocation failed");
				}
			case DISCOVER_ATTRIBUTES:
				DBG_LOG("Starting Service discovery");
				status = pas_client_start_service_discovery();
				if (status == AT_BLE_SUCCESS) {
					DBG_LOG("Started the service discovery successfully");
				} else {
					DBG_LOG("Service discovery failed");
				}
			break;
			
			default:
				DBG_LOG("Invalid Number pressed %d",function_selector);
			}
			
			#else
			if (press_count == DEVICE_SILENT) {
				DBG_LOG("\r\nDevice to silent");
			} 
			else if (press_count == DEVICE_MUTE) {
				DBG_LOG("\r\nDevice to Mute Once");
			} 
			else if (press_count == DEVICE_NORMAL) {
				DBG_LOG("\r\nDevice to cancel mute");
			} 
			else if (press_count == READ_REQUEST) {
				DBG_LOG("\r\nreading the alert status and ringer setting");
				if ((status = pas_client_read_ringer_setting_char()) != AT_BLE_SUCCESS) {
					DBG_LOG("reading ringer control point invocation failed");
				}
				
				if ((status = pas_client_read_alert_status_char()) != AT_BLE_SUCCESS) {
					DBG_LOG("reading alert status invocation failed");
				}

				press_count = DEVICE_SILENT;
				continue;
			}

			pas_client_write_ringer_control_point(press_count);
			press_count++;
			#endif
		}	
	}

    ALL_UNUSED(status);
	
	return 0;
}
