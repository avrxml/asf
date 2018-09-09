/**
 * \file
 *
 * \brief Metering Application Emulator
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
