/**
 * @file tal_phy_cfg.c
 *
 * @brief This file handles the PHY configuration
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
#include <string.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "tal_config.h"
#include "ieee_const.h"
#include "tal_pib.h"
#include "tal_internal.h"
#include "ieee_154g.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

#ifdef SUPPORT_OFDM
static retval_t conf_ofdm(trx_id_t trx_id);

#endif
#ifdef SUPPORT_OQPSK
static retval_t conf_oqpsk(trx_id_t trx_id);

#endif
#ifdef SUPPORT_LEGACY_OQPSK
static retval_t conf_leg_oqpsk(trx_id_t trx_id);

#endif
#if (defined SUPPORT_LEGACY_OQPSK) || (defined SUPPORT_OQPSK)
#endif
#ifdef SUPPORT_FSK
#ifndef SUPPORT_MODE_SWITCH
static retval_t conf_fsk(trx_id_t trx_id);

#endif
#endif

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Configure the transceiver w.r.t. modulation
 *
 * @param trx_id Transceiver identifier
 *
 * @return MAC_SUCCESS if setting could be executed, else MAC_INVALID_PARAMETER
 */
retval_t conf_trx_modulation(trx_id_t trx_id)
{
	retval_t status;

	/* Change PHY only in TRXOFF or TXPREP. Since TXPREP is not possible
	 *here, check for TRXOFF */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	rf_cmd_state_t previous_trx_state = trx_reg_read(
			reg_offset + RG_RF09_STATE);
	if (previous_trx_state != RF_TRXOFF) {
		trx_reg_write(reg_offset + RG_RF09_CMD, RF_TRXOFF);
#ifdef IQ_RADIO
		pal_dev_reg_write(RF215_RF, reg_offset + RG_RF09_CMD,
				RF_TRXOFF);
#endif
		trx_state[trx_id] = RF_TRXOFF;
  #if (defined RF215V1) && ((defined SUPPORT_FSK) || (defined SUPPORT_OQPSK))
		stop_rpc(trx_id);
  #endif
	}

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		status = conf_fsk(trx_id);
		break;
#endif

#ifdef SUPPORT_OFDM
	case OFDM:
		status = conf_ofdm(trx_id);
		break;
#endif

#ifdef SUPPORT_OQPSK
	case OQPSK:
		status = conf_oqpsk(trx_id);
		break;
#endif

#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		status = conf_leg_oqpsk(trx_id);
		break;
#endif

	default:
		status = MAC_INVALID_PARAMETER;
		break;
	}

	tal_pib[trx_id].TransmitPower = -17 + (int8_t)DEFAULT_TX_PWR_REG;

	/* Restore previous state */
	switch (previous_trx_state) {
	case RF_RX:
		switch_to_txprep(trx_id);
		switch_to_rx(trx_id);
		break;

	case RF_TXPREP:
		switch_to_txprep(trx_id);
		break;

	default:
		/* stay in TRXOFF */
		break;
	}

	return status;
}

#ifdef SUPPORT_OFDM

/**
 * @brief Configures MR-OFDM
 *
 * @param trx_id Transceiver identifier
 */
static retval_t conf_ofdm(trx_id_t trx_id)
{
	retval_t status;

	/* Configure RF */
	status = ofdm_rfcfg(tal_pib[trx_id].phy.phy_mode.ofdm.option, trx_id);
	if (status == MAC_SUCCESS) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

		/* Configure BB */
		trx_bit_write(reg_offset + SR_BBC0_PC_PT, BB_MROFDM);
		trx_bit_write(reg_offset + SR_BBC0_OFDMC_OPT,
				tal_pib[trx_id].phy.phy_mode.ofdm.option);
	}

	return status;
}

#endif /* #ifdef SUPPORT_OFDM */

#ifdef SUPPORT_OQPSK

/**
 * @brief Configures MR-OQPSK
 *
 * @param trx_id Transceiver identifier
 */
static retval_t conf_oqpsk(trx_id_t trx_id)
{
	retval_t status;

	/* Configure RF */
	status = oqpsk_rfcfg(tal_pib[trx_id].phy.phy_mode.oqpsk.chip_rate,
			trx_id);
	if (status == MAC_SUCCESS) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

		/* Configure BB */
		trx_bit_write(reg_offset + SR_BBC0_PC_PT, BB_MROQPSK);
		trx_bit_write(reg_offset + SR_BBC0_OQPSKC0_FCHIP,
				tal_pib[trx_id].phy.phy_mode.oqpsk.chip_rate);
		trx_bit_write( reg_offset + SR_BBC0_OQPSKPHRTX_LEG, 0);
		trx_bit_write( reg_offset + SR_BBC0_OQPSKC2_RXM, 0); /* MR mode
		                                                      * only */
		trx_bit_write( reg_offset + SR_BBC0_OQPSKC2_RPC,
				tal_pib[trx_id].RPCEnabled); /* RPC */
	}

	return status;
}

#endif /* #ifdef SUPPORT_OQPSK */

#ifdef SUPPORT_LEGACY_OQPSK

/**
 * @brief Configures legacy O-QPSK
 *
 * @param trx_id Transceiver identifier
 */
static retval_t conf_leg_oqpsk(trx_id_t trx_id)
{
	retval_t status;

	/* Configure RF */
	status = oqpsk_rfcfg(tal_pib[trx_id].phy.phy_mode.leg_oqpsk.chip_rate,
			trx_id);
	if (status == MAC_SUCCESS) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

		/* set bb_core0 baseband mode to OQPSK */
		trx_bit_write(reg_offset + SR_BBC0_PC_PT, BB_MROQPSK);
		trx_bit_write(reg_offset + SR_BBC0_OQPSKC0_FCHIP,
				tal_pib[trx_id].phy.phy_mode.leg_oqpsk.chip_rate);
		trx_bit_write(reg_offset + SR_BBC0_OQPSKC2_RXM, 1); /* legacy
		                                                     * mode only */
		trx_bit_write(reg_offset + SR_BBC0_OQPSKPHRTX_LEG, 1); /*
		                                                        * configure
		                                                        * legacy
		                                                        * transmit */
	}

	return status;
}

#endif /* #ifdef SUPPORT_LEGACY_OQPSK */

#ifdef SUPPORT_FSK

/**
 * @brief Configures FSK
 *
 * @param trx_id Transceiver identifier
 */
#ifdef SUPPORT_MODE_SWITCH
retval_t conf_fsk(trx_id_t trx_id)
#else
static retval_t conf_fsk(trx_id_t trx_id)
#endif
{
	retval_t status;

	/* Configure RF */
	fsk_data_rate_t sym_rate;
	if (tal_pib[trx_id].phy.phy_mode.fsk.mod_type == F2FSK) {
		sym_rate = tal_pib[trx_id].phy.phy_mode.fsk.data_rate;
	} else { /* F4FSK */
		sym_rate
			= (fsk_data_rate_t)(tal_pib[trx_id].phy.phy_mode.fsk.
				data_rate
				/ 2);
	}

	status = fsk_rfcfg(tal_pib[trx_id].phy.phy_mode.fsk.mod_type, sym_rate,
			tal_pib[trx_id].phy.phy_mode.fsk.mod_idx, trx_id);
	if (status == MAC_SUCCESS) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

		trx_bit_write(reg_offset + SR_BBC0_PC_PT, BB_MRFSK);
		/* FSKC0 */
		trx_bit_write(reg_offset + SR_BBC0_FSKC0_MORD,
				tal_pib[trx_id].phy.phy_mode.fsk.mod_type);
		trx_bit_write(reg_offset + SR_BBC0_FSKC0_MIDX,
				tal_pib[trx_id].phy.phy_mode.fsk.mod_idx);
		trx_bit_write(reg_offset + SR_BBC0_FSKC0_BT,
				tal_pib[trx_id].phy.phy_mode.fsk.bt);
		/* FSKC1 */
		trx_bit_write( reg_offset + SR_BBC0_FSKC1_SRATE, sym_rate);
		/* FSKC2 */
		uint8_t pdtm;
		if (tal_pib[trx_id].FSKPreambleLength < 8) {
			pdtm = 1;
		} else {
			pdtm = 0;
		}

		trx_bit_write( reg_offset + SR_BBC0_FSKC2_PDTM, pdtm);
		/* Configure RPC */
		config_fsk_rpc(trx_id, sym_rate);
		/* Configure SFD */
		set_sfd(trx_id);
	}

	return status;
}

