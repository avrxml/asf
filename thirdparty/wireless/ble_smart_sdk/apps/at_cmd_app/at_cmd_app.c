/**
 * \file
 *
 * \brief AT CMD Application
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the iBeacon Application
 */
/*- Includes ---------------------------------------------------------------*/

#include "at_cmd_app.h"
#include "at_ble_api.h"
#include <asf.h>
#include <string.h>
#include <conf_console_serial.h>
#include "platform.h"
#include "console_serial.h"
#include "led.h"
#include "button.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#define UART_CONFIG_ISR_CHR_LEN		1

#define UART_READ(buff)	uart_read_buffer_wait(UART_CONFIG_MODULE,buff, UART_CONFIG_ISR_CHR_LEN);

#define UART_CONFIG_MODULE			UART_HW_MODULE_UART1	
enum uart_hw_module{
	UART_HW_MODULE_UART1 = 1,
	UART_HW_MODULE_UART2
};

enum uart_rx_callback {
	/** Callback for Receive FiFo not empty */
	UART_CALLBACK_RX_FIFO_NOT_EMPTY,
	/** Callback for Receive FiFo Quarter full */
	UART_CALLBACK_RX_FIFO_QUARTER_FULL,
	/** Callback for Receive FiFo Half full */
	UART_CALLBACK_RX_FIFO_HALF_FULL,
	/** Callback for Receive FiFo Three Quarter full */
	UART_CALLBACK_RX_FIFO_THREE_QUARTER_FULL,
	#  if !defined(__DOXYGEN__)
	/** Number of available callbacks. */
	UART_RX_CALLBACK_N,
	#  endif
};

/** Type definition for an GPIO/PORT pin interrupt module callback function. */
typedef void (*portint_callback_t)(void);

void callback_uart_rx(uint8_t input);
void callback_btn(void);
void callback_resume(void);
void init_globals(void);

uint8_t gau8UartRxBuffer;
static tstrAt_cmd_content	gstrAt_cmdContent;	
static strAtCMD_Handler		gastrAtCMD_Handler[AT_MAX_COMMANDS_COUNT];

volatile static uint8_t		gau8DataBuffer[AT_MAX_RX_BUFF_SIZE];
volatile static uint8_t		gau8DataBufferIndex;

volatile static uint8_t		gu8CmdStatus		= AT_CMD_STATUS_IDLE;
volatile static uint8_t 	gu8InvalidPrinted   = 1;
volatile static int8_t 		gs8CmdIndex			= -1;
volatile static uint8_t 	carriage_return 	= 0;

volatile uint32_t 	event_pool_memory[256] 		= {0};
volatile uint32_t 	event_params_memory[1024] 	= {0};
extern bool button_debounce;


void callback_uart_rx(uint8_t input)
{
	send_plf_int_msg_ind(RAM_ISR_TABLE_UARTRX0_INDEX,UART_CALLBACK_RX_FIFO_HALF_FULL,NULL,0);
	switch(gu8CmdStatus)
	{
		case AT_CMD_STATUS_IDLE:
		{
			switch(strlen((const char *)gau8DataBuffer))
			{
				case 0:
				{
					if('A' != input)
					{
						goto INVALID_CH;
					}
				}//End case 0 of strlen(gau8DataBuffer)
				break;
				case 1:
				{
					if('T' != input) 
					{
						goto INVALID_CH;
					}
				}//End case 1 of strlen(gau8DataBuffer)
				break;
				case 2:
				{
					if('+' != input)
					{
						goto INVALID_CH;
					}	
				}//End case 2 of strlen(gau8DataBuffer)
				break;
			}
			switch(input)
			{
				case '\r':
					carriage_return = true;
				case '\n':
				{
					if(carriage_return == true)
					{
						carriage_return = false;
						if(gu8CmdStatus != AT_CMD_STATUS_CHECK)
						{
							gu8CmdStatus = AT_CMD_STATUS_CHECK;
							if(0 == strlen((const char *)gau8DataBuffer))
							{
								gu8InvalidPrinted = false;
							}
						}
					} 
					else
					{
						gau8DataBuffer[gau8DataBufferIndex++] = input;	
					}
					goto EXIT;
				}
				default:
				{
					gau8DataBuffer[gau8DataBufferIndex++] = input;
					goto EXIT;
				}	
			}
		}
		default:
		{
			goto EXIT;
		}
	}
	
INVALID_CH:
	gu8InvalidPrinted = 0;
EXIT:
	gau8UartRxBuffer = input;
	register_uart_callback(callback_uart_rx);
	return;
}

