/**
 * @file tal.c
 *
 * @brief This file implements the TAL state machine and provides general
 * functionality used by the TAL.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
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
#include "tal_irq_handler.h"
#include "at86rf230b.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_rx.h"
#include "tal_tx.h"
#include "tal_constants.h"
#include "tal_internal.h"
#ifdef BEACON_SUPPORT
#include "tal_slotted_csma.h"
#endif  /* BEACON_SUPPORT */
#include "mac_build_config.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/*
 * TAL PIBs
 */
tal_pib_t tal_pib;

/*
 * Global TAL variables
 * These variables are only to be used by the TAL internally.
 */

/**
 * Current state of the TAL state machine.
 * \ingroup group_tal_state_machine_230b
 */
tal_state_t tal_state;

/**
 * Current state of the transceiver.
 * \ingroup group_tal_state_machine_230b
 */
tal_trx_status_t tal_trx_status;

/**
 * Indicates if the transceiver needs to switch on its receiver by tal_task(),
 * because it could not be switched on due to buffer shortage.
 * \ingroup group_tal_state_machine_230b
 */
bool tal_rx_on_required;

/**
 * Pointer to the 15.4 frame created by the TAL to be handed over
 * to the transceiver.
 */
uint8_t *tal_frame_to_tx;

/**
 * Pointer to receive buffer that can be used to upload a frame from the trx.
 */
buffer_t *tal_rx_buffer = NULL;

/**
 * Queue that contains all frames that are uploaded from the trx, but have not
 * be processed by the MCL yet.
 */
queue_t tal_incoming_frame_queue;

/**
 * Frame pointer for the frame structure provided by the MCL.
 */
frame_info_t *mac_frame_ptr;

/* Last frame length for IFS handling. */
uint8_t last_frame_length;
#ifdef BEACON_SUPPORT

/**
 * CSMA state machine variable
 */
csma_state_t tal_csma_state;
#endif  /* BEACON_SUPPORT */

/* === PROTOTYPES ========================================================== */

static void switch_pll_on(void);

#ifdef ENABLE_FTN_PLL_CALIBRATION
static void handle_ftn_pll_calibration(void);

#endif  /* ENABLE_FTN_PLL_CALIBRATION */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief TAL task handling
 *
 * This function
 * - Checks and allocates the receive buffer.
 * - Processes the TAL incoming frame queue.
 * - Implements the TAL state machine.
 */
void tal_task(void)
{
	/* Check if the receiver needs to be switched on. */
	if (tal_rx_on_required && (tal_state == TAL_IDLE)) {
		/* Check if a receive buffer has not been available before. */
		if (tal_rx_buffer == NULL) {
			tal_rx_buffer = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
		}

		/* Check if buffer could be allocated */
		if (NULL != tal_rx_buffer) {
			/*
			 * Note:
			 * This flag needs to be reset BEFORE the received is
			 *switched on.
			 */
			tal_rx_on_required = false;

#ifdef PROMISCUOUS_MODE
			if (tal_pib.PromiscuousMode) {
				set_trx_state(CMD_RX_ON);
			} else {
				set_trx_state(CMD_RX_AACK_ON);
			}

#else   /* Normal operation */
			set_trx_state(CMD_RX_AACK_ON);
#endif
		}
	} else {
		/* no free buffer is available; try next time again */
	}

	/*
	 * If the transceiver has received a frame and it has been placed
	 * into the queue of the TAL, the frame needs to be processed further.
	 */
	if (tal_incoming_frame_queue.size > 0) {
		buffer_t *rx_frame;

		/* Check if there are any pending data in the
		 *incoming_frame_queue. */
		rx_frame = qmm_queue_remove(&tal_incoming_frame_queue, NULL);
		if (NULL != rx_frame) {
			process_incoming_frame(rx_frame);
		}
	}

#ifdef BEACON_SUPPORT
	/* handle slotted CSMA state machine if required */
	if (tal_csma_state != CSMA_IDLE) {
		slotted_csma_state_handling(); /* see tal_slotted_csma.c */
	}

#endif  /* BEACON_SUPPORT */

	/* handle the TAL state machines */
	if (tal_state != TAL_IDLE) {
		uint8_t tal_sub_state;

		tal_sub_state = tal_state & 0x0F;

		switch (tal_sub_state) {
		case TAL_TX_BASIC:
		case TAL_TX_AUTO:
#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))
		case TAL_TX_BEACON:
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */
			tx_state_handling(); /* see tal_tx.c */
			break;

		case TAL_SLEEP:
			break;

#ifdef BEACON_SUPPORT
		case TAL_SLOTTED_CSMA:
			break;
#endif  /* BEACON_SUPPORT */

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
		case TAL_ED:
			/* Do nothing here. */
			break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

		default:
			Assert("tal_state is not handled" == 0);
			break;
		}
	}
} /* tal_task() */

