/**
* \file  wsndemo.c
*
* \brief WSNDemo application implementation
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "phy.h"
#include "sysTimer.h"
#include "commands.h"
#include "miwi_api.h"

#if defined(PAN_COORDINATOR)
#include "sio2host.h"
#endif

#if defined(ENDDEVICE)
#include "sleep_mgr.h"
#endif

#if SAMD || SAMR21 || SAML21 || SAMR30
#include "system.h"
#else
#include "sysclk.h"
#if (LED_COUNT > 0)
#include "led.h"
#endif
#endif

#include "asf.h"
#include "wsndemo.h"
#if defined(ENABLE_NETWORK_FREEZER)
#include "pdsMemIds.h"
#include "pdsDataServer.h"
#include "wlPdsTaskManager.h"
#endif
/*****************************************************************************
*****************************************************************************/
#define APP_SCAN_DURATION 10
#define APP_CAPTION_SIZE  (sizeof(APP_CAPTION) - 1 + SHORT_ADDRESS_CAPTION_SIZE)

/*- Types ------------------------------------------------------------------*/
COMPILER_PACK_SET(1)
typedef struct  AppMessage_t {
	uint8_t commandId;
	uint8_t nodeType;
	uint64_t extAddr;
	uint16_t shortAddr;
	uint32_t softVersion;
	uint32_t channelMask;
	uint16_t panId;
	uint8_t workingChannel;
	uint16_t nextHopAddr;
	uint8_t lqi;
	int8_t rssi;

	struct {
		uint8_t type;
		uint8_t size;
		int32_t battery;
		int32_t temperature;
		int32_t light;
	} sensors;

	struct {
		uint8_t type;
		uint8_t size;
		char text[APP_CAPTION_SIZE];
	} caption;
} AppMessage_t;

typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_START_NETWORK,
	APP_STATE_CONNECT_NETWORK,
	APP_STATE_CONNECTING_NETWORK,
	APP_STATE_IN_NETWORK,
	APP_STATE_SEND,
	APP_STATE_WAIT_CONF,
	APP_STATE_SENDING_DONE,
	APP_STATE_WAIT_SEND_TIMER,
	APP_STATE_WAIT_COMMAND_TIMER,
	APP_STATE_PREPARE_TO_SLEEP,
	APP_STATE_SLEEP,
	APP_STATE_WAKEUP,
} AppState_t;
COMPILER_PACK_RESET()
/*- Variables --------------------------------------------------------------*/
static AppState_t appState = APP_STATE_INITIAL;

#if defined(COORDINATOR) || defined (ENDDEVICE)
static SYS_Timer_t appNetworkStatusTimer;
static bool appNetworkStatus;
#endif

#if defined(PAN_COORDINATOR)
static uint8_t rx_data[APP_RX_BUF_SIZE];
#endif

static AppMessage_t appMsg;
SYS_Timer_t appDataSendingTimer;
#ifndef PAN_COORDINATOR
static uint8_t wsnmsghandle;
#endif

void UartBytesReceived(uint16_t bytes, uint8_t *byte );
static void Connection_Confirm(miwi_status_t status);
#ifndef PAN_COORDINATOR
void searchConfim(uint8_t foundScanResults, void* ScanResults);
void appLinkFailureCallback(void);
#endif

#if defined(ENABLE_NETWORK_FREEZER)
bool reconnectStatus = false;
static void ReconnectionIndication (miwi_status_t status);
#endif
/*- Implementations --------------------------------------------------------*/

#if defined(PAN_COORDINATOR)

/*****************************************************************************
*****************************************************************************/
void UartBytesReceived(uint16_t bytes, uint8_t *byte )
{
	for (uint16_t i = 0; i < bytes; i++) {
		APP_CommandsByteReceived(byte[i]);
	}
}

static void appUartSendMessage(uint8_t *data, uint8_t size)
{
	uint8_t cs = 0;

	sio2host_putchar(0x10);
	sio2host_putchar(0x02);

	for (uint8_t i = 0; i < size; i++) {
		if (data[i] == 0x10) {
			sio2host_putchar(0x10);
			cs += 0x10;
		}

		sio2host_putchar(data[i]);
		cs += data[i];
	}

	sio2host_putchar(0x10);
	sio2host_putchar(0x03);
	cs += 0x10 + 0x02 + 0x10 + 0x03;

	sio2host_putchar(cs);
}

#endif

/*****************************************************************************
*****************************************************************************/
static void appDataInd(RECEIVED_MESH_MESSAGE *ind)
{
	AppMessage_t *msg = (AppMessage_t *)ind->payload;
#if (LED_COUNT > 0)
	LED_Toggle(LED_DATA);
#endif
	msg->lqi = ind->packetLQI;
	msg->rssi = ind->packetRSSI;
#if defined(PAN_COORDINATOR)
	appUartSendMessage(ind->payload, ind->payloadSize);
#else
    appCmdDataInd(ind);
#endif
}