void callback_btn(void)
{
	send_plf_int_msg_ind(RAM_ISR_TABLE_PORT1_COMB_INDEX,PORTINT_CALLBACK_TYPE_DETECT,NULL,0);
}

void callback_resume(void)
{
	serial_console_init();
	gpio_init();
	button_init();
	button_register_callback(callback_btn);
	led_init();
}

void init_globals(void)
{
	memset((uint8_t *) &gstrAt_cmdContent, 0, sizeof(tstrAt_cmd_content));
	memset((uint8_t *) &gastrAtCMD_Handler, 0, sizeof(strAtCMD_Handler));
	memset((uint8_t *) gau8DataBuffer, 0, AT_MAX_RX_BUFF_SIZE);
	
	carriage_return		= 0;
	gu8CmdStatus		= AT_CMD_STATUS_IDLE;
	gu8InvalidPrinted	= 1;
	gau8DataBufferIndex	= 0;
}

at_ble_status_t init_ble_stack(void)
{
	at_ble_status_t enuStatus;
	at_ble_init_config_t pf_cfg;
	at_ble_addr_t addr;
	
	/* BLE core Intialization */
	memset((uint8_t *)event_pool_memory, 0, sizeof(event_pool_memory));
	memset((uint8_t *)event_params_memory, 0, sizeof(event_params_memory));
	memset(&pf_cfg,0,sizeof(pf_cfg));

	pf_cfg.event_mem_pool.memStartAdd		 = (uint8_t *)event_pool_memory;
	pf_cfg.event_mem_pool.memSize			 = sizeof(event_pool_memory);
	pf_cfg.event_params_mem_pool.memStartAdd = (uint8_t *)event_params_memory;
	pf_cfg.event_params_mem_pool.memSize	 = sizeof(event_params_memory);

	enuStatus = at_ble_init(&pf_cfg);
    CHECK_ERROR(enuStatus, __EXIT);
	//Set device name
    enuStatus = at_ble_device_name_set((uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    DBG_LOG("Device name   : %s", DEVICE_NAME);
	CHECK_ERROR(enuStatus, __EXIT);
	//Get device address
	enuStatus = at_ble_addr_get(&addr);
	DBG_LOG("Device Address: ");
    CHECK_ERROR(enuStatus, __EXIT);
    DBG_LOG("0x%02X%02X%02X%02X%02X%02X,", addr.addr[5], addr.addr[4], addr.addr[3],
		 addr.addr[2], addr.addr[1], addr.addr[0]);
	DBG_LOG("Type : %d", addr.type);
	
__EXIT:
	DBG_LOG(">>");
	return enuStatus;
}

int main(void)
{
	uint16_t plf_event_type;
	uint8_t plf_event_data[16];
	uint16_t plf_event_data_len;
	at_ble_events_t event;
	uint8_t params[100] = {0};
	uint32_t loopCntr = 0;
	init_globals();
	platform_driver_init();
	register_resume_callback(callback_resume);
	
	serial_console_init();
	register_uart_callback(callback_uart_rx);
	DBG_LOG("UART Initialized\r\n");
	
	gpio_init();
	button_init();
	button_register_callback(callback_btn);
	led_init();
	
	if(AT_BLE_SUCCESS != init_ble_stack())
	{
		DBG_LOG("Unable to initialize BLE stack\r\n");
		while(1);
	}
	DBG_LOG("BLE Stack Initialized, use \"AT+\" for AT-Commands\r\n");
	
	init_at_cmd_handler(gastrAtCMD_Handler);
	print_cmd_handler_arr(gastrAtCMD_Handler);
	
	acquire_sleep_lock();
	while(1)
	{
		while(at_ble_event_get(&event, params, (uint32_t)-1) == AT_BLE_SUCCESS)
		{
			switch(event)
			{
				case AT_BLE_SCAN_INFO:
				{
						at_ble_scan_info_t *data = (at_ble_scan_info_t *)((void *)params);
						DBG_LOG("AT_BLE_SCAN_INFO: ");
						DBG_LOG("Device Addr.: 0x%02X%02X%02X%02X%02X%02X",
								data->dev_addr.addr[5], data->dev_addr.addr[4],
								data->dev_addr.addr[3], data->dev_addr.addr[2],
								data->dev_addr.addr[1], data->dev_addr.addr[0]
						);
						DBG_LOG("AddrType    : 0x%02X", data->dev_addr.type);
						DBG_LOG("RSSI        : %d", data->rssi);
				}
				break;
				case AT_BLE_SCAN_REPORT:
				{
					at_ble_scan_report_t *data = (at_ble_scan_report_t *)((void *)params);
					DBG_LOG("AT_BLE_SCAN_REPORT:");
					if (AT_BLE_SUCCESS == data->status)
					{
						DBG_LOG("Scan has been finished");
						DBG_LOG("Status %d", data->status);
					}
				}
				break;
				case AT_PLATFORM_EVENT:
				{
					platform_event_get(&plf_event_type, plf_event_data, &plf_event_data_len);
					platform_event_handler(plf_event_type, plf_event_data, plf_event_data_len);
				}
				break;
				default:
				{
					break;	//Ignore build warning
				}
			}
		}
		printf("Timedout #%05lu\r\n",++loopCntr);
	}
}	

uint8_t platform_event_handler(uint16_t type, uint8_t *data, uint16_t len)
{
	uint8_t u8Status = 0;
	if(type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)| RAM_ISR_TABLE_PORT1_COMB_INDEX))
	{
		DBG_LOG(">>btn");
		LED_Toggle(LED0);
		button_debounce = true;
	}
	else if(type == ((UART_CALLBACK_RX_FIFO_HALF_FULL << 8)| RAM_ISR_TABLE_UARTRX0_INDEX))
	{
		DBG_LOG_CONT("%c",gau8UartRxBuffer);
		switch(gu8CmdStatus)
		{
			case AT_CMD_STATUS_IDLE:
			{
				if(!gu8InvalidPrinted)
				{
					DBG_LOG(":Invalid Input\r\n");
					memset((void *)gau8DataBuffer,0,sizeof(gau8DataBuffer));
					gau8DataBufferIndex = 0;	
					gu8InvalidPrinted = 1;
				}
			}
			break;
			case AT_CMD_STATUS_CHECK:
			{
				int8_t s8Ret = -1;
				DBG_LOG("\r\n");
				memcpy((void *)&gstrAt_cmdContent.au8Cmd[0], (void *)&gau8DataBuffer[strlen(AT_HEADER)], gau8DataBufferIndex);
				gstrAt_cmdContent.u8NumOfParameters = 0;
				s8Ret = get_index_cmdHandler(gstrAt_cmdContent, (strAtCMD_Handler *)&gastrAtCMD_Handler);
				gau8DataBufferIndex = 0;
				if(s8Ret >= 0)
				{
					gs8CmdIndex = s8Ret;
					gu8CmdStatus = AT_CMD_STATUS_RUN;	//Keep processing
				}
				else
				{
					DBG_LOG("Invalid command");
					memset((void *)gau8DataBuffer,0,sizeof(gau8DataBuffer));
					gu8CmdStatus = AT_CMD_STATUS_IDLE;
					break;
				}
			}
			case AT_CMD_STATUS_RUN:
			{
				DBG_LOG("Processing...");
				gs8CmdIndex = gastrAtCMD_Handler[gs8CmdIndex].at_cmd_handler(gstrAt_cmdContent, NULL, 0);
				if(0 == gs8CmdIndex)
				{
					DBG_LOG("DONE\r\n");
					if(strcmp((const char *)gstrAt_cmdContent.au8Cmd, (const char *)gastrAtCMD_Handler[AT_INDEX_INIT].au8AtCMD) == 0)
					{
						u8Status = 0xC0;	//start handling in at_ble_event_get
					}
				}
				else
				{
					DBG_LOG("Failed <%d>", gs8CmdIndex);
				}
				gu8CmdStatus = AT_CMD_STATUS_IDLE;
				memset((void *)gau8DataBuffer,0,sizeof(gau8DataBuffer));
			}
			break;
		}
	}

	return u8Status;
}
