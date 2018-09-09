/**
 * \file nwkRoute.h
 *
 * \brief Routing interface
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

#ifndef _NWK_ROUTE_H_
#define _NWK_ROUTE_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "nwkRx.h"
#include "nwkFrame.h"

/*- Definitions ------------------------------------------------------------*/
#define NWK_ROUTE_UNKNOWN            0xffff
#define NWK_ROUTE_NON_ROUTING        0x8000

#ifdef NWK_ENABLE_ROUTING

/*- Types ------------------------------------------------------------------*/
typedef struct NWK_RouteTableEntry_t {
	uint8_t fixed     : 1;
	uint8_t multicast : 1;
	uint8_t reserved  : 2;
	uint8_t score     : 4;
	uint16_t dstAddr;
	uint16_t nextHopAddr;
	uint8_t rank;
	uint8_t lqi;
} NWK_RouteTableEntry_t;

/*- Prototypes -------------------------------------------------------------*/
NWK_RouteTableEntry_t *NWK_RouteFindEntry(uint16_t dst, uint8_t multicast);
NWK_RouteTableEntry_t *NWK_RouteNewEntry(void);
void NWK_RouteFreeEntry(NWK_RouteTableEntry_t *entry);
uint16_t NWK_RouteNextHop(uint16_t dst, uint8_t multicast);
NWK_RouteTableEntry_t *NWK_RouteTable(void);

void nwkRouteInit(void);
void nwkRouteRemove(uint16_t dst, uint8_t multicast);
void nwkRouteFrameReceived(NwkFrame_t *frame);
void nwkRouteFrameSent(NwkFrame_t *frame);
void nwkRoutePrepareTx(NwkFrame_t *frame);
void nwkRouteFrame(NwkFrame_t *frame);
bool nwkRouteErrorReceived(NWK_DataInd_t *ind);
void nwkRouteUpdateEntry(uint16_t dst, uint8_t multicast, uint16_t nextHop,
		uint8_t lqi);

#endif /* NWK_ENABLE_ROUTING */

#endif /* _NWK_ROUTE_H_ */
