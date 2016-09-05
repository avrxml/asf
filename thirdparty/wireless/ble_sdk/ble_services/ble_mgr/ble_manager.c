/**
* \file
*
* \brief BLE Manager
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


#include <asf.h>
#include <string.h>
#include "at_ble_api.h"
#include "at_ble_trace.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "platform.h"

#if BLE_DEVICE_ROLE == BLE_ROLE_ALL
#ifndef ATT_DB_MEMORY
#define ATT_DB_MEMORY
#endif
#endif

#if defined ATT_DB_MEMORY
uint32_t att_db_data[BLE_ATT_DB_MEMORY_SIZE/4] = {0};
#endif


volatile uint8_t ble_device_count;

ble_connected_dev_info_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];

static at_ble_addr_t ble_peripheral_dev_address;

at_ble_connected_t connected_state_info;

const ble_event_callback_t *ble_mgr_gap_event_cb[MAX_GAP_EVENT_SUBSCRIBERS];
const ble_event_callback_t *ble_mgr_gatt_client_event_cb[MAX_GATT_CLIENT_SUBSCRIBERS];
const ble_event_callback_t *ble_mgr_gatt_server_event_cb[MAX_GATT_SERVER_SUBSCRIBERS];
const ble_event_callback_t *ble_mgr_l2cap_event_cb[MAX_L2CAP_EVENT_SUBSCRIBERS];
const ble_event_callback_t *ble_mgr_htpt_event_cb[MAX_HTPT_EVENT_SUBSCRIBERS];
const ble_event_callback_t *ble_mgr_dtm_event_cb[MAX_DTM_EVENT_SUBSCRIBERS];
const ble_event_callback_t *ble_mgr_custom_event_cb[MAX_CUSTOM_EVENT_SUBSCRIBERS];

static const ble_event_callback_t ble_mgr_gap_handle[] = {
	ble_undefined_event_handler,
	ble_scan_info_handler,
	ble_scan_report_handler,
	NULL,
	NULL,
	ble_connected_state_handler,
	ble_disconnected_state_handler,
	ble_conn_param_update,
	ble_conn_param_update_req,
	ble_pair_done_handler,
	ble_pair_request_handler,
	ble_slave_security_request_handler,
	ble_pair_key_request_handler,	
	ble_encryption_request_handler,
	ble_encryption_status_change_handler,
	ble_resolv_rand_addr_handler,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t ble_mgr_gatt_server_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ble_mtu_changed_indication_handler,
	ble_mtu_changed_cmd_complete_handler,
	ble_characteristic_write_cmd_complete_handler,
	NULL
};

volatile uint8_t scan_response_count = 0;
at_ble_scan_info_t scan_info[MAX_SCAN_DEVICE];


at_ble_events_t event;
uint8_t ble_event_params[BLE_EVENT_PARAM_MAX_SIZE];

/** @brief initializes the platform */
static void ble_init(at_ble_init_config_t * args);

/** @brief Set BLE Address, If address is NULL then it will use BD public address */
static void ble_set_dev_config(at_ble_addr_t *addr);

/** @brief function to get event from stack */
at_ble_status_t ble_event_task(void)
{
    if (at_ble_event_get(&event, ble_event_params, BLE_EVENT_TIMEOUT) == AT_BLE_SUCCESS) 
    {
            ble_event_manager(event, ble_event_params);
            return AT_BLE_SUCCESS;
    }
    
    return AT_BLE_FAILURE;
}

at_ble_init_config_t pf_cfg;

/** @brief BLE device initialization */
void ble_device_init(at_ble_addr_t *addr)
{
	uint8_t idx;	
	char *dev_name = NULL;

	/* Initialize the BLE Event callbacks */
	for (idx = 0; idx < MAX_GAP_EVENT_SUBSCRIBERS; idx++)
	{
		ble_mgr_gap_event_cb[idx] = NULL;
	}
	
	for (idx = 0; idx < MAX_GATT_CLIENT_SUBSCRIBERS; idx++)
	{
		ble_mgr_gatt_client_event_cb[idx] = NULL;
	}
	
	for (idx = 0; idx < MAX_GATT_SERVER_SUBSCRIBERS; idx++)
	{
		ble_mgr_gatt_server_event_cb[idx] = NULL;
	}
	
	for (idx = 0; idx < MAX_L2CAP_EVENT_SUBSCRIBERS; idx++)
	{
		ble_mgr_l2cap_event_cb[idx] = NULL;
	}
	
	for (idx = 0; idx < MAX_HTPT_EVENT_SUBSCRIBERS; idx++)
	{
		ble_mgr_htpt_event_cb[idx] = NULL;
	}
	
	for (idx = 0; idx < MAX_DTM_EVENT_SUBSCRIBERS; idx++)
	{
		ble_mgr_dtm_event_cb[idx] = NULL;
	}
	
	for (idx = 0; idx < MAX_CUSTOM_EVENT_SUBSCRIBERS; idx++)
	{
		ble_mgr_custom_event_cb[idx] = NULL;
	}
        
    /* Set the BLE Device connection state */
    for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
    {
        memset(&ble_dev_info[idx], 0, sizeof(ble_connected_dev_info_t));
		ble_dev_info[idx].conn_state = BLE_DEVICE_DEFAULT_IDLE;
		/* Set Invalid to Bonding information */
		ble_dev_info[idx].bond_info.status = AT_BLE_GAP_INVALID_PARAM;
    }
	/* Need to reset the count to 0 for storing it only in SRAM */
	ble_device_count = 0; 
	
	
#if defined ATT_DB_MEMORY
	pf_cfg.memPool.memSize = BLE_ATT_DB_MEMORY_SIZE;
	pf_cfg.memPool.memStartAdd = (uint8_t *)&att_db_data;
#else
	pf_cfg.memPool.memSize = 0;
	pf_cfg.memPool.memStartAdd = NULL;
#endif
	/*Bus configuration*/
	pf_cfg.bus_info.bus_type = AT_BLE_UART;

#if UART_FLOWCONTROL_6WIRE_MODE == true
	/* Enable Hardware Flow-control on BTLC1000 */
   pf_cfg.bus_info.bus_flow_control_enabled = true; // enable flow control
#else
	/* Disable Hardware Flow-control on BTLC1000 */
   pf_cfg.bus_info.bus_flow_control_enabled = false; // Disable flow control
#endif

	/* Register Platform callback API's */
	pf_cfg.platform_api_list.at_ble_create_timer = platform_create_timer;
	pf_cfg.platform_api_list.at_ble_delete_timer = platform_delete_timer;
	pf_cfg.platform_api_list.at_ble_start_timer = platform_start_timer;
	pf_cfg.platform_api_list.at_ble_stop_timer = platform_stop_timer;
	pf_cfg.platform_api_list.at_ble_sleep = platform_sleep;
	pf_cfg.platform_api_list.at_ble_gpio_set = platform_gpio_set;
	pf_cfg.platform_api_list.at_ble_send_sync = platform_send_sync;
	pf_cfg.platform_api_list.at_ble_recv_async = platform_recv_async;
	pf_cfg.platform_api_list.at_ble_reconfigure_usart = platform_configure_hw_fc_uart;
	
	platform_init(pf_cfg.bus_info.bus_type, pf_cfg.bus_info.bus_flow_control_enabled);
	
	ble_init(&pf_cfg);

	/* Register it in first index of callback handler */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, 
									BLE_GAP_EVENT_TYPE, 
									ble_mgr_gap_handle);
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GATT_SERVER_EVENT_TYPE,
									ble_mgr_gatt_server_handle);
									
	dev_name = (char *)BLE_DEVICE_NAME;
	if (ble_set_device_name((uint8_t *)dev_name, strlen(dev_name)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Device name set failed");
	}
	
	ble_set_dev_config(addr);	
}

/** @brief set device name to BLE Device*/
at_ble_status_t ble_set_device_name(uint8_t *name, uint8_t name_len)
{
	if ((name == NULL) || (name_len < 1))
	{
		return AT_BLE_INVALID_PARAM;
	}
	return at_ble_device_name_set(name, name_len);
}

/* Initialize the BLE */
static void ble_init(at_ble_init_config_t * args)
{
	/* Initialize the platform */
	DBG_LOG("Initializing BTLC1000");
	
	/* Init BLE device */
	if(at_ble_init(args) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BTLC1000 Initialization failed");
		DBG_LOG("Please check the power and connection / hardware connector");	
		while(1);
	}
}


