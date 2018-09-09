/**
 * \file
 *
 * \brief Find Me Profile Application
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
 * \mainpage FIND ME Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The FIND-ME Application describes the bring-up of Bluetooth® Find Me Profile and Atmel® ATSAMB11 setup on supported platforms.
 * + The Find Me Profile **(FMP)**  is an example application, which would provide smart alert signaling about SAMB11 device. 
 *   Hence SAMB11 device can be tracked from BLE compatible Android/iPhone by using Atmel SMART Connect application.
 *- Running the Demo -
 * + 1. Build and flash the binary into supported evaluation board.
 * + 2. Open the console using TeraTerm or any serial port monitor.
 * + 3. Press the Reset button.
 * + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 * + 5. The device is now in advertising mode.
 * + 6. On a BLE compatible iPhone®/Android phone, enable Bluetooth in the Settings page. Start the Atmel Smart Connect App and scan for devices. 
 *      ATMEL-FMP will be appear amongst the devices scanned. Click on ATMEL-FMP to connect to supported platform.
 * + 7. Once connected, the client side will request for the pairing procedure with the iPhone/Android phone. The console log provides a guidance for the user to 
 *       enter the pass-key on iPhone/Android phone.
 * + 8. Upon completion of connection establishment, User can see the notifications based on the alert level settings .
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
 *- BLE Profile -
 *  + The Find Me profile defines the behaviour when a button is pressed on a device to cause an immediate alert on a peer device. 
 *    This can be used to allow users to find devices that have been misplaced.
 *  + Find Me profile supports two services:
 *  + **Immediate Alert service**: 
 *    + The Immediate Alert service is instantiated as a Primary Service. There is only one instance of the Immediate Alert service on a device. 
 *      There is only one instance of the Alert Level characteristic in an Immediate Alert service. This alert continues until one of following conditions occurs: 
 *    + An implementation specific time out 
 *    + User interaction on this device 
 *    + A new alert level is written 
 *    + The physical link is disconnected
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>*/

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/
#include "asf.h"
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "led.h"
#include "immediate_alert.h"
#include "find_me_app.h"
#include "find_me_target.h"


/* === GLOBALS ============================================================ */

/** @brief Timer interval variable for application task */
static uint8_t timer_interval = INIT_TIMER_INTERVAL;

/**
 * \brief Timer callback handler
 */
static void timer_callback_handler(void)
{
	
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/**
 * @brief Alerting function on immediate alert
 *
 * @param[in] alert level of alert level characteristic of immediate alert
 * service
 *
 */
static void app_immediate_alert(uint8_t alert_val)
{
	if (alert_val == IAS_HIGH_ALERT) {
		DBG_LOG("Find Me : High Alert");
		LED_On(LED0);
		timer_interval = LED_FAST_INTERVAL;
		hw_timer_start(timer_interval);
	} else if (alert_val == IAS_MID_ALERT) {
		DBG_LOG("Find Me : Mild Alert");
		LED_On(LED0);
		timer_interval = LED_MILD_INTERVAL;
		hw_timer_start(timer_interval);
	} else if (alert_val == IAS_NO_ALERT) {
		DBG_LOG("Find Me : No Alert");
		hw_timer_stop();
		LED_Off(LED0);
	}
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	
	/* Wait for some time */
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		/* Stop the timer */
		hw_timer_stop();
		LED_Toggle(LED0);
		hw_timer_start(timer_interval);
	}
	else if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
	
	}
	return status;
}

/* Custom events like user defined event, platform event callback handlers for Find Me profile */
static const ble_custom_event_cb_t findme_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

/**
 * \brief Find Me Application main function
 */
int main(void)
{
	timer_interval = INIT_TIMER_INTERVAL;

	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();

	/* Initialize the hardware timer */
	hw_timer_init();

	led_init();

	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);

	DBG_LOG("Initializing Find Me Application");

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	fmp_target_init(NULL);

	/* callback registration for immediate alert value*/
	register_find_me_handler(app_immediate_alert);

	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&findme_app_custom_event_handle);

	/* Capturing the events  */
	while (1) {
		/* BLE Event Task */
		ble_event_task(0XFFFFFFFF);
	}
}
