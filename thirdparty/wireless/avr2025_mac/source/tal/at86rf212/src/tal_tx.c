/**
 * @file tal_tx.c
 *
 * @brief This file handles the frame transmission within the TAL.
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
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#ifdef SW_CONTROLLED_CSMA
#include <stdlib.h>
#endif
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "tal_irq_handler.h"
#include "tal_constants.h"
#include "tal_tx.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_rx.h"
#include "tal_internal.h"
#include "at86rf212.h"
#ifdef BEACON_SUPPORT
#include "tal_slotted_csma.h"
#endif  /* BEACON_SUPPORT */
#include "mac_build_config.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

static trx_trac_status_t trx_trac_status;
#ifdef SW_CONTROLLED_CSMA
static uint8_t number_of_tx_retries;
static uint8_t NB;
static uint8_t BE;
#endif

/* === PROTOTYPES ========================================================== */

#ifdef SW_CONTROLLED_CSMA
static inline void sw_controlled_csma(csma_mode_t csma_mode, bool retransmit);
static void csma_start(void);
static void cca_start(void *parameter);
static void cca_done_irq_handler(void);
static void tx_frame(void);
static void start_backoff(void);

#endif

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Requests to TAL to transmit frame
 *
 * This function is called by the MAC to deliver a frame to the TAL
 * to be transmitted by the transceiver.
 *
 * \param tx_frame Pointer to the frame_info_t structure updated by the MAC
 * layer
 * \param csma_mode Indicates mode of csma-ca to be performed for this frame
 * \param perform_frame_retry Indicates whether to retries are to be performed
 * for
 *                            this frame
 *
 * \return MAC_SUCCESS  if the TAL has accepted the data from the MAC for frame
 *                 transmission
 *         TAL_BUSY if the TAL is busy servicing the previous MAC request
 */
retval_t tal_tx_frame(frame_info_t *tx_frame, csma_mode_t csma_mode,
		bool perform_frame_retry)
{
	if (tal_state != TAL_IDLE) {
		return TAL_BUSY;
	}

	/*
	 * Store the pointer to the provided frame structure.
	 * This is needed for the callback function.
	 */
	mac_frame_ptr = tx_frame;

	/* Set pointer to actual mpdu to be downloaded to the transceiver. */
	tal_frame_to_tx = tx_frame->mpdu;
	last_frame_length = tal_frame_to_tx[0] - 1;

	/*
	 * In case the frame is too large, return immediately indicating
	 * invalid status.
	 */
	if (tal_frame_to_tx == NULL) {
		return MAC_INVALID_PARAMETER;
	}

#ifdef BEACON_SUPPORT
	/* Check if beacon mode is used */
	if (csma_mode == CSMA_SLOTTED) {
		if (!slotted_csma_start(perform_frame_retry)) {
			return MAC_CHANNEL_ACCESS_FAILURE;
		}
	} else {
#if (MAC_INDIRECT_DATA_FFD == 1)

		/*
		 * Check if frame is using indirect transmission, but do not use
		 * the
		 * indirect_in_transit flag. This flag is not set for null data
		 * frames.
		 */
		if ((tal_pib.BeaconOrder < NON_BEACON_NWK) &&
				(csma_mode == NO_CSMA_WITH_IFS) &&
				(perform_frame_retry == false)) {
			/*
			 * Check if indirect transmission can be completed
			 * before the next
			 * beacon transmission.
			 */
			uint32_t time_between_beacons_sym;
			uint32_t next_beacon_time_sym;
			uint32_t now_time_sym;
			uint32_t duration_before_beacon_sym;

			/* Calculate the entire transaction duration. Re-use
			 * function of slotted CSMA.
			 * The additional two backoff periods used for CCA are
			 * kept as a guard time.
			 */
			calculate_transaction_duration();

			/* Calculate the duration until the next beacon needs to
			 * be transmitted. */
			time_between_beacons_sym = TAL_GET_BEACON_INTERVAL_TIME(
					tal_pib.BeaconOrder);
			next_beacon_time_sym = tal_add_time_symbols(
					tal_pib.BeaconTxTime,
					time_between_beacons_sym);
			pal_get_current_time(&now_time_sym);
			now_time_sym = TAL_CONVERT_US_TO_SYMBOLS(now_time_sym);
			duration_before_beacon_sym = tal_sub_time_symbols(
					next_beacon_time_sym, now_time_sym);

			/* Check if transaction can be completed before next
			 * beacon transmission. */
			if ((now_time_sym >= next_beacon_time_sym) ||
					((transaction_duration_periods *
					aUnitBackoffPeriod) >
					duration_before_beacon_sym)) {
				/*
				 * Transaction will not be completed before next
				 * beacon transmission.
				 * Therefore the transmission is not executed.
				 */
				return MAC_CHANNEL_ACCESS_FAILURE;
			}
		}
#endif  /* #if (MAC_INDIRECT_DATA_FFD == 1) */
#ifdef SW_CONTROLLED_CSMA
		sw_controlled_csma(csma_mode, perform_frame_retry);
#else
		send_frame(csma_mode, perform_frame_retry);
#endif
	}

#else   /* No BEACON_SUPPORT */
#ifdef SW_CONTROLLED_CSMA
	sw_controlled_csma(csma_mode, perform_frame_retry);
#else
	send_frame(csma_mode, perform_frame_retry);
#endif
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

	return MAC_SUCCESS;
}

