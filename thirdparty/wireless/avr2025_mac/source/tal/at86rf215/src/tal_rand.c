/**
 * @file tal_rand.c
 *
 * @brief This file implements the random seed function.
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
#include "tal_config.h"
#include "tal_internal.h"

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Generates a 16-bit random number used as initial seed for srand()
 *
 * This function generates a 16-bit random number by means of using the
 * Random Number Generator from the transceiver.
 *
 * The generated random 16-bit number is feed into function srand()
 * as initial seed.
 */
retval_t tal_generate_rand_seed(void)
{
	trx_id_t trx_id;
	rf_cmd_state_t previous_trx_state = RF_NOP;

	/* Check for non sleeping device */
	if ((tal_state[RF09] == TAL_SLEEP) && (tal_state[RF24] == TAL_SLEEP)) {
		return TAL_TRX_ASLEEP;
	}

	/* Find a trx that is idle */
	if (tal_state[RF09] == TAL_IDLE) {
		trx_id = RF09;
	} else {
		if (tal_state[RF24] == TAL_IDLE) {
			trx_id = RF24;
		} else {
			return TAL_BUSY;
		}
	}

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	/* Set widest filter bandwidth and set IF shift */
	uint8_t previous_bwc;
#ifdef IQ_RADIO
	previous_bwc = trx_reg_read(RF215_RF, reg_offset + RG_RF09_RXBWC);
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_RXBWC, 0x1B);
#else
	previous_bwc = trx_reg_read( reg_offset + RG_RF09_RXBWC);
	trx_reg_write( reg_offset + RG_RF09_RXBWC, 0x1B);
#endif

	/* Ensure that transceiver is not in off mode */
	if (trx_state[trx_id] == RF_TRXOFF) {
		previous_trx_state = RF_TRXOFF;

		/* Disable BB to avoid receiving any frame */
		trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 0);

		switch_to_txprep(trx_id);

		/*
		 * Enter Rx state
		 * Use direct register access to change to Rx state, since no
		 *buffer is
		 * required.
		 */
		trx_bit_write( reg_offset + SR_RF09_CMD_CMD, RF_RX);
#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, reg_offset + SR_RF09_CMD_CMD, RF_RX);
#endif
		trx_state[trx_id] = RF_RX;

		pal_timer_delay(10); /* Allow frontend to settle */
	} else {
	}

	uint16_t seed;
	for (uint8_t i = 0; i < 2; i++) {
		seed = trx_reg_read(reg_offset + RG_RF09_RNDV);
		seed |= (uint16_t)trx_reg_read(reg_offset + RG_RF09_RNDV) << 8;
	}

	/* Restore previous transceiver state */
	if (previous_trx_state == RF_TRXOFF) {
#ifdef IQ_RADIO
		trx_bit_write(RF215_BB, reg_offset + SR_RF09_CMD_CMD,
				RF_TRXOFF);
		trx_bit_write(RF215_RF, reg_offset + SR_RF09_CMD_CMD,
				RF_TRXOFF);
		trx_state[trx_id] = RF_TRXOFF;
		/* Enable BB again */
		trx_bit_write(RF215_BB, reg_offset + SR_BBC0_PC_BBEN, 1);
#else
		trx_bit_write( reg_offset + SR_RF09_CMD_CMD, RF_TRXOFF);
		trx_state[trx_id] = RF_TRXOFF;
		/* Enable BB again */
		trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 1);
#endif
	}

	/* Restore previous filter bandwidth setting */
#ifdef IQ_RADIO
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_RXBWC, previous_bwc);
#else
	trx_reg_write( reg_offset + RG_RF09_RXBWC, previous_bwc);
#endif

	/* Set the seed for the random number generator. */
	srand(seed);

	return MAC_SUCCESS;
}

/* EOF */
