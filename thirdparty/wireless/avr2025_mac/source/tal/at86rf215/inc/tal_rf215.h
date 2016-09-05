/**
 * @file tal_rf215.h
 *
 * @brief This file contains types and variable definition that are
 *        used within the TAL only.
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

/* Prevent double inclusion */
#ifndef TAL_RF215_H
#define TAL_RF215_H

/* === INCLUDES ============================================================ */

#include "tal.h"
#include "tal_config.h"
#include "pal.h"
#include "ieee_154g.h"
#include "at86rf215.h"

/* === TYPES =============================================================== */

/* TRX Parameter: Tolerance of the phyTransmitPower PIB attribute.
 *                This is encoded into the two MSBits of the attribute,
 *                and is effectively read-only.
 */
#define TX_PWR_TOLERANCE   (0x80)

/**
 * Enumeration for TRX identification
 */
typedef enum trx_id_tag {
	RF09, /**< Id for sub-1 GHz device */
	RF24, /**< Id for 2.4 GHz device */
	RFBOTH /**< Id for both device parts */
} SHORTENUM trx_id_t;

#define TRX_SUPPORTED_CHANNELS_LEG(trx)	\
	( \
		(trx == RF24) ?	\
		(0x07FFF800) : \
		(0x000007FF) \
	)

/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */

/** Clear all interrupts for provided baseband core */
#define TAL_BB_IRQ_CLR_ALL(BB_CORE)     tal_bb_irqs[BB_CORE] = BB_IRQ_NO_IRQ
/** Check if certain IRQ is set for provided baseband core */
#define TAL_BB_IS_IRQ_SET(BB_CORE, IRQ) ((tal_bb_irqs[BB_CORE] & IRQ) ? 1 : 0)
/** Clear interrupt(s) for provided baseband core, avoid Pa091 */
#define TAL_BB_IRQ_CLR(BB_CORE,	\
			IRQ)    tal_bb_irqs[BB_CORE] \
		&= (uint8_t)(~((uint32_t)IRQ))
/** Add interrupt(s) for provided baseband core */
#define TAL_BB_IRQ_ADD(BB_CORE, IRQ)    tal_bb_irqs[BB_CORE] |= IRQ

/** Clear all interrupts for provided radio core */
#define TAL_RF_IRQ_CLR_ALL(RF_CORE)     tal_rf_irqs[RF_CORE] = RF_IRQ_NO_IRQ
/** Check if certain IRQ is set for provided radio core */
#define TAL_RF_IS_IRQ_SET(RF_CORE, IRQ) ((tal_rf_irqs[RF_CORE] & IRQ) ? 1 : 0)
/** Clear interrupt(s) for provided radio core,  avoid Pa091 */
#define TAL_RF_IRQ_CLR(RF_CORE,	\
			IRQ)    tal_rf_irqs[RF_CORE] \
		&= (uint8_t)(~((uint32_t)IRQ))
/** Add interrupt(s) for provided radio core */
#define TAL_RF_IRQ_ADD(RF_CORE, IRQ)    tal_rf_irqs[RF_CORE] |= IRQ

/** Register offset between RF09 and TRX24 */
#define RF_BASE_ADDR_OFFSET             (BASE_ADDR_RF24_RF24 - \
	BASE_ADDR_RF09_RF09)
/** Register offset between the baseband cores */
#define BB_BASE_ADDR_OFFSET             (BASE_ADDR_BBC1_CORE1 -	\
	BASE_ADDR_BBC0_CORE0)
/** Register offset between the Tx frame buffers. */
#define BB_TX_FRM_BUF_OFFSET            (RG_BBC1_FBTXS - RG_BBC0_FBTXS)
/** Register offset between the Rx frame buffers. */
#define BB_RX_FRM_BUF_OFFSET            (RG_BBC1_FBRXS - RG_BBC0_FBRXS)
/** Register offset for test register area. */
#define RFT_TST_ADDR_OFFSET             (0x280 - 0x180)

/* === PROTOTYPES ========================================================== */
void get_ofdm_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		ofdm_option_t option, uint32_t *freq, uint32_t *spacing);
void get_oqpsk_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		uint32_t *freq, uint32_t *spacing);
void get_leg_oqpsk_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		uint32_t *freq, uint32_t *spacing);
void get_fsk_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		fsk_op_mode_t option, uint32_t *freq, uint32_t *spacing);
oqpsk_chip_rate_t get_oqpsk_chip_rate(trx_id_t trx_id,
		sun_freq_band_t freq_band);
uint16_t get_sun_max_ch_no(trx_id_t trx_id);

#endif /* TAL_RF215_H */
