/**
 * @file sleep_mgr.c
 *
 * @brief
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
#include "sleep_mgr.h"
#include "sleepmgr.h"
#include "conf_sleepmgr.h"
#include "sysclk.h"
#include "delay.h"

/*! \brief This macro initiates read synchronization of the RTC32 CNT register
 *
 *  Initiates synchronization of CNT register from RTC to system clock domain.
 */
#define RTC32_SyncCnt()                (RTC32.SYNCCTRL |= RTC32_SYNCCNT_bm)

/*! \brief This macro returns the RTC32 CNT read synchronization flag.
 *
 *  \return Non-Zero if RTC32 is busy synchronizing CNT from RTC to system
 *  clock domain, zero otherwise.
 */
#define RTC32_SyncCntBusy()            (RTC32.SYNCCTRL & RTC32_SYNCCNT_bm)

/*! \brief This macro returns the RTC32 CNT/CTRL write synchronization flag.
 *
 *  \return Non-Zero if RTC32 is busy synchronizing CNT/CTRL from system to
 *  RTC32 clock domain, and zero otherwise.
 */
#define RTC32_SyncBusy()               (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm)

static void RTC32_Initialize( uint32_t period, uint32_t count,
		uint32_t compareValue );
static void RTC32_SetTimeout( uint32_t alarmTimeout );

/**
 * \brief This function Initializes the Sleep functions
 */
void sm_init(void)
{
	/* Set the sleep mode to initially lock. */
	enum sleepmgr_mode mode = SLEEPMGR_PSAVE;

	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);

	RTC32_Initialize(0xffffffff, 0, 0);

	RTC32.INTCTRL |= RTC32_COMPINTLVL_LO_gc;

	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();
	sleepmgr_lock_mode(mode);
}

/**
 * \brief This function puts the  device to sleep
 */
void sm_sleep(uint32_t interval)
{
	RTC32_SetTimeout((interval * 1000));
	sleepmgr_enter_sleep();
}

/* Interrupt Service Routine definitions */
ISR(RTC32_COMP_vect)
{
}

static void RTC32_Initialize( uint32_t period, uint32_t count,
		uint32_t compareValue )
{
	/* Enable R/W access to the battery backup module. */
	VBAT.CTRL = VBAT_ACCEN_bm;

	/* Reset the module. (Reset bit is protected by CCP.) */

	{uint8_t flags = cpu_irq_save();
	 CCP = 0xD8;
	 VBAT.CTRL = VBAT_RESET_bm;
	 cpu_irq_restore(flags); }

	/* Enable the failure detection. */
	VBAT.CTRL |= VBAT_XOSCFDEN_bm;

	/* A delay is needed to give the voltage in the backup system some time
	 * to stabilise.
	 */
	delay_us(200);

	VBAT.CTRL |= VBAT_XOSCEN_bm | VBAT_XOSCSEL_bm;

	/* Disable the RTC32 module before writing to it. Wait for synch. */
	RTC32.CTRL &= ~RTC32_ENABLE_bm;
	do { } while (RTC32_SyncBusy());

	/* Write PER, COMP and CNT. */
	RTC32.PER = period - 1;
	RTC32.COMP = compareValue;
	RTC32.CNT = count;

	/* Re-enable the RTC32 module, synchronize before returning. */
	RTC32.CTRL |= RTC32_ENABLE_bm;
	do { } while (RTC32_SyncBusy());
}

static void RTC32_SetTimeout( uint32_t alarmTimeout )
{
	uint32_t compareValue;

	/* Synchronize CNT from RTC to system clock domain. */
	RTC32_SyncCnt();
	do { } while (RTC32_SyncCntBusy());

	/* Calculate compare time. */
	compareValue = RTC32.CNT + alarmTimeout;

	/* Wrap on period. */
	if (compareValue > RTC32.PER) {
		compareValue -= RTC32.PER;
	}

	/* Add the timeout value to get the absolute time of the alarm. */
	RTC32.COMP = compareValue;
}
