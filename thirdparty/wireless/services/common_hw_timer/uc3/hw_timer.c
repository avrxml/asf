/**
 * @file hw_timer.c
 *
 * @brief
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

/* Options for enabling TC interrupts */
static tc_interrupt_t tc_interrupt = {
	.etrgs = 0,
	.ldrbs = 0,
	.ldras = 0,
	.cpcs  = 0, /* Enable interrupt on RC compare alone */
	.cpbs  = 0,
	.cpas  = 0,
	.lovrs = 0,
	.covfs = 0
};

extern uint8_t timer_multiplier;
tmr_callback_t tmr_callback;

/* === Prototypes =========================================================== */
static void configure_irq_handler(void);

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = AVR32_TC0_IRQ_GROUP, 1
__interrupt
#endif
static void tc0_callback(void)
{
	uint32_t status = tc_read_sr(TIMER, TIMER_CHANNEL_ID);
	status &= tc_get_interrupt_settings(TIMER, TIMER_CHANNEL_ID);

	if ((status & AVR32_TC_IDR2_COVFS_MASK) == AVR32_TC_IDR2_COVFS_MASK) {
		tmr_ovf_callback();
	}

	if ((status & AVR32_TC_IDR2_CPCS_MASK) == AVR32_TC_IDR2_CPCS_MASK) {
		tmr_cca_callback();
	}
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = AVR32_TC1_IRQ_GROUP, 1
__interrupt
#endif
static void tc1_callback(void)
{
	uint32_t status = tc_read_sr(TIMER, TIMER_CHANNEL_ID);
	status &= tc_get_interrupt_settings(TIMER, TIMER_CHANNEL_ID);

	if ((status & AVR32_TC_IDR2_COVFS_MASK) == AVR32_TC_IDR2_COVFS_MASK) {
		tmr_ovf_callback();
	}

	if ((status & AVR32_TC_IDR2_CPCS_MASK) == AVR32_TC_IDR2_CPCS_MASK) {
		tmr_cca_callback();
	}
}

/*! \brief  read the actual timer count from register
 */
uint16_t tmr_read_count(void)
{
	return tc_read_tc(TIMER, TIMER_CHANNEL_ID);
}

/*! \brief  to save current interrupt status
 */
uint8_t save_cpu_interrupt(void)
{
	return cpu_irq_save();
}

/*! \brief  to restore saved interrupt status
 *  \param  saved interrupt status
 */
void restore_cpu_interrupt(uint8_t flags)
{
	cpu_irq_restore(flags);
}

/*! \brief  to stop the running timer
 */
void tmr_stop(void)
{
	tc_stop(TIMER, TIMER_CHANNEL_ID);
}

/*! \brief  to load compare value in channel compare register
 */
void tmr_write_cmpreg(uint16_t compare_value)
{
	tc_write_rc(TIMER, TIMER_CHANNEL_ID, compare_value);
}

/*! \brief  to initialiaze hw timer
 */
uint8_t tmr_init(void)
{
	uint8_t timer_multiplier;
	/* Options for waveform generation. */
	tc_waveform_opt_t waveform_opt = {
		.channel  = TIMER_CHANNEL_ID,    /* Channel selection. */

		.bswtrg   = TC_EVT_EFFECT_NOOP,       /* Software trigger effect
		                                       * on TIOB. */
		.beevt    = TC_EVT_EFFECT_NOOP,       /* External event effect
		                                       * on TIOB. */
		.bcpc     = TC_EVT_EFFECT_NOOP,       /* RC compare effect on
		                                       * TIOB. */
		.bcpb     = TC_EVT_EFFECT_NOOP,       /* RB compare effect on
		                                       * TIOB. */

		.aswtrg   = TC_EVT_EFFECT_NOOP,       /* Software trigger effect
		                                       * on TIOA. */
		.aeevt    = TC_EVT_EFFECT_NOOP,       /* External event effect
		                                       * on TIOA. */
		.acpc     = TC_EVT_EFFECT_NOOP,       /* RC compare effect on
		                                       * TIOA */
		.acpa     = TC_EVT_EFFECT_NOOP,       /* RA compare effect on
		                                       * TIOA */

		.wavsel   = TC_WAVEFORM_SEL_UP_MODE,  /* Waveform selection: Up
		                                       * mode without automatic
		                                       * trigger on RC compare.
		                                       **/
		.enetrg   = false,                    /* External event trigger
		                                       * enable. */
		.eevt     = TC_EXT_EVENT_SEL_TIOB_INPUT, /* External event
		                                          * selection. */
		.eevtedg  = TC_SEL_NO_EDGE,           /* External event edge
		                                       * selection. */
		.cpcdis   = false,                    /* Counter disable when RC
		                                       * compare. */
		.cpcstop  = false,                    /* Counter clock stopped
		                                       * with RC compare. */

		.burst    = TC_BURST_NOT_GATED,       /* Burst signal selection.
		                                      **/
		.clki     = TC_CLOCK_RISING_EDGE,     /* Clock inversion. */
		.tcclks   = TC_CLOCK_SOURCE_TC2       /* Internal source clock
		                                       * 3, connected to fPBA /
		                                       * 8. */
	};

	sysclk_enable_peripheral_clock(TIMER);
	/* Initialize the timer/counter. */
	tc_init_waveform(TIMER, &waveform_opt);

	/* calculate how faster the timer with current clk freq compared to
	 * timer with 1Mhz */
	timer_multiplier = sysclk_get_peripheral_bus_hz(TIMER) / DEF_1MHZ;
	/* */
	timer_multiplier = timer_multiplier >> 1;

	configure_irq_handler();

	tmr_enable_ovf_interrupt();

	tmr_disable_cc_interrupt();
	tc_start(TIMER, TIMER_CHANNEL_ID);
	return timer_multiplier;
}

/*! \brief  to disable compare interrupt
 */
void tmr_disable_cc_interrupt(void)
{
	tc_interrupt.cpcs = 0;
	tc_configure_interrupts(TIMER, TIMER_CHANNEL_ID, &tc_interrupt);
}

/*! \brief  to enable compare interrupt
 */
void tmr_enable_cc_interrupt(void)
{
	tc_read_sr(TIMER, TIMER_CHANNEL_ID);
	tc_interrupt.cpcs = 1;
	tc_configure_interrupts(TIMER, TIMER_CHANNEL_ID, &tc_interrupt);
}

/*! \brief  to disable overflow interrupt
 */
void tmr_disable_ovf_interrupt(void)
{
	tc_interrupt.covfs = 0;
	tc_configure_interrupts(TIMER, TIMER_CHANNEL_ID, &tc_interrupt);
}

/*! \brief  to enable overflow interrupt
 */
void tmr_enable_ovf_interrupt(void)
{
	tc_read_sr(TIMER, TIMER_CHANNEL_ID);
	tc_interrupt.covfs = 1;
	tc_configure_interrupts(TIMER, TIMER_CHANNEL_ID, &tc_interrupt);
}

/*! \brief  to register the timer callback with the corresponding handler ID
 */
static void configure_irq_handler(void)
{
	if (&AVR32_TC0 == TIMER) {
		switch (TIMER_CHANNEL_ID) {
		case 0:
			irq_register_handler(tc0_callback, AVR32_TC0_IRQ0,
					TIMER_CHANNEL_ID);
			break;

		case 1:
			irq_register_handler(tc0_callback, AVR32_TC0_IRQ1,
					TIMER_CHANNEL_ID);
			break;

		case 2:
			irq_register_handler(tc0_callback, AVR32_TC0_IRQ2,
					TIMER_CHANNEL_ID);
			break;

		default:
			break;
		}
	}

	if (&AVR32_TC1 == TIMER) {
		switch (TIMER_CHANNEL_ID) {
		case 0:
			irq_register_handler(tc1_callback, AVR32_TC1_IRQ0,
					TIMER_CHANNEL_ID);
			break;

		case 1:
			irq_register_handler(tc1_callback, AVR32_TC1_IRQ1,
					TIMER_CHANNEL_ID);
			break;

		case 2:
			irq_register_handler(tc1_callback, AVR32_TC1_IRQ2,
					TIMER_CHANNEL_ID);
			break;

		default:
			break;
		}
	}
}

/* EOF */
