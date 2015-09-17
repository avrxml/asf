/**
 * \file
 *
 * \brief Metering Application Emulator for all node types
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

/* System includes */
#include <stdio.h>
#include <string.h>

/* CS includes */
#include "app_emu_common.h"

#define MULTIPLIER      0x015a4e35L
#define INCREMENT       1

static long l_app_emu_seed; /* Randomize seed */

uint32_t ul_time_app_emu;

/** @brief	Copies the time stamp in the buffer requested
 *
 *      @param TimeStamp		Time stamp with the current instant
 *
 * This function builds the time stamp included in requests as the number
 * of seconds passed since the 00:00 PM.
 **/

void app_emu_copy_timestamp(char *pc_timestamp, uint8_t *pc_base_cmd)
{
	uint8_t i = TIMESTAMP_OFFSET;

	while (i <= TIMESTAMP_END) {
		pc_timestamp[i - TIMESTAMP_OFFSET] = pc_base_cmd[i];
		i++;
	}

	return;
}

/** @brief	Fill a string with data
 *
 *      @param String			Pointer to the buffer to write data
 *      @param Size				Data size to fill
 *      @param DownUp			Boolean to indicate if packet is uplink
 * / downlink
 *      @param Step				Number of step in application
 *      @param TimeStamp		Time stamp with the current instant
 *
 * This function prepares the data for the packet to be sent and fills it
 * with ascii characters
 **/

void app_emu_fill_string(char *pc_str, uint16_t us_size, uint8_t uc_down_up,
		uint16_t us_step, char *pc_timestamp)
{
	uint16_t i;
	char c_asc = 'A';
	char pc_down_up_str[] = {"UP"};

	/* Build the main string to be transmitted */
	if (uc_down_up) {
		sprintf(pc_down_up_str, "UP");
	} else {
		sprintf(pc_down_up_str, "DW");
	}

	sprintf(pc_str, "%s%s%d%s", pc_down_up_str,
			PROVIDER, (uint16_t)us_step, pc_timestamp);

	/* Fill it with ascii characters */
	for (i = (uint16_t)strlen(pc_str); i < us_size; i++) {
		pc_str[i] = c_asc;

		if (c_asc == 'Z') {
			c_asc = 'A';
		} else {
			c_asc++;
		}
	}
	pc_str[us_size] = '\0';
}

/** @brief	Update timers 1 ms
 *
 * This function update application timers
 **/

void app_emu_update_10ms(void)
{
	if (ul_time_app_emu) {
		ul_time_app_emu--;
	}
}


/** @brief	initializes random number generator
 *
 *       @param	puc_mac		Pointer to the MAC address
 *
 **/

void app_emu_init_random(const uint8_t *puc_mac)
{
	uint8_t i;

	l_app_emu_seed = 0;

	for (i = 0; i < 6; i++) {
		l_app_emu_seed += (uint16_t)puc_mac[i];
	}
	l_app_emu_seed <<= 2;
}


/** @brief	random number generator
 *
 *      @return	the random number
 *
 * rand uses a multiplicative congruential random number generator with period
 * 2^32 to return successive pseudo-
 * random numbers in the range from 0 to 2^15 - 1.
 * The generator is reinitialized by calling srand with an argument value of 1.
 * It can be set to a new starting point by
 * calling srand with a given seed number.
 **/

int app_emu_random(void)
{
	l_app_emu_seed = MULTIPLIER * l_app_emu_seed + INCREMENT;
	return((int)((uint32_t)l_app_emu_seed >> 16) & 0x7fff);
}
