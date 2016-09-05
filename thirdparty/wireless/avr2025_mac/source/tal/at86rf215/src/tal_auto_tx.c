/**
 * @file tal_auto_tx.c
 *
 * @brief This file handles the frame transmission within the TAL
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
#include <inttypes.h>
#include <stdio.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_internal.h"
#include "mac_build_config.h"
#ifdef ENABLE_RTB
#include "rtb.h"
#endif  /* ENABLE_RTB */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

static uint8_t number_of_tx_retries[NUM_TRX];
static csma_mode_t global_csma_mode[NUM_TRX];
static bool ack_requested[NUM_TRX];
/* Last frame length for IFS handling. */
uint16_t last_txframe_length[NUM_TRX];
bool frame_buf_filled[NUM_TRX];

/* === PROTOTYPES ========================================================== */

static void handle_ifs(trx_id_t trx_id);

#ifdef MEASURE_TIME_OF_FLIGHT
#   ifdef SUPPORT_LEGACY_OQPSK
static uint32_t calc_tof(trx_id_t trx_id);

#   endif  /* SUPPORT_LEGACY_OQPSK */
#endif  /* #ifdef MEASURE_TIME_OF_FLIGHT */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Requests to TAL to transmit frame
 *
 * This function is called by the MAC to deliver a frame to the TAL
 * to be transmitted by the transceiver.
 *
 * @param trx_id Transceiver identifier
 * @param tx_frame Pointer to the frame_info_t structure updated by the MAC
 * layer
 * @param csma_mode Indicates mode of csma-ca to be performed for this frame
 * @param perform_frame_retry Indicates whether to retries are to be performed
 * for
 *                            this frame
 *
 * @return
 *      - @ref MAC_SUCCESS  if the TAL has accepted the data from NHLE for frame
 *                 transmission
 *      - @ref TAL_BUSY if the TAL is busy servicing the previous MAC request
 *      - @ref TAL_TRX_ASLEEP if the device is in sleep mode
 *      - @ref MAC_INVALID_PARAMETER is a provided parameter is invalid
 * @ingroup apiTalApi
 */
retval_t tal_tx_frame(trx_id_t trx_id, frame_info_t *tx_frame,
		csma_mode_t csma_mode, bool perform_frame_retry)
{
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	if (tal_state[trx_id] == TAL_SLEEP) {
		return TAL_TRX_ASLEEP;
	}

	if (tal_state[trx_id] != TAL_IDLE) {
		return TAL_BUSY;
	}

	if ((csma_mode == CSMA_SLOTTED) || (tx_frame == NULL)) {
		return MAC_INVALID_PARAMETER;
	}

	/*
	 * In case the frame is too large, return immediately indicating
	 * invalid status.
	 */
	if ((tx_frame->len_no_crc + tal_pib[trx_id].FCSLen) >
			tal_pib[trx_id].MaxPHYPacketSize) {
		return MAC_INVALID_PARAMETER;
	}

	/*
	 * Store the pointer to the provided frame structure.
	 * This is needed for the callback function.
	 */
	mac_frame_ptr[trx_id] = tx_frame;

	/* Set pointer to actual MPDU to be downloaded to the transceiver. */
	tal_frame_to_tx[trx_id] = tx_frame->mpdu;

	if (perform_frame_retry) {
		number_of_tx_retries[trx_id] = 0;
	} else {
		/* No tx retry -> set current retry value to max value .*/
		number_of_tx_retries[trx_id] = tal_pib[trx_id].MaxFrameRetries;
	}

	tal_state[trx_id] = TAL_TX;
	frame_buf_filled[trx_id] = false;
	global_csma_mode[trx_id] = csma_mode;
	tal_pib[trx_id].NumRxFramesDuringBackoff = 0;

	/* Check if ACK is requested. */
	if (*mac_frame_ptr[trx_id]->mpdu & FCF_ACK_REQUEST) {
		ack_requested[trx_id] = true;
	} else {
		ack_requested[trx_id] = false;
	}

#ifdef SUPPORT_MODE_SWITCH
	if (tal_pib[trx_id].ModeSwitchEnabled) {
		save_current_phy(trx_id);
		set_csm(trx_id);
	}

#endif

	if (csma_mode == CSMA_UNSLOTTED) {
		csma_start(trx_id);
	} else { /* csma_mode == NO_CSMA_NO_IFS or NO_CSMA_WITH_IFS */
		if (csma_mode == NO_CSMA_WITH_IFS) {
			handle_ifs(trx_id);
		}

#ifdef SUPPORT_MODE_SWITCH
		if (tal_pib[trx_id].ModeSwitchEnabled) {
			tx_ms_ppdu(trx_id);
		} else
#endif
		{
			transmit_frame(trx_id, NO_CCA);
		}
	}

	return MAC_SUCCESS;
}