/*
 * \brief Implements the handling of the transmission end.
 *
 * This function handles the callback for the transmission end.
 */
void tx_done_handling(void)
{
	tal_state = TAL_IDLE;

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)

	/*
	 * The entire timestamp calculation is only required for beaconing
	 * networks
	 * or if timestamping is explicitly enabled.
	 */

	/* Calculate negative offset of timestamp */
	uint16_t offset;

	/* Calculate the tx time */
	offset
		= TAL_CONVERT_SYMBOLS_TO_US(
			(PHY_OVERHEAD + LENGTH_FIELD_LEN) * SYMBOLS_PER_OCTET)
			+ TAL_PSDU_US_PER_OCTET(*tal_frame_to_tx + FCS_LEN)
			+ TRX_IRQ_DELAY_US;
	if (mac_frame_ptr->mpdu[PL_POS_FCF_1] & FCF_ACK_REQUEST) {
		/* Tx timestamp needs to be reduced by ACK duration etc. */
		offset
			+= TAL_CONVERT_SYMBOLS_TO_US(
				(PHY_OVERHEAD +
				LENGTH_FIELD_LEN) * SYMBOLS_PER_OCTET) +
				TAL_PSDU_US_PER_OCTET(ACK_PAYLOAD_LEN +
				FCS_LEN);

#ifdef HIGH_DATA_RATE_SUPPORT
		if (tal_pib.CurrentPage == 0) {
			offset += TAL_CONVERT_SYMBOLS_TO_US(aTurnaroundTime);
		} else {
			offset += 32;
		}

#else
		offset += TAL_CONVERT_SYMBOLS_TO_US(aTurnaroundTime);
#endif  /* #ifdef HIGH_DATA_RATE_SUPPORT */
	}
	mac_frame_ptr->time_stamp -= offset;
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

	retval_t status;

	switch (trx_trac_status) {
	case TRAC_SUCCESS:
		status = MAC_SUCCESS;
		break;

	case TRAC_SUCCESS_DATA_PENDING:
		status = TAL_FRAME_PENDING;
		break;

	case TRAC_CHANNEL_ACCESS_FAILURE:
		status = MAC_CHANNEL_ACCESS_FAILURE;
		break;

	case TRAC_NO_ACK:
#ifdef SW_CONTROLLED_CSMA
		if (number_of_tx_retries < tal_pib.MaxFrameRetries) {
			number_of_tx_retries++;
			csma_start();
			return; /* next tx attempt and no tx done cb */
		} else {
			/* Number of retries have been reached. */
			status = MAC_NO_ACK;
		}

#else
		status = MAC_NO_ACK;
#endif
		break;

	case TRAC_INVALID:
		status = FAILURE;
		break;

	default:
		Assert("Unexpected tal_tx_state" == 0);
		status = FAILURE;
		break;
	}

	tal_tx_frame_done_cb(status, mac_frame_ptr);
} /* tx_done_handling() */

