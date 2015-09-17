/**
 * \file
 *
 * \brief AVR XMEGA Quadrature Decoder Driver file
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
#include "sysclk.h"
#include "qdec.h"

#if XMEGA_E
#  include "tc45.h"
#  define TC_CCA                  TC45_CCA
#  define TC_CCAEN                TC45_CCACAPT
#  define TC_CLKSEL_DIV1_gc       TC45_CLKSEL_DIV1_gc
#  define TC_EVACT_FRQ_gc         TC45_EVACT_PWF_gc
#  define TC_EVACT_QDEC_gc        TC45_EVACT_QDEC_gc
#  define TC_EVSEL_CH0_gc         TC45_EVSEL_CH0_gc
#  define TC_EVSEL_t              TC45_EVSEL_t
#  define tc_enable               tc45_enable
#  define tc_enable_cc_channels   tc45_enable_cc_channels
#  define tc_get_resolution       tc45_get_resolution
#  define tc_read_cc              tc45_read_cc
#  define tc_read_count           tc45_read_count
#  define tc_set_input_capture    tc45_set_input_capture
#  define tc_set_resolution       tc45_set_resolution
#  define tc_write_clock_source   tc45_write_clock_source
#  define tc_write_count          tc45_write_count
#  define tc_write_period         tc45_write_period
#  define tc_is_cc_interrupt      tc45_is_cc_interrupt
#else
#  include "tc.h"
#endif

/* Fix header file about Prescaler Filter for XMEGA E */
#undef EVSYS_PRESCFILT_CH04_gc
#undef EVSYS_PRESCFILT_CH15_gc
#undef EVSYS_PRESCFILT_CH26_gc
#undef EVSYS_PRESCFILT_CH37_gc
#define EVSYS_PRESCFILT_CH04_gc (1<<4)
#define EVSYS_PRESCFILT_CH15_gc (2<<4)
#define EVSYS_PRESCFILT_CH26_gc (4<<4)
#define EVSYS_PRESCFILT_CH37_gc (8<<4)
/* Fix header files which uses TC_EVACT_FRW_gc instead of TC_EVACT_FRQ_gc */
#ifndef TC_EVACT_FRQ_gc
#  define TC_EVACT_FRQ_gc         (0x05<<5)  /* Frequency Capture */
#endif

/* Define pin position mask of phase0, phase 90 and index */
#define QDEC_PIN_PH0_MASK    (1 << 0)
#define QDEC_PIN_PH90_MASK   (1 << 1)
#define QDEC_PIN_PHASES_MASK (QDEC_PIN_PH0_MASK | QDEC_PIN_PH90_MASK)
#define QDEC_PIN_INDEX_MASK  (1 << 2)

/* Internal functions */
static void qdec_enabled_pins(qdec_config_t *config);
static void qdec_evsys_pin_2_chmux(PORT_t *port, uint8_t pins_base,
		uint8_t event_channel);
static void qdec_enabled_evsys(qdec_config_t *config);
static void qdec_enabled_tc(qdec_config_t *config);
static void qdec_enabled_tc_freq(qdec_config_t *config);

static void qdec_enabled_pins(qdec_config_t *config)
{
	/* Configure pins QDPH0 and QDPH90  as inputs */
	config->port->DIRCLR = QDEC_PIN_PHASES_MASK << config->pins_base;

	/* Select MPCMASK for to configure QDPH0 and QDPH90 signals */
	PORTCFG.MPCMASK = QDEC_PIN_PHASES_MASK << config->pins_base;

	/* Configure for signal sensing and inversion if needed, for
	 * QDPH0 and QDPH90
	 */
	config->port->PIN0CTRL = (config->port->PIN0CTRL & ~PORT_ISC_gm)
			| PORT_ISC_BOTHEDGES_gc
			| (config->phases.pins_invert ? PORT_INVEN_bm : 0);

	/* Check if Index signal is used and configure accordingly */
	if (config->index.enabled) {
		/* Check that port pin is not 6 or 7, because 3 contiguous pins
		 * are needed as decoder inputs including index pin
		 */
		Assert(config->pins_base < 6);

		/* Configure pins QDINDX as inputs */
		config->port->DIRCLR = QDEC_PIN_INDEX_MASK << config->pins_base;

		/* Select MPCMASK for to configure Index signal */
		PORTCFG.MPCMASK = QDEC_PIN_INDEX_MASK << config->pins_base;

		/* Configure for Index signal sensing and inversion if needed */
		config->port->PIN0CTRL = (config->port->PIN0CTRL & ~PORT_ISC_gm)
				| PORT_ISC_BOTHEDGES_gc
				| (config->index.pin_invert ? PORT_INVEN_bm : 0);
	} else {
		/* Check that port pin is not 7, because 2 contiguous pins
		 * are needed as decoder inputs.
		 */
		Assert(config->pins_base < 7);
	}
}

