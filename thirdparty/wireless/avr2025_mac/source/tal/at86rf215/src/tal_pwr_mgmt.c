/**
 * @file tal_pwr_mgmt.c
 *
 * @brief This file implements TAL power management functionality of
 *        the transceiver.
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

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "pal.h"
#include "return_val.h"
#include "ieee_const.h"
#include "tal.h"
#include "tal_config.h"
#include "tal_internal.h"
#include "tal_pib.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Sets the transceiver to SLEEP
 *
 * This function sets the transceiver to state SLEEP.
 *
 * @param trx_id Transceiver identifier
 *
 * @return
 *      - @ref TAL_BUSY - The transceiver is busy in TX or RX
 *      - @ref MAC_SUCCESS - The transceiver is put to sleep
 *      - @ref TAL_TRX_ASLEEP - The transceiver is already asleep
 *      - @ref MAC_INVALID_PARAMETER - The specified sleep mode is not supported
 * @ingroup apiTalApi
 */
retval_t tal_trx_sleep(trx_id_t trx_id)
{
	if (tal_state[trx_id] == TAL_SLEEP) {
		return TAL_TRX_ASLEEP;
	}

	/* Device can be put to sleep only when the TAL is in IDLE state. */
	if (tal_state[trx_id] != TAL_IDLE) {
		return TAL_BUSY;
	}

	tal_state[trx_id] = TAL_SLEEP;

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_reg_write(reg_offset + RG_RF09_CMD, RF_TRXOFF);
#ifdef IQ_RADIO
	pal_dev_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
	trx_reg_write(reg_offset + RG_RF09_CMD, RF_SLEEP);
#ifdef IQ_RADIO
	pal_dev_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_SLEEP);
#endif
	TAL_BB_IRQ_CLR_ALL(trx_id);
	TAL_RF_IRQ_CLR_ALL(trx_id);
	trx_state[trx_id] = RF_SLEEP;

	/*
	 * Free TAL Rx buffer. During sleep no buffer is required.
	 * With tal_trx_wakeup() a new buffer gets allocated.
	 */
	bmm_buffer_free(tal_rx_buffer[trx_id]);
	tal_rx_buffer[trx_id] = NULL;
	tal_buf_shortage[trx_id] = false;

#ifdef ENABLE_FTN_PLL_CALIBRATION
	stop_ftn_timer(trx_id);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */

	return MAC_SUCCESS;
}

/**
 * @brief Wakes up the transceiver from SLEEP
 *
 * This function awakes the transceiver from state SLEEP.
 *
 * @param trx_id Transceiver identifier
 *
 * @return
 *      - @ref TAL_TRX_AWAKE - The transceiver is already awake
 *      - @ref MAC_SUCCESS - The transceiver is woken up from sleep
 *      - @ref FAILURE - The transceiver did not wake-up from sleep
 * @ingroup apiTalApi
 */
retval_t tal_trx_wakeup(trx_id_t trx_id)
{
	if (tal_state[trx_id] != TAL_SLEEP) {
		return TAL_TRX_AWAKE;
	}

	tal_state[trx_id] = TAL_WAKING_UP;

	/* Check if other trx is sleep too. */
	uint8_t other_trx_id;
	if (trx_id == RF09) {
		other_trx_id = RF24;
	} else {
		other_trx_id = RF09;
	}

	if (tal_state[other_trx_id] == TAL_SLEEP) {
		tal_state[other_trx_id] = TAL_WAKING_UP;
	}

	/* Write command to wake device up */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_reg_write(reg_offset + RG_RF09_CMD, RF_TRXOFF);
#ifdef IQ_RADIO
	pal_dev_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
	trx_state[trx_id] = RF_TRXOFF;

	/* Wait for transceiver wakeup */
	uint32_t start_time;
	uint32_t current_time;
	pal_get_current_time(&start_time);
	pal_get_current_time(&current_time);
	while (1) {
		if (TAL_RF_IS_IRQ_SET(trx_id, RF_IRQ_WAKEUP)) {
			TAL_RF_IRQ_CLR(trx_id, RF_IRQ_WAKEUP);

			if (tal_state[other_trx_id] == TAL_WAKING_UP) {
				/* Wait for the other trx to wake-up as well. */
				if (TAL_RF_IS_IRQ_SET(other_trx_id,
						RF_IRQ_WAKEUP)) {
					TAL_RF_IRQ_CLR(other_trx_id,
							RF_IRQ_WAKEUP);
					break;
				}
			} else {
				break;
			}
		}

		pal_get_current_time(&current_time);
		/* @ToDo: Use no magic number for "1000" */
		if (pal_sub_time_us(current_time, start_time) > 1000) {
			tal_state[trx_id] = TAL_SLEEP;
			if (tal_state[other_trx_id] == TAL_WAKING_UP) {
				tal_state[other_trx_id] = TAL_SLEEP;
			}

			return FAILURE;
		}
	}

	/*
	 * If the other transceiver block was in SLEEP mode, enable SLEEP for
	 *that
	 * block again.
	 */
	if (tal_state[other_trx_id] == TAL_WAKING_UP) {
		uint16_t other_reg_offset = RF_BASE_ADDR_OFFSET * other_trx_id;
		trx_reg_write(other_reg_offset + RG_RF09_CMD, RF_SLEEP);
#ifdef IQ_RADIO
		pal_dev_reg_write(RF215_RF, other_reg_offset + RG_RF09_CMD,
				RF_SLEEP);
#endif
		tal_state[other_trx_id] = TAL_SLEEP;
		TAL_RF_IRQ_CLR_ALL(other_trx_id);
		TAL_RF_IRQ_CLR_ALL(other_trx_id);
	}

	trx_config(trx_id); /* see tal_init.c */
	write_all_tal_pib_to_trx(trx_id); /* see 'tal_pib.c' */
	config_phy(trx_id);

	/*
	 * Allocate a new buffer
	 * The previous buffer got freed with entering sleep mode.
	 */
	tal_rx_buffer[trx_id] = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	tal_state[trx_id] = TAL_IDLE;

	return MAC_SUCCESS;
}

/* EOF */