#endif /* #ifdef SUPPORT_FSK */

#ifdef SUPPORT_FSK

/**
 * @brief Configures reduced power consumption for FSK
 *
 * @param trx_id Transceiver identifier
 */
void config_fsk_rpc(trx_id_t trx_id, fsk_data_rate_t sym_rate)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	if (!tal_pib[trx_id].RPCEnabled) {
		/* FSKRPC */
		trx_bit_write( reg_offset + SR_BBC0_FSKRPC_EN, 0);
		return;
	}

	/* Minimum preamble length depends on symbol rate */
	uint8_t min_pream_len;
	uint8_t div_reg;
	switch (sym_rate) {
	default:
	case FSK_DATA_RATE_50:
		min_pream_len = 2;
		div_reg = 6;
		break;

	case FSK_DATA_RATE_100:
		min_pream_len = 3;
		div_reg = 5;
		break;

	case FSK_DATA_RATE_150: /* fall through */
	case FSK_DATA_RATE_200: /* fall through */
	case FSK_DATA_RATE_300:
		min_pream_len = 8;
		div_reg = 4;
		break;

	case FSK_DATA_RATE_400:
		min_pream_len = 10;
		div_reg = 3;
		break;
	}
	uint8_t div = 1 << (div_reg - 1);

	if (tal_pib[trx_id].FSKPreambleLength > (2 * min_pream_len)) {
		/* Configure preamble length for reception */
		trx_reg_write( reg_offset + RG_BBC0_FSKPLL,
				min_pream_len & 0xFF);
		trx_bit_write( reg_offset + SR_BBC0_FSKC1_FSKPLH,
				min_pream_len >> 8);
		/* Configure ton */
		uint16_t ton = tal_pib[trx_id].OctetDuration_us * min_pream_len;
		uint8_t reg_on = ton / div;
		if ((ton % div) > 0) {
			reg_on++;
		}

		/* Configure toff */
		uint16_t toff = tal_pib[trx_id].FSKPreambleLength *
				tal_pib[trx_id].OctetDuration_us;
		toff -= 2 * reg_on * div;
		uint8_t reg_off = toff / div;
		/* Download settings to register */
		uint8_t reg[3];
		reg[0] = div_reg; /* FSKRPC; RPC is disabled */
		reg[1] = reg_on; /* on time */
		reg[2] = reg_off; /* off time */
		trx_write(reg_offset + RG_BBC0_FSKRPC, reg, 3);
	} else {
		/* Disable RPC, since preamble is too short. */
		tal_pib[trx_id].RPCEnabled = false;
	}
}

#endif

#ifdef SUPPORT_FSK

/**
 * @brief Configures SFDs
 *
 * @param trx_id Transceiver identifier
 */
void set_sfd(trx_id_t trx_id)
{
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	uint32_t sfd;

	if (tal_pib[trx_id].MRFSKSFD == 0) {
		sfd = (uint32_t)F2FSK_SFD_0_UNCODED |
				((uint32_t)F2FSK_SFD_0_CODED << 16);
	} else { /* SFD 1 */
		sfd = (uint32_t)F2FSK_SFD_1_UNCODED |
				((uint32_t)F2FSK_SFD_1_CODED << 16);
	}

	trx_write(reg_offset + RG_BBC0_FSKSFD0L, (uint8_t *)&sfd, 4);
}

#endif /* #ifdef SUPPORT_FSK */

/* EOF */
