/**
 * @file tal_mode_switch.c
 *
 * @brief This file handles the mode switch feature
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
#include "ieee_const.h"
#include "tal_config.h"
#include "tal_pib.h"
#include "tal_internal.h"
#include "ieee_154g.h"

#if (defined SUPPORT_FSK) && (defined SUPPORT_MODE_SWITCH)

/* === TYPES =============================================================== */

typedef struct fsk_pib_tag {
	uint16_t FSKPreambleLength : 9;
	uint16_t FSKFECEnabled : 1;
	uint16_t FSKFECInterleavingRSC : 1;
	uint16_t FSKFECScheme : 1;
	uint16_t MRFSKSFD : 1;
	uint16_t FSKScramblePSDU : 1;
} fsk_pib_t;

typedef struct temp_phy_tag {
	phy_t phy;
	fsk_pib_t pib;
	rate_t rate;
} temp_phy_t;

typedef struct ms_phr_tag {
	uint16_t ms : 1;
	uint16_t ms_param_entry : 2;
	uint16_t new_fec : 1;
	uint16_t new_mode : 7;
	uint16_t checksum : 4;
	uint16_t pc : 1;
} ms_phr_t;

/* === MACROS ============================================================== */

#define MS(x)   (x & 0x0001)
#define M1(x)   ((x >> 1) & 0x0001)
#define M0(x)   ((x >> 2) & 0x0001)
#define FEC(x)  ((x >> 3) & 0x0001)
#define PAGE(x) ((x >> 4) & 0x0001)
#define MOD1(x) ((x >> 5) & 0x0001)
#define MOD0(x) ((x >> 6) & 0x0001)
#define MD3(x)  ((x >> 7) & 0x0001)
#define MD2(x)  ((x >> 8) & 0x0001)
#define MD1(x)  ((x >> 9) & 0x0001)
#define MD0(x)  ((x >> 10) & 0x0001)

#define OQPSK_CHIP_RATE_REGION_TABLE \
        /* rows: freq band ; column: chip rate (kchip/s) * 10 */ \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 10 }, \
	{ CHINA_780, 100 }, \
	{ EU_863, 10 },	\
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 100 }, \
	{ KOREA_917, 100 }, \
	{ JAPAN_920, 10 }, \
        /* US_928 */ \
        /* { JAPAN_950, 32 }, */ \
        /* US_1427 */ \
	{ WORLD_2450, 200 }

#define OQPSK_CHIP_RATE_REGION_TABLE_ROW_SIZE 7
#define OQPSK_CHIP_RATE_REGION_TABLE_COL_SIZE 2
#define OQPSK_CHIP_RATE_REGION_TABLE_DATA_TYPE  uint8_t

/* === GLOBALS ============================================================= */

static temp_phy_t previous_phy[NUM_TRX];
static phy_t csm_phy;
bool csm_active[NUM_TRX] = {false, false};
#ifdef SUPPORT_OQPSK
FLASH_DECLARE(OQPSK_CHIP_RATE_REGION_TABLE_DATA_TYPE
		oqpsk_chip_rate_region_table[
			OQPSK_CHIP_RATE_REGION_TABLE_ROW_SIZE][
			OQPSK_CHIP_RATE_REGION_TABLE_COL_SIZE]) =
{OQPSK_CHIP_RATE_REGION_TABLE};
#endif

/* === PROTOTYPES ========================================================== */

static void tx_actual_frame(void *cb_timer_element);
static void configure_new_tx_mode(trx_id_t trx_id);
static void cancel_new_mode_reception(void *cb_timer_element);
static retval_t convert_fsk_op_mode_to_data_rate(fsk_op_mode_t op_mode,
		sun_freq_band_t band, fsk_sym_rate_t *rate,
		fsk_mod_type_t *type);
static inline uint16_t create_mode_switch_phr(trx_id_t trx_id);

#ifdef SUPPORT_OQPSK
static uint16_t oqpsk_get_chip_rate_region(trx_id_t trx_id);
static oqpsk_chip_rate_t convert_oqpsk_chip_rate_to_register(uint16_t chip_rate);

