/**
 * @file tal_fe.c
 *
 * @brief This file handles the front-end configuration
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
#include "tal_internal.h"
#include "ieee_154g.h"
#ifdef SUPPORT_FSK
#include "tal_fe_fsk_params.h"
#endif

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

#ifndef FWNAME
/* SR, AVGS=0 AGCI=0, AVGS=0 AGCI=1,  AVGS=1 AGCI=0, AVGS=1 AGCI=1 ... */
#define AGC_SETTLE_TABLE \
	{ \
		{1, 9, 8, 10, 9, 12, 11, 16, 15}, \
		{2, 16, 14, 18, 16, 22, 20, 30, 28}, \
		{3, 23, 20, 26, 23, 32, 29, 44, 41}, \
		{4, 31, 27, 35, 31, 43, 39, 59, 55}, \
		{5, 39, 34, 44, 39, 54, 49, 74, 69}, \
		{6, 46, 40, 52, 46, 64, 58, 88, 82}, \
		{8, 56, 53, 69, 61, 85, 77, 117, 109}, \
		{10, 73, 63, 83, 73, 103, 93, 143, 133}	\
	}

#define AGC_SETTLE_TABLE_ROW_SIZE 8
#define AGC_SETTLE_TABLE_COL_SIZE 9
#define AGC_SETTLE_TABLE_TABLE_DATA_TYPE  uint8_t
#endif

#ifdef IQ_RADIO
#    define rf_blk_write(reg, addr, len)    trx_write(RF215_RF, reg, addr, \
		len)
#    define rf_bit_write(reg, val)          trx_bit_write(RF215_RF, reg, \
		val)
#    define rf_reg_write(reg, val)          trx_reg_write(RF215_RF, reg, \
		val)
#    define bb_bit_read(reg)                trx_bit_read(RF215_BB, reg)
#    define bb_reg_read(reg)                trx_reg_read(RF215_BB, reg)
#    define bb_bit_write(reg, val)          trx_bit_write(RF215_BB, reg, \
		val)
#    define bb_blk_write(reg, addr, len)    trx_write(RF215_BB, reg, addr, \
		len)
#else
#    define rf_blk_write(reg, addr, len)    trx_write(reg, addr, len)
#    define rf_bit_write(reg, val)          trx_bit_write(reg, val)
#    define rf_reg_write(reg, val)          trx_reg_write(reg, val)
#    define bb_bit_read(reg)                trx_bit_read(reg)
#    define bb_reg_read(reg)                trx_reg_read(reg)
#    define bb_bit_write(reg, val)          trx_bit_write(reg, val)
#    define bb_blk_write(reg, addr, len)    trx_write(reg, addr, len)
#endif

/* === GLOBALS ============================================================= */

#ifndef FWNAME
FLASH_DECLARE(AGC_SETTLE_TABLE_TABLE_DATA_TYPE
		agc_settle[AGC_SETTLE_TABLE_ROW_SIZE][AGC_SETTLE_TABLE_COL_SIZE])
	= AGC_SETTLE_TABLE;
#endif
#ifdef SUPPORT_FSK
FLASH_DECLARE(uint8_t fsk_params_tbl[48][11]) = FSK_PARAMS;
#   if (defined RF215v3)
FLASH_DECLARE(uint8_t fsk_pe_tbl[3 * 6]) = FSK_PE_TABLE;
#   endif
#endif

/* === PROTOTYPES ========================================================== */

#ifndef FWNAME
static uint16_t get_agc_settling_period(uint8_t sr, uint8_t avgs, uint8_t agci);

#endif

/* === IMPLEMENTATION ====================================================== */

#ifdef SUPPORT_OFDM

/**
 * @brief Configures RF according MR-OFDM
 *
 * @param trx_id Transceiver identifier
 * @param ofdm_opt OFDM option
 */
