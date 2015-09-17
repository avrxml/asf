/**
 * @file hw_timer.c
 *
 * @brief
 *
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
 */

#include <compiler.h>
#include <parts.h>
#include "tc.h"
#include "hw_timer.h"
#include "sysclk.h"
#include "common_hw_timer.h"
#include "conf_hw_timer.h"

#define TIMER_PERIOD  UINT16_MAX

/* === Prototypes =========================================================== */
void tc_ovf_callback(void);
void tc_cca_callback(void);

/*! \brief  read the actual timer count from register
 */
uint16_t tmr_read_count(void)
{
	return tc_read_count(TIMER);
}

/*! \brief  to disable compare interrupt
 */
void tmr_disable_cc_interrupt(void)
{
	tc_set_cca_interrupt_level(TIMER, TC_INT_LVL_OFF);
	tc_clear_cc_interrupt(TIMER, TC_CCA);
}

/*! \brief  to enable compare interrupt
 */
void tmr_enable_cc_interrupt(void)
{
	tc_clear_cc_interrupt(TIMER, TC_CCA);
	tc_set_cca_interrupt_level(TIMER, TC_INT_LVL_HI);
}

/*! \brief  to disable overflow interrupt
 */
void tmr_disable_ovf_interrupt(void)
{
	tc_set_overflow_interrupt_level(TIMER, TC_INT_LVL_OFF);
	tc_clear_overflow(TIMER);
}

/*! \brief  to stop the running timer
 */
void tmr_stop(void)
{
	tc_disable(TIMER);
}

/*! \brief  to load compare value in channel compare register
 */
void tmr_write_cmpreg(uint16_t compare_value)
{
	tc_write_cc(TIMER, TC_CCA, compare_value);
}

/*! \brief  to save current interrupts status
 */
uint8_t save_cpu_interrupt(void)
{
	return cpu_irq_save();
}

/*! \brief  to restore saved interrupts status
 *  \param  saved interrupt status
 */
void restore_cpu_interrupt(uint8_t flags)
{
	cpu_irq_restore(flags);
}

/*! \brief  to initialiaze hw timer
 */
uint8_t tmr_init(void)
{
	uint8_t timer_multiplier;

	tc_enable(TIMER);

	tc_set_overflow_interrupt_callback(TIMER,
			(tc_callback_t)tc_ovf_callback);

	/*initialize timer in waveform generator - Normal mode */
	tc_set_wgm(TIMER, TC_WG_NORMAL);

	tc_write_period(TIMER, TIMER_PERIOD);
	/* select clock division as 1 */
	tc_write_clock_source(TIMER, TC_CLKSEL_DIV1_gc);

	tc_set_overflow_interrupt_level(TIMER, TC_INT_LVL_HI);

	tc_set_cca_interrupt_callback(TIMER, (tc_callback_t)tc_cca_callback);

	tc_enable_cc_channels(TIMER, TC_CCAEN);

	tc_set_cca_interrupt_level(TIMER, TC_INT_LVL_OFF);

	/* calculate how faster the timer with current clk freq compared to
	 * timer with 1Mhz */
	timer_multiplier = sysclk_get_peripheral_bus_hz(TIMER) / DEF_1MHZ;

	return timer_multiplier;
}

/*! \brief  hw timer overflow callback
 */
void tc_ovf_callback(void)
{
	tmr_ovf_callback();
}

/*! \brief  hw timer compare callback
 */
void tc_cca_callback(void)
{
	tmr_cca_callback();
}

/*EOF*/