#endif
static inline void download_ms_ppdu(trx_id_t trx_id);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Initialize the TAL PIB
 *
 * This function initializes the TAL information base attributes
 * to their default values.
 */
void init_mode_switch(void)
{
	/* Configure phy for CSM */
	csm_phy.modulation = FSK;
	csm_phy.phy_mode.fsk.mod_type = F2FSK;
	csm_phy.phy_mode.fsk.mod_idx = MOD_IDX_1_0;
	csm_phy.phy_mode.fsk.sym_rate = FSK_SYM_RATE_50;
}

/* --- TX ------------------------------------------------------------------ */

/**
 * @brief Sets the common signaling mode
 *
 * @param trx_id Transceiver identifier
 */
void set_csm(trx_id_t trx_id)
{
	if (csm_active[trx_id] == false) {
		if (trx_state[trx_id] != RF_TXPREP) {
			switch_to_txprep(trx_id);
		}

		/* Configure phy for CSM */
		memcpy(&tal_pib[trx_id].phy, &csm_phy, sizeof(phy_t));

		tal_pib[trx_id].FSKFECEnabled = false;
		tal_pib[trx_id].FSKFECInterleavingRSC = false;
		tal_pib[trx_id].FSKFECScheme = FEC_SCHEME_NRNSC;
		tal_pib[trx_id].FSKPreambleLength = 8;
		tal_pib[trx_id].MRFSKSFD = 0;
		tal_pib[trx_id].FSKScramblePSDU = false;

		/* Apply new settings */
		set_fsk_pibs(trx_id);

		/* Disable raw mode */
		configure_raw_mode(trx_id, false);

		conf_fsk(trx_id);
		csm_active[trx_id] = true;
	} else {
	}
}

/**
 * @brief Save/Stores current PHY to variable
 *
 * @param trx_id Transceiver identifier
 */
void save_current_phy(trx_id_t trx_id)
{
	memcpy(&previous_phy[trx_id].phy, &tal_pib[trx_id].phy, sizeof(phy_t));

	previous_phy[trx_id].pib.FSKFECEnabled = tal_pib[trx_id].FSKFECEnabled;
	previous_phy[trx_id].pib.FSKFECInterleavingRSC
		= tal_pib[trx_id].FSKFECInterleavingRSC;
	previous_phy[trx_id].pib.FSKFECScheme = tal_pib[trx_id].FSKFECScheme;
	previous_phy[trx_id].pib.FSKPreambleLength
		= tal_pib[trx_id].FSKPreambleLength;
	previous_phy[trx_id].pib.MRFSKSFD = tal_pib[trx_id].MRFSKSFD;
	previous_phy[trx_id].pib.FSKScramblePSDU
		= tal_pib[trx_id].FSKScramblePSDU;

#ifdef SUPPORT_OFDM
	if (tal_pib[trx_id].ModeSwitchNewMode.modulation == OFDM) {
		previous_phy[trx_id].rate.ofdm_mcs = tal_pib[trx_id].OFDMMCS;
	}

#endif
#ifdef SUPPORT_OQPSK
	if (tal_pib[trx_id].ModeSwitchNewMode.modulation == OQPSK) {
		previous_phy[trx_id].rate.oqpsk_rate_mod
			= tal_pib[trx_id].OQPSKRateMode;
	}

#endif
}

/**
 * @brief Downloads the mode switch PPDU to a transceiver
 *
 * @param trx_id Transceiver identifier
 */
static inline void download_ms_ppdu(trx_id_t trx_id)
{
	/* fill length field */
	uint16_t len = 2; /* fixed size - no CRC */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_write( reg_offset + RG_BBC0_TXFLL, (uint8_t *)&len, 2);

	/* fill frame buffer */
	uint16_t phr = create_mode_switch_phr(trx_id);
	uint16_t tx_frm_buf_offset = BB_TX_FRM_BUF_OFFSET * trx_id;
	trx_write( tx_frm_buf_offset + RG_BBC0_FBTXS, (uint8_t *)&phr, 2);
}

