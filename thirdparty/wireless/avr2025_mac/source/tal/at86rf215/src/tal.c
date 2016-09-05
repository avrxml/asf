/**
 * @file tal.c
 *
 * @brief This file implements the TAL state machine and provides general
 *        functionality used by the TAL.
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "tal_config.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_internal.h"
#include "mac_build_config.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* Workaround for errata reference #4908; deaf period */
#define AGC_LEG_OQPSK_DEAF_PERIOD   300

/* === GLOBALS ============================================================= */

/*
 * Global TAL variables
 * These variables are only to be used by the TAL internally.
 */

/**
 * TAL PIBs
 */
tal_pib_t tal_pib[NUM_TRX];

/**
 * Current state of the TAL state machine.
 */
tal_state_t tal_state[NUM_TRX];

/**
 * Current state of the TX state machine.
 */
tx_state_t tx_state[NUM_TRX];

/**
 * Indicates if a buffer shortage issue needs to handled by tal_task().
 */
bool tal_buf_shortage[NUM_TRX];

/**
 * Pointer to the 15.4 frame created by the TAL to be handed over
 * to the transceiver.
 */
uint8_t *tal_frame_to_tx[NUM_TRX];

/**
 * Pointer to receive buffer that can be used to upload a frame from the trx.
 */
buffer_t *tal_rx_buffer[NUM_TRX];

/**
 * Queue that contains all frames that are uploaded from the trx, but have not
 * be processed by the MCL yet.
 */
queue_t tal_incoming_frame_queue[NUM_TRX];

/**
 * Frame pointer for the frame structure provided by the MCL.
 */
frame_info_t *mac_frame_ptr[NUM_TRX];

/**
 * Shadow variable for BB IRQS; variable is filled during TRX ISR,
 * see trx_irq_handler_cb() in tal_irq_handler.c
 */
#if (NUM_TRX == 1)
volatile bb_irq_t tal_bb_irqs[NUM_TRX] = {BB_IRQ_NO_IRQ};
#else
volatile bb_irq_t tal_bb_irqs[NUM_TRX] = {BB_IRQ_NO_IRQ, BB_IRQ_NO_IRQ};
#endif

/**
 * Shadow variable for RF IRQS; variable is filled during TRX ISR,
 * see trx_irq_handler_cb() in tal_irq_handler.c
 */
#if (NUM_TRX == 1)
volatile rf_irq_t tal_rf_irqs[NUM_TRX] = {RF_IRQ_NO_IRQ};
#else
volatile rf_irq_t tal_rf_irqs[NUM_TRX] = {RF_IRQ_NO_IRQ, RF_IRQ_NO_IRQ};
#endif

/**
 * Last retrieved energy value;
 * variable is filled by handle_ed_end_irq() in tal_ed.c
 */
int8_t tal_current_ed_val[NUM_TRX];

/** Parameter to handle timer callback functions */
#if (NUM_TRX == 1)
const uint8_t timer_cb_parameter[NUM_TRX] = {ACTIVE_TRX};
#else
const uint8_t timer_cb_parameter[NUM_TRX] = {RF09, RF24};
#endif

/** Current trx state */
rf_cmd_state_t trx_state[NUM_TRX];

/** Default/Previous trx state while entering a transaction */
rf_cmd_state_t trx_default_state[NUM_TRX];

/** Flag indicating ongoing ACK transmission */
bool ack_transmitting[NUM_TRX];

#if (defined ENABLE_TSTAMP) || (defined MEASURE_ON_AIR_DURATION)

/**
 * Frame start time stamp
 * During Tx transaction it stores the time when the frame is actually send,
 * i.e. frame start.
 * During Rx transaction it stores the time when the frame is actually received,
 * i.e. frame end - RXFE.
 */
uint32_t fs_tstamp[NUM_TRX];
#endif  /* #ifdef ENABLE_TSTAMP */

/**
 * Timestamp for end of reception, i.e. RXFE IRQ;
 * used during Rx transaction to calculate ACK transmission time.
 * Timestamp for end of transmission;
 * used during Tx transaction to calculate ACK timeout.
 * Both scenarios use the same variable
 */
uint32_t rxe_txe_tstamp[NUM_TRX];

/**
 * TX calibration values
 */
