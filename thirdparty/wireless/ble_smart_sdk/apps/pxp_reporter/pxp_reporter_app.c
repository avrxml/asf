/**
* \file
*
* \brief Proximity Reporter Profile Application
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
#include "ble_manager.h"
#include "immediate_alert.h"
#include "link_loss.h"
#include "tx_power.h"
#include "led.h"
#include "pxp_reporter.h"
#include "pxp_reporter_app.h"



/* === GLOBALS ============================================================ */
/* PXP Application LED State */
bool pxp_led_state = true;

/** @brief Timer interval for timer used for led blinking */
uint8_t timer_interval = INIT_TIMER_INTERVAL;

/* To make the app execute*/
bool app_exec = true;

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
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

/**
* \brief Timer callback handler called on timer expiry
*/
void timer_callback_handler(void)
{
	hw_timer_stop();
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t pxp_reporter_sensor_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

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

/**
* \brief Proximity Reporter Application main function
*/
int main(void)
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize LED */
	led_init();
		
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

	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&pxp_reporter_sensor_app_custom_event_handle);
		
	/* Capturing the events  */ 
	while(app_exec)
	{
		ble_event_task(0xFFFFFFFF);
	}
	
	return 0;
}
