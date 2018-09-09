/**
* \file otau.c
*
* \brief OTAU implementation
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

#if defined(OTAU_ENABLED)
#include "compiler.h"
#include "asf.h"
#include "otau.h"
#include "system.h"
#include "sysTimer.h"

#ifdef OTAU_SERVER
#include "otau_parser.h"
#else
#include "common_nvm.h"
#endif

#include "miwi_api.h"
#include "mimem.h"
#include "miqueue.h"

#ifdef OTAU_SERVER
#include "otau_parser.h"
#endif
#include "otau_notify.h"
#include "otau_upgrade.h"

static void otauDataInd(RECEIVED_MESH_MESSAGE *ind);
static void otauDataConf(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer);

static void initDataHandleTable(void);
static bool addDataHandleTableEntry(uint8_t domainId, addr_mode_t addrMode, uint8_t *addr, uint8_t handle);
static bool getAddressFromDataHandleTable(uint8_t handle, uint8_t *addr, uint8_t *addrMode, uint8_t *domainId);

static SYS_Timer_t otauNotifyTimer;
static SYS_Timer_t otauUpgradeTimer;

static bool otauInited = false;

uint16_t shortAddress;
uint16_t pan_id = MY_PAN_ID;

uint8_t seq_no = 0;

MiQueue_t networkFrame;

DataHandleTable_t dataHandleTable[DATA_HANDLE_TABLE_SIZE];

static uint8_t msgHandle = 0;

#ifndef OTAU_SERVER
uint16_t serverShortAddress = 0x0000;
#endif

void otauInit(void)
{
	miQueueInit(&networkFrame);
	otauNotifyInit();
	otauUpgradeInit();
#ifdef OTAU_SERVER
	otauParserInit();
#else
	nvm_init(INT_FLASH);
#endif
	initDataHandleTable();
	MiApp_SubscribeManuSpecDataIndicationCallback(otauDataInd);
}

void otauTask(void)
{
	if (MiApp_IsConnected())
	{
		MiApp_Get(SHORT_ADDRESS, (uint8_t *)&shortAddress);
		if (!otauInited)
		{
			otauInit();
			otauInited = true;
		}
		if (0 != networkFrame.size)
		{
			uint8_t *event = (uint8_t *)miQueueRemove(&networkFrame, NULL);
			if (NULL != event)
			{
				/* + 4  to store the next pointer in Queue */
				otau_rcvd_frame_t *buff = (otau_rcvd_frame_t *)(event + 4);
				otauRcvdFrame(buff);
				MiMem_Free(event);
			}
		}
	}
	#ifdef OTAU_SERVER
	serialDataHandler();
	if (0 != dataFromTool.size)
	{
		uint8_t *event = (uint8_t *)miQueueRemove(&dataFromTool, NULL);
		if (NULL != event)
		{
			/* + 4  to store the next pointer in Queue */
			otau_domain_msg_t *buff = (otau_domain_msg_t *)(event + 4);
			otauHandleMsg(buff);
			MiMem_Free(event);
		}
	}
	#endif
}

void otauHandleMsg(otau_domain_msg_t *otau_domain_msg)
{
#ifdef OTAU_SERVER
	switch (otau_domain_msg->domainId)
	{
		case DOMAIN_OTAU_NOTIFY:
			otauHandleNotifyMsg(otau_domain_msg);
			break;
		case DOMAIN_OTAU_UPGRADE:
			handle_upgrade_otau_msg(otau_domain_msg);
			break;
	}
#endif
}

void otauRcvdFrame(otau_rcvd_frame_t *frame)
{
	uint8_t *domain = frame->frame_payload;
	if (DOMAIN_OTAU_NOTIFY == *domain)
	{
		otauNotifyRcvdFrame(frame->addr_mode, frame->src_addr, frame->frame_length, frame->frame_payload);
	}
	else if (DOMAIN_OTAU_UPGRADE == *domain)
	{
		otauUpgradeRcvdFrame(frame->addr_mode, frame->src_addr, frame->frame_length, frame->frame_payload);
	}
}

