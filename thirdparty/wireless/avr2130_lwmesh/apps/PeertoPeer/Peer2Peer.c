/**
 * \file Peer2Peer.c
 *
 * \brief Peer2Peer application implementation
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
 *
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the LWMesh Peer2Peer Application
 * The LightWeight Mesh Peer2Peer  implements a wireless UART application.Two nodes are used in this application
 * These two nodes must be configured with addresses 0x0001 and 0x0000 respectively.
 * To test this application,open a terminal for both the nodes.On entering text in the terminal the data is transmitted from one 
 * node to another node(0x0001 to 0x0000 and vice-versa)
 */
/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "sys.h"
#if SAMD || SAMR21 || SAML21
#include "system.h"
#else
#include "led.h"
#include "sysclk.h"
#endif
#include "phy.h"
#include "nwk.h"
#include "sysTimer.h"
#include "sio2host.h"
#include "asf.h"

/*- Definitions ------------------------------------------------------------*/
#ifdef NWK_ENABLE_SECURITY
  #define APP_BUFFER_SIZE     (NWK_MAX_PAYLOAD_SIZE - NWK_SECURITY_MIC_SIZE)
#else
  #define APP_BUFFER_SIZE     NWK_MAX_PAYLOAD_SIZE
#endif

static uint8_t rx_data[APP_RX_BUF_SIZE];

/*- Types ------------------------------------------------------------------*/
typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
} AppState_t;

/*- Prototypes -------------------------------------------------------------*/
static void appSendData(void);

/*- Variables --------------------------------------------------------------*/
static AppState_t appState = APP_STATE_INITIAL;
static SYS_Timer_t appTimer;
static NWK_DataReq_t appDataReq;
static bool appDataReqBusy = false;
static uint8_t appDataReqBuffer[APP_BUFFER_SIZE];
static uint8_t appUartBuffer[APP_BUFFER_SIZE];
static uint8_t appUartBufferPtr = 0;
static uint8_t sio_rx_length;
/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static void appDataConf(NWK_DataReq_t *req)
{
	appDataReqBusy = false;
	(void)req;
}

/*************************************************************************//**
*****************************************************************************/
static void appSendData(void)
{
	if (appDataReqBusy || 0 == appUartBufferPtr) {
		return;
	}

	memcpy(appDataReqBuffer, appUartBuffer, appUartBufferPtr);

	appDataReq.dstAddr = 1 - APP_ADDR;
	appDataReq.dstEndpoint = APP_ENDPOINT;
	appDataReq.srcEndpoint = APP_ENDPOINT;
	appDataReq.options = NWK_OPT_ENABLE_SECURITY;
	appDataReq.data = appDataReqBuffer;
	appDataReq.size = appUartBufferPtr;
	appDataReq.confirm = appDataConf;
	NWK_DataReq(&appDataReq);

	appUartBufferPtr = 0;
	appDataReqBusy = true;
	LED_Toggle(LED0);
}

/*************************************************************************//**
*****************************************************************************/
static void appTimerHandler(SYS_Timer_t *timer)
{
	appSendData();
	(void)timer;
}

/*************************************************************************//**
*****************************************************************************/
static bool appDataInd(NWK_DataInd_t *ind)
{
	for (uint8_t i = 0; i < ind->size; i++) {
		sio2host_putchar(ind->data[i]);
	}
	LED_Toggle(LED0);
	return true;
}

/*************************************************************************//**
*****************************************************************************/
static void appInit(void)
{
	NWK_SetAddr(APP_ADDR);
	NWK_SetPanId(APP_PANID);
	PHY_SetChannel(APP_CHANNEL);
#ifdef PHY_AT86RF212
	PHY_SetBand(APP_BAND);
	PHY_SetModulation(APP_MODULATION);
#endif
	PHY_SetRxState(true);

	NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);

	appTimer.interval = APP_FLUSH_TIMER_INTERVAL;
	appTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appTimer.handler = appTimerHandler;
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	switch (appState) {
	case APP_STATE_INITIAL:
	{
		appInit();
		appState = APP_STATE_IDLE;
	}
	break;

	case APP_STATE_IDLE:
		break;

	default:
		break;
	}
	sio_rx_length = sio2host_rx(rx_data, APP_RX_BUF_SIZE);
	if (sio_rx_length) {
		for (uint16_t i = 0; i < sio_rx_length; i++) {
			sio2host_putchar(rx_data[i]);
			if (appUartBufferPtr == sizeof(appUartBuffer)) {
				appSendData();
			}

			if (appUartBufferPtr < sizeof(appUartBuffer)) {
				appUartBuffer[appUartBufferPtr++] = rx_data[i];
			}
		}

		SYS_TimerStop(&appTimer);
		SYS_TimerStart(&appTimer);
	}
}

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	irq_initialize_vectors();
	#if SAMD || SAMR21 || SAML21
	system_init();
	delay_init();
	#else
	sysclk_init();
	board_init();
	#endif
	SYS_Init();
	sio2host_init();
	cpu_irq_enable();
	LED_On(LED0);
	while (1) {
		SYS_TaskHandler();
		APP_TaskHandler();
	}
}
