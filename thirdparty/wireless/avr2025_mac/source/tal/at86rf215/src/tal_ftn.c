/**
 * @file tal_ftn.c
 *
 * @brief This file implements the filter periodic tuning
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

/* === INCLUDES ============================================================= */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_config.h"
#include "tal_pib.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_internal.h"
#include "mac_build_config.h"

/* === TYPES ================================================================ */

/* === MACROS =============================================================== */

/*
 * PLL calibration and filter tuning timer timeout in minutes
 */
#define TAL_CALIBRATION_TIMEOUT_MIN         (30UL)

/*
 * PLL calibration and filter tuning timer timeout in us,
 */
#define TAL_CALIBRATION_TIMEOUT_US          ((TAL_CALIBRATION_TIMEOUT_MIN) * \
	(60UL) * (1000UL) * (1000UL))

/*
 * Backoff during to postpone tuning; 1s
 */
#define POSTPONE_PERIOD                     1000000

/*
 * Number of trim loops
 */
#define TRIM_LOOPS                          7

/*
 * Number of sufficient measurements
 */
#define NUM_SUFFICIENT_MEASUREMENTS         3

/*
 * Trim threshold values
 */
#define TRIM_THRESHOLD                      3

/*
 * Narrow trim threshold values
 */
#define NARROW_TRIM_THRESHOLD               2

/* === GLOBALS ============================================================== */

/* === PROTOTYPES =========================================================== */

#ifdef ENABLE_FTN_PLL_CALIBRATION
static void ftn_timer_cb(void *cb_timer_element);
static void postpone_tuning(trx_id_t trx_id);

#endif
#ifdef RF215v1
static inline uint8_t get_median(int *temp_array, uint8_t len_of_array);
static inline int compare_uin32_t(const void *f1, const void *f2);

#endif

/* === Implementation ======================================================= */

#ifdef ENABLE_FTN_PLL_CALIBRATION

/**
 * @brief Starts the filter tuning timer
 *
 * @param trx_id Transceiver identifier
 */
void start_ftn_timer(trx_id_t trx_id)
{
	/* Handle PLL calibration and filter tuning. */
	retval_t timer_status;

	/* Calibration timer has already been stopped within this function. */
	uint8_t timer_id;
	if (trx_id == RF09) {
		timer_id = TAL_T_CALIBRATION_0;
	} else {
		timer_id = TAL_T_CALIBRATION_1;
	}

	/* Start periodic calibration timer. */
	timer_status = pal_timer_start(timer_id,
			TAL_CALIBRATION_TIMEOUT_US,
			TIMEOUT_RELATIVE,
			(FUNC_PTR())ftn_timer_cb,
			(void *)trx_id);

	if (timer_status != MAC_SUCCESS) {
		Assert("PLL calibration timer start problem" == 0);
	}
}

#endif

#ifdef ENABLE_FTN_PLL_CALIBRATION

/**
 * @brief Stops the filter tuning timer
 *
 * @param trx_id Transceiver identifier
 */
void stop_ftn_timer(trx_id_t trx_id)
{
	uint8_t timer_id;

	if (trx_id == RF09) {
		timer_id = TAL_T_CALIBRATION_0;
	} else {
		timer_id = TAL_T_CALIBRATION_1;
	}

	pal_timer_stop(timer_id);
}

#endif

#ifdef ENABLE_FTN_PLL_CALIBRATION

/**
 * @brief Timer callback function for the filter tuning
 *
 * This function executes the filter tuning and restarts its timer again.
 * @param parameter Pointer to trx_id
 */
static void ftn_timer_cb(void *cb_timer_element)
{
	/* Immediately store trx id from callback. */
	trx_id_t trx_id = (trx_id_t)cb_timer_element;
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	if ((tal_state[trx_id] == TAL_IDLE) &&
			(ack_transmitting[trx_id] == false)) {
		if (trx_state[trx_id] == RF_RX) {
			uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

			/* Check if current a frame is received */
#ifdef IQ_RADIO
			if (trx_bit_read(RF215_RF, reg_offset +
					SR_RF09_AGCC_FRZS) == 1)
#else
			if (trx_bit_read(reg_offset + SR_RF09_AGCC_FRZS) == 1)
#endif
			{
				postpone_tuning(trx_id);

				/*
				 * The timer start is postponed due to ongoing
				 *transaction.
				 * No further processing required here.
				 */
				return;
			} else {
#ifdef RF215_v1
				calibrate_LO(trx_id);
#else
				trx_reg_write( reg_offset + RG_RF09_CMD,
						RF_TRXOFF);
				trx_state[trx_id] = RF_TRXOFF;
#endif
				switch_to_rx(trx_id);
			}
		} else {
			unexpected Trx state
		}

		start_ftn_timer(trx_id); /* Restart timer again */
	} else {
		postpone_tuning(trx_id);
	}
}