/* Set BLE Address and device configuration, If address is NULL then it will use BD public address */
static void ble_set_dev_config(at_ble_addr_t *addr)
{
	at_ble_dev_config_t stDevConfig;
	at_ble_addr_t address = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
	
	if (addr == NULL)
	{		
		/* get BD address from BLE device */
		if(at_ble_addr_get(&address) != AT_BLE_SUCCESS)
		{
			DBG_LOG("BD address get failed");
		}
		
		/* Copy the BD address into address pointer */
		addr = &address;
	}
	
	DBG_LOG("BD Address:0x%02X%02X%02X%02X%02X%02X, Address Type:%d",
	addr->addr[5],
	addr->addr[4],
	addr->addr[3],
	addr->addr[2],
	addr->addr[1],
	addr->addr[0], addr->type);
	
	/* Set device configuration */
	/* Device role */
	stDevConfig.role = (at_ble_dev_role_t)BLE_DEVICE_ROLE;
	/* device renew duration */
	stDevConfig.renew_dur = AT_RENEW_DUR_VAL_MIN;
	/* device address type */
	memcpy((uint8_t *)&stDevConfig.address, (uint8_t *)addr, sizeof(at_ble_addr_t));
	/* Attributes */
	stDevConfig.att_cfg.b2NamePerm = AT_BLE_WRITE_DISABLE;
	stDevConfig.att_cfg.b2AppearancePerm = AT_BLE_WRITE_DISABLE;
	stDevConfig.att_cfg.b1EnableSpcs = 0;
	stDevConfig.att_cfg.b1EnableServiceChanged = 0;
	stDevConfig.att_cfg.b2Rfu = AT_BLE_WRITE_DISABLE;
	/* Handles */
	stDevConfig.gap_start_hdl = AT_BLE_AUTO_ALLOC_HANDLE;
	stDevConfig.gatt_start_hdl = AT_BLE_AUTO_ALLOC_HANDLE;
	/* MTU */
	stDevConfig.max_mtu = AT_MTU_VAL_RECOMMENDED;
	
	if(at_ble_set_dev_config(&stDevConfig) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Set BLE Device configuration failed");
	}
	
	if (at_ble_addr_set(addr) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Set BLE Device Address failed");
	}
}

bool ble_mgr_events_callback_handler(ble_mgr_event_cb_t event_cb_type, 
									ble_mgr_event_t event_type,
									const ble_event_callback_t *ble_event_handler)
{
	uint8_t idx;
	uint8_t max_event_handler = 0;
	bool status = true;
	const ble_event_callback_t **ble_event_cb_ptr = NULL;
	if (ble_event_handler != NULL)
	{
		switch(event_type)
		{
			case BLE_GAP_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_gap_event_cb;
				max_event_handler = MAX_GAP_EVENT_SUBSCRIBERS;
			}
			break;
			
			case BLE_GATT_CLIENT_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_gatt_client_event_cb;
				max_event_handler = MAX_GATT_CLIENT_SUBSCRIBERS;
			}
			break;
			
			case BLE_GATT_SERVER_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_gatt_server_event_cb;
				max_event_handler = MAX_GATT_SERVER_SUBSCRIBERS;
			}
			break;
			
			case BLE_L2CAP_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_l2cap_event_cb;
				max_event_handler = MAX_L2CAP_EVENT_SUBSCRIBERS;
			}
			break;
			
			case BLE_GATT_HTPT_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_htpt_event_cb;
				max_event_handler = MAX_HTPT_EVENT_SUBSCRIBERS;
			}
			break;
			
			case BLE_DTM_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_dtm_event_cb;
				max_event_handler = MAX_DTM_EVENT_SUBSCRIBERS;
			}
			break;
			
			case BLE_CUSTOM_EVENT_TYPE:
			{
				ble_event_cb_ptr = ble_mgr_custom_event_cb;
				max_event_handler = MAX_CUSTOM_EVENT_SUBSCRIBERS;
			}
			break;
			
			default:
			{
				status = false; //Invalid Parameter
			}			
			break;
		}
		
		if (status)
		{
			status = false; //Defaults to Failure[No Space to register, Not Found, Invalid Type]scenario
			if (event_cb_type == REGISTER_CALL_BACK)
			{
				for (idx = 0; idx < max_event_handler; idx++)
				{
					if(ble_event_cb_ptr[idx] == NULL)
					{
						ble_event_cb_ptr[idx] = ble_event_handler;
						status = true; //Successfully registered to table
						break;
					}
				}
			}
			else if(event_cb_type == UNREGISTER_CALL_BACK)
			{
				for (idx = 0; idx < max_event_handler; idx++)
				{
					if(ble_event_cb_ptr[idx] == ble_event_handler)
					{
						ble_event_cb_ptr[idx] = NULL;
						status = true;//Successfully unregistered from table
						break;
					}
				}
			}			
		}
	}
	else
	{
		status = false; //Invalid parameter
	}
	return status;
}

at_ble_status_t ble_undefined_event_handler(void *params)
{
	DBG_LOG_DEV("Undefined Event Received");
    ALL_UNUSED(params);
	return AT_BLE_SUCCESS;
}

/** @brief request the peer device for connection */
at_ble_status_t gap_dev_connect(at_ble_addr_t *dev_addr)
{
	at_ble_connection_params_t gap_conn_parameter;
	
	memcpy((uint8_t *)&ble_peripheral_dev_address, (uint8_t *)dev_addr, sizeof(at_ble_addr_t));

	gap_conn_parameter.con_intv_min = GAP_CONN_INTERVAL_MIN;
	gap_conn_parameter.con_intv_max = GAP_CONN_INTERVAL_MAX;
	gap_conn_parameter.con_latency = GAP_CONN_SLAVE_LATENCY;
	gap_conn_parameter.ce_len_min = GAP_CE_LEN_MIN;
	gap_conn_parameter.ce_len_max = GAP_CE_LEN_MAX;
	gap_conn_parameter.superv_to = GAP_SUPERVISION_TIMOUT;

	return (at_ble_connect(dev_addr, GAP_CONNECT_PEER_COUNT, SCAN_INTERVAL, SCAN_WINDOW, &gap_conn_parameter));
}

/** @brief instructs device to start scanning */
at_ble_status_t gap_dev_scan(void)
{
	/* Device Scan discover started*/
	DBG_LOG("Scanning...Please wait...");
	/* make service discover counter to zero*/
	scan_response_count = 0;
	return(at_ble_scan_start(SCAN_INTERVAL, SCAN_WINDOW, SCAN_TIMEOUT, SCAN_TYPE, AT_BLE_SCAN_GEN_DISCOVERY, false,true)) ;
}

/** @brief function handling scaned information */
at_ble_status_t ble_scan_info_handler(void *params)
{
	at_ble_scan_info_t *scan_param;
	scan_param = (at_ble_scan_info_t *)params;
	if(scan_response_count < MAX_SCAN_DEVICE)
	{
		// store the advertising report data into scan_info[]
		memcpy((uint8_t *)&scan_info[scan_response_count], scan_param, sizeof(at_ble_scan_info_t));
		DBG_LOG_DEV("Info:Device found address [%d]  0x%02X%02X%02X%02X%02X%02X ",
		scan_response_count,
		scan_param->dev_addr.addr[5],
		scan_param->dev_addr.addr[4],
		scan_param->dev_addr.addr[3],
		scan_param->dev_addr.addr[2],
		scan_param->dev_addr.addr[1],
		scan_param->dev_addr.addr[0]);
		scan_response_count++;
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("Info:maximum no.of scan device reached...Stopping Scan");
		if(at_ble_scan_stop() != AT_BLE_SUCCESS)
		{
			DBG_LOG("Failed to stop scanning");
		}
		
		return AT_BLE_FAILURE;
	}
}

/** @brief function handles scan report */
at_ble_status_t ble_scan_report_handler(void *params)
{
	at_ble_scan_report_t *scan_report;
	scan_report = (at_ble_scan_report_t *)params;
	if (scan_report->status == AT_BLE_SUCCESS)
	{
		/* All scan data should be handled */
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("Scanning  failed");
	}
	return AT_BLE_FAILURE;
}

