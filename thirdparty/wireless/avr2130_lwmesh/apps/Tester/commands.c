/**
 * \file commands.c
 *
 * \brief Command handlers
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "tester.h"
#include "filter.h"
#include "events.h"
#include "time.h"
#include "commands.h"

/*- Definitions ------------------------------------------------------------*/
#define APP_NUMBER_OF_ENDPOINTS       16
#define APP_RESPONDER_ENDPOINT        15

/*- Types ------------------------------------------------------------------*/
typedef struct AppRequestBuffer_t {
	bool busy;
	uint8_t handle;
	NWK_DataReq_t req;
	uint8_t buf[NWK_MAX_PAYLOAD_SIZE];
} AppRequestBuffer_t;

/*- Prototypes -------------------------------------------------------------*/
static void appOpenEndpoint(uint8_t index, bool state);
static bool appDataInd(NWK_DataInd_t *ind);
static void appDataConf(NWK_DataReq_t *req);
static bool appResponderDataInd(NWK_DataInd_t *ind);

/*- Variables --------------------------------------------------------------*/
static AppRequestBuffer_t appRequestBuffer[APP_NUMBER_OF_REQUEST_BUFFERS];
static bool appEndpointAckState[APP_NUMBER_OF_ENDPOINTS];

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void appCommandsInit(void)
{
	for (uint8_t i = 0; i < APP_NUMBER_OF_ENDPOINTS; i++) {
		appEndpointAckState[i] = true;
	}

	for (uint8_t i = 0; i < APP_NUMBER_OF_REQUEST_BUFFERS; i++) {
		appRequestBuffer[i].busy = false;
		appRequestBuffer[i].req.data = appRequestBuffer[i].buf;
	}

	appOpenEndpoint(1, true);
	NWK_OpenEndpoint(APP_RESPONDER_ENDPOINT, appResponderDataInd);
}

/*************************************************************************//**
*****************************************************************************/
static void appOpenEndpoint(uint8_t index, bool state)
{
	NWK_OpenEndpoint(index, state ? appDataInd : NULL);
}

/*************************************************************************//**
*****************************************************************************/
#ifdef NWK_ENABLE_ROUTING
static void appCommandsSendRouteEntry(uint16_t index,
		NWK_RouteTableEntry_t *entry)
{
	AppCommandRouteEntry_t cmd;

	cmd.id          = APP_COMMAND_ROUTE_ENTRY;
	cmd.index       = index;
	cmd.fixed       = entry->fixed;
	cmd.multicast   = entry->multicast;
	cmd.score       = entry->score;
	cmd.dstAddr     = entry->dstAddr;
	cmd.nextHopAddr = entry->nextHopAddr;
	cmd.rank        = entry->rank;
	cmd.lqi         = entry->lqi;

	appUartSendCommand((uint8_t *)&cmd, sizeof(cmd));
}

#endif

/*************************************************************************//**
*****************************************************************************/
static void appCommandsSendDataInd(NWK_DataInd_t *ind)
{
	AppCommandDataInd_t cmd;

	cmd.id          = APP_COMMAND_DATA_IND;
	cmd.time        = appTimeGet();
	cmd.srcAddr     = ind->srcAddr;
	cmd.dstAddr     = ind->dstAddr;
	cmd.srcEndpoint = ind->srcEndpoint;
	cmd.dstEndpoint = ind->dstEndpoint;
	cmd.options     = ind->options;
	cmd.lqi         = ind->lqi;
	cmd.rssi        = ind->rssi;
	cmd.size        = ind->size;
	memcpy(cmd.data, ind->data, ind->size);

	appUartSendCommand((uint8_t *)&cmd,
			sizeof(cmd) - NWK_MAX_PAYLOAD_SIZE + ind->size);
}

/*************************************************************************//**
*****************************************************************************/
static void appCommandsSendDataConf(NWK_DataReq_t *req, uint8_t handle,
		uint32_t time)
{
	AppCommandDataConf_t cmd;

	cmd.id              = APP_COMMAND_DATA_CONF;
	cmd.handle          = handle;
	cmd.time            = time;
	cmd.status          = req->status;
	cmd.dstAddr         = req->dstAddr;
	cmd.dstEndpoint     = req->dstEndpoint;
	cmd.srcEndpoint     = req->srcEndpoint;
	cmd.options         = req->options;
#ifdef NWK_ENABLE_MULTICAST
	cmd.memberRadius    = req->memberRadius;
	cmd.nonMemberRadius = req->nonMemberRadius;
#else
	cmd.memberRadius    = 0;
	cmd.nonMemberRadius = 0;
#endif

	appUartSendCommand((uint8_t *)&cmd, sizeof(cmd));
}

/*************************************************************************//**
*****************************************************************************/
#ifdef APP_ENABLE_EVENTS_BUFFER
static void appCommandsSendEvents(void)
{
	AppCommandEvents_t cmd;

	cmd.id   = APP_COMMAND_EVENTS;
	cmd.size = appEventsGetSize();
	memcpy(cmd.events, appEventsGetBuffer(), cmd.size);
	appEventsClearBuffer();

	appUartSendCommand((uint8_t *)&cmd,
			sizeof(cmd) - APP_EVENTS_BUFFER_SIZE + cmd.size);
}

