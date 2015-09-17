/**
 * \file widbg_mgr.c
 *
 * \brief widbg_mgr module implementation
 *
 * Copyright (C) 2015, Atmel Corporation. All rights reserved.
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
 * This is the reference manual for the LWMesh OtauSvrMgr Application
 * //TODO
 */
/*- Includes ---------------------------------------------------------------*/
#include "stddef.h"
#if 0
#include "string.h"
#include "sysTypes.h"
#include "stdbool.h"
#include "config.h"
#include "widbg.h"
#include "widbg_mgr.h"
#include "sys.h"
#include "phy.h"
#include "nwk.h"
#include "sysTimer.h"
#include "halLed.h"
#include "widbg_common.h"
#include "widbg_upgrade.h"
#include "widbg_debug.h"
#ifdef WIDBG_SERVER
#include "widbg_mgr_server.h"
#include "widbg_parser.h"
#include "halUart.h"
#else
#include "widbg_mgr_client.h"
#endif
#include "sysTimer.h"
#ifndef WIDBG_SERVER
#include "widbg_nvm.h"
#endif

#define WIDBG_LED     0

NWK_DataReq_t widbgCommonDataReq;
NWK_DataReq_t widbgUpgradeDataReq;
NWK_DataReq_t widbgDebugDataReq;

uint8_t widbg_common_data[128];
uint8_t widbg_upgrade_data[128];
uint8_t widbg_debug_data[128];

static bool widbg_common_data_ind(NWK_DataInd_t *ind);
static bool widbg_upgrade_data_ind(NWK_DataInd_t *ind);
static bool widbg_debug_data_ind(NWK_DataInd_t *ind);
static void widbg_data_conf(NWK_DataReq_t *req);

static SYS_Timer_t widbgCommonTimer;
static SYS_Timer_t widbgUpgradeTimer;
static SYS_Timer_t widbgDebugTimer;

module_code_t last_phy_msg_code;

node_info_t node_info = {
	.image_start = 0x00002000,
	.image_size = 0x00002D7F,
	.new_image = 0,
	.supported_mode = 0x03,
	.firmware = "LWMesh    ",
	.firmware_version = "1.2.1",
	.board = "SAMR21XPro",
	.board_version = "1.0.0"
};

uint64_t ieee_address = IEEE_ADDRESS;
uint16_t short_address = APP_ADDR;
uint16_t pan_id = APP_PANID;

uint8_t seq_no = 0;

void widbg_mgr_data_req(module_code_t msg_code, addr_mode_t addr_mode, uint8_t *addr, void *payload, uint8_t len)
{
	NWK_DataReq_t *data_req;
	
	if(COMMON == msg_code)
	{
		data_req = &widbgCommonDataReq;
		data_req->dstEndpoint = WIDBG_COMMON_ENDPOINT;
		data_req->srcEndpoint = WIDBG_COMMON_ENDPOINT;
		memcpy(&widbg_common_data, payload, len);
		data_req->data = widbg_common_data;
		if(BROADCAST_MODE == addr_mode)
		{
			data_req->dstAddr = NWK_BROADCAST_ADDR;
		}
		else if(NATIVE_ADDR_MODE == addr_mode)
		{
			if(NULL != addr)
			{
				memcpy(&(data_req->dstAddr), addr, NATIVE_ADDR_SIZE);
			}
			else
			{
				#ifndef WIDBG_SERVER
				memcpy(&(data_req->dstAddr), &common_server_address.native_addr, NATIVE_ADDR_SIZE);
				#endif
			}
		}
	}
	else if (UPGRADE == msg_code)
	{
		data_req = &widbgUpgradeDataReq;
		data_req->dstEndpoint = WIDBG_UPGRADE_ENDPOINT;
		data_req->srcEndpoint = WIDBG_UPGRADE_ENDPOINT;
		memcpy(&widbg_upgrade_data, payload, len);
		data_req->data = widbg_upgrade_data;
		if(BROADCAST_MODE == addr_mode)
		{
			data_req->dstAddr = NWK_BROADCAST_ADDR;
		}
		else if(NATIVE_ADDR_MODE == addr_mode)
		{
			if(NULL != addr)
			{
				memcpy(&(data_req->dstAddr), addr, NATIVE_ADDR_SIZE);
			}
			else
			{
				#ifndef WIDBG_SERVER
				memcpy(&(data_req->dstAddr), &upgrade_server_address.native_addr, NATIVE_ADDR_SIZE);
				#endif
			}
		}
	}
	else if (DEBUG == msg_code)
	{
		data_req = &widbgDebugDataReq;
		data_req->dstEndpoint = WIDBG_DEBUG_ENDPOINT;
		data_req->srcEndpoint = WIDBG_DEBUG_ENDPOINT;
		memcpy(&widbg_debug_data, payload, len);
		data_req->data = widbg_debug_data;
		if(BROADCAST_MODE == addr_mode)
		{
			data_req->dstAddr = NWK_BROADCAST_ADDR;
		}
		else if(NATIVE_ADDR_MODE == addr_mode)
		{
			if(NULL != addr)
			{
				memcpy(&(data_req->dstAddr), addr, NATIVE_ADDR_SIZE);
			}
			else
			{
				#ifndef WIDBG_SERVER
				memcpy(&(data_req->dstAddr), &common_server_address.native_addr, NATIVE_ADDR_SIZE);
				#endif
			}
		}
	}
	else
	{
		data_req = NULL;
	}

	if (data_req != NULL)
	{
		if(NWK_BROADCAST_ADDR != data_req->dstAddr)
		{
			data_req->options = NWK_OPT_ACK_REQUEST;
		}
		else
		{
			data_req->options = 0;
		}
		#ifdef NWK_ENABLE_SECURITY
		data_req->options |= NWK_OPT_ENABLE_SECURITY;
		#endif

		data_req->size = len;
		data_req->confirm = widbg_data_conf;
		NWK_DataReq(data_req);
	}
}