/**
 * @brief Transmits the mode switch PPDU
 *
 * @param trx_id Transceiver identifier
 */
void tx_ms_ppdu(trx_id_t trx_id)
{
	if (trx_state[trx_id] != RF_TXPREP) {
		switch_to_txprep(trx_id);
	}

	/* Configure auto modes: Disable CCATX, AACK and TX2RX */
	/* Other auto mode settings can be set to 0 */
	uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
	trx_reg_write(reg_offset + RG_BBC0_AMCS, 0);

	/* Enable raw mode */
	configure_raw_mode(trx_id, true);

	download_ms_ppdu(trx_id);

#ifdef IQ_RADIO
	if (trx_id == RF09) {
		/* Check if the other radio is currently in use */
		if (trx_state[RF24] == RF_TX) {
		}
	} else {
		/* Check if the other radio is currently in use */
		if (trx_state[RF09] == RF_TX) {
		}
	}
	trx_bit_write(RF215_BB, SR_RF_IQIFC2_CSELTX, trx_id);
#endif

	trx_reg_write( reg_offset + RG_RF09_CMD, RF_TX);
	trx_state[trx_id] = RF_TX;

	tx_state[trx_id] = TX_MS_PPDU;

#if (defined ENABLE_TSTAMP) || (defined MEASURE_ON_AIR_DURATION)
	pal_get_current_time(&fs_tstamp[trx_id]);
#endif

	frame_buf_filled[trx_id] = false;
}

/**
 * @brief Creates the mode switch PPDU
 *
 * @param trx_id Transceiver identifier
 *
 * @return Mode switch PPDU
 */
static inline uint16_t create_mode_switch_phr(trx_id_t trx_id)
{
	/* Create PHR */
	uint16_t phr = 0x0001; /* Mode switch bit */
	ms_phr_t *ms_phr;
	ms_phr = (ms_phr_t *)&phr;

	/* Mode switch parameter entry */

	/* New mode fec */
	if (tal_pib[trx_id].ModeSwitchNewMode.fec_enabled) {
		ms_phr->new_fec = 1;
	}

	/* New mode field; Page field is kept to 0. */
	uint8_t modu = tal_pib[trx_id].ModeSwitchNewMode.modulation;
	modu = ((modu & 0x01) << 1) | (modu >> 1); /* MSB instead of LSB */
	ms_phr->new_mode = modu << 1;
	/* Mode field */
	uint8_t op = 0;
	switch (tal_pib[trx_id].ModeSwitchNewMode.modulation) {
	case FSK:
		op = tal_pib[trx_id].ModeSwitchNewMode.phy_mode.fsk.op_mode;
		break;

#ifdef SUPPORT_OFDM
	case OFDM:
		/* For over-the-air encoding see table 68i */
		op = tal_pib[trx_id].ModeSwitchNewMode.phy_mode.ofdm.option - 1;
		break;

#endif
#ifdef SUPPORT_OQPSK
	case OQPSK:
		/* keep op zero */
		break;

#endif
	default:
		break;
	}
	/* MSB instead of LSB only required for the lowest to bits; other bits
	 *are unused */
	uint8_t md = ((op & 0x01) << 3) | ((op & 0x02) << 1); /* | ((op & 0x04)
	                                                       * >> 1) | ((op &
	                                                       * 0x08) >> 3); */
	ms_phr->new_mode |= md << 3;

	/* Checksum */
	/* @ToDo */

	/* Parity check */
	/* @ToDo */

	return phr;
}

/**
 * @brief Prepares/Schedules actual transmission after mode switch PPDU
 *
 * @param trx_id Transceiver identifier
 */
void prepare_actual_transmission(trx_id_t trx_id)
{
	uint8_t timer_id;
	if (trx_id == RF09) {
		timer_id = TAL_T_0;
	} else {
		timer_id = TAL_T_1;
	}

	uint32_t now;
	pal_get_current_time(&now);
	uint32_t diff = tal_pib[trx_id].ModeSwitchSettlingDelay -
			(now - rxe_txe_tstamp[trx_id]);
	retval_t status
		= pal_timer_start(timer_id,
			diff,
			TIMEOUT_RELATIVE,
			(FUNC_PTR())tx_actual_frame,
			(void *)trx_id);

	if (status == MAC_SUCCESS) {
		configure_new_tx_mode(trx_id);
		tx_state[trx_id] = TX_WAIT_FOR_NEW_MODE_TRANSMITTING;
	} else {
		tx_done_handling(trx_id, status);
	}
}

