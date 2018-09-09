/**
 * \file
 *
 * \brief Eddystone Beacon Application
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
 * \mainpage Eddystone Beacon Application
 * \section Introduction
 * **************************** Introduction *********************************
 * Eddystone beacon application demonstrates the Google's Eddystone proximity
 * beacon messages on the Atmel BLE device. It is completely complaint with
 * the Eddystone specification provided in the link https://github.com/google/eddystone
 * and supports all the three Eddystone frames (UID, URL and TLM).
 *- Running the Demo -
 *  + 1. Build and flash the binary into one of the supported evaluation board.
 *  + 2. Open the EDBG COM port using TeraTerm or any serial port monitor application in the host PC.
 *  + 3. Press the Reset button on the board to restart the firmware.
 *  + 4. Wait for around 10 seconds for the SAMB11 to be initialized with patches etc.
 *  + 5. The device will now start sending Eddystone beacons (URL or UID based on the APP_TYPE define).
 *  + 6. Use a BLE compatible Android or iPhone® mobile device to run Atmel SmartConnect application.
 *  + 7. Enable Bluetooth from the settings page of the mobile device and open Atmel SmartConnect app.
 *  + 8. Click on the Beacons navigation pane to view the ranging screen. The Eddystone beacon will be scanned and displayed on the ranging screen with an Eddystone icon.
 *  + 9. Click on the Eddystone icon image to view the beacon information in a small pop-up window.  Click on the pop-up window to view detailed information of the beacon.
 *  + 10. In case of Eddystone-URL, long press the SW0 button on the evaluation board to put the beacon device into configuration mode. 
 *  + Click 'Configure' button in the detailed beacon information page of the mobile app to view the Eddystone devices in configuration mode.
 *  + Connect to the listed 'Atmel-Eddystone' device to configure the URL, beacon period, TX power mode etc.
 *  + Disconnect from the beacon device to see the beacon working with updated values.
 *
 * \section Application features
 * ********************** Application features *********************************
 * The application supports two different configuration which is controlled by the 'APP_TYPE'
 * define. Setting 'APP_TYPE' to 'EDDYSTONE_UID_APP' demonstrates the Eddystone-UID frames along with TLM and setting the 'APP_TYPE' to 'EDDYSTONE_URL_APP' demonstrate the Eddystone-URL frames with TLM and the Eddystone-URL configuration service.
 *- EDDYSTONE_UID_APP -
 * In this app configuration, the Eddystone-UID frames are transmitted for every beacon interval
 * and interleaved with TLM frames based on a ratio value(uid_to_tlm_frame_ratio). The UID frame fields like Namspace ID,
 * Instance ID, Calibrated TX Power at 0 meter etc can be configured in the 'conf_eddystone.h' file.
 * There are API functions available to configure these values at runtime too.
 *- EDDYSTONE_URL_APP -
 * + In this app configuration, the Eddystone-URL frames are transmitted for every beacon interval
 * and interleaved with TLM frames based on a ratio value(url_to_tlm_frame_ratio). The URL frame fields
 * like encoded URI data, advertised tx power at 0 meters etc can be configured at compile time in the
 * 'conf_eddystone.h' file and at runtime using the available API functions.
 * + In TLM frame, the beacon temperature value is obtained from the on-board temperature sensor of the evaluation board and the 0.1 seconds counter value from the hardware timer.
 * + The Eddystone-URL configuration service is also supported in this application. 
 * The SW0 button on the evaluation board is used to toggle between configuration mode and beacon mode.
 * The Configuration service provides characteristics to update the URL value, Advertised tx power level and beacon period etc.
 *
 * \section Used Modules
 * ************************* Used Modules ************************************
 * Following modules are used by the Eddystone application in the BluSDK.
*- BLE Manager -  
 *  + The Event Manager handles generic BLE events triggered by BLE stack and also responsible 
 *    for invoking all registered callbacks for respective events. BLE Manager 
 *    handles all GAP related functionality. In addition to that it handles multiple connection 
 *    instances, Pairing, Encryption, Scanning etc.
 *
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
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
 *							        Includes	                                     		            *
****************************************************************************************/
#include <asf.h>
#include <string.h>
#include "console_serial.h"
#include "platform.h"
#include "timer_hw.h"
#include "button.h"
#include "led.h"
#include "eddystone.h"
#include "samb11_delay.h"
/****************************************************************************************
 *							        Macros	                                     		            *
****************************************************************************************/

/** Define to support different Eddystone apps */
#define EDDYSTONE_URL_APP	(0x01)
#define EDDYSTONE_UID_APP	(0x02)