/*****************************************************************************
*****************************************************************************/
static void appDataSendingTimerHandler(SYS_Timer_t *timer)
{
	if (APP_STATE_WAIT_SEND_TIMER == appState) {
		appState = APP_STATE_SEND;
	} else {
		SYS_TimerStart(&appDataSendingTimer);
	}

	(void)timer;
}

#if defined(COORDINATOR) || defined (ENDDEVICE)

/*****************************************************************************
*****************************************************************************/
static void appNetworkStatusTimerHandler(SYS_Timer_t *timer)
{
#if (LED_COUNT > 0)
	LED_Toggle(LED_NETWORK);
#endif
	(void)timer;
}
#endif

/*****************************************************************************
*****************************************************************************/
#if defined(COORDINATOR) || defined (ENDDEVICE)
static void appDataConf(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer)
{
#if (LED_COUNT > 0)
	LED_Off(LED_DATA);
#endif

	if (SUCCESS == status) {
		if (!appNetworkStatus) {
#if (LED_COUNT > 0)
			LED_On(LED_NETWORK);
#endif
			SYS_TimerStop(&appNetworkStatusTimer);
			appNetworkStatus = true;
		}
	} else {
		if (appNetworkStatus) {
#if (LED_COUNT > 0)
			LED_Off(LED_NETWORK);
#endif
			SYS_TimerStart(&appNetworkStatusTimer);
			appNetworkStatus = false;
		}
	}
	if (APP_STATE_WAIT_CONF == appState)
	{
		appState = APP_STATE_SENDING_DONE;
	}
}

#endif

/*****************************************************************************
*****************************************************************************/
static void appSendData(void)
{
    uint16_t shortAddressLocal = 0xFFFF;
    uint16_t shortAddressPanId = 0xFFFF;
#ifndef PAN_COORDINATOR
    uint16_t dstAddr = 0; /* PAN Coordinator Address */
#endif

	appMsg.sensors.battery     = rand() & 0xffff;
	appMsg.sensors.temperature = rand() & 0x7f;
	appMsg.sensors.light       = rand() & 0xff;

	/* Get Short address */
	MiApp_Get(SHORT_ADDRESS, (uint8_t *)&shortAddressLocal);
        appMsg.shortAddr = shortAddressLocal;
	appMsg.extAddr   = appMsg.shortAddr;
#ifndef PAN_COORDINATOR
    /* Get Next Hop Short address to reach PAN Coordinator*/
	appMsg.nextHopAddr = MiApp_MeshGetNextHopAddr(PAN_COORDINATOR_SHORT_ADDRESS);
#endif

	/* Get current working channel */
	MiApp_Get(CHANNEL, (uint8_t *)&appMsg.workingChannel);

	/* Get current working PanId */
	MiApp_Get(PANID, (uint8_t *)&shortAddressPanId);
        appMsg.panId = shortAddressPanId;
#if defined(PAN_COORDINATOR)
	sprintf(&(appMsg.caption.text[APP_CAPTION_SIZE - SHORT_ADDRESS_CAPTION_SIZE]), "-0x%04X", shortAddressLocal);
	appUartSendMessage((uint8_t *)&appMsg, sizeof(appMsg));
	SYS_TimerStart(&appDataSendingTimer);
	appState = APP_STATE_WAIT_SEND_TIMER;
#else
#if (LED_COUNT > 0)
	LED_On(LED_DATA);
#endif

	appMsg.caption.type         = 32;
#if defined(COORDINATOR)
	if (shortAddressLocal & RXON_ENDEVICE_ADDRESS_MASK)
	{
		appMsg.caption.size         = APP_CAPTION_ED_SIZE;
	    memcpy(appMsg.caption.text, APP_CAPTION_ED, APP_CAPTION_ED_SIZE);
		sprintf(&(appMsg.caption.text[APP_CAPTION_ED_SIZE - SHORT_ADDRESS_CAPTION_SIZE]), "-0x%04X", shortAddressLocal);
	}
	else
#endif
	{
	    appMsg.caption.size         = APP_CAPTION_SIZE;
	    memcpy(appMsg.caption.text, APP_CAPTION, APP_CAPTION_SIZE);
		sprintf(&(appMsg.caption.text[APP_CAPTION_SIZE - SHORT_ADDRESS_CAPTION_SIZE]), "-0x%04X", shortAddressLocal);
	}

	if (MiApp_SendData(2, (uint8_t *)&dstAddr, sizeof(appMsg), (uint8_t *)&appMsg, wsnmsghandle, true, appDataConf))
	{
		++wsnmsghandle;
		appState = APP_STATE_WAIT_CONF;
	}
	else
	{
		appState = APP_STATE_SENDING_DONE;
	}
#endif
}

/*************************************************************************//**
*****************************************************************************/
static void appInit(void)
{
	appMsg.commandId            = APP_COMMAND_ID_NETWORK_INFO;
	appMsg.nodeType             = APP_NODE_TYPE;
	appMsg.extAddr              = 0;
	appMsg.shortAddr            = 0;
	appMsg.softVersion          = 0x01100000;
	appMsg.channelMask          = CHANNEL_MAP;
	appMsg.nextHopAddr          = 0;
	appMsg.lqi                  = 0;
	appMsg.rssi                 = 0;

	appMsg.sensors.type        = 1;
	appMsg.sensors.size        = sizeof(int32_t) * 3;
	appMsg.sensors.battery     = 0;
	appMsg.sensors.temperature = 0;
	appMsg.sensors.light       = 0;

	appMsg.caption.type         = 32;
	appMsg.caption.size         = APP_CAPTION_SIZE;
	memcpy(appMsg.caption.text, APP_CAPTION, APP_CAPTION_SIZE);

	appDataSendingTimer.interval = APP_SENDING_INTERVAL;
	appDataSendingTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appDataSendingTimer.handler = appDataSendingTimerHandler;

#if defined(COORDINATOR) || defined (ENDDEVICE)
	appNetworkStatus = false;
	appNetworkStatusTimer.interval = APP_NWKSTATUS_INTERVAL;
	appNetworkStatusTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appNetworkStatusTimer.handler = appNetworkStatusTimerHandler;
	SYS_TimerStart(&appNetworkStatusTimer);
#else
#if (LED_COUNT > 0)
	LED_On(LED_NETWORK);
#endif
#endif

	APP_CommandsInit();
	MiApp_SubscribeDataIndicationCallback(appDataInd);
#ifndef PAN_COORDINATOR
	MiApp_SubscribeLinkFailureCallback(appLinkFailureCallback);
#endif

#if defined(ENABLE_NETWORK_FREEZER)
     if (reconnectStatus)
	 {
		 appState = APP_STATE_SEND;		 
	 }
	 else
#endif
	 {
#if defined(PAN_COORDINATOR)
         appState = APP_STATE_START_NETWORK;
#else
         appState = APP_STATE_CONNECT_NETWORK;
#endif 
	 }
}

#if defined(ENABLE_NETWORK_FREEZER)
static void ReconnectionIndication (miwi_status_t status)
{
	if(SUCCESS == status)
	{
		reconnectStatus = true;
#if defined(ENDDEVICE)
		appState = APP_STATE_SEND;
#endif
	}
	else
	{
        reconnectStatus = false;
#if defined(ENDDEVICE)
         appState = APP_STATE_CONNECT_NETWORK;
#endif
	}
}
#endif


/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	switch (appState) {
	case APP_STATE_INITIAL:
	{
		appInit();
	}
	break;
#if defined(PAN_COORDINATOR)
	case APP_STATE_START_NETWORK:
	{
		MiApp_StartConnection(START_CONN_DIRECT, APP_SCAN_DURATION, CHANNEL_MAP, Connection_Confirm);
		appState = APP_STATE_SEND;
	}
	break;
#else
	case APP_STATE_CONNECT_NETWORK:
	{
		MiApp_SearchConnection(APP_SCAN_DURATION, CHANNEL_MAP, searchConfim);
		appState = APP_STATE_CONNECTING_NETWORK;
	}
	break;
#endif

	case APP_STATE_SEND:
	{
		appSendData();
	}
	break;

	case APP_STATE_SENDING_DONE:
	{
#if defined(ENABLE_SLEEP_FEATURE) && defined(ENDDEVICE) && (CAPABILITY_INFO == CAPABILITY_INFO_ED)
		appState = APP_STATE_PREPARE_TO_SLEEP;
#else
		SYS_TimerStart(&appDataSendingTimer);
		appState = APP_STATE_WAIT_SEND_TIMER;
#endif
	}
	break;
#if defined(ENABLE_SLEEP_FEATURE) && defined(ENDDEVICE)
	case APP_STATE_PREPARE_TO_SLEEP:
	{
		uint32_t timeToSleep = 0;
		if (!SYS_TimerStarted(&appCmdIdentifyDurationTimer) && 
		    !SYS_TimerStarted(&appCmdIdentifyPeriodTimer) &&
		    MiApp_ReadyToSleep(&timeToSleep)
		   )
		{
			if (timeToSleep > APP_SENDING_INTERVAL)
			{
				timeToSleep = APP_SENDING_INTERVAL;
			}

			if (timeToSleep > MIN_SLEEP_INTERVAL)
			{
				sm_sleep(timeToSleep / 1000);
				SYS_TimerAdjust_SleptTime(timeToSleep);
				appState = APP_STATE_WAKEUP;
			}
			else
			{
				SYS_TimerStart(&appDataSendingTimer);
		        appState = APP_STATE_WAIT_SEND_TIMER;
			}
		}
	}
	break;

	case APP_STATE_WAKEUP:
	{
		appState = APP_STATE_SEND;
	}
	break;
#endif

	default:
		break;
	}

