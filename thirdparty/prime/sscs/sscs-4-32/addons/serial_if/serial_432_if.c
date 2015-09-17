/**
 * \file
 *
 * \brief PRIME Serial Interface for IEC-432
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
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
 */

#include "compiler.h"
#include "conf_usi.h"
#include "serial_432_if.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup serial_sscs_prime_group
 * @{
 */

/* System includes */
#include <stdio.h>
#include "string.h"

/* From module: PLC Universal Serial Interface */
#include "usi.h"

/* Board config */
#include "conf_board.h"

/* sscs432 */
#include "sscs432.h"
#include "dl432.h"

static uint8_t *rxPtr;  /* Ptr to rxMsg */
static uint16_t txLen;  /* Data length in txMsg */

/* Store callback information */
static dl432_LlcCallback_t callBackCmd;

#pragma message("SSCS432_CFG: SSCS432_USI_SERIALIZATION")
static uint8_t puc_sscs432_tx_buff[SSCS432_DATA_SIZE];
static uint8_t puc_sscs432_cmd_to_pack[SSCS432_DATA_SIZE];
static sscs432_NotBase_t x_base_not;
static sscs432_NotService_t x_service_not;


/** @brief	This function extracts the 432 SAP Establish.Request.
 *
 * @param	-
 *
 * @return
 * - 0 primitive stored
 * - -1 primitive could not be stored
 *
 * This function extracts the 432 SAP Establish.Request from the
 * received packet
 */

static int _sscs432_Unpack_Cl432EstablishRequest(void)
{
	uint8_t *rxBufferPtr;
	uint8_t lenSerialNumber;

	/* Extract 432 primitive */
	rxBufferPtr = rxPtr;
	lenSerialNumber = *rxBufferPtr++;

	/* Store the primitive in the circular buffer */
	return (sscs432_establish_request((uint8_t *)rxBufferPtr, lenSerialNumber));
}


/** @brief	This function extracts the 432 SAP Release.Request.
 *
 * @param	-
 *
 * @return
 * - 0 primitive stored
 * - -1 primitive could not be stored
 *
 * This function extracts the 432 SAP Release.Request from the
 * received packet
 */

static int _sscs432_Unpack_Cl432ReleaseRequest(void)
{
	uint8_t *rxBufferPtr;
	uint16_t dstAddress;

	/* Extract 432 primitive */
	rxBufferPtr = rxPtr;
	dstAddress = (((uint16_t)rxBufferPtr[0]) << 8) + (uint16_t)rxBufferPtr[1];

	/* Store the primitive in the circular buffer */
	return (sscs432_release_request(dstAddress));
}


/** @brief	This function extracts the 432 SAP Data.Request.
 *
 * @param	-
 *
 * @return
 * - 0 primitive stored
 * - -1 primitive could not be stored
 *
 * This function extracts the 432 SAP Data.Request from the
 * received packet
 */

static int _sscs432_Unpack_Dl432DataRequest(void)
{
	uint8_t *rxBufferPtr;
	uint8_t dstLSAP;
	uint8_t srcLSAP;
	uint16_t dstAddress;
	uint16_t lenMsg;
	uint8_t linkClass;

	/* Extract 432 primitive */
	rxBufferPtr = rxPtr;
	dstLSAP = *rxBufferPtr++;
	srcLSAP = *rxBufferPtr++;
	dstAddress = (((uint16_t)rxBufferPtr[0]) << 8) + (uint16_t)rxBufferPtr[1];
	rxBufferPtr += 2;
	lenMsg = (((uint16_t)rxBufferPtr[0]) << 8) + (uint16_t)rxBufferPtr[1];
	rxBufferPtr += 2;
	linkClass = *rxBufferPtr++;

	/* Store the primitive in the circular buffer */
	return (dl432_dl_data_request(dstLSAP, srcLSAP, dstAddress,
	       (dl432Buffer_t *)rxBufferPtr, lenMsg, linkClass));
}


/** @brief	This function extracts the 432 SAP Update Reply.Request.
 *
 * @param	-
 *
 * @return
 * - 0 primitive stored
 * - -1 primitive could not be stored
 *
 * This function extracts the 432 SAP  Update Reply.Request from the
 * received packet
 */

static int _sscs432_Unpack_Dl432UpdateReplyRequest(void)
{
	uint8_t *rxBufferPtr;
	uint8_t srcLSAP;
	uint16_t lenMsg;

	/* Extract 432 primitive */
	rxBufferPtr = rxPtr;
	srcLSAP = *rxBufferPtr++;
	lenMsg = (((uint16_t)rxBufferPtr[0]) << 8) + (uint16_t)rxBufferPtr[1];
	rxBufferPtr++;

	/* Store the primitive in the circular buffer */
	return (dl432_dl_update_reply_request(srcLSAP, (uint8_t *)rxBufferPtr, lenMsg));
}


/** @brief	This function extracts the 432 SAP Reply.Request.
 *
 * @param	-
 *
 * @return
 * - 0 primitive stored
 * - -1 primitive could not be stored
 *
 * This function extracts the 432 SAP Reply.Request from the
 * received packet
 */

static int _sscs432_Unpack_Dl432ReplyRequest(void)
{
	uint8_t *rxBufferPtr;
	uint8_t dstLSAP;
	uint8_t srcLSAP;
	uint16_t dstAddress;
	uint8_t maxLenAnswer;

	/* Extract 432 primitive */
	rxBufferPtr = rxPtr;
	dstLSAP = *rxBufferPtr++;
	srcLSAP = *rxBufferPtr++;
	dstAddress = (((uint16_t)rxBufferPtr[0]) << 8) + (uint16_t)rxBufferPtr[1];
	rxBufferPtr += 2;
	maxLenAnswer = *rxBufferPtr++;

	/* Store the primitive in the circular buffer */
	return (dl432_dl_reply_request(dstLSAP, srcLSAP, dstAddress, maxLenAnswer));
}


/** @brief	This function extracts the 432 SAP Broadcast.Request.
 *
 * @param	-
 *
 * @return
 * - 0 primitive stored
 * - -1 primitive could not be stored
 *
 * This function extracts the 432 SAP Broadcast.Request from the
 * received packet
 */

static int _sscs432_Unpack_Dl432BroadcastRequest(void)
{
	uint8_t *rxBufferPtr;
	uint8_t srcLSAP;
	uint16_t lenMsg;
	uint8_t linkClass;

	/* Extract 432 primitive */
	rxBufferPtr = rxPtr;
	srcLSAP = *rxBufferPtr++;
	lenMsg = (((uint16_t)rxBufferPtr[0]) << 8) + (uint16_t)rxBufferPtr[1];
	rxBufferPtr += 2;
	linkClass = *rxBufferPtr++;

	/* Store the primitive in the circular buffer */
	return (dl432_dl_broadcast_request(srcLSAP, (dl432Buffer_t *)rxBufferPtr,
	       lenMsg, linkClass));
}


/** @brief	Packs a 432 message
 *
 * @param	callCmd Pointer to the callback command
 * @return	SSCS432_OK
 *
 * Packs a 432 message
 */
static int8_t _sscs432_Pack_432Msg(dl432_LlcCallback_t *callCmd)
{
	uint8_t *txBufferPtr;
	uint16_t headerLen;

	/* Fill in received data, leaving space for required overhead */
	txBufferPtr = puc_sscs432_tx_buff;
	*txBufferPtr++ = callCmd->cmd | SET_TYPE_432_PROTOCOL(DATA_TYPE);
	*txBufferPtr++ = callCmd->dstLSAP;
	*txBufferPtr++ = callCmd->srcLSAP;
	*txBufferPtr++ = callCmd->status;
	*txBufferPtr++ = (uint8_t)(callCmd->dstAddress >> 8);
	*txBufferPtr++ = (uint8_t)callCmd->dstAddress;
	*txBufferPtr++ = (uint8_t)(callCmd->srcAddress >> 8);
	*txBufferPtr++ = (uint8_t)callCmd->srcAddress;
	*txBufferPtr++ = (uint8_t)(callCmd->length >> 8);
	*txBufferPtr++ = (uint8_t)callCmd->length;
	*txBufferPtr++ = callCmd->linkClass;

	/* Calculate header length */
	headerLen = (uint16_t)(txBufferPtr - puc_sscs432_tx_buff);

	/* Insert data */
	memcpy(txBufferPtr, &puc_sscs432_cmd_to_pack[0],
			LPDU_HEADER + callCmd->length);

	/* Calculate message length */
	txLen = headerLen + callCmd->length + LPDU_HEADER;

	return (SSCS432_OK);
}


/** @brief	Packs a 432 message
 *
 * @param	ptrNotService Pointer to a notification
 *
 * @return	SSCS432_OK
 *
 * Packs a 432 message
 */

static int8_t _sscs432_Pack_432NotService(sscs432_NotService_t *ptrNotService)
{
	uint8_t *txBufferPtr;
	uint16_t i;

	/* Fill in received data, leaving space for required overhead */
	txBufferPtr = puc_sscs432_tx_buff;
	*txBufferPtr++ = ptrNotService->cmd | SET_TYPE_432_PROTOCOL(CTRL_TYPE);
	*txBufferPtr++ = (uint8_t)(ptrNotService->address >> 8);
	*txBufferPtr++ = (uint8_t)ptrNotService->address;
	*txBufferPtr++ = ptrNotService->lenSerialNumber;
	for (i = 0; i < LEN_432_SERIAL_NUMBER; i++) {
		*txBufferPtr++ = ptrNotService->serialNumber[i];
	}

	*txBufferPtr++ = (uint8_t)(ptrNotService->baseAddress >> 8);
	*txBufferPtr++ = (uint8_t)ptrNotService->baseAddress;
	*txBufferPtr++ = ptrNotService->arq;

	/* Calculate message length */
	txLen = (uint16_t)(txBufferPtr - puc_sscs432_tx_buff);

	return (SSCS432_OK);
}


/** @brief	Packs a 432 message
 *
 * @param	ptrNotBase Pointer to a notification
 *
 * @return	SSCS432_OK
 *
 * Packs a 432 message
 */
static int8_t _sscs432_Pack_432NotBase(sscs432_NotBase_t *ptrNotBase)
{
	uint8_t *txBufferPtr;
	uint16_t i;

	/* Fill in received data, leaving space for required overhead */
	txBufferPtr = puc_sscs432_tx_buff;
	*txBufferPtr++ = ptrNotBase->cmd | SET_TYPE_432_PROTOCOL(CTRL_TYPE);
	*txBufferPtr++ = (uint8_t)(ptrNotBase->address >> 8);
	*txBufferPtr++ = (uint8_t)ptrNotBase->address;
	*txBufferPtr++ = ptrNotBase->lenSerialNumber;
	for (i = 0; i < LEN_432_SERIAL_NUMBER; i++) {
		*txBufferPtr++ = ptrNotBase->serialNumber[i];
	}

	for (i = 0; i < MAC_LENGTH; i++) {
		*txBufferPtr++ = ptrNotBase->macAddress[i];
	}

	*txBufferPtr++ = ptrNotBase->arq;

	/* Calculate message length */
	txLen = (uint16_t)(txBufferPtr - puc_sscs432_tx_buff);

	return (SSCS432_OK);
}

/**
 * \brief Received message
 *
 * \note Process Dialogue with peer
 *      - Proces Profiles
 *      - Wait for received message
 *      - Process incoming message
 *      - Send response if necessary
 *
 * \param puc_rx_msg  Pointer to the received data
 * \param us_len      Data length of the received data
 *
 * \retval 1 primitive stored
 * \retval 0 primitive could not be stored
 */
uint8_t serial_432_if_api_parser(uint8_t *puc_rx_msg, uint16_t us_len)
{
	uint8_t cmd, type;
	int8_t result = SSCS432_ERROR;

	/* Check if it is data or control msg */
	type = GET_TYPE_432_PROTOCOL(puc_rx_msg[0]);

	/* Extract command */
	cmd = CMD_432_PROTOCOL(puc_rx_msg[0]);

	/* Pointer to message data */
	rxPtr = (uint8_t *)&puc_rx_msg[1];
	UNUSED(us_len);

	if (type) {
		/* Extract and store MAC control message depending on the
		 * primitive */
		switch (cmd) {
		case CL_432_ESTABLISH_REQUEST:
			result = _sscs432_Unpack_Cl432EstablishRequest();
			break;

		case CL_432_RELEASE_REQUEST:
			result = _sscs432_Unpack_Cl432ReleaseRequest();
			break;

		default:
			/* Primitive is ignored */
			result = SSCS432_OK;
		}
	} else {
		/* Extract and store MAC data msg depending on the primitive */
		switch (cmd) {
		case DL432_DATA_REQUEST:
			result = _sscs432_Unpack_Dl432DataRequest();
			break;

		case DL432_UPDATE_REPLY_REQUEST:
			result = _sscs432_Unpack_Dl432UpdateReplyRequest();
			break;

		case DL432_REPLY_REQUEST:
			result = _sscs432_Unpack_Dl432ReplyRequest();
			break;

		case DL432_BROADCAST_REQUEST:
			result = _sscs432_Unpack_Dl432BroadcastRequest();
			break;

		default:
			/* Primitive is ignored */
			result = SSCS432_OK;
		}
	}

	if (result == 0) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * \brief Function to encapsulate 432 Tx Control Process
 *
 * \note This function will be called using a pointer
 *
 * \retval 0 if the request has been sent
 * \retval -1 if the request has not been sent
 */
int8_t serial_432_if_api_process(void)
{
	int8_t result = SSCS432_ERROR;
	x_usi_serial_cmd_params_t pktUsi;

	/* Check data message */
	if (dl432_dl_callback(&callBackCmd, (dl432Buffer_t *)puc_sscs432_cmd_to_pack,
			MAX_LENGTH_432_DATA)) {
		result = _sscs432_Pack_432Msg(&callBackCmd);
	}
	/* Check opens or closes a 4-32 connections messages */
	else if (sscs432_poll_joins_leaves(&x_base_not)) {
		result = _sscs432_Pack_432NotBase(&x_base_not);
	}
	/* Check connection/disconnection messages */
	else if (sscs432_poll_establish_release(&x_service_not)) {
		result = _sscs432_Pack_432NotService(&x_service_not);
	}

	if (result == SSCS432_OK) {
		/* Pack and send the primitive */
		pktUsi.uc_protocol_type = PROTOCOL_432_PRIME;
		pktUsi.ptr_buf = (uint8_t *)&puc_sscs432_tx_buff[0];
		pktUsi.us_len = txLen;
		result = usi_send_cmd(&pktUsi);
	}

	return (result);
}

/* @} */

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */
