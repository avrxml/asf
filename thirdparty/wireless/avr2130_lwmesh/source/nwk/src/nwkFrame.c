/**
 * \file nwkFrame.c
 *
 * \brief Frame buffers management implementation
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
#include "nwk.h"
#include "nwkFrame.h"

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_FRAME_STATE_FREE = 0x00,
};

/*- Variables --------------------------------------------------------------*/
static NwkFrame_t nwkFrameFrames[NWK_BUFFERS_AMOUNT];

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*  @brief Initializes the Frame module
*****************************************************************************/
void nwkFrameInit(void)
{
	for (uint8_t i = 0; i < NWK_BUFFERS_AMOUNT; i++) {
		nwkFrameFrames[i].state = NWK_FRAME_STATE_FREE;
	}
}

/*************************************************************************//**
*  @brief Allocates an empty frame from the buffer pool
*  @return Pointer to the frame or @c NULL if there are no free frames
*****************************************************************************/
NwkFrame_t *nwkFrameAlloc(void)
{
	for (uint8_t i = 0; i < NWK_BUFFERS_AMOUNT; i++) {
		if (NWK_FRAME_STATE_FREE == nwkFrameFrames[i].state) {
			memset(&nwkFrameFrames[i], 0, sizeof(NwkFrame_t));
			nwkFrameFrames[i].size = sizeof(NwkFrameHeader_t);
			nwkFrameFrames[i].payload = nwkFrameFrames[i].data +
					sizeof(NwkFrameHeader_t);
			nwkIb.lock++;
			return &nwkFrameFrames[i];
		}
	}
	return NULL;
}

/*************************************************************************//**
*  @brief Frees a @a frame and returns it to the buffer pool
*  @param[in] frame Pointer to the frame to be freed
*****************************************************************************/
void nwkFrameFree(NwkFrame_t *frame)
{
	frame->state = NWK_FRAME_STATE_FREE;
	nwkIb.lock--;
}

/*************************************************************************//**
*  @brief Cycles through the allocated frames starting from the specified @a
* frame
*  @param[in] frame Pointer to the current frame or @c NULL for the first frame
*  @return Next allocated frame or @c NULL if there are no more frames
*****************************************************************************/
NwkFrame_t *nwkFrameNext(NwkFrame_t *frame)
{
	if (NULL == frame) {
		frame = nwkFrameFrames;
	} else {
		frame++;
	}

	for (; frame < &nwkFrameFrames[NWK_BUFFERS_AMOUNT]; frame++) {
		if (NWK_FRAME_STATE_FREE != frame->state) {
			return frame;
		}
	}

	return NULL;
}

/*************************************************************************//**
*  @brief Sets default parameters for the the command @a frame
*  @param[in] frame Pointer to the command frame
*****************************************************************************/
void nwkFrameCommandInit(NwkFrame_t *frame)
{
	frame->tx.status = NWK_SUCCESS_STATUS;
	frame->header.nwkSeq = ++nwkIb.nwkSeqNum;
	frame->header.nwkSrcAddr = nwkIb.addr;
#ifdef NWK_ENABLE_SECURE_COMMANDS
	frame->header.nwkFcf.security = 1;
#endif
}
