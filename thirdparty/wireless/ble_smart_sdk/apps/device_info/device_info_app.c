/**
* \file
*
* \brief Device Information Service - Application
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
/* #include "conf_extint.h" */
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "device_info.h"
#include "device_info_app.h"
#include "timer_hw.h"
#include "ble_utils.h"

#include "ble_manager.h"
#include "button.h"
#include "led.h"

#define APP_STACK_SIZE  (1024)

volatile unsigned char app_stack_patch[APP_STACK_SIZE];

/* === GLOBALS ============================================================ */
bool volatile timer_cb_done = false; 
uint8_t fw_version[10];
at_ble_handle_t dis_conn_handle;
dis_gatt_service_handler_t dis_service_handler;
/* To keep the applicaiton in execution continuosly*/
bool app_exec = true;

/**
* \Timer callback handler called on timer expiry
*/

static void timer_callback_handler(void)
{
	//Timer call back
	timer_cb_done = true;

	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* Advertisement data set and Advertisement start */
static at_ble_status_t device_information_advertise(void)
{	
	at_ble_status_t status = AT_BLE_FAILURE;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return status;
	}
	
	/* Start of advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_DIS_FAST_ADV, APP_DIS_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE Adv start Failed status :%d",status);
	}
	return status;
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	at_ble_pair_done_t *at_ble_pair_done = (at_ble_pair_done_t *)param;
	LED_On(LED0);
	hw_timer_start(FIRMWARE_UPDATE_INTERVAL);
	dis_conn_handle = at_ble_pair_done->handle;
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	hw_timer_stop();
	timer_cb_done = false;
	LED_Off(LED0);
	device_information_advertise();
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

static void button_cb(void)
{
	/* For user usage */
	DBG_LOG("button_cb");
}

static const ble_event_callback_t device_info_app_gap_cb[] = {
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
 
/**
* \Device Information Service Application main function
*/
int main(void)
{
	at_ble_status_t status;

	platform_driver_init();
	acquire_sleep_lock();

	/* initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* initialize the LED */
	led_init();

	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Device Information Service Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);



	/* Initialize the dis */
	dis_init_service(&dis_service_handler);
	
	/* Define the primary service in the GATT server database */
	if ((status = dis_primary_service_define(&dis_service_handler)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Device Information Service definition failed,reason %x",status);
	}
	
	device_information_advertise();
	
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									device_info_app_gap_cb);
										
	/* Capturing the events  */ 
	while (app_exec) {
		/* BLE Event Task */
		ble_event_task(BLE_EVENT_TIMEOUT);	
		if (timer_cb_done)
		{
			static uint8_t fw_update;
			dis_info_data fw_info_data;
			timer_cb_done = false;
			strcpy((char *)fw_version, "FW_VER-000");
			fw_update = fw_update %100;				
			fw_version[8] = (fw_update / 10)+'0';
			fw_version[9] = (fw_update % 10)+'0';
			fw_info_data.info_data = (uint8_t *)fw_version;
			fw_info_data.data_len = 10;
			UPDATE_FIRMWARE_REVISION(&dis_service_handler, &fw_info_data, dis_conn_handle);
			fw_update++;
			DBG_LOG("Updating Firmware to ver:%s", fw_version);
			hw_timer_start(FIRMWARE_UPDATE_INTERVAL);
		}
			
	}	
	return 0;
}