/**
 * @brief Configures new PHY mode after transmitting/receiving mode switch PPDU
 *
 * @param trx_id Transceiver identifier
 */
static void configure_new_tx_mode(trx_id_t trx_id)
{
	/* Configure new mode */
	csm_active[trx_id] = false;

	/* Disable raw mode */
	configure_raw_mode(trx_id, false);

	/* Check if ACK is requested */
	if (*mac_frame_ptr[trx_id]->mpdu & FCF_ACK_REQUEST) {
		uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
		trx_bit_write( reg_offset + SR_BBC0_AMCS_TX2RX, 1);
	}

	tal_pib[trx_id].phy.modulation
		= tal_pib[trx_id].ModeSwitchNewMode.modulation;
	memcpy(&tal_pib[trx_id].phy.phy_mode,
			&tal_pib[trx_id].ModeSwitchNewMode.phy_mode,
			sizeof(phy_t));

#ifdef SUPPORT_OFDM
	if (tal_pib[trx_id].ModeSwitchNewMode.modulation == OFDM) {
		tal_pib[trx_id].OFDMMCS
			= tal_pib[trx_id].ModeSwitchNewMode.rate.
				ofdm_mcs;
	}

#endif
#ifdef SUPPORT_OQPSK
	if (tal_pib[trx_id].ModeSwitchNewMode.modulation == OQPSK) {
		tal_pib[trx_id].OQPSKRateMode
			= tal_pib[trx_id].ModeSwitchNewMode.rate.
				oqpsk_rate_mod;
	}

#endif

	conf_trx_modulation(trx_id);
}

/**
 * @brief Transmits actual frame following mode switch PPDU
 *
 * @param parameter Contains transceiver identifier
 */
static void tx_actual_frame(void *cb_timer_element)
{
	/* Immediately store trx id from callback. */
	trx_id_t trx_id = (trx_id_t)cb_timer_element;
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

#ifndef BASIC_MODE
	transmit_frame(trx_id, NO_CCA);
#else
	transmit_frame(trx_id);
#endif

	tx_state[trx_id] = TX_MS_NEW_MODE_TRANSMITTING;
}

/**
 * @brief Restores previous PHY
 *
 * @param trx_id Transceiver identifier
 */
void restore_previous_phy(trx_id_t trx_id)
{
	memcpy(&tal_pib[trx_id].phy, &previous_phy[trx_id].phy, sizeof(phy_t));

	tal_pib[trx_id].FSKFECEnabled = previous_phy[trx_id].pib.FSKFECEnabled;
	tal_pib[trx_id].FSKFECInterleavingRSC
		= previous_phy[trx_id].pib.FSKFECInterleavingRSC;
	tal_pib[trx_id].FSKFECScheme = previous_phy[trx_id].pib.FSKFECScheme;
	tal_pib[trx_id].FSKPreambleLength
		= previous_phy[trx_id].pib.FSKPreambleLength;
	tal_pib[trx_id].MRFSKSFD = previous_phy[trx_id].pib.MRFSKSFD;
	tal_pib[trx_id].FSKScramblePSDU
		= previous_phy[trx_id].pib.FSKScramblePSDU;

	set_fsk_pibs(trx_id);

#ifdef SUPPORT_OFDM
	if (tal_pib[trx_id].ModeSwitchNewMode.modulation == OFDM) {
		tal_pib[trx_id].OFDMMCS = previous_phy[trx_id].rate.ofdm_mcs;
	}

#endif
#ifdef SUPPORT_OQPSK
	if (tal_pib[trx_id].ModeSwitchNewMode.modulation == OQPSK) {
		tal_pib[trx_id].OQPSKRateMode
			= previous_phy[trx_id].rate.oqpsk_rate_mod;
	}

#endif

	/* Apply new settings */

	conf_fsk(trx_id);
	csm_active[trx_id] = false;
}

