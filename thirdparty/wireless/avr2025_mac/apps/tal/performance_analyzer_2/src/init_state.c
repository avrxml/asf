/**
 * \file init_state.c
 *
 * \brief Initilization functions and utilities -
 * Performance Analyzer application for AT86RF215
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* === INCLUDES ============================================================ */

#include <stdbool.h>
#include <stdint.h>
#include "tal.h"
#include "tal_internal.h"
#include "tal_helper_2.h"
#include "ieee_const.h"
#include "app_init.h"
#include "perf_api.h"
#include "perf_api_serial_handler.h"
#include "common_sw_timer.h"
#include "tal_generic.h"
#include "user_interface.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */
static void configuration_mode_selection(trx_id_t trx);
static void app_timers_init(void);

/* === GLOBALS ============================================================= */
uint8_t T_APP_TIMER;
uint8_t T_APP_TIMER_RANGE_RF09;
uint8_t T_APP_TIMER_RANGE_RF24;
uint8_t APP_TIMER_TO_TX;
uint8_t APP_TIMER_TO_TX_LED_OFF;
uint8_t APP_TIMER_TO_RX_LED_OFF;
/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initialization task for INIT STATE. All hardware, PAL, TAL and stack
 *        level initialization must be done in this function
 *
 * \param arg arguments for INIT state
 */
void init_state_init(trx_id_t trx, void *arg)
{
	sw_timer_init();

	/* Set the node information base */
	config_node_ib(trx);

	/* Initialize the TAL layer */
	if (tal_init() != MAC_SUCCESS) {
		/* something went wrong during initialization */
		app_alert();
	}

	app_timers_init();

	/* Initialize sio rx state */
	init_sio(trx);

	/* select the configuration mode */
	configuration_mode_selection(trx);

	/* if(trx == RF09) */
	{
		/* Configure PHY for sub-1GHz */
		phy_t phy;
		phy.modulation = LEG_OQPSK;
		phy.phy_mode.leg_oqpsk.chip_rate  = CHIP_RATE_1000;
		phy.freq_band = US_915;
		phy.ch_spacing = LEG_915_CH_SPAC;
		phy.freq_f0 = LEG_915_F0;
		if (tal_pib_set(RF09, phySetting,
				(pib_value_t *)&phy) != MAC_SUCCESS) {
			app_alert();
		}
	}
	/* else */
	{
		/* Configure PHY for 2.4GHz */
		phy_t phy;
		phy.modulation = LEG_OQPSK;
		phy.phy_mode.leg_oqpsk.chip_rate = CHIP_RATE_2000;
		phy.freq_band = WORLD_2450;
		phy.ch_spacing = LEG_2450_CH_SPAC;
		phy.freq_f0 = LEG_2450_F0;
		if (tal_pib_set(RF24, phySetting,
				(pib_value_t *)&phy) != MAC_SUCCESS) {
			app_alert();
		}
	}

	/* Keep compiler happy */
	arg = arg;
}

void init_after_disconnect(trx_id_t trx)
{
	node_info[trx].peer_found = false;
	/* Reset trx */
	if (trx_reset(trx) != MAC_SUCCESS) {
		app_alert();
	}

	/* Configure transceiver */
	trx_config(trx);

	tal_rx_buffer[trx] = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Init incoming frame queue */
	qmm_queue_init(&tal_incoming_frame_queue[trx]);

	tal_state[trx] = TAL_IDLE;

	/* Initialize sio rx state */
	init_sio(trx);

	/* select the configuration mode */
	configuration_mode_selection(trx);

	if (trx == RF09) {
		/* Configure PHY for sub-1GHz */
		phy_t phy;
		phy.modulation = LEG_OQPSK;
		phy.phy_mode.leg_oqpsk.chip_rate  = CHIP_RATE_1000;
		phy.freq_band = US_915;
		phy.ch_spacing = LEG_915_CH_SPAC;
		phy.freq_f0 = LEG_915_F0;
		if (tal_pib_set(RF09, phySetting,
				(pib_value_t *)&phy) != MAC_SUCCESS) {
			app_alert();
		}
	} else {
		/* Configure PHY for 2.4GHz */
		phy_t phy;
		phy.modulation = LEG_OQPSK;
		phy.phy_mode.leg_oqpsk.chip_rate = CHIP_RATE_2000;
		phy.freq_band = WORLD_2450;
		phy.ch_spacing = LEG_2450_CH_SPAC;
		phy.freq_f0 = LEG_2450_F0;
		if (tal_pib_set(RF24, phySetting,
				(pib_value_t *)&phy) != MAC_SUCCESS) {
			app_alert();
		}
	}
}

/**
 * \brief Checks whether Configuration Mode is selected or not
 *
 * This will be checked during the INIT state
 * \ingroup group_config_mode
 */
static void configuration_mode_selection(trx_id_t trx)
{
	/* Is button pressed */
	if (button_pressed()) {
		/* Enable configuration mode */
		node_info[trx].configure_mode = true;
	} else {
		node_info[trx].configure_mode = false;
	}

	/*
	 * Wait for the user to release the button to proceed further, otherwise
	 * button press will start Peer search in Range measurement mode which
	 * is
	 * not an intended behavior
	 */
	while (button_pressed()) {
	}
}

static void app_timers_init(void)
{
	if (STATUS_OK != sw_timer_get_id(&T_APP_TIMER)) {
		app_alert();
	}

	if (STATUS_OK != sw_timer_get_id(&APP_TIMER_TO_TX)) {
		app_alert();
	}

	if (STATUS_OK != sw_timer_get_id(&APP_TIMER_TO_TX_LED_OFF)) {
		app_alert();
	}

	if (STATUS_OK != sw_timer_get_id(&APP_TIMER_TO_RX_LED_OFF)) {
		app_alert();
	}

	if (STATUS_OK != sw_timer_get_id(&T_APP_TIMER_RANGE_RF09)) {
		app_alert();
	}

	if (STATUS_OK != sw_timer_get_id(&T_APP_TIMER_RANGE_RF24)) {
		app_alert();
	}
}
