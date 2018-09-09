/**
 * \file
 *
 * \brief Operative System Support Interface
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
