/**
* \file client_notify.c
*
* \brief Client Notify implementation
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

#if defined(OTAU_ENABLED) && !defined(OTAU_SERVER)

#include "compiler.h"
#include "string.h"
#include "client_notify.h"
#include "otau_notify.h"
#include "miwi_config.h"
#include "miwi_api.h"

static void otauClientSendNotify(void);
static void otauClientStartTimer(void);

#define MULTIPLIER  (10)

otauNotifyState_t notifyState;
otauNotifyTimerState_t notifyTimerState;
uint8_t otauLedToggleCount;
uint8_t clientNotifyRetryRunningCount;
uint8_t otauNotifyConfirmWait = 0;

uint16_t otauClientNotifyInterval = CLIENT_NOTIFY_INTERVAL_SEC;
uint8_t otauClientNotifyJitterPercent = CLIENT_NOTIFY_JITTER_PERCENT;
uint8_t otauClientNotifyRetryCount = CLIENT_NOTIFY_RETRY_COUNT;

const otauClientInfoIndication_t clientInfo = {
	.domainId = DOMAIN_OTAU_NOTIFY,
	.msgId = OTA_CLIENT_INFO,
#if defined (PAN_COORDINATOR)
	.device_type = "PAN Coord ",
#elif defined (COORDINATOR)
	.device_type = "Coord     ",
#else
	.device_type = "EndDevice ",
#endif
	.firmware = "MiWi Mesh ",
	.firmware_version = "6.1.0",
	.board = "SAMR21    ",
	.board_version = "1.0.0"
};

void otauNotifyInit(void)
{
	notifyTimerState = CLIENT_NOTIFY_STATE;
	notifyState = NOTIFY_IDLE;
	clientNotifyRetryRunningCount = otauClientNotifyRetryCount;
}

void otauNotifyRcvdFrame(addr_mode_t addr_mode, uint8_t *src_addr, uint16_t length, uint8_t *payload)
{
	uint8_t msg_code = *(payload + 1);
	switch (msg_code)
	{
		case OTA_CLIENT_DISCOVERY:
		{
			otauClientDiscoveryReq_t clientDiscReq;
			memcpy((uint8_t *)&clientDiscReq, payload, sizeof(otauClientDiscoveryReq_t));
			otauClientNotifyInterval = clientDiscReq.interval;
			otauClientNotifyJitterPercent = clientDiscReq.jitter_percent;
			otauClientNotifyRetryCount = clientDiscReq.retry_count;
			otauSetServerDetails(src_addr);
			otauClientStartTimer();
			break;
		}
		case OTA_CLIENT_INFO_REQ:
		{
			if(!otauNotifyConfirmWait)
			{
				otauNotifyConfirmWait = 1;
				notifyState = NOTIFY_CLIENT_INFO_STATE;
				otauSetServerDetails(src_addr);
				otauDataSend(addr_mode, src_addr, (uint8_t *)&clientInfo, sizeof(otauClientInfoIndication_t));
			}
			break;
		}
		case OTA_IDENTIFY_REQ:
		{
			otauSetServerDetails(src_addr);
			otauTimerStart(DOMAIN_OTAU_NOTIFY, LED_TOGGLE_INTERVAL_MILLISEC, TIMER_MODE_PERIODIC);
			notifyTimerState = CLIENT_IDENTIFY_STATE;
			otauLedToggleCount = LED_TOGGLE_COUNT;
		}
		break;
		case OTA_RESET_REQ:
		{
			otauResetDevice();
			break;
		}
	}
}

void otauNotifySentFrame(addr_mode_t addr_mode, uint8_t *addr, uint8_t status)
{    
	otauNotifyConfirmWait = 0;

	if (NOTIFY_CLIENT_NOTIFY_STATE == notifyState)
	{
		if (OTAU_SUCCESS != status && 0 != clientNotifyRetryRunningCount)
		{
			clientNotifyRetryRunningCount--;
			otauClientSendNotify();
		}
		else
		{
			clientNotifyRetryRunningCount = otauClientNotifyRetryCount;
			otauClientStartTimer();
		}
	}
	notifyState = NOTIFY_IDLE;
}

void otauNotifyTimerHandler(struct SYS_Timer_t *timer)
{
	if(CLIENT_NOTIFY_STATE == notifyTimerState && !otauNotifyConfirmWait)
	{
		clientNotifyRetryRunningCount = otauClientNotifyRetryCount;
		otauClientSendNotify();
	}
	else if (CLIENT_IDENTIFY_STATE == notifyTimerState)
	{
		otauLed(OTAU_LED_TOGGLE);
		--otauLedToggleCount;
		
		if(!otauLedToggleCount)
		{
			otauClientStartTimer();
			notifyTimerState = CLIENT_NOTIFY_STATE;
		}
	}
}

static void otauClientSendNotify(void)
{
	otauClientNotifyIndication_t clientNotify;
	clientNotify.msgId = OTA_CLIENT_NOTIFY;
	clientNotify.supported_modes = SUPPORTED_MODE;
	clientNotify.arch_type = 0; //0 for ARM and 1 for mega
	clientNotify.ieee_addr_mode = EXTENDED_ADDR_MODE;
	memcpy((uint8_t *)&clientNotify.ieee_addr, get_node_address(EXTENDED_ADDR_MODE), EXTENDED_ADDR_SIZE);
	clientNotify.pan_id = MY_PAN_ID;
	clientNotify.parent_addr_mode = NATIVE_ADDR_MODE;
	clientNotify.parent_short_addr = MiApp_MeshGetNextHopAddr(serverShortAddress);
	otauNotifyConfirmWait = 1;
	notifyState = NOTIFY_CLIENT_NOTIFY_STATE;
	clientNotify.domainId = DOMAIN_OTAU_NOTIFY;
	otauDataSend(NATIVE_ADDR_MODE,NULL, &clientNotify, sizeof(otauClientNotifyIndication_t));
}

static void otauClientStartTimer(void)
{
	if(otauClientNotifyInterval)
	{
		uint32_t jitterRange;
		uint32_t notifyInterval;
		uint32_t minNotifyInterval;
		uint8_t randPercent;
		uint32_t clientNotifyIntervalSec;

		if (MIN_CLIENT_NOTIFY_INTERVAL_SEC > otauClientNotifyInterval)
		{
			clientNotifyIntervalSec = MIN_CLIENT_NOTIFY_INTERVAL_SEC;
		}
		else
		{
			clientNotifyIntervalSec = otauClientNotifyInterval;
		}

		jitterRange = (clientNotifyIntervalSec * MULTIPLIER * otauClientNotifyJitterPercent);
		notifyInterval = clientNotifyIntervalSec * 1000;
		minNotifyInterval = notifyInterval - jitterRange;
		randPercent = rand() % 100;

		jitterRange <<= 1; //For multiplying by 2

		notifyInterval = minNotifyInterval + ((jitterRange * randPercent) / 100);

		if (!notifyInterval)
		{
			notifyInterval = clientNotifyIntervalSec * 1000;
		}

		otauTimerStart(DOMAIN_OTAU_NOTIFY, notifyInterval, TIMER_MODE_PERIODIC);
	}
	else
	{
		otauTimerStop(DOMAIN_OTAU_NOTIFY);
	}
}

#endif //#if defined(OTAU_ENABLED) && !defined(OTAU_SERVER)