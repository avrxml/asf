/**
 *
 * \file
 *
 * \brief Handling BT related activity for provision AP.
 *
 * Copyright (c) 2018 Atmel Corporation. All rights reserved.
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include "btstack_chipset_atwilc3000.h"
#include "btstack_debug.h"
#include "btstack_memory.h"
#include "btstack_run_loop.h"
#include "btstack_run_loop_freertos.h"
#include "classic/btstack_link_key_db.h"
#include "hci.h"
#include "hci_dump.h"
#include "le_prov.h"
#include "le_prov_db.h"
#include "btstack.h"
#include "wifi_prov.h"

extern TaskHandle_t btstack_run_loop_task;
static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

/*Used for BT state*/
static bt_state_t state = BT_IDLE;

/*Used for BT Connection Handle*/
static uint16_t le_prov_conn_handle = 0;

/*Used for registering handler for HCI event*/
static btstack_packet_callback_registration_t hci_event_callback_registration;

/*Used for notifying data*/
uint8_t payload_data;

static const btstack_uart_block_t * uart_driver;
static btstack_uart_config_t uart_config;

static hci_transport_config_uart_t transport_config = {
	HCI_TRANSPORT_CONFIG_UART,
	115200,
	0,
	1,
	NULL,
};

/*Data set for advertisement*/
const uint8_t le_prov_adv_data[] = {
    /*Flags general discoverable, BR/EDR not supported*/
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06, 
};

/*Length of advertisement data*/
const uint8_t le_prov_adv_data_len = sizeof(le_prov_adv_data);

/*Data set for scan response data*/
const uint8_t le_prov_scanrsp_data[] = {
	
	/*Incomplete List of 128-bit Service Class UUID*/
	0x11, BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS,  WIFICON_UUID(0x0001),
	
	/*Device name*/
	0x0a, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'M', 'C', 'H', 'P', '_', 'W', 'I', 'F', 'I',	
};

/*Length of scan response data*/
const uint8_t le_prov_scanrsp_data_len = sizeof(le_prov_scanrsp_data);

/**
 * \brief This function is called by BT stack during attribute write
 */
static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
	UNUSED(offset);
	if (att_handle == WIFI_CONN_STATE_CCD_HANDLE)
	{
		conn_ap.conn_state_descriptor = buffer[0];
	}
	else if (att_handle == WIFI_SCAN_MODE_HANDLE)
	{
		if (buffer[0] == SCANLIST_SCANNING)
		{
			scan_ap.scan_mode = SCANLIST_SCANNING;
			state = BT_AP_SCANNING;
			att_server_request_can_send_now_event(le_prov_conn_handle);
		}
	} 
	else if(att_handle == WIFI_AP_PARAM_HANDLE)
	{
		if((conn_ap.conn_param_offset + buffer_size) <= CONN_PARAM_SIZE)
		{
			memcpy(&conn_ap.conn_param[conn_ap.conn_param_offset], buffer, buffer_size);
			conn_ap.conn_param_offset += buffer_size;
		}
	}
	
	if (transaction_mode == ATT_TRANSACTION_MODE_EXECUTE)
	{
		conn_ap.conn_state = WIFI_CONNECT_INPROGRESS;
		state = BT_AP_CONN_STATE;
		att_server_request_can_send_now_event(le_prov_conn_handle);
	}
	
	return 0;
}

