/**
 * \file events.c
 *
 * \brief Events buffer implementation
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