/** Switch to compile two different Eddystone applications; 
 *  One for UID+TLM frames and another for URL+TLM frames, 
 *  the configuration service is enabled for the URL app */
#define APP_TYPE			(EDDYSTONE_URL_APP)
	
/**  Button timeout to detect long press event */
#define BUTTON_LONG_PRESS_INTERVAL_MS	(3000)

/** Application timer timeout in ms */
#define APP_TIMER_TIMEOUT_MS			(100)

/* BLE Event timeout */
#define EVENT_TIMEOUT_MS				(100)


/****************************************************************************************
 *							    Structures	                                   	                   *
****************************************************************************************/

/** TLM data that needs to be updated for every TLM frame */
static eddystone_tlm_data_t tlm_data = 
	{
		.battery_voltage_mV = 3300,				/* 3.3 V (simulated value) */
		.temperature = FLOAT_2_FIX88(35.4),		/* Dummy temperature value */
	};

/** Memory and Platform API configuration information */
at_ble_init_config_t pf_cfg;

/** Button state information */
typedef struct button_state_t
{
	uint8_t pressed;
	uint32_t interval_ms;
	
} button_state_t;

/****************************************************************************************
 *							    Globals	                                   	                   *
****************************************************************************************/
/** Initial button state values */
static volatile button_state_t button_state = {
	.pressed = 0,
	.interval_ms = BUTTON_LONG_PRESS_INTERVAL_MS,
};

/** UID to TLM frame ratio; Change this value to control the frequency of sending TLM frames */
static const uint32_t uid_to_tlm_frame_ratio = 20;

/** URL to TLM frame ratio; Change this value to control the frequency of sending TLM frames */
static const uint32_t url_to_tlm_frame_ratio = 20;

bool app_initialise = false;

#if (APP_TYPE == EDDYSTONE_UID_APP)
	uint8_t beacon_frame_type = EDDYSTONE_UID;
#elif (APP_TYPE == EDDYSTONE_URL_APP)
	uint8_t beacon_frame_type = EDDYSTONE_URL;
#endif

uint32_t loop_counter = 0, adv_pdu_cnt = 0;
extern bool button_debounce;

/****************************************************************************************
 *							    Functions	                                   	                   *
****************************************************************************************/
static void eddystone_app_task(void);

/**@brief  Non-return function to handle fatal application error */
static void app_error(void)
{
	DBG_LOG("Application Error!");
	while(1) {
		
	}
}

