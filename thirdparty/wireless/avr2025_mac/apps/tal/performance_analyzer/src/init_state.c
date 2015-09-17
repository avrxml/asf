/**
 * \file init_state.c
 *
 * \brief Initilization functions and utilities -
 * Performance Analyzer application
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* === INCLUDES ============================================================ */

#include <stdbool.h>
#include <stdint.h>
#include "tal.h"
#include "tal_helper.h"
#include "app_init.h"
#include "perf_api_serial_handler.h"
#include "sio2host.h"
#include "common_sw_timer.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */
static void configuration_mode_selection(void);

/* === GLOBALS ============================================================= */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initialization task for INIT STATE. All hardware, PAL, TAL and stack
 *        level initialization must be done in this function
 *
 * \param arg arguments for INIT state
 */

static void app_timers_init(void);

uint8_t T_APP_TIMER;
uint8_t T_APP_TIMER_RANGE;
uint8_t APP_TIMER_TO_TX;
uint8_t APP_TIMER_TO_TX_LED_OFF;
uint8_t APP_TIMER_TO_RX_LED_OFF;
uint8_t CW_TX_TIMER;

void init_state_init(void *arg)
{
	sw_timer_init();

	/* Set the node information base */
	config_node_ib();

	/* Initialize the TAL layer */
	if (tal_init() != MAC_SUCCESS) {
		/* something went wrong during initialization */
		app_alert();
	}

	app_timers_init();

	/* Initilaize sio rx state */
	init_sio();

	/* select the configurtion mode */
	configuration_mode_selection();

#if (TAL_TYPE == AT86RF233) && (ANTENNA_DIVERSITY == 1)
	/* In order to demonstrate RPC the antenna diversity is disabled. */
	tal_ant_div_config(ANT_DIVERSITY_DISABLE, ANT_CTRL_1); /* Enable A1/X2
	                                                       **/
#endif

	/* Keep compiler happy */
	arg = arg;
}

/**
 * \brief Checks whether Configuartion Mode is selected or not
 *
 * This will be checked during the INIT state
 * \ingroup group_config_mode
 */
static void configuration_mode_selection(void)
{
	/* Is button pressed */
	if (button_pressed()) {
		/* Enable configuration mode */
		node_info.configure_mode = true;
	} else {
		node_info.configure_mode = false;
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
	if (STATUS_OK != (status_code_genare_t)sw_timer_get_id(&T_APP_TIMER)) {
		app_alert();
	}

	if (STATUS_OK !=
			(status_code_genare_t)sw_timer_get_id(&APP_TIMER_TO_TX))
	{
		app_alert();
	}

	if (STATUS_OK !=
			(status_code_genare_t)sw_timer_get_id(&
			APP_TIMER_TO_TX_LED_OFF)) {
		app_alert();
	}

	if (STATUS_OK !=
			(status_code_genare_t)sw_timer_get_id(&
			APP_TIMER_TO_RX_LED_OFF)) {
		app_alert();
	}

	if (STATUS_OK !=
			(status_code_genare_t)sw_timer_get_id(&T_APP_TIMER_RANGE))
	{
		app_alert();
	}

	if (STATUS_OK !=
			(status_code_genare_t)sw_timer_get_id(&CW_TX_TIMER)) {
		app_alert();
	}
}
