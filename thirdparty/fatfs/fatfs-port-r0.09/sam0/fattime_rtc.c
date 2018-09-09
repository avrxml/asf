/**
 * \file
 *
 * \brief Implementation of low level disk I/O module skeleton for FatFS.
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
#include "compiler.h"
#include "rtc_calendar.h"

extern struct rtc_module rtc_instance;

uint32_t get_fattime(void);
/**
 * \brief Current time returned is packed into a DWORD value.
 *
 * The bit field is as follows:
 *
 * bit31:25  Year from 1980 (0..127)
 *
 * bit24:21  Month (1..12)
 *
 * bit20:16  Day in month(1..31)
 *
 * bit15:11  Hour (0..23)
 *
 * bit10:5   Minute (0..59)
 *
 * bit4:0    Second (0..29)
 *
 * \return Current time.
 */
uint32_t get_fattime(void)
{
	uint32_t ul_time;
	struct rtc_calendar_time current_time;

	/* Retrieve date and time */
	rtc_calendar_get_time(&rtc_instance, &current_time);

	ul_time = ((current_time.year - 1980) << 25)
			| (current_time.month << 21)
			| (current_time.day << 16)
			| (current_time.hour << 11)
			| (current_time.minute << 5)
			| ((current_time.second >> 1) << 0);

	return ul_time;
}