#if (!defined SW_CONTROLLED_CSMA) || (defined BEACON_SUPPORT)

/*
 * \brief Sends frame using trx features to handle CSMA and re-transmissions
 *
 * \param use_csma Flag indicating if CSMA is requested
 * \param tx_retries Flag indicating if transmission retries are requested
 *                   by the MAC layer
 */
void send_frame(csma_mode_t csma_mode, bool tx_retries)
{
	tal_trx_status_t trx_status;

	/* Configure tx according to tx_retries */
	if (tx_retries) {
		trx_bit_write(SR_MAX_FRAME_RETRIES,
				tal_pib.MaxFrameRetries);
	} else {
		trx_bit_write(SR_MAX_FRAME_RETRIES, 0);
	}

	/* Configure tx according to csma usage */
	if ((csma_mode == NO_CSMA_NO_IFS) || (csma_mode == NO_CSMA_WITH_IFS)) {
		trx_bit_write(SR_MAX_CSMA_RETRIES, 7); /* immediate
		                                        * transmission */
	} else {
		trx_bit_write(SR_MAX_CSMA_RETRIES, tal_pib.MaxCSMABackoffs);
	}

	do {
		trx_status = set_trx_state(CMD_TX_ARET_ON);
	} while (trx_status != TX_ARET_ON);

	pal_trx_irq_dis();

	/* Handle interframe spacing */
	if (csma_mode == NO_CSMA_WITH_IFS) {
		if (last_frame_length > aMaxSIFSFrameSize) {
			pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(
					macMinLIFSPeriod_def)
					- TRX_IRQ_DELAY_US -
					PRE_TX_DURATION_US);
		} else {
			pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(
					macMinSIFSPeriod_def)
					- TRX_IRQ_DELAY_US -
					PRE_TX_DURATION_US);
		}
	}

	/* Toggle the SLP_TR pin triggering transmission. */
	TRX_SLP_TR_HIGH();
	PAL_WAIT_65_NS();
	TRX_SLP_TR_LOW();

	/*
	 * Send the frame to the transceiver.
	 * Note: The PhyHeader is the first byte of the frame to
	 * be sent to the transceiver and this contains the frame
	 * length.
	 * The actual length of the frame to be downloaded
	 * (parameter two of trx_frame_write)
	 * is
	 * 1 octet frame length octet
	 * + n octets frame (i.e. value of frame_tx[0])
	 * + 1 extra octet (see datasheet)
	 * - 2 octets FCS
	 */
	trx_frame_write(tal_frame_to_tx, tal_frame_to_tx[0]);

	tal_state = TAL_TX_AUTO;

#ifndef NON_BLOCKING_SPI
	pal_trx_irq_en();
#endif
#ifdef TX_OCTET_COUNTER
	tal_tx_octet_cnt += PHY_OVERHEAD + LENGTH_FIELD_LEN + frame_tx[0];
#endif
}

#endif

/*
 * \brief Handles interrupts issued due to end of transmission
 *
 * \param underrun_occured  true if under-run has occurred
 */
