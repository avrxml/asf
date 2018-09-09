/**
* \file  commands.c
*
* \brief WSNDemo command handler implementation
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "asf.h"
#include "config.h"
#include "miwi_api.h"
#include "sysTimer.h"
#include "commands.h"
#include "wsndemo.h"
#if !(SAMD || SAMR21||SAML21||SAMR30)
#if (LED_COUNT > 0)
#include "led.h"
#endif
#endif

/*- Definitions ------------------------------------------------------------*/
#define APP_CMD_UART_BUFFER_SIZE    16
#define APP_CMD_PENDING_TABLE_SIZE  5
#define APP_CMD_INVALID_ADDR        0xffff

/*- Types ------------------------------------------------------------------*/
typedef enum {
	APP_CMD_UART_STATE_IDLE,
	APP_CMD_UART_STATE_SYNC,
	APP_CMD_UART_STATE_DATA,
	APP_CMD_UART_STATE_MARK,
	APP_CMD_UART_STATE_CSUM,
} AppCmdUartState_t;

COMPILER_PACK_SET(1)

typedef struct {
	uint8_t commandId;
	uint64_t dstAddr;
} AppCmdUartHeader_t;

typedef struct {
	uint8_t commandId;
	uint64_t dstAddr;
	uint16_t duration;
	uint16_t period;
} AppCmdUartIdentify_t;

typedef struct {
	uint8_t id;
} AppCmdHeader_t;

typedef struct {
	uint8_t id;
	uint16_t duration;
	uint16_t period;
} AppCmdIdentify_t;

COMPILER_PACK_RESET()
/*- Prototypes -------------------------------------------------------------*/
static void appCmdUartProcess(uint8_t *data, uint8_t size);
static void appCmdBuffer(uint16_t addr, uint8_t *data, uint8_t size);
static void appCmdDataRequest(uint16_t addr, uint8_t size, uint8_t* payload);
static void appCmdDataConf(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer);
static bool appCmdHandle(uint8_t *data, uint8_t size);
static void appCmdIdentifyDurationTimerHandler(SYS_Timer_t *timer);
static void appCmdIdentifyPeriodTimerHandler(SYS_Timer_t *timer);

/*- Variables --------------------------------------------------------------*/
static AppCmdUartState_t appCmdUartState = APP_CMD_UART_STATE_IDLE;
static uint8_t appCmdUartPtr;
static uint8_t appCmdUartBuf[APP_CMD_UART_BUFFER_SIZE];
static uint8_t appCmdUartCsum;
SYS_Timer_t appCmdIdentifyDurationTimer;
SYS_Timer_t appCmdIdentifyPeriodTimer;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void APP_CommandsInit(void)
{
	appCmdIdentifyDurationTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appCmdIdentifyDurationTimer.handler = appCmdIdentifyDurationTimerHandler;

	appCmdIdentifyPeriodTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appCmdIdentifyPeriodTimer.handler = appCmdIdentifyPeriodTimerHandler;
}

