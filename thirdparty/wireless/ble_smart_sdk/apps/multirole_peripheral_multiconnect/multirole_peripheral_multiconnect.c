/**
* \file
*
* \brief Multi-Role Peripheral Multi-connection Application
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
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Atmel
*Support</a>
*/

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the Bas MultiPeripheral/ Find Me Central Application
*/
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "timer_hw.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "multirole_peripheral_multiconnect.h"
#include "find_me_locator.h"
#include "button.h"
#include "led.h"
#include "system_sam_b.h"
#include "samb11_delay.h"

#if defined IMMEDIATE_ALERT_SERVICE
#include "immediate_alert.h"
#endif

#if defined BATTERY_SERVICE
#include "battery.h"
#endif

#define APP_INVALID_EVENT_ID	(0)
#define APP_TIMER_EVENT_ID		(1)
#define APP_BUTTON_EVENT_ID		(2)

#define APP_STACK_SIZE			(2048)
volatile unsigned char			app_stack_patch[APP_STACK_SIZE];

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

volatile fmp_scan_state_t fmp_app_scan_state = FMP_IDLE;
bool volatile findme_timer_done = false;

fmp_app_state_t fmp_app_info = {{AT_BLE_ADDRESS_PUBLIC, {0x00,0x00,0x00,0x00,0x00,0x00}}, \
								BLE_INVALID_CONNECTION_HANDLE};

bas_app_state_t bas_app_info[BAS_MAX_DEVICE_CONNECTION] = {{BLE_INVALID_CONNECTION_HANDLE, \
							BAS_NOTIFICATION_DISABLED, false}, };

extern ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTION];
extern volatile uint8_t scan_response_count;
extern at_ble_scan_info_t scan_info[MAX_SCAN_DEVICE];
extern gatt_ias_char_handler_t ias_handle;

bat_gatt_service_handler_t bas_service_handler;
bool volatile bat_timer_done = false;
bool volatile bat_device_adv = false;//LL State
bool volatile battery_level_flag = true;
uint8_t battery_level = BATTERY_MIN_LEVEL;
uint8_t fmp_supp_scan_index[MAX_SCAN_DEVICE];
extern bool button_debounce;

fmp_locator_alert_level_t alert_level = FMP_NO_ALERT;

static at_ble_status_t bas_paired_app_event(void *param);
static at_ble_status_t bas_encryption_status_changed_app_event(void *param);
static at_ble_status_t ble_disconnected_app_event(void *param);
static at_ble_status_t battery_start_advertisement(void);
static at_ble_status_t battery_set_advertisement_data(void);
static at_ble_status_t ble_char_changed_app_event(void *param);
static at_ble_status_t ble_notification_confirmed_app_event(void *param);
static at_ble_status_t battery_simulation_task(void *param);
static at_ble_status_t ble_connected_app_event(void *param);
static at_ble_status_t ble_adv_report_app_event(void *param);
static at_ble_status_t ble_scan_report_app_event(void *param);
static uint16_t app_no_device_found_handler(void);
static at_ble_status_t multirole_app_platform_event(void *param);
static at_ble_status_t bas_send_notification(void *param);
static void button_cb(void);

static const ble_gap_event_cb_t battery_app_gap_cb = {
	.adv_report = ble_adv_report_app_event,
	.scan_report = ble_scan_report_app_event,
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
	.pair_done = bas_paired_app_event,
	.encryption_status_changed = bas_encryption_status_changed_app_event
};

static const ble_gatt_server_event_cb_t battery_app_gatt_server_cb = {
	.notification_confirmed = ble_notification_confirmed_app_event,
	.characteristic_changed = ble_char_changed_app_event
};

/* All Multi-role Peripheral Custom Event callback */
static const ble_custom_event_cb_t multirole_custom_event_cb = {
	.platform_event_ready = multirole_app_platform_event
};

static void button_cb(void)
{
	send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* @brief timer call back for rssi update
* enable the flags to execute the application task
*
*/
static void timer_callback_handler(void)
{
	/* Enable the flag the serve the task */
	bat_timer_done = true;
	findme_timer_done = true;
	
	/* Post the platform event */
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);
}

