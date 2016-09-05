/**
 * @file tfa_batmon.c
 *
 * @brief Implementation of TFA battery monitor functionality.
 *
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "tal_config.h"

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || \
	(defined TFA_BAT_MON_READ) || (defined TFA_BAT_MON_IRQ)

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "at86rf215.h"
#include "tal_internal.h"
#include "tfa.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

#define BATMON_MON_VTH_MIN      1700
#define BATMON_MON_VTH_MAX      3675

/* === GLOBALS ============================================================= */

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined TFA_BAT_MON_IRQ)
static irq_handler_t irq_hdl_batmon;
#endif

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined TFA_BAT_MON_READ)

/**
 * @brief Gets the transceiver's supply voltage
 *
 * @return mv Milli Volt; 0 if below range, 0xFFFF if above range
 */
uint16_t tfa_get_batmon_voltage(void)
{
	uint16_t mv;
	if ((tal_state[RF09] == TAL_SLEEP) && (tal_state[RF24] == TAL_SLEEP)) {
		return 0;
	}

	/* Store previous settings: range, threshold, IRQ */
#ifdef IQ_RADIO
	bool previous_range = trx_bit_read(RF215_RF, SR_RF_BMDVC_BMHR);
	uint8_t pre_vth = trx_bit_read(RF215_RF, SR_RF_BMDVC_BMVTH);
#else
	bool previous_range = trx_bit_read( SR_RF_BMDVC_BMHR);
	uint8_t pre_vth = trx_bit_read( SR_RF_BMDVC_BMVTH);
#endif

	/* Disable both battery monitor interrupt during measurement */
	for (uint8_t i = 0; i < NUM_TRX; i++) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * i;
#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, reg_offset + SR_RF09_IRQM_BATLOW, 0);
#else
		trx_bit_write( reg_offset + SR_RF09_IRQM_BATLOW, 0);
#endif
	}

	/* Check if supply voltage is within lower range */
#ifdef IQ_RADIO
	trx_bit_write(RF215_RF, SR_RF_BMDVC_BMHR, 0);
	trx_bit_write(RF215_RF, SR_RF_BMDVC_BMVTH, 0x0F);
	pal_timer_delay(2); /* Wait until Batmon has been settled. */
	bool high_range = trx_bit_read(RF215_RF, SR_RF_BMDVC_BMS);
#else
	trx_bit_write( SR_RF_BMDVC_BMHR, 0);
	trx_bit_write( SR_RF_BMDVC_BMVTH, 0x0F);
	pal_timer_delay(2); /* Wait until Batmon has been settled. */
	bool high_range = trx_bit_read( SR_RF_BMDVC_BMS);
#endif
	if (high_range) {
		/* EVDD is above threshold */
		/* Set to high range */
#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, SR_RF_BMDVC_BMHR, 1);
#else
		trx_bit_write( SR_RF_BMDVC_BMHR, 1);
#endif
	} else {
		/* EVDD is below threshold */
		/* Keep current range; i.e. low range */
	}

	/* Find the voltage factor */
	uint8_t vth = 0xFF; /* 0xFF used as flag */
	for (uint8_t i = 0; i < 16; i++) {
#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, SR_RF_BMDVC_BMVTH, i);
		pal_timer_delay(2); /* Wait until Batmon has been settled. */
		if (trx_bit_read(RF215_RF, SR_RF_BMDVC_BMS) == 0)
#else
		trx_bit_write( SR_RF_BMDVC_BMVTH, i);
		pal_timer_delay(2); /* Wait until Batmon has been settled. */
		if (trx_bit_read( SR_RF_BMDVC_BMS) == 0)
#endif
		{
			vth = i;
			break;
		}
	}

	/* Calculate EVDD */
	if (high_range) {
		/* High range */
		if (vth == 0xFF) {
			mv = BATMON_MON_VTH_MAX; /* Above range: return max
			                          *value */
		} else {
			mv = 2550 + ((uint16_t)75 * (uint16_t)vth);
		}
	} else {
		/* Low range */
		if (vth == 0) {
			mv = BATMON_MON_VTH_MIN; /* Below range: return min
			                          *value */
		} else {
			mv = 1700 + ((uint16_t)50 * (uint16_t)vth);
		}
	}

	/* Re-store previous settings: range, threshold, IRQ */
#ifdef IQ_RADIO
	trx_bit_write(RF215_RF, SR_RF_BMDVC_BMHR, previous_range);
	trx_bit_write(RF215_RF, SR_RF_BMDVC_BMVTH, pre_vth);
#else
	trx_bit_write( SR_RF_BMDVC_BMHR, previous_range);
	trx_bit_write( SR_RF_BMDVC_BMVTH, pre_vth);
#endif
	/* Clear pending BATLOW IRQ */
	uint8_t irqs[2];
	trx_read( RG_RF09_IRQS, irqs, 2);
	for (trx_id_t i = (trx_id_t)0; i < NUM_TRX; i++) {
		irqs[i] &=  (uint8_t)(~(uint32_t)RF_IRQ_BATLOW);
		TAL_RF_IRQ_ADD(i, irqs[i]);
		TAL_RF_IRQ_CLR(i, RF_IRQ_TRXRDY);
	}

	for (uint8_t i = 0; i < NUM_TRX; i++) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * i;
#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, reg_offset + SR_RF09_IRQM_BATLOW, 1);
#else
		trx_bit_write( reg_offset + SR_RF09_IRQM_BATLOW, 1);
#endif
	}

	return mv;
}

#endif /* #if (defined SUPPORT_TFA) || (defined TFA_BAT_MON_READ) */

#endif /* #if (defined ENABLE_TFA) || (defined TFA_BAT_MON_READ) */

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined TFA_BAT_MON_IRQ)

/*
 * @brief Setups the battery monitor interrupt
 *
 * @param   batmon_irq_cb Callback function for the batmon interrupt
 * @param   vth Threshold value in millivolt
 *
 * @return MAC_SUCCESS if provided voltage is within range, else
 *         MAC_INVALID_PARAMETER
 *
 * @ingroup apiTfaApi
 */
retval_t tfa_batmon_irq_init(FUNC_PTR(batmon_irq_cb), uint16_t vth)
{
	retval_t ret;
	if ((vth < BATMON_MON_VTH_MIN) || (vth > BATMON_MON_VTH_MAX)) {
		ret = MAC_INVALID_PARAMETER;
	} else {
		/* Store callback function */
		irq_hdl_batmon = (irq_handler_t)batmon_irq_cb;

		/* Configure battery monitor threshold */
		bool high_range;
		uint8_t reg;
		if (vth >= 2550) {
			high_range = true;
			reg = (vth - 2550) / 75;
		} else {
			high_range = false;
			if (vth >= 2450) {
				reg = 0x0F;
			} else {
				reg = (vth - 1700) / 50;
			}
		}

#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, SR_RF_BMDVC_BMHR, high_range);
		trx_bit_write(RF215_RF, SR_RF_BMDVC_BMVTH, reg);
#else
		trx_bit_write( SR_RF_BMDVC_BMHR, high_range);
		trx_bit_write( SR_RF_BMDVC_BMVTH, reg);
#endif

		ret = MAC_SUCCESS;
	}

	return ret;
}

#endif /* #if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined
        * TFA_BAT_MON_IRQ) */

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined TFA_BAT_MON_IRQ)
void handle_batmon_irq(void)
{
	if (irq_hdl_batmon != NULL) {
		irq_hdl_batmon();
	}
}

#endif /* #if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined
        * TFA_BAT_MON_IRQ) */

/* EOF */
