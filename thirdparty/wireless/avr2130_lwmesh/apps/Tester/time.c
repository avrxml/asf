/**
 * \file time.c
 *
 * \brief Timing routines implementation
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
#include "time.h"

/*- Variables --------------------------------------------------------------*/
volatile uint16_t sys_time;

/*- Prototypes --------------------------------------------------------------*/
static inline uint32_t gettime(void);

/*- Implementations --------------------------------------------------------*/

void appTimeInit(void)
{
	set_common_tc_overflow_callback(hw_overflow_cb);
}

/*************************************************************************//**
*****************************************************************************/
uint32_t appTimeGet(void)
{
	uint16_t current_sys_time;
	uint32_t current_time;

	do {
		current_sys_time = sys_time;
		current_time = current_sys_time;
		current_time = current_time << 16;
		current_time = current_time | common_tc_read_count();
	} while (current_sys_time != sys_time);

	return current_time;
}

void hw_overflow_cb(void)
{
	sys_time++;
}
