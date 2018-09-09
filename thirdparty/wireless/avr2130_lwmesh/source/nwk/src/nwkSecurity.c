/**
 * \file nwkSecurity.c
 *
 * \brief Security routines implementation
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
#include "sysConfig.h"
#include "sysEncrypt.h"
#include "nwk.h"
#include "nwkTx.h"
#include "nwkFrame.h"
#include "nwkSecurity.h"

#ifdef NWK_ENABLE_SECURITY

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_SECURITY_STATE_ENCRYPT_PENDING = 0x30,
	NWK_SECURITY_STATE_DECRYPT_PENDING = 0x31,
	NWK_SECURITY_STATE_PROCESS         = 0x32,
	NWK_SECURITY_STATE_WAIT            = 0x33,
	NWK_SECURITY_STATE_CONFIRM         = 0x34,
};

/*- Variables --------------------------------------------------------------*/
static uint8_t nwkSecurityActiveFrames;
static NwkFrame_t *nwkSecurityActiveFrame;
static uint8_t nwkSecuritySize;
static uint8_t nwkSecurityOffset;
static bool nwkSecurityEncrypt;
static uint32_t nwkSecurityVector[4];

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*  @brief Initializes the Security module
*****************************************************************************/
void nwkSecurityInit(void)
{
	nwkSecurityActiveFrames = 0;
	nwkSecurityActiveFrame = NULL;
}

/*************************************************************************//**
*****************************************************************************/
void NWK_SetSecurityKey(uint8_t *key)
{
	memcpy((uint8_t *)nwkIb.key, key, NWK_SECURITY_KEY_SIZE);
}

/*************************************************************************//**
*****************************************************************************/
void nwkSecurityProcess(NwkFrame_t *frame, bool encrypt)
{
	if (encrypt) {
		frame->state = NWK_SECURITY_STATE_ENCRYPT_PENDING;
	} else {
		frame->state = NWK_SECURITY_STATE_DECRYPT_PENDING;
	}

	++nwkSecurityActiveFrames;
}

/*************************************************************************//**
*****************************************************************************/
static void nwkSecurityStart(void)
{
	NwkFrameHeader_t *header = &nwkSecurityActiveFrame->header;

	nwkSecurityVector[0] = header->nwkSeq;
	nwkSecurityVector[1]
		= ((uint32_t)header->nwkDstAddr <<
			16) | header->nwkDstEndpoint;
	nwkSecurityVector[2]
		= ((uint32_t)header->nwkSrcAddr <<
			16) | header->nwkSrcEndpoint;
	nwkSecurityVector[3]
		= ((uint32_t)header->macDstPanId <<
			16) | *(uint8_t *)&header->nwkFcf;

	if (NWK_SECURITY_STATE_DECRYPT_PENDING ==
			nwkSecurityActiveFrame->state) {
		nwkSecurityActiveFrame->size -= NWK_SECURITY_MIC_SIZE;
	}

	nwkSecuritySize = nwkFramePayloadSize(nwkSecurityActiveFrame);
	nwkSecurityOffset = 0;
	nwkSecurityEncrypt
		= (NWK_SECURITY_STATE_ENCRYPT_PENDING ==
			nwkSecurityActiveFrame->state);

	nwkSecurityActiveFrame->state = NWK_SECURITY_STATE_PROCESS;
}

/*************************************************************************//**
*****************************************************************************/
void SYS_EncryptConf(void)
{
	uint8_t *vector = (uint8_t *)nwkSecurityVector;
	uint8_t *text = &nwkSecurityActiveFrame->payload[nwkSecurityOffset];
	uint8_t block;

	block
		= (nwkSecuritySize <
			NWK_SECURITY_BLOCK_SIZE) ? nwkSecuritySize :
			NWK_SECURITY_BLOCK_SIZE;

	for (uint8_t i = 0; i < block; i++) {
		text[i] ^= vector[i];

		if (nwkSecurityEncrypt) {
			vector[i] = text[i];
		} else {
			vector[i] ^= text[i];
		}
	}

	nwkSecurityOffset += block;
	nwkSecuritySize -= block;

	if (nwkSecuritySize > 0) {
		nwkSecurityActiveFrame->state = NWK_SECURITY_STATE_PROCESS;
	} else {
		nwkSecurityActiveFrame->state = NWK_SECURITY_STATE_CONFIRM;
	}
}

/*************************************************************************//**
*****************************************************************************/
static bool nwkSecurityProcessMic(void)
{
	uint8_t *mic = &nwkSecurityActiveFrame->payload[nwkSecurityOffset];
	uint32_t vmic = nwkSecurityVector[0] ^ nwkSecurityVector[1] ^
			nwkSecurityVector[2] ^ nwkSecurityVector[3];
	uint32_t tmic;

	if (nwkSecurityEncrypt) {
		memcpy(mic, (uint8_t *)&vmic, NWK_SECURITY_MIC_SIZE);
		nwkSecurityActiveFrame->size += NWK_SECURITY_MIC_SIZE;
		return true;
	} else {
		memcpy((uint8_t *)&tmic, mic, NWK_SECURITY_MIC_SIZE);
		return vmic == tmic;
	}
}

/*************************************************************************//**
*  @brief Security Module task handler
*****************************************************************************/
void nwkSecurityTaskHandler(void)
{
	NwkFrame_t *frame = NULL;

	if (0 == nwkSecurityActiveFrames) {
		return;
	}

	if (nwkSecurityActiveFrame) {
		if (NWK_SECURITY_STATE_CONFIRM ==
				nwkSecurityActiveFrame->state) {
			bool micStatus = nwkSecurityProcessMic();

			if (nwkSecurityEncrypt) {
				nwkTxEncryptConf(nwkSecurityActiveFrame);
			} else {
				nwkRxDecryptConf(nwkSecurityActiveFrame,
						micStatus);
			}

			nwkSecurityActiveFrame = NULL;
			--nwkSecurityActiveFrames;
		} else if (NWK_SECURITY_STATE_PROCESS ==
				nwkSecurityActiveFrame->state) {
			nwkSecurityActiveFrame->state = NWK_SECURITY_STATE_WAIT;
			SYS_EncryptReq((uint32_t *)nwkSecurityVector,
					(uint32_t *)nwkIb.key);
		}

		return;
	}

	while (NULL != (frame = nwkFrameNext(frame))) {
		if (NWK_SECURITY_STATE_ENCRYPT_PENDING == frame->state ||
				NWK_SECURITY_STATE_DECRYPT_PENDING ==
				frame->state) {
			nwkSecurityActiveFrame = frame;
			nwkSecurityStart();
			return;
		}
	}
}

#endif /* NWK_ENABLE_SECURITY */