void otauDataSend(addr_mode_t addr_mode, uint8_t *addr, void *payload, uint16_t len)
{
	uint16_t dstAddr;
	uint8_t domainId = *((uint8_t *)payload);
	if (NATIVE_ADDR_MODE == addr_mode && NULL == addr)
	{
#ifndef OTAU_SERVER
		addr = (uint8_t *)&serverShortAddress;
#endif
	}
	else if (BROADCAST_MODE == addr_mode)
	{
		dstAddr = MESH_BROADCAST_TO_ALL;
		addr = (uint8_t *)&dstAddr;
	}
	if (DOMAIN_OTAU_NOTIFY == domainId)
	{
		if (!MiApp_ManuSpecSendData(SHORT_ADDR_LEN, addr, len, payload, msgHandle, 1, otauDataConf))
		{
			otauNotifySentFrame(NATIVE_ADDR_MODE, addr, OTAU_ERROR);
		}
		else
		{
			addDataHandleTableEntry(domainId, NATIVE_ADDR_MODE, addr, msgHandle);
			msgHandle++;
		}		
	}
	else if (DOMAIN_OTAU_UPGRADE == domainId)
	{
		if (!MiApp_ManuSpecSendData(SHORT_ADDR_LEN, addr, len, payload, msgHandle, 1, otauDataConf))
		{
			otauUpgradeSentFrame(NATIVE_ADDR_MODE, addr, OTAU_ERROR);
		}
		else
		{
			addDataHandleTableEntry(domainId, NATIVE_ADDR_MODE, addr, msgHandle);
			msgHandle++;
		}
	}
}

static void otauDataConf(uint8_t msgConfHandle, miwi_status_t status, uint8_t* msgPointer)
{
	uint16_t dstAddr;
	uint8_t domainId = DOMAIN_OTAU_NOTIFY;
	uint8_t addrMode;
	getAddressFromDataHandleTable(msgConfHandle, &domainId, &addrMode, (uint8_t *)&dstAddr);
	switch (status)
	{
		case SUCCESS:
		status = OTAU_SUCCESS;
		break;
		case NO_ACK:
		status = OTAU_NO_ACK;
		break;
		case CHANNEL_ACCESS_FAILURE:
		status = OTAU_CCA_FAILURE;
		break;
		default:
		status = OTAU_ERROR;
	}
	if (DOMAIN_OTAU_NOTIFY == domainId)
	{
		otauNotifySentFrame(NATIVE_ADDR_MODE, (uint8_t *)&dstAddr, status);
	}
	else if (DOMAIN_OTAU_UPGRADE == domainId)
	{
		otauUpgradeSentFrame(NATIVE_ADDR_MODE, (uint8_t *)&dstAddr, status);
	}
}

static void otauDataInd(RECEIVED_MESH_MESSAGE *ind)
{
	if((NULL != ind) && (NULL != ind->payload))
	{
		/* + 4  to store the next pointer in Queue */
		uint8_t *buffer_header = MiMem_Alloc(sizeof(otau_rcvd_frame_t) 
							+ NATIVE_ADDR_SIZE + ind->payloadSize + 4);
		if (NULL != buffer_header)
		{
			uint8_t *frame_ptr;
			uint16_t size;
			/* + 4  to store the next pointer in Queue */
			otau_rcvd_frame_t *rcvd_frame = (otau_rcvd_frame_t *)(buffer_header + 4);

			frame_ptr = (uint8_t *)rcvd_frame + sizeof(otau_rcvd_frame_t);
			memcpy(&(rcvd_frame->src_addr), &frame_ptr, 4);
			memcpy(frame_ptr, (uint8_t *)&(ind->sourceAddress), NATIVE_ADDR_SIZE);
			frame_ptr += NATIVE_ADDR_SIZE;

			memcpy(&(rcvd_frame->frame_payload), &frame_ptr, 4);
			memcpy(rcvd_frame->frame_payload, ind->payload, ind->payloadSize);

			size = ind->payloadSize;
			memcpy(&(rcvd_frame->frame_length), &size, 2);
			rcvd_frame->addr_mode = NATIVE_ADDR_MODE;

			miQueueAppend(&networkFrame, (miQueueBuffer_t *)buffer_header);
		}
	}
}