/*
 * \brief Sets transceiver state
 *
 * \param trx_cmd needs to be one of the trx commands
 *
 * \return current trx state
 */
tal_trx_status_t set_trx_state(trx_cmd_t trx_cmd)
{
	if (tal_trx_status == TRX_SLEEP) {
		uint8_t bit_status;

		PAL_SLP_TR_LOW();
		do {
			bit_status = pal_trx_bit_read(SR_TRX_STATUS);
		} while (bit_status != TRX_OFF);

		if ((trx_cmd == CMD_TRX_OFF) ||
				(trx_cmd == CMD_FORCE_TRX_OFF)) {
			tal_trx_status = TRX_OFF;
			return TRX_OFF;
		}
	}

	tal_trx_status = (tal_trx_status_t)pal_trx_bit_read(SR_TRX_STATUS);

	switch (trx_cmd) { /* new state */
	case CMD_TRX_SLEEP:
		pal_trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);

		{
			uint16_t rand_value;

			/*
			 * Init the SEED value of the CSMA backoff algorithm.
			 */
			rand_value = (uint16_t)rand();
			pal_trx_reg_write(RG_CSMA_SEED_0, (uint8_t)rand_value);
			pal_trx_bit_write(SR_CSMA_SEED_1,
					(uint8_t)(rand_value >> 8));
		}

		PAL_WAIT_1_US();
		PAL_SLP_TR_HIGH();
		pal_timer_delay(TRX_OFF_TO_SLEEP_TIME);
		tal_trx_status = TRX_SLEEP;
		return TRX_SLEEP; /* transceiver register cannot be read during
		                   *TRX_SLEEP */

	/* break; // do not use break, since it is unreachable */

	case CMD_TRX_OFF:
		switch (tal_trx_status) {
		case TRX_OFF:
			break;

		default:
			pal_trx_reg_write(RG_TRX_STATE, CMD_TRX_OFF);
			PAL_WAIT_1_US();
			break;
		}
		break;

	case CMD_FORCE_TRX_OFF:
		switch (tal_trx_status) {
		case TRX_OFF:
			break;

		case TX_ARET_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
			PAL_SLP_TR_HIGH();
			pal_timer_delay(2);
			PAL_SLP_TR_LOW();
			break;

		default:
			pal_trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
			PAL_WAIT_1_US();
			break;
		}
		break;

	case CMD_PLL_ON:
		switch (tal_trx_status) {
		case PLL_ON:
			break;

		case TRX_OFF:
			switch_pll_on();
			break;

		case RX_ON:
		case RX_AACK_ON:
		case TX_ARET_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			PAL_WAIT_1_US();
			break;

		case BUSY_RX:
		case BUSY_TX:
		case BUSY_RX_AACK:
		case BUSY_TX_ARET:
			/* do nothing if trx is busy */
			break;

		default:
			Assert("state transition not handled" == 0);
			break;
		}
		break;

	case CMD_FORCE_PLL_ON:  /* software state */
		switch (tal_trx_status) {
		case RX_ON:
		case BUSY_RX:
		case RX_AACK_ON:
		case BUSY_RX_AACK:
			ENTER_TRX_REGION();
			pal_trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			/* clear trx irq in case a frame is received meanwhile */
			pal_trx_reg_read(RG_IRQ_STATUS);
			pal_trx_irq_flag_clr();
			LEAVE_TRX_REGION();
			PAL_WAIT_1_US();
			break;

		case BUSY_TX:
			ENTER_TRX_REGION();
			pal_trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			LEAVE_TRX_REGION();
			PAL_WAIT_1_US();
			break;

		case BUSY_TX_ARET:
			ENTER_TRX_REGION();
			pal_trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
			PAL_SLP_TR_LOW();
			pal_timer_delay(SLP_TR_TOGGLE_US);
			PAL_SLP_TR_HIGH();
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			LEAVE_TRX_REGION();
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on();
			break;

		case PLL_ON:
			break;

		default:
			Assert("state transition not handled" == 0);
			break;
		}
		break;

	case CMD_RX_ON:
		switch (tal_trx_status) {
		case RX_ON:
			break;

		case PLL_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_RX_ON);
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on();
			pal_trx_reg_write(RG_TRX_STATE, CMD_RX_ON);
			PAL_WAIT_1_US();
			break;

		case RX_AACK_ON:
		case TX_ARET_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			PAL_WAIT_1_US();
			/* check if state change could be applied */
			tal_trx_status = (tal_trx_status_t)pal_trx_bit_read(
					SR_TRX_STATUS);
			if (tal_trx_status != PLL_ON) {
				return tal_trx_status;
			}

			pal_trx_reg_write(RG_TRX_STATE, CMD_RX_ON);
			PAL_WAIT_1_US();
			break;

		case BUSY_RX:
		case BUSY_TX:
		case BUSY_RX_AACK:
		case BUSY_TX_ARET:
			/* do nothing if trx is busy */
			break;

		default:
			Assert("state transition not handled" == 0);
			break;
		}
		break;

	case CMD_RX_AACK_ON:
		switch (tal_trx_status) {
		case RX_AACK_ON:
			break;

		case PLL_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_RX_AACK_ON);
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on(); /* state change from TRX_OFF to
			                  * RX_AACK_ON can be done directly, too */
			pal_trx_reg_write(RG_TRX_STATE, CMD_RX_AACK_ON);
			PAL_WAIT_1_US();
			break;

		case TX_ARET_ON:
		case RX_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			PAL_WAIT_1_US();
			/* check if state change could be applied */
			tal_trx_status = (tal_trx_status_t)pal_trx_bit_read(
					SR_TRX_STATUS);
			if (tal_trx_status != PLL_ON) {
				return tal_trx_status;
			}

			pal_trx_reg_write(RG_TRX_STATE, CMD_RX_AACK_ON);
			PAL_WAIT_1_US();
			break;

		case BUSY_RX:
		case BUSY_TX:
		case BUSY_RX_AACK:
		case BUSY_TX_ARET:
			/* do nothing if trx is busy */
			break;

		default:
			Assert("state transition not handled" == 0);
			break;
		}
		break;

	case CMD_TX_ARET_ON:
		switch (tal_trx_status) {
		case TX_ARET_ON:
			break;

		case PLL_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_TX_ARET_ON);
			PAL_WAIT_1_US();
			break;

		case RX_ON:
		case RX_AACK_ON:
			pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			PAL_WAIT_1_US();
			/* check if state change could be applied */
			tal_trx_status = (tal_trx_status_t)pal_trx_bit_read(
					SR_TRX_STATUS);
			if (tal_trx_status != PLL_ON) {
				return tal_trx_status;
			}

			pal_trx_reg_write(RG_TRX_STATE, CMD_TX_ARET_ON);
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on(); /* state change from TRX_OFF to
			                  * TX_ARET_ON can be done directly, too */
			pal_trx_reg_write(RG_TRX_STATE, CMD_TX_ARET_ON);
			PAL_WAIT_1_US();
			break;

		case BUSY_RX:
		case BUSY_TX:
		case BUSY_RX_AACK:
		case BUSY_TX_ARET:
		case STATE_TRANSITION_IN_PROGRESS:
			/* do nothing if trx is busy */
			break;

		default:
			Assert("state transition not handled" == 0);
			break;
		}
		break;

	default:
		/* CMD_NOP, CMD_TX_START */
		Assert("trx command not handled" == 0);
		break;
	}

	do {
		tal_trx_status = (tal_trx_status_t)pal_trx_bit_read(
				SR_TRX_STATUS);
	} while (tal_trx_status == STATE_TRANSITION_IN_PROGRESS);

	return tal_trx_status;
} /* set_trx_state() */

