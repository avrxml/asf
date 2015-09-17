/**
 * \file
 *
 * \brief Operative System Support Interface
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

/* From module: FreeRTOS - Kernel 7.3.0 */
#include <FreeRTOS.h>
#include <FreeRTOS_CLI.h>
#include <StackMacros.h>
#include <croutine.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>

/* From module: PLC Physical Layer Interface */
#include <atpl230.h>

/* From module: Physical Abstraction Layer (PAL) interface - ATPL230 and PRIME
 * MAC 1.3 */
#include <pal.h>

/* From module: PLC Universal Serial Interface */
#include <usi.h>

/* From module: PRIME IEC 61334-4-32 Convergence Layer */
#include <sscs432.h>

/* From module: PRIME MAC Layer */
#include <mngl.h>
#include <prime.h>

/* From module: PRIME Operative System Support (OSS) */
#include <oss_if.h>
#include "conf_oss.h"

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* / @endcond */

/**
 * \weakgroup oss_prime_group
 * @{
 */

/* Tasks handlers */
xTaskHandle xPrimeHnd;
/* Timers handlers */
xTimerHandle xUpdateTimer;

/**
 * \internal
 * \brief Task to update internal PRIME systick.
 *
 * This function must be called every 1 ms.
 *
 */
static void _update_1ms_proc(xTimerHandle pxTimer)
{
	UNUSED(pxTimer);
	taskENTER_CRITICAL();

	prime_upd1ms();

	sscs432_upd1ms();

	usi_txrx_block_timer();

	taskEXIT_CRITICAL();
}

/**
 * \internal
 * \brief Periodic task to process PRIME. Initialize and start every layer.
 *
 */
static void _prime_stack_process(void *pvParameters)
{
	static portTickType xLastWakeTime;
	static portTickType xPeriod;

	UNUSED(pvParameters);

	phy_init(DISABLE_SERIAL);

	pal_init();

	prime_init();

	sscs432_init();

	usi_init();

	prime_start();

	sscs432_start();

#ifdef OSS_PRIME_DISABLE_PRM
	prime_MLME_SET_request(PIB_MAC_PRM_ACTIVATION, 0);
#endif
#ifdef OSS_PRIME_ENABLE_EMB_SNIFFER
	prime_MLME_SET_request(PIB_MAC_SNIFFER_ACTIVATION, 1);
#endif

	/* Start timer to update counters in phy layer */
	xTimerStart(xUpdateTimer, PRIME_UPDATE_PROCESS_TIMER_RATE);

	xPeriod = PRIME_PROCESS_TIMER_RATE;
	xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
		taskENTER_CRITICAL();

		pal_process();

		prime_process();

		sscs432_process();

		usi_process();

		taskEXIT_CRITICAL();
	}
}

/**
 * \internal
 * \brief Create main PRIME task and create timer to update internal counters.
 *
 */
void vPrimeStackInitTask(void)
{
	/* Create new task to USI_process */
	xTaskCreate(_prime_stack_process, (const signed char *const)"PrimeProc",
			TASK_PRIME_STACK, NULL, TASK_PRIME_PRIO, &xPrimeHnd);

	/* Create timer to update counters in phy layer */
	xUpdateTimer = xTimerCreate((const signed char *const)"UPD timer",
			PRIME_UPDATE_PROCESS_TIMER_RATE,
			pdTRUE,
			NULL,
			_update_1ms_proc
			);
	configASSERT(xUpdateTimer);
}

/* @} */

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */
