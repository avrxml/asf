/**
 * \file filter.c
 *
 * \brief Address filter implementation
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
