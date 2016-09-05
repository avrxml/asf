/**
 * \file
 *
 * \brief AT CMD Application
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

//from uart.h keil project
#define UART_CONFIG_ISR_CHR_LEN		1
#ifdef CHIPVERSION_B0
#define UART0_RX_VECTOR_TABLE_INDEX		16
#define UART0_TX_VECTOR_TABLE_INDEX		17
#define UART1_RX_VECTOR_TABLE_INDEX		18
#define UART1_TX_VECTOR_TABLE_INDEX		19
#else
#define UART0_RX_VECTOR_TABLE_INDEX		16
#define UART0_TX_VECTOR_TABLE_INDEX		17
#define UART1_RX_VECTOR_TABLE_INDEX		18
#define UART1_TX_VECTOR_TABLE_INDEX		19
#endif	//CHIPVERSION_B0

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

#ifdef CHIPVERSION_B0
	#define GPIO0_COMBINED_VECTOR_TABLE_INDEX		39
	#define GPIO1_COMBINED_VECTOR_TABLE_INDEX		40
	#define GPIO2_COMBINED_VECTOR_TABLE_INDEX		41
#else
	#define GPIO0_COMBINED_VECTOR_TABLE_INDEX		23
	#define GPIO1_COMBINED_VECTOR_TABLE_INDEX		24
#endif	//CHIPVERSION_B0

/** Enum for the possible callback types for the GPIO/PORT pin interrupt module. */
enum portint_callback_type
{
	/** Callback type for when an external interrupt detects the configured
	 *  channel criteria (i.e. edge or level detection)
	 */
	PORTINT_CALLBACK_TYPE_DETECT = 1,
};


//from uart.h keil project

void callback_uart_rx(uint8_t input);
void callback_btn(void);
void callback_resume(void);
void init_globals(void);

//uint8_t gau8UartRxBuffer[UART_CONFIG_ISR_CHR_LEN+1]; //+1 for NULL terminator
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


void callback_uart_rx(uint8_t input)
{
	//UART_READ(gau8UartRxBuffer);
	send_plf_int_msg_ind(UART0_RX_VECTOR_TABLE_INDEX,UART_CALLBACK_RX_FIFO_HALF_FULL,NULL,0);
	//gau8UartRxBuffer[UART_CONFIG_ISR_CHR_LEN] = 0;
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
							//gau8DataBufferIndex = 0;
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
	send_plf_int_msg_ind(GPIO1_COMBINED_VECTOR_TABLE_INDEX,PORTINT_CALLBACK_TYPE_DETECT,NULL,0);
}

void callback_resume(void)
{
	//uart_config_init(UART_CONFIG_BAUD_RATE);
	//gpio_config_led_init();
	//gpio_config_btn_init(callback_btn);
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
    printf("Device name   : %s\r\n", DEVICE_NAME);
	CHECK_ERROR(enuStatus, __EXIT);
	//Get device address
	enuStatus = at_ble_addr_get(&addr);
	printf("Device Address: ");
    CHECK_ERROR(enuStatus, __EXIT);
    printf("0x%02X%02X%02X%02X%02X%02X,", addr.addr[5], addr.addr[4], addr.addr[3],
		 addr.addr[2], addr.addr[1], addr.addr[0]);
	printf("\tType : %d\r\n", addr.type);
	
__EXIT:
	printf("\r\n>>");
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
	
	//uart_config_init(UART_CONFIG_BAUD_RATE);
	//uart_config_callback(callback_uart_rx, gau8UartRxBuffer, UART_CONFIG_ISR_CHR_LEN); 
	serial_console_init();
	register_uart_callback(callback_uart_rx);

	printf("UART Initialized\r\n");
	
	//gpio_config_btn_init(callback_btn);
	//gpio_config_led_init();
	gpio_init();
	button_init();
	button_register_callback(callback_btn);
	led_init();
	
	if(AT_BLE_SUCCESS != init_ble_stack())
	{
		printf("Unable to initialize BLE stack\r\n");
		while(1);
	}
	printf("BLE Stack Initialized, use \"AT+\" for AT-Commands\r\n");
	
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
						printf("AT_BLE_SCAN_INFO: \r\n");
						printf("\tDevice Addr.: 0x%02X%02X%02X%02X%02X%02X\r\n",
								data->dev_addr.addr[5], data->dev_addr.addr[4],
								data->dev_addr.addr[3], data->dev_addr.addr[2],
								data->dev_addr.addr[1], data->dev_addr.addr[0]
						);
						printf("\tAddrType    : 0x%02X\r\n", data->dev_addr.type);
						printf("\tRSSI        : %d\r\n", data->rssi);
				}
				break;
				case AT_BLE_SCAN_REPORT:
				{
					at_ble_scan_report_t *data = (at_ble_scan_report_t *)((void *)params);
					printf("AT_BLE_SCAN_REPORT:\r\n");
					if (AT_BLE_SUCCESS == data->status)
					{
						printf("Scan has been finished\r\n");
						printf("Status %d\r\n", data->status);
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
	if(type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)| GPIO1_COMBINED_VECTOR_TABLE_INDEX))
	{
		printf(">>btn\r\n");
		LED_Toggle(LED0);
		//GPIO_CONFIG_LED_STATE(LED_TOGGLE);
	}
	else if(type == ((UART_CALLBACK_RX_FIFO_HALF_FULL << 8)| UART0_RX_VECTOR_TABLE_INDEX))
	{
//		#if ECHO
			printf("%c",gau8UartRxBuffer);
//		#endif		
		switch(gu8CmdStatus)
		{
			case AT_CMD_STATUS_IDLE:
			{
				if(!gu8InvalidPrinted)
				{
					printf(":Invalid Input\r\n");
					memset((void *)gau8DataBuffer,0,sizeof(gau8DataBuffer));
					gau8DataBufferIndex = 0;	
					gu8InvalidPrinted = 1;
				}
			}
			break;
			case AT_CMD_STATUS_CHECK:
			{
				int8_t s8Ret = -1;
				printf("\r\n");
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
					//printf("Invalid command\r\n", strlen(AT_HEADER));
					printf("Invalid command\r\n");
					memset((void *)gau8DataBuffer,0,sizeof(gau8DataBuffer));
					gu8CmdStatus = AT_CMD_STATUS_IDLE;
					break;
				}
			}
			case AT_CMD_STATUS_RUN:
			{
				printf("Processing...");
				gs8CmdIndex = gastrAtCMD_Handler[gs8CmdIndex].at_cmd_handler(gstrAt_cmdContent, NULL, 0);
				if(0 == gs8CmdIndex)
				{
					printf("DONE\r\n");
					if(strcmp((const char *)gstrAt_cmdContent.au8Cmd, (const char *)gastrAtCMD_Handler[AT_INDEX_INIT].au8AtCMD) == 0)
					{
						u8Status = 0xC0;	//start handling in at_ble_event_get
					}
				}
				else
				{
					printf("Failed <%d>\r\n", gs8CmdIndex);
				}
				gu8CmdStatus = AT_CMD_STATUS_IDLE;
				memset((void *)gau8DataBuffer,0,sizeof(gau8DataBuffer));
			}
			break;
		}
	}

	return u8Status;
}
