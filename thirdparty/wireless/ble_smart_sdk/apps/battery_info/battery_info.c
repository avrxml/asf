/**
* \file
*
* \brief Battery Information Service - Application
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
 * \mainpage BATTERY INFO Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The Battery Application is an example application that bring-up Bluetooth Battery Service. 
 * + Battery service provides the capability to report battery level of the device using the battery characteristics. 
 * + Any application discovering the database can access the Battery service instance during discovery services. 
 * + For the purpose of demonstration, the example simulates battery level of the device. The battery level will be 
 *   increased by 1% for every 100ms and decreased by 1% every 1 second. The range of battery level is from 0% to 100%.
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
#include "aon_sleep_timer_basic.h"
#include "gpio.h"
#include "button.h"
#include "samb11_delay.h"

#define _AON_TIMER_

/* === GLOBALS ============================================================ */
#define BATTERY_UPDATE_INTERVAL	(1) //1 second
#define BATTERY_MAX_LEVEL		(100)
#define BATTERY_MIN_LEVEL		(0)
	
uint8_t db_mem[1024] = {0};
bat_gatt_service_handler_t bas_service_handler;
uint8_t battery_level = BATTERY_MIN_LEVEL;

bool volatile bas_notification_enabled = true;
bool volatile battery_level_notify = true;
at_ble_handle_t bat_connection_handle;
bool volatile battery_charging_status = true;

#define MAX_CONNECTION_PARAM	(2)
volatile uint8_t conn_param_idx = 0;
volatile bool conn_param_update_progress = false;
extern bool button_debounce;

static at_ble_status_t bas_connection_param_update(uint8_t idx);

static at_ble_connection_params_t connection_param_table[MAX_CONNECTION_PARAM] = \
{
	{
		.ce_len_max = 0,
		.ce_len_min = 0,
		.con_intv_max = 50,
		.con_intv_min = 24,
		.con_latency = 0,
		.superv_to = 50
	},
	{
		.ce_len_max = 0,
		.ce_len_min = 0,
		.con_intv_max = 320,
		.con_intv_min = 304,
		.con_latency = 4,
		.superv_to = 600
	}
};

void platform_resume_cb(void);

/**
 * \Timer callback handler called on timer expiry
 */
static void aon_sleep_timer_callback(void)
{
	//Timer call back
	send_plf_int_msg_ind(RAM_ISR_TABLE_AON_SLEEP_TIMER_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

void button_cb(void);
/**
 * @brief Button Press Callback 
 */
void button_cb(void)
{	
	if (!conn_param_update_progress)	
	{
		send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, AON_PIN_CALLBACK_TYPE_DETECT, NULL, 0);
	}	
}

static at_ble_status_t bas_connection_param_update(uint8_t idx)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	DBG_LOG("Connection Interval Min:%d & Max:%d, Connection latency:%d, Hdl:%d, Supervision Timeout:%d, CE MIN:%d & MAX:%d ",
	connection_param_table[idx].con_intv_min,
	connection_param_table[idx].con_intv_max,
	connection_param_table[idx].con_latency,
	bat_connection_handle,
	connection_param_table[idx].superv_to,
	connection_param_table[idx].ce_len_min,
	connection_param_table[idx].ce_len_max
	);
	
	if (at_ble_connection_param_update(bat_connection_handle, &connection_param_table[idx]) == AT_BLE_SUCCESS)
	{
		DBG_LOG("Connection Parameter Update Request Success");
	}
	else
	{
		status = AT_BLE_FAILURE;
		DBG_LOG("Connection Parameter Update Request Failed");
	}
	return status;
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
		battery_level_notify = true;
		ALL_UNUSED(param);
		aon_sleep_timer_service_init(1);
		aon_sleep_timer_service_start(aon_sleep_timer_callback);
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
		battery_level_notify = true;
		ALL_UNUSED(param);
		aon_sleep_timer_service_init(1);
		aon_sleep_timer_service_start(aon_sleep_timer_callback);
		return encryption_status_changed->status;
	}
	return encryption_status_changed->status;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	bas_notification_enabled = false;
	battery_level_notify = false;
	aon_sleep_timer_service_stop();
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
		battery_level_notify = true;
		DBG_LOG_DEV("sending notification to the peer success");				
	}
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	at_ble_status_t status;
	at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;
	
	status = bat_char_changed_event(&bas_service_handler, char_handle);
	if (status == AT_BLE_SUCCESS)
	{
		battery_level_notify = true;
		bas_notification_enabled = true;		
	}
	else if (status == AT_BLE_PRF_NTF_DISABLED)
	{
		bas_notification_enabled = false;
	}
	
	return status;	
}

