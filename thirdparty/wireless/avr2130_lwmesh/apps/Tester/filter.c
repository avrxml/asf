/**
 * \file filter.c
 *
 * \brief Address filter implementation
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
#include "nwk.h"
#include "filter.h"

#ifdef NWK_ENABLE_ADDRESS_FILTER

/*- Types ------------------------------------------------------------------*/
typedef struct {
	uint16_t addr;
	uint8_t allow    : 1;
	uint8_t setLqi   : 1;
	uint8_t reserved : 6;
	uint8_t lqi;
} AppFilterTableEntry_t;

/*- Variables --------------------------------------------------------------*/
static AppFilterTableEntry_t appFilterTable[APP_FILTER_TABLE_SIZE];
static uint8_t appFilterSize;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void appFilterInit(void)
{
	appFilterSize = 0;
}

/*************************************************************************//**
*****************************************************************************/
bool appFilterAdd(uint16_t addr, uint8_t allow, uint8_t setLqi, uint8_t lqi)
{
	for (uint8_t i = 0; i < appFilterSize; i++) {
		if (addr == appFilterTable[i].addr) {
			appFilterTable[appFilterSize].allow = allow;
			appFilterTable[appFilterSize].setLqi = setLqi;
			appFilterTable[appFilterSize].lqi = lqi;
			return true;
		}
	}

	if (appFilterSize < APP_FILTER_TABLE_SIZE) {
		appFilterTable[appFilterSize].addr = addr;
		appFilterTable[appFilterSize].allow = allow;
		appFilterTable[appFilterSize].setLqi = setLqi;
		appFilterTable[appFilterSize].lqi = lqi;
		appFilterSize++;
		return true;
	}

	return false;
}

/*************************************************************************//**
*****************************************************************************/
bool appFilterRemove(uint16_t addr)
{
	for (uint8_t i = 0; i < appFilterSize; i++) {
		if (addr == appFilterTable[i].addr) {
			if (appFilterSize > 0) {
				appFilterSize--;
				appFilterTable[i]
					= appFilterTable[appFilterSize];
			}

			return true;
		}
	}

	return false;
}

/*************************************************************************//**
*****************************************************************************/
bool NWK_FilterAddress(uint16_t addr, uint8_t *lqi)
{
	for (uint8_t i = 0; i < appFilterSize; i++) {
		if (addr == appFilterTable[i].addr) {
			if (appFilterTable[i].setLqi) {
				*lqi = appFilterTable[i].lqi;
			}

			return appFilterTable[i].allow;
		}
	}

	return false;
}

#endif /* NWK_ENABLE_ADDRESS_FILTER */