#endif

#ifdef ENABLE_FTN_PLL_CALIBRATION

/**
 * @brief Postpones the filter tuning by a short period
 *
 * @param trx_id Transceiver identifier
 */
static void postpone_tuning(trx_id_t trx_id)
{
	uint8_t timer_id;
	if (trx_id == RF09) {
		timer_id = TAL_T_CALIBRATION_0;
	} else {
		timer_id = TAL_T_CALIBRATION_1;
	}

	/* Postpone filter tuning, since TAL is busy */
	pal_timer_start(timer_id,
			POSTPONE_PERIOD,
			TIMEOUT_RELATIVE,
			(FUNC_PTR())ftn_timer_cb,
			(void *)trx_id);
}

#endif  /* ENABLE_FTN_PLL_CALIBRATION */

#ifdef RF215v1

/**
 * @brief Calibrate the LO value; workaround for errata reference #4807
 *
 * @param trx_id Transceiver identifier
 */
void calibrate_LO(trx_id_t trx_id)
{
	uint8_t temp[TRIM_LOOPS][2];
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	uint16_t avg[2] = {0, 0};
	bool reduced_measurements = true;
	uint8_t *ptr = (uint8_t *)temp;

	if (trx_state[trx_id] != RF_TRXOFF) {
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#else
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
		trx_state[trx_id] = RF_TRXOFF;
	}

	for (uint8_t i = 0; i < TRIM_LOOPS; i++) {
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TXPREP);
#else
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TXPREP);
#endif
		wait_for_txprep(trx_id);
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#else
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
		trx_state[trx_id] = RF_TRXOFF;
#ifdef IQ_RADIO
		trx_read(RF215_RF, reg_offset + 0x125, (uint8_t *)&temp[i][0],
				2);
#else
		trx_read( reg_offset + 0x125, (uint8_t *)&temp[i][0], 2);
#endif

		/* Check if the short loop measurement is sufficient */
		if (i == (NUM_SUFFICIENT_MEASUREMENTS - 1)) {
			uint8_t val[3];
			for (uint8_t k = 0; k < 2; k++) {
				val[0] = abs(temp[0][k] - temp[1][k]);
				val[1] = abs(temp[0][k] - temp[2][k]);
				val[2] = abs(temp[1][k] - temp[2][k]);
				for (uint8_t j = 0;
						j < NUM_SUFFICIENT_MEASUREMENTS;
						j++) {
					if (val[j] > NARROW_TRIM_THRESHOLD) {
						reduced_measurements = false;
						break;
					}
				}
				if (reduced_measurements == false) {
					break;
				}
			}
			if (reduced_measurements == true) {
				/* Do stop measuring - do no more trim loops */
				break;
			}
		}
	}

	if (reduced_measurements == true) {
		/* Round value */
		for (uint8_t i = 0; i < NUM_SUFFICIENT_MEASUREMENTS; i++) {
			avg[0] += *ptr++;
			avg[1] += *ptr++;
		}
		txc[trx_id][0]
			= (uint8_t)(((float)avg[0] /
				NUM_SUFFICIENT_MEASUREMENTS) + 0.5);
		txc[trx_id][1]
			= (uint8_t)(((float)avg[1] /
				NUM_SUFFICIENT_MEASUREMENTS) + 0.5);
	} else { /* if (reduced_measurements == false) */
		int arr[TRIM_LOOPS];
		for (uint8_t k = 0; k < 2; k++) {
			ptr = &temp[0][k];
			for (uint8_t i = 0; i < TRIM_LOOPS; i++) {
				arr[i] = *ptr;
				ptr += 2;
			}
			txc[trx_id][k] = get_median(arr, TRIM_LOOPS);
		}
	}
}

#endif /* #ifdef RF215v1 */

#ifdef RF215v1

/**
 * @brief Implements required compare function for qsort().
 */
static inline int compare_uin32_t(const void *f1, const void *f2)
{
	return (*(int *)f1 - *(int *)f2);
}

#endif /* #ifdef RF215v1 */

#ifdef RF215v1

/**
 * @brief Gets a median value
 */
static inline uint8_t get_median(int *temp_array, uint8_t len_of_array)
{
	uint8_t idx = len_of_array / 2;

	/* First sort the array. */
	qsort(temp_array, len_of_array, sizeof(uint8_t), compare_uin32_t);

	/* Get actual median value. */
	if (len_of_array % 2) {
		/*
		 * Odd number of values, so the median is the
		 * value in the middle of the sorted array.
		 */
		return temp_array[idx];
	} else {
		/*
		 * Even number of values, so the median is to be calculated.
		 */
		uint32_t temp = temp_array[idx - 1] + temp_array[idx];
		return (temp / 2);
	}
}

#endif /* #ifdef RF215v1 */

/* EOF */
