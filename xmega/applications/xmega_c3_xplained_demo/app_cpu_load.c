/**
 * \file
 *
 * \brief Task to manage CPU load monitoring
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
static uint32_t app_cpu_load_time_actif;
/* ! Total time in sleep mode */
static uint32_t app_cpu_load_time_sleep;

void app_cpu_load_init(void)
{
	/* Reset counters */
	app_cpu_load_time_actif = 0;
	app_cpu_load_time_sleep = 0;
	/* Start Timer counter used to monitor CPU timing */
	tc_enable(&TCC1);
	tc_write_clock_source(&TCC1, TC_CLKSEL_DIV256_gc); /* 24MHz / 256 */
	tc_set_direction(&TCC1, TC_UP);
	/* Display static background */
	gfx_mono_draw_string(DISPLAY_CPU_LOAD_TEXT,
			DISPLAY_CPU_LOAD_TEXT_POS_X,
			DISPLAY_CPU_LOAD_TEXT_POS_Y,
			&sysfont);
}

void app_cpu_load_enter_sleep(void)
{
	/* Disable all interrupts to avoid interrupt
	 * before enter the CPU in sleep mode */
	cpu_irq_disable();
	app_cpu_load_sleep = true;
	app_cpu_load_time_actif += tc_read_count(&TCC1);
	/* Reset Timer counter */
	tc_write_count(&TCC1, 0);
}

void app_cpu_load_exit_sleep(void)
{
	if (app_cpu_load_sleep) {
		/* It is a sleep mode exit */
		app_cpu_load_sleep = false;
		app_cpu_load_time_sleep += tc_read_count(&TCC1);
		/* Reset Timer counter */
		tc_write_count(&TCC1, 0);
	}
}

void app_cpu_load_task(void)
{
	static uint8_t bar_pos_last = 0;
	uint8_t bar_pos;

	/* Wait a total time > 250ms to update display each 250ms */
	if ((app_cpu_load_time_actif + app_cpu_load_time_sleep)
			< ((24000lu / 256lu) * 250)) {
		return;
	}

	/* Compute CPU load */
	if (tc_is_overflow(&TCC1)) {
		tc_clear_overflow(&TCC1);
		bar_pos = DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X;
	} else {
		bar_pos = (app_cpu_load_time_actif *
				DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X)
				/ (app_cpu_load_time_actif +
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
	app_cpu_load_time_actif = 0;
	app_cpu_load_time_sleep = 0;
}

/* ! @} */
