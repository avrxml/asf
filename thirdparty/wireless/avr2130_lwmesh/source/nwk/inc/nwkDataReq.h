/**
 * \file nwkDataReq.h
 *
 * \brief NWK_DataReq() interface
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

#ifndef _NWK_DATA_REQ_H_
#define _NWK_DATA_REQ_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "sysConfig.h"

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_OPT_ACK_REQUEST          = 1 << 0,
	NWK_OPT_ENABLE_SECURITY      = 1 << 1,
	NWK_OPT_BROADCAST_PAN_ID     = 1 << 2,
	NWK_OPT_LINK_LOCAL           = 1 << 3,
	NWK_OPT_MULTICAST            = 1 << 4,
};

typedef struct NWK_DataReq_t {
	/* service fields */
	void *next;
	void *frame;
	uint8_t state;

	/* request parameters */
	uint16_t dstAddr;
	uint8_t dstEndpoint;
	uint8_t srcEndpoint;
	uint8_t options;
#ifdef NWK_ENABLE_MULTICAST
	uint8_t memberRadius;
	uint8_t nonMemberRadius;
#endif
	uint8_t *data;
	uint8_t size;
	void (*confirm)(struct NWK_DataReq_t *req);

	/* confirmation parameters */
	uint8_t status;
	uint8_t control;
} NWK_DataReq_t;

/*- Prototypes -------------------------------------------------------------*/
void NWK_DataReq(NWK_DataReq_t *req);

void nwkDataReqInit(void);
void nwkDataReqTaskHandler(void);

#endif /* _NWK_DATA_REQ_H_ */