static bool widbg_common_data_ind(NWK_DataInd_t *ind)
{
    if((NULL != ind) && (NULL != ind->data))
	{
       widbg_common_rcvd_frame(NATIVE_ADDR_MODE, (uint8_t *)&(ind->srcAddr), ind->size, ind->data,ind->lqi);
		return true;
	}
	return false;
}

static bool widbg_upgrade_data_ind(NWK_DataInd_t *ind)
{
	if((NULL != ind) && (NULL != ind->data))
	{
		widbg_upgrade_rcvd_frame(NATIVE_ADDR_MODE, (uint8_t *)&(ind->srcAddr), ind->size, ind->data,ind->lqi);
		return true;
	}
	return false;
}

static bool widbg_debug_data_ind(NWK_DataInd_t *ind)
{
	if((NULL != ind) && (NULL != ind->data))
	{
		widbg_debug_rcvd_frame(NATIVE_ADDR_MODE, (uint8_t *)&(ind->srcAddr), ind->size, ind->data,ind->lqi);
		return true;
	}
	return false;
}

static void widbg_data_conf(NWK_DataReq_t *req)
{
	uint8_t status;
	if (NULL != req)
	{
		switch (req->status)
		{
		case NWK_SUCCESS_STATUS:
			status = SUCCESS;
			break;
		case NWK_NO_ACK_STATUS:
		case NWK_PHY_NO_ACK_STATUS:
			status = NO_ACK;
			break;
		case NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS:
			status = CCA_FAILURE;
			break;
		default:
			status = ERROR;
		}
	}
	
    if(req == &widbgCommonDataReq)
    {
	    widbg_common_sent_frame(NATIVE_ADDR_MODE, (uint8_t *)&req->dstAddr, status);
    }
    else if(req == &widbgUpgradeDataReq)
    {
	    widbg_upgrade_sent_frame(NATIVE_ADDR_MODE, (uint8_t *)&req->dstAddr, status);
    }
    else if(req == &widbgDebugDataReq)
    {
	    widbg_debug_sent_frame(NATIVE_ADDR_MODE, (uint8_t *)&req->dstAddr, status);
    }
	else
	{
		/* Log error */
	}
}

/*****************************************************************************
*****************************************************************************/

