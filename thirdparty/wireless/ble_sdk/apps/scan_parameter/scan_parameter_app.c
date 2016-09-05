/**
* \file
*
* \brief Scan parameters Service - Application
*
* Copyright (c) 2016 Atmel Corporation. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
#include "conf_extint.h"
#include "scan_param.h"
#include "ble_manager.h"
#include "scan_parameter_app.h"

bool app_exec  = true;
/* === GLOBALS ============================================================ */

#define SCAN_PRAM_UPDATE_INTERVAL	(5) //5 second

sps_gatt_service_handler_t sps_service_handler;
uint16_t scan_interval_window[2];
uint8_t scan_refresh;

bool volatile timer_cb_done = false;
bool volatile flag = true;

uint8_t scan_refresh_value = 0;

/**
* \Timer callback handler called on timer expiry
*/
void timer_callback_handler(void)
{
	//Timer call back
	timer_cb_done = true;
}

/* Advertisement data set and Advertisement start */
static at_ble_status_t sps_service_advertise(void)
{
	at_ble_status_t status;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return status;
	}
	
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE,
						 NULL, AT_BLE_ADV_FP_ANY, APP_SCP_FAST_ADV, 
						 APP_SCP_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS) { 
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else {
		DBG_LOG("BLE Adv start Failed");
	}
	return status;
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	timer_cb_done = true;
	hw_timer_start(SCAN_PRAM_UPDATE_INTERVAL);
	LED_On(LED0);
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	timer_cb_done = false;
	hw_timer_stop();
	sps_service_advertise();
	LED_Off(LED0);
    ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CONNECTED event from stack */
static at_ble_status_t ble_connected_app_event(void *param)
{
	#if !BLE_PAIR_ENABLE
		ble_paired_app_event(param);
	#else
		ALL_UNUSED(param);
	#endif
	return AT_BLE_SUCCESS;
}

/**
* \Service Characteristic change handler function
*/
static at_ble_status_t sps_char_changed_cb(void *param)
{
	at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;
	return sps_char_changed_event(&sps_service_handler, char_handle, &flag);
}

/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t sps_notification_confirmed_cb(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(!notification_status->status)
	{
		flag = true;
		DBG_LOG_DEV("sending notification to the peer success");
	}
	return AT_BLE_SUCCESS;
}

void button_cb(void)
{
	/* For user usage */
}

static const ble_event_callback_t ble_scan_param_app_gap_cb[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ble_connected_app_event,
	ble_disconnected_app_event,
	NULL,
	NULL,
	ble_paired_app_event,
	NULL,
	NULL,
	NULL,
	NULL,
	ble_paired_app_event,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t ble_scan_param_app_gatt_server_cb[] = {
	sps_notification_confirmed_cb,
	NULL,
	sps_char_changed_cb,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/**
* \Scan Parameter Application main function
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
	
	/* Initialize the button */
	button_init();
	
	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Scan Parameter Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initialize the scan parameter service */
	sps_init_service(&sps_service_handler, scan_interval_window, &scan_refresh);
	
	/* Define the primary service in the GATT server database */
	sps_primary_service_define(&sps_service_handler);
	
	sps_service_advertise();
	
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									ble_scan_param_app_gap_cb);
	
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									ble_scan_param_app_gatt_server_cb);
	
	/* Capturing the events  */ 
	while (app_exec) {
		/* BLE Event Task */
		ble_event_task();
		if (timer_cb_done)
		{
			timer_cb_done = false;
			LED_Toggle(LED0);
			/* send the notification and Update the scan parameter */	
			
			if(sps_scan_refresh_char_update(&sps_service_handler, scan_refresh_value, &flag) == AT_BLE_SUCCESS)
			{
				DBG_LOG("Scan Refresh Characteristic Value: %d", scan_refresh_value);
			}
		}
	}	
	return 0;
}


