/**
 * \file
 *
 * \brief SAM D20 Event System Driver Quick Start
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
#include <asf.h>

void configure_event_channel(void);
void configure_event_user(void);

//! [setup]
#define EXAMPLE_EVENT_GENERATOR    0
#define EXAMPLE_EVENT_CHANNEL      EVENT_CHANNEL_0
#define EXAMPLE_EVENT_USER         0

void configure_event_channel(void)
{
//! [setup_1]
	struct events_chan_config config_events_chan;
//! [setup_1]
//! [setup_2]
	events_chan_get_config_defaults(&config_events_chan);
//! [setup_2]

//! [setup_3]
	config_events_chan.generator_id   = EXAMPLE_EVENT_GENERATOR;
	config_events_chan.edge_detection = EVENT_EDGE_RISING;
	config_events_chan.path           = EVENT_PATH_SYNCHRONOUS;
//! [setup_3]
//! [setup_4]
	events_chan_set_config(EXAMPLE_EVENT_CHANNEL, &config_events_chan);
//! [setup_4]
}

void configure_event_user(void)
{
//! [setup_5]
	struct events_user_config config_events_user;
//! [setup_5]
//! [setup_6]
	events_user_get_config_defaults(&config_events_user);
//! [setup_6]

//! [setup_7]
	config_events_user.event_channel_id = EXAMPLE_EVENT_CHANNEL;
//! [setup_7]
//! [setup_8]
	events_user_set_config(EXAMPLE_EVENT_USER, &config_events_user);
//! [setup_8]
}
//! [setup]

int main(void)
{
	//! [setup_init]
	events_init();

	configure_event_user();
	configure_event_channel();
	//! [setup_init]

	//! [main]
	//! [main_1]
	while (events_chan_is_ready(EXAMPLE_EVENT_CHANNEL) == false) {
		/* Wait for channel */
	};
	//! [main_1]

	//! [main_2]
	events_chan_software_trigger(EXAMPLE_EVENT_CHANNEL);
	//! [main_2]

	while (true) {
		/* Nothing to do */
	}
	//! [main]
}
