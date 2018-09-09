/**
 * \file nwk.h
 *
 * \brief Network layer public interface
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

#ifndef _NWK_H_
#define _NWK_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "sysConfig.h"
#include "nwkRoute.h"
#include "nwkGroup.h"
#include "nwkSecurity.h"
#include "nwkDataReq.h"

/**
 * \ingroup group_lwmesh_nwk
 * @{
 */

/*- Definitions ------------------------------------------------------------*/
#define NWK_MAX_PAYLOAD_SIZE            (127 - 16 /*NwkFrameHeader_t*/ - \
	2 /*crc*/)

#define NWK_BROADCAST_PANID             0xffff
#define NWK_BROADCAST_ADDR              0xffff

#define NWK_ENDPOINTS_AMOUNT            16

/*- Types ------------------------------------------------------------------*/
typedef enum {
	NWK_SUCCESS_STATUS                      = 0x00,
	NWK_ERROR_STATUS                        = 0x01,
	NWK_OUT_OF_MEMORY_STATUS                = 0x02,

	NWK_NO_ACK_STATUS                       = 0x10,
	NWK_NO_ROUTE_STATUS                     = 0x11,

	NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS   = 0x20,
	NWK_PHY_NO_ACK_STATUS                   = 0x21,
} NWK_Status_t;

typedef struct NwkIb_t {
	uint16_t addr;
	uint16_t panId;
	uint8_t nwkSeqNum;
	uint8_t macSeqNum;
	bool(*endpoint[NWK_ENDPOINTS_AMOUNT]) (NWK_DataInd_t * ind);
#ifdef NWK_ENABLE_SECURITY
	uint32_t key[4];
#endif
	uint16_t lock;
} NwkIb_t;

/*- Variables --------------------------------------------------------------*/
extern NwkIb_t nwkIb;

/*- Prototypes -------------------------------------------------------------*/
void NWK_Init(void);
void NWK_SetAddr(uint16_t addr);
void NWK_SetPanId(uint16_t panId);
void NWK_OpenEndpoint(uint8_t id, bool (*handler)(NWK_DataInd_t *ind));
bool NWK_Busy(void);
void NWK_Lock(void);
void NWK_Unlock(void);
void NWK_SleepReq(void);
void NWK_WakeupReq(void);
void NWK_TaskHandler(void);

uint8_t NWK_LinearizeLqi(uint8_t lqi);

/**  @} */
#endif /* _NWK_H_ */