retval_t ofdm_rfcfg(ofdm_option_t ofdm_opt, trx_id_t trx_id)
{
	retval_t status = MAC_SUCCESS;
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	/* gather additional PHY configuration data */
	uint8_t lfo = bb_bit_read(reg_offset + SR_BBC0_OFDMC_LFO);    /* RX Low
	                                                               *frequency
	                                                               *offset
	                                                               *(TCXO)
	                                                               **/
	uint8_t spc = bb_bit_read(reg_offset + SR_BBC0_OFDMPHRRX_SPC); /* RX
	                                                                *spurious
	                                                                *compensation
	                                                                **/

	uint8_t sr, txrcut, txfc, rxrcut09, rxrcut24, bw09, bw24, ifs09, ifs24,
			agci, pdt;

	/* Note: all settings here are sourced from OFDM_DFE_configs.xlsx */
	switch (ofdm_opt) {
	case OFDM_OPT_1: /*OFDM Option 1: BW nom.: 1094 kHz */
		sr = 3; txrcut = 4; txfc = 10; rxrcut09 = 4; rxrcut24 = 4;
		bw09 = 9; bw24 = 10; ifs09 = 1; ifs24 = 1;
		agci = 0; pdt = 5; break;

	case OFDM_OPT_2: /*OFDM Option 2: BW nom.: 552 kHz */
		sr = 3; txrcut = 3; txfc = 8; rxrcut09 = 2; rxrcut24 = 2; bw09
			= 7; bw24 = 7; ifs09 = 1; ifs24 = 1; agci = 0;
		pdt = 5; break;

	case OFDM_OPT_3: /*OFDM Option 3: BW nom.: 281 kHz */
		sr = 6; txrcut = 3; txfc = 5; rxrcut09 = 2; rxrcut24 = 3; bw09
			= 4; bw24 = 4; ifs09 = 0; ifs24 = 0; agci = 0;
		pdt = 4; break;

	case OFDM_OPT_4: /*OFDM Option 4: BW nom.: 156 kHz */
		sr = 6; txrcut = 2; txfc = 3; rxrcut09 = 1; rxrcut24 = 1; bw09
			= 2; bw24 = 3; ifs09 = 1; ifs24 = 0; agci = 0;
		pdt = 3; break;

	default:
		/* ERROR: OFDM option musst be in [1..4]*/
		status = FAILURE;
		return status;

		break;
	}

	/* Tx settings: TXCUTC, TXDFE, PAC */
	uint8_t tx[3];
	/* TXCUTC */
	tx[0] = (0 << TXCUTC_PARAMP_SHIFT) | (txfc << TXCUTC_LPFCUT_SHIFT);
	/* TXDFE */
	tx[1] = (txrcut << TXDFE_RCUT_SHIFT) | (sr << TXDFE_SR_SHIFT);
	/* PAC */
	tx[2]
		= (3 <<
			PAC_PACUR_SHIFT) |
			(DEFAULT_TX_PWR_REG << PAC_TXPWR_SHIFT);
	rf_blk_write(reg_offset + RG_RF09_TXCUTC, tx, 3);

	/* Rx settings: RXBWC, RXDFE, AGCC, AGCS */
	uint8_t rx[4];
	if ((trx_id == RF09) || lfo) {
		/* RXBWC */
		rx[0] = (ifs09 << RXBWC_IFS_SHIFT) | (bw09 << RXBWC_BW_SHIFT);
		/* RXDFE */
		rx[1] = (rxrcut09 << RXDFE_RCUT_SHIFT) | (sr << RXDFE_SR_SHIFT);
	} else {
		/* RXBWC */
		rx[0] = (ifs24 << RXBWC_IFS_SHIFT) | (bw24 << RXBWC_BW_SHIFT);
		/* RXDFE */
		rx[1] = (rxrcut24 << RXDFE_RCUT_SHIFT) | (sr << RXDFE_SR_SHIFT);
	}

	/* AGCC */
	rx[2] = 0x81 | (agci << AGCC_AGCI_SHIFT) | (0 << AGCC_AVGS_SHIFT);
	/* AGCS */
	if (spc) { /* RX Spurious Compensation enabled? */
		rx[3] = (0 << AGCS_TGT_SHIFT) | (0x17 << AGCS_GCW_SHIFT); /* AGC
		                                                           *target:
		                                                           *-21
		                                                           *dB
		                                                           **/
	} else {
		rx[3] = (3 << AGCS_TGT_SHIFT) | (0x17 << AGCS_GCW_SHIFT); /* AGC
		                                                           *target:
		                                                           *-30
		                                                           *dB
		                                                           **/
	}

	rf_blk_write(reg_offset + RG_RF09_RXBWC, rx, 4);

	/* PDT threshold */
	bb_bit_write(reg_offset + SR_BBC0_OFDMSW_PDT, pdt);

#ifdef IQ_RADIO
	trx_bit_write(RF215_BB, reg_offset + SR_RF09_TXCUTC_PARAMP,
			RF_PARAMP32U);                                           /*
	                                                                          *PA
	                                                                          *rampup
	                                                                          *time
	                                                                          **/
#endif /* #ifdef IQ_RADIO */

#ifndef FWNAME
	tal_pib[trx_id].agc_settle_dur = get_agc_settling_period(sr, 0, agci);
#endif

	return status;
}