#endif

/*************************************************************************//**
*****************************************************************************/
static void appResponderDataConf(NWK_DataReq_t *req)
{
	for (uint8_t i = 0; i < APP_NUMBER_OF_REQUEST_BUFFERS; i++) {
		if (req == &appRequestBuffer[i].req) {
			appRequestBuffer[i].busy = false;
			break;
		}
	}
}

/*************************************************************************//**
*****************************************************************************/
static bool appResponderDataInd(NWK_DataInd_t *ind)
{
	NWK_DataReq_t *dataReq = NULL;
	uint8_t options = 0;

	if (ind->data[0] == 0xff) {
		return true;
	}

	for (uint8_t i = 0; i < APP_NUMBER_OF_REQUEST_BUFFERS; i++) {
		if (false == appRequestBuffer[i].busy) {
			appRequestBuffer[i].busy = true;
			dataReq = &appRequestBuffer[i].req;
			break;
		}
	}

	if (NULL == dataReq) {
		return false;
	}

	options
		|= (ind->options &
			NWK_IND_OPT_ACK_REQUESTED) ? NWK_OPT_ACK_REQUEST : 0;
	options
		|= (ind->options &
			NWK_IND_OPT_SECURED) ? NWK_OPT_ENABLE_SECURITY : 0;

	dataReq->dstAddr = ind->srcAddr;
	dataReq->dstEndpoint = ind->srcEndpoint;
	dataReq->srcEndpoint = ind->dstEndpoint;
	dataReq->options = options;
	dataReq->size = ind->size;
	memcpy(dataReq->data, ind->data, ind->size);
	dataReq->confirm = appResponderDataConf;

	NWK_DataReq(dataReq);

	return true;
}

/*************************************************************************//**
*****************************************************************************/
static bool appDataInd(NWK_DataInd_t *ind)
{
	appCommandsSendDataInd(ind);
	return appEndpointAckState[ind->dstEndpoint];
}

/*************************************************************************//**
*****************************************************************************/
static void appDataConf(NWK_DataReq_t *req)
{
	uint8_t handle = 0;
	uint32_t time = appTimeGet();

	for (uint8_t i = 0; i < APP_NUMBER_OF_REQUEST_BUFFERS; i++) {
		if (req == &appRequestBuffer[i].req) {
			appRequestBuffer[i].busy = false;
			handle = appRequestBuffer[i].handle;
			break;
		}
	}

	appCommandsSendDataConf(req, handle, time);
}

/*************************************************************************//**
*****************************************************************************/
static AppStatus_t appProcessDataReq(AppCommandDataReq_t *req)
{
	NWK_DataReq_t *dataReq = NULL;
	uint8_t size;

	for (uint8_t i = 0; i < APP_NUMBER_OF_REQUEST_BUFFERS; i++) {
		if (false == appRequestBuffer[i].busy) {
			appRequestBuffer[i].busy = true;
			appRequestBuffer[i].handle = req->handle;
			dataReq = &appRequestBuffer[i].req;
			break;
		}
	}

	if (NULL == dataReq) {
		return APP_STATUS_OUT_OF_MEMORY;
	}

	size = NWK_MAX_PAYLOAD_SIZE;

#ifdef NWK_ENABLE_SECURITY
	if (req->options & NWK_OPT_ENABLE_SECURITY) {
		size -= NWK_SECURITY_MIC_SIZE;
	}
#endif

#ifdef NWK_ENABLE_MULTICAST
	if (req->options & NWK_OPT_MULTICAST) {
		size -= NWK_MULTICAST_HEADER_SIZE;
	}
#endif

	if (req->size > size) {
		return APP_STATUS_INVALID_PARAMETERS;
	}

	dataReq->dstAddr = req->dstAddr;
	dataReq->dstEndpoint = req->dstEndpoint;
	dataReq->srcEndpoint = req->srcEndpoint;
	dataReq->options = req->options;
#ifdef NWK_ENABLE_MULTICAST
	dataReq->memberRadius = req->memberRadius;
	dataReq->nonMemberRadius = req->nonMemberRadius;
#endif
	dataReq->size = req->size;
	memcpy(dataReq->data, &req->data, req->size);
	dataReq->confirm = appDataConf;

	NWK_DataReq(dataReq);

	return APP_STATUS_SUCESS;
}