/**
 * @brief Transmits frame
 *
 * @param trx_id Transceiver identifier
 * @param cca If WITH_CCA CCA is enforced, else no CCA is applied before
 *transmission.
 */
void transmit_frame(trx_id_t trx_id, cca_use_t cca)
{
	cancel_any_reception(trx_id);

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	/* Configure auto modes */
	uint8_t amcs = 0;
	if (cca == WITH_CCA) {
		amcs |= AMCS_CCATX_MASK;
	}

	if (ack_requested[trx_id]) {
		/* Enable Tx2RX */
		amcs |= AMCS_TX2RX_MASK;
		/* Configure frame filter to receive only ACK frames */
		trx_reg_write(  reg_offset + RG_BBC0_AFFTM,
				ACK_FRAME_TYPE_ONLY);
	}

	/* Other auto mode settings can be set to 0 */
	trx_reg_write( reg_offset + RG_BBC0_AMCS, amcs);

	if (frame_buf_filled[trx_id] == false) {
		/* fill length field */
		uint16_t len = mac_frame_ptr[trx_id]->len_no_crc +
				tal_pib[trx_id].FCSLen;
		trx_write(reg_offset + RG_BBC0_TXFLL, (uint8_t *)&len, 2);

		/* Store tx frame length to handle IFS next time */
		last_txframe_length[trx_id] = mac_frame_ptr[trx_id]->len_no_crc;

		/* Disable automatic FCS appending */
		trx_bit_write(reg_offset + SR_BBC0_PC_TXAFCS, 0);
	}

	if (cca == WITH_CCA) {
		/* Trigger CCA measurement */
		/* Disable BB; it will enabled for transmission automatically
		 *again */
		trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 0);
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_RX);
		trx_state[trx_id] = RF_RX;
		pal_timer_delay(tal_pib[trx_id].agc_settle_dur); /* allow
		                                                  * filters to
		                                                  * settle */

		/* Start single ED measurement; use reg_write - it's the only
		 *sub-register */
		trx_reg_write( reg_offset + RG_RF09_EDC, RF_EDSINGLE);
		tx_state[trx_id] = TX_CCATX;
	} else { /* no CCA */
		/*("switch to Tx")*/
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TX);
		trx_state[trx_id] = RF_TX;
		tx_state[trx_id] = TX_TX;
	}

#if (defined ENABLE_TSTAMP) || (defined MEASURE_ON_AIR_DURATION)
	pal_get_current_time(&fs_tstamp[trx_id]);