#endif /* #ifdef SUPPORT_OFDM */

#if (defined SUPPORT_LEGACY_OQPSK) || (defined SUPPORT_OQPSK)

/**
 * @brief Configures RF according MR-OQPSK
 *
 * @param trx_id Transceiver identifier
 * @param chip_rate OQPSK chip mode
 */
retval_t oqpsk_rfcfg(oqpsk_chip_rate_t chip_rate, trx_id_t trx_id)
{
	retval_t status = MAC_SUCCESS;
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

	uint8_t paramp, lpfcut, txrcut, sr, rxbw, rxrcut, avgs;
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
	uint8_t dm;
#endif

	switch (chip_rate) {
	case CHIP_RATE_100:    /* F_CHIP = 100 kHz */
		/* -- TX
		 *----------------------------------------------------------- */
		paramp = RF_PARAMP32U; /* PA ramp time 32 us */
		sr = 10; /* I/F rate: 0.4 Ms/sec */
		txrcut = 4; /* DFE cut-off: 1.0 */
		lpfcut = RF_FLC400KHZ; /* analog tx-filter */
		/* -- RX
		 *----------------------------------------------------------- */
		rxbw = RF_BW160KHZ_IF250KHZ; /* analog rx-filter */
		rxrcut = 1; /* DFE cut-off: 0.375 */
		avgs = 2; /* AGC setup: 32 samples */
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
		dm = 1; /* Direct modulation */
#endif
		break;

	case CHIP_RATE_200:    /* F_CHIP = 200 kHz */
		/* -- TX
		 *----------------------------------------------------------- */
		paramp = RF_PARAMP16U; /* PA ramp time 16 us */
		sr = 5; /* I/F rate: 0.8 Ms/sec */
		txrcut = 4; /* DFE cut-off: 1.0 */
		lpfcut = RF_FLC400KHZ; /* analog tx-filter */
		/* -- RX
		 *----------------------------------------------------------- */
		rxbw = RF_BW250KHZ_IF250KHZ; /* analog rx-filter */
		rxrcut = 1; /* DFE cut-off: 0.375 */
		avgs = 2; /* AGC setup: 32 samples */
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
		dm = 1; /* Direct modulation */
#endif
		break;

	case CHIP_RATE_1000:    /* F_CHIP = 1000 KHz */
		/* -- TX
		 *----------------------------------------------------------- */
		paramp = RF_PARAMP4U; /* PA ramp time 4 us */
		sr = 1; /* I/F rate: 4 Ms/sec */
		txrcut = 3; /* DFE cut-off: 0.75 */
		lpfcut = RF_FLC1000KHZ; /* analog tx-filter */
		/* -- RX
		 *----------------------------------------------------------- */
		rxbw = RF_BW1000KHZ_IF1000KHZ; /* analog rx-filter */
		rxrcut = 0; /* DFE cut-off: 0.25 */
		avgs = 0; /* AGC setup: 8 samples */
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
		dm = 0; /* Direct modulation */
#endif
		break;

	case CHIP_RATE_2000:   /* F_CHIP = 2000 KHz */
		/* -- TX
		 *----------------------------------------------------------- */
		paramp = RF_PARAMP4U; /* PA ramp time 4 us */
		sr = 1; /* I/F rate: 4 Ms/sec */
		txrcut = 4; /* DFE cut-off: 1.0 */
		lpfcut = RF_FLC1000KHZ; /* analog tx-filter */
		/* -- RX
		 *----------------------------------------------------------- */
		rxbw = RF_BW2000KHZ_IF2000KHZ; /* analog rx-filter */
		rxrcut = 2; /* DFE cut-off: 0.5 */
		avgs = 0; /* AGC setup: 8 samples */
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
		dm = 0; /* Direct modulation */
#endif
		break;

	default:
		/*ERROR: MODE_FCHIP  must be in [0..3]*/
		return FAILURE;

		break;
	}

	/* Tx settings: TXCUTC, TXDFE, PAC */
	uint8_t tx[3];
	/* TXCUTC */
	tx[0]
		= (paramp <<
			TXCUTC_PARAMP_SHIFT) | (lpfcut << TXCUTC_LPFCUT_SHIFT);
	/* TXDFE */
	tx[1] = (txrcut << TXDFE_RCUT_SHIFT) | (sr << TXDFE_SR_SHIFT);
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
	tx[1] |= dm << TXDFE_DM_SHIFT; /* Direct modulation */
#endif
	/* PAC */
	tx[2]
		= (3 <<
			PAC_PACUR_SHIFT) |
			(DEFAULT_TX_PWR_REG << PAC_TXPWR_SHIFT);
	rf_blk_write(reg_offset + RG_RF09_TXCUTC, tx, 3);

#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
	bb_bit_write(reg_offset + SR_BBC0_OQPSKC0_DM, dm);
#endif

	/* Rx settings: RXBWC, RXDFE, AGCC, AGCS */
	uint8_t rx[4];
	/* RXBWC */
	rx[0] = (0 << RXBWC_IFS_SHIFT) | (rxbw << RXBWC_BW_SHIFT);
	/* RXDFE */
	rx[1] = (rxrcut << RXDFE_RCUT_SHIFT) | (sr << RXDFE_SR_SHIFT);
	/* AGCC */
	rx[2] = 0x81 | (0 << AGCC_AGCI_SHIFT) | (avgs << AGCC_AVGS_SHIFT); /*
	                                                                    *AGC
	                                                                    *input:
	                                                                    *after
	                                                                    *channel
	                                                                    *filter
	                                                                    **/
	rx[3] = (3 << AGCS_TGT_SHIFT) | (0x17 << AGCS_GCW_SHIFT); /* AGC target:
	                                                           *-30 dB */
	rf_blk_write(reg_offset + RG_RF09_RXBWC, rx, 4);

#ifdef IQ_RADIO
	/* PA ramp time 32 us to BB(!) */
	trx_bit_write(RF215_BB, reg_offset + SR_RF09_TXCUTC_PARAMP,
			RF_PARAMP32U);
#endif /* #ifdef IQ_RADIO */

#ifndef FWNAME
	tal_pib[trx_id].agc_settle_dur = get_agc_settling_period(sr, avgs, 0);
#endif

	return status;
}

#endif /* #if (defined SUPPORT_LEGACY_OQPSK) || (defined SUPPORT_OQPSK) */

#ifdef SUPPORT_FSK

/**
 * @brief Configures RF according FSK
 *
 * @param mod_type Modulation order / type; i.e. F2FSK or F4FSK
 * @param srate Symbol rate
 * @param mod_idx Modulation index
 * @param trx_id Transceiver identifier
 */
retval_t fsk_rfcfg(fsk_mod_type_t mod_type, fsk_sym_rate_t srate,
		mod_idx_t mod_idx, trx_id_t trx_id)
{
	retval_t status = MAC_SUCCESS;
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	uint8_t srate_midx = (srate << 3) + mod_idx;
	uint8_t temp[2];

	/* TX configuration: */
	/* - PA ramp time +  TX-SSBF fcut */
	/* - DFE sampling rate reduction + TX-DFE fcut */
	PGM_READ_BLOCK(temp, (uint8_t *)&fsk_params_tbl[srate_midx][0], 2);
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
	temp[1] |= 1 << TXDFE_DM_SHIFT; /* DM */
#endif
	rf_blk_write(reg_offset + RG_RF09_TXCUTC, temp, 2);
#if ((!defined RF215v1) && (defined DIRECT_MODULATION))
	uint8_t pe[4];
	pe[0] = (1 << FSKDM_EN_SHIFT) | (1 << FSKDM_PE_SHIFT); /* PE = 1, DM = 1
	                                                        **/
	PGM_READ_BLOCK((uint8_t *)&pe[1],
			(uint8_t *)&fsk_pe_tbl[3 * (uint8_t)srate], 3);
	bb_blk_write(reg_offset + RG_BBC0_FSKDM, pe, 4);
#endif

	/* - Transmit Power*/
#ifdef IQ_RADIO
	trx_reg_write(RF215_RF, reg_offset + RG_RF09_PAC,
			((3 <<
			PAC_PACUR_SHIFT) |
			(DEFAULT_TX_PWR_REG << PAC_TXPWR_SHIFT)));
#else
	trx_reg_write( reg_offset + RG_RF09_PAC,
			((3 <<
			PAC_PACUR_SHIFT) |
			(DEFAULT_TX_PWR_REG << PAC_TXPWR_SHIFT)));
#endif

	/* RX configuration: */
	/* - RX-SSBF bandwidth + RX-SSBF IF shift */
	/* - DFE sampling rate reduction  + RX-DFE cut-off ratio */
	if (trx_id == RF09) {
		PGM_READ_BLOCK(temp, (uint8_t *)&fsk_params_tbl[srate_midx][5],
				2);
	} else {
		PGM_READ_BLOCK(temp, (uint8_t *)&fsk_params_tbl[srate_midx][7],
				2);
	}

	rf_blk_write(reg_offset + RG_RF09_RXBWC, temp, 2);

	/* - AGC input + AGC average period */
	/* - AGC target */
	PGM_READ_BLOCK(temp, (uint8_t *)&fsk_params_tbl[srate_midx][9], 2);
	rf_blk_write(reg_offset + RG_RF09_AGCC, temp, 2);

#ifndef FWNAME
	uint8_t agcc = (uint8_t)PGM_READ_BYTE(&fsk_params_tbl[srate_midx][9]);
	uint8_t agci = (agcc & AGCC_AGCI_MASK) >> AGCC_AGCI_SHIFT;
	uint8_t avgs = (agcc & AGCC_AVGS_MASK) >> AGCC_AVGS_SHIFT;
	uint8_t rxdfe;
	if (trx_id == RF09) {
		rxdfe = (uint8_t)PGM_READ_BYTE(&fsk_params_tbl[srate_midx][6]);
	} else {
		rxdfe = (uint8_t)PGM_READ_BYTE(&fsk_params_tbl[srate_midx][8]);
	}

	uint8_t sr = rxdfe & RXDFE_SR_MASK;
	tal_pib[trx_id].agc_settle_dur
		= get_agc_settling_period(sr, avgs, agci);
#endif

	/* Keep compiler happy */
	mod_type = mod_type;

	return status;
}

#endif /* #ifdef SUPPORT_FSK */

#ifndef FWNAME

/**
 * @brief Gets the AGC settling period
 *
 * @param sr    Sample rate
 * @param avgs  AGC average time in number of samples
 * @param agci  AGC input
 */
static uint16_t get_agc_settling_period(uint8_t sr, uint8_t avgs, uint8_t agci)
{
	uint16_t period = 0;

	for (uint8_t i = 0; i < AGC_SETTLE_TABLE_ROW_SIZE; i++) {
		if (sr == (uint8_t)PGM_READ_BYTE(&agc_settle[i][0])) {
			uint8_t idx = 1 + 2 * avgs + agci;
			period = (uint8_t)PGM_READ_BYTE(&agc_settle[i][idx]);
			break;
		}
	}

	period *= 2;

	return period;
}

#endif

/* EOF */
