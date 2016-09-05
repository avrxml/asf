/**
 * \file
 *
 * \brief Simple BTLC1000 Beacon Application
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

 /**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Simple BTLC1000 Beacon Application
 */
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "conf_extint.h"

/* Beacon identifier value */
#define BEACON_IDENTIFIER (0x46)

/* Beacon advertisement Interval */
#define BEACON_ADV_INTERVAL (1600) //in 0.625ms

/* Beacon Advertisement timeout */
#define BEACON_ADV_TIMEOUT	(0) //in 0.625ms

/* Beacon Advertise at exact interval */
#define BEACON_ABSOLUTE_INTERVAL_ADV (true)

/* Debug Log printer */
#define DBG_LOG printf("\r\n");\
				printf
				
/* Initialize the BLE */
static void ble_init(void);

/* LED Status to user */

/* BLE connected event indication */
static void ble_device_connected_ind(void);

/* BLE disconnected event indication */
static void ble_device_disconnected_ind(void);

/* BLE data send event confirmation */
static void ble_data_sent_confim(void);

/* Set BLE Address, If address is NULL then it will use BD public address */
static void ble_set_address(at_ble_addr_t *addr);

/* Beacon intialization */
static void beacon_init(void);

/* Advertisement data */
static uint8_t adv_data[] = {0x1a, 0xff, 0x4c, 0x00, 0x02, 0x15, 0x21, 0x8A,
	                         0xF6, 0x52, 0x73, 0xE3, 0x40, 0xB3, 0xB4, 0x1C,
	                         0x19, 0x53, 0x24, 0x2C, 0x72, 0xf4, 0x00, 0xbb,
                             0x00, 0x45, 0xc5};

/* scan response data */
static uint8_t scan_rsp_data[] = {0x11, 0x07, 0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00,
	                              0x37, 0xaa, 0xe3, 0x11, 0x2a, 0xdc, 0x00, 0xcd,
                                  0x30, 0x57};
								  
uint8_t db_mem[1024] = {0};							  

/* service UUID definition */
static at_ble_uuid_t service_uuid = {AT_BLE_UUID_128 ,
	{0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0x37, 0xaa,
	 0xe3, 0x11, 0x2a, 0xdc, 0x00, 0xcd, 0x30, 0x57}};

/* characteristics definitions */
static at_ble_characteristic_t chars[] = {
	{
		0, /* handle stored here */
		{	AT_BLE_UUID_128, {0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0x3b, 0x8e,
								0xe3, 0x11, 0x2a, 0xdc, 0xa0, 0xd3, 0x20, 0x8e}}, /* UUID */
			AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE | AT_BLE_CHAR_NOTIFY, /* Properties */
			(uint8_t *)"char1", sizeof("char1"), 100, /* value */
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR, /* permissions */

			NULL, 0, 0, /* user defined name */
			NULL, /* presentation format */
			AT_BLE_ATTR_NO_PERMISSIONS,
			AT_BLE_ATTR_NO_PERMISSIONS,
			AT_BLE_ATTR_NO_PERMISSIONS,
			0,0,0, /* Handles */
	},
			
	{
		0, /* handle stored here */
		{	AT_BLE_UUID_128, {0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0xff, 0xbb,
			0xe3, 0x11, 0x2a, 0xdc, 0xe0, 0x40, 0xd2, 0x98}}, /* UUID */
			AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE | AT_BLE_CHAR_RELIABLE_WRITE, /* Properties */
			(uint8_t *)"char2", sizeof("char1"), 100, /* value */
			AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR, /* permissions */

			NULL, 0, 0, /* user defined name */
			NULL, /* presentation format */
			AT_BLE_ATTR_NO_PERMISSIONS,
			AT_BLE_ATTR_NO_PERMISSIONS,
			AT_BLE_ATTR_NO_PERMISSIONS,
			0,0,0, /* Handles */
	}
};

at_ble_init_config_t pf_cfg;

/* Set BLE Address, If address is NULL then it will use BD public address */
static void ble_set_address(at_ble_addr_t *addr)
{
	at_ble_addr_t address = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
	at_ble_addr_t *address_ptr = addr;
	
	if (addr == NULL)
	{
		/* get BD address from BLE device */
		if(at_ble_addr_get(&address) != AT_BLE_SUCCESS)
		{
			DBG_LOG("BD address get failed");
		}
		address_ptr = &address;
	}
	
	/* set the BD address */
	if(at_ble_addr_set(address_ptr) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BD address set failed");
	}
	
	DBG_LOG("BD Address:0x%02X%02X%02X%02X%02X%02X, Address Type:%d",
	address_ptr->addr[5],
	address_ptr->addr[4],
	address_ptr->addr[3],
	address_ptr->addr[2],
	address_ptr->addr[1],
	address_ptr->addr[0], address_ptr->type);
}