/*************************************************************************//**
*****************************************************************************/
AppStatus_t appCommandReceived(uint8_t *buf, uint16_t size)
{
	AppReceiveCommand_t *cmd = (AppReceiveCommand_t *)buf;

	switch (cmd->header.id) {
	case APP_COMMAND_DUMMY:
	{
	}
	break;

	case APP_COMMAND_RESET:
	{
		/* TODO: reset */
	}
	break;

	case APP_COMMAND_RANDOMIZE:
	{
		srand(cmd->randomize.rnd);
	}
	break;

#ifdef APP_ENABLE_EVENTS_BUFFER
	case APP_COMMAND_GET_EVENTS:
	{
		appCommandsSendEvents();
	}
	break;
#endif

	case APP_COMMAND_DATA_REQ:
	{
		return appProcessDataReq(&cmd->dataReq);
	}
	break;

	case APP_COMMAND_OPEN_ENDPOINT:
	{
		appOpenEndpoint(cmd->openEndpoint.index,
				cmd->openEndpoint.state);
	}
	break;

	case APP_COMMAND_SET_ACK_STATE:
	{
		appEndpointAckState[cmd->setAckState.index]
			= cmd->setAckState.state;
	}
	break;

	case APP_COMMAND_SET_ADDR:
	{
		NWK_SetAddr(cmd->setAddr.addr);
	}
	break;

	case APP_COMMAND_SET_PANID:
	{
		NWK_SetPanId(cmd->setPanId.panId);
	}
	break;

	case APP_COMMAND_SET_CHANNEL:
	{
		PHY_SetChannel(cmd->setChannel.channel);
    #ifdef PHY_AT86RF212
		PHY_SetBand(cmd->setChannel.band);
		PHY_SetModulation(cmd->setChannel.modulation);
    #endif
	}
	break;

	case APP_COMMAND_SET_RX_STATE:
	{
		PHY_SetRxState(cmd->setRxState.rxState);
	}
	break;

#ifdef NWK_ENABLE_SECURITY
	case APP_COMMAND_SET_SECURITY_KEY:
	{
		NWK_SetSecurityKey(cmd->setSecurityKey.securityKey);
	}
	break;
#endif

	case APP_COMMAND_SET_TX_POWER:
	{
		PHY_SetTxPower(cmd->setTxPower.txPower);
	}
	break;

#ifdef NWK_ENABLE_MULTICAST
	case APP_COMMAND_GROUP_ADD:
	{
		NWK_GroupAdd(cmd->groupAdd.group);
	}
	break;

	case APP_COMMAND_GROUP_REMOVE:
	{
		NWK_GroupRemove(cmd->groupRemove.group);
	}
	break;
#endif

#ifdef NWK_ENABLE_ROUTING
	case APP_COMMAND_ROUTE_ADD:
	{
		NWK_RouteTableEntry_t *entry;

		entry = NWK_RouteNewEntry();
		entry->fixed       = cmd->routeAdd.fixed;
		entry->multicast   = cmd->routeAdd.multicast;
		entry->dstAddr     = cmd->routeAdd.dstAddr;
		entry->nextHopAddr = cmd->routeAdd.nextHopAddr;
		entry->lqi         = cmd->routeAdd.lqi;
	}
	break;

	case APP_COMMAND_ROUTE_REMOVE:
	{
		NWK_RouteTableEntry_t *entry;

		entry = NWK_RouteFindEntry(cmd->routeRemove.dstAddr,
				cmd->routeRemove.multicast);

		if (entry) {
			if (cmd->routeRemove.removeFixed) {
				entry->fixed = 0;
			}

			NWK_RouteFreeEntry(entry);
		}
	}
	break;

	case APP_COMMAND_ROUTE_FLUSH:
	{
		NWK_RouteTableEntry_t *entry = NWK_RouteTable();

		for (uint16_t i = 0; i < NWK_ROUTE_TABLE_SIZE; i++, entry++) {
			if (NWK_ROUTE_UNKNOWN == entry->dstAddr) {
				continue;
			}

			if (cmd->routeFlush.removeFixed) {
				entry->fixed = 0;
			}

			NWK_RouteFreeEntry(entry);
		}
	}
	break;

	case APP_COMMAND_ROUTE_TABLE:
	{
		NWK_RouteTableEntry_t *entry = NWK_RouteTable();

		for (uint16_t i = 0; i < NWK_ROUTE_TABLE_SIZE; i++, entry++) {
			if (NWK_ROUTE_UNKNOWN != entry->dstAddr) {
				appCommandsSendRouteEntry(i, entry);
			}
		}
	}
	break;
#endif

#ifdef NWK_ENABLE_ADDRESS_FILTER
	case APP_COMMAND_FILTER_ADD:
	{
		if (!appFilterAdd(cmd->filterAdd.addr, cmd->filterAdd.allow,
				cmd->filterAdd.setLqi,
				cmd->filterAdd.lqi)) {
			return APP_STATUS_TABLE_IS_FULL;
		}
	}
	break;

	case APP_COMMAND_FILTER_REMOVE:
	{
		if (!appFilterRemove(cmd->filterRemove.addr)) {
			return APP_STATUS_ENTRY_NOT_FOUND;
		}
	}
	break;
#endif

	default:
		return APP_STATUS_UNKNOWN_COMMAND;
	}

	(void)size;
	return APP_STATUS_SUCESS;
}
