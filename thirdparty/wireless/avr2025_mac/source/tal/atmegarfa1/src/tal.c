/**
 * @file tal.c
 *
 * @brief This file implements the TAL state machine and provides general
 * functionality used by the TAL.
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
 *
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "tal_irq_handler.h"
#include "atmega128rfa1.h"
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

/*
 * Value used for checking proper locking of PLL during switch from
 * TRX_PFF to PLL_ON.
 */
#define PLL_LOCK_ATTEMPTS           (3)

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
 * \ingroup group_tal_state_machine_rfa1
 */
tal_state_t tal_state;

/**
 * Current state of the transceiver.
 * \ingroup group_tal_state_machine_rfa1
 */
tal_trx_status_t tal_trx_status;

/**
 * Indicates if the transceiver needs to switch on its receiver by tal_task(),
 * because it could not be switched on due to buffer shortage.
 * \ingroup group_tal_state_machine_rfa1
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

/* Flag indicating awake end irq at successful wake-up from sleep. */
volatile bool tal_awake_end_flag;

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)

/**
 * Timestamp
 * The timestamping is only required for beaconing networks
 * or if timestamping is explicitly enabled.
 */
uint32_t tal_rx_timestamp;
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

#ifdef BEACON_SUPPORT

/**
 * CSMA state machine variable
 */
csma_state_t tal_csma_state;
#endif  /* BEACON_SUPPORT */

#if ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT))

/*
 * Flag indicating if beacon transmission is currently in progress.
 */