void handle_tx_end_irq(bool underrun_occured)
{
#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
	if (tal_beacon_transmission) {
		tal_beacon_transmission = false;

		if (tal_csma_state == BACKOFF_WAITING_FOR_BEACON) {
			/* Slotted CSMA has been waiting for a beacon, now it
			 * can continue. */
			tal_csma_state = CSMA_HANDLE_BEACON;
		}
	} else
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */
	{
#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)

		/*
		 * Store tx timestamp to frame_info_t structure.
		 * The timestamping is only required for beaconing networks
		 * or if timestamping is explicitly enabled.
		 */
		pal_trx_read_timestamp(&mac_frame_ptr->time_stamp);
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

		/* Read trac status before enabling RX_AACK_ON. */
		if (underrun_occured) {
			trx_trac_status = TRAC_INVALID;
		} else {
			trx_trac_status = (trx_trac_status_t)trx_bit_read(
					SR_TRAC_STATUS);
		}

#ifdef BEACON_SUPPORT
		if (tal_csma_state == FRAME_SENDING) { /* Transmission was
			                                * issued by slotted CSMA
			                                **/
			PIN_TX_END();
			tal_state = TAL_SLOTTED_CSMA;

			/* Map status message of transceiver to TAL constants.
			**/
			switch (trx_trac_status) {
			case TRAC_SUCCESS_DATA_PENDING:
				PIN_ACK_OK_START();
				tal_csma_state = TX_DONE_FRAME_PENDING;
				break;

			case TRAC_SUCCESS:
				PIN_ACK_OK_START();
				tal_csma_state = TX_DONE_SUCCESS;
				break;

			case TRAC_CHANNEL_ACCESS_FAILURE:
				PIN_NO_ACK_START();
				tal_csma_state = CSMA_ACCESS_FAILURE;
				break;

			case TRAC_NO_ACK:
				PIN_NO_ACK_START();
				tal_csma_state = TX_DONE_NO_ACK;
				break;

			case TRAC_INVALID: /* Handle this in the same way as
				            * default. */
			default:
				Assert("not handled trac status" == 0);
				tal_csma_state = CSMA_ACCESS_FAILURE;
				break;
			}
			PIN_ACK_OK_END();
			PIN_ACK_WAITING_END();
		} else
#endif  /* BEACON_SUPPORT */
		/* Trx has handled the entire transmission incl. CSMA */
		{
			tal_state = TAL_TX_DONE; /* Further handling is done by
			                          * tx_done_handling() */
		}
	}

	/*
	 * After transmission has finished, switch receiver on again.
	 * Check if receive buffer is available.
	 */
	if (NULL == tal_rx_buffer) {
		set_trx_state(CMD_PLL_ON);
		tal_rx_on_required = true;
	} else {
		set_trx_state(CMD_RX_AACK_ON);
	}
}

/*
 * \brief Beacon frame transmission
 */
#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
void tal_tx_beacon(frame_info_t *tx_frame)
{
	tal_trx_status_t trx_status;

	/* Set pointer to actual mpdu to be downloaded to the transceiver. */
	uint8_t *tal_beacon_to_tx = tx_frame->mpdu;

	/* Avoid that the beacon is transmitted while other transmision is
	 * on-going. */
	if (tal_state == TAL_TX_AUTO) {
		Assert(
				"trying to transmit beacon while ongoing transmission" ==
				0);
		return;
	}

	/* Send the pre-created beacon frame to the transceiver. */
	do {
		trx_status = set_trx_state(CMD_PLL_ON);
#if (_DEBUG_ > 1)
		if (trx_status != PLL_ON) {
			Assert("PLL_ON failed for beacon transmission" == 0);
		}
#endif
	} while (trx_status != PLL_ON);

	/* \TODO wait for talbeaconTxTime */

	pal_trx_irq_dis();

	/* Toggle the SLP_TR pin triggering transmission. */
	TRX_SLP_TR_HIGH();
	PAL_WAIT_65_NS();
	TRX_SLP_TR_LOW();

	/*
	 * Send the frame to the transceiver.
	 * Note: The PhyHeader is the first byte of the frame to
	 * be sent to the transceiver and this contains the frame
	 * length.
	 * The actual length of the frame to be downloaded
	 * (parameter two of trx_frame_write)
	 * is
	 * 1 octet frame length octet
	 * + n octets frame (i.e. value of frame_tx[0])
	 * + 1 extra octet (see datasheet)
	 * - 2 octets FCS
	 */
	trx_frame_write(tal_beacon_to_tx, tal_beacon_to_tx[0]);

	tal_beacon_transmission = true;

#ifndef NON_BLOCKING_SPI
	pal_trx_irq_en();
#endif
#ifdef TX_OCTET_COUNTER
	tal_tx_octet_cnt += PHY_OVERHEAD + LENGTH_FIELD_LEN +
			tal_beacon_to_tx[0];
#endif
}