/**
 * \brief This function is called by BT stack during attribute read
 */
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
	UNUSED(connection_handle);
	
	uint16_t req_buff_size = 0;
	uint8_t ap_idx = 0;
	uint8_t inv_req_handle = 0;
	
	if(buffer)
	{
		req_buff_size = 0;
		if ((att_handle >= WIFI_AP_LIST_START_HANDLE) && (att_handle <= WIFI_AP_LIST_END_HANDLE))
		{
			switch(att_handle)
			{
				case WIFI_AP_1_HANDLE:
				ap_idx = WIFI_AP_1_INDEX;
				break;
				
				case WIFI_AP_2_HANDLE:
				ap_idx = WIFI_AP_2_INDEX;
				break;
				
				case WIFI_AP_3_HANDLE:
				ap_idx = WIFI_AP_3_INDEX;
				break;
				
				case WIFI_AP_4_HANDLE:
				ap_idx = WIFI_AP_4_INDEX;
				break;
				
				case WIFI_AP_5_HANDLE:
				ap_idx = WIFI_AP_5_INDEX;
				break;
				
				case WIFI_AP_6_HANDLE:
				ap_idx = WIFI_AP_6_INDEX;
				break;
				
				case WIFI_AP_7_HANDLE:
				ap_idx = WIFI_AP_7_INDEX;
				break;
				
				case WIFI_AP_8_HANDLE:
				ap_idx = WIFI_AP_8_INDEX;
				break;
				
				case WIFI_AP_9_HANDLE:
				ap_idx = WIFI_AP_9_INDEX;
				break;
				
				case WIFI_AP_10_HANDLE:
				ap_idx = WIFI_AP_10_INDEX;
				break;
				
				case WIFI_AP_11_HANDLE:
				ap_idx = WIFI_AP_11_INDEX;
				break;
				
				case WIFI_AP_12_HANDLE:
				ap_idx = WIFI_AP_12_INDEX;
				break;
				
				case WIFI_AP_13_HANDLE:
				ap_idx = WIFI_AP_13_INDEX;
				break;
				
				case WIFI_AP_14_HANDLE:
				ap_idx = WIFI_AP_14_INDEX;
				break;
				
				case WIFI_AP_15_HANDLE:
				ap_idx = WIFI_AP_15_INDEX;
				break;
				
				default:
				printf("\r\nInvalid AP Handle\n");
				inv_req_handle = 1;
				break;
			} //End Switch
			if (!inv_req_handle)
			{
				if (offset == 0)
				{
					buffer[SECURITY_BYTE_INDEX] = scan_ap.ap_val[ap_idx].security_type;
					buffer[RSSI_VALUE_INDEX] = scan_ap.ap_val[ap_idx].rssi_value;
					buffer[SSID_LEN_INDEX] = scan_ap.ap_val[ap_idx].ssid_len;
					
					if (scan_ap.ap_val[ap_idx].ssid_len < (buffer_size - 3))
					{
						memcpy(&buffer[SSID_VAL_INDEX], scan_ap.ap_val[ap_idx].ssid_val, scan_ap.ap_val[ap_idx].ssid_len);
						req_buff_size = (SECURITY_BYTE_SIZE + RSSI_VALUE_SIZE + SSID_LEN_SIZE + scan_ap.ap_val[ap_idx].ssid_len);
					}
					else
					{
						memcpy(&buffer[SSID_VAL_INDEX], scan_ap.ap_val[ap_idx].ssid_val, (buffer_size - 3));
						req_buff_size = buffer_size;
					}					
				} //if (offset == 0)
				else
				{
					memcpy(&buffer[0], &scan_ap.ap_val[ap_idx].ssid_val[19], (scan_ap.ap_val[ap_idx].ssid_len - 19));
					req_buff_size = (scan_ap.ap_val[ap_idx].ssid_len - 19);
				}	
			}
			else
			{
				req_buff_size = 0;
			}			
		}//End if ((att_handle >= WIFI_AP_LIST_START_HANDLE) || (att_handle <= WIFI_AP_LIST_END_HANDLE))
		else if (att_handle == WIFI_SCAN_MODE_HANDLE)
		{
			buffer[0] = scan_ap.scan_mode;
			req_buff_size = 1;
		}
		else if (att_handle == WIFI_AP_SCAN_HANDLE)
		{
			buffer[0] = scan_ap.ap_count;
			req_buff_size = 1;
		}
	}
	else
	{
		if ((att_handle >= WIFI_AP_LIST_START_HANDLE) && (att_handle <= WIFI_AP_LIST_END_HANDLE))
		{
			req_buff_size = (SECURITY_BYTE_SIZE + RSSI_VALUE_SIZE + SSID_LEN_SIZE + SSID_VAL_SIZE);
		}
		else if ((att_handle == WIFI_CONN_STATE_HANDLE) || (att_handle == WIFI_SCAN_MODE_HANDLE) || (att_handle == WIFI_AP_SCAN_HANDLE))
		{
			req_buff_size = 1;
		}
		else
		{
			printf("\r\nInvalid Handle Request\n");
			req_buff_size = 0;
		}
	}
	return req_buff_size;
}

/**
 * \brief This function is called to configure and start the advertisement.
 */
void le_prov_adv_start(void)
{
	// setup advertisements
	uint16_t adv_int_min = 0x0030;
	uint16_t adv_int_max = 0x0030;
	uint8_t adv_type = 0;
	bd_addr_t null_addr;
	memset(null_addr, 0, 6);
	gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
	gap_advertisements_set_data(le_prov_adv_data_len, (uint8_t*) le_prov_adv_data);
	gap_scan_response_set_data(le_prov_scanrsp_data_len, (uint8_t*)le_prov_scanrsp_data);
	gap_advertisements_enable(1);
}

/**
 * \brief This function is called for application setup.
 */
void le_prov_app_setup(void)
{
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    l2cap_init();
    le_device_db_init();
    sm_init();
    att_server_init(profile_data, att_read_callback, att_write_callback);    
    att_server_register_packet_handler(packet_handler);
	le_prov_adv_start();
}

/**
 * \brief This function is called for handling HCI Event.
 */
