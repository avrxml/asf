/**
 * @file tal_rand.c
 *
 * @brief This file implements the random seed function.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 */

/*
 * Copyright (c) 2015-2018, Microchip Technology Inc All rights reserved.
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