/* Parse the received advertising data for service and local name */
uint8_t scan_info_parse(at_ble_scan_info_t *scan_info_data,
				at_ble_uuid_t *ble_service_uuid, uint8_t adv_type)
{
	if (scan_info_data->adv_data_len) {
		uint8_t adv_data_size;
		uint8_t index = 0;
		volatile adv_element_t *adv_element_p;
		volatile adv_element_t adv_element_data;

		adv_data_size = scan_info_data->adv_data_len;
		adv_element_data.len = 0;
		while (adv_data_size) {
			adv_element_data.len = scan_info_data->adv_data[index];
			adv_element_data.type
			= scan_info_data->adv_data[index + 1];
			adv_element_data.data
			= &scan_info_data->adv_data[index + 2];
			adv_element_p = &adv_element_data;

			if (adv_element_p->type == adv_type) {
				/* passing the length of data type */
				uint8_t adv_type_size = adv_element_p->len;
				/* actual size of the data	*/
				adv_type_size -= 1;
				while (adv_type_size) {
					volatile int cmp_status = -1;
					if (ble_service_uuid->type ==
					AT_BLE_UUID_16) {
						cmp_status = memcmp(
						adv_element_p->data, ble_service_uuid->uuid,
						AT_BLE_UUID_16_LEN);
						adv_element_p->data
						+= AT_BLE_UUID_16_LEN;
						adv_type_size
						-= AT_BLE_UUID_16_LEN;
					} else if (ble_service_uuid->type ==
					AT_BLE_UUID_32) {
						cmp_status = memcmp(
						adv_element_p->data, ble_service_uuid->uuid,
						AT_BLE_UUID_32_LEN);
						adv_element_p->data
						+= AT_BLE_UUID_32_LEN;
						adv_type_size
						-= AT_BLE_UUID_32_LEN;
					} else if (ble_service_uuid->type ==
					AT_BLE_UUID_128) {
						cmp_status = memcmp(
						adv_element_p->data, ble_service_uuid->uuid,
						AT_BLE_UUID_128_LEN);
						adv_element_p->data
						+= AT_BLE_UUID_128_LEN;
						adv_type_size
						-= AT_BLE_UUID_32_LEN;
					}

					if (cmp_status == 0) {
						return AT_BLE_SUCCESS;
					}
				}
			}

			index += (adv_element_data.len + 1);
			adv_element_data.len += 1;
			adv_data_size -= adv_element_data.len;
		}
	}

	return AT_BLE_FAILURE;
}

/** @brief function to send slave security request */
at_ble_status_t ble_send_slave_sec_request(at_ble_handle_t conn_handle)
{
	#if BLE_PAIR_ENABLE
		if (at_ble_send_slave_sec_request(conn_handle, BLE_MITM_REQ, BLE_BOND_REQ) == AT_BLE_SUCCESS)
		{
			DBG_LOG_DEV("Slave security request successful");
			return AT_BLE_SUCCESS;
		}
		else
		{
			DBG_LOG("Slave security request failed");
		}
	#endif
        ALL_UNUSED(conn_handle);
	return AT_BLE_FAILURE;
}

bool ble_check_ispheripheral(at_ble_handle_t handle)
{
	at_ble_dev_role_t dev_role;
	
	if(ble_connected_device_role(handle, &dev_role) == AT_BLE_SUCCESS)
	{
		if (dev_role == AT_BLE_ROLE_PERIPHERAL)
		{
			return true;
		}
	}
	return false;
}

bool ble_check_iscentral(at_ble_handle_t handle)
{
	at_ble_dev_role_t dev_role = AT_BLE_ROLE_NONE;
	
	if(ble_connected_device_role(handle, &dev_role) == AT_BLE_SUCCESS)
	{
		if (dev_role == AT_BLE_ROLE_CENTRAL)
		{
			return true;
		}
	}
	return false;
}

bool ble_check_disconnected_iscentral(at_ble_handle_t handle)
{
	at_ble_dev_role_t dev_role;
	
	if(ble_disconnected_device_role(handle, &dev_role) == AT_BLE_SUCCESS)
	{
		if (dev_role == AT_BLE_ROLE_CENTRAL)
		{
			return true;
		}
	}
	return false;
}

at_ble_status_t ble_connected_device_role(at_ble_handle_t conn_handle, at_ble_dev_role_t *dev_role)
{
	uint8_t idx;
	at_ble_status_t status = AT_BLE_FAILURE;
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_state != BLE_DEVICE_DEFAULT_IDLE) && 
		  (ble_dev_info[idx].conn_state != BLE_DEVICE_DISCONNECTED) &&
		  (ble_dev_info[idx].conn_info.handle == conn_handle))
		{
			*dev_role = ble_dev_info[idx].dev_role;
			status = AT_BLE_SUCCESS;
			break;
		}
	}
	return status;
}

at_ble_status_t ble_disconnected_device_role(at_ble_handle_t conn_handle, at_ble_dev_role_t *dev_role)
{
	uint8_t idx;
	at_ble_status_t status = AT_BLE_FAILURE;
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if(((ble_dev_info[idx].conn_state == BLE_DEVICE_DEFAULT_IDLE) ||
		(ble_dev_info[idx].conn_state == BLE_DEVICE_DISCONNECTED))&&
		(ble_dev_info[idx].conn_info.handle == conn_handle))
		{
			*dev_role = ble_dev_info[idx].dev_role;
			status = AT_BLE_SUCCESS;
			break;
		}
	}
	return status;
}

at_ble_status_t ble_check_device_state(at_ble_handle_t conn_handle, ble_device_state_t state)
{
	uint8_t idx;
	at_ble_status_t status = AT_BLE_FAILURE;
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_state == state) &&
		  (ble_dev_info[idx].conn_info.handle == conn_handle))
		{
			status = AT_BLE_SUCCESS;
			break;
		}
	}
	return status;
}

/** @brief function to handle connected event received from stack */
at_ble_status_t ble_connected_state_handler(void *params)
{
	at_ble_connected_t *conn_params;
	uint8_t idx = 0;
	conn_params = (at_ble_connected_t *)params;
	bool peripheral_device_added = false;
	
	if (conn_params->conn_status == AT_BLE_SUCCESS)
	{
		DBG_LOG("Connected to peer device with address 0x%02x%02x%02x%02x%02x%02x",
		conn_params->peer_addr.addr[5],
		conn_params->peer_addr.addr[4],
		conn_params->peer_addr.addr[3],
		conn_params->peer_addr.addr[2],
		conn_params->peer_addr.addr[1],
		conn_params->peer_addr.addr[0]);
		
		DBG_LOG("Connection Handle %d", conn_params->handle);
		
		memcpy((uint8_t *)&connected_state_info, (uint8_t *)conn_params, sizeof(at_ble_connected_t));	
		if(memcmp((uint8_t *)&ble_peripheral_dev_address, (uint8_t *)&conn_params->peer_addr, sizeof(at_ble_addr_t)))
		{
			/* Peripheral Device */
			if (conn_params->peer_addr.type == AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE) 
			{
				uint8_t idx1, idx2;
				uint16_t key_len = 0;
				uint8_t nb_keys = BLE_MAX_DEVICE_CONNECTED;
				uint8_t irk_key[BLE_MAX_DEVICE_CONNECTED * AT_BLE_MAX_KEY_LEN] = {0, };
				for (idx1 = 0; idx1 < BLE_MAX_DEVICE_CONNECTED; idx1++)
				{
					for (idx2 = 0; idx2 < AT_BLE_MAX_KEY_LEN; idx2++)
					{
						irk_key[key_len++] = ble_dev_info[idx1].bond_info.peer_irk.key[idx2];
					}
					
				}
				if (nb_keys)
				{
					if(at_ble_random_address_resolve(BLE_MAX_DEVICE_CONNECTED, &conn_params->peer_addr, irk_key) == AT_BLE_SUCCESS)
					{
						DBG_LOG_DEV("Resolving Random address success**");
						return AT_BLE_SUCCESS;
					}
					else
					{
						DBG_LOG("Resolving Random address failed**");
						return AT_BLE_FAILURE;
					}					
				}
				else
				{
					DBG_LOG_DEV("New Connection");
				}				
			}
		}
			
		
		{
			bool conn_exists = false;
			for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
			{
				if(!memcmp((uint8_t *)&ble_dev_info[idx].conn_info.peer_addr, (uint8_t *)&conn_params->peer_addr, sizeof(at_ble_addr_t)))
				{
					ble_dev_info[idx].conn_state = BLE_DEVICE_CONNECTED;
					conn_exists = true;
					break;
				}
			}
			
			if (!conn_exists)
			{
				if (ble_device_count == BLE_MAX_DEVICE_CONNECTED)
				{
					DBG_LOG("Max number of connection reached: %d ===>Disconnecting...", ble_device_count);
					at_ble_disconnect(conn_params->handle, AT_BLE_TERMINATED_BY_USER);
					return AT_BLE_FAILURE;
				}
				for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
				{
					if(ble_dev_info[idx].conn_state == BLE_DEVICE_DEFAULT_IDLE)
					{
						/* @Todo Need to Handle Random Address */
						memcpy(&ble_dev_info[idx].conn_info, (uint8_t *)conn_params, sizeof(at_ble_connected_t));
						ble_device_count++;
						ble_dev_info[idx].conn_state = BLE_DEVICE_CONNECTED;
						break;
					}
				}
			}
			
			if(!memcmp((uint8_t *)&ble_peripheral_dev_address, (uint8_t *)&conn_params->peer_addr, sizeof(at_ble_addr_t)))
			{
				ble_dev_info[idx].dev_role = AT_BLE_ROLE_CENTRAL;
			}
			else
			{
				ble_dev_info[idx].dev_role = AT_BLE_ROLE_PERIPHERAL;
				peripheral_device_added = true;
			}
			/* Reset the ble_peripheral_dev_address to identify the initiator */
			memset((uint8_t *)&ble_peripheral_dev_address, 0, sizeof(at_ble_addr_t));
					
		}
		
		
#if ((BLE_DEVICE_ROLE == BLE_ROLE_PERIPHERAL) || (BLE_DEVICE_ROLE == BLE_ROLE_ALL))
		
        if((ble_dev_info[idx].dev_role == AT_BLE_ROLE_PERIPHERAL) && (peripheral_device_added))
        {
	 		ble_send_slave_sec_request(conn_params->handle);
        }
#endif
	} 
	else
	{
		DBG_LOG("Device Connection Failed - Status: %d", conn_params->conn_status);
	}
	ALL_UNUSED(peripheral_device_added);
	return AT_BLE_SUCCESS;
}

