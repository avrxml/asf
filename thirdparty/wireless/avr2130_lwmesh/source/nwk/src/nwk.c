/**
 * \file nwk.c
 *
 * \brief Network layer management functions implementation
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

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "phy.h"
#include "sysConfig.h"
#include "nwkRx.h"
#include "nwkTx.h"
#include "nwkGroup.h"
#include "nwkFrame.h"
#include "nwkRoute.h"
#include "nwkSecurity.h"
#include "nwkRouteDiscovery.h"

/*- Variables --------------------------------------------------------------*/
NwkIb_t nwkIb;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*  @brief Initializes all network layer modules
*****************************************************************************/
void NWK_Init(void)
{
	nwkIb.nwkSeqNum = 0;
	nwkIb.macSeqNum = 0;
	nwkIb.addr = 0;
	nwkIb.lock = 0;

	for (uint8_t i = 0; i < NWK_ENDPOINTS_AMOUNT; i++) {
		nwkIb.endpoint[i] = NULL;
	}

	nwkTxInit();
	nwkRxInit();
	nwkFrameInit();
	nwkDataReqInit();

#ifdef NWK_ENABLE_ROUTING
	nwkRouteInit();
#endif

#ifdef NWK_ENABLE_SECURITY
	nwkSecurityInit();
#endif

#ifdef NWK_ENABLE_MULTICAST
	nwkGroupInit();
#endif

#ifdef NWK_ENABLE_ROUTE_DISCOVERY
	nwkRouteDiscoveryInit();
#endif
}

/*************************************************************************//**
*  @brief Sets network address of the node
*  @param[in] addr Adddress to set
*****************************************************************************/
void NWK_SetAddr(uint16_t addr)
{
	nwkIb.addr = addr;
	PHY_SetShortAddr(addr);
}

/*************************************************************************//**
*  @brief Sets network identifier (PAN) of the node
*  @param[in] panId PAN ID to set
*****************************************************************************/
void NWK_SetPanId(uint16_t panId)
{
	nwkIb.panId = panId;
	PHY_SetPanId(panId);
}

/*************************************************************************//**
*  @brief Registers callback @a ind for the endpoint @a endpoint
*  @param[in] id Endpoint index (1-15)
*  @param[in] handler Pointer to the callback function
*****************************************************************************/
void NWK_OpenEndpoint(uint8_t id, bool (*handler)(NWK_DataInd_t *ind))
{
	nwkIb.endpoint[id] = handler;
}

/*************************************************************************//**
*  @brief Checks if network layer is ready for sleep
*  @return @c true if network layer is ready for sleep or @c false otherwise
*****************************************************************************/
bool NWK_Busy(void)
{
	return nwkIb.lock > 0;
}

/*************************************************************************//**
*  @brief Increases the lock counter and sets a busy state
*****************************************************************************/
void NWK_Lock(void)
{
	nwkIb.lock++;
}

/*************************************************************************//**
*  @brief Decreases the lock counter and sets a free state if counter reaches 0
*****************************************************************************/
void NWK_Unlock(void)
{
	nwkIb.lock--;
}

/*************************************************************************//**
*  @brief Puts network layer to a sleeping state
*****************************************************************************/
void NWK_SleepReq(void)
{
	PHY_Sleep();
}

/*************************************************************************//**
*  @brief Puts network layer to an active state
*****************************************************************************/
void NWK_WakeupReq(void)
{
	PHY_Wakeup();
}

/*************************************************************************//**
*  @brief Calculates linearized value for the given value of the LQI
*  @param[in] lqi LQI value as provided by the transceiver
*  @return linearized value directly proportional to the probability of delivery
*****************************************************************************/
uint8_t NWK_LinearizeLqi(uint8_t lqi)
{
	const uint8_t val[] = { 3, 8, 26, 64, 128, 190, 230, 247, 252 };
	uint8_t cl = 25;

	for (uint8_t i = 0; i < sizeof(val); i++) {
		if (lqi < cl) {
			return val[i];
		}

		cl += 25;
	}

	return 255;
}

/*************************************************************************//**
*  @brief Network layer task handler
*****************************************************************************/
void NWK_TaskHandler(void)
{
	nwkRxTaskHandler();
	nwkTxTaskHandler();
	nwkDataReqTaskHandler();
#ifdef NWK_ENABLE_SECURITY
	nwkSecurityTaskHandler();
#endif
}
