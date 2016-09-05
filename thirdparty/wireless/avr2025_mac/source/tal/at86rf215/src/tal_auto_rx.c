/**
 * @file tal_auto_rx.c
 *
 * @brief This file implements function to handle received frames.
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
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_pib.h"
#include "tal_internal.h"

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

frame_info_t *rx_frm_info[NUM_TRX];

/* === PROTOTYPES ========================================================== */

static void handle_incoming_frame(trx_id_t trx_id);
static bool upload_frame(trx_id_t trx_id);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Handle received frame interrupt
 *
 * This function handles transceiver interrupts for received frames.
 *
 * @param trx_id Transceiver identifier
 */
void handle_rx_end_irq(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
	stop_rpc(trx_id);
#endif

#ifdef SUPPORT_FSK
	/* Check if incoming FSK frame uses the expected CRC length */
	if (tal_pib[trx_id].phy.modulation == FSK) {
		if (trx_bit_read(  reg_offset + SR_BBC0_FSKPHRRX_FCST) !=
				(uint8_t)tal_pib[trx_id].FCSType) {
			/* Received FCS value is not equal to the required value
			 *-> cancel ACK transmission */
			if (trx_bit_read( reg_offset + SR_BBC0_AMCS_AACKFT)) {
				trx_bit_write(reg_offset + SR_BBC0_AMCS_AACK,
						0);
				trx_bit_write(reg_offset + SR_BBC0_AMCS_AACK,
						1);
			}

			/* Continue receiving */
			trx_reg_write( reg_offset + RG_RF09_CMD, RF_RX);
			trx_state[trx_id] = RF_RX;
			start_rpc(trx_id);
			return;
		}
	}

#endif

	if (upload_frame(trx_id) == false) {
		switch_to_rx(trx_id); /* buffer shortage will handled by
		                       * tal_task() */
		return;
	}

	if (tx_state[trx_id] == TX_BACKOFF) {
		/* Stop backoff timer */
		stop_tal_timer(trx_id);
		tx_state[trx_id] = TX_DEFER;
		tal_pib[trx_id].NumRxFramesDuringBackoff++;
	}

#ifdef SUPPORT_MODE_SWITCH
	if (tal_pib[trx_id].ModeSwitchEnabled) {
		if (tal_pib[trx_id].phy.modulation == FSK) {
			uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
			if (trx_bit_read( reg_offset + SR_BBC0_FSKPHRRX_MS) ==
					0x01) {
				handle_rx_ms_packet(trx_id);
				return;
			}
		}

		if (tal_state[trx_id] == TAL_NEW_MODE_RECEIVING) {
			/* Restore previous PHY, i.e. CSM */
			/* Stop timer waiting for incoming frame at new mode */
			stop_tal_timer(trx_id);
			set_csm(trx_id);
			tal_state[trx_id] = TAL_IDLE;
		}
	}

#endif

#ifdef PROMISCUOUS_MODE
	if (tal_pib[trx_id].PromiscuousMode) {
		complete_rx_transaction(trx_id);
		switch_to_rx(trx_id);
		return;
	}

#endif /* #ifdef PROMISCUOUS_MODE */

	handle_incoming_frame(trx_id);
}

/**
 * @brief Handles incoming frame from transceiver
 *
 * @param trx_id Transceiver identifier
 */
static void handle_incoming_frame(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	if (is_frame_an_ack(trx_id)) {
		trx_state[trx_id] = RF_TXPREP;
		if (tx_state[trx_id] == TX_WAITING_FOR_ACK) {
			if (is_ack_valid(trx_id)) {
				/* Stop ACK timeout timer */
				stop_tal_timer(trx_id);
				/* Re-store frame filter to pass "normal" frames
				 **/
				/* Configure frame filter to receive all allowed
				 *frame types */
#ifdef SUPPORT_FRAME_FILTER_CONFIGURATION
				trx_reg_write( reg_offset + RG_BBC0_AFFTM,
						tal_pib[trx_id].frame_types);
#else
				trx_reg_write( reg_offset + RG_BBC0_AFFTM,
						DEFAULT_FRAME_TYPES);
#endif
				retval_t status;
				if (rx_frm_info[trx_id]->mpdu[PL_POS_FCF_1] &
						FCF_FRAME_PENDING) {
					status = TAL_FRAME_PENDING;
				} else {
					status = MAC_SUCCESS;
				}

				tx_done_handling(trx_id, status);
			} else {
				/* Continue waiting for incoming ACK */
				switch_to_rx(trx_id);
			}
		} else {
			/* No interest in ACKs */
			switch_to_rx(trx_id);
		}

		return; /* no further processing of ACK frames */
	}

	/* Check if ACK transmission is done by transceiver */
	ack_transmitting[trx_id] = (bool)trx_bit_read(
			reg_offset + SR_BBC0_AMCS_AACKFT);

#if (defined RF215v1) && (!defined BASIC_MODE)
	/* Workaround for errata reference #4830 */
	/* Check if workaround is applicable */
	if (ack_transmitting[trx_id]) {
		uint8_t fcf0 = rx_frm_info[trx_id]->mpdu[0];
		if ((fcf0 & FCF_ACK_REQUEST) == 0x00) {
			/* Unwanted ACK transmission has already by canceled
			 *within ISR context */
			ack_transmitting[trx_id] = false;
		}
	}

#endif
	if (ack_transmitting[trx_id]) {
		trx_state[trx_id] = RF_TX; /* Sync with trx state; automatic
		                            * state switch */
#ifdef SUPPORT_FSK
		if (tal_pib[trx_id].RPCEnabled &&
				tal_pib[trx_id].phy.modulation == FSK) {
			/* Configure preamble length for transmission */
			trx_reg_write( reg_offset + RG_BBC0_FSKPLL,
					(uint8_t)(tal_pib[trx_id].
					FSKPreambleLength & 0xFF));
			trx_bit_write( reg_offset + SR_BBC0_FSKC1_FSKPLH,
					(uint8_t)(tal_pib[trx_id].
					FSKPreambleLength >> 8));
		}

#endif
	} else {
		trx_state[trx_id] = RF_TXPREP;
		complete_rx_transaction(trx_id);
		switch_to_rx(trx_id);
	}
}

