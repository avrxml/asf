/**
 * \file init_state.c
 *
 * \brief Initilization functions and utilities -
 * Performance Analyzer application
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