void otauTimerStart(otau_domain_t domain_code, uint32_t interval, otau_timer_mode_t mode)
{
	switch (domain_code)
	{
		case DOMAIN_OTAU_NOTIFY:
		if (SYS_TimerStarted(&otauNotifyTimer))
		{
			SYS_TimerStop(&otauNotifyTimer);
		}
		otauNotifyTimer.interval = interval;
		otauNotifyTimer.handler = otauNotifyTimerHandler;
		if(TIMER_MODE_SINGLE == mode) {
			otauNotifyTimer.mode = SYS_TIMER_INTERVAL_MODE;
			} else {
			otauNotifyTimer.mode = SYS_TIMER_PERIODIC_MODE;
		}
		SYS_TimerStart(&otauNotifyTimer);
		break;
		case DOMAIN_OTAU_UPGRADE:
		if (SYS_TimerStarted(&otauUpgradeTimer))
		{
			SYS_TimerStop(&otauUpgradeTimer);
		}
		otauUpgradeTimer.interval = interval;
		otauUpgradeTimer.handler = otauUpgradeTimerHandler;
		if(TIMER_MODE_SINGLE == mode) {
			otauUpgradeTimer.mode = SYS_TIMER_INTERVAL_MODE;
			} else {
			otauUpgradeTimer.mode = SYS_TIMER_PERIODIC_MODE;
		}
		SYS_TimerStart(&otauUpgradeTimer);
		break;
		default:
		break;
	}
}

void otauTimerStop(otau_domain_t domain_code)
{
	switch (domain_code)
	{
		case DOMAIN_OTAU_NOTIFY:
		SYS_TimerStop(&otauNotifyTimer);
		break;
		case DOMAIN_OTAU_UPGRADE:
		SYS_TimerStop(&otauUpgradeTimer);
		break;
		default:
		break;
	}
}

uint8_t *get_node_address(addr_mode_t addr_mode)
{
	if (NATIVE_ADDR_MODE == addr_mode)
	{
		return (uint8_t *)&shortAddress;
	}
	else
	{
		return (uint8_t *)&myLongAddress;
	}
}

void otauLed(otau_led_t led_state)
{
	if (OTAU_LED_ON == led_state)
	{
		LED_On(LED0);
	}
	else if (OTAU_LED_OFF == led_state)
	{
		LED_Off(LED0);
	}
	else
	{
		LED_Toggle(LED0);
	}
}


void otauResetDevice(void)
{
	NVIC_SystemReset();
}

void reverseMemcpy(uint8_t *dst, uint8_t *src, uint8_t len)
{
	uint8_t i;
	for (i=0; i < len; ++i)
	{
		dst[len-1-i] = src[i];
	}
}

static bool getAddressFromDataHandleTable(uint8_t handle, uint8_t *domainId, uint8_t *addrMode, uint8_t *addr)
{
	uint8_t loopIndex;
	for (loopIndex = 0; loopIndex < DATA_HANDLE_TABLE_SIZE; loopIndex++)
	{
		if (dataHandleTable[loopIndex].dataHandle == handle)
		{
			*domainId = dataHandleTable[loopIndex].domainId;
			*addrMode = dataHandleTable[loopIndex].addrMode;
			memcpy(addr, &(dataHandleTable[loopIndex].nativeAddr), SHORT_ADDR_LEN);
			dataHandleTable[loopIndex].domainId = 0xFF;
			return true;
		}
	}
	return false;
}

static bool addDataHandleTableEntry(uint8_t domainId, addr_mode_t addrMode, uint8_t *addr, uint8_t handle)
{
	uint8_t loopIndex;
	for (loopIndex = 0; loopIndex < DATA_HANDLE_TABLE_SIZE; loopIndex++)
	{
		if (dataHandleTable[loopIndex].domainId == 0xFF)
		{
			dataHandleTable[loopIndex].domainId = domainId;
			dataHandleTable[loopIndex].addrMode = addrMode;
			dataHandleTable[loopIndex].dataHandle = handle;
			memcpy(&(dataHandleTable[loopIndex].nativeAddr), addr, SHORT_ADDR_LEN);
			return true;
		}
	}
	return false;
}

static void initDataHandleTable(void)
{
	uint8_t loopIndex;
	for (loopIndex = 0; loopIndex < DATA_HANDLE_TABLE_SIZE; loopIndex++)
	{
		dataHandleTable[loopIndex].domainId = 0xFF;
	}
}

#ifndef OTAU_SERVER
void otauSetServerDetails(uint8_t *addr)
{
	memcpy(&serverShortAddress, addr, NATIVE_ADDR_SIZE);
}

void otauGetServerDetails(uint8_t *addr)
{
	addr = (uint8_t *)&serverShortAddress;
}
#endif
#endif //#if defined(OTAU_ENABLED)
