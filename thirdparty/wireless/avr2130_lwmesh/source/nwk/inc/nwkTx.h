/**
 * \file nwkTx.h
 *
 * \brief Transmit routines interface
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

#ifndef _NWK_TX_H_
#define _NWK_TX_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "nwkRx.h"
#include "nwkFrame.h"

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_TX_CONTROL_BROADCAST_PAN_ID = 1 << 0,
	NWK_TX_CONTROL_ROUTING          = 1 << 1,
	NWK_TX_CONTROL_DIRECT_LINK      = 1 << 2,
};

/*- Prototypes -------------------------------------------------------------*/
void nwkTxInit(void);
void nwkTxFrame(NwkFrame_t *frame);
void nwkTxBroadcastFrame(NwkFrame_t *frame);
bool nwkTxAckReceived(NWK_DataInd_t *ind);
void nwkTxConfirm(NwkFrame_t *frame, uint8_t status);
void nwkTxEncryptConf(NwkFrame_t *frame);
void nwkTxTaskHandler(void);

#endif /* _NWK_TX_H_ */