static void qdec_evsys_pin_2_chmux(PORT_t *port, uint8_t pins_base,
		uint8_t event_channel)
{
	volatile uint8_t *evsys_chmux;

	evsys_chmux = &EVSYS.CH0MUX + event_channel;
#if !XMEGA_B3
	if (port == &PORTA) {
		*evsys_chmux = EVSYS_CHMUX_PORTA_PIN0_gc | pins_base;
	}
#endif

#if !XMEGA_E
	if (port == &PORTB) {
		*evsys_chmux = EVSYS_CHMUX_PORTB_PIN0_gc | pins_base;
	}

#endif
	if (port == &PORTC) {
		*evsys_chmux = EVSYS_CHMUX_PORTC_PIN0_gc | pins_base;
	}

	if (port == &PORTD) {
		*evsys_chmux = EVSYS_CHMUX_PORTD_PIN0_gc | pins_base;
	}

#if XMEGA_A || XMEGA_C || XMEGA_D
	if (port == &PORTE) {
		*evsys_chmux = EVSYS_CHMUX_PORTE_PIN0_gc | pins_base;
	}
#  if (!XMEGA_A4) && (!XMEGA_C4) && (!XMEGA_D4)
	if (port == &PORTF) {
		*evsys_chmux = EVSYS_CHMUX_PORTF_PIN0_gc | pins_base;
	}
#  endif
#endif
}

static void qdec_enabled_evsys(qdec_config_t *config)
{
	volatile uint8_t *evsys_chctrl;
	uint32_t nb_filter_cycle;

	/* Configure event channel for quadrature decoding of pins */
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);

	/* Initialize CHMUX register */
	qdec_evsys_pin_2_chmux(config->port, config->pins_base,
			config->event_channel);

	/* In event channel enable Quadrature Decode with digital
	 * filter and Rotary filter
	 * (Rotary filter is only for XMEGA-E devices)
	 */
	evsys_chctrl = &EVSYS.CH0CTRL + config->event_channel;
	if (config->pins_filter_us > 1000) {
		nb_filter_cycle = (config->pins_filter_us / 1000)
				* (sysclk_get_per_hz() / 1000);
	} else {
		nb_filter_cycle = config->pins_filter_us
				* (sysclk_get_per_hz() / 1000000);
	}

#if XMEGA_E
	if (nb_filter_cycle > 7) {
		/* Compute prescaler need */
		uint8_t presc;
		for (presc = 0; presc < 4; presc++) {
			nb_filter_cycle /= 8;
			if (nb_filter_cycle < 8) {
				break;
			}
		}
		if (nb_filter_cycle > 7) {
			nb_filter_cycle = 7;
		}

		/* Enable prescaler of filter */
		EVSYS.DFCTRL = EVSYS_PRESCFILT_CH04_gc
				| (config->index.enabled ? EVSYS_PRESCFILT_CH15_gc : 0)
				| presc;
	}

	*evsys_chctrl = EVSYS_QDEN_bm | nb_filter_cycle
			| (config->rotary ? EVSYS_ROTARY_bm : 0);
#else
	if (nb_filter_cycle > 7) {
		nb_filter_cycle = 7;
	}
	*evsys_chctrl = EVSYS_QDEN_bm | nb_filter_cycle;
#endif

	if (config->index.enabled) {
		/* Configure event channel as index channel. When
		 * enabling index,in event channel n,event channel n+1
		 * should be used for index signal.
		 * And port pin-n+2 should be used for index
		 * signal input
		 */

		/* Enable quadrature decode index for event channel-0 */
		*evsys_chctrl |= ((uint8_t)config->index.rec_state
				<< EVSYS_QDIRM_gp)
				| EVSYS_QDIEN_bm;

		/* Initialize CHMUX register */
		qdec_evsys_pin_2_chmux(config->port, config->pins_base + 2,
				config->event_channel + 1);

		/* Enable digital filter*/
		evsys_chctrl++;
		*evsys_chctrl = nb_filter_cycle;
	}
}

static void qdec_enabled_tc(qdec_config_t *config)
{
	/* Configure TC for quadrature decode for event action and
	 * event channel selection:
	 * - Set event source and event action as per sent parameters
	 * - Load Period register of TC with number of counts for single
	 *   revolution
	 * - Write clock value and start timer
	 */
	tc_enable(config->timer);
	tc_set_input_capture(config->timer,
			(TC_EVSEL_t)(TC_EVSEL_CH0_gc + config->event_channel),
			TC_EVACT_QDEC_gc);
	tc_write_count(config->timer, 0);
	tc_write_period(config->timer, config->revolution - 1);
	tc_write_clock_source(config->timer, TC_CLKSEL_DIV1_gc);
}

static void qdec_enabled_tc_freq(qdec_config_t *config)
{
	volatile uint8_t *evsys_chctrl, *evsys_chctrl_freq;

	/* Configuration of frequency calculation */
	Assert(config->event_channel != config->freq_opt.event_channel);
	if (config->index.enabled) {
		Assert((config->event_channel + 1)
				!= config->freq_opt.event_channel);
	}

#if XMEGA_E

	/* Channel must be < 4, because QDec channel is 0
	 * and EVSYS.DFCTRL enables filter per event group */
	Assert(config->freq_opt.event_channel < 4);
#endif

	/* In event channel enable digital filter as QDec event channel */
#if XMEGA_E
	if (EVSYS.DFCTRL & EVSYS_PRESCFILT_CH04_gc) {
		if (config->freq_opt.event_channel == 1) {
			EVSYS.DFCTRL |= EVSYS_PRESCFILT_CH15_gc;
		} else if (config->freq_opt.event_channel == 2) {
			EVSYS.DFCTRL |= EVSYS_PRESCFILT_CH26_gc;
		} else {
			EVSYS.DFCTRL |= EVSYS_PRESCFILT_CH37_gc;
		}
	}

#endif
	evsys_chctrl_freq = &EVSYS.CH0CTRL + config->freq_opt.event_channel;
	evsys_chctrl = &EVSYS.CH0CTRL + config->event_channel;
	*evsys_chctrl_freq = *evsys_chctrl & EVSYS_DIGFILT_gm;

	/* Configure event channel for frequency calculation */
	qdec_evsys_pin_2_chmux(config->port, config->pins_base,
			config->freq_opt.event_channel);

	/* Configure TC to capture frequency
	 * Load timer period register
	 * Enable capture on CCA channel
	 * Select timer clock source
	 */
	tc_enable(config->freq_opt.timer);
	tc_set_input_capture(config->freq_opt.timer,
			(TC_EVSEL_t)(TC_EVSEL_CH0_gc
			+ config->freq_opt.event_channel),
			TC_EVACT_FRQ_gc);
	tc_write_count(config->freq_opt.timer, 0);
	tc_write_period(config->freq_opt.timer, 0xFFFF);
	tc_enable_cc_channels(config->freq_opt.timer, TC_CCAEN);
	tc_set_resolution(config->freq_opt.timer,
			(config->freq_opt.unit / 1000) / config->revolution);
	config->freq_opt.coef
			= (((uint64_t)tc_get_resolution(config->freq_opt.timer) * 1000)
			/ config->freq_opt.unit)
			* 4
			/ config->revolution;
	config->freq_opt.last_freq = 0; /* Initialize frequence to 0Hz */
}

void qdec_get_config_defaults(qdec_config_t *config)
{
#if !XMEGA_B3
	config->port = &PORTA;
#else
	config->port = &PORTB;
#endif
	config->pins_base = 0;
	config->pins_filter_us = 2;
	config->phases.pins_invert = false;
	config->index.enabled = false;
	config->rotary = false;
	config->event_channel = 0;
#if XMEGA_E
	config->timer = &TCC5;
	config->freq_opt.timer = &TCC4;
#else
	config->timer = &TCC0;
	config->freq_opt.timer = &TCC1;
#endif
	config->freq_opt.enabled = false;
	config->freq_opt.event_channel = 2;
	config->freq_opt.unit = 1000;
	config->revolution = 4;
}