#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

#ifdef SW_CONTROLLED_CSMA

/**
 * \brief Handles software-controlled CSMA.
 *
 * \param csma_mode     CSMA Mode; eg. ED or CS
 * \param retransmit    true if frame re-transmission is requested
 */
static inline void sw_controlled_csma(csma_mode_t csma_mode, bool retransmit)
{
	if (retransmit) {
		number_of_tx_retries = 0; /* actual number of retries */
	} else {
		/* no further retries */
		number_of_tx_retries = tal_pib.MaxFrameRetries;
	}

	/* Handle interframe spacing */
	if (csma_mode == NO_CSMA_WITH_IFS) {
		if (last_frame_length > aMaxSIFSFrameSize) {
			pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(
					macMinLIFSPeriod_def)
					- TRX_IRQ_DELAY_US -
					PRE_TX_DURATION_US);
			last_frame_length = 0;
		} else {
			pal_timer_delay(TAL_CONVERT_SYMBOLS_TO_US(
					macMinSIFSPeriod_def)
					- TRX_IRQ_DELAY_US -
					PRE_TX_DURATION_US);
			last_frame_length = 0;
		}
	}

	if ((csma_mode == NO_CSMA_WITH_IFS) || (csma_mode == NO_CSMA_NO_IFS)) {
		tx_frame();
	} else {
		csma_start();
	}
}

#endif

#ifdef SW_CONTROLLED_CSMA

/**
 * \brief Starts software-controlled CSMA.
 */
static void csma_start(void)
{
	/* Initialize CSMA variables */
	NB = 0;
	BE = tal_pib.MinBE;

	/* Start backoff timer to trigger CCA */
	start_backoff();
}

#endif

#ifdef SW_CONTROLLED_CSMA

/**
 * \brief Starts the timer for the backoff period and enables receiver.
 */
static void start_backoff(void)
{
	/* Start backoff timer to trigger CCA */
	uint8_t backoff_8;
	backoff_8  = (uint8_t)rand() & ((1 << BE) - 1);
	if (backoff_8 > 0) {
		uint16_t backoff_16;
		uint32_t backoff_duration_us;
		backoff_16 = backoff_8 * aUnitBackoffPeriod;
		backoff_duration_us = TAL_CONVERT_SYMBOLS_TO_US(backoff_16);
		pal_timer_start(TAL_T_BOFF, backoff_duration_us,
				TIMEOUT_RELATIVE,
				(FUNC_PTR)cca_start, NULL);
		tal_state = TAL_BACKOFF;

#ifdef RX_WHILE_BACKOFF
		/* Switch receiver on during backoff */
		if (NULL == tal_rx_buffer) {
			set_trx_state(CMD_PLL_ON);
			tal_rx_on_required = true;
		} else {
			set_trx_state(CMD_RX_AACK_ON); /* receive while backoff
			                               **/
		}

#else
		set_trx_state(CMD_PLL_ON);
#endif
	} else {
		/* Start CCA immediately - no backoff */
		cca_start(NULL);
	}
}

#endif

#ifdef SW_CONTROLLED_CSMA

/*
 * \brief Continues CSMA; handles next CSMA retry.
 */
void csma_continue(void)
{
	NB++;
	if (NB > tal_pib.MaxCSMABackoffs) {
		trx_trac_status = TRAC_CHANNEL_ACCESS_FAILURE;
		tal_state = TAL_TX_DONE;
		set_trx_state(CMD_RX_AACK_ON);
	} else {
		BE++;
		if (BE > tal_pib.MaxBE) {
			BE = tal_pib.MaxBE;
		}

		/* Start backoff timer to trigger CCA */
		start_backoff();
	}
}

