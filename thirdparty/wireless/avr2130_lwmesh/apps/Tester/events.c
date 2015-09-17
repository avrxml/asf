/**
 * \file events.c
 *
 * \brief Events buffer implementation
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "events.h"

#ifdef APP_ENABLE_EVENTS_BUFFER

/*- Variables --------------------------------------------------------------*/
static uint8_t appEventsBuffer[APP_EVENTS_BUFFER_SIZE];
static uint8_t appEventsSize;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void appEventsInit(void)
{
	appEventsSize = 0;
}

/*************************************************************************//**
*****************************************************************************/
uint16_t appEventsGetSize(void)
{
	return appEventsSize;
}

/*************************************************************************//**
*****************************************************************************/
uint8_t *appEventsGetBuffer(void)
{
	return appEventsBuffer;
}

/*************************************************************************//**
*****************************************************************************/
void appEventsClearBuffer()
{
	appEventsSize = 0;
}

/*************************************************************************//**
*****************************************************************************/
void appEventsLog(uint8_t event)
{
	if (appEventsSize < APP_EVENTS_BUFFER_SIZE) {
		appEventsBuffer[appEventsSize++] = event;
	} else {
		appEventsBuffer[appEventsSize - 1] = 0xff;
	}
}

#endif /* APP_ENABLE_EVENTS_BUFFER */