#if defined(ENABLE_NETWORK_FREEZER)
	/* Read the button level */
	if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE)
	{
        MiApp_ResetToFactoryNew();
	}
#endif

#if defined(PAN_COORDINATOR)
	uint16_t bytes;
	if ((bytes = sio2host_rx(rx_data, APP_RX_BUF_SIZE)) > 0) {
		UartBytesReceived(bytes, (uint8_t *)&rx_data);
	}
#endif
}

/*****************************************************************************
*****************************************************************************/

/**
 * Init function of the WSNDemo application
 */
void wsndemo_init(void)
{
	uint8_t i;
	uint64_t ieeeAddr;
	uint64_t invalidIEEEAddr;

#if defined(ENABLE_NETWORK_FREEZER)
    MiApp_SubscribeReConnectionCallback((ReconnectionCallback_t)ReconnectionIndication );
#endif
	MiApp_ProtocolInit(&defaultParamsRomOrRam, &defaultParamsRamOnly);
	/* Check if a valid IEEE address is available. */
	memcpy((uint8_t *)&ieeeAddr, (uint8_t *)&myLongAddress, LONG_ADDR_LEN);
	memset((uint8_t *)&invalidIEEEAddr, 0xFF, sizeof(invalidIEEEAddr));
	srand(PHY_RandomReq());
	/*
		* This while loop is on purpose, since just in the
		* rare case that such an address is randomly
		* generated again, we must repeat this.
		*/
	while ((ieeeAddr == 0x0000000000000000) || (ieeeAddr == invalidIEEEAddr))
	{
		/*
			* In case no valid IEEE address is available, a random
			* IEEE address will be generated to be able to run the
			* applications for demonstration purposes.
			* In production code this can be omitted.
			*/
		uint8_t* peui64 = (uint8_t *)&myLongAddress;
		for(i = 0; i<MY_ADDRESS_LENGTH; i++)
		{
			*peui64++ = (uint8_t)rand();
		}
		memcpy((uint8_t *)&ieeeAddr, (uint8_t *)&myLongAddress, LONG_ADDR_LEN);
	}
	PHY_SetIEEEAddr((uint8_t *)&ieeeAddr);

#ifdef ENABLE_SLEEP_FEATURE
#if defined(ENDDEVICE)
    sm_init();
#endif
#endif

#if defined(PAN_COORDINATOR)
	sio2host_init();
#endif
}

#ifndef PAN_COORDINATOR
/**
 * Search confirmation
 */
void searchConfim(uint8_t foundScanResults, void* ScanResults)
{
	searchConf_t* searchConfRes = (searchConf_t *)ScanResults;
	if (foundScanResults)
	{
		for (uint8_t loopindex = 0; loopindex < foundScanResults; loopindex++)
		{
			if (searchConfRes->beaconList[loopindex].connectionPermit)
			{
				MiApp_EstablishConnection(searchConfRes->beaconList[loopindex].logicalChannel,
				SHORT_ADDR_LEN, (uint8_t*)&searchConfRes->beaconList[loopindex].shortAddress, CAPABILITY_INFO, Connection_Confirm);
				return;
			}
		}
		/* Initiate the search again since no connection permit found to join */
		appState = APP_STATE_CONNECT_NETWORK;
	}
	else
	{
		/* Initiate the search again since no beacon */
		appState = APP_STATE_CONNECT_NETWORK;
	}
}
#endif

/**
 * Connection confirmation
 */
static void Connection_Confirm(miwi_status_t status)
{
	if (SUCCESS == status)
	{
        appState = APP_STATE_SEND;
	}
	else
	{
#if defined(PAN_COORDINATOR)
		appState = APP_STATE_START_NETWORK;
#else
        appState = APP_STATE_CONNECT_NETWORK;
#endif
	}
}

/**
 * Task of the WSNDemo application
 * This task should be called in a while(1)
 */
void wsndemo_task(void)
{
	MeshTasks();
#if defined(ENABLE_NETWORK_FREEZER)
#if PDS_ENABLE_WEAR_LEVELING
    PDS_TaskHandler();
#endif
#endif
	APP_TaskHandler();
}

#ifndef PAN_COORDINATOR
void appLinkFailureCallback(void)
{
	/* On link failure initiate search to establish connection */
	appState = APP_STATE_CONNECT_NETWORK;
	SYS_TimerStop(&appDataSendingTimer);
}
#endif
