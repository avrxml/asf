/**
 * \file commands.c
 *
 * \brief WSNDemo command handler implementation
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "nwk.h"
#include "sysTimer.h"
#include "commands.h"
#include "wsndemo.h"
#if !(SAMD || SAMR21||SAML21)
#if (LED_COUNT > 0)
#include "led.h"
#endif
#endif
#include "asf.h"

/*- Definitions ------------------------------------------------------------*/
#define APP_CMD_UART_BUFFER_SIZE    16
#define APP_CMD_PENDING_TABLE_SIZE  5
#define APP_CMD_INVALID_ADDR        0xffff

#define APP_CMD_ENDPOINT    2

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

typedef struct {
	uint16_t addr;
	uint8_t size;
	bool ready;
	union {
		uint8_t payload;
		AppCmdHeader_t header;
		AppCmdIdentify_t identify;
	};
} AppCmdPendingTableEntry_t;
COMPILER_PACK_RESET()
/*- Prototypes -------------------------------------------------------------*/
static void appCmdUartProcess(uint8_t *data, uint8_t size);
static void appCmdBuffer(uint16_t addr, uint8_t *data, uint8_t size);
static void appCmdDataRequest(AppCmdPendingTableEntry_t *entry);
static void appCmdDataConf(NWK_DataReq_t *req);
static void appCmdCheckPendingTable(void);
static bool appCmdDataInd(NWK_DataInd_t *ind);
static bool appCmdHandle(uint8_t *data, uint8_t size);
static void appCmdIdentifyDurationTimerHandler(SYS_Timer_t *timer);
static void appCmdIdentifyPeriodTimerHandler(SYS_Timer_t *timer);

/*- Variables --------------------------------------------------------------*/
static AppCmdUartState_t appCmdUartState = APP_CMD_UART_STATE_IDLE;
static uint8_t appCmdUartPtr;
static uint8_t appCmdUartBuf[APP_CMD_UART_BUFFER_SIZE];
static uint8_t appCmdUartCsum;
static SYS_Timer_t appCmdIdentifyDurationTimer;
static SYS_Timer_t appCmdIdentifyPeriodTimer;
static AppCmdPendingTableEntry_t appCmdPendingTable[APP_CMD_PENDING_TABLE_SIZE];
static AppCmdPendingTableEntry_t *appCmdInProgress;
static NWK_DataReq_t appCmdDataReq;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void APP_CommandsInit(void)
{
	appCmdIdentifyDurationTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appCmdIdentifyDurationTimer.handler
		= appCmdIdentifyDurationTimerHandler;

	appCmdIdentifyPeriodTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appCmdIdentifyPeriodTimer.handler = appCmdIdentifyPeriodTimerHandler;

	appCmdInProgress = NULL;
	appCmdDataReq.dstAddr = 0;
	appCmdDataReq.dstEndpoint = APP_CMD_ENDPOINT;
	appCmdDataReq.srcEndpoint = APP_CMD_ENDPOINT;
	appCmdDataReq.options = NWK_OPT_ENABLE_SECURITY;
	appCmdDataReq.confirm = appCmdDataConf;

	for (uint8_t i = 0; i < APP_CMD_PENDING_TABLE_SIZE; i++) {
		appCmdPendingTable[i].addr = APP_CMD_INVALID_ADDR;
		appCmdPendingTable[i].ready = false;
	}

	NWK_OpenEndpoint(APP_CMD_ENDPOINT, appCmdDataInd);
}

/*************************************************************************//**
*****************************************************************************/
bool APP_CommandsPending(uint16_t addr)
{
	for (uint8_t i = 1; i < APP_CMD_PENDING_TABLE_SIZE; i++) {
		if (addr == appCmdPendingTable[i].addr) {
			appCmdPendingTable[i].ready = true;
			appCmdCheckPendingTable();
			return true;
		}
	}

	return false;
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
	if (APP_ADDR == addr) {
		appCmdHandle(data, size);
	} else if (addr & NWK_ROUTE_NON_ROUTING) {
		for (uint8_t i = 1; i < APP_CMD_PENDING_TABLE_SIZE; i++) {
			if (APP_CMD_INVALID_ADDR ==
					appCmdPendingTable[i].addr) {
				appCmdPendingTable[i].addr = addr;
				appCmdPendingTable[i].size = size;
				appCmdPendingTable[i].ready = false;
				memcpy(&appCmdPendingTable[i].payload, data,
						size);
				break;
			}
		}
	} else {
		/* Entry 0 in the table is reserved for non-sleeping devices */
		if (APP_CMD_INVALID_ADDR == appCmdPendingTable[0].addr) {
			appCmdPendingTable[0].addr = addr;
			appCmdPendingTable[0].size = size;
			appCmdPendingTable[0].ready = true;
			memcpy(&appCmdPendingTable[0].payload, data, size);
			appCmdCheckPendingTable();
		}
	}
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdDataRequest(AppCmdPendingTableEntry_t *entry)
{
	appCmdInProgress = entry;

	appCmdDataReq.dstAddr = entry->addr;
	appCmdDataReq.data = &entry->payload;
	appCmdDataReq.size = entry->size;
	NWK_DataReq(&appCmdDataReq);
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdDataConf(NWK_DataReq_t *req)
{
	appCmdInProgress->addr = APP_CMD_INVALID_ADDR;
	appCmdInProgress->ready = false;
	appCmdInProgress = NULL;

	appCmdCheckPendingTable();

	(void)req;
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdCheckPendingTable(void)
{
	if (appCmdInProgress) {
		return;
	}

	for (uint8_t i = 0; i < APP_CMD_PENDING_TABLE_SIZE; i++) {
		if (appCmdPendingTable[i].ready) {
			appCmdDataRequest(&appCmdPendingTable[i]);
		}
	}
}

/*************************************************************************//**
*****************************************************************************/
static bool appCmdDataInd(NWK_DataInd_t *ind)
{
	return appCmdHandle(ind->data, ind->size);
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
		NWK_Lock();

		return true;
	}

	return false;
}

/*************************************************************************//**
*****************************************************************************/
static void appCmdIdentifyDurationTimerHandler(SYS_Timer_t *timer)
{
	NWK_Unlock();
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
