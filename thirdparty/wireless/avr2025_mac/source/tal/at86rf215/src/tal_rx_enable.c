/**
 * @file tal_rx_enable.c
 *
 * @brief This file provides Functionalities supporting RX-Enable feature
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
#include <string.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_internal.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Switches receiver on or off
 *
 * This function switches the receiver on (PHY_RX_ON) or off (PHY_TRX_OFF).
 *
 * @param trx_id Transceiver identifier
 * @param state New state of receiver
 *
 * @return
 *      - @ref TAL_BUSY if the TAL state machine cannot switch receiver on or
 *off,
 *      - @ref PHY_TRX_OFF if receiver has been switched off, or
 *      - @ref PHY_RX_ON otherwise.
 *
 * @ingroup apiTalApi
 */
uint8_t tal_rx_enable(trx_id_t trx_id, uint8_t state)
{
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	uint8_t ret_val;
	if (tal_state[trx_id] == TAL_SLEEP) {
		return TAL_TRX_ASLEEP;
	}

	/*
	 * Trx can only be enabled if TAL is not busy;
	 * i.e. if TAL is IDLE.
	 */
	if (tal_state[trx_id] != TAL_IDLE) {
		return TAL_BUSY;
	}

	/*
	 * Check current state
	 */
	if ((state == PHY_TRX_OFF) && (trx_state[trx_id] == RF_TRXOFF)) {
		return PHY_TRX_OFF;
	}

	if ((state == PHY_RX_ON) && (trx_state[trx_id] == RF_RX)) {
		return PHY_RX_ON;
	}

	if (state == PHY_TRX_OFF) {
		/*
		 * If the rx needs to be switched off,
		 * we are not interested in a frame that is currently being
		 *received.
		 */
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
		stop_rpc(trx_id);
#endif
		trx_state[trx_id] = RF_TRXOFF;
		tal_buf_shortage[trx_id] = false;
		ret_val = PHY_TRX_OFF;
#ifdef ENABLE_FTN_PLL_CALIBRATION
		stop_ftn_timer(trx_id);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
		trx_default_state[trx_id] = RF_TRXOFF;
	} else {
		switch_to_txprep(trx_id);
		switch_to_rx(trx_id);
#if (defined SUPPORT_FSK) || (defined SUPPORT_OQPSK)
		start_rpc(trx_id);
#endif
		ret_val = PHY_RX_ON;
#ifdef ENABLE_FTN_PLL_CALIBRATION

		/*
		 * Start the FTN timer.
		 *
		 * Since we do not have control when tal_rx_enable is
		 * from the NHLE, and this timer is running for quite some
		 * time, we first need to check, if this timer has not been
		 *started
		 * been already. In this case we leave it running.
		 * Otherwise this would lead to Assertions.
		 */
		uint8_t timer_id;
		if (trx_id == RF09) {
			timer_id = TAL_T_CALIBRATION_0;
		} else {
			timer_id = TAL_T_CALIBRATION_1;
		}

		if (!pal_is_timer_running(timer_id)) {
			start_ftn_timer(trx_id);
		}
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
		trx_default_state[trx_id] = RF_RX;
	}

	return ret_val;
}

/* EOF */
