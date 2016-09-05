/**
 * @file tal_auto_ack.c
 *
 * @brief This file implements acknowledgement handling
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
#include <inttypes.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "stack_config.h"
#include "tal_pib.h"
#include "tal_internal.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

static void ack_timout_cb(void *cb_timer_element);

/* === IMPLEMENTATION ====================================================== */

/* --- ACK transmission ---------------------------------------------------- */

/**
 * @brief Handles end of ACK transmission
 *
 * This function is called with the TXFE IRQ.
 * It handles further processing after an ACK has been transmitted.
 *
 * @param trx_id Id of the corresponding trx
 */
void ack_transmission_done(trx_id_t trx_id)
{
	ack_transmitting[trx_id] = false;

#ifdef SUPPORT_FSK
	if (tal_pib[trx_id].RPCEnabled && tal_pib[trx_id].phy.modulation ==
			FSK) {
		/* Configure preamble length for reception */
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_reg_write( reg_offset + RG_BBC0_FSKPLL,
				(uint8_t)(tal_pib[trx_id].FSKPreambleLengthMin &
				0xFF));
		trx_bit_write( reg_offset + SR_BBC0_FSKC1_FSKPLH,
				(uint8_t)(tal_pib[trx_id].FSKPreambleLengthMin
				>> 8));
	}

#endif

#ifdef MEASURE_ON_AIR_DURATION
	tal_pib[trx_id].OnAirDuration += tal_pib[trx_id].ACKDuration_us;
#endif

	complete_rx_transaction(trx_id);

	switch (tx_state[trx_id]) {
	case TX_IDLE:
		switch_to_rx(trx_id);
		break;

	case TX_DEFER:
		/* Continue with deferred transmission */
		continue_deferred_transmission(trx_id);
		break;

	default:
		Assert("Unexpected tx_state" == 0);
		break;
	}
}

/* --- ACK reception ------------------------------------------------------- */

/**
 * @brief Checks if received frame is an ACK frame
 *
 * @param trx_id Transceiver identifier
 *
 * @return true if frame is an ACK frame, else false
 */
bool is_frame_an_ack(trx_id_t trx_id)
{
	bool ret;

	/* Check frame length */
	if (rx_frm_info[trx_id]->len_no_crc == 3) {
		/* Check frame type and frame version */
		if ((rx_frm_info[trx_id]->mpdu[0] & FCF_FRAMETYPE_ACK) &&
				(((rx_frm_info[trx_id]->mpdu[1] >>
				FCF1_FV_SHIFT) & 0x03) <=
				FCF_FRAME_VERSION_2006)) {
			ret = true;
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}

	return ret;
}

/**
 * @brief Checks if received ACK is an valid ACK frame
 *
 * @param trx_id Transceiver identifier
 *
 * @return true if ACK frame is valid, else false
 */
bool is_ack_valid(trx_id_t trx_id)
{
	bool ret;

	/* Check sequence number */
	if (rx_frm_info[trx_id]->mpdu[2] == mac_frame_ptr[trx_id]->mpdu[2]) {
		ret = true;
	} else {
		ret = false;
	}

	return ret;
}

/**
 * @brief Starts the timer to wait for an ACK reception
 *
 * @param trx_id Id of the corresponding trx
 */
void start_ack_wait_timer(trx_id_t trx_id)
{
	tx_state[trx_id] = TX_WAITING_FOR_ACK;
	uint8_t timer_id;
	if (trx_id == RF09) {
		timer_id = TAL_T_0;
	} else {
		timer_id = TAL_T_1;
	}

	retval_t status
		= pal_timer_start(timer_id,
			tal_pib[trx_id].ACKWaitDuration,
			TIMEOUT_RELATIVE,
			(FUNC_PTR)ack_timout_cb,
			(void *)trx_id);
	Assert(status == MAC_SUCCESS);

	if (status != MAC_SUCCESS) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
		trx_state[trx_id] = RF_TRXOFF;
		tx_done_handling(trx_id, status);
	} else {
		/* Configure frame filter to receive only ACK frames */
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_reg_write( reg_offset + RG_BBC0_AFFTM, ACK_FRAME_TYPE_ONLY);
		/* Sync with Trx state; due to Tx2Rx the transceiver is
		 *alreadyswitches automatically to Rx */
		trx_state[trx_id] = RF_RX;
	}
}

/**
 * @brief Callback function for ACK timeout
 *
 * This function is called if the ACK timeout timer fires.
 *
 * @param parameter Pointer to trx_id
 */
void ack_timout_cb(void *cb_timer_element)
{
	/* Immediately store trx id from callback. */
	trx_id_t trx_id = (trx_id_t)cb_timer_element;
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	switch_to_txprep(trx_id);

	/* Configure frame filter to receive all allowed frame types */
	/* Re-store frame filter to pass "normal" frames */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
#ifdef SUPPORT_FRAME_FILTER_CONFIGURATION
	trx_reg_write( reg_offset + RG_BBC0_AFFTM, tal_pib[trx_id].frame_types);
#else
	trx_reg_write( reg_offset + RG_BBC0_AFFTM, DEFAULT_FRAME_TYPES);
#endif

	tx_done_handling(trx_id, MAC_NO_ACK);
}

#endif /* #ifndef BASIC_MODE */

/*  EOF */
