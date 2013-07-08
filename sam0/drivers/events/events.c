/**
 * \file
 *
 * \brief SAM D20 Event System Controller Driver
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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

#include <events.h>
#include <system.h>

/**
 * \brief Initializes the event driver.
 *
 * Initializes the event driver ready for use. This resets the underlying
 * hardware modules, clearing any existing event channel configuration(s).
 */
void events_init(void)
{
	/* Turn on the event system interface clock in the PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_EVSYS);

	/* Software reset the module to ensure it is re-initialized correctly */
	EVSYS->CTRL.reg = EVSYS_CTRL_SWRST;

	while (EVSYS->CTRL.reg & EVSYS_CTRL_SWRST) {
	}
}

/**
 * \brief Writes an Event System channel configuration to the hardware module.
 *
 * Writes out a given configuration of a Event System channel configuration to
 * the hardware module.
 *
 * \pre The user must be configured before the channel is configured, see
 * \ref events_user_set_config
 *
 * \param[in] event_channel  Event channel to configure
 * \param[in] config         Configuration settings for the event channel
 */
void events_chan_set_config(
		const enum events_channel event_channel,
		struct events_chan_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Get the channel number from the enum selector */
	uint8_t channel = (uint8_t)event_channel;

	/* Setting up GCLK for the event channel only takes effect for the
	 * synchronous and re-synchronous paths */
	if (config->path != EVENT_PATH_ASYNCHRONOUS) {
		/* Set up a GLCK channel to use with the specific channel */
		struct system_gclk_chan_config gclk_chan_conf;
		system_gclk_chan_get_config_defaults(&gclk_chan_conf);
		gclk_chan_conf.source_generator = config->clock_source;
		system_gclk_chan_set_config(EVSYS_GCLK_ID_0 + channel, &gclk_chan_conf);
		system_gclk_chan_enable(EVSYS_GCLK_ID_0 + channel);
	}

	/* Select and configure the event channel (must be done in one
	 * word-access write as specified in the module datasheet */
	EVSYS->CHANNEL.reg = (channel << EVSYS_CHANNEL_CHANNEL_Pos) |
			(config->generator_id << EVSYS_CHANNEL_EVGEN_Pos) |
			(config->edge_detection << EVSYS_CHANNEL_EDGSEL_Pos) |
			(config->path << EVSYS_CHANNEL_PATH_Pos);
}

/**
 * \brief Writes an Event System user MUX configuration to the hardware module.
 *
 * Writes out a given configuration of a Event System user MUX configuration to
 * the hardware module.
 *
 * \param[in] event_user  Event User MUX index to configure, a \c EVSYS_ID_USER_*
 *                        constant from the device header files
 * \param[in] config      Configuration settings for the event user MUX
 */
void events_user_set_config(
		const uint8_t event_user,
		struct events_user_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Get the event channel number from the channel selector */
	uint8_t channel = (uint8_t)(config->event_channel_id);

	/* Add one to the channel selector as the channel number is 1 indexed for
	   the user MUX setting */
	channel = channel + 1;

	/* Select and configure the user MUX channel (must be done in one
	 * word-access write as specified in the module datasheet */
	EVSYS->USER.reg = (event_user << EVSYS_USER_USER_Pos) |
			(channel << EVSYS_USER_CHANNEL_Pos);
}
