/**
* \file
*
* \brief ANCS Application
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
/**
 * \mainpage ANCS Example
 * \section Introduction
 * **************************** Introduction *********************************
 *  + Bluetooth 'Apple Notification Center Service' is an example application where the device act as an Alert Notification Client with custom UUID as per Apple's ANCS specification.
 *  + This service is unique to Apple products and available only on iOS devices.
 *  + The ANCS Profile is used to enable a SAMB11 device to obtain notifications from an iOS device (e.g. iPhone) that exposes 'Apple Notification Center Service'. 
 *  + The ANCS Profile defines two roles:
 *  + **Notification Provider (NP)**: 
 *    The Notification Provider is a device that provides the IOS notifications.
 *  + **Notification Consumer (NC)**: 
 *    The Notification Consumer is a device that receives the IOS notifications and notification related data from Notification Provider. 
 *- Running the Demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up
 *  + 5. Goto iPhone settings page and  enable Bluetooth.  The phone will start to scan for devices.
 *       ATMEL-ANCS will be appear amongst the devices scanned. Click on ATMEL-ANCS to connect on supported platform
 *  + 6. Once connected, on the client side will request for pairing procedure with iPhone. The console log provides a guidance for the user to enter the pass-
 *       key on iPhone. 
 *  + 7. Upon completion of pairing process user can initiate a mobile terminated call to the iPhone. When the iPhone indicates an incoming call, 
 *       the corresponding incoming call alert is indicated in the console log on supported platform . 
 *       Once the call has terminated the device will wait for a new alert to occur.
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
 *  + The purpose of the Apple Notification Center Service (ANCS) is to give Bluetooth accessories (that connect to iOS devices through a Bluetooth low-energy link)
 *    a simple and convenient way to access many kinds of notifications that are generated on iOS devices.
 *  + The ANCS exposes two characteristics:
 *    + Notification Source: 
 *      + The Notification Source characteristic is the characteristic upon which an NC is informed of:
 *      + the arrival of a new iOS notification on the NP
 *      + the modification of an iOS notification on the NP
 *      + the removal of an iOS notification on the NP
 *    + Control Point and Data source: 
 *      + An NC may want to interact with an iOS notification. It may want to retrieve more information about it, including its contents, or it may want to perform actions on it. 
 *        The retrieval of these attributes is performed through the Control Point and Data Source characteristics.
 *      + An NC can issue a request to retrieve more information about an iOS notification by writing specific commands to the Control Point characteristic. 
 *        If the write to the Control Point characteristic is successful, the NP will promptly respond to the request through a stream of 
 *        GATT notifications on the Data Source characteristic.
 *
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *	  + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 *- Links for SAMB11-
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
#include "ble_manager.h"
#include "ancs_profile.h"
#include "ancs_app.h"
#include "at_ble_errno.h"
#include "at_ble_trace.h"

static const ble_gap_event_cb_t app_gap_handle = {
	.connected = app_connected_event_handler,
	.disconnected = app_disconnected_event_handler
};

/**
* @brief app_connected_state blemanager notifies the application about state
* @param[in] at_ble_connected_t
*/
static at_ble_status_t app_connected_event_handler(void *params)
{
	ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

/**
 * @brief app_connected_state ble manager notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_disconnected_event_handler(void *params)
{
		anp_client_adv();
		ALL_UNUSED(params);
		return AT_BLE_SUCCESS;
}

int main(void)
{
	/*intialize system driver */
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();
	
	DBG_LOG("ANCS Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initializing the profile */
	anp_client_init(NULL);
	
	/* Starting advertisement */
	anp_client_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&app_gap_handle);
		
	/* Capturing the events  */
	while (1) {
		ble_event_task(BLE_EVENT_TIMEOUT);
	}
	
	return 0;
}
