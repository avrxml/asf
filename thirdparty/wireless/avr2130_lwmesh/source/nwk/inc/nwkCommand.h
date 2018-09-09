/**
 * \file nwkCommand.h
 *
 * \brief Network commands interface
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef _NWK_COMMAND_H_
#define _NWK_COMMAND_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_COMMAND_ACK                 = 0x00,
	NWK_COMMAND_ROUTE_ERROR         = 0x01,
	NWK_COMMAND_ROUTE_REQUEST       = 0x02,
	NWK_COMMAND_ROUTE_REPLY         = 0x03,
};
COMPILER_PACK_SET(1)
typedef struct  NwkCommandAck_t {
	uint8_t id;
	uint8_t seq;
	uint8_t control;
} NwkCommandAck_t;

typedef struct  NwkCommandRouteError_t {
	uint8_t id;
	uint16_t srcAddr;
	uint16_t dstAddr;
	uint8_t multicast;
} NwkCommandRouteError_t;

typedef struct  NwkCommandRouteRequest_t {
	uint8_t id;
	uint16_t srcAddr;
	uint16_t dstAddr;
	uint8_t multicast;
	uint8_t linkQuality;
} NwkCommandRouteRequest_t;

typedef struct  NwkCommandRouteReply_t {
	uint8_t id;
	uint16_t srcAddr;
	uint16_t dstAddr;
	uint8_t multicast;
	uint8_t forwardLinkQuality;
	uint8_t reverseLinkQuality;
} NwkCommandRouteReply_t;
COMPILER_PACK_RESET()
#endif /* _NWK_COMMAND_H_ */
