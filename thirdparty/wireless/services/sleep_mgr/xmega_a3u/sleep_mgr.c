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

/**
 * \brief This function Initializes the Sleep functions
 */
void sm_init(void)
{
	/* Set the sleep mode to initially lock. */
	enum sleepmgr_mode mode = SLEEPMGR_PSAVE;

	/* Enable RTC with Internal RCOSC as clock source. */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);

	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;

	RTC.INTCTRL = RTC_OVFINTLVL_LO_gc;

	/* Wait until RTC is ready before continuing. */
	do { } while (RTC.STATUS & RTC_SYNCBUSY_bm);

	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_init();
	sleepmgr_lock_mode(mode);
}

/**
 * \brief This function puts the  device to sleep
 */
void sm_sleep(uint32_t interval)
{
	/* Configure RTC for wakeup at interval period . */
	RTC.PER = interval - 1;
	RTC.CNT = 0;
	RTC.CTRL = RTC_PRESCALER_DIV1024_gc;
	do { } while (RTC.STATUS & RTC_SYNCBUSY_bm);
	sleepmgr_enter_sleep();
}

/* Interrupt Service Routine definitions */
ISR(RTC_OVF_vect)
{
}