/* Callback registered for AT_BLE_CONNECTED_DONE event from stack */
static at_ble_status_t ble_connected_app_event(void *param)
{
	at_ble_connected_t *connected = (at_ble_connected_t *)param;
	bat_device_adv = false;
	if (connected->conn_status == AT_BLE_SUCCESS)
	{
		uint8_t idx;
 		volatile uint8_t max_peripheral_dev = 0;
    		for(idx = 0; idx < BLE_MAX_DEVICE_CONNECTION; idx++)
   			{
    			if (ble_check_ispheripheral(ble_dev_info[idx].conn_info.handle))
    			{
    				max_peripheral_dev++;
    			}
    		}
		   max_peripheral_dev = 0;
  		for(idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
  		{
  			if (bas_app_info[idx].conn_handle == BLE_INVALID_CONNECTION_HANDLE)
  			{
				if(!ble_check_iscentral(connected->handle))
				{
					bas_app_info[idx].conn_handle = connected->handle;
					bas_app_info[idx].bas_level_notified = false;
					max_peripheral_dev++;
  					break;
				}
  			}
			else
			{
				 max_peripheral_dev++; 
			}
  		}
  		
		 /* if peripheral device is connected then initiate scanning if 
			there is no active connection for central. and if there 
			is any active find me connection then initiate advertisement */
 		if(ble_check_iscentral(connected->handle))
 		{
			 DBG_LOG("central device connected");
			 fmp_app_scan_state = FMP_CONNECTED;
 				fmp_app_info.fmp_conn_handle = connected->handle;
 				if (max_peripheral_dev < BAS_MAX_DEVICE_CONNECTION)
 				{
 					if(battery_start_advertisement() == AT_BLE_SUCCESS)
 					{
 						bat_device_adv = true;
 					}
 				}
 		}
 		else
 		{
			 if(fmp_app_scan_state == FMP_IDLE)
 			{
 				if(gap_dev_scan() != AT_BLE_SUCCESS)
 				{
 					DBG_LOG("Failed to initiate scanning");
 				}
 				else
 				{
 					fmp_app_scan_state = FMP_SCANNING;
 				}
 			}
 			else
 			{
 				if (max_peripheral_dev < BAS_MAX_DEVICE_CONNECTION)
 				{
 					if(battery_start_advertisement() == AT_BLE_SUCCESS)
 					{
 						bat_device_adv = true;
 					}
 				}
 			}	
 		}	
		
	}	
	return AT_BLE_SUCCESS;
}

/* handling AT_BLE_ADV_REPORT event */
static at_ble_status_t ble_adv_report_app_event(void *param)
{
	at_ble_adv_report_t *adv_report = (at_ble_adv_report_t *)param;
	bat_device_adv = false;
	DBG_LOG("Advertisement stopped due to timeout");
	if(adv_report->status == AT_BLE_GAP_TIMEOUT)
	{
		if(fmp_app_scan_state == FMP_IDLE)
		{
			if(gap_dev_scan() != AT_BLE_SUCCESS)
			{
				DBG_LOG("Failed to initiate scanning");
			}
			else
			{
				fmp_app_scan_state = FMP_SCANNING;
			}
		}
		else
		{
			if(!bat_device_adv)
 			{
 				if(battery_start_advertisement() == AT_BLE_SUCCESS)
 				{
 					bat_device_adv = true;
 				}
 			}
		}
	}
	return AT_BLE_SUCCESS;
}

/**@brief Connect to a peer device
*
*/
at_ble_status_t app_connect_request(at_ble_scan_info_t *scan_buffer,
uint8_t index)
{
	memcpy((uint8_t *)&fmp_app_info.fmp_peer_address,
	(uint8_t *)&scan_buffer[index].dev_addr,
	sizeof(at_ble_addr_t));

	if (gap_dev_connect(&fmp_app_info.fmp_peer_address) == AT_BLE_SUCCESS) {
		DBG_LOG("FMP Connect request sent");
		return AT_BLE_SUCCESS;
		} else {
		DBG_LOG("FMP Connect request send failed");
	}

	return AT_BLE_FAILURE;
}

static uint16_t app_no_device_found_handler(void)
{
	uint16_t index = 0;
	do
	{
		if(index != 0xFF)
		{
			DBG_LOG("%c", index);
			DBG_LOG("Select [s] to scan again or [a] to advertise");
		}
		index = getchar_timeout(30000);
	} while (!(index == 's' || index == 'a'));
	DBG_LOG("%c", index);
	
	if(index == 's') 
	{
		if(fmp_app_scan_state == FMP_IDLE)
		{
			if(gap_dev_scan() != AT_BLE_SUCCESS)
			{
				DBG_LOG("Failed to initiate scanning");
			}
			else
			{
				fmp_app_scan_state = FMP_SCANNING;
				return AT_BLE_SUCCESS;
			}	
		}
	}
	return index;
}

/**@brief Handles AT_BLE_SCAN_REPORT event from stack
*
*/
static at_ble_status_t ble_scan_report_app_event(void *param)
{
	uint8_t fmp_scan_device_count = 0;	
	uint8_t scanned_dev_count = scan_response_count;
	uint8_t scan_index = 0;
	uint8_t index = 0;
	uint8_t scan_device[MAX_SCAN_DEVICE];
	at_ble_scan_info_t *scan_buffer = (at_ble_scan_info_t *)scan_info;
	
	fmp_app_scan_state = FMP_IDLE;
	memset(scan_device, 0, MAX_SCAN_DEVICE);
	if (scanned_dev_count) 
	{
		
		at_ble_uuid_t service_uuid;

		for (index = 0; index < scanned_dev_count; index++) {
			/* Display only the connectible devices*/
			if((scan_buffer[index].type == AT_BLE_ADV_TYPE_DIRECTED)
			|| (scan_buffer[index].type == AT_BLE_ADV_TYPE_UNDIRECTED)) {
				scan_device[fmp_scan_device_count++] = index;
			}
		}
		
		if (fmp_scan_device_count) 
		{
			/* Service type to be searched */
			service_uuid.type = AT_BLE_UUID_16;

			/* Service UUID */
			service_uuid.uuid[1] = (IMMEDIATE_ALERT_SERVICE_UUID >> 8);
			service_uuid.uuid[0] = (uint8_t)IMMEDIATE_ALERT_SERVICE_UUID;
			
			for (index = 0; index < fmp_scan_device_count; index++) {
				DBG_LOG("Info: Device found address [%d]  0x%02X%02X%02X%02X%02X%02X ",
				index,
				scan_buffer[scan_device[index]].dev_addr.addr[5],
				scan_buffer[scan_device[index]].dev_addr.addr[4],
				scan_buffer[scan_device[index]].dev_addr.addr[3],
				scan_buffer[scan_device[index]].dev_addr.addr[2],
				scan_buffer[scan_device[index]].dev_addr.addr[1],
				scan_buffer[scan_device[index]].dev_addr.addr[0]);
				
				if (scan_info_parse(&scan_buffer[scan_device[index]], &service_uuid,
				AD_TYPE_COMPLETE_LIST_UUID) ==
				AT_BLE_SUCCESS) {
					/* Device Service UUID  matched */
					fmp_supp_scan_index[scan_index++] = index;
					DBG_LOG_CONT("---FMP");
				}
			}
		}

		if (!scan_index)  
		{
			DBG_LOG("FMP Profile supported device not found ");
		}
		
		/* Stop the current scan active */
		at_ble_scan_stop();
		
		/*Updating the index pointer to connect */
		if(fmp_scan_device_count) 
		{
			/* Successful device found event*/
			uint8_t deci_index = fmp_scan_device_count;
			deci_index+=FMP_ASCII_TO_DECIMAL_VALUE;
			
			do {
				if(index != 0xFF)
				{
					DBG_LOG("%c", index);
					DBG_LOG("Select Index number to Connect or [s] to scan or [a] to advertise");
				}
				index = getchar_timeout(30000);
			} while (!(((index < (deci_index)) && (index >='0')) || (index == 's') || (index == 'a')));
			
			DBG_LOG("%c", index);
			if(index == 's') 
			{
				if(fmp_app_scan_state == FMP_IDLE)
				{
					if(gap_dev_scan() != AT_BLE_SUCCESS)
					{
						DBG_LOG("Failed to initiate scanning");
					}
					else
					{
						fmp_app_scan_state = FMP_SCANNING;
						return AT_BLE_SUCCESS;
					}
				}
			} 
			else if(index != 'a')
			{
				index -= FMP_ASCII_TO_DECIMAL_VALUE;
				return app_connect_request(scan_buffer,	scan_device[index]);
			}
		}
		else
		{
			index = app_no_device_found_handler();
		}
	} 
	else 
	{
		/* from no device found event*/
		index = app_no_device_found_handler();
	}
	if(index == 'a')
	{
		if(bat_device_adv == false)
		{
			if(battery_start_advertisement() == AT_BLE_SUCCESS)
 			{
				 bat_device_adv = true;
 			}	
		}
	}
	ALL_UNUSED(param);
	return AT_BLE_FAILURE;
	
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t bas_paired_app_event(void *param)
{
	at_ble_pair_done_t *ble_pair_done;
	ble_pair_done = (at_ble_pair_done_t *)param;
	
	if (ble_pair_done->status == AT_BLE_SUCCESS)
	{
		hw_timer_start(BATTERY_UPDATE_INTERVAL);
	}
	else
	{
		if(ble_check_iscentral(ble_pair_done->handle))
		{
			fmp_app_scan_state = FMP_IDLE;
		}
	}

	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack */
static at_ble_status_t bas_encryption_status_changed_app_event(void *param)
{
	at_ble_encryption_status_changed_t *ble_enc_status;
	ble_enc_status = (at_ble_encryption_status_changed_t *)param;
	
	if (ble_enc_status->status == AT_BLE_SUCCESS)
	{
		hw_timer_start(BATTERY_UPDATE_INTERVAL);
	}
	else
	{
		if(ble_check_iscentral(ble_enc_status->handle))
		{
			fmp_app_scan_state = FMP_IDLE;
		}
	}
	
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	at_ble_disconnected_t *ble_disconnected;
	ble_disconnected = (at_ble_disconnected_t *)param;
	DBG_LOG("disconnected handle %d",ble_disconnected->handle);
	uint8_t idx;
	for(idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
	{
		if (bas_app_info[idx].conn_handle == ble_disconnected->handle)
		{
			DBG_LOG("Disconnected bas central with handle %d with entry %d",ble_disconnected->handle,idx);
			bas_app_info[idx].conn_handle = BLE_INVALID_CONNECTION_HANDLE;
			bas_app_info[idx].notification_state = BAS_NOTIFICATION_DISABLED;
			bas_app_info[idx].bas_level_notified = false;
			if(bat_device_adv == false && fmp_app_scan_state != FMP_SCANNING)
			{
				if(battery_start_advertisement() == AT_BLE_SUCCESS)
				{
					bat_device_adv = true;
				}			
			}
			break;
		}
	}
	if(fmp_app_info.fmp_conn_handle == ble_disconnected->handle)
	{
		fmp_app_scan_state = FMP_IDLE;
		if(bat_device_adv == false && fmp_app_scan_state != FMP_SCANNING)
		{
			if(gap_dev_scan() == AT_BLE_SUCCESS)
			{
				fmp_app_scan_state = FMP_SCANNING;
			}
		}
	}
	
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Advertisement start */
static at_ble_status_t battery_start_advertisement(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	bool volatile max_not_reached = false;
	uint8_t idx;
	
	for(idx = 0; idx <BAS_MAX_DEVICE_CONNECTION ; idx++)
	{
		if(bas_app_info[idx].conn_handle == BLE_INVALID_CONNECTION_HANDLE)
		{
			max_not_reached = true;
			break;
		}
	}
	
	if(!max_not_reached)
	{
		DBG_LOG("Battery reached maximum connections, can not go to advertisement");
		if(fmp_app_scan_state == FMP_IDLE)
		{
			if(gap_dev_scan() == AT_BLE_SUCCESS)
			{
				fmp_app_scan_state = FMP_SCANNING;
			}	
		}
		return status;
	}
	/* Start of advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_BAS_FAST_ADV, APP_BAS_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS)
	{		
		DBG_LOG("BLE Started Advertisement");
	}
	else
	{
		DBG_LOG("BLE Advertisement start Failed reason :%d", status);
	}
	return status;
}

/* Advertisement data set and Advertisement start */
static at_ble_status_t battery_set_advertisement_data(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
	}
		
	return status;
}

/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t ble_notification_confirmed_app_event(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(notification_status->status == AT_BLE_SUCCESS)
	{
		uint8_t idx;
		for (idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
		{
			if(bas_app_info[idx].conn_handle == notification_status->conn_handle)
			{
				bas_app_info[idx].notification_state = BAS_NOTIFICATION_CONFIRMED;
				bas_send_notification(NULL);
				break;
			}
		}
		
		DBG_LOG_DEV("sending notification to the peer success");
	}
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_characteristic_changed_t *char_handle;
	char_handle = (at_ble_characteristic_changed_t *)param;
	uint8_t idx;
	for (idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
	{
		if(bas_app_info[idx].conn_handle == char_handle->conn_handle)
		{
			status = AT_BLE_SUCCESS;
			if(bat_char_changed_event(&bas_service_handler, char_handle) == AT_BLE_SUCCESS)
			{
				bas_app_info[idx].notification_state = BAS_NOTIFICATION_ENABLED;				
			}
			else
			{
				bas_app_info[idx].notification_state = BAS_NOTIFICATION_DISABLED;
			}			
		}
	}

	return status;
}

/* Simulating the Battery level */
static void simulate_battery_level(void)
{
	if(battery_level == BATTERY_MAX_LEVEL)
	{
		battery_level_flag = false;
	}
	else if(battery_level == BATTERY_MIN_LEVEL)
	{
		battery_level_flag = true;
	}
	if(battery_level_flag)
	{
		battery_level++;
	}
	else
	{
		battery_level--;
	}
}

static at_ble_status_t bas_send_notification(void *param)
{
	uint8_t idx;
	at_ble_status_t status = AT_BLE_FAILURE;
		
	/* send the notification and Update the battery level  */
	for(idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
	{
		if (((bas_app_info[idx].conn_handle != BLE_INVALID_CONNECTION_HANDLE) && \
			 (bas_app_info[idx].bas_level_notified == false)) && \
			((bas_app_info[idx].notification_state == BAS_NOTIFICATION_ENABLED) || \
			 (bas_app_info[idx].notification_state == BAS_NOTIFICATION_CONFIRMED)))
		{
			if((bat_update_char_value(bas_app_info[idx].conn_handle, &bas_service_handler, \
									battery_level, true) == AT_BLE_SUCCESS))
			{
				bas_app_info[idx].notification_state = BAS_NOTIFICATION_SENT;
				bas_app_info[idx].bas_level_notified = true;
				status = AT_BLE_SUCCESS;
				break;
			}
		}
	}
	UNUSED(param);
	return status;
}

/* Simulating the battery level and sending notifications to the peer devices */
static at_ble_status_t battery_simulation_task(void *param)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	if (bat_timer_done)
	{
		uint8_t idx;
		bat_timer_done = false;
		DBG_LOG("Battery Level:%d%%", battery_level);	
		status = bas_send_notification(NULL);
		simulate_battery_level();	
		for (idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
		{
			bas_app_info[idx].bas_level_notified = false;
		}
		
		if (status == AT_BLE_FAILURE)
		{
			status = bas_send_notification(NULL);
		}
		hw_timer_start(BATTERY_UPDATE_INTERVAL);
	}
    ALL_UNUSED(param);
	return status;
}

/**@brief Check for Link Loss and Path Loss alert
* check for Low Alert value if crossed write Low Alert value to Immediate Alert
*Service. High Alert value if crossed write High Alert value to IAS service
*
* @param[in] conn_handle Connection handle of a connected device
*/
static void rssi_update(at_ble_handle_t conn_handle)
{
	int8_t rssi_power = 0;
	at_ble_status_t status;

	/* Get the Received signal strength intensity of the connect
	*device/handle*/
	DBG_LOG("RSSI update: Handle %d", conn_handle);
	if ((status = at_ble_rx_power_get(conn_handle, &rssi_power)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("at_ble_rx_power_get failed,reason %d",status);
	}
	DBG_LOG("Rx Power(RSSI):%04d dBm\r\n", rssi_power);

	/* if received rssi is above no alert zone and below high alert zone */
	if ((rssi_power <= FMP_LOW_ALERT_RANGE) &&
			(rssi_power >= FMP_HIGH_ALERT_RANGE)) {
		if (!(alert_level == FMP_MID_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_MID_ALERT);
			alert_level = FMP_MID_ALERT;
		}

		DBG_LOG_CONT("---Mild Alert!");
		LED_Toggle(LED0);
	}
	/* if received rssi is above mild alert zone */
	else if (rssi_power < FMP_HIGH_ALERT_RANGE) {
		if (!(alert_level == FMP_HIGH_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_HIGH_ALERT);
			alert_level = FMP_HIGH_ALERT;
		}
		DBG_LOG_CONT("---High Alert!!!");
		LED_On(LED0);
	}
	/* if received rssi is below mild alert zone */
	else {
		if (!(alert_level == FMP_NO_ALERT)) {
			ias_alert_level_write(conn_handle,
					ias_handle.char_handle,
					IAS_NO_ALERT);
			alert_level = FMP_NO_ALERT;
			LED_Off(LED0);
		}
		DBG_LOG_CONT("---No Alert");
	}
}

/* Find me sending the alert level to peer devices after checking the rssi */
static void fmp_simulation_task(void)
{
	if(findme_timer_done)
	{
		if(fmp_app_scan_state == FMP_CONNECTED)
		{
			findme_timer_done = false;
			rssi_update(fmp_app_info.fmp_conn_handle);
		}
	}	
}

static at_ble_status_t multirole_app_platform_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		DBG_LOG("Platform Event: Timer");
		fmp_simulation_task();
		DBG_LOG("Platform Event: FMP ");
		battery_simulation_task(NULL);
		DBG_LOG("Platform Event: Timer Exit");	
	}
	else if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
		DBG_LOG("Platform Event: Button");
		if(fmp_app_scan_state == FMP_CONNECTED)
		{
			if(at_ble_disconnect(fmp_app_info.fmp_conn_handle, AT_BLE_TERMINATED_BY_USER) != AT_BLE_SUCCESS)
			{
				DBG_LOG("FMP disconnection failed");
			}
			else
			{
				fmp_app_scan_state = FMP_IDLE;
			}
		}
		//delay_ms(200);
		button_debounce = true;
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	return status;
}

int main(void)
{	
	at_ble_status_t status;
	uint32_t idx;
	
	platform_driver_init();
	acquire_sleep_lock();
	
	/* Initialize serial console */
	serial_console_init();	
	/* Initialize button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);
	/* Initialize LED */
	led_init();
	/* Initialize the hardware timer */
	hw_timer_init();
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Multi-Role Peripheral Multi-Connection Application");

	/* initialize the BLE chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initialize the battery service */
	bat_init_service(&bas_service_handler, &battery_level);
	
	/* Define the primary service in the GATT server database */
	if((status = bat_primary_service_define(&bas_service_handler))!= AT_BLE_SUCCESS)
	{
		DBG_LOG("defining battery service failed %d", status);
	}
	
	for(idx = 0; idx < BAS_MAX_DEVICE_CONNECTION; idx++)
	{
		bas_app_info[idx].conn_handle = BLE_INVALID_CONNECTION_HANDLE;;
		bas_app_info[idx].notification_state = BAS_NOTIFICATION_DISABLED;
		bas_app_info[idx].bas_level_notified = false;
	}
	
	/* Register callbacks for gap related events */ 
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&battery_app_gap_cb);
									
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									&battery_app_gatt_server_cb);
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&multirole_custom_event_cb);
									
	battery_set_advertisement_data();
	battery_start_advertisement();
				
	fmp_locator_init();
	
	ble_set_ulp_mode(BLE_ULP_MODE_CLEAR);

	while (1) {
		/* BLE Event Task */
		ble_event_task(0xFFFFFFFF);
	}
}
