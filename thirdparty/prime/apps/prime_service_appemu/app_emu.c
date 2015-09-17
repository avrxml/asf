/**
 * \file
 *
 * \brief Metering Application Emulator
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

#include <string.h>
#include <stdio.h>

/* Application includes */
#include "app_emu.h"
#include "app_emu_common.h"

#include "asf.h"

/* Tasks handlers */
xTaskHandle xAppEmuHnd;

/**
 * \brief Periodic task to process App EMU.
 *
 */
static void _prv_process_app_emu(void *pvParameters)
{
	static portTickType xLastWakeTime;
	static portTickType xPeriod;

	UNUSED(pvParameters);
	/* Start AppEmu */
	app_emu_start();

	xPeriod = APPEMU_TIMER_RATE;
	xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		taskENTER_CRITICAL();
		app_emu_process();
		app_emu_update_10ms();
		taskEXIT_CRITICAL();
	}
}

/**
 * \brief Initialization task to process App EMU.
 *
 */
void vAppEmuInitTask(void)
{
	/* Create new task to AppEmu Application */
	xTaskCreate(_prv_process_app_emu,
				(const signed char *const)"AppEmuTask",
				TASK_APPEMU_LAYER_STACK, NULL, TASK_APPEMU_LAYER_PRIO,
				&xAppEmuHnd);
}
