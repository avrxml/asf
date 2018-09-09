/**
 * \file nwkFrame.h
 *
 * \brief Frame buffers management interface
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

#ifndef _NWK_FRAME_H_
#define _NWK_FRAME_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "compiler.h"

/*- Definitions ------------------------------------------------------------*/
#define NWK_FRAME_MAX_PAYLOAD_SIZE   127

/*- Types ------------------------------------------------------------------*/
COMPILER_PACK_SET(1)
typedef struct  NwkFrameHeader_t {
	uint16_t macFcf;
	uint8_t macSeq;
	uint16_t macDstPanId;
	uint16_t macDstAddr;
	uint16_t macSrcAddr;

	struct {
		uint8_t ackRequest : 1;
		uint8_t security   : 1;
		uint8_t linkLocal  : 1;
		uint8_t multicast  : 1;
		uint8_t reserved   : 4;
	} nwkFcf;
	uint8_t nwkSeq;
	uint16_t nwkSrcAddr;
	uint16_t nwkDstAddr;
	struct {
		uint8_t nwkSrcEndpoint : 4;
		uint8_t nwkDstEndpoint : 4;
	};
} NwkFrameHeader_t;

typedef struct  NwkFrameMulticastHeader_t {
	uint16_t nonMemberRadius    : 4;
	uint16_t maxNonMemberRadius : 4;
	uint16_t memberRadius       : 4;
	uint16_t maxMemberRadius    : 4;
} NwkFrameMulticastHeader_t;

typedef struct NwkFrame_t {
	uint8_t state;
	uint8_t size;

	union {
		NwkFrameHeader_t header;
		uint8_t data[NWK_FRAME_MAX_PAYLOAD_SIZE];
	};

	uint8_t *payload;

	union {
		struct {
			uint8_t lqi;
			int8_t rssi;
		} rx;

		struct {
			uint8_t status;
			uint16_t timeout;
			uint8_t control;
			void (*confirm)(struct NwkFrame_t *frame);
		} tx;
	};
} NwkFrame_t;
COMPILER_PACK_RESET()
/*- Prototypes -------------------------------------------------------------*/
void nwkFrameInit(void);
NwkFrame_t *nwkFrameAlloc(void);
void nwkFrameFree(NwkFrame_t *frame);
NwkFrame_t *nwkFrameNext(NwkFrame_t *frame);
void nwkFrameCommandInit(NwkFrame_t *frame);

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static inline uint8_t nwkFramePayloadSize(NwkFrame_t *frame)
{
	return frame->size - (frame->payload - frame->data);
}

#endif /* _NWK_FRAME_H_ */
