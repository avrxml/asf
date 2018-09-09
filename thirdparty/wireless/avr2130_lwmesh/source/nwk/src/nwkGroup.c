/**
 * \file nwkGroup.c
 *
 * \brief Multicast group management implementation
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

#ifdef NWK_ENABLE_MULTICAST

/*- Definitions ------------------------------------------------------------*/
#define NWK_GROUP_FREE      0xffff

/*- Prototypes -------------------------------------------------------------*/
static bool nwkGroupSwitch(uint16_t from, uint16_t to);

/*- Variables --------------------------------------------------------------*/
static uint16_t nwkGroups[NWK_GROUPS_AMOUNT];

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*  @brief Initializes the Group module
*****************************************************************************/
void nwkGroupInit(void)
{
	for (uint8_t i = 0; i < NWK_GROUPS_AMOUNT; i++) {
		nwkGroups[i] = NWK_GROUP_FREE;
	}
}

/*************************************************************************//**
*  @brief Adds node to the @a group
*  @param[in] group Group ID
*  @return @c true in case of success and @c false otherwise
*****************************************************************************/
bool NWK_GroupAdd(uint16_t group)
{
	return nwkGroupSwitch(NWK_GROUP_FREE, group);
}

/*************************************************************************//**
*  @brief Removes node from the @a group
*  @param[in] group Group ID
*  @return @c true in case of success and @c false otherwise
*****************************************************************************/
bool NWK_GroupRemove(uint16_t group)
{
	return nwkGroupSwitch(group, NWK_GROUP_FREE);
}

/*************************************************************************//**
*  @brief Verifies if node is a member of the @a group
*  @param[in] group Group ID
*  @return @c true if node is a member of the group and @c false otherwise
*****************************************************************************/
bool NWK_GroupIsMember(uint16_t group)
{
	for (uint8_t i = 0; i < NWK_GROUPS_AMOUNT; i++) {
		if (group == nwkGroups[i]) {
			return true;
		}
	}
	return false;
}

/*************************************************************************//**
*  @brief Switches records with IDs @a from and @a to in the the group table
*  @param[in] from Source group ID
*  @param[in] to   Destination group ID
*  @return @c true if @a from entry was found and @c false otherwise
*****************************************************************************/
static bool nwkGroupSwitch(uint16_t from, uint16_t to)
{
	for (uint8_t i = 0; i < NWK_GROUPS_AMOUNT; i++) {
		if (from == nwkGroups[i]) {
			nwkGroups[i] = to;
			return true;
		}
	}
	return false;
}

#endif /* NWK_ENABLE_MULTICAST */
