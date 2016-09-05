/**
 * @file tfa.c
 *
 * @brief Implementation of Transceiver Feature Access (TFA) functionality.
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

#if (defined ENABLE_TFA) || (defined TFA_CW) || (defined TFA_CCA)

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
#ifdef IQ_RADIO
#include "pal_internal.h"
#endif

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

#if (defined ENABLE_TFA) || (defined TFA_CCA)

/**
 * @brief Perform a CCA
 *
 * This blocking function performs a CCA request.
 *
 * @return phy_enum_t PHY_IDLE or PHY_BUSY
 */
phy_enum_t tfa_cca_perform(trx_id_t trx_id)
{
	phy_enum_t ret;

	if (tal_state[trx_id] != TAL_IDLE) {
		ret = PHY_BUSY;
	} else {
		rf_cmd_state_t previous_state = trx_state[trx_id];

		if (trx_state[trx_id] == RF_TRXOFF) {
			switch_to_txprep(trx_id);
		}

		if (trx_state[trx_id] != RF_RX) {
			switch_to_rx(trx_id);
			pal_timer_delay(tal_pib[trx_id].agc_settle_dur); /*
			                                                  * allow
			                                                  * filters
			                                                  * to
			                                                  * settle */
		}

		/* Disable BB */
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 0);

#ifndef BASIC_MODE
		/* Enable EDC interrupt */
		trx_bit_write( reg_offset + SR_RF09_IRQM_EDC, 1);
#endif
		/* Start single ED measurement; use reg_write - it's the only
		 *subregister */
		tal_state[trx_id] = TAL_TFA_CCA;
#ifdef IQ_RADIO
		/* Enable EDC interrupt */
		trx_bit_write(RF215_RF, reg_offset + SR_RF09_IRQM_EDC, 1);
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_EDC, RF_EDSINGLE);
#else
		trx_reg_write( reg_offset + RG_RF09_EDC, RF_EDSINGLE);
#endif

		/* Wait until measurement is completed */
		while (TAL_RF_IS_IRQ_SET(trx_id, RF_IRQ_EDC) == false) {
		}
		TAL_RF_IRQ_CLR(trx_id, RF_IRQ_EDC);
#ifndef BASIC_MODE
		/* Disable EDC interrupt again */
		trx_bit_write( reg_offset + SR_RF09_IRQM_EDC, 0);
#endif
#ifdef IQ_RADIO
		trx_bit_write(RF215_RF, reg_offset + SR_RF09_IRQM_EDC, 0);
#endif
		/* Since it is a blocking function, restore TAL state */
		tal_state[trx_id] = TAL_IDLE;

		switch_to_txprep(trx_id); /* Leave Rx mode */

		/* Switch BB on again */
		trx_bit_write( reg_offset + SR_BBC0_PC_BBEN, 1);

		/* Capture ED value for current frame / ED scan */
#ifdef IQ_RADIO
		tal_current_ed_val[trx_id] = trx_reg_read(RF215_RF,
				reg_offset +
				RG_RF09_EDV);
#else
		tal_current_ed_val[trx_id] = trx_reg_read(
				reg_offset + RG_RF09_EDV);
#endif

		if (tal_current_ed_val[trx_id] < tal_pib[trx_id].CCAThreshold) {
			/* Idle */
			ret = PHY_IDLE;
		} else {
			/* Busy */
			ret = PHY_BUSY;
		}

		/* Restore previous trx state */
		if (previous_state == RF_RX) {
			switch_to_rx(trx_id);
		} else {
			/* Switch to TRXOFF */
			trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
#ifdef IQ_RADIO
			trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD,
					RF_TRXOFF);
#endif
			trx_state[trx_id] = RF_TRXOFF;
		}
	}

	return ret;
}

#endif

/**
 * @brief Starts continuous transmission on current channel
 *
 * @param trx_id Identifier of the transceiver
 * @param tx_mode Transmission mode
 */
