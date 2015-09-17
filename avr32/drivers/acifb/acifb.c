/**
 * \file
 *
 * \brief ACIFB software driver for AVR32 UC3.
 *
 * Copyright (C) 2009-2015 Atmel Corporation. All rights reserved.
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

#include <avr32/io.h>
#include "compiler.h"
#include "acifb.h"

#ifdef AVR32_ACIFB_202_H_INCLUDED
#define AVR32_ACIFB_CONF7_EVENP                                      17
#define AVR32_ACIFB_CONF7_EVENP_MASK                         0x00020000
#define AVR32_ACIFB_CONF7_EVENP_OFFSET                               17
#define AVR32_ACIFB_CONF7_EVENP_SIZE                                  1
#define AVR32_ACIFB_CONF7_EVENN                                      16
#define AVR32_ACIFB_CONF7_EVENN_MASK                         0x00010000
#define AVR32_ACIFB_CONF7_EVENN_OFFSET                               16
#define AVR32_ACIFB_CONF7_EVENN_SIZE                                  1
/* These defines are missing or wrong in the toolchain header acifb_202.h */
#endif

/**
 * \brief Configure a set of AC channels in normal mode
 *
 * \param acifb Base address of the ACIFB
 * \param ac_chan Array of channels configuration [INPUT]
 * \param nb_chan The size of the ac_chan[] array [INPUT]
 *
 */
void acifb_channels_setup(volatile avr32_acifb_t *acifb,
		const acifb_channel_t *ac_chan, uint32_t nb_chan)
{
	uint32_t i;
	for (i = 0; i < nb_chan; i++) {
		/* Apply the channel configuration. */
		*(&(acifb->conf0) + ac_chan[i].ac_n) = (
			((ac_chan[i].filter_len <<
			AVR32_ACIFB_CONF7_FLEN_OFFSET)
			& AVR32_ACIFB_CONF7_FLEN_MASK)
			| ((ac_chan[i].hysteresis_value <<
			AVR32_ACIFB_CONF7_HYS_OFFSET)
			& AVR32_ACIFB_CONF7_HYS_MASK)
			| (((ac_chan[i].event_negative) ? 1 : 0)
			<< AVR32_ACIFB_CONF7_EVENN_OFFSET)
			| (((ac_chan[i].event_positive) ? 1 : 0)
			<< AVR32_ACIFB_CONF7_EVENP_OFFSET)
			| ((ac_chan[i].positive_input <<
			AVR32_ACIFB_CONF7_INSELP_OFFSET)
			& AVR32_ACIFB_CONF7_INSELP_MASK)
			| ((ac_chan[i].negative_input <<
			AVR32_ACIFB_CONF7_INSELN_OFFSET)
			& AVR32_ACIFB_CONF7_INSELN_MASK)
			| ((ac_chan[i].mode << AVR32_ACIFB_CONF7_MODE_OFFSET)
			& AVR32_ACIFB_CONF7_MODE_MASK)
			| ((ac_chan[i].interrupt_settings <<
			AVR32_ACIFB_CONF7_IS_OFFSET)
			& AVR32_ACIFB_CONF7_IS_MASK));
	}
}

/**
 * \brief Configure a set of AC channels in Window mode
 *
 * \param acifb Base address of the ACIFB
 * \param ac_window Array of Windows configuration [INPUT]
 * \param nb_combinations The size of the ac_window[] array [INPUT]
 *
 */
void acifb_windows_setup(volatile avr32_acifb_t *acifb,
		const acifb_window_t *ac_window, uint32_t nb_combinations)
{
	uint32_t i;
	for (i = 0; i < nb_combinations; i++) {
		/* Apply the channel configuration. */
		*(&(acifb->confw0) + ac_window[i].ac_combination_number) = (
			(ac_window[i].window_mode <<
			AVR32_ACIFB_CONFW0_WFEN_OFFSET) |
			(ac_window[i].window_event_enable <<
			AVR32_ACIFB_CONFW0_WEVEN_OFFSET) |
			((ac_window[i].window_event <<
			AVR32_ACIFB_CONFW0_WEVSRC_OFFSET) &
			AVR32_ACIFB_CONFW0_WEVSRC_MASK) |
			((ac_window[i].interrupt_settings <<
			AVR32_ACIFB_CONFW0_WIS_OFFSET) &
			AVR32_ACIFB_CONFW0_WIS_MASK));
	}
}

/**
 * \brief Configure and enable the ACIFB module
 *
 * \param acifb Base address of the ACIFB
 * \param ac ACIFB configuration [INPUT]
 *
 */
void acifb_setup_and_enable(volatile avr32_acifb_t *acifb, const acifb_t *ac)
{
	acifb->ctrl = (
		((ac->sut << AVR32_ACIFB_SUT_OFFSET) & AVR32_ACIFB_SUT_MASK)
		| ((ac->actest <<
		AVR32_ACIFB_ACTEST_OFFSET) & AVR32_ACIFB_ACTEST_MASK)
		| ((ac->eventen << AVR32_ACIFB_EVENTEN_OFFSET)
		& AVR32_ACIFB_EVENTEN_MASK)
		| (1u << AVR32_ACIFB_EN_OFFSET));
}

/**
 * \brief Wait for channels to be ready
 *
 * \param acifb Base address of the ACIFB
 * \param acrdy_mask Mask of several AVR32_ACIFB_SR_ACRDYn_MASK [INPUT]
 *
 * \retval false Timeout on ACIFB configuration
 * \retval true  ACIFB channels are ready
 */
bool acifb_wait_channels_ready(volatile avr32_acifb_t *acifb,
		const uint32_t acrdy_mask)
{
	uint32_t timeout = ACIFB_POLL_TIMEOUT;
	while ((acifb->sr & acrdy_mask) != acrdy_mask) {
		if (--timeout == 0) {
			return false;
		}
	}
	return true;
}