at_ble_status_t ble_resolv_rand_addr_handler(void *params)
{
	at_ble_resolv_rand_addr_status_t *ble_resolv_rand_addr_status;
	uint8_t idx;	
	bool device_found = false;
	bool peripheral_device_added = false;
	ble_resolv_rand_addr_status = (at_ble_resolv_rand_addr_status_t *)params;	
	
	if(ble_resolv_rand_addr_status->status == AT_BLE_SUCCESS)
	{		
		for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
		{
			/* Check the Resolved Address */
			if (!memcmp((uint8_t *)ble_resolv_rand_addr_status->irk, (uint8_t *)ble_dev_info[idx].bond_info.peer_irk.key, AT_BLE_MAX_KEY_LEN))
			{
				device_found = true;
				break;
			}
		}	
		
	}
	
	if (device_found)
	{
		DBG_LOG_DEV("##########Device Found");
		ble_dev_info[idx].conn_state = BLE_DEVICE_CONNECTED;
		ble_dev_info[idx].dev_role = AT_BLE_ROLE_PERIPHERAL;
		memcpy((uint8_t *)&ble_dev_info[idx].conn_info, (uint8_t *)&connected_state_info, sizeof(at_ble_connected_t));
		peripheral_device_added = true;
	}
	else
	{
		DBG_LOG_DEV("##########Device Not Found");
		if (ble_device_count == BLE_MAX_DEVICE_CONNECTED)
		{
			DBG_LOG("Max number of connection reached: %d ===>Disconnecting...", ble_device_count);
			at_ble_disconnect(connected_state_info.handle, AT_BLE_TERMINATED_BY_USER);
			return AT_BLE_FAILURE;
		}
		for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
		{
			if(ble_dev_info[idx].conn_state == BLE_DEVICE_DEFAULT_IDLE)
			{
				memcpy(&ble_dev_info[idx].conn_info, (uint8_t *)&connected_state_info, sizeof(at_ble_connected_t));
				ble_device_count++;
				ble_dev_info[idx].conn_state = BLE_DEVICE_CONNECTED;
				ble_dev_info[idx].dev_role = AT_BLE_ROLE_PERIPHERAL;
				peripheral_device_added = true;
				break;
			}
		}
	}
	DBG_LOG_DEV("Device idx:%d",idx);
	#if ((BLE_DEVICE_ROLE == BLE_ROLE_PERIPHERAL) || (BLE_DEVICE_ROLE == BLE_ROLE_ALL))
	if((ble_dev_info[idx].dev_role == AT_BLE_ROLE_PERIPHERAL) && (peripheral_device_added))
	{
		ble_send_slave_sec_request(connected_state_info.handle);
	}
	#endif
	ALL_UNUSED(peripheral_device_added);

	return AT_BLE_SUCCESS;
	
}

at_ble_status_t ble_mtu_changed_indication_handler(void *params)
{
	at_ble_mtu_changed_ind_t *mtu_changed_ind;
	mtu_changed_ind = (at_ble_mtu_changed_ind_t *)params;
	DBG_LOG_DEV("BLE-MTU Changed, Connection Handle: %d, New Value: %d", 
										mtu_changed_ind->conhdl, 
										mtu_changed_ind->mtu_value);
	return AT_BLE_SUCCESS;
}

at_ble_status_t ble_mtu_changed_cmd_complete_handler(void *params)
{
	at_ble_cmd_complete_event_t *cmd_complete_event;
	cmd_complete_event = (at_ble_cmd_complete_event_t *)params;
	
	if (cmd_complete_event->status == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("MTU Changed, Connection Handle:%d, Operation:%d", 
		cmd_complete_event->conn_handle, 
		cmd_complete_event->operation);
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("MTU Change Failed, Connection Handle:%d, Operation:%d",
		cmd_complete_event->conn_handle,
		cmd_complete_event->operation);
	}
	return AT_BLE_FAILURE;
}

at_ble_status_t ble_characteristic_write_cmd_complete_handler(void *params)
{
	at_ble_cmd_complete_event_t *cmd_complete_event;
	cmd_complete_event = (at_ble_cmd_complete_event_t *)params;
	if (cmd_complete_event->status == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("Char Write Cmd Complete, Connection Handle:%d, Operation:%d",
		cmd_complete_event->conn_handle,
		cmd_complete_event->operation);
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG_DEV("Char Write Cmd Failed, Connection Handle:%d, Operation:%d",
		cmd_complete_event->conn_handle,
		cmd_complete_event->operation);
	}
	return AT_BLE_FAILURE;
}

/** @brief function handles disconnection event received from stack */
at_ble_status_t ble_disconnected_state_handler(void *params)
{
	at_ble_disconnected_t *disconnect;
	uint8_t idx;
	disconnect = (at_ble_disconnected_t *)params;
	
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == disconnect->handle) && 
		((ble_dev_info[idx].conn_state == BLE_DEVICE_PAIRED) || (ble_dev_info[idx].conn_state == BLE_DEVICE_ENCRYPTION_COMPLETED)))
		{
			ble_dev_info[idx].conn_state = BLE_DEVICE_DISCONNECTED;
		}		
		else if(ble_dev_info[idx].conn_info.handle == disconnect->handle)
		{
			switch (ble_dev_info[idx].conn_state)
			{
				case BLE_DEVICE_CONNECTED:			
				case BLE_DEVICE_PAIRING:
				case BLE_DEVICE_PAIRING_FAILED:
				case BLE_DEVICE_ENCRYPTION_STATE:
				case BLE_DEVICE_ENCRYPTION_FAILED:
				{
					/* Device is not paired so remove the device information */
					ble_dev_info[idx].conn_state = BLE_DEVICE_DEFAULT_IDLE;
					if (ble_device_count > 0)
					{
						ble_device_count--;
					}
				}
				break;
				
				case BLE_DEVICE_DISCONNECTED:
				break;
				
				default:
				DBG_LOG_DEV("State Not Handled %d", ble_dev_info[idx].conn_state);
				ble_dev_info[idx].conn_state = BLE_DEVICE_DEFAULT_IDLE;
				break;				
			}
		} 
	}
	
	DBG_LOG("Device disconnected Reason:0x%02x Handle=0x%x", disconnect->reason, disconnect->handle);
	return AT_BLE_SUCCESS;
}

/** @brief connection update parameter function */
at_ble_status_t ble_conn_param_update(void *params)
{
	at_ble_conn_param_update_done_t * conn_param_update;
	conn_param_update = (at_ble_conn_param_update_done_t *)params;
	DBG_LOG_DEV("AT_BLE_CONN_PARAM_UPDATE ");
	ALL_UNUSED(conn_param_update);  //To avoid compiler warning
	return AT_BLE_SUCCESS;
}

at_ble_status_t ble_conn_param_update_req(void *params)
{
	at_ble_conn_param_update_request_t * conn_param_req;
	conn_param_req = (at_ble_conn_param_update_request_t *)params;
	at_ble_conn_update_reply(conn_param_req->handle, true, 1, 120);
	return AT_BLE_SUCCESS;
}

