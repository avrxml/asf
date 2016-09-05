/**
* \file
*
* \brief OTAU reference application (Battery and Device Information Service)
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
#include "battery.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "bas_otau.h"
#include "ble_manager.h"
#include "otau_service.h"
#include "otau_profile.h"
#include "button.h"
#ifdef DEVICE_INFORMATION_SERVICE
#include "device_info.h"
#endif
#include "aon_sleep_timer_basic.h"

/* === GLOBALS ============================================================ */

#define BATTERY_UPDATE_INTERVAL	(1) //1 second
#define BATTERY_MAX_LEVEL		(100)
#define BATTERY_MIN_LEVEL		(0)
	
uint8_t db_mem[1024] = {0};
bat_gatt_service_handler_t bas_service_handler;

#ifdef DEVICE_INFORMATION_SERVICE
dis_gatt_service_handler_t dis_service_handler;
#endif

bool volatile timer_cb_done = false;
bool volatile flag = true;
bool volatile battery_flag = true;
at_ble_handle_t bat_connection_handle;
volatile bool otau_paused = false;
volatile bool button_pressed = false;

void ulp_to_active_resume_cb(void);

static void otau_image_nofification_handler (firmware_version_t *new_firmware_ver,
											firmware_version_t *old_firmware_ver, 
											bool *permission);
static void otau_image_switch_handler (firmware_version_t *fw_version, bool *permission);
static void otau_progress_handler (uint8_t section_id, uint8_t completed);

/**
 * \Timer callback handler called on timer expiry
 */
static void aon_sleep_timer_callback(void)
{
	timer_cb_done = true;
	send_plf_int_msg_ind(USER_TIMER_CALLBACK, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
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
		timer_cb_done = false;
		aon_sleep_timer_service_init(1);
		aon_sleep_timer_service_start(aon_sleep_timer_callback);
	}
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack */
static at_ble_status_t ble_encryption_status_changed_app_event(void *param)
{
	at_ble_encryption_status_changed_t *encryption_status_changed = (at_ble_encryption_status_changed_t *)param;
	if(encryption_status_changed->status == AT_BLE_SUCCESS)
	{
		timer_cb_done = false;
		aon_sleep_timer_service_init(1);
		aon_sleep_timer_service_start(aon_sleep_timer_callback);
	}
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	timer_cb_done = false;
	flag = true;
	aon_sleep_timer_service_stop();
	battery_service_advertise();
	
	#if OTAU_FEATURE
		otau_paused = false;
	#endif
	
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
		flag = true;
		DBG_LOG_DEV("sending notification to the peer success");				
	}
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	at_ble_characteristic_changed_t *char_handle = (at_ble_characteristic_changed_t *)param;
	return bat_char_changed_event(char_handle->conn_handle,&bas_service_handler, char_handle, &flag);
}