/**@brief  Button press callback handler */
static void button_cb(void)
{
	if(!button_state.pressed)
	{
		send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/**@brief Timer callback handler - called every 100ms.
 *  It increments Eddystone 100ms counter and also times-out long press for the button  
 */
static void timer_callback_handler(void)
{
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/**@brief  Initialize the Eddystone application */
static void eddystone_app_init(void)
{
	at_ble_status_t status;
	
	eddystone_init();
	
#if (APP_TYPE == EDDYSTONE_URL_APP)
	/* Initialize and define the primary service in the GATT server database */
	if ((status = esurl_conf_service_init(&esurl_conf_service))!= AT_BLE_SUCCESS)
	{
		DBG_LOG("Eddystone URL configuration service definition failed, reason %x",status);
		app_error();
	}
	
	/* Register callbacks to handle GAP events for configuration service */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&esurl_conf_gap_cb);
	/* Register callbacks to handle GATT server events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									&esurl_conf_gatt_server_cb);
									
	// Start sending Eddystone URL frame
	status = eddystone_url_frame_send();
	
#elif (APP_TYPE == EDDYSTONE_UID_APP)
	status = eddystone_uid_frame_send();
#endif

	if(AT_BLE_SUCCESS != status) app_error();
}

/**@brief  Read temperature in 8:8 fixed point format */
static int16_t read_temperature_fix88(void)
{
	#if SAMD21 || SAML21 || SAMB11
	float temperature;
	/* Read Temperature Value */
	temperature = at30tse_read_temperature();
							
	#endif

	#if SAMG55 || SAM4S
	double temperature;
	/* Read Temperature Value */
	at30tse_read_temperature(&temperature);
	#endif
	
	return FLOAT_2_FIX88(temperature);
}

static void eddystone_app_task(void)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	
	UNUSED(uid_to_tlm_frame_ratio);

	// Advertising mode
	if ((EDDYSTONE_BEACON_MODE == eddystone_get_mode()) && (AT_BLE_SUCCESS == at_ble_get_adv_cnt(&adv_pdu_cnt))) {
		
		if (loop_counter < adv_pdu_cnt) {
			DBG_LOG("Adv count: %u", (unsigned int) adv_pdu_cnt);
			
			switch(beacon_frame_type)
			{
				#if (APP_TYPE == EDDYSTONE_UID_APP)
				case EDDYSTONE_UID:
					// Send Eddystone UID frame
					DBG_LOG("Tx UID");
					status = eddystone_uid_frame_send();
					loop_counter = adv_pdu_cnt + uid_to_tlm_frame_ratio;
					// Next frame to be transmitted
					beacon_frame_type = EDDYSTONE_TLM;
				break;
				
				#elif (APP_TYPE == EDDYSTONE_URL_APP)
				case EDDYSTONE_URL:
					// Send Eddystone URL frame
					DBG_LOG("Tx URL");
					status = eddystone_url_frame_send();
					loop_counter = adv_pdu_cnt + url_to_tlm_frame_ratio;
					// Next frame to be transmitted
					beacon_frame_type = EDDYSTONE_TLM;
					break;
				#endif
				case EDDYSTONE_TLM:
					tlm_data.temperature = read_temperature_fix88();
					// Send Eddystone TLM frame
					DBG_LOG("Tx TLM");
					status = eddystone_tlm_frame_send(&tlm_data);
					loop_counter = adv_pdu_cnt + 1;
					#if (APP_TYPE == EDDYSTONE_UID_APP)
						// Next frame to be transmitted
						beacon_frame_type = EDDYSTONE_UID;
					#elif (APP_TYPE == EDDYSTONE_URL_APP)
						// Next frame to be transmitted
						beacon_frame_type = EDDYSTONE_URL;
					#endif
				break;
				
				default:
				break;

			} // switch
		} // if (loop_counter < adv_pdu_cnt)
	} // if
	status = status;
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	
	/* Wait for some time */
	if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
       		button_state.pressed = true;
			button_state.interval_ms = BUTTON_LONG_PRESS_INTERVAL_MS;
	}
	
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
	   eddystone_100ms_counter++;
	   	   
	   /* Handle button press */
	   if (button_state.pressed) {
		   /* Check if button is still pressed */
		   #if (BLE_MODULE == SAMB11_ZR)
		   if (button_0_input_level() == true)
		   #elif (BLE_MODULE == SAMB11_MR)
		   if (button_0_input_level() == 0)
		   #endif
		   {
			   button_state.interval_ms -= 100;
			   if (button_state.interval_ms < 100) {
				   
				   #if (APP_TYPE == EDDYSTONE_URL_APP)					   
					   // Toggle the mode by starting adv
					   if((EDDYSTONE_BEACON_MODE == eddystone_get_mode()) || (EDDYSTONE_IDLE_MODE == eddystone_get_mode())) {
						   // Change to configuration mode
						   esurl_conf_adv_start(ESURL_CONF_ADV_INTERVAL);
						   DBG_LOG("Eddystone-URL config mode");
						   // Set beacon type to URL to send URL frames first when got back from Config mode
						   beacon_frame_type = EDDYSTONE_URL;
						   } else {
						   // Change to beacon mode
						   eddystone_url_frame_send();
						   DBG_LOG("Eddystone beacon mode");
					   }
				   #endif
				   button_state.pressed = false;
			   }
		   }
		   else
		   {
			   button_state.pressed = false;
		   }
		
		}
		eddystone_app_task();
		button_debounce = true;
	}

	return status;
}

/* Custom events like user defined event, platform event callback handlers for PXP Monitor profile */
static const ble_custom_event_cb_t eddystone_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

/** Application main function */
int main(void)
{
	/* Initialize the platform drivers */
	platform_driver_init();
	acquire_sleep_lock();
	
	/* Initialize the button & LED */
	gpio_init();
	button_init();
	button_register_callback(&button_cb);
	led_init();
	
	/* Initialize serial console */
	serial_console_init();
	
	/*Enable I2C  system clock */
	system_clock_peripheral_enable(PERIPHERAL_I2C0_CORE);

   	/* Initialize the temperature sensor */
	at30tse_init();

	/* Configure the temperature sensor ADC */
	at30tse_write_config_register(AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit));

	/* Initialize the hardware timer */
	hw_timer_init();

	/* Register the timer callback */
	hw_timer_register_callback(timer_callback_handler);

   	/* Initialize the BLE device  and Set the device mac address */
	ble_device_init(NULL);

  	hw_timer_start_ms(APP_TIMER_TIMEOUT_MS);
	  
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&eddystone_app_custom_event_handle);  

	/* Initialize the Eddystone application */
	eddystone_app_init();
    
	app_initialise = true;
	
	DBG_LOG("Eddystone beacon started");

	while(1) {
		/* BLE event task */
		ble_event_task(0xFFFFFFFF);
	} // while(1)
}