bool tal_beacon_transmission;
#endif /* ((MAC_START_REQUEST_CONFIRM == 1) && (defined BEACON_SUPPORT)) */

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
			 * switched on.
			 */
			tal_rx_on_required = false;

			/*
			 * Release the protected buffer and set it again for
			 * further protection
			 * since the buffer is available now.
			 */
			trx_bit_write(SR_RX_SAFE_MODE, RX_SAFE_MODE_DISABLE); /*
			                                                       *
			                                                       *
			                                                       *Disable
			                                                       *
			                                                       *
			                                                       *buffer
			                                                       *
			                                                       *
			                                                       *protection
			                                                       *
			                                                       *
			                                                       *mode
			                                                       **/
			pal_timer_delay(2); /* Allow pin change to get effective
			                    **/
			trx_bit_write(SR_RX_SAFE_MODE, RX_SAFE_MODE_ENABLE); /*
			                                                      *
			                                                      *
			                                                      *Enable
			                                                      *
			                                                      *
			                                                      *buffer
			                                                      *
			                                                      *
			                                                      *protection
			                                                      *
			                                                      *
			                                                      *mode
			                                                      **/

			CONF_REG_WRITE();

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
		 * incoming_frame_queue. */
		rx_frame = qmm_queue_remove(&tal_incoming_frame_queue, NULL);
		if (NULL != rx_frame) {
			process_incoming_frame(rx_frame);
		}
	}

	/* Handle the TAL state machines */
	switch (tal_state) {
	case TAL_IDLE:
	/* Do nothing, but fall through... */
	case TAL_TX_AUTO:

		/* Wait until state is changed to TAL_TX_DONE inside tx end ISR
		**/
		break;

	case TAL_TX_DONE:
		tx_done_handling(); /* see tal_tx.c */
		break;

#ifdef BEACON_SUPPORT
	case TAL_SLOTTED_CSMA:
		slotted_csma_state_handling(); /* see tal_slotted_csma.c */
		break;
#endif  /* BEACON_SUPPORT */

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case TAL_ED_RUNNING:
		/* Do nothing here. Wait until ED is completed. */
		break;

	case TAL_ED_DONE:
		ed_scan_done();
		break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */
	default:
		Assert("tal_state is not handled" == 0);
		break;
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
		/*
		 * Since the wake-up procedure relies on the Awake IRQ and
		 * the global interrupts may be disabled at this point of time,
		 * we need to make sure that the global interrupts are enabled
		 * during wake-up procedure.
		 * Once the TRX is awake, the original state of the global
		 * interrupts
		 * will be restored.
		 */
		/* Reset wake-up interrupt flag. */
		if (CMD_SLEEP == trx_cmd) {
			return TRX_SLEEP;
		}

		tal_awake_end_flag = false;
		/* Set callback function for the awake interrupt. */
		pal_trx_irq_init_awake((FUNC_PTR)trx_awake_handler_cb);
		/* Save current state of global interrupts. */
		ENTER_CRITICAL_REGION();
		/* Force enabling of global interrupts. */
		ENABLE_GLOBAL_IRQ();
		/* Leave trx sleep mode. */
		TRX_SLP_TR_LOW();
		/* Poll wake-up interrupt flag until set within ISR. */
		while (!tal_awake_end_flag) {
		}
		/* Restore original state of global interrupts. */
		LEAVE_CRITICAL_REGION();
		/* Clear existing interrupts */
		trx_reg_write(RG_IRQ_STATUS, 0xFF);
		/* Re-enable TRX_END interrupt */
		trx_reg_write(RG_IRQ_MASK, TRX_IRQ_DEFAULT);

#if (ANTENNA_DIVERSITY == 1)
		/* Enable antenna diversity. */
		trx_bit_write(SR_ANT_EXT_SW_EN, ANT_EXT_SW_ENABLE);
#endif

		if ((trx_cmd == CMD_TRX_OFF) ||
				(trx_cmd == CMD_FORCE_TRX_OFF)) {
			tal_trx_status = TRX_OFF;
			return TRX_OFF;
		}
	}

	switch (trx_cmd) { /* requested state */
	case CMD_SLEEP:
		trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
#if (ANTENNA_DIVERSITY == 1)
		/* Disable antenna diversity: sets pulls */
		trx_bit_write(SR_ANT_EXT_SW_EN, ANT_EXT_SW_DISABLE);
#endif
		{
			uint16_t rand_value;

			/*
			 * Init the SEED value of the CSMA backoff algorithm.
			 */
			rand_value = (uint16_t)rand();
			trx_reg_write(RG_CSMA_SEED_0, (uint8_t)rand_value);
			trx_bit_write(SR_CSMA_SEED_1,
					(uint8_t)(rand_value >> 8));
		}

		/*
		 * Clear existing interrupts to have clear interrupt flags
		 * during wake-up.
		 */
		trx_reg_write(RG_IRQ_STATUS, 0xFF);

		/*
		 * Enable Awake_end interrupt.
		 * This is used for save wake-up from sleep later.
		 */
		trx_reg_write(RG_IRQ_MASK, TRX_IRQ_AWAKE_ONLY);

		PAL_WAIT_1_US();
		TRX_SLP_TR_HIGH();
		pal_timer_delay(TRX_OFF_TO_SLEEP_TIME_CLKM_CYCLES);
		tal_trx_status = TRX_SLEEP;
		return TRX_SLEEP; /* transceiver register cannot be read during
		                   * TRX_SLEEP */

	case CMD_TRX_OFF:
		switch (tal_trx_status) {
		case TRX_OFF:
			break;

		default:
			trx_reg_write(RG_TRX_STATE, CMD_TRX_OFF);
			PAL_WAIT_1_US();
			break;
		}
		break;

	case CMD_FORCE_TRX_OFF:
		switch (tal_trx_status) {
		case TRX_OFF:
			break;

		default:
			trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
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
			trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
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

	case CMD_FORCE_PLL_ON:
		switch (tal_trx_status) {
		case TRX_OFF:
			switch_pll_on();
			break;

		case PLL_ON:
			break;

		default:
			trx_reg_write(RG_TRX_STATE, CMD_FORCE_PLL_ON);
			break;
		}
		break;

	case CMD_RX_ON:
		switch (tal_trx_status) {
		case RX_ON:
			break;

		case PLL_ON:
		case RX_AACK_ON:
		case TX_ARET_ON:
			trx_reg_write(RG_TRX_STATE, CMD_RX_ON);
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on();
			trx_reg_write(RG_TRX_STATE, CMD_RX_ON);
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

		case TX_ARET_ON:
		case PLL_ON:
			trx_reg_write(RG_TRX_STATE, CMD_RX_AACK_ON);
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on(); /* state change from TRX_OFF to
			                  * RX_AACK_ON can be done directly, too
			                  **/
			trx_reg_write(RG_TRX_STATE, CMD_RX_AACK_ON);
			PAL_WAIT_1_US();
			break;

		case RX_ON:
			trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			PAL_WAIT_1_US();
			/* check if state change could be applied */
			tal_trx_status = (tal_trx_status_t)trx_bit_read(
					SR_TRX_STATUS);
			if (tal_trx_status != PLL_ON) {
				return tal_trx_status;
			}

			trx_reg_write(RG_TRX_STATE, CMD_RX_AACK_ON);
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
			trx_reg_write(RG_TRX_STATE, CMD_TX_ARET_ON);
			PAL_WAIT_1_US();
			break;

		case RX_ON:
		case RX_AACK_ON:
			trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
			PAL_WAIT_1_US();
			/* check if state change could be applied */
			tal_trx_status = (tal_trx_status_t)trx_bit_read(
					SR_TRX_STATUS);
			if (tal_trx_status != PLL_ON) {
				return tal_trx_status;
			}

			trx_reg_write(RG_TRX_STATE, CMD_TX_ARET_ON);
			PAL_WAIT_1_US();
			break;

		case TRX_OFF:
			switch_pll_on(); /* state change from TRX_OFF to
			                  * TX_ARET_ON can be done directly, too
			                  **/
			trx_reg_write(RG_TRX_STATE, CMD_TX_ARET_ON);
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

	default:
		/* CMD_NOP, CMD_TX_START */
		Assert("trx command not handled" == 0);
		break;
	}

	do {
		tal_trx_status = (tal_trx_status_t)trx_bit_read(SR_TRX_STATUS);
	} while (tal_trx_status == STATE_TRANSITION_IN_PROGRESS);

	return tal_trx_status;
} /* set_trx_state() */

/**
 * \brief Switches the PLL on
 * \ingroup group_tal_state_machine_rfa1
 */
static void switch_pll_on(void)
{
	trx_irq_reason_t irq_status;
	uint8_t poll_counter = 0;

	/* Check if trx is in TRX_OFF; only from PLL_ON the following procedure
	 * is applicable */
	if (trx_bit_read(SR_TRX_STATUS) != TRX_OFF) {
		Assert(
				"Switch PLL_ON failed, because trx is not in TRX_OFF" ==
				0);
		return;
	}

	IRQ_STATUS = _BV(PLL_LOCK); /* clear PLL lock bit */
	/* Switch PLL on */
	trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);

	/* Check if PLL has been locked. */
	do {
		irq_status = (trx_irq_reason_t)trx_reg_read(RG_IRQ_STATUS);

		if (irq_status & TRX_IRQ_PLL_LOCK) {
			return; /* PLL is locked now */
		}

		/* Wait a time interval of typical value for timer TR4. */
		pal_timer_delay(TRX_OFF_TO_PLL_ON_TIME_US);

		poll_counter++;
	} while (poll_counter < PLL_LOCK_ATTEMPTS);

#if (_DEBUG_ > 0)
	Assert("PLL switch failed" == 0);
	app_alert();
#endif
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
	trx_bit_write(SR_FTN_START, 1);
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
		 * change from
		 * state TRX_OFF to any of the PLL_ACTIVE state
		 * (RX_ON, PLL_ON, TX_ARET_ON, RX_AACK_ON).
		 *
		 * So whenever the radio is woken up is goes into TRX_OFF first.
		 * Later from TRX_OFF we always go via one of the above states
		 * when the
		 * transceiver shall be used actively.
		 */
	} else if (TRX_OFF == tal_trx_status) {
		/* Filter tuning */
		do_ftn_calibration();

		/*
		 * PLL calibration:
		 * Do nothing, because the PLL is calibrated during a state
		 * change from
		 * state TRX_OFF to any of the PLL_ACTIVE state
		 * (RX_ON, PLL_ON, TX_ARET_ON, RX_AACK_ON).
		 *
		 * From TRX_OFF we always go via one of the above states when
		 * the
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
