/**
 * \file commands.h
 *
 * \brief Command structures definitions
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

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/*- Includes ---------------------------------------------------------------*/
#include "compiler.h"
#include "config.h"

/*- Types ------------------------------------------------------------------*/
typedef enum AppCommandId_t {
	APP_COMMAND_ACK              = 0x00,
	APP_COMMAND_DUMMY            = 0x01,
	APP_COMMAND_RESET            = 0x02,
	APP_COMMAND_RANDOMIZE        = 0x03,
	APP_COMMAND_GET_EVENTS       = 0x04,
	APP_COMMAND_EVENTS           = 0x05,

	APP_COMMAND_DATA_REQ         = 0x10,
	APP_COMMAND_DATA_CONF        = 0x11,
	APP_COMMAND_DATA_IND         = 0x12,
	APP_COMMAND_OPEN_ENDPOINT    = 0x13,
	APP_COMMAND_SET_ACK_STATE    = 0x14,
	APP_COMMAND_SET_ACK_CONTROL  = 0x15,

	APP_COMMAND_SET_ADDR         = 0x20,
	APP_COMMAND_SET_PANID        = 0x21,
	APP_COMMAND_SET_CHANNEL      = 0x22,
	APP_COMMAND_SET_RX_STATE     = 0x23,
	APP_COMMAND_SET_SECURITY_KEY = 0x24,
	APP_COMMAND_SET_TX_POWER     = 0x25,

	APP_COMMAND_GROUP_ADD        = 0x50,
	APP_COMMAND_GROUP_REMOVE     = 0x51,

	APP_COMMAND_ROUTE_ADD        = 0x60,
	APP_COMMAND_ROUTE_REMOVE     = 0x61,
	APP_COMMAND_ROUTE_FLUSH      = 0x62,
	APP_COMMAND_ROUTE_TABLE      = 0x63,
	APP_COMMAND_ROUTE_ENTRY      = 0x64,

	APP_COMMAND_FILTER_ADD       = 0x71,
	APP_COMMAND_FILTER_REMOVE    = 0x72,
} AppCommandId_t;

COMPILER_PACK_SET(1)

typedef struct {
	uint8_t id;
} AppCommandHeader_t;

typedef struct {
	uint8_t id;
	uint8_t status;
	uint32_t time;
} AppCommandAck_t;

typedef struct {
	uint8_t id;
} AppCommandDummy_t;

typedef struct {
	uint8_t id;
} AppCommandReset_t;

typedef struct {
	uint8_t id;
	uint16_t rnd;
} AppCommandRandomize_t;

typedef struct {
	uint8_t id;
} AppCommandGetEvents_t;

typedef struct {
	uint8_t id;
	uint16_t size;
	uint8_t events[APP_EVENTS_BUFFER_SIZE];
} AppCommandEvents_t;

typedef struct {
	uint8_t id;
	uint8_t handle;
	uint16_t dstAddr;
	uint8_t dstEndpoint;
	uint8_t srcEndpoint;
	uint8_t options;
	uint8_t memberRadius;
	uint8_t nonMemberRadius;
	uint8_t size;
	uint8_t data; /* An array actually, uint8_t is used to retrieve a
	               * pointer */
} AppCommandDataReq_t;

typedef struct {
	uint8_t id;
	uint8_t handle;
	uint8_t status;
	uint32_t time;
	uint16_t dstAddr;
	uint8_t dstEndpoint;
	uint8_t srcEndpoint;
	uint8_t options;
	uint8_t memberRadius;
	uint8_t nonMemberRadius;
} AppCommandDataConf_t;

typedef struct {
	uint8_t id;
	uint32_t time;
	uint16_t srcAddr;
	uint16_t dstAddr;
	uint8_t srcEndpoint;
	uint8_t dstEndpoint;
	uint8_t options;
	uint8_t lqi;
	int8_t rssi;
	uint8_t size;
	uint8_t data[NWK_MAX_PAYLOAD_SIZE];
} AppCommandDataInd_t;

