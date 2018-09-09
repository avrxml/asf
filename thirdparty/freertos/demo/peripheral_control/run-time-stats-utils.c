/**
 *
 * \file
 *
 * \brief Generic FreeRTOS peripheral control functions
 *
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
