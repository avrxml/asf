/**
 *
 * \file
 *
 * \brief Generic FreeRTOS peripheral control functions
 *
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
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

/*
 * Utility functions for gathering run time statistical information.
 */

/* Standard includes. */
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* ASF includes. */
#include "sysclk.h"

/*-----------------------------------------------------------*/

/* Used in the run time stats calculations. */
static uint32_t clocks_per_10th_ms = 0UL;

/*-----------------------------------------------------------*/

void configure_timer_for_run_time_stats(void)
{
	/* How many clocks are there per tenth of a millisecond? */
	clocks_per_10th_ms = configCPU_CLOCK_HZ / 10000UL;
}

/*-----------------------------------------------------------*/

uint32_t get_run_time_counter_value(void)
{
	uint32_t sys_tick_counts, tick_count, ulReturn;
	const uint32_t sys_tick_reload_value
		= (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
	volatile uint32_t *const current_sys_tick_count
		= ((volatile uint32_t *) 0xe000e018);
	volatile uint32_t *const interrupt_ctrl_statee
		= ((volatile uint32_t *) 0xe000ed04);
	const uint32_t sys_tick_pending_bit = 0x04000000UL;

	/* NOTE: There are potentially race conditions here.  However, it is
	used anyway to keep the examples simple, and to avoid reliance on a
	separate timer peripheral. */

	/* The SysTick is a down counter.  How many clocks have passed since it
	was last reloaded? */
	sys_tick_counts = sys_tick_reload_value - *current_sys_tick_count;

	/* How many times has it overflowed? */
	tick_count = xTaskGetTickCountFromISR();

	/* This is called from the context switch, so will be called from a
	critical section.  xTaskGetTickCountFromISR() contains its own critical
	section, and the ISR safe critical sections are not designed to nest,
	so reset the critical section. */
	ulReturn = portSET_INTERRUPT_MASK_FROM_ISR();
	(void) ulReturn;

	/* Is there a SysTick interrupt pending? */
	if ((*interrupt_ctrl_statee & sys_tick_pending_bit) != 0UL) {
		/* There is a SysTick interrupt pending, so the SysTick has overflowed
		but the tick count not yet incremented. */
		tick_count++;

		/* Read the SysTick again, as the overflow might have occurred since
		it was read last. */
		sys_tick_counts = sys_tick_reload_value -
				*current_sys_tick_count;
	}

	/* Convert the tick count into tenths of a millisecond.  THIS ASSUMES
	configTICK_RATE_HZ is 1000! */
	ulReturn = (tick_count * 10UL);

	/* Add on the number of tenths of a millisecond that have passed since
	the tick count last got updated. */
	ulReturn += (sys_tick_counts / clocks_per_10th_ms);

	return ulReturn;
}