/**
 * @brief Parses received frame and create the frame_info_t structure
 *
 * This function parses the received frame and creates the frame_info_t
 * structure to be sent to the MAC as a parameter of tal_rx_frame_cb().
 *
 * @param trx_id Transceiver identifier
 */
static bool upload_frame(trx_id_t trx_id)
{
	if (tal_rx_buffer[trx_id] == NULL) {
		Assert("no tal_rx_buffer available" == 0);
		return false;
	}

	rx_frm_info[trx_id]
		= (frame_info_t *)BMM_BUFFER_POINTER(
			tal_rx_buffer[trx_id]);

	/* Get Rx frame length */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	uint16_t phy_frame_len;
	trx_read((reg_offset + RG_BBC0_RXFLL), (uint8_t *)&phy_frame_len, 2);
	rx_frm_info[trx_id]->len_no_crc = phy_frame_len -
			tal_pib[trx_id].FCSLen;

	/* Update payload pointer to store received frame. */
	rx_frm_info[trx_id]->mpdu = (uint8_t *)rx_frm_info[trx_id] +
			LARGE_BUFFER_SIZE -
			phy_frame_len - ED_VAL_LEN - LQI_LEN;

#ifdef ENABLE_TSTAMP
	/* Store the timestamp. */
	rx_frm_info[trx_id]->time_stamp = fs_tstamp[trx_id];
#endif

	/* Upload received frame to buffer */
#ifdef UPLOAD_CRC
	uint16_t len = rx_frm_info[trx_id]->len_no_crc + tal_pib[trx_id].FCSLen;
#else
	uint16_t len = rx_frm_info[trx_id]->len_no_crc;
#endif
	uint16_t rx_frm_buf_offset = BB_RX_FRM_BUF_OFFSET * trx_id;
	trx_read( rx_frm_buf_offset + RG_BBC0_FBRXS, rx_frm_info[trx_id]->mpdu,
			len);

	return true;
}

/**
 * @brief Completes Rx transaction
 *
 * @param trx_id Transceiver identifier
 */
void complete_rx_transaction(trx_id_t trx_id)
{
	/* Get energy of received frame */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	uint8_t ed = trx_reg_read( reg_offset + RG_RF09_EDV);
	uint16_t ed_pos = rx_frm_info[trx_id]->len_no_crc + 1 +
			tal_pib[trx_id].FCSLen;
	rx_frm_info[trx_id]->mpdu[ed_pos] = ed; /* PSDU, LQI, ED */

	/* Append received frame to incoming_frame_queue and get new rx buffer.
	 **/
	qmm_queue_append(&tal_incoming_frame_queue[trx_id],
			tal_rx_buffer[trx_id]);
	/* The previous buffer is eaten up and a new buffer is not assigned yet.
	 **/
	tal_rx_buffer[trx_id] = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
	/* Fill trx_id in new buffer */
	if (tal_rx_buffer[trx_id] != NULL) {
		frame_info_t *frm_info = (frame_info_t *)BMM_BUFFER_POINTER(
				tal_rx_buffer[trx_id]);
		frm_info->trx_id = trx_id;
	}
}

/**
 * @brief Parses received frame and create the frame_info_t structure
 *
 * This function parses the received frame and creates the frame_info_t
 * structure to be sent to the MAC as a parameter of tal_rx_frame_cb().
 *
 * @param trx_id Transceiver identifier
 * @param buf_ptr Pointer to the buffer containing the received frame
 */
void process_incoming_frame(trx_id_t trx_id, buffer_t *buf_ptr)
{
	frame_info_t *receive_frame
		= (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);
	receive_frame->buffer_header = buf_ptr;

	/* Scale ED value to a LQI value: 0x00 - 0xFF */
	uint16_t lqi_pos = receive_frame->len_no_crc + tal_pib[trx_id].FCSLen;
	receive_frame->mpdu[lqi_pos]
		= scale_ed_value((int8_t)receive_frame->mpdu[lqi_pos + 1]);

#ifdef ENABLE_RTB
	/* The callback function implemented by RTB is invoked. */
	rtb_rx_frame_cb(trx_id, receive_frame);
#else
	/* The callback function implemented by MAC is invoked. */
	tal_rx_frame_cb(trx_id, receive_frame);
#endif
} /* process_incoming_frame() */

#endif /* #ifndef BASIC_MODE */

/*  EOF */