#endif

	/* Download frame content during CCA or during preamble transmission */
	if (frame_buf_filled[trx_id] == false) {
		/* fill frame buffer; do not provide FCS values */
		uint16_t tx_frm_buf_offset = BB_TX_FRM_BUF_OFFSET * trx_id;
		trx_write( tx_frm_buf_offset + RG_BBC0_FBTXS,
				(uint8_t *)mac_frame_ptr[trx_id]->mpdu,
				mac_frame_ptr[trx_id]->len_no_crc);

		/* Check if under-run has occurred */
		bool underrun = trx_bit_read(reg_offset + SR_BBC0_PS_TXUR);
		if (underrun) {
			/* Abort ongoing transmission */
			trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
			trx_state[trx_id] = RF_TRXOFF;

			/* Enable BB again and TXAFCS */
			uint8_t pc = trx_reg_read(reg_offset + RG_BBC0_PC);
			pc |= PC_TXAFCS_MASK | PC_BBEN_MASK;
			trx_reg_write(reg_offset + RG_BBC0_PC, pc);

			TAL_BB_IRQ_CLR(trx_id, BB_IRQ_TXFE);
			TAL_RF_IRQ_CLR(trx_id,
					RF_IRQ_TRXERR | RF_IRQ_TRXRDY |
					RF_IRQ_EDC);
			tx_done_handling(trx_id, FAILURE);
		} else {
			/* Enable automatic FCS appending for remaining frame or
			 *later ACK transmission */
#ifdef RF215v1
			uint16_t reg_pc = reg_offset + RG_BBC0_PC;
			uint8_t pc = trx_reg_read( reg_pc);
			pc |= PC_TXAFCS_MASK;
			trx_reg_write( reg_pc, pc);
			if (cca == WITH_CCA) {
				rf_cmd_state_t state
					= (rf_cmd_state_t)trx_reg_read(
						reg_offset + RG_RF09_STATE);
				if (state != RF_RX) {
					pc |= PC_BBEN_MASK;
					trx_reg_write(reg_pc, pc);
				}
			}

#else
			trx_bit_write( reg_offset + SR_BBC0_PC_TXAFCS, 1);
#endif
			frame_buf_filled[trx_id] = true;
		}
	}
}

/**
 * @brief Handles interrupts issued due to end of transmission
 *
 * @param trx_id Transceiver identifier
 */
void handle_tx_end_irq(trx_id_t trx_id)
{
	/* ACK transmission completed */
	if (ack_transmitting[trx_id]) {
		ack_transmission_done(trx_id);
		return;
	}

	switch (tx_state[trx_id]) {
	case TX_CCATX:
	{
		/* Check channel */
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		uint8_t ccaed = trx_bit_read( reg_offset + SR_BBC0_AMCS_CCAED);
		if (ccaed == BB_CH_CLEAR) {
			/* Channel idle and frame has been sent */
#ifdef MEASURE_ON_AIR_DURATION
			tal_pib[trx_id].OnAirDuration
				+= pal_sub_time_us(rxe_txe_tstamp[trx_id],
					fs_tstamp[trx_id]);
			tal_pib[trx_id].OnAirDuration
				-= tal_pib[trx_id].CCADuration_us;
			/* @ToDo: Minus processing delay */
#endif
			/* Start ACK wait timer - see below */
		} else { /* channel busy */
			/* Switch BB on again */
			trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 1);
			switch_to_txprep(trx_id);
			csma_continue(trx_id);
			return;
		}
	}
	break;

	case TX_TX:
#ifdef MEASURE_ON_AIR_DURATION
		tal_pib[trx_id].OnAirDuration
			+= pal_sub_time_us(rxe_txe_tstamp[trx_id],
				fs_tstamp[trx_id]);
		/* @ToDo: Minus processing delay */
#endif
		/* Start ACK wait timer - see below */
		break;

#ifdef SUPPORT_MODE_SWITCH
	case TX_MS_PPDU:
		switch_to_txprep(trx_id);
#ifdef MEASURE_ON_AIR_DURATION
		tal_pib[trx_id].OnAirDuration
			+= pal_sub_time_us(rxe_txe_tstamp[trx_id],
				fs_tstamp[trx_id]);
		/* @ToDo: Minus processing delay */
#endif
		prepare_actual_transmission(trx_id);
		return;

	case TX_MS_NEW_MODE_TRANSMITTING:
		switch_to_txprep(trx_id);
#ifdef MEASURE_ON_AIR_DURATION
		tal_pib[trx_id].OnAirDuration
			+= pal_sub_time_us(rxe_txe_tstamp[trx_id],
				fs_tstamp[trx_id]);
		/* @ToDo: Minus processing delay */
#endif
		set_csm(trx_id);
		break;
#endif

	default:
		Assert("Unexpected tx_state" == 0);
		break;
	}

	if (ack_requested[trx_id]) {
#ifdef SUPPORT_MODE_SWITCH
		if (tal_pib[trx_id].ModeSwitchEnabled) {
			switch_to_rx(trx_id);
		} else
#endif
		{
			/* Trx is switched to RX automatically due to TX2RX
			 *setting */
			trx_state[trx_id] = RF_RX;
		}

		start_ack_wait_timer(trx_id);
	} else { /* No ACK requested */
		trx_state[trx_id] = RF_TXPREP;
		tx_done_handling(trx_id, MAC_SUCCESS);
	}
}

