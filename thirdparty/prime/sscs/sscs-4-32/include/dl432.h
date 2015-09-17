/**
 * \file
 *
 * \brief DL432
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#ifndef DL432_H_INCLUDE
#define DL432_H_INCLUDE

#include "compiler.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup sscs_prime_group
 * @{
 */

/* \name Length of the LPDU */
/* @{ */
#define LPDU_HEADER                    3
/* @} */

/* \name LSDU Data length */
/* \note It must be smaller than PRIME_MACSAP_DATA_SIZE */
/* @{ */
#define MAX_LENGTH_432_DATA            (1024 - LPDU_HEADER)
/* @} */

/* \name DL result values */
/* \note Values translated from mac layer */
/* @{ */
#define DL432_RESULT_SUCCESS           0
#define DL432_RESULT_ERROR             0x80
/* @} */

/* \name DL commands */
/* @{ */
#define DL432_NO_COMMAND               0
#define DL432_DATA_CONFIRM             1
#define DL432_DATA_INDICATION          2
#define DL432_UPDATE_REPLY_CONFIRM     3
#define DL432_UPDATE_REPLY_INDICATION  4
#define DL432_REPLY_CONFIRM            5
#define DL432_REPLY_INDICATION         6
#define DL432_DATA_REQUEST             11
#define DL432_UPDATE_REPLY_REQUEST     12
#define DL432_REPLY_REQUEST            13
#define DL432_BROADCAST_REQUEST        14
/* @} */

/* \name Maximum lenght of the stored LSDU */
/* @{ */
#define MAX_LENGTH_STORED_LSDU         7
/* @} */

#define lsdu dl.buff

/* Buffer defined for reception/ transmission */
typedef union {
	uint8_t lpdu[MAX_LENGTH_432_DATA + LPDU_HEADER];

	struct {
		uint8_t control;
		uint8_t dsap;
		uint8_t lsap;
		uint8_t buff[MAX_LENGTH_432_DATA];
	} dl;
} dl432Buffer_t;

/* Information of commands */
typedef struct {
	uint8_t cmd;
	uint8_t dstLSAP;
	uint8_t srcLSAP;
	uint8_t status;
	uint16_t dstAddress;
	uint16_t srcAddress;
	uint16_t length;
	uint8_t linkClass;
} dl432_LlcCallback_t;

/* \name DL432 interface */
/* @{ */
int dl432_dl_data_request(uint8_t dstLSAP, uint8_t locLSAP, uint16_t dstAddress, dl432Buffer_t *txBuff, uint16_t lenLsdu, uint8_t linkClass);
int dl432_dl_update_reply_request(uint8_t locLSAP, uint8_t *prBuff, uint16_t length);
int dl432_dl_reply_request(uint8_t dstLSAP, uint8_t srcLSAP, uint16_t dstAddress, uint8_t maxLenAnswer);
int dl432_dl_broadcast_request(uint8_t srcLSAP, dl432Buffer_t *buff, uint16_t lenMsg, uint8_t linkClass);

int dl432_dl_callback(dl432_LlcCallback_t *cmd, dl432Buffer_t *buf, uint16_t maxLenBuf);

/* @} */

/* @} */

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */

#endif