uint8_t txc[NUM_TRX][2];

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
static bool agc_timer_running[NUM_TRX] = {false, false};
#endif

/* === PROTOTYPES ========================================================== */

static void handle_trxerr(trx_id_t trx_id);

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
/* Workaround for errata reference #4908 */
static void inline start_agc_timer(trx_id_t trx_id);
static void inline stop_agc_timer(trx_id_t trx_id);
static void inline trigger_agc_workaround(void *cb_timer_element);

#endif

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief TAL task handling
 *
 * This function
 * - handles any pending IRQ flags
 * - Processes the TAL incoming frame queue.
 * The function needs to be called on a regular basis.
 */
void tal_task(void)
{
	for (uint8_t trx_id = 0; trx_id < NUM_TRX; trx_id++) {
		if (tal_state[trx_id] == TAL_SLEEP) {
			continue;
		}

		/*
		 * Handle buffer shortage.
		 * Check if the receiver needs to be switched on.
		 */
		if (tal_buf_shortage[trx_id]) {
			/* If necessary, try to allocate a new buffer. */
			if (tal_rx_buffer[trx_id] == NULL) {
				tal_rx_buffer[trx_id] = bmm_buffer_alloc(
						LARGE_BUFFER_SIZE);
			}

			/* Check if buffer could be allocated */
			if (tal_rx_buffer[trx_id] != NULL) {
				tal_buf_shortage[trx_id] = false;
				if ((tal_state[trx_id] == TAL_IDLE)) { /* &&
					                                *
					                                *(trx_default_state[trx_id]
					                                * ==
					                                * RF_RX)
					                                **/
					switch_to_rx((trx_id_t)trx_id);
				} else {
					/*
					 * If TAL is currently not IDLE, it will
					 * recover once it
					 * gets IDLE again.
					 */
				}
			} else {
			}
		}

		/*
		 * If the transceiver has received a frame and it has been
		 * placed
		 * into the queue of the TAL, the frame needs to be processed
		 * further.
		 */
		if (tal_incoming_frame_queue[trx_id].size > 0) {
			buffer_t *rx_frame;

			/* Check if there are any pending data in the
			 * incoming_frame_queue. */
			rx_frame
				= qmm_queue_remove(
					&tal_incoming_frame_queue[trx_id],
					NULL);
			if (rx_frame != NULL) {
				process_incoming_frame((trx_id_t)trx_id,
						rx_frame);
			}
		}

		/*
		 * Handle pending interrupt events
		 * that have not been processed within the ISR
		 */
		bb_irq_t bb_irqs;
		rf_irq_t rf_irqs;
		ENTER_CRITICAL_REGION();
		bb_irqs = tal_bb_irqs[trx_id];
		rf_irqs = tal_rf_irqs[trx_id];
		LEAVE_CRITICAL_REGION();

		if (bb_irqs != BB_IRQ_NO_IRQ) {
			if (bb_irqs & BB_IRQ_RXEM) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_RXEM);
			}

			if (bb_irqs & BB_IRQ_RXAM) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_RXAM);
			}

			if (bb_irqs & BB_IRQ_RXFS) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_RXFS);
				#if ((defined RF215v1) && \
				(defined SUPPORT_LEGACY_OQPSK))
				/* Workaround for errata reference #4908 */
				stop_agc_timer(trx_id);
				#endif
			}

			if (bb_irqs & BB_IRQ_RXFE) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_RXFE);

				handle_rx_end_irq((trx_id_t)trx_id);
			}

			if (bb_irqs & BB_IRQ_TXFE) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_TXFE);
#ifndef BASIC_MODE
				if (tx_state[trx_id] == TX_CCATX) {
					/* Clear TRXRDY IRQ that has been issued
					 * while switching from Rx to TX via
					 * TXPREP */

					/* Clear EDC IRQ that has been issued
					 * for CCA measurement */
					TAL_RF_IRQ_CLR(trx_id,
							(RF_IRQ_EDC |
							RF_IRQ_TRXRDY));
					rf_irqs
						&= (uint8_t)(~((uint32_t)(
								RF_IRQ_EDC
								| RF_IRQ_TRXRDY)));
				}

#endif
				handle_tx_end_irq((trx_id_t)trx_id);
			}

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
			if (bb_irqs & BB_IRQ_AGCH) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_AGCH);
				/* Workaround for errata reference #4908 */
				if ((bb_irqs & BB_IRQ_AGCR) == 0) {
					start_agc_timer(trx_id);
				}
			}

#endif
#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
			/* Workaround for errata reference #4908 */
			if (bb_irqs & BB_IRQ_AGCR) {
				TAL_BB_IRQ_CLR(trx_id, BB_IRQ_AGCR);
				stop_agc_timer(trx_id);
			}

#endif
		}

		if (rf_irqs & RF_IRQ_EDC) {
			TAL_RF_IRQ_CLR(trx_id, RF_IRQ_EDC);
			handle_ed_end_irq((trx_id_t)trx_id);
		}

		if (rf_irqs & RF_IRQ_TRXERR) {
			TAL_RF_IRQ_CLR(trx_id, RF_IRQ_TRXERR);
			handle_trxerr((trx_id_t)trx_id);
		}

		if (rf_irqs & RF_IRQ_BATLOW) {
			TAL_RF_IRQ_CLR(trx_id, RF_IRQ_BATLOW);
#if (defined ENABLE_TFA) || (defined TFA_BAT_MON_IRQ)
			handle_batmon_irq();         /* see tfa_batmon.c */
#endif
		}

		if (rf_irqs & RF_IRQ_WAKEUP) {
			TAL_RF_IRQ_CLR(trx_id, RF_IRQ_WAKEUP);
		}

		if (rf_irqs & RF_IRQ_IQIFSF) {
			TAL_RF_IRQ_CLR(trx_id, RF_IRQ_IQIFSF);
		}
	}
}

/* tal_task() */

/**
 * @brief Switches the transceiver to Rx
 *
 * This function tries to switch the transceiver to Rx. It checks if a Rx buffer
 * is available. Only if a buffer is available, the receiver is actually
 * switched on. If no buffer is available, the tal_buf_shortage flag is set.
 * This buffer shortage issue is tried to solve within tal_task() again.
 *
 * @param trx_id Transceiver identifier
 */
void switch_to_rx(trx_id_t trx_id)
{
	/* Check if buffer is available now. */
	if (tal_rx_buffer[trx_id] != NULL) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_reg_write(reg_offset + RG_RF09_CMD, RF_RX);
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_RX);
#endif
		trx_state[trx_id] = RF_RX;
#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
		start_rpc(trx_id);
#endif
	} else {
		switch_to_txprep(trx_id);
		tal_buf_shortage[trx_id] = true;
	}
}

/**
 * @brief Switches the transceiver to TxPREP
 *
 * This function switches the transceiver to TxPREP and waits until it has
 * has reached this state. Do not call this function within an ISR.
 *
 * @param trx_id Transceiver identifier
 */
void switch_to_txprep(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	trx_reg_write(reg_offset + RG_RF09_CMD, RF_TXPREP);
#ifdef IQ_RADIO
	pal_dev_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TXPREP);
#endif

	wait_for_txprep(trx_id);

#ifdef RF215v1
	/* Workaround for errata reference #4807 */
#   ifdef IQ_RADIO
	trx_write(RF215_RF, reg_offset + 0x125, (uint8_t *)&txc[trx_id][0], 2);
#   else
	trx_write( reg_offset + 0x125, (uint8_t *)&txc[trx_id][0], 2);
#   endif /* #ifdef RF215v1 */
#endif /* ifdef IQ_RADIO */
}

/**
 * @brief Wait to reach TXPREP
 *
 * @param trx_id Transceiver identifier
 */
void wait_for_txprep(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	rf_cmd_state_t state;

#ifdef RF215v1
	uint32_t start_time = 0;

	pal_get_current_time(&start_time);

	do {
#ifdef IQ_RADIO
		state = (rf_cmd_state_t)trx_reg_read(RF215_RF,
				reg_offset +
				RG_RF09_STATE);
#else
		state
			= (rf_cmd_state_t)trx_reg_read( reg_offset +
				RG_RF09_STATE);
#endif
		if (state != RF_TXPREP) {
#if (POLL_TIME_GAP > 0)
			pal_timer_delay(POLL_TIME_GAP); /* Delay to reduce SPI
			                                 *storm */
#endif
			/* Workaround for errata reference #4810 */
			uint32_t now;
			pal_get_current_time(&now);
			if (abs(now - start_time) > MAX_PLL_LOCK_DURATION) {
				do {
					trx_reg_write( reg_offset + RG_RF09_PLL,
							9);
					pal_timer_delay(
							PLL_FRZ_SETTLING_DURATION);
					trx_reg_write( reg_offset + RG_RF09_PLL,
							8);
					pal_timer_delay(
							PLL_FRZ_SETTLING_DURATION);
					state = (rf_cmd_state_t)trx_reg_read(
							reg_offset +
							RG_RF09_STATE);
				} while (state != RF_TXPREP);
			}
		}
	} while (state != RF_TXPREP);

#else /* #if RF215v1 */
	do {
#ifdef IQ_RADIO
		state = (rf_cmd_state_t)trx_reg_read(RF215_RF,
				reg_offset +
				RG_RF09_STATE);
#else
		state
			= (rf_cmd_state_t)trx_reg_read( reg_offset +
				RG_RF09_STATE);
#endif
		if (state != RF_TXPREP) {
#if (POLL_TIME_GAP > 0)
			pal_timer_delay(POLL_TIME_GAP); /* Delay to reduce SPI
			                                 *storm */
#endif
		}
	} while (state != RF_TXPREP);
#endif /* #if RF215v1 */

	trx_state[trx_id] = RF_TXPREP;
}

/**
 * @brief Handles a transceiver error
 *
 * This function handles a TRXERR interrupt.
 *
 * @param trx_id Transceiver identifier
 */
static void handle_trxerr(trx_id_t trx_id)
{
	/* Set device to TRXOFF */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
	trx_state[trx_id] = RF_TRXOFF;
	tx_state[trx_id] = TX_IDLE;
	stop_tal_timer(trx_id);
#ifdef ENABLE_FTN_PLL_CALIBRATION
	stop_ftn_timer(trx_id);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */

	switch (tal_state[trx_id]) {
	case TAL_TX:
		tx_done_handling(trx_id, FAILURE);
		break;

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case TAL_ED_SCAN:
		stop_ed_scan(trx_id); /* see tal_ed.c; covers state handling */
		break;
#endif

	default:
		if (trx_default_state[trx_id] == RF_RX) {
			tal_rx_enable(trx_id, PHY_RX_ON);
		}

		tal_state[trx_id] = TAL_IDLE;
		break;
	}
}

/**
 * @brief Stops TAL timer
 *
 * @param trx_id Transceiver identifier
 */
void stop_tal_timer(trx_id_t trx_id)
{
	#if (TAL_FIRST_TIMER_ID == 0)
	pal_timer_stop(trx_id);
	#else
	uint8_t timer_id;
	if (trx_id == RF09) {
		timer_id = TAL_T_0;
	} else {
		timer_id = TAL_T_1;
	}
	pal_timer_stop(timer_id);
	#endif
}

#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)

/**
 * @brief Stops RPC; SW workaround for errata reference 4841
 *
 * @param trx_id Transceiver identifier
 */
void stop_rpc(trx_id_t trx_id)
{
	if (tal_pib[trx_id].RPCEnabled &&
			((tal_pib[trx_id].phy.modulation == OQPSK) ||
			(tal_pib[trx_id].phy.modulation == FSK))) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_OQPSK
		case OQPSK:
			trx_bit_write( reg_offset + SR_BBC0_OQPSKC2_RPC, 0);
			break;

#endif
#ifdef SUPPORT_FSK
		case FSK:
			trx_bit_write( reg_offset + SR_BBC0_FSKRPC_EN, 0);
			/* Configure preamble length for transmission */
			trx_reg_write( reg_offset + RG_BBC0_FSKPLL,
					(uint8_t)(tal_pib[trx_id].
					FSKPreambleLength & 0xFF));
			trx_bit_write( reg_offset + SR_BBC0_FSKC1_FSKPLH,
					(uint8_t)(tal_pib[trx_id].
					FSKPreambleLength >> 8));
			break;

#endif
		default:
			break;
		}
#   ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_PLL, 8);
#   else
		trx_reg_write( reg_offset + RG_RF09_PLL, 8);
#   endif
	}
}