/**
 * \brief Switches the PLL on
 * \ingroup group_tal_state_machine_230b
 */
static void switch_pll_on(void)
{
	trx_irq_reason_t irq_status;
	uint32_t start_time, now;

	/* Check if trx is in TRX_OFF; only from PLL_ON the following procedure
	 *is applicable */
	if (pal_trx_bit_read(SR_TRX_STATUS) != TRX_OFF) {
		Assert(
				"Switch PLL_ON failed, because trx is not in TRX_OFF" ==
				0);
		return;
	}

	/* use the IRQ status register checking for the actual PLL status */
	pal_trx_irq_dis();
	pal_trx_reg_write(RG_IRQ_MASK, TRX_IRQ_PLL_LOCK); /* allow PLL lock IRQ
	                                                   *only*/
	pal_trx_reg_read(RG_IRQ_STATUS); /* clear PLL lock bit */

	/* Switch PLL on */
	pal_trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);

	/* Check if PLL has been locked. */
	pal_get_current_time(&start_time);
	while (1) {
		irq_status = (trx_irq_reason_t)pal_trx_reg_read(RG_IRQ_STATUS);
		if (irq_status & TRX_IRQ_PLL_LOCK) {
			break; /* PLL is locked now */
		}

		/* Check if polling needs too much time. */
		pal_get_current_time(&now);
		if (pal_sub_time_us(now,
				start_time) > (10 * PLL_LOCK_TIME_US)) {
			/* leave poll loop and throw assertion */
#if (_DEBUG_ > 0)
			Assert("PLL switch failed" == 0);
#endif
			break;
		}
	}
	pal_trx_irq_flag_clr();
	pal_trx_reg_write(RG_IRQ_MASK, TRX_IRQ_TRX_END); /* enable TRX_END
	                                                  *interrupt */

	pal_trx_irq_en();
}

