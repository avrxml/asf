/**
 * \file
 *
 * \brief SAM4L Peripheral Event Controller (PEVC) Driver.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include <events.h>
#include <sysclk.h>
#include <sleepmgr.h>

/**
 * \brief Initialize an events configuration structure to defaults.
 *
 *  The default configuration is as follows:
 *  - Input Glitch Filter Divider is set to \ref EVENT_IGF_DIVIDER_1024
 *
 *  \param[out] config    Configuration structure to initialize to default values
 */
void events_get_config_defaults(
		struct events_conf *const config)
{
	/* Validate parameters. */
	Assert(config);
	
	config->igf_divider = EVENT_IGF_DIVIDER_1024;
}

/**
 * \brief Initialize the events module.
 *
 *  \param[in] config    Configuration structure to initialize to default values
 */
void events_init(
		struct events_conf *const config)
{
	/* Validate parameters. */
	Assert(config);
	
	/* Enable clock for PEVC module */
	sysclk_enable_peripheral_clock(PEVC);

	/* Set configuration */
	events_set_igf_divider(config->igf_divider);
}

/**
 * \brief Enable the events module.
 */
void events_enable(void)
{
	sysclk_enable_peripheral_clock(PEVC);
	sleepmgr_lock_mode(SLEEPMGR_BACKUP);
}

/**
 * \brief Disable the events module.
 */
void events_disable(void)
{
	sysclk_disable_peripheral_clock(PEVC);
	sleepmgr_unlock_mode(SLEEPMGR_BACKUP);
}

/**
 * \brief Initialize an Event Channel configuration structure to defaults.
 *
 *  The default configuration is as follows:
 *  - Channel ID is initialized to invalid number
 *  - Generator ID is initialized to invalid number
 *  - Event shaper is disabled
 *  - Event Input Glitch Filter is disabled
 *
 *  \param[out] config    Configuration structure to initialize to default values
 */
void events_ch_get_config_defaults(
		struct events_ch_conf *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->channel_id = EVENT_CHANNEL_N;
	config->generator_id = EVENT_GENERATOR_N;
	config->shaper_enable = false;
	config->igf_edge = EVENT_IGF_EDGE_NONE;
}

/**
 * \brief Configure an Event Channel.
 *
 * \param[in,out] config      Configuration settings for the Event Channel
 */
void events_ch_configure(
		struct events_ch_conf *const config)
{
	uint32_t evs_val = 0;

	/* Sanity check arguments */
	Assert(config);
	Assert(config->channel_id < EVENT_CHANNEL_N);
	Assert(config->generator_id < EVENT_GENERATOR_N);

	/* Disable the channel first */
	events_ch_disable(config->channel_id);

	/* Configure the event channel */
	PEVC->PEVC_CHMX[config->channel_id].PEVC_CHMX =
		PEVC_CHMX_EVMX(config->generator_id);
	if (config->shaper_enable) {
		evs_val |= PEVC_EVS_EN;
	} else {
		evs_val &= (~PEVC_EVS_EN);
	}
	switch (config->igf_edge) {
	case EVENT_IGF_EDGE_RISING:
		evs_val |= PEVC_EVS_IGFR_1;
		break;
	case EVENT_IGF_EDGE_FALLING:
		evs_val |= PEVC_EVS_IGFF_1;
		break;
	case EVENT_IGF_EDGE_BOTH:
		evs_val |= (PEVC_EVS_IGFR_1 | PEVC_EVS_IGFF_1);
		break;
	case EVENT_IGF_EDGE_NONE:
	default:
		break;
	}
	PEVC->PEVC_EVS[config->generator_id].PEVC_EVS = evs_val;
}