void widbg_mgr_init(void)
{
#ifndef WIDBG_SERVER
	widbg_nvm_init();

	uint32_t* nvm_addr_loc = (uint32_t *)NVM_APP_SHORT_ADDR;
	reverse_memcpy((uint8_t *)&short_address, (uint8_t *)nvm_addr_loc, sizeof(short_address));
	if(short_address == 0xffff)
	{
		short_address = APP_ADDR;
	}

	nvm_addr_loc = (uint32_t *)NVM_APP_IEEE_ADDR;
	reverse_memcpy((uint8_t *)&ieee_address, (uint8_t *)nvm_addr_loc, sizeof(ieee_address));
	if(ieee_address == 0xffffffffffffffff || ieee_address == 0x0)
	{
		ieee_address = IEEE_ADDRESS;
	}

	nvm_addr_loc = (uint32_t *) NVM_DEVICE_INFO_LOCATION;
	if(0xFFFFFFFF == *nvm_addr_loc)
	{
		widbg_nvm_write(MEMORY_ABSOLUTE_ADDRESS, (uint32_t)nvm_addr_loc, sizeof(node_info_t),(uint8_t *)&node_info);
	}
	else
	{
		widbg_nvm_read(MEMORY_ABSOLUTE_ADDRESS, (uint32_t)nvm_addr_loc, sizeof(node_info_t),(uint8_t *)&node_info);
	}
#endif
	HAL_LedInit();
	widbg_common_init();
	widbg_upgrade_init();
	widbg_debug_init();

	PHY_SetIEEEAddr((uint8_t *)&ieee_address);
	PHY_EnableReservedFrameRx();

	NWK_SetAddr(short_address);
	NWK_SetPanId(APP_PANID);

	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);

	NWK_OpenEndpoint(WIDBG_COMMON_ENDPOINT, widbg_common_data_ind);
	NWK_OpenEndpoint(WIDBG_UPGRADE_ENDPOINT, widbg_upgrade_data_ind);
	NWK_OpenEndpoint(WIDBG_DEBUG_ENDPOINT, widbg_debug_data_ind);
}

void widbg_mgr_task(void)
{
#ifdef WIDBG_SERVER
	HAL_UartTaskHandler();
#endif
}

void widbg_mgr_timer_start(module_code_t msg_code, uint32_t interval, widbg_timer_mode_t mode, void *handler)
{
	widbg_log(LOG_INFO, LWMESH_SYS, ENTRY, 5, (uint8_t *)"Hello");
	widbg_trace(TRACE_ENTRY);
	switch (msg_code)
	{
	case COMMON:
		if (SYS_TimerStarted(&widbgCommonTimer))
		{
			SYS_TimerStop(&widbgCommonTimer);
		}
		widbgCommonTimer.interval = interval;
		widbgCommonTimer.handler = handler;
		if(TIMER_MODE_SINGLE == mode)
			widbgCommonTimer.mode = SYS_TIMER_INTERVAL_MODE;
		else
			widbgCommonTimer.mode = SYS_TIMER_PERIODIC_MODE;
		SYS_TimerStart(&widbgCommonTimer);
		break;
	case UPGRADE:
		if (SYS_TimerStarted(&widbgUpgradeTimer))
		{
			SYS_TimerStop(&widbgUpgradeTimer);
		}
		widbgUpgradeTimer.interval = interval;
		widbgUpgradeTimer.handler = handler;
		if(TIMER_MODE_SINGLE == mode)
		widbgUpgradeTimer.mode = SYS_TIMER_INTERVAL_MODE;
		else
		widbgUpgradeTimer.mode = SYS_TIMER_PERIODIC_MODE;
		SYS_TimerStart(&widbgUpgradeTimer);
		break;
	case DEBUG:
		if (SYS_TimerStarted(&widbgDebugTimer))
		{
			SYS_TimerStop(&widbgDebugTimer);
		}
		widbgDebugTimer.interval = interval;
		widbgDebugTimer.handler = handler;
		if(TIMER_MODE_SINGLE == mode)
		widbgDebugTimer.mode = SYS_TIMER_INTERVAL_MODE;
		else
		widbgDebugTimer.mode = SYS_TIMER_PERIODIC_MODE;
		SYS_TimerStart(&widbgDebugTimer);
		break;
	default:
		break;
	}
}

void widbg_mgr_timer_stop(module_code_t msg_code)
{
	switch (msg_code)
	{
		case COMMON:
			SYS_TimerStop(&widbgCommonTimer);
			break;
		case UPGRADE:
			SYS_TimerStop(&widbgUpgradeTimer);
			break;
		case DEBUG:
			SYS_TimerStop(&widbgDebugTimer);
			break;
		default:
			break;
	}
}

uint8_t *get_node_address(addr_mode_t addr_mode)
{
	if (NATIVE_ADDR_MODE == addr_mode)
	{
		return (uint8_t *)&short_address;
	}
	else
	{
		return (uint8_t *)&ieee_address;
	}
}

uint8_t *get_pan_id(void)
{
	return &pan_id;
}

void widbg_mgr_set_channel(uint8_t channel)
{
	PHY_SetChannel(channel);
}

void widbg_mgr_led(widbg_led_t led_state)
{
	if (WIDBG_LED_ON == led_state)
	{
		HAL_LedOn(WIDBG_LED);
	}
	else if (WIDBG_LED_OFF == led_state)
	{
		HAL_LedOff(WIDBG_LED);
	} 
	else
	{
		HAL_LedToggle(WIDBG_LED);
	}
}

void widbg_mgr_reset_device(void)
{
	NVIC_SystemReset();
}
#endif