#endif

#ifdef SW_CONTROLLED_CSMA

/**
 * \brief Start CCA.
 *
 * \param parameter Unused callback parameter
 */
static void cca_start(void *parameter)
{
	tal_state = TAL_CCA;

	if (set_trx_state(CMD_PLL_ON) == PLL_ON) {
		tal_trx_status_t trx_state;
		/* No interest in receiving frames while doing CCA */
		trx_bit_write(SR_RX_PDT_DIS, RX_DISABLE); /* disable frame
		                                           * reception
		                                           * indication */
		do {
			trx_state = set_trx_state(CMD_RX_ON);
		} while (trx_state != RX_ON);
		/* Setup interrupt handling for CCA IRQ */
		trx_irq_init((FUNC_PTR)cca_done_irq_handler);
		trx_reg_write(RG_IRQ_MASK, TRX_IRQ_4_CCA_ED_DONE); /* enable
		                                                    * CCA
		                                                    * interrupt
		                                                    **/
		/* Start CCA */
		trx_bit_write(SR_CCA_REQUEST, CCA_START);
	} else {
		/* Channel is busy, i.e. device is receiving */
		tal_state = TAL_CSMA_CONTINUE;
	}

	/* Keep compiler happy. */
	parameter = parameter;
}

#endif

#ifdef SW_CONTROLLED_CSMA

/**
 * \brief Callback function for CCA completion.
 */
static void cca_done_irq_handler(void)
{
	/* Clear CCA interrupt. */
	trx_reg_read(RG_IRQ_STATUS);

	tal_state = TAL_CCA_DONE;
}

#endif

#ifdef SW_CONTROLLED_CSMA

/*
 * \brief handling of CCA result.
 */
void cca_done_handling(void)
{
	set_trx_state(CMD_PLL_ON); /* leave RX_ON */
	/* Restore IRQ handling */
	trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
	trx_reg_write(RG_IRQ_MASK, TRX_IRQ_DEFAULT);
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE); /* Enable frame reception.
	                                         **/

	/* Check if channel was idle or busy */
	if (trx_bit_read(SR_CCA_STATUS) == CCA_CH_IDLE) {
		tx_frame();
	} else {
		tal_state = TAL_CSMA_CONTINUE;
	}
}

#endif

#ifdef SW_CONTROLLED_CSMA

/**
 * \brief Transmits the frame over-the-air
 */
static void tx_frame(void)
{
	tal_trx_status_t trx_status;

	/*
	 * Trigger transmission
	 * In case of an ongoing reception,
	 * the incoming frame is handled first within ISR.
	 */
	do {
		trx_status = set_trx_state(CMD_TX_ARET_ON);
	} while (trx_status != TX_ARET_ON);

	pal_trx_irq_dis();

	/* Toggle the SLP_TR pin triggering transmission. */
	TRX_SLP_TR_HIGH();
	PAL_WAIT_65_NS();
	TRX_SLP_TR_LOW();

	/*
	 * Send the frame to the transceiver.
	 * Note: The PhyHeader is the first byte of the frame to
	 * be sent to the transceiver and this contains the frame
	 * length.
	 * The actual length of the frame to be downloaded
	 * (parameter two of trx_frame_write)
	 * is
	 * 1 octet frame length octet
	 * + n octets frame (i.e. value of frame_tx[0])
	 * + 1 extra octet (see datasheet)
	 * - 2 octets FCS
	 */
	trx_frame_write(tal_frame_to_tx, tal_frame_to_tx[0]);

	tal_state = TAL_TX_AUTO;

#ifndef NON_BLOCKING_SPI
	pal_trx_irq_en();
#endif
#ifdef TX_OCTET_COUNTER
	tal_tx_octet_cnt += PHY_OVERHEAD + LENGTH_FIELD_LEN +
			tal_frame_to_tx[0];
#endif
}

#endif

/* EOF */
