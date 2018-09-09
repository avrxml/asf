/**
* \file
*
* \brief Proximity Reporter Profile Application
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
 * \mainpage PXP reporter application
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 *    The PXP-Reporter example application bring-up the Proximity profile defined 
 *    by the Bluetooth SIG which would send the proximity information to Proximity
 *    Monitor enbled BLE device.
 *    The Proximity Reporter example application supports the following features:
 * 	+ Advertisement.
 * 	+ Pairing / Bonding.
 * 	+ Services: Link Loss Service, Immediate Alert Service and TX Power Service.
 *
 *    The Proximity Reporter example application supports the following characteristics:
 *  + Immediate Alert Level.
 *  + Link Loss Alert Level.
 *  + Transmit Power Level.
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
 *  + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings 
 *       page. Start the Atmel Smart Connect App and scan for devices. ATMEL-PXP will 
 *       be appear among the devices scanned. Click on "ATMEL-PXP" to connect to 
 *       supported platform.
 *  + 7. Once connected, the client side will request for the pairing procedure . 
 *       The console log provides a guidance for the user to enter the pass-key.
 *  + 8. Once paired, the application displays the PXP reporter service.
 *  + 9. Click on the desired service for configuretion of alert level characteristics
 *  + 10. After configuration of the desired alert levels, click on desired service
 *        and then move the mobile away from reporter. Based on the distance of seperation 
 *        path loss is reported .
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
 *     of this alert, a device may take further action, for example to lock one of
 *     the devices so that it is no longer usable.
 *	+ The Proximity Reporter - shall be a GATT server.
 *
 *    The Proximity Reporter Profile supports three services:
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
 
/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/


#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "immediate_alert.h"
#include "link_loss.h"
#include "tx_power.h"
#include "pxp_reporter.h"
#include "pxp_reporter_app.h"


#define APP_TIMER_EVENT_ID		(1)

/* === GLOBALS ============================================================ */
/* PXP Application LED State */
bool pxp_led_state = true;

/** @brief Timer interval for timer used for led blinking */
uint8_t timer_interval = INIT_TIMER_INTERVAL;

user_custom_event_t app_timer_event =
{
	.id = APP_TIMER_EVENT_ID,
	.bptr = NULL
};

/**
* \brief Timer callback handler called on timer expiry
*/
void timer_callback_handler(void)
{
	hw_timer_stop();
	/* Post timer custom event */
	at_ble_event_user_defined_post(&app_timer_event);
}


/**
 * @brief function called by the profile to notify the app connected
 *
 * @param[in] state true for notification
 *
 */
static void app_connected_state(bool state)
{
	if (state) {
		hw_timer_stop();
		LED_Off(LED0);
		pxp_led_state = 0;
	} 
}

/**
 * @brief Alerting function on link loss alert 
 *
 * @param[in] alert level of alert level characteristic of linkloss service
 *
 */
static void app_pathloss_alert(uint8_t alert_val)
{
	
		if (alert_val == IAS_HIGH_ALERT) {
			DBG_LOG("Pathloss : High Alert");
			timer_interval = PL_INTERVAL_FAST;
			LED_On(LED0);
			hw_timer_start(timer_interval);
		} else if (alert_val == IAS_MID_ALERT) {
			DBG_LOG("Pathloss : Mild Alert");
			timer_interval = PL_INTERVAL_MEDIUM;
			LED_On(LED0);
			hw_timer_start(timer_interval);
		} else if (alert_val == IAS_NO_ALERT) {
			DBG_LOG("Pathloss : No Alert");
			hw_timer_stop();
			LED_Off(LED0);
			pxp_led_state = 0;
		} 
}

/**
 * @brief Alerting function on link loss alert 
 *
 * @param[in] alert level of alert level characteristic of linkloss service
 *
 */
static void app_linkloss_alert(uint8_t alert_val)
{
	if (alert_val == LLS_NO_ALERT) {
		DBG_LOG("Link loss : No Alert  ");
		hw_timer_stop();
		LED_Off(LED0);
		pxp_led_state = 0;
	} else if (alert_val == LLS_MILD_ALERT) {
		DBG_LOG("Link loss : Mild Alert  ");
		timer_interval = LL_INTERVAL_MEDIUM;
		LED_On(LED0);
		hw_timer_start(timer_interval);
	} else if (alert_val == LLS_HIGH_ALERT) {
		DBG_LOG("Link loss : High Alert ");
		timer_interval = LL_INTERVAL_FAST;
		LED_On(LED0);
		hw_timer_start(timer_interval);
	}	
}

static at_ble_status_t pxp_reporter_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	user_custom_event_t **app_timer_custom_event = (user_custom_event_t **)param;
	
	/* timer interrupt event for pxp reporter application */
	if ( ((*app_timer_custom_event)->id) == APP_TIMER_EVENT_ID)
	{
		if (pxp_led_state) {
			pxp_led_state = false;
			LED_Off(LED0);
			hw_timer_start(timer_interval);
			} else {
			pxp_led_state = true;
			LED_On(LED0);
			hw_timer_start(timer_interval);
		}
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;	
}

/* PxP Reporter Application Custom Event callback */
static const ble_custom_event_cb_t pxp_reporter_custom_event_cb = {
	.custom_event = pxp_reporter_custom_event
};

/**
* \brief Proximity Reporter Application main function
*/
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
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Proximity Reporter Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	pxp_reporter_init(NULL);
	
	DBG_LOG("Proximity Reporter Initializing Completed");
	
	register_pathloss_handler(app_pathloss_alert);
	
	register_linkloss_handler(app_linkloss_alert);
	
	register_state_handler(app_connected_state);
	
	/* Register callbacks for application custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&pxp_reporter_custom_event_cb);
		
	/* Capturing the events  */ 
	while(1)
	{
		ble_event_task();
	}
	
	return 0;
}
