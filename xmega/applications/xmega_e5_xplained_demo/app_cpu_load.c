/**
 * \file
 *
 * \brief Task to manage CPU load monitoring
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_app_display.h"
#include "app_cpu_load.h"

/**
 * \ingroup cpu_load_task_group
 * \defgroup cpu_load_task_group_implementation CPU load task implementation
 * @{
 */

/* ! Last CPU state (active or sleep) */
static bool app_cpu_load_sleep = false;
/* ! Total time in active mode */
static uint32_t app_cpu_load_time_active;
/* ! Total time in sleep mode */
static uint32_t app_cpu_load_time_sleep;

void app_cpu_load_init(void)
{
	/* Reset counters */
	app_cpu_load_time_active = 0;
	app_cpu_load_time_sleep = 0;
	/* Start Timer counter used to monitor CPU timing */
	tc45_enable(&TCC4);
	tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV256_gc); /* 24MHz / 256 */
	tc45_set_direction(&TCC4, TC45_UP);
	/* Display static background */
	gfx_mono_draw_string(DISPLAY_CPU_LOAD_TEXT,
			DISPLAY_CPU_LOAD_TEXT_POS_X,
			DISPLAY_CPU_LOAD_TEXT_POS_Y,
			&sysfont);
	gfx_mono_draw_filled_rect(
			DISPLAY_CPU_LOAD_PROBAR_START_POS_X,
			DISPLAY_CPU_LOAD_PROBAR_START_POS_Y,
			DISPLAY_CPU_LOAD_PROBAR_START_SIZE_X,
			DISPLAY_CPU_LOAD_PROBAR_START_SIZE_Y,
			GFX_PIXEL_SET);
	gfx_mono_draw_filled_rect(
			DISPLAY_CPU_LOAD_PROBAR_STOP_POS_X,
			DISPLAY_CPU_LOAD_PROBAR_STOP_POS_Y,
			DISPLAY_CPU_LOAD_PROBAR_STOP_SIZE_X,
			DISPLAY_CPU_LOAD_PROBAR_STOP_SIZE_Y,
			GFX_PIXEL_SET);
}

void app_cpu_load_enter_sleep(void)
{
	/* Disable all interrupts to avoid interrupt
	 * before enter the CPU in sleep mode */
	cpu_irq_disable();
	app_cpu_load_sleep = true;
	app_cpu_load_time_active += tc45_read_count(&TCC4);
	/* Reset Timer counter */
	tc45_write_count(&TCC4, 0);
}

void app_cpu_load_exit_sleep(void)
{
	if (app_cpu_load_sleep) {
		/* It is a sleep mode exit */
		app_cpu_load_sleep = false;
		app_cpu_load_time_sleep += tc45_read_count(&TCC4);
		/* Reset Timer counter */
		tc45_write_count(&TCC4, 0);
	}
}

void app_cpu_load_task(void)
{
	static uint8_t bar_pos_last = 0;
	uint8_t bar_pos;

	/* Wait a total time > 250ms to update display each 250ms */
	if ((app_cpu_load_time_active + app_cpu_load_time_sleep)
			< ((32000lu / 256lu) * 250)) {
		return;
	}

	/* Compute CPU load */
	if (tc45_is_overflow(&TCC4)) {
		tc45_clear_overflow(&TCC4);
		bar_pos = DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X;
	} else {
		bar_pos = (app_cpu_load_time_active *
				DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X)
				/ (app_cpu_load_time_active +
				app_cpu_load_time_sleep);
	}

	/* Update progress bar */
	if (bar_pos_last < bar_pos) {
		gfx_mono_draw_filled_rect(
				DISPLAY_CPU_LOAD_PROBAR_POS_X + bar_pos_last,
				DISPLAY_CPU_LOAD_PROBAR_POS_Y,
				bar_pos - bar_pos_last,
				DISPLAY_CPU_LOAD_PROBAR_SIZE_Y,
				GFX_PIXEL_SET);
	} else if (bar_pos_last > bar_pos) {
		gfx_mono_draw_filled_rect(
				DISPLAY_CPU_LOAD_PROBAR_POS_X + bar_pos,
				DISPLAY_CPU_LOAD_PROBAR_POS_Y,
				bar_pos_last - bar_pos,
				DISPLAY_CPU_LOAD_PROBAR_SIZE_Y,
				GFX_PIXEL_CLR);
	}

	bar_pos_last = bar_pos;

	/* Reset counters */
	app_cpu_load_time_active = 0;
	app_cpu_load_time_sleep = 0;
}

/* ! @} */