void qdec_config_phase_pins(qdec_config_t *config, PORT_t *port,
		uint8_t pin_base, bool pins_invert, uint16_t filter_us)
{
	config->port = port;
	config->pins_base = pin_base;
	config->pins_filter_us = filter_us;
	config->phases.pins_invert = pins_invert;
}

void qdec_config_enable_index_pin(qdec_config_t *config, bool pin_invert,
		enum QDec_index_rec_state state)
{
	config->index.enabled = true;
	config->index.pin_invert = pin_invert;
	config->index.rec_state = state;
}

void qdec_config_disable_index_pin(qdec_config_t *config)
{
	config->index.enabled = false;
}

#if XMEGA_E
void qdec_config_enable_rotary(qdec_config_t *config)
{
	config->rotary = true;
}

void qdec_config_disable_rotary(qdec_config_t *config)
{
	config->rotary = false;
}
#endif

void qdec_config_event_channel(qdec_config_t *config, uint8_t event_channel)
{
	config->event_channel = event_channel;
}

void qdec_config_tc(qdec_config_t *config, volatile void *timer)
{
	config->timer = timer;
}

void qdec_config_revolution(qdec_config_t *config, uint16_t revolution)
{
	Assert((revolution % 4) == 0);
	config->revolution = revolution;
}

void qdec_config_enable_freq(qdec_config_t *config, uint32_t unit)
{
	config->freq_opt.enabled = true;
	config->freq_opt.unit = unit;
}

void qdec_config_freq_event_channel(qdec_config_t *config,
		uint8_t event_channel)
{
	config->freq_opt.event_channel = event_channel;
}

void qdec_config_freq_tc(qdec_config_t *config, volatile void *timer)
{
	config->freq_opt.timer = timer;
}

void qdec_enabled(qdec_config_t *config)
{
	/* Check PORT configuration */
#if XMEGA_B3
	Assert((config->port >= &PORTB) && (config->port <= &PORTD));
#elif XMEGA_B || XMEGA_E
	Assert((config->port >= &PORTA) && (config->port <= &PORTD));
#elif (!XMEGA_A4) && (!XMEGA_C4) && (!XMEGA_D4)
	Assert((config->port >= &PORTA) && (config->port <= &PORTF));
#else
	Assert((config->port >= &PORTA) && (config->port <= &PORTE));
#endif
	/* Check channel configuration */
#if XMEGA_B || XMEGA_E
	Assert(config->event_channel == 0);
#elif XMEGA_D
	Assert((config->event_channel == 0)
			|| (config->event_channel == 2));
#elif XMEGA_A || XMEGA_C
	Assert((config->event_channel == 0)
			|| (config->event_channel == 2)
			|| (config->event_channel == 4));
#endif
	/* Check rotary configuration */
#if !XMEGA_E
	Assert(!config->rotary);
#endif

	qdec_enabled_pins(config);
	qdec_enabled_evsys(config);
	qdec_enabled_tc(config);
	if (config->freq_opt.enabled) {
		qdec_enabled_tc_freq(config);
	}
}

void qdec_reset_position(qdec_config_t *config)
{
	tc_write_count(config->timer, 0);
}

uint16_t qdec_get_position(qdec_config_t *config)
{
	return tc_read_count(config->timer);
}

bool qdec_get_direction(qdec_config_t *config)
{
#if XMEGA_E
	return !(((TC5_t *)config->timer)->CTRLGSET & TC5_DIR_bm);

#else
	return !(((TC0_t *)config->timer)->CTRLFSET & TC0_DIR_bm);

#endif
}

uint16_t qdec_get_frequency(qdec_config_t *config)
{
	uint16_t capture = 0;

	/* While the CC flag is setted, read the CC value.
	 * This allows to read all CC buffers on XMEGA E and read the last value
	 */
	while (tc_is_cc_interrupt(config->freq_opt.timer, TC_CCA)) {
		capture = tc_read_cc(config->freq_opt.timer, TC_CCA);
	}
	if (capture == 0) {
		/* Out of range and certainly due to a rebounce not filtered */
		return config->freq_opt.last_freq;
	}

	config->freq_opt.last_freq = config->freq_opt.coef / capture;
	return config->freq_opt.last_freq;
}