/**
 * @brief Implements the handling of the transmission end.
 *
 * This function handles the callback for the transmission end.
 *
 * @param trx_id Transceiver identifier
 * @param status Status of the transmission: MAC_SUCCESS, MAC_NO_ACK,
 *               CHANNEL_ACCESS_FAILURE, FAILURE
 */
void tx_done_handling(trx_id_t trx_id, retval_t status)
{
	if (status == MAC_NO_ACK) {
		if (number_of_tx_retries[trx_id] <
				tal_pib[trx_id].MaxFrameRetries) {
			number_of_tx_retries[trx_id]++;
			if (global_csma_mode[trx_id] == CSMA_UNSLOTTED) {
				csma_start(trx_id);
			} else {
				if (global_csma_mode[trx_id] ==
						NO_CSMA_WITH_IFS) {
					handle_ifs(trx_id);
				}

#ifdef SUPPORT_MODE_SWITCH
				if (tal_pib[trx_id].ModeSwitchEnabled) {
					set_csm(trx_id);
					tx_ms_ppdu(trx_id);
				} else
#endif
				{
					transmit_frame(trx_id, NO_CCA);
				}
			}

			return; /* next tx attempt and no tx done cb */
		}
	} else { /* status == CHANNEL_ACCESS_FAILURE or FAILURE or
	          * TAL_FRAME_PENDING */
		/* do nothing */
	}

#ifdef SUPPORT_MODE_SWITCH
	if (tal_pib[trx_id].ModeSwitchEnabled) {
		restore_previous_phy(trx_id);
	}

#endif

#ifdef ENABLE_TSTAMP
	mac_frame_ptr[trx_id]->time_stamp = fs_tstamp[trx_id];
#endif

#ifdef MEASURE_TIME_OF_FLIGHT
#   ifdef SUPPORT_LEGACY_OQPSK
	/* ToF only supported for Legacy-OQPSK */

	/* Initialize ToF value (in case we run in any of the non-if cases). */
	mac_frame_ptr[trx_id]->TimeOfFlight = INVALID_TOF_VALUE;

	if (ack_requested[trx_id]) {
		if (status == MAC_SUCCESS) {
			if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
				mac_frame_ptr[trx_id]->TimeOfFlight = calc_tof(
						trx_id);
			}
		}
	}

#   else    /* SUPPORT_LEGACY_OQPSK */
	/* ToF not supported */
	mac_frame_ptr[trx_id]->TimeOfFlight = INVALID_TOF_VALUE;
#   endif  /* SUPPORT_LEGACY_OQPSK */
#endif  /* #ifdef MEASURE_TIME_OF_FLIGHT */

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	/* Enable AACK again and disable CCA / TX procedure */
	trx_reg_write((reg_offset + RG_BBC0_AMCS), AMCS_AACK_MASK);

	/* Set trx state for leaving TX transaction */
	if (trx_default_state[trx_id] == RF_RX) {
		if (trx_state[trx_id] != RF_RX) {
			switch_to_rx(trx_id);
		}
	} else {
		trx_reg_write((reg_offset + RG_RF09_CMD), RF_TRXOFF);
		trx_state[trx_id] = RF_TRXOFF;
	}

#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
	start_rpc(trx_id);
