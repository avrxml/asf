/**
* \file
*
* \brief Proximity Monitor Profile Application
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
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
*Support</a>
*/

/**
 * \mainpage PXP monitor application
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 * The PXP-Monitor example application bring-up the Proximity profile defined 
 * by the Bluetooth SIG which would receives the proximity information from Proximity
 * reporter enbled BLE device.
 * The Proximity monitor example application supports the following features:
 *  + Device discovery and disconnection.
 *  + Services and characteristics discovery.
 *  + Services : Link Loss Service, Immediate Alert Service and TX Power Service.
 *  + Setting up path loss and link loss.
 *  + RSSI sampling.
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
 *  + 5. Initializes the proximity monitor applications.
 *  + 6. Device will start scan and will displays the list of all BLE devices which
 *       are advertising. Proximity reporter devices(GATT server role) are indicated
 *       with tag "--PXP".Select the appropriate index number of the proximity reporter
 *       The Proximity Monitor will then connect to the selected peer device.
 *  + 7. Once connection is established the PXP-Monitor will set link loss alert value
 *       to high alert and starts monitoring the RSSI value of the reporter device.
 *       The proximity reporter supports also ‘Immediate Alert Service’ and
 *      ‘Tx-Power’ service. The Atmel Proximity Reporter application referred in 
 *       this example supports both these optional services.
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
 * - BLE Profile-
 *  + The Proximity profile defines the behavior when a device moves away from a 
 *    peer device so that the connection is dropped or the path loss increases 
 *    above a preset level, causing an immediate alert. This alert can be used 
 *     to notify the user about disconnection of the devices from each other. As a consequence
 *    of this alert, a device may take further action, for example to lock one of
 *    the devices so that it is no longer usable.
 *	+ The Proximity monitor - shall be a GATT client.
 *
 *    The Proximity monitor Profile supports three services:
 *    + Link Loss service -
 *      + The Link Loss service defines behavior when a link is lost between two devices.
 *    + Immediate Alert service -
 *      + The Immediate Alert service defines behavior when a link is lost between two devices.
 *    + Tx Power service -
 *      + The Tx Power service exposes a device’s current transmit power level when in a connection.
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
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "platform.h"
#include "console_serial.h"
#include "pxp_monitor_app.h"

#if defined IMMEDIATE_ALERT_SERVICE
#include "immediate_alert.h"
#endif

#if defined TX_POWER_SERVICE
#include "tx_power.h"
#endif

#if defined LINK_LOSS_SERVICE
#include "link_loss.h"
#endif

#if defined PROXIMITY_REPORTER
#include "pxp_reporter.h"
#endif

at_ble_addr_t peer_addr
= {AT_BLE_ADDRESS_PUBLIC, {0x03, 0x18, 0xf0, 0x05, 0xf0, 0xf8}};

extern gatt_txps_char_handler_t txps_handle;
extern gatt_lls_char_handler_t lls_handle;
extern gatt_ias_char_handler_t ias_handle;

extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];
extern uint8_t pxp_connect_request_flag;

volatile bool app_timer_done = false;
pxp_current_alert_t alert_level = PXP_NO_ALERT;

#define APP_INVALID_EVENT_ID	(0)
#define APP_TIMER_EVENT_ID		(1)
#define APP_BUTTON_EVENT_ID		(2)

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

volatile bool button_pressed = false;
/* Flag to avoid spurious interrupt posting  during reset and initialization */
volatile bool app_init_done = false;

void button_cb(void)
{	
	if(app_init_done)
	{
		button_pressed = true;
		/* Post the custom event */
		at_ble_event_user_defined_post(&app_custom_event[1]);
	}
	
}