/* --- RX ------------------------------------------------------------------ */

/**
 * @brief Converts FSK operation mode to data rate
 *
 * @param[in] op_mode Operation mode
 * @param[in] band SUN Frequency band
 * @param[out] rate Data rate
 * @param[out] type FSK modulation type
 *
 * @return Status
 */
static retval_t convert_fsk_op_mode_to_data_rate(fsk_op_mode_t op_mode,
		sun_freq_band_t band,
		fsk_sym_rate_t *rate, fsk_mod_type_t *type)
{
	retval_t ret = MAC_SUCCESS;
	*type = F2FSK;

	switch (op_mode) {
	case FSK_OP_MOD_1:
		*rate = FSK_SYM_RATE_50;
		break;

	case FSK_OP_MOD_2:
		switch (band) {
		case US_915:
		case KOREA_917:
		case WORLD_2450:
			*rate = FSK_SYM_RATE_150;
			break;

		default:
			*rate = FSK_SYM_RATE_100;
			break;
		}
		break;

	case FSK_OP_MOD_3:
		*rate = FSK_SYM_RATE_200;
		switch (band) {
		case CHINA_470:
		case CHINA_780:
		case EU_863:
			*type = F4FSK;
			break;

		default:
			break;
		}
		break;

	case FSK_OP_MOD_4:
		*rate = FSK_SYM_RATE_400;
		switch (band) {
		case JAPAN_920:
		case JAPAN_950:
			*type = F4FSK;
			break;

		default:
			ret = FAILURE;
			break;
		}
		break;

	default:
		ret = FAILURE;
		break;
	}

	return ret;
}

/**
 * @brief Handles received mode switch frame
 *
 * @param trx_id Transceiver identifier
 */
void handle_rx_ms_packet(trx_id_t trx_id)
{
	/* Upload mode switch PHR */
	uint16_t phr;
	uint16_t rx_frm_buf_offset = BB_RX_FRM_BUF_OFFSET * trx_id;
	trx_read( rx_frm_buf_offset + RG_BBC0_FBRXS, (uint8_t *)&phr, 2);

	/* BCH calculation */
	/* @ToDo */

	/* Verify received mode switch packet */
	/* @ToDo */

	/*
	 * uint16_t pc = MS(phr) ^ M0(phr) ^ M1(phr) ^ FEC(phr) ^ PAGE(phr) ^
	 * MOD0(phr) \
	 *            ^ MOD1(phr) ^ MD0(phr) ^ MD1(phr) ^ MD2(phr) ^ MD3(phr);
	 */
	if (PAGE(phr) == 1) {
		/* No generic PHY support */
		switch_to_rx(trx_id);
		return;
	}

	/* Switch to new mode for frame reception */

	/* Parse new mode */
	modulation_t modulation = (modulation_t)(MOD0(phr) | (MOD1(phr) << 1));

	uint8_t mode = MD0(phr) | (MD1(phr) << 1) | (MD2(phr) << 2);

	phy_t temp_phy;
	memcpy(&temp_phy, &tal_pib[trx_id].phy, sizeof(phy_t));

	bool support_flag = true;
	switch (modulation) {
	case FSK:
	{
		fsk_sym_rate_t rate;
		fsk_mod_type_t type;

		if (convert_fsk_op_mode_to_data_rate((fsk_op_mode_t)mode,
				tal_pib[trx_id].phy.freq_band, &rate,
				&type) != MAC_SUCCESS) {
			/* Unsupported feature */

			support_flag = false;
			break;
		}

		tal_pib[trx_id].phy.phy_mode.fsk.sym_rate = rate;
		tal_pib[trx_id].phy.phy_mode.fsk.mod_type = type;
		tal_pib[trx_id].phy.phy_mode.fsk.mod_idx = MOD_IDX_1_0;
	}
	break;

#ifdef SUPPORT_OFDM
	case OFDM:
		/* For over-the-air encoding see table 68i */
		tal_pib[trx_id].phy.phy_mode.ofdm.option
			= (ofdm_option_t)(mode + 1);
		break;
#endif

#ifdef SUPPORT_OQPSK
	case OQPSK:

		/* Chip rate depends on current frequency band and region */
	{
		uint16_t rate = oqpsk_get_chip_rate_region(trx_id);
		oqpsk_chip_rate_t chip_rate
			= convert_oqpsk_chip_rate_to_register(
				rate);
		tal_pib[trx_id].phy.phy_mode.oqpsk.chip_rate = chip_rate;
	}
	break;
#endif

	default:
		/* Unsupported feature */
		support_flag = false;
		break;
	}

	if (support_flag) {
		tal_pib[trx_id].phy.modulation = modulation;

		if (conf_trx_modulation(trx_id) == MAC_SUCCESS) {
			tal_state[trx_id] = TAL_NEW_MODE_RECEIVING;
			uint8_t timer_id;
			if (trx_id == RF09) {
				timer_id = TAL_T_0;
			} else {
				timer_id = TAL_T_1;
			}

			/* Start timer to cancel receiving at new mode again -
			 *in case no frame is received. */
			pal_timer_start(timer_id,
					tal_pib[trx_id].ModeSwitchDuration,
					TIMEOUT_RELATIVE,
					(FUNC_PTR())cancel_new_mode_reception,
					(void *)trx_id);
		} else {
			support_flag = false;
		}
	}

	if (support_flag == false) {
		/* Restore previous settings */
		memcpy(&tal_pib[trx_id].phy, &temp_phy, sizeof(phy_t));
	}

	/* Switch the receiver on again. */
	switch_to_rx(trx_id);
}

