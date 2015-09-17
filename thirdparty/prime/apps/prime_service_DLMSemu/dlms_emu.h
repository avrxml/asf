/**
 * \file
 *
 * \brief DLMS Application Emulator for ATMEL PRIME v.1.3 Service Node
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

#ifndef DLMSEMU_H_INCLUDED
#define DLMSEMU_H_INCLUDED

#include "FreeRTOS.h"
#include "task.h"

/* Tasks priorities */
#define TASK_DLMSEMU_LAYER_PRIO                (tskIDLE_PRIORITY + 1)

/* Stack definitions */
#define TASK_DLMSEMU_LAYER_STACK               (configMINIMAL_STACK_SIZE * 10)

/* Main Task Period */
#define PRIME_DLMSEMU_APP_TIMER_RATE           (100 / portTICK_RATE_MS)

/* Check Connection Period [100ms period count]*/
#define PRIME_CHECK_CONNECTION_RATE            100

/* ENABLE DEBUG MESSAGES */
/* #define DLMS_EMU_DEBUG_ENABLE */

void vDLMSemuTask(void);

#endif /* DLMSEMU_H_INCLUDED */
