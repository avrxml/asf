/**
 * \file
 *
 * \brief Metering Application Emulator for all node types
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
 */

/* System includes */
#include <stdio.h>
#include <string.h>

/* CS includes */
#include "app_emu_common.h"

#include "compiler.h"

#define MULTIPLIER      0x015a4e35L
#define INCREMENT       1

static long l_app_emu_seed; /* /< Randomize seed */
static uint32_t ul_hundreds_of_ms = 0;
uint32_t ul_tens_of_ms = 0;

uint32_t ul_time_app_emu;


/** @brief	Build the time stamp in the buffer requested
 *
 *  @param pc_timestamp    Pointer to the current timestamp
 *
 * This function builds the time stamp included in requests as the number
 * of seconds passed since the 00:00 PM.
 **/
void app_emu_build_timestamp(char *pc_timestamp)
{
#ifdef __GNUC__
	sprintf(pc_timestamp, "%06lu", ul_hundreds_of_ms);
#endif
#ifdef __ICCARM__
	sprintf(pc_timestamp, "%06u", ul_hundreds_of_ms);
#endif

	return;
}

/** @brief	Fill a string with data
 *
 *  @param String      Pointer to the buffer to write data
 *  @param Size        Data size to fill
 *  @param DownUp      Boolean to indicate if packet is uplink / downlink
 *  @param Step        Number of step in application
 *  @param TimeStamp   Time stamp with the current instant
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

	if (ul_tens_of_ms++ == 10) {
		ul_hundreds_of_ms++;
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