at_ble_status_t ble_slave_security_request_handler(void* params)
{
	at_ble_pair_features_t features;
	uint8_t i = 0, idx;
	at_ble_slave_sec_request_t* slave_sec_req;
	bool device_found = false;
	
	slave_sec_req = (at_ble_slave_sec_request_t*)params;	
	
	//if (slave_sec_req->status != AT_BLE_SUCCESS)
	//{
		//at_ble_disconnect(slave_sec_req->handle, AT_BLE_AUTH_FAILURE);		
		//return AT_BLE_FAILURE;
		//@Todo Status is not handled in the Library
	//}

	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == slave_sec_req->handle) && (ble_dev_info[idx].conn_state == BLE_DEVICE_CONNECTED))
		{
			device_found = true;
			ble_dev_info[idx].conn_state = BLE_DEVICE_ENCRYPTION_STATE;
			break;
		}
	}
	
	if (device_found)
	{
		if((ble_dev_info[idx].bond_info.auth & AT_BLE_AUTH_NO_MITM_BOND) && (slave_sec_req->bond == true))
		{
			if(at_ble_encryption_start(slave_sec_req->handle, &ble_dev_info[idx].bond_info.peer_ltk, ble_dev_info[idx].bond_info.auth) == AT_BLE_SUCCESS)
			{
				return  AT_BLE_SUCCESS;
			}
			else
			{
				DBG_LOG("Encryption Not started");
				return AT_BLE_FAILURE;
			}			
		}
		else
		{
			ble_dev_info[idx].conn_state = BLE_DEVICE_CONNECTED;
		}
	}
	
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == slave_sec_req->handle) && (ble_dev_info[idx].conn_state == BLE_DEVICE_CONNECTED))
		{
			ble_dev_info[idx].conn_state = BLE_DEVICE_PAIRING;
			break;
		}
	}
	
	features.desired_auth =  BLE_AUTHENTICATION_LEVEL; 
	features.bond = slave_sec_req->bond;
	features.mitm_protection = true;
	/* Device capabilities is display only , key will be generated
	and displayed */
	features.io_cababilities = AT_BLE_IO_CAP_KB_DISPLAY;

	features.oob_avaiable = false;
			
	/* Distribution of LTK is required */
	if (ble_dev_info[idx].conn_info.peer_addr.type == AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE)
	{
		features.initiator_keys =   (at_ble_key_dis_t)(AT_BLE_KEY_DIST_ENC | AT_BLE_KEY_DIST_ID);
		features.responder_keys =   (at_ble_key_dis_t)(AT_BLE_KEY_DIST_ENC | AT_BLE_KEY_DIST_ID);
	}
	else
	{
		features.initiator_keys =   AT_BLE_KEY_DIST_ENC;
		features.responder_keys =   AT_BLE_KEY_DIST_ENC;
	}
	features.max_key_size = 16;
	features.min_key_size = 16;
	
	/* Check if fresh pairing requested */
	if (ble_dev_info[idx].bond_info.status == AT_BLE_GAP_INVALID_PARAM)
	{
		/* Generate LTK */
		for(i=0; i<8; i++)
		{			
			ble_dev_info[idx].host_ltk.key[i] = rand()&0x0f;
			ble_dev_info[idx].host_ltk.nb[i] = rand()&0x0f;
		}
				
		for(i=8 ; i<16 ;i++)
		{
			ble_dev_info[idx].host_ltk.key[i] = rand()&0x0f;
		}
		
		ble_dev_info[idx].host_ltk.ediv = rand()&0xffff;
		ble_dev_info[idx].host_ltk.key_size = 16;
	}
	else
	{
		/* Bonding information already exists */
		
	}

	if(at_ble_authenticate(slave_sec_req->handle, &features, &ble_dev_info[idx].host_ltk, NULL) != AT_BLE_SUCCESS)
	{
		features.bond = false;
		features.mitm_protection = false;
		DBG_LOG("Slave Security Req - Authentication Failed");
		return AT_BLE_FAILURE;
	}
	return AT_BLE_SUCCESS;
}

/** @brief function handles pair request */
at_ble_status_t ble_pair_request_handler(void *params)
{
	at_ble_pair_features_t features;
	uint8_t i = 0, idx;
	at_ble_pair_request_t* pair_req;
	pair_req = (at_ble_pair_request_t*)params;
	
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == pair_req->handle) && (ble_dev_info[idx].conn_state == BLE_DEVICE_CONNECTED))
		{
			ble_dev_info[idx].conn_state = BLE_DEVICE_PAIRING;
			break;
		}
	}
	
	features.desired_auth =  BLE_AUTHENTICATION_LEVEL; 
	features.bond = BLE_BOND_REQ;
	features.mitm_protection = BLE_MITM_REQ;
	/* Device capabilities is display only , key will be generated
	and displayed */
	features.io_cababilities = BLE_IO_CAPABALITIES;
	features.oob_avaiable = BLE_OOB_REQ;
	
	/* Distribution of LTK is required */
	if (ble_dev_info[idx].conn_info.peer_addr.type == AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE)
	{
		/* Distribution of IRK is required */
		features.initiator_keys =   (at_ble_key_dis_t)(AT_BLE_KEY_DIST_ENC | AT_BLE_KEY_DIST_ID);
		features.responder_keys =   (at_ble_key_dis_t)(AT_BLE_KEY_DIST_ENC | AT_BLE_KEY_DIST_ID);
	}
	else
	{
		features.initiator_keys =   AT_BLE_KEY_DIST_ENC;
		features.responder_keys =   AT_BLE_KEY_DIST_ENC;
	}
			
	features.max_key_size = 16;
	features.min_key_size = 16;
	
	/* Check if fresh pairing requested */
	if (ble_dev_info[idx].bond_info.status == AT_BLE_GAP_INVALID_PARAM)
	{
		/* Generate LTK */
		for(i=0; i<8; i++)
		{						
			ble_dev_info[idx].host_ltk.key[i] = rand()&0x0f;
			ble_dev_info[idx].host_ltk.nb[i] = rand()&0x0f;
		}
				
		for(i=8 ; i<16 ;i++)
		{
			ble_dev_info[idx].host_ltk.key[i] = rand()&0x0f;
		}
		DBG_LOG_DEV("Generated LTK: ");
		for (i = 0; i < 16; i++)
		{
			DBG_LOG_CONT_DEV("0x%02X ", ble_dev_info[idx].host_ltk.key[i]);
		}
		
		ble_dev_info[idx].host_ltk.ediv = rand()&0xffff;
		ble_dev_info[idx].host_ltk.key_size = 16;
	}
	else
	{
		/* Bonding information already exists */
		
	}

	/* Send pairing response */
	DBG_LOG_DEV("Sending pairing response");

	if(at_ble_authenticate(pair_req->handle, &features, &ble_dev_info[idx].host_ltk, NULL) != AT_BLE_SUCCESS)
	{
		features.bond = false;
		features.mitm_protection = false;
		DBG_LOG("Pair Request - Authentication Failed");
		return AT_BLE_FAILURE;
	}
	return AT_BLE_SUCCESS;
}

/** @brief function handles pair key request */
at_ble_status_t ble_pair_key_request_handler (void *params)
{
	at_ble_pair_key_request_t *pair_key;
	pair_key = (at_ble_pair_key_request_t *)params;
	/* Passkey has fixed value in this example MSB */
	uint8_t passkey[6]={'1','2','3','4','5','6'};
	uint8_t idx = 0;
        uint8_t pin;
        
	at_ble_pair_key_request_t pair_key_request;
        
	memcpy((uint8_t *)&pair_key_request, pair_key, sizeof(at_ble_pair_key_request_t));
	
	if (pair_key_request.passkey_type == AT_BLE_PAIR_PASSKEY_ENTRY) {
	  DBG_LOG("Enter the Passkey(6-Digit) in Terminal:");
	  for (idx = 0; idx < 6;) {          
		pin = getchar_timeout(PIN_TIMEOUT);
		if (!pin) {
			DBG_LOG("Pin Timeout");
			DBG_LOG("Disconnecting ...");
			if (!(at_ble_disconnect(pair_key->handle,
						AT_BLE_TERMINATED_BY_USER) == AT_BLE_SUCCESS)) {
				DBG_LOG("Disconnect Request Failed");
			}
			return AT_BLE_FAILURE;
		}
		if ((pin >= '0') && ( pin <= '9')) {
		  passkey[idx++] = pin;
		  DBG_LOG_CONT("%c", pin);
		} 
	  }
	}	
	
	/* Display passkey */
	if(((pair_key_request.passkey_type == AT_BLE_PAIR_PASSKEY_DISPLAY) &&
	   (pair_key_request.type == AT_BLE_PAIR_PASSKEY)) || (pair_key_request.passkey_type == AT_BLE_PAIR_PASSKEY_ENTRY))
	{
          if(pair_key_request.passkey_type == AT_BLE_PAIR_PASSKEY_ENTRY)
          {
            DBG_LOG("Entered Pass-code:");
          }
          else
          {
            DBG_LOG("Please Enter the following Pass-code(on other Device):");
          }
          
          /* Convert passkey to ASCII format */
          for(idx=0; idx<AT_BLE_PASSKEY_LEN; idx++)
          {
                  passkey[idx] = (passkey[idx]);
                  DBG_LOG_CONT("%c",passkey[idx]);
          }		
          
          if(!(at_ble_pair_key_reply(pair_key->handle, pair_key_request.type, passkey)) == AT_BLE_SUCCESS)
          {
                  DBG_LOG("Pair-key reply failed");
          }
	}
	else 
	{
		if(pair_key_request.type == AT_BLE_PAIR_OOB)
		{
			DBG_LOG("OOB Feature Not supported");
		}
	}
	return AT_BLE_SUCCESS;	
}