void platform_resume_cb(void)
{
	init_port_list();
	serial_console_init();
	
	#if defined PDS_SERVICE		
		spi_flash_turn_on();
		spi_flash_clock_init();
		spi_flash_init();
	#endif /* defined PDS_SERVICE */
}

static void simulate_battery_level(void)
{
	if(battery_level == BATTERY_MAX_LEVEL)
	{
		battery_charging_status = false;
	}
	else if(battery_level == BATTERY_MIN_LEVEL)
	{
		battery_charging_status = true;
	}
	
	if(battery_charging_status)
	{
		battery_level++;
	}
	else
	{
		battery_level--;
	}
}

static at_ble_status_t bat_platform_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_AON_SLEEP_TIMER_INDEX))
	{
		/* send the notification and Update the battery level  */
		if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, (battery_level_notify & bas_notification_enabled)) == AT_BLE_SUCCESS)
		{
			DBG_LOG("Battery Level:%d%%", battery_level);
			battery_level_notify = false;		
		}
		simulate_battery_level();
	}
	else if (plf_isr_event->event_type == ((AON_PIN_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
		if (!conn_param_update_progress)
		{
			conn_param_update_progress = true;
			conn_param_idx++;
			conn_param_idx %= MAX_CONNECTION_PARAM;
			bas_connection_param_update(conn_param_idx);			
		}
		delay_ms(200);
		button_debounce = true;
	}
	else
	{		
		status = AT_BLE_FAILURE;
		DBG_LOG("Platform ISR Event Type:%d", plf_isr_event->event_type);
	}
	
	return status;
}

/** @brief connection update parameter function */
static at_ble_status_t ble_connparam_update_app_event(void *params)
{
	at_ble_conn_param_update_done_t * conn_param_update;
	conn_param_update = (at_ble_conn_param_update_done_t *)params;
	conn_param_update_progress = false;
	ALL_UNUSED(conn_param_update);  //To avoid compiler warning
	return AT_BLE_SUCCESS;
}

static const ble_gap_event_cb_t battery_app_gap_cb = {
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
	.pair_done = ble_paired_app_event,
	.encryption_status_changed = ble_enc_status_changed_app_event,
	.conn_parameter_update_done = ble_connparam_update_app_event
};


static const ble_gatt_server_event_cb_t battery_app_gatt_server_cb = {
	.notification_confirmed = ble_notification_confirmed_app_event,
	.characteristic_changed = ble_char_changed_app_event
};

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t bat_app_platform_event_handle = {
	.platform_event_ready = bat_platform_event 
};

/**
* \Battery Service Application main function
*/
int main(void)
{
	at_ble_status_t status;
	
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();
	
	/* Initializing the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);
	
	#if defined PDS_SERVICE	
	/* Initialize the PDS Service */
	if(pds_module_init() == AT_BLE_SUCCESS){
		/* If button SW0 is pressed  during startup then the bonding information will be deleted */
		if(button_0_input_level()){
			ble_remove_bonding_info();
		}else{
			DBG_LOG("Info: To Delete Bonding Information follow the Steps: ");
			DBG_LOG("\t1) Press and hold the reset button and SW0 button");
			DBG_LOG("\t2) Release Reset Button and wait for 5 seconds");
			DBG_LOG("\t3) Release SW0 Button");
		}
	}
	#endif /* defined PDS_SERVICE */

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
									
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&bat_app_platform_event_handle);
									
	register_resume_callback(platform_resume_cb);
	#if (BLE_MODULE == SAMB11_ZR)	
		ble_set_ulp_mode(BLE_ULP_MODE_SET);
	#elif (BLE_MODULE == SAMB11_MR)
		ble_set_ulp_mode(BLE_ULP_MODE_CLEAR);
	#endif	
	
	/* Capturing the events  */ 
	while (1) {
		/* BLE Event Task */
		ble_event_task(0xFFFFFFFF);
	}	
}