typedef struct {
	uint8_t id;
	uint8_t index;
	uint8_t state;
} AppCommandOpenEndpoint_t;

typedef struct {
	uint8_t id;
	uint8_t index;
	uint8_t state;
} AppCommandSetAckState_t;

typedef struct {
	uint8_t id;
	uint16_t addr;
} AppCommandSetAddr_t;

typedef struct {
	uint8_t id;
	uint16_t panId;
} AppCommandSetPanId_t;

typedef struct {
	uint8_t id;
	uint8_t channel;
	uint8_t band;
	uint8_t modulation;
} AppCommandSetChannel_t;

typedef struct {
	uint8_t id;
	uint8_t rxState;
} AppCommandSetRxState_t;

typedef struct {
	uint8_t id;
	uint8_t securityKey[16];
} AppCommandSetSecurityKey_t;

typedef struct {
	uint8_t id;
	uint8_t txPower;
} AppCommandSetTxPower_t;

typedef struct {
	uint8_t id;
	uint16_t group;
} AppCommandGroupAdd_t;

typedef struct {
	uint8_t id;
	uint16_t group;
} AppCommandGroupRemove_t;

typedef struct {
	uint8_t id;
	uint8_t fixed;
	uint8_t multicast;
	uint16_t dstAddr;
	uint16_t nextHopAddr;
	uint8_t lqi;
} AppCommandRouteAdd_t;

typedef struct {
	uint8_t id;
	uint16_t dstAddr;
	uint8_t multicast;
	uint8_t removeFixed;
} AppCommandRouteRemove_t;

typedef struct {
	uint8_t id;
	uint8_t removeFixed;
} AppCommandRouteFlush_t;

typedef struct {
	uint8_t id;
} AppCommandRouteTable_t;

typedef struct {
	uint8_t id;
	uint16_t index;
	uint8_t fixed;
	uint8_t multicast;
	uint8_t score;
	uint16_t dstAddr;
	uint16_t nextHopAddr;
	uint8_t rank;
	uint8_t lqi;
} AppCommandRouteEntry_t;

typedef struct {
	uint8_t id;
	uint16_t addr;
	uint8_t allow;
	uint8_t setLqi;
	uint8_t lqi;
} AppCommandFilterAdd_t;

typedef struct {
	uint8_t id;
	uint16_t addr;
} AppCommandFilterRemove_t;

typedef union {
	AppCommandHeader_t header;
	AppCommandDummy_t dummy;
	AppCommandReset_t reset;
	AppCommandRandomize_t randomize;
#ifdef APP_ENABLE_EVENTS_BUFFER
	AppCommandGetEvents_t getEvents;
	AppCommandEvents_t events;
#endif

	AppCommandDataReq_t dataReq;
	AppCommandOpenEndpoint_t openEndpoint;
	AppCommandSetAckState_t setAckState;

	AppCommandSetAddr_t setAddr;
	AppCommandSetPanId_t setPanId;
	AppCommandSetChannel_t setChannel;
	AppCommandSetRxState_t setRxState;
#ifdef NWK_ENABLE_SECURITY
	AppCommandSetSecurityKey_t setSecurityKey;
#endif
	AppCommandSetTxPower_t setTxPower;

#ifdef NWK_ENABLE_MULTICAST
	AppCommandGroupAdd_t groupAdd;
	AppCommandGroupRemove_t groupRemove;
#endif

#ifdef NWK_ENABLE_ROUTING
	AppCommandRouteAdd_t routeAdd;
	AppCommandRouteRemove_t routeRemove;
	AppCommandRouteFlush_t routeFlush;
	AppCommandRouteTable_t routeTable;
#endif

#ifdef NWK_ENABLE_ADDRESS_FILTER
	AppCommandFilterAdd_t filterAdd;
	AppCommandFilterRemove_t filterRemove;
#endif
} AppReceiveCommand_t;
COMPILER_PACK_RESET()
/*- Prototypes -------------------------------------------------------------*/
void appCommandsInit(void);
void appUartSendCommand(uint8_t *buf, uint16_t size);

#endif /* _COMMANDS_H_ */
