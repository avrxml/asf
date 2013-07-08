/**
 * @file tal_ed.c
 *
 * @brief This file implements ED Scan
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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_constants.h"
#include "at86rf230b.h"
#include "tal_internal.h"
#include "mac_build_config.h"

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * \addtogroup group_tal_ed_230b
 * @{
 */

/* Constant define for the ED scaling: register value at -35dBm */
#define CLIP_VALUE_REG                  (56)

/*
 * Scan duration formula: \f$aBaseSuperframeDuration (2^SD + 1)\f$
 * where \f$0 <= SD <= 14\f$
 */
#define CALCULATE_SYMBOL_TIME_SCAN_DURATION(SD)	\
	(aBaseSuperframeDuration * ((1UL << (SD)) + 1))

/* === GLOBALS ============================================================= */

/**
 * The peak_ed_level is the maximum ED value received from the transceiver for
 * the specified Scan Duration.
 */
static uint8_t max_ed_level;

/* === PROTOTYPES ========================================================== */

static inline void initiate_ed_sampling(void);
static void ed_sampling_done_cb(void *parameter);
static void scan_duration_timer_expired_cb(void *parameter);

/* ! @} */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Starts ED Scan
 *
 * This function starts an ED Scan for the scan duration specified by the
 * MAC layer.
 *
 * \param scan_duration Specifies the ED scan duration in symbols
 *
 * \return MAC_SUCCESS - ED scan duration timer started successfully
 *         TAL_BUSY - TAL is busy servicing the previous request from MAC
 *         TAL_TRX_ASLEEP - Transceiver is currently sleeping
 *         FAILURE otherwise
 */
retval_t tal_ed_start(uint8_t scan_duration)
{
	uint32_t duration_time;
	retval_t timer_status;
	tal_trx_status_t trx_status;

	/*
	 * Check if the TAL is in idle state. Only in idle state it can
	 * accept and ED request from the MAC.
	 */
	if (TAL_IDLE != tal_state) {
		if (tal_state == TAL_SLEEP) {
			return TAL_TRX_ASLEEP;
		} else {
			Assert("TAL is TAL_BUSY" == 0);
			return TAL_BUSY;
		}
	}

	/*
	 * Disable the transceiver interrupts to prevent frame reception
	 * while performing ED scan.
	 */
	pal_trx_irq_dis();
	do {
		trx_status = set_trx_state(CMD_PLL_ON);
	} while (trx_status != PLL_ON);
	pal_trx_reg_read(RG_IRQ_STATUS);    /* Clear existing interrupts */

	/* Make sure that receiver is switched on. */
	if (set_trx_state(CMD_RX_ON) != RX_ON) {
		pal_trx_irq_en(); /* Enable transceiver main interrupt. */
		return FAILURE;
	}

	/* Perform ED in TAL_ED state. */
	tal_state = TAL_ED;

	max_ed_level = 0; /* reset max value */

	duration_time = TAL_CONVERT_SYMBOLS_TO_US(
			CALCULATE_SYMBOL_TIME_SCAN_DURATION(scan_duration));

	/* Start a timer for Scan Duration specified */
	timer_status = pal_timer_start(TAL_ED_SCAN_DURATION_TIMER,
			duration_time,
			TIMEOUT_RELATIVE,
			(FUNC_PTR)scan_duration_timer_expired_cb, NULL);

	if (timer_status == MAC_SUCCESS) {
		initiate_ed_sampling();
		return MAC_SUCCESS;
	} else {
		Assert("ed scan duration timer start failed" == 0);
		tal_state = TAL_IDLE;
		return FAILURE;
	}
}

/**
 * \brief Starts the ED sampling process
 *
 * This function is called to initiate ED sampling.
 */
