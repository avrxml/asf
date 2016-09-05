/**
 * \file
 *
 * \brief iBeacon Application
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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
 * This is the reference manual for the iBeacon Application
 */
/*- Includes ---------------------------------------------------------------*/

#include "at_ble_api.h"
#include <asf.h>
#include <string.h>
#include <conf_console_serial.h>
#include "platform.h"
#include <common.h>
#include <spi_flash.h>
#ifndef NULL
#define NULL ((void *)0)
#endif


#define PRINT(...)                  printf(__VA_ARGS__)
#define PRINT_H1(...)               printf("[APP]>> "/**/__VA_ARGS__)
#define PRINT_H2(...)               printf("\t>> "/**/__VA_ARGS__)
#define PRINT_H3(...)               printf("\t\t>> "/**/__VA_ARGS__)


// Error Checking
#define CHECK_ERROR(VAR,LABEL)  do{if(AT_BLE_SUCCESS != VAR){goto LABEL;}}while(0)


#define APP_STACK_SIZE	(1024)
#define BEACON_IDENTIFIER (0x13)
static uint8_t adv_data[31];
static uint8_t scan_rsp_data[31];
struct uart_module uart_instance;
at_ble_init_config_t pf_cfg;
volatile unsigned char app_stack_patch[APP_STACK_SIZE];

volatile uint8_t 	event_pool_memory[256] 		= {0};
volatile uint8_t 	event_params_memory[1024] 	= {0};

void ble_init(void);
int main(void);

static void configure_uart(void)
{
	struct uart_config config_uart;

	system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);

	uart_get_config_defaults(&config_uart);

	config_uart.baud_rate = 115200;
	config_uart.pin_number_pad[0] = EDBG_CDC_PIN_PAD0;
	config_uart.pin_number_pad[1] = EDBG_CDC_PIN_PAD1;
	config_uart.pin_number_pad[2] = EDBG_CDC_PIN_PAD2;
	config_uart.pin_number_pad[3] = EDBG_CDC_PIN_PAD3;
	
	config_uart.pinmux_sel_pad[0] = EDBG_CDC_MUX_PAD0;
	config_uart.pinmux_sel_pad[1] = EDBG_CDC_MUX_PAD1;
	config_uart.pinmux_sel_pad[2] = EDBG_CDC_MUX_PAD2;
	config_uart.pinmux_sel_pad[3] = EDBG_CDC_MUX_PAD3;

	stdio_serial_init(&uart_instance, CONF_STDIO_USART_MODULE, &config_uart);
}

static void resume_cb(void)
{
	init_port_list();
	configure_uart();
	spi_flash_turn_off();//for power consumption.
}

static at_ble_status_t app_init(void)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_addr_t addr = {AT_BLE_ADDRESS_PUBLIC, {0x45, 0x75, 0x11, 0x6a, 0x7f, 0x7f} };
	const uint8_t ro_adv_data[] = {0x1a, 0xff,
							0x4c, 0x00,			//Company
							0x02,							//Type
							0x15,							//Length
							0x21, 0x8A, 0xF6, 0x52, 0x73, 0xE3, 0x40, 0xB3, 0xB4, 0x1C, 0x19, 0x53, 0x24, 0x2C, 0x72, 0xf4,			//UUID
							0x00, 0xbb,			//Major
							0x00, 0x45,				//Minor
							0xc5};							//RSSI at 1m

	const uint8_t ro_scan_rsp_data[] = {0x11, 0x07, 0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00,
					    0x37, 0xaa, 0xe3, 0x11, 0x2a, 0xdc, 0x00, 0xcd,
					    0x30, 0x57};
	do
	{
		addr.addr[0] = BEACON_IDENTIFIER;
		
		memset(adv_data,0,sizeof(adv_data));
		memset(scan_rsp_data,0,sizeof(scan_rsp_data));
		memcpy(adv_data,ro_adv_data,sizeof(ro_adv_data));
		memcpy(scan_rsp_data,ro_scan_rsp_data,sizeof(ro_scan_rsp_data));
		
		memset((uint8_t *)event_pool_memory, 0, sizeof(event_pool_memory));
		memset((uint8_t *)event_params_memory, 0, sizeof(event_params_memory));
		
		memset(&pf_cfg,0,sizeof(pf_cfg));

		pf_cfg.event_mem_pool.memStartAdd        = (uint8_t *)event_pool_memory;
		pf_cfg.event_mem_pool.memSize            = sizeof(event_pool_memory);
		pf_cfg.event_params_mem_pool.memStartAdd = (uint8_t *)event_params_memory;
		pf_cfg.event_params_mem_pool.memSize     = sizeof(event_params_memory);

		// init device
		if((status = at_ble_init(&pf_cfg)) == AT_BLE_SUCCESS)
		{
			status = AT_BLE_FAILURE;
			// Set the device address
			status = at_ble_addr_set(&addr);
			if(AT_BLE_SUCCESS != status)
			{
				break;
			}

			// Set TX Power level
			//status = at_ble_tx_power_set(AT_BLE_TX_PWR_LVL_ZERO_DB);
			
			// start advertising
			adv_data[25] = BEACON_IDENTIFIER;
			status = at_ble_adv_data_set((uint8_t *)adv_data, sizeof(ro_adv_data), (uint8_t *)scan_rsp_data, sizeof(ro_scan_rsp_data));
			if(AT_BLE_SUCCESS != status)
			{
				break;
			}
			
			status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 1600, 0, 0);
		}
	}while(0);
	return status;
}