#ifdef ENABLE_FTN_PLL_CALIBRATION

/*
 * \brief PLL calibration and filter tuning timer callback
 *
 * \param parameter Unused callback parameter
 */
void calibration_timer_handler_cb(void *parameter)
{
	retval_t timer_status;

	handle_ftn_pll_calibration();

	/* Restart periodic calibration timer again.*/
	timer_status = pal_timer_start(TAL_CALIBRATION,
			TAL_CALIBRATION_TIMEOUT_US,
			TIMEOUT_RELATIVE,
			(FUNC_PTR)calibration_timer_handler_cb,
			NULL);

	if (timer_status != MAC_SUCCESS) {
		Assert("PLL calibration timer start problem" == 0);
	}

	parameter = parameter; /* Keep compiler happy. */
}

/**
 * \brief Filter tuning calibration implementation
 */
static void do_ftn_calibration(void)
{
	pal_trx_bit_write(SR_FTN_START, 1);
	/* Wait tTR16 (FTN calibration time). */
	pal_timer_delay(25);
}

/**
 * \brief Filter tuning and PLL calibration handling
 */
static void handle_ftn_pll_calibration(void)
{
	if (TRX_SLEEP == tal_trx_status) {
		/*
		 * Filter tuning:
		 * If we are currently sleeping, there is nothing to be done,
		 * since we are going to do this automatically when waking up.
		 */

		/*
		 * PLL calibration:
		 * Do nothing, because the PLL is calibrated during a state
		 *change from
		 * state TRX_OFF to any of the PLL_ACTIVE state
		 * (RX_ON, PLL_ON, TX_ARET_ON, RX_AACK_ON).
		 *
		 * So whenever the radio is woken up is goes into TRX_OFF first.
		 * Later from TRX_OFF we always go via one of the above states
		 *when the
		 * transceiver shall be used actively.
		 */
	} else if (TRX_OFF == tal_trx_status) {
		/* Filter tuning */
		do_ftn_calibration();

		/*
		 * PLL calibration:
		 * Do nothing, because the PLL is calibrated during a state
		 *change from
		 * state TRX_OFF to any of the PLL_ACTIVE state
		 * (RX_ON, PLL_ON, TX_ARET_ON, RX_AACK_ON).
		 *
		 * From TRX_OFF we always go via one of the above states when
		 *the
		 * transceiver shall be used actively.
		 */
	} else {
		/* Same for both filter tuning and PLL calibration. */
		do {
			/*
			 * Set TRX_OFF until it could be set.
			 * The trx might be busy.
			 */
		} while (set_trx_state(CMD_TRX_OFF) != TRX_OFF);

		do_ftn_calibration();

		/* Switch back to standard state. */
#if (defined PROMISCUOUS_MODE)
		if (tal_pib.PromiscuousMode) {
			set_trx_state(CMD_RX_ON);
		} else {
			set_trx_state(CMD_RX_AACK_ON);
		}

#elif (defined SNIFFER)
		set_trx_state(CMD_RX_ON);
#else   /* Normal operation */
		set_trx_state(CMD_RX_AACK_ON);
#endif
	}
}

#endif  /* ENABLE_FTN_PLL_CALIBRATION */

/**
 * @brief Conversion of symbols to microseconds
 */
uint32_t tal_convert_symbols_to_us_def(uint32_t symbols)
{
	return (TAL_CONVERT_SYMBOLS_TO_US(symbols));
}

/**
 * @brief Conversion of microseconds to symbols
 */
uint32_t tal_convert_us_to_symbols_def(uint32_t time)
{
	return (TAL_CONVERT_US_TO_SYMBOLS(time));
}

/* EOF */