#endif

	tx_state[trx_id] = TX_IDLE;
	tal_state[trx_id] = TAL_IDLE;

#ifdef ENABLE_RTB
	rtb_tx_frame_done_cb(trx_id, status, mac_frame_ptr[trx_id]);
#else
	/* Regular handling without RTB */
	tal_tx_frame_done_cb(trx_id, status, mac_frame_ptr[trx_id]);
#endif
} /* tx_done_handling() */

/**
 * @brief Handles interframe spacing (IFS)
 *
 * @param trx_id Transceiver identifier
 */
static void handle_ifs(trx_id_t trx_id)
{
	uint32_t now;
	uint32_t time_diff;

	pal_get_current_time(&now);
	time_diff = now - rxe_txe_tstamp[trx_id];
	if (last_txframe_length[trx_id] > aMaxSIFSFrameSize) {
		/* Long IFS */
		uint32_t required_spacing = macMinLIFSPeriod_def *
				tal_pib[trx_id].SymbolDuration_us;
		if (time_diff < required_spacing) {
			uint32_t delay = required_spacing - time_diff;
			pal_timer_delay(delay);
		}
	} else {
		/* Short IFS */
		uint32_t required_spacing = macMinSIFSPeriod_def *
				tal_pib[trx_id].SymbolDuration_us;
		if (time_diff < required_spacing) {
			uint32_t delay = required_spacing - time_diff;
			pal_timer_delay(delay);
		}
	}
}

/**
 * @brief Confinues with a deferred transmission
 *
 * @param trx_id Transceiver identifier
 */
void continue_deferred_transmission(trx_id_t trx_id)
{
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	if (tal_pib[trx_id].NumRxFramesDuringBackoff >
			tal_pib[trx_id].MaxNumRxFramesDuringBackoff) {
		tx_done_handling(trx_id, MAC_CHANNEL_ACCESS_FAILURE);
	} else {
		if (global_csma_mode[trx_id] == CSMA_UNSLOTTED) {
			csma_start(trx_id);
		} else {
			transmit_frame(trx_id, NO_CCA);
		}
	}
}

#ifdef MEASURE_TIME_OF_FLIGHT
#   ifdef SUPPORT_LEGACY_OQPSK

/**
 * @brief Calculates ToF value.
 *
 * @param trx_id Transceiver identifier
 */
static uint32_t calc_tof(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	uint32_t tof_counter_value;
	trx_read((reg_offset + RG_BBC0_CNT0),
			(uint8_t *)&(tof_counter_value), 4);

#       ifdef TIME_OF_FLIGHT_COUNTER_MODE
	/* Simply return the ToF counter value. */
	return tof_counter_value;

#       else   /* Regular mode */
	uint32_t tof_time_ns = tof_counter_value * 1000 / 32;
	uint32_t frame_duation_ns
		= (mac_frame_ptr[trx_id]->len_no_crc + tal_pib[trx_id].FCSLen) *
			1000 * tal_get_symbol_duration_us(trx_id) * 2;

	uint32_t offset_and_frame_ns = frame_duation_ns;
	if (trx_id == RF09) {
		offset_and_frame_ns += TOF_PROC_DELAY_OFFSET_SUB_GHZ_NS;
	} else {
		offset_and_frame_ns += TOF_PROC_DELAY_OFFSET_2_4_GHZ_NS;
	}

	/*
	 * Calculate final ToF value in ns.
	 * Note: Final time of flight must not be negative,
	 * which would result in very large unsigned ToF value.
	 */
	if (tof_time_ns >= offset_and_frame_ns) {
		tof_time_ns -= offset_and_frame_ns;
	} else {
		tof_time_ns = 0;
	}

	return tof_time_ns;

#       endif   /* TIME_OF_FLIGHT_COUNTER_MODE */
}

#   endif  /* SUPPORT_LEGACY_OQPSK */
#endif  /* #ifdef MEASURE_TIME_OF_FLIGHT */

#endif /* #ifndef BASIC_MODE */

/* EOF */