static uint8_t params[100];
int main(void)
{
	at_ble_events_t event;
	at_ble_status_t status = AT_BLE_SUCCESS;

	memset(params, 0, sizeof(params));

	platform_driver_init();
	configure_uart();
	status = app_init();
	CHECK_ERROR(status, __EXIT);
	PRINT_H1("Init done\r\n");
	
	register_resume_callback(resume_cb);
	release_sleep_lock();
	

	while((status = at_ble_event_get(&event, params, (uint32_t)-1)) == AT_BLE_SUCCESS)
	{
		acquire_sleep_lock();
		switch(event)
		{
			case AT_BLE_CONNECTED:
			{
				volatile at_ble_connected_t *conn_params = (at_ble_connected_t *)((void *)params);
				PRINT_H1("AT_BLE_CONNECTED:\r\n");
				if (AT_BLE_SUCCESS == conn_params->conn_status)
				{
					PRINT_H2("Device connected:\r\n");
					PRINT_H3("Conn. handle : 0x%04X\r\n", conn_params->handle);
					PRINT_H3("Address      : 0x%02X%02X%02X%02X%02X%02X\r\n",
						conn_params->peer_addr.addr[5],
						conn_params->peer_addr.addr[4],
						conn_params->peer_addr.addr[3],
						conn_params->peer_addr.addr[2],
						conn_params->peer_addr.addr[1],
						conn_params->peer_addr.addr[0]
					);
				PRINT_H3("Conn.Interval: 0x%04X\r\n", conn_params->conn_params.con_interval);
				PRINT_H3("Conn. Latency: 0x%04X\r\n", conn_params->conn_params.con_latency);
				PRINT_H3("Supr. Timeout: 0x%04X\r\n", conn_params->conn_params.sup_to);
				}
				else
				{
					PRINT_H2("Unable to connect to device:\r\n");
					PRINT_H3("Status : %d\r\n", conn_params->conn_status);
				}
			}
			break;
		case AT_BLE_MTU_CHANGED_INDICATION:
		{
			at_ble_mtu_changed_ind_t *args = (at_ble_mtu_changed_ind_t *)((void *)params);
			PRINT_H1("AT_BLE_MTU_CHANGED_INDICATION\r\n");
			PRINT_H2("New MTU for Conn. Handle 0x%02X is 0x%04X\r\n", args->conhdl, args->mtu_value);
		}
		break;
		case AT_BLE_CONN_PARAM_UPDATE_DONE:
		{
			at_ble_conn_param_update_done_t *args = (at_ble_conn_param_update_done_t *)((void *)params);
			PRINT_H1("AT_BLE_CONN_PARAM_UPDATE_DONE\r\n");
			PRINT_H2("New Parameters Update for Conn. handle 0x%02X:\r\n", args->handle);
			PRINT_H3("Conn. Interval    : 0x%04X\r\n",args->con_intv);
			PRINT_H3("Conn. Latency     : 0x%04X\r\n",args->con_latency);
			PRINT_H3("Conn. Sup. Timeout: 0x%04X\r\n",args->superv_to);
		}
		break;
			case AT_BLE_DISCONNECTED:
			{
				at_ble_disconnected_t *disconn_params = (at_ble_disconnected_t *)((void *)params);
				PRINT_H1("AT_BLE_DISCONNECTED:\r\n");
				PRINT_H2("Device disconnected:\r\n");
				PRINT_H3("Conn. handle : 0x%04X\r\n", disconn_params->handle);
				PRINT_H3("Reason       : 0x%02X\r\n", disconn_params->reason);
				PRINT_H2("Start Advertising again\r\n");
				status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 1600, 0, 0);
				PRINT_H3("Status : %d\r\n", status);
			}
			break;

			default:
				PRINT_H1("default: 0x%04X\r\n",event);
			break;

		}
		release_sleep_lock();
	}

__EXIT:
	return status;
}
