/**
 * @file tal_ed.c
 *
 * @brief This file implements ED scan
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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_config.h"
#include "tal_internal.h"
#include "mac_build_config.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * Values used for ED scaling in dBm
 */
#define UPPER_ED_LIMIT      -30
#define LOWER_ED_LIMIT      -100

/* === GLOBALS ============================================================= */

/**
 * The peak_ed_level is the maximum ED value received from the transceiver for
 * the specified Scan Duration.
 */
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
static int8_t max_ed_level[NUM_TRX];
static uint32_t sampler_counter[NUM_TRX];
#endif

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Starts ED Scan
 *
 * This function starts an ED Scan for the scan duration specified by the
 * MAC layer. The result is returned from the TAL by calling tal_ed_end_cb().
 *
 * @param trx_id Transceiver identifier
 * @param scan_duration Specifies the ED scan duration in superframe duration
 *
 * @return MAC_SUCCESS - ED scan duration timer started successfully
 *         TAL_BUSY - TAL is busy servicing the previous request from MAC
 *         TAL_TRX_ASLEEP - Transceiver is currently sleeping
 *         FAILURE otherwise
 */
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1) || (defined DOXYGEN)
retval_t tal_ed_start(trx_id_t trx_id, uint8_t scan_duration)
{
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	uint16_t sample_duration;
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	/*
	 * Check if the TAL is in idle state. Only in idle state it can
	 * accept and ED request from the MAC.
	 */
	if (tal_state[trx_id] == TAL_SLEEP) {
		return TAL_TRX_ASLEEP;
	}

	if (TAL_IDLE != tal_state[trx_id]) {
		Assert("TAL is TAL_BUSY" == 0);
		return TAL_BUSY;
	}

	max_ed_level[trx_id] = -127; /* set to min value */

	/* Store TRX state before entering Tx transaction */
	if ((trx_state[trx_id] == RF_RX) || (trx_state[trx_id] == RF_TXPREP)) {
		trx_default_state[trx_id] = RF_RX;
	}

	switch_to_txprep(trx_id);

	/* Disable BB */
	trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 0);

	/* Setup and start energy detection, ensure AGC is not hold */
	trx_bit_write( reg_offset + SR_RF09_AGCC_FRZC, 0);

	/* Setup energy measurement averaging duration */
	sample_duration = ED_SAMPLE_DURATION_SYM *
			tal_pib[trx_id].SymbolDuration_us;
	set_ed_sample_duration(trx_id, sample_duration);
#ifndef BASIC_MODE
	/* Enable EDC IRQ */
	trx_bit_write( reg_offset + SR_RF09_IRQM_EDC, 1);
#endif
	/* Calculate the number of samples */
	sampler_counter[trx_id] = aBaseSuperframeDuration
			* ((1UL << scan_duration) + 1);
	sampler_counter[trx_id] = sampler_counter[trx_id] /
			ED_SAMPLE_DURATION_SYM;
#ifdef REDUCED_ED_SAMPLE_COUNTER /* used for testing */
	/* used for debugging purposes only */
	sampler_counter[trx_id] = REDUCED_ED_SAMPLE_COUNTER;
#endif

	/* Set RF to Rx */
#ifdef IQ_RADIO
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_RX);
#else
	trx_reg_write( reg_offset + RG_RF09_CMD, RF_RX);
#endif
	trx_state[trx_id] = RF_RX;
	pal_timer_delay(tal_pib[trx_id].agc_settle_dur); /* allow filters to
	                                                  * settle */

	tal_state[trx_id] = TAL_ED_SCAN;
	/* Start energy measurement */
	trx_bit_write( reg_offset + SR_RF09_EDC_EDM, RF_EDCONT);

	return MAC_SUCCESS;
}

#endif /* #if (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/**
 * @brief Handles ED scan end interrupt
 *
 * This function handles an ED done interrupt from the transceiver.
 *
 * @param trx_id Transceiver identifier
 */
