/**
 * \file sysTimer.c
 *
 * \brief System timer implementation
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
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include "compiler.h"
#include "common_hw_timer.h"
#include "sysTimer.h"

volatile uint8_t SysTimerIrqCount;

/*****************************************************************************
*****************************************************************************/
static void placeTimer(SYS_Timer_t *timer);

/*- Variables --------------------------------------------------------------*/
static SYS_Timer_t *timers;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerInit(void)
{
	SysTimerIrqCount = 0;
	set_common_tc_expiry_callback(SYS_HwExpiry_Cb);
	common_tc_init();
	common_tc_delay(SYS_TIMER_INTERVAL * MS);
	timers = NULL;
}

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerStart(SYS_Timer_t *timer)
{
	if (!SYS_TimerStarted(timer)) {
		placeTimer(timer);
	}
}

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerStop(SYS_Timer_t *timer)
{
	SYS_Timer_t *prev = NULL;

	for (SYS_Timer_t *t = timers; t; t = t->next) {
		if (t == timer) {
			if (prev) {
				prev->next = t->next;
			} else {
				timers = t->next;
			}

			if (t->next) {
				t->next->timeout += timer->timeout;
			}

			break;
		}

		prev = t;
	}
}

/*************************************************************************//**
*****************************************************************************/
bool SYS_TimerStarted(SYS_Timer_t *timer)
{
	for (SYS_Timer_t *t = timers; t; t = t->next) {
		if (t == timer) {
			return true;
		}
	}
	return false;
}

/*************************************************************************//**
*****************************************************************************/
void SYS_TimerTaskHandler(void)
{
	uint32_t elapsed;
	uint8_t cnt;
	irqflags_t flags;

	if (0 == SysTimerIrqCount) {
		return;
	}

	/* Enter a critical section */
	flags = cpu_irq_save();
	cnt = SysTimerIrqCount;
	SysTimerIrqCount = 0;
	/* Leave the critical section */
	cpu_irq_restore(flags);

	elapsed = cnt * SYS_TIMER_INTERVAL;

	while (timers && (timers->timeout <= elapsed)) {
		SYS_Timer_t *timer = timers;

		elapsed -= timers->timeout;
		timers = timers->next;
		if (SYS_TIMER_PERIODIC_MODE == timer->mode) {
			placeTimer(timer);
		}

		if (timer->handler) {
			timer->handler(timer);
		}
	}

	if (timers) {
		timers->timeout -= elapsed;
	}
}

/*************************************************************************//**
*****************************************************************************/
static void placeTimer(SYS_Timer_t *timer)
{
	if (timers) {
		SYS_Timer_t *prev = NULL;
		uint32_t timeout = timer->interval;

		for (SYS_Timer_t *t = timers; t; t = t->next) {
			if (timeout < t->timeout) {
				t->timeout -= timeout;
				break;
			} else {
				timeout -= t->timeout;
			}

			prev = t;
		}

		timer->timeout = timeout;

		if (prev) {
			timer->next = prev->next;
			prev->next = timer;
		} else {
			timer->next = timers;
			timers = timer;
		}
	} else {
		timer->next = NULL;
		timer->timeout = timer->interval;
		timers = timer;
	}
}

/*****************************************************************************
*****************************************************************************/
void SYS_HwExpiry_Cb(void)
{
	SysTimerIrqCount++;
	common_tc_delay(SYS_TIMER_INTERVAL * MS);
}