/* BLE connected event indication */
static void ble_device_connected_ind(void)
{
	/* Switch on the application LED */
	LED_On(LED0);
}

/* BLE disconnected event indication */
static void ble_device_disconnected_ind(void)
{
	/* Switch off the application LED */
	LED_Off(LED0);
}

/* BLE data send event confirmation */
static void ble_data_sent_confim(void)
{
	/* Toggle the application LED of each data sent */
	LED_Toggle(LED0);
}

/* Initialize the BLE */
static void ble_init(void)
{
	/* Initialize the platform */
	DBG_LOG("Initializing BTLC1000");
	
	/*Memory allocation required by GATT Server DB*/
	pf_cfg.memPool.memSize = sizeof(db_mem);
	pf_cfg.memPool.memStartAdd = &(db_mem[0]);
	
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
	
	/* Init BLE device */
	if(at_ble_init(&pf_cfg) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BTLC1000 Initialization failed");
		DBG_LOG("Please check the power and connection / hardware connector");
		while(1);
	}
	if (at_ble_calib_config(0, 2, 2) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Auto-Calibration Disable cmd Failed");
	}
	
}

/* Define service, advertisement data set and start advertisement */
static void beacon_init(void)
{
	static at_ble_handle_t service;

	/* establish peripheral database */
	if (at_ble_primary_service_define(&service_uuid, &service,
		NULL, 0, chars, 2) !=  AT_BLE_SUCCESS)
	{
		DBG_LOG("Failed to define the primary service");
	}
	
	/* set beacon advertisement data */
	if(at_ble_adv_data_set(adv_data, sizeof(adv_data), scan_rsp_data, sizeof(scan_rsp_data)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Beacon advertisement data set failed");
	}
	
	/* BLE start advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
	   BEACON_ADV_INTERVAL, BEACON_ADV_TIMEOUT, BEACON_ABSOLUTE_INTERVAL_ADV) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Beacon advertisement failed");
		ble_device_disconnected_ind();
	}
	else
	{
		DBG_LOG("Advertisement started");		
		ble_device_connected_ind();
	}
	
}

int main (void)
{
	at_ble_handle_t handle;
	at_ble_events_t event;
	uint8_t params[512];

#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
#elif SAM0
	system_init();
#endif
	
	/* Initialize serial console */
	serial_console_init();
	
	DBG_LOG("Initializing Beacon Application");
	
	/* initialize the BLE chip */
	ble_init();
	
	/* set its own public address */
	ble_set_address(NULL);
	
	/* Initialize the Beacon advertisement */
	beacon_init();
	
	DBG_LOG("BLE Beacon Initialization completed");
	
	while(at_ble_event_get(&event, params, 0xFFFFFFFF) == AT_BLE_SUCCESS)
	{
		switch(event)
		{
			case AT_BLE_CONNECTED:
			{
				at_ble_connected_t conn_params;
				memcpy((uint8_t *)&conn_params, params, sizeof(at_ble_connected_t));
				
				DBG_LOG("Device connected to 0x%02x%02x%02x%02x%02x%02x handle=0x%x",
				conn_params.peer_addr.addr[5],
				conn_params.peer_addr.addr[4],
				conn_params.peer_addr.addr[3],
				conn_params.peer_addr.addr[2],
				conn_params.peer_addr.addr[1],
				conn_params.peer_addr.addr[0],
				conn_params.handle);
				handle = conn_params.handle;
				
				DBG_LOG("Advertisement stopped");
				
				/* BLE device connected, indicate to user  that beacon is stopped*/
				ble_device_disconnected_ind();
				
				/* keep compiler happy */
				handle = handle;
				
			}
			break;

			case AT_BLE_DISCONNECTED:
			{
				at_ble_disconnected_t disconnect;
				memcpy((uint8_t *)&disconnect, params, sizeof(at_ble_disconnected_t));
				
				DBG_LOG("Device disconnected Reason:0x%02x Handle=0x%x", disconnect.reason, disconnect.handle);
				
				/* BLE start advertisement */
				if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
					BEACON_ADV_INTERVAL, BEACON_ADV_TIMEOUT, BEACON_ABSOLUTE_INTERVAL_ADV) != AT_BLE_SUCCESS)
				{
					DBG_LOG("BLE Beacon advertisement failed");
					ble_device_disconnected_ind();
				}
				else
				{
					DBG_LOG("Advertisement started");
					/* BLE device disconnected, indicate to user  beacon advertisement started */
					ble_device_connected_ind();					
				}						
			}
			break;

			
			default:
			/* All  other events */
				ble_data_sent_confim();
			break;
		}
	}
}
