/**
 * \file
 *
 * \brief Webserver component task for the FreeRTOS Web/DSP Demo.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "task_demo.h"
#include "stdio_serial.h"
#include "ethernet_sam.h"
#include "sysclk.h"

static void webserver_task(void *pvParameters);

/**
 * \brief Create the Webserver task.
 *
 * \param stack_depth_words Task stack size in 32 bits word.
 * \param task_priority Task priority.
 */
void create_webserver_task(uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority)
{
	/** Create the task as described above. */
	xTaskCreate(webserver_task, (const signed char *const) "Webserver",
			stack_depth_words, NULL, task_priority,
			NULL);
}

/**
 * \brief Webserver task core function.
 *
 * \param pvParameters Junk parameter.
 */
static void webserver_task(void *pvParameters)
{
	/* Just to avoid compiler warnings. */
	UNUSED(pvParameters);

	/** Wait for user to read instructions. */
	WAIT_FOR_TOUCH_EVENT;

	/** Webserver task Loop. */
	while (1)
	{
		/**
		 * Check if any packets are available and process if they are
		 * ready. That function also manages the LwIP timers.
		 */
		ethernet_task();
	}
}