static inline void initiate_ed_sampling(void)
{
#ifndef ENABLE_HIGH_PRIO_TMR
	retval_t timer_status;
#endif

	/*
	 * Initiate ED operation by writing any value into transceiver register
	 * PHY_ED_LEVEL.
	 */
	pal_trx_reg_write(RG_PHY_ED_LEVEL, 0x00);

	/*
	 * Start timer for reading ED value from the transceiver after
	 * 140 microseconds.
	 */
#ifdef ENABLE_HIGH_PRIO_TMR
	pal_start_high_priority_timer(TAL_ED_SAMPLE_TIMER,
			TAL_CONVERT_SYMBOLS_TO_US(ED_SAMPLE_DURATION_SYM + 1),
			(FUNC_PTR)ed_sampling_done_cb, NULL);
#else
	do {
		timer_status = pal_timer_start(TAL_ED_SAMPLE_TIMER,
				TAL_CONVERT_SYMBOLS_TO_US(ED_SAMPLE_DURATION_SYM
				+ 1),
				TIMEOUT_RELATIVE,
				(FUNC_PTR)ed_sampling_done_cb, NULL);
#if (_DEBUG_ > 0)
		if (timer_status != MAC_SUCCESS) {
			Assert("Unable to start ED sample timer" == 0);
		}

#endif
	} while (timer_status != MAC_SUCCESS);
#endif
}

/**
 * \brief Reads ED value from the transceiver register
 *
 * This function is called upon ED request timer expiry. It reads the
 * transceiver register to get ED value.
 *
 * \param parameter unused callback parameter
 */
static void ed_sampling_done_cb(void *parameter)
{
	uint8_t ed_value;

	/* Read the ED Value. */
	ed_value = pal_trx_reg_read(RG_PHY_ED_LEVEL);

	/*
	 * Update the peak ED value received, if greater than the previously
	 * read ED value.
	 */
	if (ed_value > max_ed_level) {
		max_ed_level = ed_value;
	}

	/*
	 * The ED Scan duration timer has not expired, hence continue to take
	 * samples of the ED value from the transceiver.
	 */
	initiate_ed_sampling();

	/* Keep compiler happy. */
	parameter = parameter;
}

/**
 * \brief Scan duration timer expiry handler
 *
 * This function updates the max_ed_level and invokes the callback function
 * tal_ed_end_cb().
 *
 * \param parameter unused callback parameter
 */
static void scan_duration_timer_expired_cb(void *parameter)
{
#if (_DEBUG_ > 0)
	uint8_t irq_status;
#endif

#ifdef ENABLE_HIGH_PRIO_TMR
	pal_stop_high_priority_timer(TAL_ED_SAMPLE_TIMER);
#else
	pal_timer_stop(TAL_ED_SAMPLE_TIMER);
#endif

#if (_DEBUG_ > 0)
	if (pal_is_timer_running(TAL_ED_SAMPLE_TIMER)) {
		Assert("ED timer running after ED scan" == 0);
	}

#endif

	/*
	 * During ed scan a frame might be received.
	 * Since we are not interested in a possible frame reception,
	 * the ongoing reception is aborted and the ISR is not required.
	 */
	set_trx_state(CMD_FORCE_PLL_ON); /* cancel any ongoing reception */

#if (_DEBUG_ > 0)
	irq_status =
#endif
	pal_trx_reg_read(RG_IRQ_STATUS);        /* clear IRQ status */
	/* Avoid to trigger the ISR; input capture flag is cleared */
	pal_trx_irq_flag_clr();

#if (_DEBUG_ > 0)
	if (irq_status != TRX_NO_IRQ) {
		/* \TODO handle trx interrupts like battery monitor */
	}

#endif

	pal_trx_irq_en(); /* Enable the transceiver main interrupt. */
	tal_state = TAL_IDLE; /* ed scan is done */
	set_trx_state(CMD_RX_AACK_ON);

#ifndef TRX_REG_RAW_VALUE

	/*
	 * Scale ED result.
	 * Clip values to 0xFF if > -35dBm
	 */
	if (max_ed_level > CLIP_VALUE_REG) {
		max_ed_level = 0xFF;
	} else {
		max_ed_level
			= (uint8_t)(((uint16_t)max_ed_level *
				0xFF) / CLIP_VALUE_REG);
	}

#endif

	tal_ed_end_cb(max_ed_level);

	/* Keep compiler happy. */
	parameter = parameter;
}

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/* EOF */