/**
 * @brief Cancels reception of the frame using new PHY mode
 *
 * @param parameter Transceiver identifier
 */
static void cancel_new_mode_reception(void *cb_timer_element)
{
	/* Immediately store trx id from callback. */
	trx_id_t trx_id = (trx_id_t)cb_timer_element;
	Assert((trx_id >= 0) && (trx_id < NUM_TRX));

	/* Restore previous PHY, i.e. CSM */
	set_csm(trx_id);

	tal_state[trx_id] = TAL_IDLE;
	switch_to_rx(trx_id);
}

/**
 * @brief Gets MR-O-QPSK chip rate based on local region
 *
 * @param trx_id Transceiver identifier
 *
 * @return chip rate
 */
#ifdef SUPPORT_OQPSK
/* table 166, pg. 101 */
static uint16_t oqpsk_get_chip_rate_region(trx_id_t trx_id)
{
	uint16_t rate = 0;

	for (uint8_t i = 0; i < OQPSK_CHIP_RATE_REGION_TABLE_ROW_SIZE; i++) {
		if (tal_pib[trx_id].phy.freq_band ==
				(uint8_t)PGM_READ_BYTE(&
				oqpsk_chip_rate_region_table
				[i][0])) {
			rate = 10 * (uint16_t)PGM_READ_BYTE(
					&oqpsk_chip_rate_region_table[i][1]);
			break;
		}
	}

	return rate;
}

#endif

/**
 * @brief Converts MR-OQPSK chip rate to transceiver register value
 *
 * @param chip_rate CHip rate
 *
 * @return register value
 */
#ifdef SUPPORT_OQPSK
static oqpsk_chip_rate_t convert_oqpsk_chip_rate_to_register(uint16_t chip_rate)
{
	oqpsk_chip_rate_t reg_value = CHIP_RATE_100;

	switch (chip_rate) {
	case 100:
		reg_value = CHIP_RATE_100;
		break;

	case 200:
		reg_value = CHIP_RATE_200;
		break;

	case 1000:
		reg_value = CHIP_RATE_1000;
		break;

	case 2000:
		reg_value = CHIP_RATE_2000;
		break;

	default:
		break;
	}

	return reg_value;
}

#endif

#endif /* #if (defined SUPPORT_FSK) && (defined SUPPORT_MODE_SWITCH) */

/* EOF */
