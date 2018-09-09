/**
 * \file nwkRx.h
 *
 * \brief Receive routines interface
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

#ifndef _NWK_RX_H_
#define _NWK_RX_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "nwkFrame.h"

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_IND_OPT_ACK_REQUESTED     = 1 << 0,
	NWK_IND_OPT_SECURED           = 1 << 1,
	NWK_IND_OPT_BROADCAST         = 1 << 2,
	NWK_IND_OPT_LOCAL             = 1 << 3,
	NWK_IND_OPT_BROADCAST_PAN_ID  = 1 << 4,
	NWK_IND_OPT_LINK_LOCAL        = 1 << 5,
	NWK_IND_OPT_MULTICAST         = 1 << 6,
};

typedef struct NWK_DataInd_t {
	uint16_t srcAddr;
	uint16_t dstAddr;
	uint8_t srcEndpoint;
	uint8_t dstEndpoint;
	uint8_t options;
	uint8_t *data;
	uint8_t size;
	uint8_t lqi;
	int8_t rssi;
} NWK_DataInd_t;

/*- Prototypes -------------------------------------------------------------*/
void NWK_SetAckControl(uint8_t control);

#ifdef NWK_ENABLE_ADDRESS_FILTER
bool NWK_FilterAddress(uint16_t addr, uint8_t *lqi);

#endif

void nwkRxInit(void);
void nwkRxDecryptConf(NwkFrame_t *frame, bool status);
void nwkRxTaskHandler(void);

#endif /* _NWK_RX_H_ */