void handle_ed_end_irq(trx_id_t trx_id)
{
	/* Capture ED value for current frame / ED scan */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
#ifdef IQ_RADIO
	tal_current_ed_val[trx_id] = trx_reg_read(reg_offset + RG_RF09_EDV);
#else
	tal_current_ed_val[trx_id] = trx_reg_read(reg_offset + RG_RF09_EDV);
#endif

#if (defined SUPPORT_MODE_SWITCH) || (defined BASIC_MODE)
	if (tx_state[trx_id] == TX_CCA) {
		cca_done_handling(trx_id);
		return;
	}

#endif

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	if (tal_state[trx_id] == TAL_ED_SCAN) {
		/*
		 * Update the peak ED value received, if greater than the
		 *previously
		 * read ED value.
		 */
		if (tal_current_ed_val[trx_id] > max_ed_level[trx_id]) {
			max_ed_level[trx_id] = tal_current_ed_val[trx_id];
		}

		sampler_counter[trx_id]--;
		if (sampler_counter[trx_id] == 0) {
			/* Keep RF in Rx state */
			/* Stop continuous energy detection */
#ifdef IQ_RADIO
			trx_bit_write(RF215_RF, reg_offset + SR_RF09_EDC_EDM,
					RF_EDAUTO);
#else
			trx_bit_write( reg_offset + SR_RF09_EDC_EDM, RF_EDAUTO);
#endif
			/* Restore ED average duration for CCA */
			set_ed_sample_duration(trx_id,
					tal_pib[trx_id].CCADuration_us);
			/* Switch BB on again */
			trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 1);
			tal_state[trx_id] = TAL_IDLE;
			/* Set trx state for leaving ED scan */
			if (trx_default_state[trx_id] == RF_RX) {
				switch_to_rx(trx_id);
			} else {
#ifdef IQ_RADIO
				trx_reg_write(RF215_RF,
						reg_offset + RG_RF09_CMD,
						RF_TRXOFF);
#else
				trx_reg_write( reg_offset + RG_RF09_CMD,
						RF_TRXOFF);
#endif
			}

			/* Scale result to 0xFF */
			uint8_t ed = max_ed_level[trx_id]; /*scale_ed_value(max_ed_level[trx_id]);*/
#ifndef BASIC_MODE
			/* Disable EDC IRQ again */
			trx_bit_write(reg_offset + SR_RF09_IRQM_EDC, 0);
#endif
			tal_ed_end_cb(trx_id, ed);
		} else {
			/* Wait for the next interrupt */
		}
	}

#endif /* #if (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */
}

/**
 * @brief Sets the energy measurement duration
 *
 * @param trx_id Transceiver identifier
 * @param sample_duration_us Sample duration in us
 */
void set_ed_sample_duration(trx_id_t trx_id, uint16_t sample_duration_us)
{
	uint8_t dtb;
	uint8_t df;

	if ((sample_duration_us % 128) == 0) {
		dtb = 3;
		df = sample_duration_us / 128;
	} else if ((sample_duration_us % 32) == 0) {
		dtb = 2;
		df = sample_duration_us / 32;
	} else if ((sample_duration_us % 8) == 0) {
		dtb = 1;
		df = sample_duration_us / 8;
	} else {
		dtb = 0;
		df = sample_duration_us / 2;
	}

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
#ifdef IQ_RADIO
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_EDD, ((df << 2) | dtb));
#else
	trx_reg_write( reg_offset + RG_RF09_EDD, ((df << 2) | dtb));
#endif
}

/**
 * @brief Scale ED value
 *
 * This function scales the trx ED value to the range 0x00 - 0xFF.
 *
 * @param ed RF215 register value EDV.
 *
 * @return Scaled ED value
 */
uint8_t scale_ed_value(int8_t ed)
{
	uint8_t result;
	if (ed == 127) {
		result = 0x00;
	} else if (ed >= UPPER_ED_LIMIT) {
		result = 0xFF;
	} else if (ed <= LOWER_ED_LIMIT) {
		result = 0x00;
	} else {
		float temp = (ed - LOWER_ED_LIMIT) * (float)0xFF /
				(float)(UPPER_ED_LIMIT - LOWER_ED_LIMIT);
		result = (uint8_t)temp;
	}

	return result;
}

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)

/**
 * @brief Stops ED Scan
 *
 * This function stops an ED Scan and completes it by calling tal_ed_end_cb().
 *
 * @param trx_id Transceiver identifier
 */
void stop_ed_scan(trx_id_t trx_id)
{
	/* Stop continuous energy detection */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
#ifdef IQ_RADIO
	trx_bit_write(RF215_RF, reg_offset + SR_RF09_EDC_EDM, RF_EDAUTO);
#else
	trx_bit_write( reg_offset + SR_RF09_EDC_EDM, RF_EDAUTO);
#endif
	sampler_counter[trx_id] = 0;
	/* Clear any pending ED IRQ */
	TAL_RF_IRQ_CLR(trx_id, RF_IRQ_EDC);
	handle_ed_end_irq(trx_id);
}

#endif

/* EOF */