/**@brief Check for Link Loss and Path Loss alert
* check for Low Alert value if crossed write Low Alert value to Immediate Alert
*Service. High Alert value if crossed write High Alert value to IAS service
*
* @param[in] conn_handle Connection handle of a connected device
*/
#ifndef ENABLE_PTS
static void rssi_update(at_ble_handle_t conn_handle)
{
	int8_t rssi_power = 0;
	at_ble_status_t status;
	app_timer_done = false;

	/* Get the Received signal strength intensity of the connect
	*device/handle*/
	if ((status = at_ble_rx_power_get(conn_handle,&rssi_power)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("at_ble_rx_power_get failed,reason %d",status);
	}

	DBG_LOG("Rx Power(RSSI):%04d dBm", rssi_power);

	/* if received rssi is above no alert zone and below high alert zone */
	if ((rssi_power <= PXP_LOW_ALERT_RANGE) &&
			(rssi_power >= PXP_HIGH_ALERT_RANGE)) {
		if (!(alert_level == PXP_MID_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_MID_ALERT);
			alert_level = PXP_MID_ALERT;
		}

		DBG_LOG_CONT("---Mild Alert!");
		LED_Toggle(LED0);
	}
	/* if received rssi is above mild alert zone */
	else if (rssi_power < PXP_HIGH_ALERT_RANGE) {
		if (!(alert_level == PXP_HIGH_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_HIGH_ALERT);
			alert_level = PXP_HIGH_ALERT;
		}
		DBG_LOG_CONT("---High Alert!!!");
		LED_On(LED0);
	}
	/* if received rssi is below mild alert zone */
	else {
		if (!(alert_level == PXP_NO_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_NO_ALERT);
			alert_level = PXP_NO_ALERT;
			LED_Off(LED0);
		}
		DBG_LOG_CONT("---No Alert");
	}
}
#endif

/**@brief Proximity Application initialization
* start the device scanning process
*/
static void pxp_app_init(void)
{
	at_ble_status_t scan_status;

	/* Initialize the scanning procedure */
	scan_status = gap_dev_scan();

	/* Check for scan status */
	if (scan_status == AT_BLE_INVALID_PARAM) {
		DBG_LOG("Scan parameters are invalid");
	} else if (scan_status == AT_BLE_FAILURE) {
		DBG_LOG("Scanning Failed Generic error");
	}
}

/* @brief timer call back for rssi update
* enable the flags to execute the application task
*
*/
static void timer_callback_handler(void)
{
	/* Stop the timer */
	hw_timer_stop();
	
	/* Enable the flag the serve the task */
	app_timer_done = true;
	
	/* Post the custom event */
	at_ble_event_user_defined_post(&app_custom_event[0]);
}

static void pxp_monitor_app_timer_event(void)
{
	#ifndef ENABLE_PTS
	/* Application timer event */
	if (app_timer_done) {
		if (pxp_connect_request_flag == PXP_DEV_CONNECTED) {
			rssi_update(ble_dev_info[0].conn_info.handle);
			hw_timer_start(PXP_RSSI_UPDATE_INTERVAL);
		}
	
		app_timer_done = false;
	}
	#endif
}

static void pxp_monitor_app_button_event(void)
{
		#ifdef ENABLE_PTS
			if (button_pressed) {
				DBG_LOG("Press 1 for service discovery");
				DBG_LOG("Press 2 for encryption start");
				DBG_LOG("Press 3 for send MID ALERT on Immediate Alert Service");
				DBG_LOG("Press 4 for send HIGH ALERT on Immediate Alert Service");
				DBG_LOG("Press 5 for send NO ALERT on Immediate Alert Service");
				DBG_LOG("Press 6 for send MILD ALERT on Link Loss Service");
				DBG_LOG("Press 7 for send HIGH ALERT on Link Loss Service");
				DBG_LOG("Press 8 for send NO ALERT on Link Loss Service");
				DBG_LOG("Press 9 for disconnection");
				uint8_t option = getchar();
				switch (option) {
				case '1':
					pxp_monitor_service_discover(ble_dev_info[0].conn_info.handle);
					break;
				case '2':
					at_ble_encryption_start(ble_dev_info[0].conn_info.handle,
								&ble_dev_info[0].bond_info.peer_ltk,
								ble_dev_info[0].bond_info.auth);
					break;
				case '3':
					ias_alert_level_write(ble_dev_info[0].conn_info.handle,
								ias_handle.char_handle,
								IAS_MID_ALERT);
					break;
				case '4':
					ias_alert_level_write(ble_dev_info[0].conn_info.handle,
								ias_handle.char_handle,
								IAS_HIGH_ALERT);
					break;
				case '5':
					ias_alert_level_write(ble_dev_info[0].conn_info.handle,
								ias_handle.char_handle,
								IAS_NO_ALERT);
					break;
				case '6':
					lls_alert_level_write(ble_dev_info[0].conn_info.handle,
								lls_handle.char_handle,
								LLS_MILD_ALERT);
					break;
				case '7':
					lls_alert_level_write(ble_dev_info[0].conn_info.handle,
								lls_handle.char_handle,
								LLS_HIGH_ALERT);
					break;
				case '8':
					lls_alert_level_write(ble_dev_info[0].conn_info.handle,
								lls_handle.char_handle,
								LLS_NO_ALERT);
					break;
				case '9':
					at_ble_disconnect(ble_dev_info[0].conn_info.handle, AT_BLE_TERMINATED_BY_USER);
					break;
				}
				button_pressed = false;
			}
		#else		
			if (button_pressed)
			{
				at_ble_disconnect(ble_dev_info[0].conn_info.handle, AT_BLE_TERMINATED_BY_USER);
				button_pressed = false;
				app_timer_done = false;
			}
		#endif
}

static at_ble_status_t pxp_monitor_app_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	user_custom_event_t **pxp_app_custom_event = (user_custom_event_t **)param;
	if ((*pxp_app_custom_event)->id == APP_TIMER_EVENT_ID)
	{
		pxp_monitor_app_timer_event();
	} 
	else if((*pxp_app_custom_event)->id == APP_BUTTON_EVENT_ID)
	{
		pxp_monitor_app_button_event();
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;	
}

/* All PxP Monitor Custom Event callback */
static const ble_custom_event_cb_t pxp_monitor_custom_event_cb = {
	.custom_event = pxp_monitor_app_custom_event
};

int main(void)
{	
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif

	button_init();
	
	/* Initialize serial console */
	serial_console_init();

	/* Initialize the hardware timer */
	hw_timer_init();

	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	register_hw_timer_start_func_cb(hw_timer_start);
	register_hw_timer_stop_func_cb(hw_timer_stop);

	/* initialize the BLE chip  and Set the device mac address */
	ble_device_init(NULL);
	/* Set the Flag to accept the valid application interrupts */
	app_init_done = true;
	
	pxp_monitor_init(NULL);

	DBG_LOG("Initializing Proximity Monitor Application");

	/* Initialize the pxp service */
	pxp_app_init();
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&pxp_monitor_custom_event_cb);
	

	while (1) {
		/* BLE Event Task */
		ble_event_task();
	}
}