void tfa_continuous_tx_start(trx_id_t trx_id, continuous_tx_mode_t tx_mode)
{
	uint16_t len;
	if (tal_state[trx_id] != TAL_IDLE) {
		return;
	}

#ifdef IQ_RADIO
	if (trx_id == RF09) {
		/* Check if the other radio is currently in use */
		if (trx_state[RF24] == RF_TX) {
			return;
		} else {
			/* Select corresponding baseband core */
			trx_bit_write(RF215_BB, SR_RF_IQIFC2_CSELTX, RF09); /*
			                                                     * RF09
			                                                     * is
			                                                     * selected */
		}
	} else {
		/* Check if the other radio is currently in use */
		if (trx_state[RF09] == RF_TX) {
			return;
		} else {
			/* Select corresponding baseband core */
			trx_bit_write(RF215_BB, SR_RF_IQIFC2_CSELTX, RF24); /*
			                                                     * RF24
			                                                     * is
			                                                     * selected */
		}
	}

#endif

	if (trx_state[trx_id] == RF_RX) {
		tal_state[trx_id] = TAL_TFA_CW_RX;
	} else {
		tal_state[trx_id] = TAL_TFA_CW;
	}

	/* Set to TxPREP state */
	switch_to_txprep(trx_id);

	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	if (tx_mode == CW_MODE) {
#ifdef IQ_RADIO
		/* Disable embedded TX control */
		trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 0);
#else
		/* Enable baseband bypass */
		trx_bit_write( SR_RF_IQIFC1_CHPM, 1);
#endif

		/* Configure DAC to generate carrier signal */
		uint8_t dac_config[2] = {(0x7E | 0x80), (0x3F | 0x80)};
#ifdef IQ_RADIO
		trx_write(RF215_RF, reg_offset + RG_RF09_TXDACI, dac_config, 2);
#else
		trx_write( reg_offset + RG_RF09_TXDACI, dac_config, 2);
#endif

		/* Trigger Tx start */
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TX);
#else
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TX);
#endif
		trx_state[trx_id] = RF_TX;
	} else { /* PRBS mode */
		/* Enable continuous transmission mode */
		trx_bit_write( reg_offset + SR_BBC0_PC_CTX, 1);

		/* Fill length field */
#ifdef SUPPORT_LEGACY_OQPSK
		if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
			len = 127;
		} else
#endif
		{
			len = 2047;
		}

		trx_write( reg_offset + RG_BBC0_TXFLL, (uint8_t *)&len, 2);
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TX);
		trx_state[trx_id] = RF_TX;

		/* Fill frame buffer */
		uint16_t tx_frm_buf_offset = BB_TX_FRM_BUF_OFFSET * trx_id;
		uint8_t data[10];
		for (uint16_t k = 0; k < (len / 10); k++) {
			for (uint16_t i = 0; i < 10; i++) {
				data[i] = (uint8_t)rand();
			}
			trx_write( tx_frm_buf_offset + RG_BBC0_FBTXS, data, 10);
			tx_frm_buf_offset += 10;
		}
		uint16_t remaining_bytes = len % 10;
		for (uint16_t i = 0; i < remaining_bytes; i++) {
			data[i] = (uint8_t)rand();
		}
		trx_write( tx_frm_buf_offset + RG_BBC0_FBTXS, data,
				remaining_bytes);
	}
}

#endif

#if (defined ENABLE_TFA) || (defined TFA_CW)

/**
 * @brief Stops CW transmission
 *
 * @param trx_id Identifier of the transceiver
 */
void tfa_continuous_tx_stop(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	/* Stop continuous transmission */
	trx_bit_write( reg_offset + SR_BBC0_PC_CTX, 0);
#ifdef IQ_RADIO
	/* Allow command via SPI */
	trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 0);
#endif
	switch_to_txprep(trx_id);

	/* Disable carrier transmission - even if it has not been used. */
	uint8_t dac_config[2] = {0x00, 0x00};
	uint16_t rft_reg_offset = RFT_TST_ADDR_OFFSET * trx_id;
#ifdef IQ_RADIO
	trx_write(RF215_RF, rft_reg_offset + RG_RF09_TXDACI, dac_config, 2);
	/* Enable embedded TX control again */
	trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 1);
#else
	trx_write( rft_reg_offset + RG_RF09_TXDACI, dac_config, 2);
	/* Disable baseband bypass */
	trx_bit_write( SR_RF_IQIFC1_CHPM, 0);
#endif

	/* Restore previous settings */
	if (tal_state[trx_id] == TAL_TFA_CW_RX) {
		tal_state[trx_id] = TAL_IDLE;
		switch_to_rx(trx_id);
	} else {
		tal_state[trx_id] = TAL_IDLE;
		/* Switch to TRXOFF */
#ifdef IQ_RADIO
		trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
		trx_reg_write(RF215_BB, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#else
		trx_reg_write( reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
		trx_state[trx_id] = RF_TRXOFF;
	}
}

#endif

/* EOF */
