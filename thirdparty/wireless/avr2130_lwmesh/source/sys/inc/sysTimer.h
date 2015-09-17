/**
 * \file sysTimer.h
 *
 * \brief System timer interface
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/**
 * \ingroup group_lwmesh_sys
 * \defgroup group_lwmesh_sys_timer LWMesh System Services - Timer
 * @{
 */

 #define SYS_TIMER_INTERVAL      10ul /* ms */
 #define MS 1000

/*- Types ------------------------------------------------------------------*/
typedef enum SYS_TimerMode_t {
	SYS_TIMER_INTERVAL_MODE,
	SYS_TIMER_PERIODIC_MODE,
} SYS_TimerMode_t;

typedef struct SYS_Timer_t {
	/* Internal data */
	struct SYS_Timer_t *next;
	uint32_t timeout;

	/* Timer parameters */
	uint32_t interval;
	SYS_TimerMode_t mode;
	void (*handler)(struct SYS_Timer_t *timer);
} SYS_Timer_t;

/*- Prototypes -------------------------------------------------------------*/
void SYS_TimerInit(void);
void SYS_TimerStart(SYS_Timer_t *timer);
void SYS_TimerStop(SYS_Timer_t *timer);
bool SYS_TimerStarted(SYS_Timer_t *timer);
void SYS_TimerTaskHandler(void);
void SYS_HwExpiry_Cb(void);

/** @} */
#endif /* _SYS_TIMER_H_ */
