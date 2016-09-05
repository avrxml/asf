/**
 * @file tal_auto_csma.c
 *
 * @brief This file handles CSMA / CA before frame transmission within the TAL
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

#include "tal_config.h"

#ifndef BASIC_MODE

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_internal.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

static uint8_t NB[NUM_TRX];
static uint8_t BE[NUM_TRX];

/* === PROTOTYPES ========================================================== */

static void start_backoff(trx_id_t trx_id);
static void cca_start(void *cb_timer_element);

#ifdef SUPPORT_MODE_SWITCH
static void trigger_cca_meaurement(trx_id_t trx_id);

#endif

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Starts software-controlled CSMA.
 *
 * @param trx_id Transceiver identifier
 */
void csma_start(trx_id_t trx_id)
{
	/* Initialize CSMA variables */
	NB[trx_id] = 0;
	BE[trx_id] = tal_pib[trx_id].MinBE;

	if (BE[trx_id] == 0) {
		/* Collision avoidance is disabled during first iteration */
#ifdef SUPPORT_MODE_SWITCH
		if (tal_pib[trx_id].ModeSwitchEnabled) {
			tx_ms_ppdu(trx_id);
		} else
#endif
		{
			transmit_frame(trx_id, NO_CCA);
		}
	} else {
		/* Start backoff timer to trigger CCA */
		start_backoff(trx_id);
	}
}

/**
 * @brief Starts the timer for the backoff period and enables receiver.
 *
 * @param trx_id Transceiver identifier
 */
static void start_backoff(trx_id_t trx_id)
{
	/* Start backoff timer to trigger CCA */
	uint8_t backoff_8;
	backoff_8  = (uint8_t)(rand() & (((uint16_t)1 << BE[trx_id]) - 1));
	if (backoff_8 > 0) {
		uint16_t backoff_16;
		uint32_t backoff_duration_us;
		backoff_16 = backoff_8 * aUnitBackoffPeriod;
		backoff_duration_us
			= (uint32_t)tal_pib[trx_id].SymbolDuration_us *
				(uint32_t)backoff_16;
#ifdef REDUCED_BACKOFF_DURATION
		backoff_duration_us = REDUCED_BACKOFF_DURATION;
#endif
		uint8_t timer_id;
		if (trx_id == RF09) {
			timer_id = TAL_T_0;
		} else {
			timer_id = TAL_T_1;
		}

		retval_t status
			= pal_timer_start(timer_id,
				backoff_duration_us,
				TIMEOUT_RELATIVE,
				(FUNC_PTR)cca_start,
				(void *)trx_id);
		if (status != MAC_SUCCESS) {
			tx_done_handling(trx_id, status);
			return;
		} else {
			/* Switch to TRXOFF during backoff */
			tx_state[trx_id] = TX_BACKOFF;

			if ((trx_default_state[trx_id] == RF_TRXOFF) ||
					(tal_pib[trx_id].
					NumRxFramesDuringBackoff <
					tal_pib[trx_id].
					MaxNumRxFramesDuringBackoff)) {
				if (trx_state[trx_id] != RF_TXPREP) {
					switch_to_txprep(trx_id);
				}
			} else { /* RF_RX */
				/* Stay in Rx */
			}
		}
	} else { /* no backoff required */
		/* Start CCA immediately - no backoff */

		/*
		 * The trx id is required in the callback function,
		 * so create a proper timer element.
		 */
		cca_start((void *)trx_id);
	}
}

/**
 * @brief Start CCA.
 *
 * @param parameter Pointer to timer element containing the trx_id
 */
static void cca_start(void *cb_timer_element)
{
	/* Immediately store trx id from callback. */
	trx_id_t trx_id = (trx_id_t)cb_timer_element;
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	/* ACK transmission is understood as channel busy */
	if (ack_transmitting[trx_id]) {
		csma_continue(trx_id);
		return;
	}

	/* Check if trx is currently detecting a frame ota */
	if (trx_state[trx_id] == RF_RX) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		uint8_t agc_freeze = trx_bit_read(
				reg_offset + SR_RF09_AGCC_FRZS);
		if (agc_freeze) {
			csma_continue(trx_id);
		} else {
#ifdef SUPPORT_MODE_SWITCH
			if (tal_pib[trx_id].ModeSwitchEnabled) {
				trigger_cca_meaurement(trx_id);
			} else
#endif
			{
				transmit_frame(trx_id, WITH_CCA);
			}
		}
	} else {
#ifdef SUPPORT_MODE_SWITCH
		if (tal_pib[trx_id].ModeSwitchEnabled) {
			trigger_cca_meaurement(trx_id);
		} else
#endif
		{
			transmit_frame(trx_id, WITH_CCA);
		}
	}
}

#ifdef SUPPORT_MODE_SWITCH

/**
 * @brief Triggers CCA measurement at transceiver
 *
 * @param trx_id Transceiver identifier
 */
static void trigger_cca_meaurement(trx_id_t trx_id)
{
	/* Trigger CCA measurement */

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	/* Cancel any ongoing reception and ensure that TXPREP is reached. */
	if (trx_state[trx_id] == RF_TRXOFF) {
		switch_to_txprep(trx_id);
	}

	/* Disable BB */
	trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 0);

	/* Enable IRQ EDC */
	trx_bit_write( reg_offset + SR_RF09_IRQM_EDC, 1);

	/* CCA duration is already set by default; see apply_phy_settings() */
	/* Setup and start energy detection */
	trx_bit_write( reg_offset + SR_RF09_AGCC_FRZC, 0); /* Ensure AGC is not
	                                                    * hold */
	if (trx_state[trx_id] != RF_RX) {
		stop_rpc(trx_id);
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_RX);
		pal_timer_delay(tal_pib[trx_id].agc_settle_dur); /* allow
		                                                  * filters to
		                                                  * settle */
		trx_state[trx_id] = RF_RX;
	}

	tx_state[trx_id] = TX_CCA;
	/* Start single ED measurement; use reg_write - it's the only
	 *subregister */
	trx_reg_write( reg_offset + RG_RF09_EDC, RF_EDSINGLE);

	/* Wait for EDC IRQ and handle it within cca_done_handling() */
}

#endif

#ifdef SUPPORT_MODE_SWITCH

/**
 * @brief Callback function for CCA completion.
 *
 * @param trx_id Transceiver identifier
 */
void cca_done_handling(trx_id_t trx_id)
{
	switch_to_txprep(trx_id); /* Leave state Rx */

	/* Switch BB on again */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 1);

	/* Determine if channel is idle */
	if (tal_current_ed_val[trx_id] < tal_pib[trx_id].CCAThreshold) {
		/* Idle */
		tx_ms_ppdu(trx_id);
	} else {
		/* Busy */
		csma_continue(trx_id);
	}
}

#endif

/**
 * @brief Continues CSMA; handles next CSMA retry.
 *
 * @param trx_id Transceiver identifier
 */
void csma_continue(trx_id_t trx_id)
{
	NB[trx_id]++;

	if (NB[trx_id] > tal_pib[trx_id].MaxCSMABackoffs) {
		tx_done_handling(trx_id, MAC_CHANNEL_ACCESS_FAILURE);
	} else {
		BE[trx_id]++;
		if (BE[trx_id] > tal_pib[trx_id].MaxBE) {
			BE[trx_id] = tal_pib[trx_id].MaxBE;
		}

		/* Start backoff timer to trigger CCA */
		start_backoff(trx_id);
	}
}

#endif /* #ifndef BASIC_MODE */

/* EOF */