/*************************************************************************//**
*****************************************************************************/
void APP_CommandsByteReceived(uint8_t byte)
{
	switch (appCmdUartState) {
	case APP_CMD_UART_STATE_IDLE:
	{
		if (0x10 == byte) {
			appCmdUartPtr = 0;
			appCmdUartCsum = byte;
			appCmdUartState = APP_CMD_UART_STATE_SYNC;
		}
	}
	break;

	case APP_CMD_UART_STATE_SYNC:
	{
		appCmdUartCsum += byte;

		if (0x02 == byte) {
			appCmdUartState = APP_CMD_UART_STATE_DATA;
		} else {
			appCmdUartState = APP_CMD_UART_STATE_IDLE;
		}
	}
	break;

	case APP_CMD_UART_STATE_DATA:
	{
		appCmdUartCsum += byte;

		if (0x10 == byte) {
			appCmdUartState = APP_CMD_UART_STATE_MARK;
		} else {
			appCmdUartBuf[appCmdUartPtr++] = byte;
		}

		if (appCmdUartPtr == APP_CMD_UART_BUFFER_SIZE) {
			appCmdUartState = APP_CMD_UART_STATE_IDLE;
		}
	}
	break;

	case APP_CMD_UART_STATE_MARK:
	{
		appCmdUartCsum += byte;

		if (0x10 == byte) {
			appCmdUartBuf[appCmdUartPtr++] = byte;

			if (appCmdUartPtr == APP_CMD_UART_BUFFER_SIZE) {
				appCmdUartState = APP_CMD_UART_STATE_IDLE;
			} else {
				appCmdUartState = APP_CMD_UART_STATE_DATA;
			}
		} else if (0x03 == byte) {
			appCmdUartState = APP_CMD_UART_STATE_CSUM;
		} else {
			appCmdUartState = APP_CMD_UART_STATE_IDLE;
		}
	}
	break;

	case APP_CMD_UART_STATE_CSUM:
	{
		if (byte == appCmdUartCsum) {
			appCmdUartProcess(appCmdUartBuf, appCmdUartPtr);
		}

		appCmdUartState = APP_CMD_UART_STATE_IDLE;
	}
	break;

	default:
		break;
	}
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdUartProcess(uint8_t *data, uint8_t size)
{
	AppCmdUartHeader_t *header = (AppCmdUartHeader_t *)data;

	if (size < sizeof(AppCmdUartHeader_t)) {
		return;
	}

	if (APP_COMMAND_ID_IDENTIFY == header->commandId) {
		AppCmdUartIdentify_t *uartCmd = (AppCmdUartIdentify_t *)data;
		AppCmdIdentify_t cmd;

		cmd.id = APP_COMMAND_ID_IDENTIFY;
		cmd.duration = uartCmd->duration;
		cmd.period = uartCmd->period;

		appCmdBuffer(header->dstAddr, (uint8_t *)&cmd,
				sizeof(AppCmdIdentify_t));
	}
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdBuffer(uint16_t addr, uint8_t *data, uint8_t size)
{
	if (0 == addr)
	{
		appCmdHandle(data, size);
	}
	else
	{
		appCmdDataRequest(addr, size, data);
	}
}

uint8_t wsnmsghandle;
/*************************************************************************//**
*****************************************************************************/
static void appCmdDataRequest(uint16_t addr, uint8_t size, uint8_t* payload)
{
	MiApp_SendData(SHORT_ADDR_LEN, (uint8_t*)&addr, size, payload, wsnmsghandle, true, appCmdDataConf);
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdDataConf(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer)
{
}

/*************************************************************************//**
*****************************************************************************/
void appCmdDataInd(RECEIVED_MESH_MESSAGE *ind)
{
	appCmdHandle(ind->payload, ind->payloadSize);
}

/*************************************************************************//**
*****************************************************************************/
static bool appCmdHandle(uint8_t *data, uint8_t size)
{
	AppCmdHeader_t *header = (AppCmdHeader_t *)data;

	if (size < sizeof(AppCmdHeader_t)) {
		return false;
	}

	if (APP_COMMAND_ID_IDENTIFY == header->id) {
		AppCmdIdentify_t *req = (AppCmdIdentify_t *)data;

		if (sizeof(AppCmdIdentify_t) != size) {
			return false;
		}

		SYS_TimerStop(&appCmdIdentifyDurationTimer);
		SYS_TimerStop(&appCmdIdentifyPeriodTimer);

		appCmdIdentifyDurationTimer.interval = req->duration;
		appCmdIdentifyDurationTimer.mode = SYS_TIMER_INTERVAL_MODE;
		appCmdIdentifyDurationTimer.handler
			= appCmdIdentifyDurationTimerHandler;
		SYS_TimerStart(&appCmdIdentifyDurationTimer);

		appCmdIdentifyPeriodTimer.interval = req->period;
		appCmdIdentifyPeriodTimer.mode = SYS_TIMER_PERIODIC_MODE;
		appCmdIdentifyPeriodTimer.handler
			= appCmdIdentifyPeriodTimerHandler;
		SYS_TimerStart(&appCmdIdentifyPeriodTimer);
#if (LED_COUNT > 0)
		LED_On(LED_IDENTIFY);
#endif
		return true;
	}
	return false;
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdIdentifyDurationTimerHandler(SYS_Timer_t *timer)
{
#if (LED_COUNT > 0)
	LED_Off(LED_IDENTIFY);
#endif
	SYS_TimerStop(&appCmdIdentifyPeriodTimer);
	(void)timer;
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdIdentifyPeriodTimerHandler(SYS_Timer_t *timer)
{
#if (LED_COUNT > 0)
	LED_Toggle(LED_IDENTIFY);
#endif
	(void)timer;
}