#endif /* #if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK) */

#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)

/**
 * @brief SW workaround for errata reference 4841
 *
 * @param trx_id Transceiver identifier
 */
void start_rpc(trx_id_t trx_id)
{
	if (tal_pib[trx_id].RPCEnabled &&
			(
#if ((defined SUPPORT_FSK) && (defined SUPPORT_OQPSK))
				(tal_pib[trx_id].phy.modulation == OQPSK) ||
				(tal_pib[trx_id].phy.modulation == FSK)
#elif (defined SUPPORT_FSK)
				(tal_pib[trx_id].phy.modulation == FSK)
#else
				(tal_pib[trx_id].phy.modulation == OQPSK)
#endif
			)
			) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
#   ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_PLL, 9);
#   else
		trx_reg_write( reg_offset + RG_RF09_PLL, 9);
#   endif
		switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_OQPSK
		case OQPSK:
			trx_bit_write(reg_offset + SR_BBC0_OQPSKC2_RPC, 1);
			break;

#endif
#ifdef SUPPORT_FSK
		case FSK:
			trx_bit_write( reg_offset + SR_BBC0_FSKRPC_EN, 1);
			/* Configure preamble length for reception */
			trx_reg_write( reg_offset + RG_BBC0_FSKPLL,
					(uint8_t)(tal_pib[trx_id].
					FSKPreambleLengthMin & 0xFF));
			trx_bit_write( reg_offset + SR_BBC0_FSKC1_FSKPLH,
					(uint8_t)(tal_pib[trx_id].
					FSKPreambleLengthMin >> 8));
			break;

#endif
		default:
			break;
		}
	}
}

#endif /* #if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK) */

/**
 * @brief Cancel any ongoing receive transaction incl. ACK transmission
 *
 * @param trx_id Transceiver identifier
 */
void cancel_any_reception(trx_id_t trx_id)
{
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	if (trx_state[trx_id] != RF_TXPREP) {
		switch_to_txprep(trx_id);
	}

	ack_transmitting[trx_id] = false;

	/* Clear any pending interrupts */
	ENTER_TRX_REGION();
	/* Clear IRQS at transceiver */
	uint8_t temp;
	if (trx_id == RF09) {
		temp = trx_reg_read( RG_RF09_IRQS);
		temp = trx_reg_read( RG_BBC0_IRQS);
	} else { /* RF24 */
		temp = trx_reg_read( RG_RF24_IRQS);
		temp = trx_reg_read( RG_BBC1_IRQS);
	}

	(void)(temp); /* Keep compiler happy */
	/* Clear shadow variables */
	TAL_BB_IRQ_CLR_ALL(trx_id);
	TAL_RF_IRQ_CLR_ALL(trx_id);
	LEAVE_TRX_REGION();

#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
	stop_rpc(trx_id);
#endif
}

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
/* Workaround for errata reference #4908 */
static void inline start_agc_timer(trx_id_t trx_id)
{
	uint8_t timer_id;
	if (trx_id == RF09) {
		timer_id = TAL_T_AGC_0;
	} else {
		timer_id = TAL_T_AGC_1;
	}

	if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
		pal_timer_start(timer_id, AGC_LEG_OQPSK_DEAF_PERIOD,
				TIMEOUT_RELATIVE,
				(FUNC_PTR)trigger_agc_workaround,
				(void *)trx_id);
		agc_timer_running[trx_id] = true;
	}
}

#endif

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
/* Workaround for errata reference #4908 */
static void inline stop_agc_timer(trx_id_t trx_id)
{
	if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
		uint8_t timer_id;
		if (trx_id == RF09) {
			timer_id = TAL_T_AGC_0;
		} else {
			timer_id = TAL_T_AGC_1;
		}

		if (agc_timer_running[trx_id]) {
			pal_timer_stop(timer_id);
		}
	}
}

#endif

#if ((defined RF215v1) && (defined SUPPORT_LEGACY_OQPSK))
/* Workaround for errata reference #4908 */
static void inline trigger_agc_workaround(void *cb_timer_element)
{
	trx_id_t trx_id = (trx_id_t)cb_timer_element;
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	agc_timer_running[trx_id] = false;

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 0);
	trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 1);
}

#endif

/* EOF */