/** @brief function handles pair done event */
at_ble_status_t ble_pair_done_handler(void *params)
{
	at_ble_pair_done_t *pairing_params;
	uint8_t idx;
	bool device_found = false;
	pairing_params = (at_ble_pair_done_t *)params;
	
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == pairing_params->handle) && (ble_dev_info[idx].conn_state == BLE_DEVICE_PAIRING))
		{
			ble_dev_info[idx].conn_state = BLE_DEVICE_PAIRED;
			device_found = true;
			break;
		}
	}
	
	if(pairing_params->status == AT_BLE_SUCCESS)
	{
		DBG_LOG("Pairing procedure completed successfully");
		if (device_found)
		{
			ble_dev_info[idx].bond_info.auth = pairing_params->auth;
			ble_dev_info[idx].bond_info.status = pairing_params->status;
			memcpy((uint8_t *)&ble_dev_info[idx].bond_info.peer_csrk, (uint8_t *)&pairing_params->peer_csrk, sizeof(at_ble_CSRK_t));
			memcpy((uint8_t *)&ble_dev_info[idx].bond_info.peer_irk, (uint8_t *)&pairing_params->peer_irk, sizeof(at_ble_IRK_t));
			memcpy((uint8_t *)&ble_dev_info[idx].bond_info.peer_ltk, (uint8_t *)&pairing_params->peer_ltk, sizeof(at_ble_LTK_t));
			ble_dev_info->conn_state = BLE_DEVICE_PAIRED;
			
			DBG_LOG_DEV("LTK: ");
			for (idx = 0; idx < 16; idx++)
			{
				DBG_LOG_CONT_DEV("0x%02X, ", pairing_params->peer_ltk.key[idx]);
				
			}
			DBG_LOG_DEV("CSRK: ");
			for (idx = 0; idx < 16; idx++)
			{
				DBG_LOG_CONT_DEV("0x%02X, ", pairing_params->peer_csrk.key[idx]);
				
			}
			DBG_LOG_DEV("IRK:");
			for (idx = 0; idx < 16; idx++)
			{
				DBG_LOG_CONT_DEV("0x%02X, ", pairing_params->peer_irk.key[idx]);
				
			}
		}
		else
		{
			DBG_LOG("BLE Device not found to store the pairing info");
			return AT_BLE_FAILURE;
		}		
	}
	else
	{
		ble_dev_info[idx].conn_state = BLE_DEVICE_PAIRING_FAILED;
		DBG_LOG("Pairing failed...Disconnecting");
		if(!(at_ble_disconnect(pairing_params->handle, AT_BLE_TERMINATED_BY_USER) == AT_BLE_SUCCESS))
		{
			DBG_LOG("Disconnect Request Failed");
			return AT_BLE_FAILURE;
		}
	}
	return AT_BLE_SUCCESS;
}

/** @brief function handles encryption status change */
at_ble_status_t ble_encryption_status_change_handler(void *params)
{
	at_ble_encryption_status_changed_t *enc_status;
	uint8_t idx;
	bool device_found = false;
	
	enc_status = (at_ble_encryption_status_changed_t *)params;
	
	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == enc_status->handle) && (ble_dev_info[idx].conn_state == BLE_DEVICE_ENCRYPTION_STATE))
		{
			device_found = true;
			break;
		}
	}
	
	if(enc_status->status == AT_BLE_SUCCESS)
	{
		DBG_LOG("Encryption completed successfully");
		if (device_found)
		{
			ble_dev_info[idx].conn_state = BLE_DEVICE_ENCRYPTION_COMPLETED;
			ble_dev_info[idx].bond_info.auth = enc_status->authen;
			ble_dev_info[idx].bond_info.status = enc_status->status;
		}
		else
		{
			DBG_LOG("BLE Device not found encryption info");
			return AT_BLE_FAILURE;
		}
	}
	else
	{
		ble_dev_info[idx].bond_info.status = enc_status->status;
		ble_dev_info[idx].conn_state = BLE_DEVICE_ENCRYPTION_FAILED;
		DBG_LOG("Encryption failed");
		return AT_BLE_FAILURE;
	}
	return AT_BLE_SUCCESS;
}

/** @brief function handles encryption requests */
at_ble_status_t ble_encryption_request_handler(void *params)
{
	at_ble_encryption_request_t *enc_req;
	bool key_found = false;
	bool device_found = false;
	uint8_t idx;
	enc_req = (at_ble_encryption_request_t *)params;

	for (idx = 0; idx < BLE_MAX_DEVICE_CONNECTED; idx++)
	{
		if((ble_dev_info[idx].conn_info.handle == enc_req->handle) && (ble_dev_info[idx].conn_state != BLE_DEVICE_DISCONNECTED) &&
		(ble_dev_info[idx].conn_state != BLE_DEVICE_DEFAULT_IDLE))
		{
			device_found = true;
			ble_dev_info[idx].conn_state = BLE_DEVICE_ENCRYPTION_STATE;
			DBG_LOG_DEV("ENC-Req: Conn Handle Device Found:hdl:%d", enc_req->handle);
			break;
		}
	}
	
	if (device_found)
	{
		DBG_LOG_DEV("host device ediv %x",ble_dev_info[idx].host_ltk.ediv);
		DBG_LOG_DEV("peer device ediv %x",ble_dev_info[idx].bond_info.peer_ltk.ediv);
		DBG_LOG_DEV("enc_req ediv %x", enc_req->ediv);
		DBG_LOG_DEV("The index is %d",idx);
		if((ble_dev_info[idx].host_ltk.ediv == enc_req->ediv)
		&& !memcmp(&enc_req->nb[0],&ble_dev_info[idx].host_ltk.nb[0],8))
		{
			key_found = true;
			DBG_LOG_DEV("ENC-Req: Key Found");
		}
	}
	
	
    if(key_found == false)
    {
	    DBG_LOG("Pairing information of peer device is not available.");
	    DBG_LOG("Please unpair the device from peer device(mobile) settings menu and start pairing again");
		at_ble_disconnect(ble_dev_info[idx].conn_info.handle, AT_BLE_AUTH_FAILURE);
		return AT_BLE_FAILURE;
    }

	if(!(at_ble_encryption_request_reply(enc_req->handle, ble_dev_info[idx].bond_info.auth, key_found, &ble_dev_info[idx].host_ltk) == AT_BLE_SUCCESS))
	{
		DBG_LOG("Encryption Request Reply Failed");
	}
	else
	{      
		DBG_LOG_DEV("Encryption Request Reply");
	}
	return AT_BLE_SUCCESS;
}

