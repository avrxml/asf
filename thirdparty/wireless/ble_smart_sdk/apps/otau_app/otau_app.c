/**
* \file
*
* \brief OTAU reference application (Battery and Device Information Service)
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
#include "otau_app.h"
#include "ble_manager.h"
#include "otau_service.h"
#include "otau_profile.h"
#include "button.h"
#include "led.h"
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

bool volatile bas_notification_enabled = false;
bool volatile battery_charging = true;
at_ble_handle_t bat_connection_handle;
volatile bool otau_paused = false;
uint8_t battery_level = BATTERY_MIN_LEVEL;
extern bool button_debounce;

static void bas_simulate_battery_level(void);
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
	send_plf_int_msg_ind(RAM_ISR_TABLE_AON_SLEEP_TIMER_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

static void button_cb(void)
{
	send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, AON_PIN_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* Advertisement data set and start advertisement */
static at_ble_status_t battery_service_advertise(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE advertisement data set failed reason :%d",status);
		return status;
	}
	
	/* Start the advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_BAS_FAST_ADV, APP_BAS_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Advertisement");
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE advertisement start failed reason :%d",status);
	}
	return status;
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	at_ble_pair_done_t *pair_done = (at_ble_pair_done_t *)param;
	if(pair_done->status == AT_BLE_SUCCESS)
	{
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
		aon_sleep_timer_service_init(1);
		aon_sleep_timer_service_start(aon_sleep_timer_callback);
	}
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	bas_notification_enabled = false;
	LED_Off(LED0);
	aon_sleep_timer_service_stop();	
	
	#if OTAU_FEATURE
		otau_paused = false;
	#endif
		
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
	if(notification_status->status == AT_BLE_SUCCESS)
	{
		bas_notification_enabled = true; //enable the notification on notification confirmation event
		LED_Toggle(LED0);
		DBG_LOG_DEV("sending notification to the peer success");				
	}
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	at_ble_status_t status;
	at_ble_characteristic_changed_t *char_changed_param = (at_ble_characteristic_changed_t *)param;
	status = bat_char_changed_event(&bas_service_handler, char_changed_param);
	if (status == AT_BLE_SUCCESS)
	{
		bas_notification_enabled = true;
	} 
	else if(status == AT_BLE_PRF_NTF_DISABLED)
	{
		bas_notification_enabled = false;
	}
	return status;
}

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_AON_SLEEP_TIMER_INDEX))
	{
		/* send the notification and Update the battery level  */
		if(bat_update_char_value(bat_connection_handle,&bas_service_handler, battery_level, bas_notification_enabled) == AT_BLE_SUCCESS)
		{
			bas_notification_enabled = false;//Wait for Notification Confirmation event
			DBG_LOG("Battery Level:%d%%", battery_level);
		}
		bas_simulate_battery_level();
	}
	else if(plf_isr_event->event_type == ((AON_PIN_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
	 #if OTAU_FEATURE
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
	 #endif /* OTAU_FEATURE */
	 button_debounce = true;
	}
	return status; 
}

/* BLE GAP event callback handlers for OTAU profile */
static const ble_gap_event_cb_t otau_app_gap_cb = {
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
	.pair_done = ble_paired_app_event,
	.encryption_status_changed = ble_encryption_status_changed_app_event
};

/* BLE GATT Server event callback handlers for OTAU profile */
static const ble_gatt_server_event_cb_t otau_app_gatt_server_cb = {
	.notification_confirmed = ble_notification_confirmed_app_event,
	.characteristic_changed = ble_char_changed_app_event
};

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t otau_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event 
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

static void bas_simulate_battery_level(void)
{
	if(battery_level == BATTERY_MAX_LEVEL)
	{
		battery_charging = false;
	}
	else if(battery_level == BATTERY_MIN_LEVEL)
	{
		battery_charging = true;
	}
	if(battery_charging)
	{
		battery_level++;
	}
	else
	{
		battery_level--;
	}				
}

/**
* \Battery Service Application main function
*/
int main(void)
{
	at_ble_status_t status;	
	otau_profile_config_t otau_config;
	
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize LED */
	led_init();
	
	/* Initialize button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);
	
	DBG_LOG("Initializing SAMB11 OTAU Profile with Example Application");
	
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
		/* set permission for OTAU data Indication Characteristics */
		otau_config.service_config.char_endpoint_permission[OTAU_INDICATION_CHAR_IDX] = \
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;
		/* set permission for OTAU data Write Characteristics */
		otau_config.service_config.char_endpoint_permission[OTAU_WRITE_CHAR_IDX] = \
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR;
		/* set permission for OTAU data Read Characteristics */
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
									&otau_app_gap_cb);
									
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									&otau_app_gatt_server_cb);
									
	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&otau_app_custom_event_handle);
	
	register_resume_callback(ulp_to_active_resume_cb);
	
	ble_set_ulp_mode(BLE_ULP_MODE_SET);
	
	/* Capturing the events  */ 
	while (1) {
		/* BLE Event Task */
		ble_event_task(0xFFFFFFFF);	
	}	
}