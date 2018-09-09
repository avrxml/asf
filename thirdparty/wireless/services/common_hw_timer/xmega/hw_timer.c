/**
 * @file hw_timer.c
 *
 * @brief
 *
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