void ble_event_manager(at_ble_events_t events, void *event_params)
{
	DBG_LOG_DEV("BLE-Event:%d", events);
	switch(events)
	{		
	 /* GAP events */
	case AT_BLE_UNDEFINED_EVENT:
	case AT_BLE_SCAN_INFO:
	case AT_BLE_SCAN_REPORT:
	case AT_BLE_ADV_REPORT:
	case AT_BLE_RAND_ADDR_CHANGED:
	case AT_BLE_CONNECTED:
	case AT_BLE_DISCONNECTED:
	case AT_BLE_CONN_PARAM_UPDATE_DONE:
	case AT_BLE_CONN_PARAM_UPDATE_REQUEST:
	case AT_BLE_PAIR_DONE:
	case AT_BLE_PAIR_REQUEST:
	case AT_BLE_SLAVE_SEC_REQUEST:
	case AT_BLE_PAIR_KEY_REQUEST:
	case AT_BLE_ENCRYPTION_REQUEST:
	case AT_BLE_ENCRYPTION_STATUS_CHANGED:
	case AT_BLE_RESOLV_RAND_ADDR_STATUS:
	case AT_BLE_SIGN_COUNTERS_IND:
	case AT_BLE_PEER_ATT_INFO_IND:
	case AT_BLE_CON_CHANNEL_MAP_IND:
	{
		uint8_t idx;
		for (idx = 0; idx < MAX_GAP_EVENT_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_gap_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_gap_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	case AT_BLE_PRIMARY_SERVICE_FOUND:
	case AT_BLE_INCLUDED_SERVICE_FOUND:
	case AT_BLE_CHARACTERISTIC_FOUND:
	case AT_BLE_DESCRIPTOR_FOUND:
	case AT_BLE_DISCOVERY_COMPLETE:
	case AT_BLE_CHARACTERISTIC_READ_BY_UUID_RESPONSE:
	case AT_BLE_CHARACTERISTIC_READ_MULTIBLE_RESPONSE:
	case AT_BLE_CHARACTERISTIC_WRITE_RESPONSE:
	case AT_BLE_NOTIFICATION_RECIEVED:
	case AT_BLE_INDICATION_RECIEVED:
	{
		uint8_t idx;
		events -= (AT_BLE_CON_CHANNEL_MAP_IND + 1);
		for (idx = 0; idx < MAX_GATT_CLIENT_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_gatt_client_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_gatt_client_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	case AT_BLE_NOTIFICATION_CONFIRMED:
	case AT_BLE_INDICATION_CONFIRMED:
	case AT_BLE_CHARACTERISTIC_CHANGED:
	case AT_BLE_CHARACTERISTIC_CONFIGURATION_CHANGED:
	case AT_BLE_SERVICE_CHANGED_INDICATION_SENT:
	case AT_BLE_WRITE_AUTHORIZE_REQUEST:
	case AT_BLE_MTU_CHANGED_INDICATION:
	case AT_BLE_MTU_CHANGED_CMD_COMPLETE:
	case AT_BLE_CHARACTERISTIC_WRITE_CMD_CMP:
	case AT_BLE_READ_AUTHORIZE_REQUEST:
	{
		uint8_t idx;
		events -= (AT_BLE_INDICATION_RECIEVED + 1);
		DBG_LOG_DEV("GATT_Server Event:%d", events);
		for (idx = 0; idx < MAX_GATT_SERVER_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_gatt_server_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_gatt_server_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	case AT_BLE_LECB_CONN_REQ:
	case AT_BLE_LECB_CONNECTED:
	case AT_BLE_LECB_DISCONNECTED:
	case AT_BLE_LECB_ADD_CREDIT_IND:
	case AT_BLE_LECB_SEND_RESP:
	case AT_BLE_LECB_DATA_RECIEVED:
	{
		uint8_t idx;
		events -= (AT_BLE_READ_AUTHORIZE_REQUEST + 1);
		for (idx = 0; idx < MAX_L2CAP_EVENT_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_l2cap_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_l2cap_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	case AT_BLE_HTPT_CREATE_DB_CFM:
	case AT_BLE_HTPT_ERROR_IND:
	case AT_BLE_HTPT_DISABLE_IND:
	case AT_BLE_HTPT_TEMP_SEND_CFM:
	case AT_BLE_HTPT_MEAS_INTV_CHG_IND:
	case AT_BLE_HTPT_CFG_INDNTF_IND:
	case AT_BLE_HTPT_ENABLE_RSP:
	case AT_BLE_HTPT_MEAS_INTV_UPD_RSP:
	case AT_BLE_HTPT_MEAS_INTV_CHG_REQ:
	{
		uint8_t idx;
		events -= (AT_BLE_LECB_DATA_RECIEVED + 1);
		for (idx = 0; idx < MAX_HTPT_EVENT_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_htpt_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_htpt_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	case AT_BLE_LE_TEST_STATUS:
	case AT_BLE_LE_PACKET_REPORT:
	{
		uint8_t idx;
		events -= (AT_BLE_HTPT_MEAS_INTV_CHG_REQ + 1);
		for (idx = 0; idx < MAX_DTM_EVENT_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_dtm_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_dtm_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	case AT_BLE_CUSTOM_EVENT:
	case AT_BLE_DEVICE_READY:
	case AT_BLE_EVENT_MAX:
	{
		uint8_t idx;
		events -= (AT_BLE_LE_PACKET_REPORT + 1);
		for (idx = 0; idx < MAX_CUSTOM_EVENT_SUBSCRIBERS; idx++)
		{
			if (ble_mgr_custom_event_cb[idx] != NULL)
			{
				const ble_event_callback_t *event_cb_fn = ble_mgr_custom_event_cb[idx];
				if(event_cb_fn[events] != NULL)
				{
					event_cb_fn[events](event_params);
				}
			}
		}
	}
	break;
	
	default:
	{
		DBG_LOG_DEV("BLE-Manager:Unknown Event=0x%X", events);
		DBG_LOG("\r\n");
	}
	break;		
	}
}

/* Advertisement Data will be set based on the advertisement configuration */
at_ble_status_t ble_advertisement_data_set(void)
{
	uint8_t adv_buf[AT_BLE_ADV_MAX_SIZE];
	uint8_t scn_resp[AT_BLE_ADV_MAX_SIZE];
	adv_element_container_t adv_data_element = {0, NULL};
	scan_resp_element_t scan_resp_data_element = {0, NULL};
	adv_data_element.adv_ptr = adv_buf;
	scan_resp_data_element.scn_ptr = scn_resp;
	at_ble_status_t status = AT_BLE_SUCCESS;
	
	#if BLE_GAP_ADV_SERVICE_16BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_16BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = COMPLETE_LIST_16BIT_SERV_UUIDS;
		MREPEAT(SERVICE_UUID16_MAX_NUM, _CONF_SERVICE_16BIT_UUID, &adv_data_element);
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len) 
		{			
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE; 	
		}
	}
	#else
	if(false){}
	#endif
	#if BLE_GAP_ADV_SERVICE_16BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = COMPLETE_LIST_16BIT_SERV_UUIDS;
		MREPEAT(SERVICE_UUID16_MAX_NUM, _CONF_SERVICE_16BIT_UUID_SCAN_RSP, &scan_resp_data_element);
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len) 
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		} 
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add 16-bit UUIDs");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_32BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_32BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = COMPLETE_LIST_32BIT_SERV_UUIDS;
		MREPEAT(SERVICE_UUID32_MAX_NUM, _CONF_SERVICE_32BIT_UUID, &adv_data_element)
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if BLE_GAP_ADV_SERVICE_32BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = COMPLETE_LIST_32BIT_SERV_UUIDS;
		MREPEAT(SERVICE_UUID32_MAX_NUM, _CONF_SERVICE_32BIT_UUID_SCAN_RSP, &adv_data_element)
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add List of 32-bit Service Class UUIDs");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_128BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_128BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = COMPLETE_LIST_128BIT_SERV_UUIDS;
		MREPEAT(SERVICE_UUID128_MAX_NUM, _CONF_SERVICE_128BIT_UUID, &adv_data_element)
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if BLE_GAP_ADV_SERVICE_128BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = COMPLETE_LIST_128BIT_SERV_UUIDS;
		MREPEAT(SERVICE_UUID128_MAX_NUM, _CONF_SERVICE_128BIT_UUID_SCAN_RSP, &adv_data_element)
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add List of 128-bit Service Class UUIDs");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SHORTENED_LOCAL_NAME_ENABLE
	#if (BLE_GAP_ADV_SHORTENED_LOCAL_NAME_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME_LEN))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME_LEN + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = SHORTENED_LOCAL_NAME;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME, BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME_LEN);
		adv_data_element.len += BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME_LEN;
	}
	#else
	if(false){}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Shortened Local Name");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if (BLE_GAP_ADV_COMPLETE_LOCAL_NAME_ENABLE && !BLE_GAP_ADV_SHORTENED_LOCAL_NAME_ENABLE)
	#if (BLE_GAP_ADV_COMPLETE_LOCAL_NAME_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = COMPLETE_LOCAL_NAME;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME, BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH;
	}
	#else
	if(false){}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Complete local name");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if ( BLE_GAP_ADV_SHORTENED_LOCAL_NAME_ENABLE && BLE_GAP_ADV_COMPLETE_LOCAL_NAME_ENABLE )
	#if ((BLE_GAP_ADV_SHORTENED_LOCAL_NAME_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SHORTENED_LOCAL_NAME_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH))){
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = COMPLETE_LOCAL_NAME;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME, BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH;
	}
	#else
	if(false){}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Complete local name");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_TX_POWER_ENABLE
	#if (BLE_GAP_ADV_TX_POWER_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if(adv_data_element.len <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_TX_POWER_SIZE))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_TX_POWER_SIZE + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = TX_POWER_LEVEL;
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_TX_POWER;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_TX_POWER_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_TX_POWER_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if(scan_resp_data_element.len <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_TX_POWER_SIZE))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_TX_POWER_SIZE + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = TX_POWER_LEVEL;
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_TX_POWER;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Tx Power");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_ENABLE
	#if (BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH + BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH + BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = SLAVE_CONNECTION_INTERVAL_RANGE;
		adv_buf[adv_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MIN;
		adv_buf[adv_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MIN >> 8);
		adv_buf[adv_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MAX;
		adv_buf[adv_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MAX >> 8);
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH + BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH + BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = SLAVE_CONNECTION_INTERVAL_RANGE;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MIN;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MIN >> 8);
		scn_resp[scan_resp_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MAX;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MAX >> 8);
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to Slave connection interval range");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = LIST_16BIT_SERV_SOLICITATION_UUIDS;
		MREPEAT(SERVICE_UUID16_MAX_NUM, _CONF_SERVICE_SOLTN_16BIT_UUID, &adv_data_element);
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = LIST_16BIT_SERV_SOLICITATION_UUIDS;
		MREPEAT(SERVICE_UUID16_MAX_NUM, _CONF_SERVICE_SOLTN_16BIT_UUID_SCAN_RSP, &scan_resp_data_element);
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add List of 16-bit Service Solicitation UUIDs");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = LIST_32BIT_SERV_SOLICITATION_UUIDS;
		MREPEAT(SERVICE_UUID32_MAX_NUM, _CONF_SERVICE_SOLTN_32BIT_UUID, &adv_data_element)
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = LIST_32BIT_SERV_SOLICITATION_UUIDS;
		MREPEAT(SERVICE_UUID32_MAX_NUM, _CONF_SERVICE_SOLTN_32BIT_UUID_SCAN_RSP, &scan_resp_data_element)
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add List of 32-bit Service Solicitation UUIDs");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = LIST_128BIT_SERV_SOLICITATION_UUIDS;
		MREPEAT(SERVICE_UUID128_MAX_NUM, _CONF_SERVICE_SOLTN_128BIT_UUID, &adv_data_element)
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = LIST_128BIT_SERV_SOLICITATION_UUIDS;
		MREPEAT(SERVICE_UUID128_MAX_NUM, _CONF_SERVICE_SOLTN_128BIT_UUID_SCAN_RSP, &scan_resp_data_element)
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add List of 128-bit Service Solicitation UUIDs");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = SERVICE_DATA;
		adv_buf[adv_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_SERVICE_DATA_16BIT_UUID;
		adv_buf[adv_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_SERVICE_DATA_16BIT_UUID >> 8);
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID, BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = SERVICE_DATA;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_SERVICE_DATA_16BIT_UUID;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_SERVICE_DATA_16BIT_UUID >> 8);
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID, BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add service data of 16bits");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = SERVICE_DATA_32BIT;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_SERVICE_DATA_32BIT_UUID, BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID, BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = SERVICE_DATA_32BIT;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_DATA_SERVICE_DATA_32BIT_UUID, BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID, BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add service data of 32bits");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_ENABLE
	#if (BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = SERVICE_DATA_128BIT;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_SERVICE_DATA_128BIT_UUID, BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID, BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = SERVICE_DATA_128BIT;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_DATA_SERVICE_DATA_128BIT_UUID, BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID, BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add service data of 128bits");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_PUBLIC_TARGET_ADDR_ENABLE
	#if (BLE_GAP_ADV_PUBLIC_TARGET_ADDR_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = PUBLIC_TARGET_ADDRESS;
		MREPEAT(PUBLIC_TARGET_ADDR_MAX_NUM, _CONF_PUBLIC_TARGET_ADDR, &adv_data_element);
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_PUBLIC_TARGET_ADDR_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_PUBLIC_TARGET_ADDR_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = PUBLIC_TARGET_ADDRESS;
		MREPEAT(PUBLIC_TARGET_ADDR_MAX_NUM, _CONF_PUBLIC_TARGET_ADDR_SCAN_RSP, &scan_resp_data_element);
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Public target addresses");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_RANDOM_TARGET_ADDR_ENABLE
	#if (BLE_GAP_ADV_RANDOM_TARGET_ADDR_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t adv_element_len;
		adv_buf[adv_data_element.len] = adv_data_element.len;
		length_field_ind = adv_data_element.len;
		adv_data_element.len++;
		adv_buf[adv_data_element.len++] = RANDOM_TARGET_ADDRESS;
		MREPEAT(PUBLIC_RANDOM_ADDR_MAX_NUM, _CONF_RANDOM_TARGET_ADDR, &adv_data_element);
		adv_element_len = adv_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(adv_element_len)
		{
			adv_buf[length_field_ind] = adv_element_len+ADV_TYPE_SIZE;
		}
		else
		{
			adv_data_element.len = adv_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_RANDOM_TARGET_ADDR_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_RANDOM_TARGET_ADDR_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH))) {
		uint8_t length_field_ind;
		uint8_t scan_resp_element_len;
		scn_resp[scan_resp_data_element.len] = scan_resp_data_element.len;
		length_field_ind = scan_resp_data_element.len;
		scan_resp_data_element.len++;
		scn_resp[scan_resp_data_element.len++] = RANDOM_TARGET_ADDRESS;
		MREPEAT(PUBLIC_RANDOM_ADDR_MAX_NUM, _CONF_RANDOM_TARGET_ADDR_SCAN_RSP, &scan_resp_data_element);
		scan_resp_element_len = scan_resp_data_element.len - (length_field_ind + ADV_ELEMENT_SIZE);
		if(scan_resp_element_len)
		{
			scn_resp[length_field_ind] = scan_resp_element_len + ADV_TYPE_SIZE;
		}
		else
		{
			scan_resp_data_element.len = scan_resp_data_element.len - ADV_ELEMENT_SIZE;
		}
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Random target addresses");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_APPEARANCE_ENABLE
	#if (BLE_GAP_ADV_APPEARANCE_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_APPEARANCE_SIZE))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_APPEARANCE_SIZE + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = APPEARANCE;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_APPEARANCE, BLE_GAP_ADV_DATA_APPEARANCE_SIZE);
		adv_data_element.len += BLE_GAP_ADV_DATA_APPEARANCE_SIZE;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_APPEARANCE_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_APPEARANCE_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_APPEARANCE_SIZE))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_APPEARANCE_SIZE + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = APPEARANCE;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_DATA_APPEARANCE, BLE_GAP_ADV_DATA_APPEARANCE_SIZE);
		scan_resp_data_element.len += BLE_GAP_ADV_DATA_APPEARANCE_SIZE;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Appearance");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_ADVERTISING_INTERVAL_ENABLE
	#if (BLE_GAP_ADV_ADVERTISING_INTERVAL_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADVERTISING_INTERVAL_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_ADVERTISING_INTERVAL_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = ADVERTISING_INTERVAL;
		adv_buf[adv_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_ADVERTISING_INTERVAL;
		adv_buf[adv_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_ADVERTISING_INTERVAL >> 8);
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_ADVERTISING_INTERVAL_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_ADVERTISING_INTERVAL_SCN_RSP_ENABLE) == SCAN_RESPONSE_ONLY_ENABLE)
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_ADVERTISING_INTERVAL_LENGTH))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_ADVERTISING_INTERVAL_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = ADVERTISING_INTERVAL;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)BLE_GAP_ADV_DATA_ADVERTISING_INTERVAL;
		scn_resp[scan_resp_data_element.len++] = (uint8_t)(BLE_GAP_ADV_DATA_ADVERTISING_INTERVAL >> 8);
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Advertisement interval");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_LE_BT_DEVICE_ADDR_ENABLE
	#if (BLE_GAP_ADV_LE_BT_DEVICE_ADDR_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = LE_BLUETOOTH_DEVICE_ADDRESS;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR, BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH);
		adv_data_element.len += BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_LE_BT_DEVICE_ADDR_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_LE_BT_DEVICE_ADDR_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = LE_BLUETOOTH_DEVICE_ADDRESS;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR, BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH);
		scan_resp_data_element.len += BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Bluetooth device addresses");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_LE_ROLE_ENABLE
	#if (BLE_GAP_ADV_LE_ROLE_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_LE_ROLE_SIZE))) {
		adv_buf[adv_data_element.len++] = ADV_TYPE_SIZE + BLE_GAP_ADV_DATA_LE_ROLE_SIZE;
		adv_buf[adv_data_element.len++] = LE_ROLE;
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_LE_ROLE;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_LE_ROLE_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_LE_ROLE_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_LE_ROLE_SIZE))) {
		scn_resp[scan_resp_data_element.len++] = ADV_TYPE_SIZE + BLE_GAP_ADV_DATA_LE_ROLE_SIZE;
		scn_resp[scan_resp_data_element.len++] = LE_ROLE;
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_LE_ROLE;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add LE role");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	#if BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_ENABLE
	#if (BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_SCN_RSP_ENABLE != SCAN_RESPONSE_ONLY_ENABLE)
	if((adv_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_TYPE_FLAG_SIZE + ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE))) {
		adv_buf[adv_data_element.len++] = BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE + ADV_TYPE_SIZE;
		adv_buf[adv_data_element.len++] = MANUFACTURER_SPECIFIC_DATA;
		memcpy(&adv_buf[adv_data_element.len], BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA, BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE);
		adv_data_element.len += BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE;
	}
	#else
	if(false){}
	#endif
	#if ((BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_SCN_RSP_ENABLE == SCAN_RESPONSE_ENABLE) || (BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_SCN_RSP_ENABLE == SCAN_RESPONSE_ONLY_ENABLE))
	else if((scan_resp_data_element.len) <= (AT_BLE_ADV_MAX_SIZE - (ADV_ELEMENT_SIZE + BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE))) {
		scn_resp[scan_resp_data_element.len++] = BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE + ADV_TYPE_SIZE;
		scn_resp[scan_resp_data_element.len++] = MANUFACTURER_SPECIFIC_DATA;
		memcpy(&scn_resp[scan_resp_data_element.len], BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA, BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE);
		scan_resp_data_element.len += BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE;
	}
	#endif
	else {
		DBG_LOG_ADV("Failed to add Manufacturer specific data");
		return AT_BLE_GAP_INVALID_PARAM;
	}
	#endif
	
	if (at_ble_adv_data_set(adv_buf, adv_data_element.len, scn_resp,
	scan_resp_data_element.len) != AT_BLE_SUCCESS) {
		DBG_LOG("BLE Advertisement data set failed");
		return status;
		} else {
		DBG_LOG_DEV("BLE Advertisement data set success");
		return AT_BLE_SUCCESS;
	}
}



