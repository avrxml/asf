/**
 * @file tal_ed.c
 *
 * @brief This file implements ED Scan
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#include "at86rf212b.h"
#include "tal_internal.h"
#include "tal_irq_handler.h"
#include "mac_build_config.h"

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * \addtogroup group_tal_ed_212b
 * @{
 */

/* Constant define for the ED scaling: register value at -35dBm */

/* \todo adapt to different RSSI_BASE_VALs? --> would result in to
 * CLIP_VALUE_REG={61, 62, 63}
 */
#define CLIP_VALUE_REG                  (62)

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
static uint32_t sampler_counter;

/* === PROTOTYPES ========================================================== */

static void trx_ed_irq_handler_cb(void);

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
	/*
	 * Check if the TAL is in idle state. Only in idle state it can
	 * accept and ED request from the MAC.
	 */
	if (TAL_IDLE != tal_state) {
		if (tal_trx_status == TRX_SLEEP) {
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
	pal_trx_irq_dis(); /* Disable transceiver main interrupt. */
	set_trx_state(CMD_FORCE_PLL_ON);
	trx_reg_read(RG_IRQ_STATUS);    /* Clear existing interrupts */
	trx_bit_write(SR_RX_PDT_DIS, RX_DISABLE);
	trx_irq_init((FUNC_PTR)trx_ed_irq_handler_cb);
	trx_bit_write(SR_IRQ_MASK, TRX_IRQ_4_CCA_ED_DONE); /* Enable
	                                                    * interrupt */
	pal_trx_irq_en(); /* Enable main transceiver interrupt. */

	/* Make sure that receiver is switched on. */
	if (set_trx_state(CMD_RX_ON) != RX_ON) {
		/* Restore previous configuration */
		trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE);
		trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
		trx_reg_write(RG_IRQ_MASK, TRX_IRQ_DEFAULT); /* enable
		                                              * TRX_END
		                                              * interrupt */
		pal_trx_irq_en(); /* Enable main transceiver interrupt. */

		return FAILURE;
	}

	/* write dummy value to start measurement */
	trx_reg_write(RG_PHY_ED_LEVEL, 0xFF);

	/* Perform ED in TAL_ED_RUNNING state. */
	tal_state = TAL_ED_RUNNING;

	max_ed_level = 0; /* reset max value */

	sampler_counter = CALCULATE_SYMBOL_TIME_SCAN_DURATION(scan_duration) /
			ED_SAMPLE_DURATION_SYM;

	return MAC_SUCCESS;
}

/**
 * \brief ED Scan Interrupt
 *
 * This function handles an ED done interrupt from the transceiver.
 */
static void trx_ed_irq_handler_cb(void)
{
	uint8_t ed_value;
	trx_irq_reason_t trx_irq_cause;

	trx_irq_cause = (trx_irq_reason_t)trx_reg_read(RG_IRQ_STATUS);

	if (trx_irq_cause & TRX_IRQ_4_CCA_ED_DONE) {
		/* Read the ED Value. */
		ed_value = trx_reg_read(RG_PHY_ED_LEVEL);

		/*
		 * Update the peak ED value received, if greater than the
		 * previously
		 * read ED value.
		 */
		if (ed_value > max_ed_level) {
			max_ed_level = ed_value;
		}

		sampler_counter--;
		if (sampler_counter > 0) {
			/* write dummy value to start measurement */
			trx_reg_write(RG_PHY_ED_LEVEL, 0xFF);
		} else {
			tal_state = TAL_ED_DONE;
		}
	}

#if (_DEBUG_ > 0)
	if (trx_irq_cause & (~(TRX_IRQ_0_PLL_LOCK | TRX_IRQ_4_CCA_ED_DONE))) {
		Assert("Unexpected interrupt" == 0);
	}
#endif
}

/**
 * \brief Scan done
 *
 * This function updates the max_ed_level and invokes the callback function
 * tal_ed_end_cb().
 *
 * \param parameter unused callback parameter
 */
void ed_scan_done(void)
{
	trx_bit_write(SR_RX_PDT_DIS, RX_ENABLE);
	trx_irq_init((FUNC_PTR)trx_irq_handler_cb);
	trx_reg_write(RG_IRQ_MASK, TRX_IRQ_DEFAULT); /* enable TRX_END
	                                              * interrupt */
	pal_trx_irq_en(); /* Enable transceiver main interrupt. */

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
}

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/* EOF */