static const ble_event_callback_t battery_app_gap_cb[] = {
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
	ble_encryption_status_changed_app_event,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t battery_app_gatt_server_cb[] = {
	ble_notification_confirmed_app_event,
	NULL,
	ble_char_changed_app_event,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

void ulp_to_active_resume_cb(void)
{
	init_port_list();
	serial_console_init();
	#if OTAU_FEATURE
	if(ble_get_ulp_status() == BLE_ULP_MODE_SET)
	{
		otau_restore_from_sleep(NULL);
	}
	#endif		
}

#if OTAU_FEATURE


/* OTAU Download Progress Percentage to application */
static void otau_progress_handler (uint8_t section_id, uint8_t completed)
{
	DBG_LOG("[OTAU] Downloading Section:%d, Completed:%d%%", section_id, completed);
}
	
/* Check application for update */
static void otau_image_nofification_handler (firmware_version_t *new_firmware_ver, 
								firmware_version_t *old_firmware_ver, bool *permission)
{
	if (*permission)
	{
		DBG_LOG("[OTAU] Force image download to Firmware version:%d.%d.%d", new_firmware_ver->major_number,
		new_firmware_ver->minor_number,
		new_firmware_ver->build_number);
	}
	else
	{
		if (new_firmware_ver->major_number > old_firmware_ver->major_number)
		{
			*permission = true;
		}
		else if((new_firmware_ver->major_number == old_firmware_ver->major_number) &&
		(new_firmware_ver->minor_number > old_firmware_ver->minor_number))
		{
			*permission = true;
		}
		else if ((new_firmware_ver->major_number == old_firmware_ver->major_number) &&
		(new_firmware_ver->minor_number == old_firmware_ver->minor_number) &&
		(new_firmware_ver->build_number > old_firmware_ver->build_number))
		{
			*permission = true;
		}
		else
		{
			*permission = false;
			DBG_LOG("[OTAU] Update canceled for Firmware version:%d.%d.%d", new_firmware_ver->major_number,
			new_firmware_ver->minor_number,
			new_firmware_ver->build_number);
		}
		
		if (*permission)
		{
			DBG_LOG("[OTAU] Upgrading to New Firmware version:%d.%d.%d", new_firmware_ver->major_number,
			new_firmware_ver->minor_number,
			new_firmware_ver->build_number);
		}
	}	
}
	
/* Check the OTAU image switch permission */
static void otau_image_switch_handler (firmware_version_t *fw_version, bool *permission)
{
	*permission = true;
	DBG_LOG("[OTAU] Switching to New Firmware version:%d.%d.%d", fw_version->major_number,
																 fw_version->minor_number,
																 fw_version->build_number);
}

#endif /* OTAU_FEATURE */


/**
* \Battery Service Application main function
*/
int main(void)
{
	at_ble_status_t status;
	uint8_t battery_level = BATTERY_MIN_LEVEL;
	otau_profile_config_t otau_config;
	
	platform_driver_init();
	acquire_sleep_lock();


	/* Initialize serial console */
	serial_console_init();

	
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
	
#if OTAU_FEATURE
		/* set permission for OTAU Indication Characteristics */
		otau_config.service_config.char_endpoint_permission[OTAU_INDICATION_CHAR_IDX] = \
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;
		/* set permission for OTAU Write Characteristics */
		otau_config.service_config.char_endpoint_permission[OTAU_WRITE_CHAR_IDX] = \
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;
		/* set permission for OTAU Read Characteristics */
		otau_config.service_config.char_endpoint_permission[OTAU_READ_CHAR_IDX] = \
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;
		/* Set OTAU service needs to be registered as Primary or Secondary service */
		otau_config.service_config.service_type = PRIMARY_SERVICE;
		
		/* OTAU image switch permission to OTAU Profile */
		otau_config.app_cb.otau_image_switch = otau_image_switch_handler;
		
		/* OTAU image download progress from OTAU Profile */
		otau_config.app_cb.otau_progress_cb = otau_progress_handler;
		
		/* OTAU image upgrade permission to OTAU Profile */
		otau_config.app_cb.otau_image_notification = otau_image_nofification_handler;
		
		/* OTAU Profile initialization */
		otau_profile_init(&otau_config);
		
#endif /* OTAU_FEATURE */

#ifdef DEVICE_INFORMATION_SERVICE
	/* Initialize the dis */
	dis_init_service(&dis_service_handler);
	
	/* Define the primary service in the GATT server database */
	if ((status = dis_primary_service_define(&dis_service_handler)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Device Information Service definition failed,reason %x",status);
	}
#endif
	
	battery_service_advertise();
	
	/* Register callbacks for gap related events */ 
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									battery_app_gap_cb);
									
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									battery_app_gatt_server_cb);
	
	register_resume_callback(ulp_to_active_resume_cb);
	
	ble_set_ulp_mode(BLE_ULP_MODE_SET);
	
	/* Capturing the events  */ 
	while (1) {
		/* BLE Event Task */
		ble_event_task(BLE_EVENT_TIMEOUT);		
	#if OTAU_FEATURE
		if (button_pressed)
		{
			if (otau_paused)
			{
				if(otau_resume_update_process(NULL) == AT_BLE_SUCCESS)
				{
					DBG_LOG("OTAU Process Resumed...!!!");
				}
				otau_paused = false;
			}
			else
			{
				if(otau_pause_update_process(NULL) == AT_BLE_SUCCESS)
				{
					DBG_LOG("OTAU Process Paused...!!!");
				}
				otau_paused = true;
			}
			button_pressed = false;
		}
	 #endif /* OTAU_FEATURE */
	 
		if (timer_cb_done)
		{
			timer_cb_done = false;			
			/* send the notification and Update the battery level  */			
			if(flag){
				if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, &flag) == AT_BLE_SUCCESS)
				{
					DBG_LOG("Battery Level:%d%%", battery_level);
				}
				if(battery_level == BATTERY_MAX_LEVEL)
				{
					battery_flag = false;
				}
				else if(battery_level == BATTERY_MIN_LEVEL)
				{
					battery_flag = true;
				}
				if(battery_flag)
				{
					battery_level++;
				}
				else
				{
					battery_level--;
				}
			}
		}
	}	
}