static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    wifi_bt_if_msg_t wifi_msg;	
    UNUSED(channel);
    UNUSED(size);    
	switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet))
			{
                case HCI_EVENT_LE_META:
					switch(hci_event_le_meta_get_subevent_code(packet))
					{
						case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
							le_prov_conn_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
							scan_ap.ap_count = 0;
							scan_ap.scan_mode = SCANLIST_STOP;
							state = BT_IDLE;
							conn_ap.conn_state = WIFI_CONNECT_FAIL;
							conn_ap.conn_state_descriptor = 0;
							conn_ap.conn_param_offset = 0;
						break;
					}
				break;

				case HCI_EVENT_DISCONNECTION_COMPLETE:
					printf("\r\nBT Disconnect Event\n");
					scan_ap.ap_count = 0;
					scan_ap.scan_mode = SCANLIST_STOP;
					state = BT_IDLE;
					conn_ap.conn_state = WIFI_CONNECT_FAIL;
					conn_ap.conn_state_descriptor = 0;
					conn_ap.conn_param_offset = 0;
					break;
							
                case ATT_EVENT_CAN_SEND_NOW:
				    switch (state)
					{
						case BT_IDLE:
							printf("\r\n BT in IDLE State\n");
						break;
						
						case BT_AP_SCANNING:
							payload_data = scan_ap.scan_mode;
							if((att_server_notify(le_prov_conn_handle, WIFI_SCAN_MODE_HANDLE, &payload_data, 1)) == RES_SUCCESS)
							{
								wifi_msg.id = BT_WIFI_SCAN;
								xQueueSend(wifi_bt_if_queue, (void *)&wifi_msg, portMAX_DELAY);
								xSemaphoreTake(wifi_bt_if_sem, portMAX_DELAY);	
								scan_ap.scan_mode = SCANLIST_UPDATED;
								state = BT_AP_LIST_UPDATED;
								att_server_request_can_send_now_event(le_prov_conn_handle);
							}
						break;
						
						case BT_AP_LIST_UPDATED:
							payload_data = scan_ap.scan_mode;
							xSemaphoreGive(wifi_bt_if_sem);
							if((att_server_notify(le_prov_conn_handle, WIFI_SCAN_MODE_HANDLE, &payload_data, 1)) == RES_SUCCESS)
							{
								scan_ap.scan_mode = SCANLIST_STOP;
								state = BT_IDLE;
							}
						break;
						
						case BT_AP_CONN_STATE:
						    payload_data = conn_ap.conn_state;
						    if((att_server_notify(le_prov_conn_handle, WIFI_CONN_STATE_HANDLE, &payload_data, 1)) == RES_SUCCESS)
						    {
							    wifi_msg.id = BT_WIFI_CONNECT;
							    xQueueSend(wifi_bt_if_queue, (void *)&wifi_msg, portMAX_DELAY);
								xSemaphoreTake(wifi_bt_conn_if_sem, portMAX_DELAY);
								state = BT_AP_CONN_STATE_UPDATED;
							    att_server_request_can_send_now_event(le_prov_conn_handle);
								 
						    }
						break;
						
						case BT_AP_CONN_STATE_UPDATED:
							payload_data = conn_ap.conn_state;
							if((att_server_notify(le_prov_conn_handle, WIFI_CONN_STATE_HANDLE, &payload_data, 1)) == RES_SUCCESS)
							{
								state = BT_IDLE;
								xSemaphoreGive(wifi_bt_conn_if_sem);								
							}
							else
							{
								printf("\r\nNotification fail\n");
							}							
						break;
								
						default:
							printf("\r\n Invalid State\n");
						break;
					
					}				     
				     break;
            }
            break;
    }
}

/**
 * \brief This function is used for initializing the BT application.
 */
static void bt_app_init(int status){

	if (status){
		printf("\r\nFW download fail\n");
		return;
	}
	const hci_transport_t * transport = hci_transport_h4_instance(uart_driver);
	hci_init(transport, (void*) &transport_config);
	hci_set_chipset(btstack_chipset_atwilc3000_instance());
	hci_set_link_key_db(btstack_link_key_db_memory_instance());
	btstack_main();
	
}

/**
 * \brief This function is called during creation of BT task.
 */
void bt_task(void *arg)
{
	wifi_bt_if_msg_t bt_msg;	
	xSemaphoreTake(wifi_bt_if_sem, portMAX_DELAY);
	btstack_run_loop_task = bt_task_handle;	
	if (xQueueReceive(wifi_bt_if_queue, (void *)&bt_msg, portMAX_DELAY) != pdFALSE) 
	{
		xSemaphoreGive(wifi_bt_if_sem);
		if(bt_msg.id == WIFI_BT_INIT)
		{
			btstack_memory_init();
           	btstack_run_loop_init(btstack_run_loop_freertos_get_instance());
			uart_driver = btstack_uart_block_freertos_instance();
			uart_config.baudrate    = HCI_DEFAULT_BAUDRATE;
			uart_config.flowcontrol = 0;
			uart_config.device_name = transport_config.device_name;
			uart_driver->init(&uart_config);
			vTaskDelay(5);		
			btstack_chipset_atwilc3000_download_firmware(uart_driver, transport_config.baudrate_init, transport_config.flowcontrol, NULL, 0, &bt_app_init);
			btstack_run_loop_execute();
		}
	}	
}

/**
 * \brief This function is called after initialization of HCI transport.
 */
int btstack_main(void)
{
	le_prov_app_setup();
	hci_power_control(HCI_POWER_ON);
	return 0;
}

