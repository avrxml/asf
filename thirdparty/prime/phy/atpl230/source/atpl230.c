/**
 * \file
 *
 * \brief ATPL230 Physical layer
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 *
 */

/* System includes */
#include <stdio.h>
#include <string.h>

/* Phy includes */
#include "atpl230.h"
#include "atpl230_reg.h"
#include "atpl230_version.h"
#include "atpl230_iir_filter.h"
#include "conf_atpl230.h"
#include "conf_phy.h"

/* ASF includes */
#include "asf.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \weakgroup phy_plc_group
 * @{
 */

/* Management Function PHY interrupt */
void phy_handler(void);

/* Default empty services */
void Dummy_serial_if_init(void);
void Dummy_sniffer_if_init(uint8_t uc_enable_led);

#ifdef __GNUC__
void serial_if_init( void ) __attribute__ ((weak, alias("Dummy_serial_if_init")));
void sniffer_if_init( uint8_t uc_enable_led ) __attribute__ ((weak, alias("Dummy_sniffer_if_init")));

#endif

#ifdef __ICCARM__
extern void serial_if_init(void);
extern void sniffer_if_init(uint8_t uc_enable_led);

#pragma weak serial_if_init=Dummy_serial_if_init
#pragma weak sniffer_if_init=Dummy_sniffer_if_init
#endif

/* Internal Rx buf struct to manage RX indication */
static uint8_t puc_phy_rx_buffer_event[4]; /* 4(rx) */
/* Internal Tx buf struct to manage TX result cfm */
static uint8_t puc_phy_tx_buffer_event[4]; /* 4(tx) */
/* Internal Rx noise buf struct to manage Noise capture */
static uint8_t puc_phy_noise_buffer_event;

/* static uint8_t uc_event_tx_result; */
/* static atpl230QueueRx_t x_rx_msg_uc_queue[5]; // 4(rx) + 1 (ns) */
static uint8_t uc_last_rx_buf;
static uint8_t uc_last_tx_buf;

/* Configuration of serial interfaces */
static uint8_t uc_serial_ifaces_cfg;

/* \name Reset values for the initial addresses data frames */
/* @{ */
#define REG_ATPL230_PHY_TX_INIT_ADDRESS      0x0000
#define REG_ATPL230_PHY_RX_INIT_ADDRESS     (REG_ATPL230_PHY_TX_INIT_ADDRESS + PHY_MAX_PPDU_SIZE * PHY_NUM_TX_BUFFERS)
#define REG_ATPL230_PHY_NOISE_INIT_ADDRESS   0x0000
/* @} */

/* \name Max mac pdu size */
/* @{ */
#define MAC_GEN_HEADER_SIZE     9
#define MAC_HEADER_SIZE         7
#define PHY_DMA_OFFSET          16
/* @} */

/* \name Attenuation values */
/* @{ */
#define ATT_0dB       0xFF
#define ATT_1dB       0xE3
#define ATT_2dB       0xCA
#define ATT_3dB       0xB4
#define ATT_4dB       0xA0
#define ATT_5dB       0x8F
#define ATT_6dB       0x7F
#define ATT_7dB       0x71
#define ATT_8dB       0x65
#define ATT_9dB       0x5A
#define ATT_10dB      0x50
#define ATT_11dB      0x47
#define ATT_12dB      0x40
#define ATT_13dB      0x39
#define ATT_14dB      0x32
#define ATT_15dB      0x2D
#define ATT_16dB      0x28
#define ATT_17dB      0x24
#define ATT_18dB      0x20
#define ATT_19dB      0x1C
#define ATT_20dB      0x19
#define ATT_21dB      0x16
/* @} */

/* Minimum Output Level */
#define MOL_MINIMUM                        21

/* \name Transmission timeout in miliseconds */
/* @{ */
#define PHY_TX_TIMEOUT_IMMEDIATE           750   /* 750 ms */
#define PHY_TX_TIMEOUT_IMMEDIATE_10US      PHY_TX_TIMEOUT_IMMEDIATE * 100
#define PHY_TX_TIMEOUT_DELAYED             1500  /* 1.5 second */
#define PHY_TX_TIMEOUT_DELAYED_10US        PHY_TX_TIMEOUT_DELAYED * 100
/* @} */

/* Minimum delay for atpl230 internal timer (in 10 us) */
#define PHY_TX_MIN_DELAY        10      /* 100 us. */

/* \name Maximum symbol length for every modulation scheme */
/* @{ */
#define MAX_LEN_DBPSK            39
#define MAX_LEN_DBPSK_VTB        63
#define MAX_LEN_DQPSK            18
#define MAX_LEN_DQPSK_VTB        39
#define MAX_LEN_D8PSK            11
#define MAX_LEN_D8PSK_VTB        25
#define MAX_LEN_DBPSK_ROBO       (MAX_LEN_DBPSK_VTB << 2)
#define MAX_LEN_DQPSK_ROBO       (MAX_LEN_DQPSK_VTB << 2)
/* @} */

/* Number of CRC types */
#define VCRC_TYPES_NUMBER  4

/* \name VCRC configuration for CRC8 */
/* @{ */
#define VCRC8_POLYNOM       0x00000007
#define VCRC8_RST           0x00000000
#define VCRC8_CONF          0xC0
/* @} */

/* \name VCRC configuration for CRC16 */
/* @{ */
#define VCRC16_POLYNOM       0x00000107
#define VCRC16_RST           0x00000000
#define VCRC16_CONF          0xC1
/* @} */

/* \name VCRC configuration for CRC24 */
/* @{ */
#define VCRC24_POLYNOM       0x00010107
#define VCRC24_RST           0x00000000
#define VCRC24_CONF          0xC2
/* @} */

/* \name VCRC configuration for CRC32 */
/* @{ */
#define VCRC32_POLYNOM       0x04C11DB7
#define VCRC32_RST           0x00000000
#define VCRC32_CONF          0xC3
/* @} */

/* \name configuration structures */
/* @{ */
atpl230ChnCfg_t atpl230ChnCfg;
atpl230Cfg_t atpl230Cfg;
/* @} */

/* internal status */
static atpl230_t atpl230;

/* Filter data */
/* static NotchParams x_notchParams; */
extern uint8_t uc_data_filter_IIR [LENGTH_DATA_FILTER_IIR];
extern uint8_t uc_data_chirp [LENGTH_DATA_CHIRP];
extern uint8_t uc_data_angle_real_imag_comp [LENGTH_DATA_ANGLE_REAL_IMAG_COMP];
extern uint32_t ul_data_offset_correction [NUM_ROWS_DATA_OFFSET_CORRECTION];
extern const float f_escalado_a22;
extern const float f_escalado_a23;
extern const float f_escalado_b22;
extern const float f_escalado_b23;

/* \name Attenuation variables */
/* @{ */
static uint8_t phyTxAttChirpHighZ;     /* Attenuation chirp in High impedance */
static uint8_t phyTxAttSignalHighZ;    /* Attenuation signal in High impedance */
static uint8_t phyTxAttChirpLowZ;      /* Attenuation chirp in low impedance */
static uint8_t phyTxAttSignalLowZ;     /* Attenuation signal in low impedance */
static uint8_t phyTxAttChirpVLowZ;     /* Attenuation chirp in very low impedance */
static uint8_t phyTxAttSignalVLowZ;    /* Attenuation signal in very low impedance */
static uint16_t phyTxLoadThreshold1;   /* Threshold for RMS calculated to detect load type */
static uint16_t phyTxLoadThreshold2;   /* Threshold for RMS calculated to detect load type */
static uint16_t phyTxLoadThreshold3;   /* Threshold for RMS calculated to detect load type */
static uint16_t phyTxLoadThreshold4;   /* Threshold for RMS calculated to detect load type */
/* @} */

/* \name interrupt buffer flags */
/* @{ */
static uint8_t uc_reg_rx_int;
static uint8_t uc_reg_tx_int;
static uint8_t uc_reg_ns_int;
/* @} */

/* internal buffers to store headers */
static uint8_t uc_phy_headers_buffer[PHY_NUM_RX_BUFFERS][PHY_DMA_OFFSET];

/* vars to store values to update statistic values */
static uint16_t us_phy_last_tx_lengths[PHY_NUM_TX_BUFFERS];

#ifdef PHY_OFFSET_SYMBOL_CONTROL
/* \name PHY Timer management */
/* @{ */
#define LENGTH_DATA_CORRECT_OFFSET_FILTER  (NUM_ROWS_DATA_ANGLE_REAL_IMAG_COMP * 8)
static uint16_t us_symb_tx_count = 0;
static bool uc_first_symbol = true;
static uint8_t puc_offset_buf[LENGTH_DATA_CORRECT_OFFSET_FILTER];
static bool uc_correct_psk_flag = false;
/* @} */
#endif

/* Table to manage emit frequency */
static uint8_t const uc_emit_freq_tab[PHY_NUM_CHANNELS] = {
	MODE_EF10,  /* Channel 1 */
	MODE_EF10,  /* Channel 2 */
	MODE_EF40,  /* Channel 3 */
	MODE_EF40,  /* Channel 4 */
	MODE_EF40,  /* Channel 5 */
	MODE_EF40,  /* Channel 6 */
	MODE_EF40,  /* Channel 7 */
	MODE_EF40   /* Channel 8 */
};

/* Table to manage soft stop timeH */
static uint16_t const uc_stop_time[MODE_NUM_EF] = {
	0x0200,  /* MODE_EF10 */
	0x0200,  /* MODE_EF20 */
	0x0400   /* MODE_EF40 */
};

/* Table to manage soft stop timeH */
static uint16_t const uc_time_X1[MODE_NUM_EF] = {
	0x1010,  /* MODE_EF10 */
	0x1010,  /* MODE_EF20 */
	0x8080   /* MODE_EF40 */
};

/* Table to manage soft stop timeH */
static uint16_t const uc_time_X2[MODE_NUM_EF] = {
	0x1010,  /* MODE_EF10 */
	0x1010,  /* MODE_EF20 */
	0x8080   /* MODE_EF40 */
};

/* Table to manage soft stop timeH */
static uint8_t const uc_time_Y1[MODE_NUM_EF] = {
	0x77,  /* MODE_EF10 */
	0x77,  /* MODE_EF20 */
	0x77   /* MODE_EF40 */
};

/* Table to manage soft stop timeH */
static uint8_t const uc_time_Y2[MODE_NUM_EF] = {
	0x77,  /* MODE_EF10 */
	0x77,  /* MODE_EF20 */
	0x77   /* MODE_EF40 */
};

/* Table to manage attenuation values */
static uint8_t const uc_att_value_tab[MOL_MINIMUM + 1] = {
	ATT_0dB,
	ATT_1dB,
	ATT_2dB,
	ATT_3dB,
	ATT_4dB,
	ATT_5dB,
	ATT_6dB,
	ATT_7dB,
	ATT_8dB,
	ATT_9dB,
	ATT_10dB,
	ATT_11dB,
	ATT_12dB,
	ATT_13dB,
	ATT_14dB,
	ATT_15dB,
	ATT_16dB,
	ATT_17dB,
	ATT_18dB,
	ATT_19dB,
	ATT_20dB,
	ATT_21dB
};

/* Identifier for channel Configuration */
static uint32_t const ul_channel_tx_tab[PHY_NUM_CHANNELS] = {
	PHY_ID_TX_CHN1,
	PHY_ID_TX_CHN2,
	PHY_ID_TX_CHN3,
	PHY_ID_TX_CHN4,
	PHY_ID_TX_CHN5,
	PHY_ID_TX_CHN6,
	PHY_ID_TX_CHN7,
	PHY_ID_TX_CHN8
};

/* Identifier for crc type in function of header type */
static uint8_t const uc_crc_type_tab[3] = {
	CRC_TYPE_32,
	CRC_TYPE_8,
	CRC_TYPE_32
};

/* \name Peak Cut definition */
/* @{ */
#define PEAK_CFG_ENABLE       0x80
#define PEAK_CFG_DISABLE      0
#define NOT_ALLOWED           0
static uint8_t const uc_peak_cut_on_enable_tab[16] = {
	PEAK_CFG_DISABLE,  /* 0 - BPSK */
	PEAK_CFG_DISABLE,  /* 1 - QOPSK */
	PEAK_CFG_DISABLE,  /* 2 - 8PSK */
	NOT_ALLOWED,       /* 3 - not permitted */
	PEAK_CFG_ENABLE,   /* 4 - BPSK_VTB */
	PEAK_CFG_ENABLE,   /* 5 - QPSK_VTB */
	PEAK_CFG_ENABLE,   /* 6 - 8PSK_VTB */
	NOT_ALLOWED,       /* 7 - not permitted */
	NOT_ALLOWED,       /* 8 - not permitted */
	NOT_ALLOWED,       /* 9 - not permitted */
	NOT_ALLOWED,       /* 10 - not permitted */
	NOT_ALLOWED,       /* 11 - not permitted */
	PEAK_CFG_ENABLE,   /* 12 - BPSK_ROBO */
	PEAK_CFG_ENABLE,   /* 13 - QPSK_ROBO */
	NOT_ALLOWED,       /* 14 - not permitted */
	NOT_ALLOWED        /* 15 - not permitted */
};
/* @} */

/* Peak Cut gain definition */
static uint8_t const uc_peak_cut_gain_tab[16] = {
	0xFF,              /* 0 - BPSK */
	0xFF,              /* 1 - QPSK */
	0xFF,              /* 2 - 8PSK */
	NOT_ALLOWED,       /* 3 - not permitted */
	0xC0,              /* 4 - BPSK_VTB */
	0xA0,              /* 5 - QPSK_VTB */
	0x60,              /* 6 - 8PSK_VTB */
	NOT_ALLOWED,       /* 7 - not permitted */
	NOT_ALLOWED,       /* 8 - not permitted */
	NOT_ALLOWED,       /* 9 - not permitted */
	NOT_ALLOWED,       /* 10 - not permitted */
	NOT_ALLOWED,       /* 11 - not permitted */
	0xC0,              /* 12 - BPSK_ROBO */
	0xA0,              /* 13 - QPSK_ROBO */
	NOT_ALLOWED,       /* 14 - not permitted */
	NOT_ALLOWED        /* 15 - not permitted */
};

/* \name Emit gain definition */
/* @{ */
#define EMIT_GAIN_CHIRP         0x24
#define EMIT_GAIN_SIGNAL        0x1C
static uint8_t const uc_emit_gain_tab[16] = {
	EMIT_GAIN_CHIRP,   /* 0 - BPSK */
	EMIT_GAIN_CHIRP,   /* 1 - QPSK */
	EMIT_GAIN_CHIRP,   /* 2 - 8PSK */
	NOT_ALLOWED,       /* 3 - not permitted */
	EMIT_GAIN_CHIRP,   /* 4 - BPSK_VTB */
	EMIT_GAIN_CHIRP,   /* 5 - QPSK_VTB */
	EMIT_GAIN_CHIRP,   /* 6 - 8PSK_VTB */
	NOT_ALLOWED,       /* 7 - not permitted */
	NOT_ALLOWED,       /* 8 - not permitted */
	NOT_ALLOWED,       /* 9 - not permitted */
	NOT_ALLOWED,       /* 10 - not permitted */
	NOT_ALLOWED,       /* 11 - not permitted */
	EMIT_GAIN_CHIRP,   /* 12 - BPSK_ROBO */
	EMIT_GAIN_CHIRP,   /* 13 - QPSK_ROBO */
	NOT_ALLOWED,       /* 14 - not permitted */
	NOT_ALLOWED        /* 15 - not permitted */
};
/* @} */

/* Number of bytes per symbol depending on Modulation scheme */
static uint8_t const uc_num_bytes_per_symbol_tab[16] = {
	12,                /* 0 - BPSK */
	24,                /* 1 - QPSK */
	36,                /* 2 - 8PSK */
	NOT_ALLOWED,       /* 3 - not permitted */
	6,                 /* 4 - BPSK_VTB */
	12,                /* 5 - QPSK_VTB */
	18,                /* 6 - 8PSK_VTB */
	NOT_ALLOWED,       /* 7 - not permitted */
	NOT_ALLOWED,       /* 8 - not permitted */
	NOT_ALLOWED,       /* 9 - not permitted */
	NOT_ALLOWED,       /* 10 - not permitted */
	NOT_ALLOWED,       /* 11 - not permitted */
	6,                 /* 12 - BPSK_ROBO */
	12,                /* 13 - QPSK_ROBO */
	NOT_ALLOWED,       /* 14 - not permitted */
	NOT_ALLOWED        /* 15 - not permitted */
};

static const uint32_t v_crc_polynom[VCRC_TYPES_NUMBER] = {
	VCRC8_POLYNOM,
	VCRC16_POLYNOM,
	VCRC24_POLYNOM,
	VCRC32_POLYNOM
};

static const uint32_t v_crc_rst[VCRC_TYPES_NUMBER] = {
	VCRC8_RST,
	VCRC16_RST,
	VCRC24_RST,
	VCRC32_RST
};

static const uint8_t v_crc_conf[VCRC_TYPES_NUMBER] = {
	VCRC8_CONF,
	VCRC16_CONF,
	VCRC24_CONF,
	VCRC32_CONF
};

static uint32_t v_crc_rst_sna[VCRC_TYPES_NUMBER] = {
	VCRC8_RST,
	VCRC16_RST,
	VCRC24_RST,
	VCRC32_RST
};

/* Table to manage soft stop timeH */
static uint8_t const uc_bc_mode_config_value[8] = {
	0x41, 0x1A, 0x7A, 0x2B, 0xCB, 0xCF, 0xAB, 0xAA
};

/**
 * \internal
 * \brief Get CRC value from the buffer content
 *
 * \param uc_crc_type       CRC type
 * \param puc_buf           Pointer to Data to calculate CRC
 * \param us_len            Data length
 * \param uc_header_type    Header Type
 *
 * \retval crc value
 */
static uint32_t _get_crc(uint8_t uc_crc_type, uint8_t *puc_buf, uint16_t us_len, uint8_t uc_header_type)
{
	uint32_t crcVal;

	if (uc_crc_type >= VCRC_TYPES_NUMBER) {
		return 0;
	}

	/* CONFIG */
	pplc_if_write32(REG_ATPL230_VCRC_POLY_VH, v_crc_polynom[uc_crc_type]);
	if (uc_header_type == PHY_HT_GENERIC) {
		pplc_if_write32(REG_ATPL230_VCRC_RST_VH, v_crc_rst_sna[uc_crc_type]);
	} else {
		pplc_if_write32(REG_ATPL230_VCRC_RST_VH, v_crc_rst[uc_crc_type]);
	}

	pplc_if_write8(REG_ATPL230_VCRC_CONF, v_crc_conf[uc_crc_type]);

	/* Reset */
	pplc_if_or8(REG_ATPL230_VCRC_CTL, 0x01);
	pplc_if_and8(REG_ATPL230_VCRC_CTL, 0xFE);

	/* DATA */
	pplc_if_write_rep(REG_ATPL230_VCRC_INPUT, 0, puc_buf, us_len);

	/* Read CRC result */
	crcVal = pplc_if_read32(REG_ATPL230_VCRC_CRC_VH);

	/* Apply correspoding mask to crc type */
	switch (uc_crc_type) {
	case CRC_TYPE_8:
		crcVal &= 0x000000FF;
		break;

	case CRC_TYPE_16:
		crcVal &= 0x0000FFFF;
		break;

	case CRC_TYPE_24:
		crcVal &= 0x00FFFFFF;
		break;

	case CRC_TYPE_32:
		break;
	}

	return crcVal;
}

/**
 * \internal
 * \brief Update CRC of SNA
 *
 * Used as initial CRC value in case of generic header type
 *
 * \param puc_sna Pointer to SNA
 */
static void _upd_sna_crc(uint8_t *puc_sna)
{
	/* Get CRC initial value depending on SNA */
	v_crc_rst_sna[CRC_TYPE_8] = _get_crc(CRC_TYPE_8, puc_sna, 6, 0xFF);
	v_crc_rst_sna[CRC_TYPE_16] = _get_crc(CRC_TYPE_16, puc_sna, 6, 0xFF);
	v_crc_rst_sna[CRC_TYPE_24] = _get_crc(CRC_TYPE_24, puc_sna, 6, 0xFF);
	v_crc_rst_sna[CRC_TYPE_32] = _get_crc(CRC_TYPE_32, puc_sna, 6, 0xFF);
}

/**
 * \internal
 * \brief Store Filter secuence
 *
 * \param puc_fir_data      Pointer to filter table data (start pointer)
 * \param uc_cmd            Internal Memory CMD
 * \param uc_num_rows       Number of rows in the filter table
 * \param uc_way_mode       Mode to read filter table [0: normal mode, 1:
 *inverse mode]
 * \param uc_start_mem_byte Byte to start write operation in internal memory
 * \param uc_inv_mode       Select inverse data mode
 *
 */
static void _store_filter_sec(uint8_t *puc_fir_data, uint8_t uc_cmd, uint8_t uc_num_rows,
		uint8_t uc_way_mode, uint8_t uc_start_mem_byte, uint8_t uc_inv_mode)
{
	uint8_t *puc_data;
	uint8_t uc_write_fir_data[8];
	uint16_t uc_idx;
	uint16_t us_value, us_inv_value;

	/* Get pointer to filter table */
	puc_data = puc_fir_data;

	/* Init specific filter bytes */
	memset(uc_write_fir_data, 0, sizeof(uc_write_fir_data));
	if (uc_cmd == 0x21) {
		uc_write_fir_data[1] = 0x04;
		uc_write_fir_data[2] = 0x24;
	}

	uc_write_fir_data[7] = uc_cmd;

	/* Configure internal filter */
	if (!uc_way_mode) {
		/* Normal mode */
		if (!uc_inv_mode) { /* normal data mode */
			for (uc_idx = 0; uc_idx < uc_num_rows; uc_idx++) {
				uc_write_fir_data[0] = uc_start_mem_byte + uc_idx;
				uc_write_fir_data[3] = *puc_data++;
				uc_write_fir_data[4] = *puc_data++;
				if (uc_cmd == 0x11) { /* chirp filter(0x11) */
					uc_write_fir_data[5] = *puc_data++;
					uc_write_fir_data[6] = *puc_data++;
				}

				pplc_if_write_buf(REG_ATPL230_LOAD_ADDRL, uc_write_fir_data, 8);
			}
		} else { /* inverse data mode */
			for (uc_idx = 0; uc_idx < uc_num_rows; uc_idx++) {
				uc_write_fir_data[0] = uc_start_mem_byte + uc_idx;
				us_value = ((uint16_t)*puc_fir_data++) << 8;
				us_value += (uint16_t)*puc_fir_data++;
				us_inv_value = 0x10000 - us_value;
				uc_write_fir_data[3] = (uint8_t)(us_inv_value >> 8);
				uc_write_fir_data[4] = (uint8_t)(us_inv_value);
				if (uc_cmd == 0x11) { /* only for chirp filter */
					us_value = ((uint16_t)*puc_fir_data++) << 8;
					us_value += (uint16_t)*puc_fir_data++;
					us_inv_value = 0x10000 - us_value;
					uc_write_fir_data[5] = (uint8_t)(us_inv_value >> 8);
					uc_write_fir_data[6] = (uint8_t)(us_inv_value);
				}

				pplc_if_write_buf(REG_ATPL230_LOAD_ADDRL, uc_write_fir_data, 8);
			}
		}
	} else {
		/* Inverse mode */
		if (!uc_inv_mode) { /* normal data mode */
			for (uc_idx = 0; uc_idx < uc_num_rows; uc_idx++) {
				uc_write_fir_data[0] = uc_start_mem_byte + uc_idx;
				if (uc_cmd == 0x11) { /* only for chirp filter */
					uc_write_fir_data[6] = *puc_fir_data--;
					uc_write_fir_data[5] = *puc_fir_data--;
				}

				uc_write_fir_data[4] = *puc_fir_data--;
				uc_write_fir_data[3] = *puc_fir_data--;
				pplc_if_write_buf(REG_ATPL230_LOAD_ADDRL, uc_write_fir_data, 8);
			}
		} else { /* inverse data mode */
			for (uc_idx = 0; uc_idx < uc_num_rows; uc_idx++) {
				uc_write_fir_data[0] = uc_start_mem_byte + uc_idx;
				if (uc_cmd == 0x11) { /* only for chirp filter */
					us_value = (uint16_t)*puc_fir_data--;
					us_value += ((uint16_t)*puc_fir_data--) << 8;
					us_inv_value = 0x10000 - us_value;
					uc_write_fir_data[5] = (uint8_t)(us_inv_value >> 8);
					uc_write_fir_data[6] = (uint8_t)(us_inv_value);
				}

				us_value = (uint16_t)*puc_fir_data--;
				us_value += ((uint16_t)*puc_fir_data--) << 8;
				us_inv_value = 0x10000 - us_value;
				uc_write_fir_data[3] = (uint8_t)(us_inv_value >> 8);
				uc_write_fir_data[4] = (uint8_t)(us_inv_value);

				pplc_if_write_buf(REG_ATPL230_LOAD_ADDRL, uc_write_fir_data, 8);
			}
		}
	}
}

#ifdef PHY_OFFSET_SYMBOL_CONTROL

/**
 * \internal
 * \brief Init Timer to correct offset each tx symbol
 *
 */
static void _init_timer_tx_offset_symb(void)
{
	uint8_t uc_row;
	uint8_t *puc_ptr;

	/* Configure PMC */
	pmc_enable_periph_clk(ID_TC_PHY_TX_OFFSET_SYM);

	/* Configure and enable interrupt on RC compare */
	NVIC_SetPriority((IRQn_Type)TC_PHY_TX_OFFSET_SYM_IRQn, 0);
	NVIC_EnableIRQ((IRQn_Type)TC_PHY_TX_OFFSET_SYM_IRQn);

	/* Init phy symbol counter */
	memset(puc_offset_buf, 0, sizeof(puc_offset_buf));
	puc_ptr = puc_offset_buf;
	/* init filter in case of use offset correction */
	for (uc_row = 0; uc_row < NUM_ROWS_DATA_ANGLE_REAL_IMAG_COMP; uc_row++) {
		*(puc_ptr++) = uc_row;
		*(puc_ptr++) = 0x04;
		*(puc_ptr++) = 0x24;
		*(puc_ptr++) = (uint8_t)(ul_data_offset_correction[0] >> 24);
		*(puc_ptr++) = (uint8_t)(ul_data_offset_correction[0] >> 16);
		*(puc_ptr++) = (uint8_t)(ul_data_offset_correction[0] >> 8);
		*(puc_ptr++) = (uint8_t)(ul_data_offset_correction[0]);
		*(puc_ptr++) = 0xA1;
	}

	/* adjust filter: initial offset */
	pplc_if_write_rep( REG_ATPL230_LOAD_ADDRL, 7, puc_offset_buf, sizeof(puc_offset_buf));
	pplc_if_and8(REG_ATPL230_LOAD_CTL, 0xDF);
}

/**
 * \internal
 * \brief Start Timer to correct offset each tx symbol
 *
 */
static void _start_timer_tx_offset_symb(uint32_t uc_time_us, bool uc_8psk_en)
{
	uint32_t ul_div, ul_tcclks;

	uc_correct_psk_flag = uc_8psk_en;

	/* Disable Int */
	tc_disable_interrupt(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN, TC_IER_CPCS);

	/* MCK = 120000000 -> tcclks = 2 : TCLK3 = MCK/32 = 3750000 = 0.266us ->
	 * ul_div = 1ms/0.2666us = 3750 */
	ul_tcclks = 2;
	ul_div = (3750 * uc_time_us) / 1000;
	tc_init(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN, ul_tcclks | TC_CMR_CPCTRG);

	tc_write_rc(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN, ul_div);

	/** Start the timer. */
	tc_start(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN);

	/* Enable Int */
	tc_enable_interrupt(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN, TC_IER_CPCS);
}

/**
 * \internal
 * \brief Stop Timer to correct offset each tx symbol
 *
 */
static void _stop_timer_tx_offset_symb(void)
{
	/* Disable Int */
	tc_disable_interrupt(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN, TC_IER_CPCS);

	/** Stop the PHY timer */
	tc_stop(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN);
}

/**
 * \internal
 * \brief Correct offset for next symbol
 *
 */
static void _correct_offset_symbol(void)
{
	uint8_t uc_row;
	uint16_t us_idx;
	uint32_t ul_offset_value;
	bool uc_correct_filter;

	us_symb_tx_count++;

	if ((us_symb_tx_count) == (atpl230.txPayloadLenSym + 2)) {
		/* it is the last symbol. Rewrite the initial offset config */
		ul_offset_value = ul_data_offset_correction[0];
		uc_correct_filter = true;
		/* stop timer */
		_stop_timer_tx_offset_symb();
		/* reset tx symb counter */
		us_symb_tx_count = 0;
		/* next symbol is new tx */
		uc_first_symbol = true;
	} else {
		/* correct symbol offset for next tx */
		if (uc_first_symbol) {
			uc_first_symbol = false;
			uc_correct_filter = true;
			ul_offset_value = ul_data_offset_correction[us_symb_tx_count & 0x0F];
		} else {
			if (uc_correct_psk_flag) {
				uc_correct_filter = true;
				if (us_symb_tx_count & 0x01) {
					ul_offset_value = ul_data_offset_correction[15];
				} else {
					ul_offset_value = ul_data_offset_correction[6];
				}
			} else {
				uc_correct_filter = true;
				ul_offset_value = ul_data_offset_correction[us_symb_tx_count & 0x0F];
			}
		}
	}

	if (uc_correct_filter) {
		for (uc_row = 0, us_idx = 3; uc_row < NUM_ROWS_DATA_ANGLE_REAL_IMAG_COMP; uc_row++) {
			*(puc_offset_buf + us_idx++) = (uint8_t)(ul_offset_value >> 24);
			*(puc_offset_buf + us_idx++) = (uint8_t)(ul_offset_value >> 16);
			*(puc_offset_buf + us_idx++) = (uint8_t)(ul_offset_value >> 8);
			*(puc_offset_buf + us_idx++) = (uint8_t)(ul_offset_value);
			us_idx += 4;
		}
		/* Correct offset */
		pplc_if_write_rep( REG_ATPL230_LOAD_ADDRL, 7, puc_offset_buf, sizeof(puc_offset_buf));
		pplc_if_and8(REG_ATPL230_LOAD_CTL, 0xDF);
	}
}

/**
 * \internal
 * \brief PHY Timer handler
 *
 */

void TC_PHY_TX_OFFSET_SYM_Handler(void)
{
	volatile uint32_t ul_dummy;

	/* Clear status bit to acknowledge interrupt */
	ul_dummy = tc_get_status(TC_PHY_TX_OFFSET_SYM, TC_PHY_TX_OFFSET_SYM_CHN);
	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/* restart timer */
	_start_timer_tx_offset_symb(2240, uc_correct_psk_flag);
	_correct_offset_symbol();
}

#endif /* PHY_OFFSET_SYMBOL_CONTROL */

/**
 * \brief Filter IIR initialization
 *
 */
static void _init_IIR_filter(void)
{
	/* Filter IIR initialization */
	pplc_if_write8(REG_ATPL230_IIR_CONFIG, 0x08);
	pplc_if_write8(REG_ATPL230_LOAD_ADDRH, 0x00);
	_store_filter_sec(uc_data_filter_IIR, 0x41, NUM_ROWS_DATA_FILTER_IIR, 0, 0, 0);
	pplc_if_and8(REG_ATPL230_LOAD_CTL, 0xBF);
	pplc_if_write8(REG_ATPL230_IIR_CONFIG, 0x00);

	/* Chirp initialization */
	pplc_if_write8(REG_ATPL230_LOAD_ADDRH, 0x00);
	_store_filter_sec(uc_data_chirp, 0x11, NUM_ROWS_DATA_CHIRP, 0, 0, 0);
	_store_filter_sec(&uc_data_chirp[LENGTH_DATA_CHIRP - 5], 0x11, NUM_ROWS_DATA_CHIRP - 1, 1, 65, 0);
	_store_filter_sec(&uc_data_chirp[4], 0x11, NUM_ROWS_DATA_CHIRP - 1, 0, 129, 0);
	_store_filter_sec(&uc_data_chirp[LENGTH_DATA_CHIRP - 5], 0x11, NUM_ROWS_DATA_CHIRP - 1, 1, 193, 0);
	pplc_if_write8(REG_ATPL230_LOAD_ADDRH, 0x01);
	_store_filter_sec(uc_data_chirp, 0x11, NUM_ROWS_DATA_CHIRP, 0, 0, 0);
	_store_filter_sec(&uc_data_chirp[LENGTH_DATA_CHIRP - 5], 0x11, NUM_ROWS_DATA_CHIRP - 1, 1, 65, 0);
	_store_filter_sec(uc_data_chirp, 0x11, NUM_ROWS_DATA_CHIRP, 0, 128, 1);
	_store_filter_sec(&uc_data_chirp[LENGTH_DATA_CHIRP - 5], 0x11, NUM_ROWS_DATA_CHIRP - 2, 1, 193, 1);
	pplc_if_and8(REG_ATPL230_LOAD_CTL, 0xEF);

	/* Angle Real Imag Compensation initialization ram_97x48 */
	pplc_if_write8(REG_ATPL230_LOAD_ADDRH, 0x00);
	_store_filter_sec(uc_data_angle_real_imag_comp, 0x21, NUM_ROWS_DATA_ANGLE_REAL_IMAG_COMP, 0, 0, 0);
	pplc_if_and8(REG_ATPL230_LOAD_CTL, 0xDF);
}

/**
 * \brief Update channel in atpl230 device.
 * Update attenuation values depending on channel to use.
 *
 * \param uc_channel    Communication Channel [1..8]
 *
 */
static uint8_t _update_channel(uint8_t uc_channel)
{
	uint8_t uc_channel_idx;
	uint16_t us_mem_offset;

	uc_channel_idx = uc_channel - 1;
	if ((uc_channel > 0) && (uc_channel < 9)) {
		/* update channel vars */
		atpl230Cfg.txrxChannel = uc_channel;
		/* get structure idx channel */
		uc_channel_idx = uc_channel - 1;
		/* Change channel configuration */
		pplc_if_write32(REG_ATPL230_PASO_FASE_CORDIC_TX_VH, ul_channel_tx_tab[uc_channel_idx]);
		/* Update attenuation values */
		phyTxAttChirpHighZ = (uint8_t)*(&atpl230ChnCfg.tx1AttChirpHighZ + (uc_channel_idx * 14));
		phyTxAttSignalHighZ = (uint8_t)*(&atpl230ChnCfg.tx1AttSignalHighZ + (uc_channel_idx * 14));
		phyTxAttChirpLowZ = (uint8_t)*(&atpl230ChnCfg.tx1AttChirpLowZ + (uc_channel_idx * 14));
		phyTxAttSignalLowZ = (uint8_t)*(&atpl230ChnCfg.tx1AttSignalLowZ + (uc_channel_idx * 14));
		phyTxAttChirpVLowZ = (uint8_t)*(&atpl230ChnCfg.tx1AttChirpVLowZ + (uc_channel_idx * 14));
		phyTxAttSignalVLowZ = (uint8_t)*(&atpl230ChnCfg.tx1AttSignalVLowZ + (uc_channel_idx * 14));
		us_mem_offset = (uc_channel_idx * 14) >> 1;
		phyTxLoadThreshold1 = (uint16_t)*(&atpl230ChnCfg.tx1LoadThreshold1 + us_mem_offset);
		phyTxLoadThreshold2 = (uint16_t)*(&atpl230ChnCfg.tx1LoadThreshold2 + us_mem_offset);
		phyTxLoadThreshold3 = (uint16_t)*(&atpl230ChnCfg.tx1LoadThreshold3 + us_mem_offset);
		phyTxLoadThreshold4 = (uint16_t)*(&atpl230ChnCfg.tx1LoadThreshold4 + us_mem_offset);
		/* Set emitter frequency */
		pplc_if_write8(REG_ATPL230_EMITTER_FREC, uc_emit_freq_tab[uc_channel_idx]);
		return true;
	}

	return false;
}

/**
 * \brief Update polarity of TXRX 1 in atpl230 device.
 * Update Polarity register configuration.
 *
 * \param uc_pol    Polarity Mode [DRV_POL_TX_0_RX_1, DRV_POL_TX_1_RX_0]
 */
static void _update_txrx1_polarity(uint8_t uc_pol)
{
	if (uc_pol) {
		pplc_if_or8(REG_ATPL230_AFE_CTL, 0x01);
	} else {
		pplc_if_and8(REG_ATPL230_AFE_CTL, 0xFE);
	}
}

/**
 * \brief Update polarity of TXRX 2 in atpl230 device.
 * Update Polarity register configuration.
 *
 * \param uc_pol    Polarity Mode [DRV_POL_TX_0_RX_1, DRV_POL_TX_1_RX_0]
 */
static void _update_txrx2_polarity(uint8_t uc_pol)
{
	if (uc_pol) {
		pplc_if_or8(REG_ATPL230_AFE_CTL, 0x02);
	} else {
		pplc_if_and8(REG_ATPL230_AFE_CTL, 0xFD);
	}
}

/**
 * \brief Update emit 1 mode in atpl230 device.
 * Update Emit mode register configuration.
 *
 * \param uc_emode    Emit Mode [INTERNAL DRIVER, EXTERNAL DRIVER]
 */
static void _update_emit1_mode(uint8_t uc_emode)
{
	if (uc_emode == EXTERNAL_DRV_MODE) {
		/* config driver 1 as external mode */
		pplc_if_or8(REG_ATPL230_EMIT_CONFIG, 0x01);
		/* Transistors delays */
		pplc_if_write8(REG_ATPL230_N1_DELAY, atpl230Cfg.n1Delay);
		pplc_if_write8(REG_ATPL230_P1_DELAY, atpl230Cfg.p1Delay);
		/* Config Emit outputs */
		pplc_if_write8(REG_ATPL230_EMIT1_ACTIVE, atpl230Cfg.emit1Active);
		pplc_if_write8(REG_ATPL230_EMIT2_ACTIVE, atpl230Cfg.emit2Active);
	} else {
		/* config driver 1 as internal mode */
		pplc_if_and8(REG_ATPL230_EMIT_CONFIG, 0xFE);
		/* Config Emit outputs */
		pplc_if_write8(REG_ATPL230_EMIT1_ACTIVE, atpl230Cfg.emit1Active);
		pplc_if_write8(REG_ATPL230_EMIT2_ACTIVE, atpl230Cfg.emit2Active);
	}
}

/**
 * \brief Update emit 2 mode in atpl230 device.
 * Update Emit mode register configuration.
 *
 * \param uc_emode    Emit Mode [INTERNAL DRIVER, EXTERNAL DRIVER]
 */
static void _update_emit2_mode(uint8_t uc_emode)
{
	if (uc_emode == EXTERNAL_DRV_MODE) {
		/* config driver 2 as external mode */
		pplc_if_or8(REG_ATPL230_EMIT_CONFIG, 0x02);
		/* Transistors delays */
		pplc_if_write8(REG_ATPL230_N2_DELAY, atpl230Cfg.n2Delay);
		pplc_if_write8(REG_ATPL230_P2_DELAY, atpl230Cfg.p2Delay);
		/* Config Emit outputs */
		pplc_if_write8(REG_ATPL230_EMIT3_ACTIVE, atpl230Cfg.emit3Active);
		pplc_if_write8(REG_ATPL230_EMIT4_ACTIVE, atpl230Cfg.emit4Active);
	} else {
		/* config driver 2 as internal mode */
		pplc_if_and8(REG_ATPL230_EMIT_CONFIG, 0xFD);
		/* Config Emit outputs */
		pplc_if_write8(REG_ATPL230_EMIT3_ACTIVE, atpl230Cfg.emit3Active);
		pplc_if_write8(REG_ATPL230_EMIT4_ACTIVE, atpl230Cfg.emit4Active);
	}
}

/**
 * \brief PHY init layer
 *
 * \param uc_rst_type    0: Complete initialization; 1: No init internal vars
 *neither internal filters
 */
static void _init_phy_layer(uint8_t uc_rst_type)
{
	uint8_t uc_idx;
	uint8_t uc_sna[6];
	uint8_t uc_mac_en_cfg;

	/* set critical region */
	Disable_global_interrupt();
	/* Store old configuration */
	if (uc_rst_type == PHY_RESET_HARD_TYPE) {
		/* init sw strcut values */
		memset(&atpl230, 0, sizeof(atpl230));

		/* configure atpl230 version information */
		memcpy(atpl230.prodId, ATPL230_PRODID, 10);
		atpl230.version = ATPL230_VERSION_NUM;

		atpl230.rxQRMode = 0x00;
		atpl230.txQRMode = 0x00;

		/* init configuration vars */
		phy_init_cfg();
		phy_init_coupling_cfg();
		phy_init_channel_cfg();
	} else {
		/* store configuration to restore after reset */
		pplc_if_read_buf(REG_ATPL230_SNA0, uc_sna, sizeof(uc_sna));
		uc_mac_en_cfg = phy_get_mac_en();
	}

	/* Reset the PHY layer */
	pplc_if_or8(REG_ATPL230_SYS_CONFIG, ATPL230_SYS_CONFIG_RST_Msk);
	pplc_if_and8(REG_ATPL230_SYS_CONFIG, (uint8_t) ~ATPL230_SYS_CONFIG_RST_Msk);

	if (uc_rst_type == PHY_RESET_HARD_TYPE) {
		/* Initialize IIR filter */
		_init_IIR_filter();
		/* Reset the PHY layer */
		pplc_if_or8(REG_ATPL230_SYS_CONFIG, ATPL230_SYS_CONFIG_RST_Msk);
		pplc_if_and8(REG_ATPL230_SYS_CONFIG, (uint8_t) ~ATPL230_SYS_CONFIG_RST_Msk);
	} else {
		/* restore sna configuration */
		pplc_if_write_buf(REG_ATPL230_SNA0, uc_sna, sizeof(uc_sna));
		/* restore mac coproc configuration */
		if (uc_mac_en_cfg) {
			phy_mac_crc_enable();
		} else {
			phy_mac_crc_disable();
		}
	}

	/* Enable RAM memory (IIR filters) */
	pplc_if_or8(REG_ATPL230_LOAD_CTL, ATPL230_SYS_CONFIG_STOP_ENRAM_Msk);

	/* Disable II FIlter */
	pplc_if_write8(REG_ATPL230_IIR_CONFIG, 0x00);

	/* Analog Front End configuration */
	pplc_if_write16(REG_ATPL230_SOFT_STOP_TIMEH, 0x0200);
	pplc_if_write16(REG_ATPL230_SOFT_TIME_X1_H, 0x1010);
	pplc_if_write16(REG_ATPL230_SOFT_TIME_X2_H, 0x1010);
	pplc_if_write8(REG_ATPL230_SOFT_TIME_Y1, 0x77);
	pplc_if_write8(REG_ATPL230_SOFT_TIME_Y2, 0x77);

	/* Forces Factor Threshold Values */
	pplc_if_write8(REG_ATPL230_FACTOR_THR_CTL, 0x30);
	pplc_if_write8(REG_ATPL230_FACTOR_THRESHOLD_4_AND_1, 0xDA);
	pplc_if_write8(REG_ATPL230_FACTOR_THRESHOLD_3_AND_2, 0xBD);
	pplc_if_write8(REG_ATPL230_FACTOR_THRESHOLD_2_3_AND_1_3, 0xBA);
	pplc_if_write8(REG_ATPL230_FACTOR_ROBUST_3_2, 0x23);

	/* Automatic Factor Threshold */
	pplc_if_write8(REG_ATPL230_FT_SUMADOR_CONF, 0x11);
	pplc_if_write8(REG_ATPL230_FACTOR_COMP_DOWN, 0x60);
	pplc_if_write8(REG_ATPL230_FACTOR_COMP_UP, 0x50);
	pplc_if_write8(REG_ATPL230_FT_TIME_DOWN, 0xC8);
	pplc_if_write8(REG_ATPL230_FT_TIME_UP, 0x18);
	pplc_if_write8(REG_ATPL230_FT_STEP_UP, 0x02);
	pplc_if_write8(REG_ATPL230_FT_STEP_UP_FD, 0x04);
	pplc_if_write8(REG_ATPL230_FT_STEP_DOWN, 0x01);
	pplc_if_write8(REG_ATPL230_FT_MIN_FACTOR_TH, 0x06);
	pplc_if_write8(REG_ATPL230_FT_MAX_FACTOR_TH, 0x20);
	pplc_if_write8(REG_ATPL230_FT_CORR_MIN, 0x00);
	pplc_if_write8(REG_ATPL230_FACTOR_COMP_DOWN_ROBO, 0x60);
	pplc_if_write8(REG_ATPL230_FACTOR_COMP_UP_ROBO, 0x50);
	pplc_if_write8(REG_ATPL230_FT_TIME_DOWN_ROBO, 0xC8);
	pplc_if_write8(REG_ATPL230_FT_TIME_UP_ROBO, 0x18);
	pplc_if_write8(REG_ATPL230_FT_STEP_UP_ROBO, 0x02);
	pplc_if_write8(REG_ATPL230_FT_STEP_UP_FD_ROBO, 0x04);
	pplc_if_write8(REG_ATPL230_FT_STEP_DOWN_ROBO, 0x01);
	pplc_if_write8(REG_ATPL230_FT_MIN_FACTOR_TH_ROBO, 0x06);
	pplc_if_write8(REG_ATPL230_FT_MAX_FACTOR_TH_ROBO, 0x17);
	pplc_if_write8(REG_ATPL230_FT_CORR_MIN_ROBO, 0x00);

	/* CARRIER DETECT */
	pplc_if_write8(REG_ATPL230_N_MOD, 0x30);
	pplc_if_write16(REG_ATPL230_N_RAMPH, 0x0600);
	pplc_if_write8(REG_ATPL230_N_CHIRP, 0x58);
	pplc_if_write8(REG_ATPL230_STEP_M_UP, 0x99);
	pplc_if_write8(REG_ATPL230_STEP_M_DOWN, 0x79);
	pplc_if_write16(REG_ATPL230_MIN_M_TH_HIGH, 0x0021);
	pplc_if_write8(REG_ATPL230_STEP_R_UP, 0x03);
	pplc_if_write8(REG_ATPL230_STEP_R_DOWN, 0x01);
	pplc_if_write8(REG_ATPL230_MIN_R_TH, 0x02);
	pplc_if_write8(REG_ATPL230_R_MARGIN, 0x04);
	pplc_if_write8(REG_ATPL230_CD_CONTROL, 0x3F);

	/* EQUALIZATION */
	pplc_if_write8(REG_ATPL230_EQUALIZE_H, 0x00);
	pplc_if_write8(REG_ATPL230_EQUALIZE_L, 0x00);

	/* BC configuration value */
	pplc_if_write_buf(0xFF00, (uint8_t *)uc_bc_mode_config_value, sizeof(uc_bc_mode_config_value));

	/* Chirp Prime & ROBO */
	/* Prime Chirp initial RAM address Register */
	pplc_if_write8(REG_ATPL230_INI_CHIRP1, 0xC0);
	/* Robust Chirp initial RAM address Register */
	pplc_if_write8(REG_ATPL230_INI_CHIRP2, 0x00);
	/* Prime - Robust Chirp Mode Register (bits[1:0]=[Robust, Prime]) */
	pplc_if_write8(REG_ATPL230_MODE_CONJ, 0x02);

	/* Pad len represented before coding */
	pplc_if_and8(REG_ATPL230_PHY_CONFIG, 0xEF);

	/* Disable cinr mode */
	pplc_if_and8(REG_ATPL230_PHY_CONFIG, 0xDF);

	/* Configure physical channels */
	_update_channel(atpl230Cfg.txrxChannel);

	/* Enable Carrier Detect by default */
	pplc_if_or8(REG_ATPL230_PHY_CONFIG, 0x04);

	/* Disable watchdog by default */
	pplc_if_and8(REG_ATPL230_SYS_CONFIG, 0xFB);

	/* Encryption disabled by default */
	pplc_if_and8(REG_ATPL230_PHY_CONFIG, 0xF7);

	/* TXRX polarization configuration */
	_update_txrx1_polarity(atpl230Cfg.txrx1Polarity);
	_update_txrx2_polarity(atpl230Cfg.txrx2Polarity);

	/* Emit mode configuration */
	_update_emit1_mode(atpl230Cfg.driver1Mode);
	_update_emit2_mode(atpl230Cfg.driver2Mode);

	/* Set value for RSSI offset reg : R&D value */
	pplc_if_write8(REG_ATPL230_RSSI_OFFSET, 0x88);

	/* Init tx buffer id 0 by default */
	atpl230.txIdBuff = 0;

	/* Affects only to Prime V1.3 */
	/* Maximum symbol length for every modulation scheme */
	pplc_if_write8(REG_ATPL230_MAX_LEN_DBPSK, MAX_LEN_DBPSK);
	pplc_if_write8(REG_ATPL230_MAX_LEN_DBPSK_VTB, MAX_LEN_DBPSK_VTB);
	pplc_if_write8(REG_ATPL230_MAX_LEN_DQPSK, MAX_LEN_DQPSK);
	pplc_if_write8(REG_ATPL230_MAX_LEN_DQPSK_VTB, MAX_LEN_DQPSK_VTB);
	pplc_if_write8(REG_ATPL230_MAX_LEN_D8PSK, MAX_LEN_D8PSK);
	pplc_if_write8(REG_ATPL230_MAX_LEN_D8PSK_VTB, MAX_LEN_D8PSK_VTB);

	/* Affects only to Prime V1.3 */
	/* Reset MAC Coprocessor errors */
	pplc_if_write16(REG_ATPL230_CRC8_PHY_HIGH, 0);
	pplc_if_write16(REG_ATPL230_CRC32_MAC_HIGH, 0);
	pplc_if_write16(REG_ATPL230_CRC8_MAC_HIGH, 0);
	pplc_if_write16(REG_ATPL230_CRC8_MAC_HD_HIGH, 0);
	pplc_if_write8(REG_ATPL230_POSITIVE_FAIL_CONFIG, 0x3F);
	pplc_if_write16(REG_ATPL230_POSITIVE_FAIL_HIGH, 0);

	/* Transmission buffers configuration: PRIMEv1.3 by default */
	for (uc_idx = 0; uc_idx < PHY_NUM_TX_BUFFERS; uc_idx++) {
		/* Tell PHY from where transmit data */
		pplc_if_write16(REG_ATPL230_TXRXBUF_INITAD1_TX0 + (uc_idx << 1), (REG_ATPL230_PHY_TX_INIT_ADDRESS + (PHY_MAX_PPDU_SIZE * uc_idx)));
		/* Transmission timeout in 10's of us */
		pplc_if_write32(REG_ATPL230_TXRXBUF_TIMEOUT1_TX0 + (uc_idx << 2), PHY_TX_TIMEOUT_IMMEDIATE_10US);
	}

	/* Init rx buffer id 0 by default */
	atpl230.rxIdBuff = 0;

	/* RX Configuration Register */
	/* Enable Interrupt in every Rx Buffers, Select reception buffer 0, mode
	 * store 1 (automatic) and header interrupt disabled */
	pplc_if_write8(REG_ATPL230_TXRXBUF_RXCONF, 0x01);

	/* Enable all reception buffers */
	pplc_if_and8(REG_ATPL230_TXRXBUF_SELECT_BUFF_RX, 0xF0);
	pplc_if_or8(REG_ATPL230_TXRXBUF_SELECT_BUFF_RX, 0x0F);

	/* Enable RX: Set minimum value for autocorrelation filter */
	pplc_if_write8(REG_ATPL230_FILTER_MIN_HIGH, 0x00);
	pplc_if_write16(REG_ATPL230_FILTER_MIN_MED, atpl230Cfg.rxCorrThreshold);
	pplc_if_write16(REG_ATPL230_AGC_THRESHOLD_HIGH, 0x01FF);

	/* Reception limit with an agc active */
	pplc_if_write8(REG_ATPL230_AGC0_KRSSI, atpl230Cfg.agc0KrssiOffset);
	pplc_if_write8(REG_ATPL230_AGC1_KRSSI, atpl230Cfg.agc1KrssiOffset);
	pplc_if_write8(REG_ATPL230_AGC2_KRSSI, atpl230Cfg.agc2KrssiOffset);
	pplc_if_write8(REG_ATPL230_AGC3_KRSSI, atpl230Cfg.agc3KrssiOffset);

	/* Reception information */
	pplc_if_write8(REG_ATPL230_RX_TIME_OFFSET, 0);

	/* Tell PHY where to put received data */
	for (uc_idx = 0; uc_idx < PHY_NUM_RX_BUFFERS; uc_idx++) {
		pplc_if_write16(REG_ATPL230_TXRXBUF_INITAD1_RX0 + (uc_idx << 1), (REG_ATPL230_PHY_RX_INIT_ADDRESS + (PHY_MAX_PPDU_SIZE * uc_idx)));
	}

	/* Noise Capture Configuration */
	/* Tell PHY where to put noise data */
	pplc_if_write16(REG_ATPL230_TXRXBUF_INITREG_ATPL230_NOISE1, REG_ATPL230_PHY_NOISE_INIT_ADDRESS);
	/* Set Noise AGC configuration variation to 0, so AGC pairs will not
	 * change during a noise capture */
	pplc_if_write8(REG_ATPL230_AGC_CTL_AUX, 0x28);

	/* Write valid chip configuration key */
	pplc_if_write8(0xFFBF, ATPL230_VALID_CFG_KEY);

	/* Clear tx interrupt flags */
	pplc_if_write8(REG_ATPL230_TXRXBUF_TX_INT, 0x00);
	/* Clear rx interrupt flags */
	pplc_if_write8(REG_ATPL230_TXRXBUF_RX_INT, 0x00);
	/* clear interrupt flag */
	phy_clear_global_interrupt();

	/* init static var to get last rx message */
	uc_last_rx_buf = 0;
	uc_last_tx_buf = 0;

	/* init buf events */
	memset(puc_phy_rx_buffer_event, 0, sizeof(puc_phy_rx_buffer_event));
	memset(puc_phy_tx_buffer_event, 0, sizeof(puc_phy_tx_buffer_event));
	puc_phy_noise_buffer_event = false;

	/* clear critical region */
	Enable_global_interrupt();
}

/**
 * \brief Task to process TX PLC
 *
 */
void phy_tx_frame_result_cb(xPhyMsgTxResult_t *px_tx_result)
{
	uint8_t uc_tx_result_value = ATPL230_TXRXBUF_RESULT_SUCCESSFUL;
	uint16_t us_tx_result;
	uint16_t us_rms_calc;
	uint8_t uc_event_flag;
	uint8_t uc_idx;
	uint8_t uc_buf_idx;

	/* check TX result event */
	uc_event_flag = false;
	for (uc_idx = 0; uc_idx < 4; uc_idx++) {
		if (puc_phy_tx_buffer_event[uc_last_tx_buf]) {
			puc_phy_tx_buffer_event[uc_last_tx_buf] = false;
			/* get buffer idx of the current event */
			uc_buf_idx = uc_last_tx_buf;
			uc_event_flag = true;
			break;
		}

		if (++uc_last_tx_buf == 4) {
			uc_last_tx_buf = 0;
		}
	}

	if (!uc_event_flag) {
		px_tx_result->uc_id_buffer = 0xFF;
		px_tx_result->uc_result = 0xFF;
		return;
	}

	/* read tx results */
	us_tx_result = pplc_if_read16(REG_ATPL230_TXRXBUF_RESULT_TX10);

	switch (uc_buf_idx) {
	case 0:
		uc_tx_result_value = ATPL230_GET_TX_RESULT_TX0(us_tx_result);
		break;

	case 1:
		uc_tx_result_value = ATPL230_GET_TX_RESULT_TX1(us_tx_result);
		break;

	case 2:
		uc_tx_result_value = ATPL230_GET_TX_RESULT_TX2(us_tx_result);
		break;

	case 3:
		uc_tx_result_value = ATPL230_GET_TX_RESULT_TX3(us_tx_result);
		break;
	}

	switch (uc_tx_result_value) {
	case ATPL230_TXRXBUF_RESULT_SUCCESSFUL:
		/* check load at emission */
		if (atpl230Cfg.autoDetectBranch) {
			us_rms_calc = pplc_if_read16(REG_ATPL230_TXRXBUF_RMSCALC1_TX0 + (uc_buf_idx << 1));

			if (!atpl230Cfg.enable_vlow_pk) {
				if (atpl230Cfg.impedance == HI_STATE) {
					if ((us_rms_calc < phyTxLoadThreshold1) && (us_rms_calc > phyTxLoadThreshold2)) {
						atpl230Cfg.impedance = VLO_STATE;
					} else if (us_rms_calc < phyTxLoadThreshold2) {
						atpl230Cfg.impedance = LO_STATE;
					}
				} else if (atpl230Cfg.impedance == LO_STATE) {
					if (us_rms_calc > phyTxLoadThreshold3) {
						atpl230Cfg.impedance = HI_STATE;
					}

					if (us_rms_calc < phyTxLoadThreshold4) {
						atpl230Cfg.impedance = VLO_STATE;
					}
				}
			} else {
				if (atpl230Cfg.impedance == HI_STATE) {
					if ((us_rms_calc < phyTxLoadThreshold1) && (us_rms_calc > phyTxLoadThreshold2)) {
						atpl230Cfg.impedance = VLO_STATE;
					} else if (us_rms_calc < phyTxLoadThreshold2) {
						atpl230Cfg.impedance = LO_STATE_PK;
					}
				} else if (atpl230Cfg.impedance == LO_STATE_PK) {
					if (us_rms_calc < phyTxLoadThreshold4) {
						atpl230Cfg.impedance = VLO_STATE;
					}
				} else if (atpl230Cfg.impedance == VLO_STATE) {
					if (us_rms_calc > phyTxLoadThreshold3) {
						atpl230Cfg.impedance = HI_STATE;
					}
				}
			}
		}

		/* update stats values */
		atpl230.txTotal++;
		atpl230.txTotalBytes += us_phy_last_tx_lengths[uc_buf_idx];
		break;

	case ATPL230_TXRXBUF_RESULT_INPROCESS:
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txBadBusyTx++;
		break;

	case ATPL230_TXRXBUF_RESULT_WRONG_LEN:
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txBadLen++;
		break;

	case ATPL230_TXRXBUF_RESULT_BUSY_CHANNEL:
		/* clear busy channel indication */
		pplc_if_or8(REG_ATPL230_PHY_SFR, ATPL230_SFR_BCH_ERR_Msk);
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txBadBusyChannel++;
		break;

	case ATPL230_TXRXBUF_RESULT_PREV_TX_INPROCESS:
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txBadBusyTx++;
		break;

	case ATPL230_TXRXBUF_RESULT_RX_INPROCESS:
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txBadBusyChannel++;
		break;

	case ATPL230_TXRXBUF_RESULT_INVALID_SCHEME:
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txBadFormat++;
		break;

	case ATPL230_TXRXBUF_RESULT_TIMEOUT:
		/* reset phy layer using restore param */
		_init_phy_layer(PHY_RESET_SOFT_TYPE);
		/* update stats values */
		atpl230.txTotalErrors++;
		atpl230.txTimeout++;
		break;

	default:
		break;
	}

	/* update phy status var. */
	px_tx_result->uc_id_buffer = uc_buf_idx;
	px_tx_result->uc_result = uc_tx_result_value;

	/* store information about the last transmission */
	if ((atpl230.txQRMode) && (px_tx_result->uc_id_buffer < PHY_NUM_TX_BUFFERS)) {
		px_tx_result->rms_calc = pplc_if_read16(REG_ATPL230_TXRXBUF_RMSCALC1_TX0 + ((px_tx_result->uc_id_buffer) << 1));
	} else {
		px_tx_result->rms_calc = 0;
	}
}

/**
 * \brief Task to process TX PLC
 *
 */
static void _phy_tx_result_task(void)
{
	uint8_t uc_buf_idx;
	uint8_t uc_tx_result_ind;

	uc_tx_result_ind = uc_reg_tx_int;

	/* check first buffer */
	uc_buf_idx = 0;

	while (uc_tx_result_ind) {
		if (ATPL230_GET_TXINT_TX0(uc_tx_result_ind)) {
			/* set buff tx event */
			puc_phy_tx_buffer_event[uc_buf_idx] = true;
			/* clear buffer interrupt flag */
			pplc_if_and8(REG_ATPL230_TXRXBUF_TX_INT, (uint8_t) ~(ATPL230_TXRXBUF_TX_INT_TX0_Msk << uc_buf_idx));
			/* reset interrupt sw global flag */
			uc_reg_tx_int &= (uint8_t) ~(ATPL230_TXRXBUF_TX_INT_TX0_Msk << uc_buf_idx);

			/* enable reception and carrier detect */
			phy_carrier_detect_buff_enable(uc_buf_idx);
			phy_reception_buff_enable(uc_buf_idx);
		}

		/* Check next buffer indication */
		uc_tx_result_ind >>= 1;
		uc_buf_idx++;
		if (uc_buf_idx > 4) {
			return;
		}
	}
}

/**
 * \brief Task to process RX PLC
 *
 */
static void _phy_rx_task(void)
{
	uint8_t uc_rx_payload_ind;
	uint8_t uc_rx_header_ind;
	uint8_t uc_buf_idx;

	uc_rx_payload_ind = uc_reg_rx_int & 0xF0;
	uc_rx_header_ind = uc_reg_rx_int & 0x0F;
	uc_buf_idx = 0;
	while (uc_rx_payload_ind || uc_rx_header_ind) {
		if (ATPL230_GET_RXINT_HRX0(uc_rx_header_ind)) {
			/* clear interrupt flag */
			pplc_if_and8(REG_ATPL230_TXRXBUF_RX_INT, (uint8_t) ~(ATPL230_TXRXBUF_RX_INT_HRX0_Msk << uc_buf_idx));
			/* reset interrupt sw header flag */
			uc_reg_rx_int &= (uint8_t) ~(ATPL230_TXRXBUF_RX_INT_HRX0_Msk << uc_buf_idx);
		}

		if (ATPL230_GET_RXINT_PRX0(uc_rx_payload_ind)) {
			/* set buff rx event */
			puc_phy_rx_buffer_event[uc_buf_idx] = true;
		}

		/* check next buffer */
		uc_rx_header_ind >>= 1;
		uc_rx_payload_ind = (uc_rx_payload_ind >> 1) & 0xF0;
		uc_buf_idx++;
		if (uc_buf_idx > 4) {
			return;
		}
	}

	/* Noise capture */
	if (uc_reg_ns_int) {
		/* set buff event */
		puc_phy_noise_buffer_event = true;
		/* clear interrupt flag */
		pplc_if_and8(REG_ATPL230_TXRXBUF_TX_INT, (uint8_t) ~(ATPL230_TXRXBUF_TX_INT_N_Msk));
	}
}

/**
 * \brief Get PHY layer parameter
 *
 * \param us_id       Parameter Identification (see atpl230.h)
 * \param *p_val      Pointer to parameter value
 * \param uc_len      Length of the parameter
 *
 * \return PHY_CFG_SUCCESS if there is no error, otherwise returns
 *PHY_CFG_INVALID_INPUT.
 */
uint8_t phy_get_cfg_param(uint16_t us_id, void *p_val, uint16_t uc_len)
{
	uint8_t *memPtr = NULL;
	uint32_t val;

	if (ATPL230_REG_PARAM(us_id)) {
		if (uc_len == 1) {
			*((uint8_t *)p_val) = pplc_if_read8(us_id);
		} else if (uc_len == 2) {
			val = (uint32_t)pplc_if_read16(us_id);
			memcpy(p_val, (uint8_t *)&val, uc_len);
		} else if (uc_len == 4) {
			val = pplc_if_read32(us_id);
			memcpy(p_val, (uint8_t *)&val, uc_len);
		} else {
			pplc_if_read_buf(us_id, p_val, uc_len);
		}

		return(PHY_CFG_SUCCESS);
	} else if (ATPL230_ATT_PARAM(us_id) == ATPL230_ATT_PARAM_MSK) {
		memPtr = (uint8_t *)&atpl230ChnCfg + (us_id & 0xFF);
	} else if (ATPL230_CFG_PARAM(us_id) == ATPL230_CFG_PARAM_MSK) {
		memPtr = (uint8_t *)&atpl230Cfg + (us_id & 0xFF);
	} else if (ATPL230_PARAM(us_id) == ATPL230_PARAM_MSK) {
		memPtr = (uint8_t *)&atpl230 + (us_id & 0xFF);
	} else {
		return(PHY_CFG_INVALID_INPUT);
	}

	memcpy(p_val, memPtr, uc_len);

	return(PHY_CFG_SUCCESS);
}

/**
 * \brief Set PHY layer parameter
 *
 * \param us_id       Parameter Identification (see atpl230.h)
 * \param *p_val      Pointer to parameter value
 * \param uc_len      Length of the parameter
 *
 * \return PHY_CFG_SUCCESS if there is no error, otherwise returns
 *PHY_CFG_INVALID_INPUT.
 */
uint8_t phy_set_cfg_param(uint16_t us_id, void *p_val, uint16_t uc_len)
{
	uint8_t *memPtr = NULL;
	uint32_t val = 0;

	if (ATPL230_REG_PARAM(us_id)) {
		if (uc_len == 1) {
			pplc_if_write8(us_id, *((uint8_t *)p_val));
		} else if (uc_len == 2) {
			memcpy((uint8_t *)&val, (uint8_t *)p_val, uc_len);
			pplc_if_write16(us_id, (uint16_t)val);
		} else if (uc_len == 4) {
			memcpy((uint8_t *)&val, (uint8_t *)p_val, uc_len);
			pplc_if_write32(us_id, val);
		} else {
			pplc_if_write_buf(us_id, p_val, uc_len);
		}

		/* Manage special case to change SNA */
		if (us_id == REG_ATPL230_SNA0) {
			_upd_sna_crc((uint8_t *)p_val);
		}

		return(PHY_CFG_SUCCESS);
	} else if (ATPL230_ATT_PARAM(us_id) == ATPL230_ATT_PARAM_MSK) {
		memPtr = (uint8_t *)&atpl230ChnCfg + (us_id & 0xFF);
	} else if (ATPL230_CFG_PARAM(us_id) == ATPL230_CFG_PARAM_MSK) {
		memPtr = (uint8_t *)&atpl230Cfg + (us_id & 0xFF);
		/* Manage special case to change of polarity and emit modes */
		if (us_id == PHY_ID_CFG_TXRX1_POLARITY) {
			_update_txrx1_polarity(*((uint8_t *)p_val));
		} else if (us_id == PHY_ID_CFG_TXRX2_POLARITY) {
			_update_txrx1_polarity(*((uint8_t *)p_val));
		} else if (us_id == PHY_ID_CFG_DRIVER1_MODE) {
			_update_emit1_mode(*((uint8_t *)p_val));
		} else if (us_id == PHY_ID_CFG_DRIVER2_MODE) {
			_update_emit2_mode(*((uint8_t *)p_val));
		} else if (us_id == PHY_ID_CFG_COUPLING_BOARD) {
			atpl230Cfg.atplCoupling = *((uint8_t *)p_val);
			phy_init_coupling_cfg();
			phy_init_channel_cfg();
			_update_channel(atpl230Cfg.txrxChannel);
		} else if (us_id == PHY_ID_CFG_TXRX_CHANNEL) {
			if (!_update_channel(*((uint8_t *)p_val))) {
				return(PHY_CFG_INVALID_CHANNEL);
			}
		}
	} else if (ATPL230_PARAM(us_id) == ATPL230_PARAM_MSK) {
		memPtr = (uint8_t *)&atpl230 + (us_id & 0xFF);
	} else {
		return(PHY_CFG_INVALID_INPUT);
	}

	memcpy(memPtr, p_val, uc_len);

	return(PHY_CFG_SUCCESS);
}

/**
 * \brief Set PHY layer parameter
 *
 * \param us_id			Parameter Identification (see atpl230.h)
 * \param uc_cmd			Command to operation (PHY_CMD_CFG_AND,
 *PHY_CMD_CFG_OR, PHY_CMD_CFG_XOR)
 * \param uc_mask		Mask Bits
 *
 * \return PHY_CFG_SUCCESS if there is no error, otherwise returns
 *PHY_CFG_INVALID_INPUT.
 */
uint8_t phy_cmd_cfg_param(uint16_t us_id, uint8_t uc_cmd, uint8_t uc_mask)
{
	uint8_t result = PHY_CFG_SUCCESS;

	if (ATPL230_REG_PARAM(us_id)) {
		if (uc_cmd == PHY_CMD_CFG_AND) {
			pplc_if_and8(us_id, uc_mask);
		} else if (uc_cmd == PHY_CMD_CFG_OR) {
			pplc_if_or8(us_id, uc_mask);
		} else if (uc_cmd == PHY_CMD_CFG_XOR) {
			pplc_if_xor8(us_id, uc_mask);
		} else {
			result = PHY_CFG_INVALID_INPUT;
		}
	} else {
		result = PHY_CFG_INVALID_INPUT;
	}

	return result;
}

/**
 * \brief Write the transmitted data with ATPL230 device.
 *
 * \param px_msg	Pointer to message structure data.
 *
 * \retval PHY_TX_RESULT_PROCESS on success.
 * \retval PHY_TX_RESULT_INV_LENGTH on invalid length error.
 * \retval PHY_TX_RESULT_BUSY_CH on busy channel error.
 * \retval PHY_TX_RESULT_BUSY_TX on busy transmission error.
 * \retval PHY_TX_RESULT_INV_SCHEME on invalid scheme error.
 * \retval PHY_TX_RESULT_TIMEOUT on timeout error.
 */
uint8_t phy_tx_frame(xPhyMsgTx_t *px_msg)
{
	uint8_t uc_buff_id;
	uint8_t uc_scheme;
	uint8_t uc_mac_enable;
	uint8_t uc_head_type;
	uint8_t uc_pad_len;
	uint8_t uc_phy_buffer[PHY_MAX_PPDU_SIZE];
	uint8_t uc_tx_driver_mode;
	uint8_t uc_tx_driver;
	uint8_t uc_crc_type;
	uint8_t uc_robo_ctl;
	uint8_t uc_idx;
	uint8_t uc_ef_mode;
	uint16_t uc_phy_buffer_idx;
	uint16_t us_data_len;
	uint16_t us_max_phy_len;
	uint16_t us_total_len;
	uint16_t us_total_phy_len;
	uint16_t us_num_bytes_symbol;
	uint16_t us_payload_len;
	uint32_t ul_time_ref;
	uint32_t ul_tx_timeout;

	/* update local atpl230 data */
	atpl230.txIdBuff = px_msg->uc_buff_id;
	atpl230.txLevel = px_msg->att_level;
	atpl230.txScheme = px_msg->scheme;
	atpl230.txTdelay = px_msg->tdelay;
	atpl230.txMode = px_msg->mode;
	atpl230.txDisableRx = px_msg->disable_rx;
	uc_buff_id = atpl230.txIdBuff;
	uc_scheme = atpl230.txScheme;

	/* CHECK: Id buffer to transmit */
	if (uc_buff_id >= PHY_NUM_TX_BUFFERS) {
		return PHY_TX_RESULT_INV_BUFFER;
	}

	/*  CHECK: if tx buffer is enable (tx in process) */
	if (phy_transmission_buff_is_enable(uc_buff_id)) {
		/* update stats values */
		atpl230.txBadBusyTx++;
		/* return PHY_TX_RESULT_BUSY_TX; */
	}

	/*  MODE_TEST: R&D TX continuous mode */
	if (px_msg->mode == MODE_TEST) {
		/* Data is directly passed to FFT, so no header is needed */
		memcpy(uc_phy_buffer, px_msg->data_buf, px_msg->data_len);
		/* Transfer data to physical transmission buffer */
		pplc_if_write_buf(REG_ATPL230_PHY_TX_INIT_ADDRESS + (uc_buff_id * PHY_MAX_PPDU_SIZE), uc_phy_buffer, px_msg->data_len);

		/* set Global Attenuation */
		atpl230ChnCfg.txAttGlobal = uc_att_value_tab[px_msg->att_level];
		pplc_if_write8(REG_ATPL230_TXRXBUF_GLOBAL_AMP_TX0 + uc_buff_id, atpl230ChnCfg.txAttGlobal);

		/* config gains */
		pplc_if_write8(REG_ATPL230_TXRXBUF_EMIT_GAIN_TX0 + uc_buff_id, uc_emit_gain_tab[uc_scheme]);
		pplc_if_write8(REG_ATPL230_TXRXBUF_PEAK_CUT_GAIN_TX0 + uc_buff_id, uc_peak_cut_gain_tab[uc_scheme]);

		/* clear peak cut */
		pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t) ~ATPL230_TXRXBUF_TXCONF_PCO_Msk);

		if (atpl230Cfg.impedance == HI_STATE) {
			/* set attenuation: signal and chirp */
			pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpHighZ);
			pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalHighZ);
			/* bit flipping disable */
			pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t) ~ATPL230_TXRXBUF_TXCONF_BF_Msk);
			/* select branch (driver) */
			uc_tx_driver = atpl230Cfg.high_Z_driver;
		} else if (atpl230Cfg.impedance == LO_STATE) {
			/* set attenuation: signal and chirp */
			pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpLowZ);
			pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalLowZ);
			/* bit flipping enable */
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t)ATPL230_TXRXBUF_TXCONF_BF_Msk);
			/* select branch (driver) */
			uc_tx_driver = atpl230Cfg.low_Z_driver;
		} else if (atpl230Cfg.impedance == LO_STATE_PK) {
			/* set peak cut on */
			if (uc_peak_cut_on_enable_tab[uc_scheme]) {
				pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, uc_peak_cut_on_enable_tab[uc_scheme]);
			}

			/* set attenuation: signal and chirp */
			pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpLowZ);
			pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalLowZ);
			/* bit flipping enable */
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t)ATPL230_TXRXBUF_TXCONF_BF_Msk);
			/* select branch (driver) */
			uc_tx_driver = atpl230Cfg.vlow_Z_driver;
		} else {
			/* set peak cut on */
			if (uc_peak_cut_on_enable_tab[uc_scheme]) {
				pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, uc_peak_cut_on_enable_tab[uc_scheme]);
			}

			/* set attenuation: signal and chirp */
			pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpVLowZ);
			pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalVLowZ);
			/* bit flipping enable */
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t)ATPL230_TXRXBUF_TXCONF_BF_Msk);
			/* select branch (driver) */
			uc_tx_driver = atpl230Cfg.vlow_Z_driver;
		}

		/* Select driver */
		if (uc_tx_driver == DRIVER_1) {
			/* Set branch 1 */
			uc_tx_driver_mode = atpl230Cfg.driver1Mode;
			pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ~(ATPL230_TXRXBUF_TXCONF_BR_Msk << (uc_buff_id << 1)));
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ATPL230_TXRXBUF_TXCONF_BR1 << (uc_buff_id << 1));
		} else {
			/* Set branch 2 */
			uc_tx_driver_mode = atpl230Cfg.driver2Mode;
			pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ~(ATPL230_TXRXBUF_TXCONF_BR_Msk << (uc_buff_id << 1)));
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ATPL230_TXRXBUF_TXCONF_BR2 << (uc_buff_id << 1));
		}

		/* Enable buffer to transmit */
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, ATPL230_TXRXBUF_TXCONF_EB_Msk);
		/* Set continuous mode if it is needed */
		pplc_if_or8(REG_ATPL230_FFT_MODE, ATPL230_FFT_MODE_CONT_Msk);
		/* Set test mode if it is needed (Test mode ENABLED) */
		pplc_if_or8(REG_ATPL230_FFT_MODE, ATPL230_FFT_MODE_EN_Msk);
		/* force tx immediately */
		phy_force_tx_buff_enable(uc_buff_id);

		return PHY_TX_RESULT_PROCESS;
	}

	switch (uc_scheme) {
	case PROTOCOL_DBPSK:
	case PROTOCOL_DQPSK:
	case PROTOCOL_D8PSK:
	case PROTOCOL_DBPSK_VTB:
	case PROTOCOL_DQPSK_VTB:
	case PROTOCOL_D8PSK_VTB:
		break;

	case PROTOCOL_DBPSK_ROBO:
	case PROTOCOL_DQPSK_ROBO:
		if (px_msg->mode == MODE_TYPE_A) {
			/* update stats values */
			atpl230.txBadFormat++;
			return PHY_TX_RESULT_INV_SCHEME;
		}

		break;

	default:
		/* update stats values */
		atpl230.txBadFormat++;
		return PHY_TX_RESULT_INV_SCHEME;
	}

	uc_mac_enable = phy_get_mac_en();
	us_max_phy_len = PHY_MAX_PPDU_SIZE - PHY_DMA_OFFSET;
	us_data_len = px_msg->data_len;
	/* store length to use in statistic value after */
	us_phy_last_tx_lengths[uc_buff_id] = us_data_len;

	/*  CHECK: message len */
	if ((us_data_len < MAC_HEADER_SIZE) || (us_data_len > us_max_phy_len)) {
		/* update stats values */
		atpl230.txBadLen++;
		/* Length is not permitted */
		return PHY_TX_RESULT_INV_LENGTH;
	}

	/* Configuration OK */
	/* Adjust total length depending on CRC and PRIME mode */
	us_total_len = us_data_len;
	if (px_msg->mode == MODE_TYPE_A) {
		us_total_len -= MAC_HEADER_SIZE;

		uc_head_type = ATPL230_GET_HEADER_TYPE(px_msg->data_buf[0]);
		/* Get CRC type depending on packet type (only for Prime 1.3) */
		if (uc_mac_enable) {
			uc_crc_type = uc_crc_type_tab[uc_head_type];
			us_total_len += 1 +  uc_crc_type;
		}
	}

	/* Get symbol capacity depending on modulation scheme */
	us_num_bytes_symbol = uc_num_bytes_per_symbol_tab[uc_scheme];
	/* Add flushing byte in case of use VTB */
	if (uc_scheme & 0x04) {
		us_total_len++;
	}

	/* Get payload length in symbols */
	us_payload_len = (uint8_t)(us_total_len / us_num_bytes_symbol);
	/* Get symbol padding in bytes */
	uc_pad_len = us_num_bytes_symbol - (us_total_len % us_num_bytes_symbol);
	/* Correct symbol padding and payload length */
	if (uc_pad_len == us_num_bytes_symbol) {
		uc_pad_len = 0;
	} else {
		us_payload_len++;
	}

	/* For robust mode the number of symbols is multiplied by 4 */
	if (uc_scheme & 0x08) {
		us_payload_len <<= 2;
	}

	/* store tx payload len in symbols */
	atpl230.txPayloadLenSym = us_payload_len;

	/* adjust timeout for current msg and buffer */
	if (px_msg->mode == MODE_TYPE_A) {
		ul_tx_timeout = (7 + atpl230.txPayloadLenSym * 3) * 100;
		pplc_if_write32(REG_ATPL230_TXRXBUF_TIMEOUT1_TX0 + (px_msg->uc_buff_id << 2), ul_tx_timeout);
	} else if (px_msg->mode == MODE_TYPE_B) {
		ul_tx_timeout = (18 + atpl230.txPayloadLenSym * 3) * 100;
		pplc_if_write32(REG_ATPL230_TXRXBUF_TIMEOUT1_TX0 + (px_msg->uc_buff_id << 2), ul_tx_timeout);
	} else if (px_msg->mode == MODE_TYPE_BC) {
		ul_tx_timeout = (25 + atpl230.txPayloadLenSym * 3) * 100;
		pplc_if_write32(REG_ATPL230_TXRXBUF_TIMEOUT1_TX0 + (px_msg->uc_buff_id << 2), ul_tx_timeout);
	}

	/* Get total len to transmit and build PHY headers in each prime mode */
	memset(uc_phy_buffer, 0, sizeof(uc_phy_buffer));
	switch (px_msg->mode) {
	case MODE_TYPE_A:
		/* MAC header is included in both PHY header and payload */
		us_total_phy_len = PHY_DMA_OFFSET + (us_data_len - MAC_GEN_HEADER_SIZE);
		/* First two bytes corresponds to physical header */
		uc_phy_buffer[0] = ((uc_scheme << 4) & 0xf0) | ((us_payload_len >> 2) & 0x0f);
		uc_phy_buffer[1] = ((us_payload_len << 6) & 0xc0) | (uc_pad_len & 0x3f);
		/* Move 7 first bytes from MAC header (54 bits) */
		memcpy(&uc_phy_buffer[2], &px_msg->data_buf[0], MAC_HEADER_SIZE);
		/* Last bytes in header corresponds to CRC8(hardware) and flushing byte */
		uc_phy_buffer[9] = 0xAA;
		uc_phy_buffer[10] = 0;
		/* Add last two bytes from MAC header: bytes 8, 9 are in phy header, but they transmit as payload */
		if (us_data_len > MAC_HEADER_SIZE) {
			uc_phy_buffer[11] = px_msg->data_buf[7];
		}

		if (us_data_len > (MAC_HEADER_SIZE + 1)) {
			uc_phy_buffer[12] = px_msg->data_buf[8];
		}

		/* Add MAC payload */
		if (us_data_len > MAC_GEN_HEADER_SIZE) {
			memcpy(&uc_phy_buffer[PHY_DMA_OFFSET], &px_msg->data_buf[MAC_GEN_HEADER_SIZE], us_data_len - MAC_GEN_HEADER_SIZE);
		}

		break;

	case MODE_TYPE_B:
		/* MAC header is included in PHY payload */
		us_total_phy_len = PHY_DMA_OFFSET + us_data_len;
		/* First three bytes corresponds to physical header */
		uc_phy_buffer[0] = ((uc_scheme << 4) & 0xf0) | ((us_payload_len >> 4) & 0x0f);
		uc_phy_buffer[1] = ((us_payload_len << 4) & 0xf0) | ((uc_pad_len >> 5) & 0x0f);
		uc_phy_buffer[2] = ((uc_pad_len << 3) & 0xf8);
		/* Last bytes in header corresponds to CRC12 and flushing byte */
		uc_phy_buffer[3] = 0xAA;
		uc_phy_buffer[4] = 0xA0;
		uc_phy_buffer[5] = 0x00;
		/* Add type B payload */
		memcpy(&uc_phy_buffer[PHY_DMA_OFFSET], px_msg->data_buf, us_data_len);
		break;

	case MODE_TYPE_BC:
		/* MAC header is included in PHY payload */
		us_total_phy_len = (PHY_DMA_OFFSET << 1) + us_data_len;
		/* Build Type A header */
		uc_phy_buffer[0] = 0x40 | (((us_payload_len + 8) >> 2) & 0x0f);
		uc_phy_buffer[1] = (((us_payload_len + 8) << 6) & 0xc0) | 0x04;
		uc_phy_buffer[2] = 0x1A;
		uc_phy_buffer[3] = uc_bc_mode_config_value[2];
		uc_phy_buffer[4] = uc_bc_mode_config_value[3];
		uc_phy_buffer[5] = uc_bc_mode_config_value[4];
		uc_phy_buffer[6] = uc_bc_mode_config_value[5];
		uc_phy_buffer[7] = uc_bc_mode_config_value[6];
		uc_phy_buffer[8] = uc_bc_mode_config_value[7];
		/* Last bytes in header corresponds to CRC8(hardware) and flushing byte */
		uc_phy_buffer[9] = 0xAA;
		uc_phy_buffer[10] = 0;
		/* Build Type B header */
		uc_phy_buffer[PHY_DMA_OFFSET] = ((uc_scheme << 4) & 0xf0) | ((us_payload_len >> 4) & 0x0f);
		uc_phy_buffer[PHY_DMA_OFFSET + 1] = ((us_payload_len << 4) & 0xf0) | ((uc_pad_len >> 5) & 0x0f);
		uc_phy_buffer[PHY_DMA_OFFSET + 2] = ((uc_pad_len << 3) & 0xf8);
		/* Last bytes in header corresponds to CRC12 and flushing byte */
		uc_phy_buffer[PHY_DMA_OFFSET + 3] = 0xAA;
		uc_phy_buffer[PHY_DMA_OFFSET + 4] = 0xA0;
		uc_phy_buffer[PHY_DMA_OFFSET + 5] = 0x00;
		/* Add type B payload */
		memcpy(&uc_phy_buffer[PHY_DMA_OFFSET << 1], px_msg->data_buf, us_data_len);
		break;

	default:
		return PHY_TX_RESULT_INV_PRIME_MODE;
	}

	/* set idx to access phy buffer */
	uc_phy_buffer_idx = us_total_phy_len;

	/* Set flushing byte to zero */
	if ((uc_scheme & 0x08) || (uc_scheme & 0x04)) {
		uc_phy_buffer[uc_phy_buffer_idx++] = 0;
	}

	/* Set symbol padding bytes to zero */
	for (uc_idx = 0; uc_idx < uc_pad_len; uc_idx++) {
		uc_phy_buffer[uc_phy_buffer_idx++] = 0;
	}

	/* update total len */
	us_total_phy_len = uc_phy_buffer_idx;

	/* Transfer data to physical transmission buffer */
	pplc_if_write_buf(REG_ATPL230_PHY_TX_INIT_ADDRESS + (uc_buff_id * PHY_MAX_PPDU_SIZE), uc_phy_buffer, us_total_phy_len);

	/* reset robo ctl value */
	pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_ROBO_CTL, ~(0x03 << (uc_buff_id << 1)));

	/* Configure transmission mode: ROBO_CTL */
	switch (px_msg->mode) {
	case MODE_TYPE_A:
		uc_robo_ctl = 0x00;
		break;

	case MODE_TYPE_B:
		uc_robo_ctl = 0x02;
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_ROBO_CTL, uc_robo_ctl << (uc_buff_id << 1));
		break;

	case MODE_TYPE_BC:
		uc_robo_ctl = 0x03;
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_ROBO_CTL, uc_robo_ctl << (uc_buff_id << 1));
		break;

	default:
		break;
	}

	/* set Global Attenuation */
	atpl230ChnCfg.txAttGlobal = uc_att_value_tab[px_msg->att_level];
	pplc_if_write8(REG_ATPL230_TXRXBUF_GLOBAL_AMP_TX0 + uc_buff_id, atpl230ChnCfg.txAttGlobal);

	/* config gains */
	pplc_if_write8(REG_ATPL230_TXRXBUF_EMIT_GAIN_TX0 + uc_buff_id, uc_emit_gain_tab[uc_scheme]);
	pplc_if_write8(REG_ATPL230_TXRXBUF_PEAK_CUT_GAIN_TX0 + uc_buff_id, uc_peak_cut_gain_tab[uc_scheme]);

	/* clear peak cut on */
	pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, 0x7F);

	if (atpl230Cfg.impedance == HI_STATE) {
		/* set attenuation: signal and chirp */
		pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpHighZ);
		pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalHighZ);
		/* set hardware times */
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TB1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeBeforeTxHighZ);
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TA1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeAfterTxHighZ);
		/* bit flipping disable */
		pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t) ~ATPL230_TXRXBUF_TXCONF_BF_Msk);
		/* select branch (driver) */
		uc_tx_driver = atpl230Cfg.high_Z_driver;
	} else if (atpl230Cfg.impedance == LO_STATE) {
		/* set attenuation: signal and chirp */
		pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpLowZ);
		pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalLowZ);
		/* set hardware times */
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TB1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeBeforeTxLowZ);
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TA1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeAfterTxLowZ);
		/* bit flipping enable */
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t)ATPL230_TXRXBUF_TXCONF_BF_Msk);
		/* select branch (driver) */
		uc_tx_driver = atpl230Cfg.low_Z_driver;
	} else if (atpl230Cfg.impedance == LO_STATE_PK) {
		/* set peak cut on */
		if (uc_peak_cut_on_enable_tab[uc_scheme]) {
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, uc_peak_cut_on_enable_tab[uc_scheme]);
		}

		/* set attenuation: signal and chirp */
		pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpLowZ);
		pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalLowZ);
		/* set hardware times */
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TB1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeBeforeTxLowZ);
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TA1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeAfterTxLowZ);
		/* bit flipping enable */
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t)ATPL230_TXRXBUF_TXCONF_BF_Msk);
		/* select branch (driver) */
		uc_tx_driver = atpl230Cfg.vlow_Z_driver;
	} else {
		/* set peak cut on */
		if (uc_peak_cut_on_enable_tab[uc_scheme]) {
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, uc_peak_cut_on_enable_tab[uc_scheme]);
		}

		/* set attenuation: signal and chirp */
		pplc_if_write8(REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 + uc_buff_id, phyTxAttChirpVLowZ);
		pplc_if_write8(REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 + uc_buff_id, phyTxAttSignalVLowZ);
		/* set hardware times */
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TB1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeBeforeTxLowZ);
		pplc_if_write16(REG_ATPL230_TXRXBUF_TXRX_TA1_TX0 + (uc_buff_id << 1), atpl230Cfg.timeAfterTxLowZ);
		/* bit flipping enable */
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, (uint8_t)ATPL230_TXRXBUF_TXCONF_BF_Msk);
		/* select branch (driver) */
		uc_tx_driver = atpl230Cfg.vlow_Z_driver;
	}

	/* Select driver */
	if (uc_tx_driver == DRIVER_1) {
		/* Set branch 1 */
		uc_tx_driver_mode = atpl230Cfg.driver1Mode;
		pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ~(ATPL230_TXRXBUF_TXCONF_BR_Msk << (uc_buff_id << 1)));
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ATPL230_TXRXBUF_TXCONF_BR1 << (uc_buff_id << 1));
	} else if (uc_tx_driver == DRIVER_2) {
		/* Set branch 2 */
		uc_tx_driver_mode = atpl230Cfg.driver2Mode;
		pplc_if_and8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ~(ATPL230_TXRXBUF_TXCONF_BR_Msk << (uc_buff_id << 1)));
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, ATPL230_TXRXBUF_TXCONF_BR2 << (uc_buff_id << 1));
	} else {
		/* Set branch 1 + 2 */
		uc_tx_driver_mode = atpl230Cfg.driver1Mode;
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH, (ATPL230_TXRXBUF_TXCONF_BR1 | ATPL230_TXRXBUF_TXCONF_BR2) << (uc_buff_id << 1));
	}

	/* Set hardware times depending on driver mode */
	uc_ef_mode = uc_emit_freq_tab[atpl230Cfg.txrxChannel];
	if (uc_tx_driver_mode == EXTERNAL_DRV_MODE) {
		/* set hardware times for external mode */
		pplc_if_write16(REG_ATPL230_SOFT_STOP_TIMEH, uc_stop_time[uc_ef_mode]);
		pplc_if_write16(REG_ATPL230_SOFT_TIME_X1_H, uc_time_X1[uc_ef_mode]);
		pplc_if_write16(REG_ATPL230_SOFT_TIME_X2_H, uc_time_X2[uc_ef_mode]);
		pplc_if_write8(REG_ATPL230_SOFT_TIME_Y1, uc_time_Y1[uc_ef_mode]);
		pplc_if_write8(REG_ATPL230_SOFT_TIME_Y2, uc_time_Y2[uc_ef_mode]);
	} else {
		/* set hardware times for internal mode */
		pplc_if_write16(REG_ATPL230_SOFT_STOP_TIMEH, uc_stop_time[uc_ef_mode]);
		pplc_if_write16(REG_ATPL230_SOFT_TIME_X1_H, uc_time_X1[uc_ef_mode]);
		pplc_if_write16(REG_ATPL230_SOFT_TIME_X2_H, uc_time_X2[uc_ef_mode]);
	}

	/* Disable reception and carrier detect if it is needed */
	if (px_msg->disable_rx) {
		phy_carrier_detect_buff_disable(uc_buff_id);
		phy_reception_buff_disable(uc_buff_id);
	}

	/* set delay in transmission */
	if (px_msg->tdelay) {
		/* use delay mode */
		if (px_msg->tmode == PHY_TX_SCHEDULING_MODE_RELATIVE) {
			/* Differential time mode */
			if (px_msg->tdelay < PHY_TX_MIN_DELAY) {
				px_msg->tdelay = PHY_TX_MIN_DELAY;
			}

			ul_time_ref = pplc_if_read32(REG_ATPL230_VHIGH_TIMER_BEACON_REF);
			pplc_if_write32(REG_ATPL230_TXRXBUF_EMITIME1_TX0 + (uc_buff_id << 2), ul_time_ref + px_msg->tdelay);
			/* Enable buffer to transmit */
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, ATPL230_TXRXBUF_TXCONF_EB_Msk);
		} else {
			/* Absolute time mode */
			pplc_if_write32(REG_ATPL230_TXRXBUF_EMITIME1_TX0 + (uc_buff_id << 2), px_msg->tdelay);
			/* Enable buffer to transmit */
			pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, ATPL230_TXRXBUF_TXCONF_EB_Msk);
		}
	} else { 
		/* use forced mode: avoid false tx in case of forced tx */
		ul_time_ref = pplc_if_read32(REG_ATPL230_VHIGH_TIMER_BEACON_REF);
		pplc_if_write32(REG_ATPL230_TXRXBUF_EMITIME1_TX0 + (uc_buff_id << 2), ul_time_ref - 10);
		/* Enable buffer to transmit */
		pplc_if_or8(REG_ATPL230_TXRXBUF_TXCONF_TX0 + uc_buff_id, ATPL230_TXRXBUF_TXCONF_EB_Msk);
		/* force tx immediately (must be after enable flag set) */
		phy_force_tx_buff_enable(uc_buff_id);

#ifdef PHY_OFFSET_SYMBOL_CONTROL
		/* launch phy tx timer */
		if (uc_scheme == PROTOCOL_D8PSK) {
			_start_timer_tx_offset_symb(2240, true);
		} else {
			_start_timer_tx_offset_symb(2240, false);
		}
#endif
	}

	return PHY_TX_RESULT_PROCESS;
}

/**
 * \brief Reset reception interrupt
 *
 * \param uc_buf_idx	Buffer index
 *
 */
static void _reset_rx_flag_interrupt(uint8_t uc_buf_idx)
{
#ifdef PPLC_INT_IRQn
	/* enter in critical region */
	NVIC_DisableIRQ(PPLC_INT_IRQn);
#endif
	/* clear interrupt flag -> it must execute after read all information
	 * trhough spi to avoid overwrite data */
	pplc_if_and8(REG_ATPL230_TXRXBUF_RX_INT, (uint8_t) ~(ATPL230_TXRXBUF_RX_INT_PRX0_Msk << uc_buf_idx));
	/* reset interrupt sw global flag */
	uc_reg_rx_int &= (uint8_t) ~(ATPL230_TXRXBUF_RX_INT_PRX0_Msk << uc_buf_idx);
#ifdef PPLC_INT_IRQn
	/* exit in critical region */
	NVIC_EnableIRQ(PPLC_INT_IRQn);
#endif
}

/**
 * \brief Read the received data with ATPL230 device.
 *
 * \param px_msg	Pointer to message structure data.
 *
 */
void phy_rx_frame_cb(xPhyMsgRx_t *px_msg)
{
	uint8_t uc_buf_idx = 0;
	uint8_t uc_event_flag;
	uint8_t uc_reg_robo_mode;
	uint8_t *puc_phy_header_buf;
	uint8_t uc_mac_enable;
	uint8_t uc_crc_type;
	uint32_t ul_data_buf_address;
	uint8_t uc_idx;
	uint8_t uc_rx_padlen = 0;
	uint8_t uc_noise_conf;
	uint16_t us_read_phy_header_address;
	uint16_t us_rx_symbol_len = 0;

	/* check RX event */
	uc_event_flag = false;
	for (uc_idx = 0; uc_idx < 4; uc_idx++) {
		if (puc_phy_rx_buffer_event[uc_last_rx_buf]) {
			puc_phy_rx_buffer_event[uc_last_rx_buf] = false;
			/* get buffer idx of the current event */
			uc_buf_idx = uc_last_rx_buf++;
			uc_last_rx_buf &= 0x03;
			uc_event_flag = true;
			break;
		}
	}

	if (!uc_event_flag) {
		/* check noise event */
		if (puc_phy_noise_buffer_event) {
			px_msg->mode = MODE_NOISE;
			uc_noise_conf = pplc_if_read8(REG_ATPL230_TXRXBUF_NOISECONF);
			px_msg->noise_result = ATPL230_GET_NOISE_RESULT(uc_noise_conf);
			px_msg->data_len = ATPL230_GET_NOISE_NS(uc_noise_conf) + 1;
			px_msg->data_len *= 97;
		} else {
			px_msg->data_len = 0;
			return;
		}
	}

	/* extract info from phy header: robo mode, scheme, len, padlen. */
	if (puc_phy_noise_buffer_event) {
		puc_phy_noise_buffer_event = false;
		px_msg->mode = MODE_NOISE;
	} else {
		/* get physical reception data address */
		us_read_phy_header_address = REG_ATPL230_PHY_RX_INIT_ADDRESS + (PHY_MAX_PPDU_SIZE * uc_buf_idx);
		puc_phy_header_buf = &uc_phy_headers_buffer[uc_buf_idx][0];
		/* read data header */
		pplc_if_read_buf(us_read_phy_header_address, puc_phy_header_buf, PHY_DMA_OFFSET);
		/* get robo mode */
		uc_reg_robo_mode = pplc_if_read8(REG_ATPL230_TXRXBUF_RXCONF_INFO_ROBO_MODE);
		px_msg->mode = ATPL230_GET_ROBO_MODE_RX(uc_buf_idx, uc_reg_robo_mode);
	}

	switch (px_msg->mode) {
	case MODE_TYPE_A:
		/* Get received modulation scheme from PHY 1.3 header */
		px_msg->scheme = 0x0F & (*puc_phy_header_buf >> 4);
		/* Get payload length in symbols */
		us_rx_symbol_len = (*puc_phy_header_buf++ & 0x0F) << 2;
		us_rx_symbol_len += (*puc_phy_header_buf >> 6) & 0x03;
		/* Get padding len in bytes from PHY header */
		uc_rx_padlen = *puc_phy_header_buf & 0x3F;
		/* update stats values */
		/* store rx payload len in symbols (before adjust in robust
		 * mode) */
		atpl230.rxPayloadLenSym = us_rx_symbol_len;
		break;

	case MODE_TYPE_BC:
	case MODE_TYPE_B:
		/* Get received modulation scheme from PHY 1.4 header */
		px_msg->scheme = 0x0F & (*puc_phy_header_buf >> 4);
		/* Get payload length in symbols */
		us_rx_symbol_len = (*puc_phy_header_buf++ & 0x0F) << 4;
		us_rx_symbol_len += (*puc_phy_header_buf >> 4) & 0x0F;
		/* Get padding len in bytes from PHY header */
		uc_rx_padlen = (*puc_phy_header_buf++ & 0x0F) << 5;
		uc_rx_padlen += (*puc_phy_header_buf >> 3) & 0x1F;
		uc_rx_padlen &= 0x3F;             /* ajuste de 6 bits */
		/* update stats values */
		/* store rx payload len in symbols (before adjust in robust
		 * mode) */
		atpl230.rxPayloadLenSym = us_rx_symbol_len;
		break;

	case MODE_NOISE:
		/* Read received Data */
		ul_data_buf_address = REG_ATPL230_PHY_NOISE_INIT_ADDRESS;
		pplc_if_read_buf(ul_data_buf_address, px_msg->data_buf, px_msg->data_len);
		return;

	case MODE_TEST:
		break;

	default:
		atpl230.rxTotalErrors++;
		px_msg->data_len = 0;
		/* reset phy interrupt flag */
		_reset_rx_flag_interrupt(uc_buf_idx);
		return;
	}

	/* For robust mode the number of symbols is divided by 4 */
	if (px_msg->scheme & 0x08) {
		us_rx_symbol_len >>= 2;
	}

	/* Get received length in bytes */
	px_msg->data_len = us_rx_symbol_len * uc_num_bytes_per_symbol_tab[px_msg->scheme];
	/* Remove symbol padding */
	px_msg->data_len -= uc_rx_padlen;
	/* Divide length and remove flushing byte if convolution used */
	if (px_msg->scheme & 0x04) {
		px_msg->data_len--;
	}

	/* check data_len value: Maximum value or */
	/* minimum value: mac header size is 9 bytes and there are 7 bytes of
	 * them in phy header, */
	/* so minimun len is 2 bytes at least (9-7). */
	if ((px_msg->data_len > PHY_MAX_PPDU_SIZE) ||
			(px_msg->data_len < (MAC_GEN_HEADER_SIZE - MAC_HEADER_SIZE))) {
		/* update stats values */
		atpl230.rxTotalErrors++;
		atpl230.rxBadLen++;
		px_msg->data_len = 0;
		/* reset phy interrupt flag */
		_reset_rx_flag_interrupt(uc_buf_idx);
		return;
	}

	/* send message notification to rx queue */
	px_msg->uc_buff_id = uc_buf_idx;

	/* get message from atpl230 internal buffers */
	ul_data_buf_address = REG_ATPL230_PHY_RX_INIT_ADDRESS + (uc_buf_idx * PHY_MAX_PPDU_SIZE);
	switch (px_msg->mode) {
	case MODE_TYPE_A:
		/* get mac enable status */
		uc_mac_enable = phy_get_mac_en();
		/* Update bytes in phy header in case of PRIME v1.3 */
		px_msg->data_len += MAC_HEADER_SIZE;
		/* Build header received Data */
		memcpy(px_msg->data_buf, &uc_phy_headers_buffer[uc_buf_idx][2], MAC_HEADER_SIZE);
		/* Last two bytes of the MAC header */
		px_msg->data_buf[7] = uc_phy_headers_buffer[uc_buf_idx][11];
		px_msg->data_buf[8] = uc_phy_headers_buffer[uc_buf_idx][12];
		/* Read header type. In v1.3 header type is in phy header, but
		 * we get it from buffer data */
		px_msg->header_type = ATPL230_GET_HEADER_TYPE(px_msg->data_buf[0]);
		if (px_msg->header_type > PHY_HT_BEACON) {
			/* error in PRIME mode. Invalid msg. */
			px_msg->data_len = 0;
			/* reset phy interrupt flag */
			_reset_rx_flag_interrupt(uc_buf_idx);
			return;
		}

		/* get crc type depending on header type */
		if (uc_mac_enable) {
			uc_crc_type = uc_crc_type_tab[px_msg->header_type];
			px_msg->data_len -= 1 + uc_crc_type;
		}

		/* read payload data */
		pplc_if_read_buf(ul_data_buf_address  + PHY_DMA_OFFSET, &px_msg->data_buf[MAC_GEN_HEADER_SIZE], px_msg->data_len - MAC_GEN_HEADER_SIZE);
		break;

	case MODE_TYPE_BC:
	case MODE_TYPE_B:
		/* Read received Data */
		pplc_if_read_buf(ul_data_buf_address  + PHY_DMA_OFFSET, px_msg->data_buf, px_msg->data_len);
		break;

	case MODE_NOISE:
	case MODE_TEST:
		break;

	default:
		/* error in PRIME mode. Invalid msg. */
		px_msg->data_len = 0;
		/* reset phy interrupt flag */
		_reset_rx_flag_interrupt(uc_buf_idx);
		return;
	}

	/* update stats values */
	atpl230.rxTotal++;

	/* update stats values */
	atpl230.rxTotalBytes += px_msg->data_len;

	/* store information about the last received frame */
	atpl230.rxIdBuff = px_msg->uc_buff_id;
	atpl230.rxScheme = px_msg->scheme;
	atpl230.rxHdrRcv = px_msg->header_type;
	atpl230.rxPayloadLen = px_msg->data_len;
	atpl230.rxMode = px_msg->mode;
	if ((atpl230.rxQRMode) && (uc_buf_idx < PHY_NUM_RX_BUFFERS)) {
		px_msg->rssi_avg = pplc_if_read8(REG_ATPL230_TXRXBUF_RSSIAVG_RX0 + uc_buf_idx);
		if (atpl230.rxPayloadLenSym < 4) {
			px_msg->cinr_avg = pplc_if_read8(REG_ATPL230_TXRXBUF_CINRMIN_RX0 + uc_buf_idx);
		} else {
			px_msg->cinr_avg = pplc_if_read8(REG_ATPL230_TXRXBUF_CINRAVG_RX0 + uc_buf_idx);
		}

		px_msg->evm_header = pplc_if_read16(REG_ATPL230_TXRXBUF_EVM_HEADER1_RX0 + (uc_buf_idx << 1));
		px_msg->evm_payload = pplc_if_read16(REG_ATPL230_TXRXBUF_EVM_PAYLOAD1_RX0 + (uc_buf_idx << 1));
		px_msg->evm_header_acum = pplc_if_read32(REG_ATPL230_TXRXBUF_EVM_HEADACUM1_RX0 + (uc_buf_idx << 2));
		px_msg->evm_payload_acum = pplc_if_read32(REG_ATPL230_TXRXBUF_EVM_PAYLACUM1_RX0 + (uc_buf_idx << 2));
	} else {
		px_msg->rssi_avg = 0;
		px_msg->cinr_avg = 0;
		px_msg->evm_header = 0;
		px_msg->evm_payload = 0;
		px_msg->evm_header_acum = 0;
		px_msg->evm_payload_acum = 0;
	}

	/* get rx time */
	px_msg->rx_time = pplc_if_read32(REG_ATPL230_TXRXBUF_RECTIME1_RX0 + (uc_buf_idx << 2));
	/* reset phy interrupt flag */
	_reset_rx_flag_interrupt(uc_buf_idx);
}

/**
 * \brief PHY interrupt management
 *
 */
void phy_handler(void)
{
	volatile uint8_t vuc_int_tx;
	volatile uint8_t vuc_int_rx;

	if (phy_get_sfr_err()) {
		/* read only rx payload interrupts */
		vuc_int_rx = (pplc_if_read8(REG_ATPL230_TXRXBUF_RX_INT) & 0xF0);
		/* detect only new flags */
		vuc_int_rx ^= uc_reg_rx_int;
		/* clear only new interrupt flags */
		pplc_if_and8(REG_ATPL230_TXRXBUF_RX_INT, (uint8_t)(0xFF - vuc_int_rx));
		/* clear sfr ERR */
		phy_clear_sfr_err();
		/* clear INT */
		phy_clear_global_interrupt();
	} else {
		/* read interrupts status */
		vuc_int_tx = pplc_if_read8(REG_ATPL230_TXRXBUF_TX_INT);
		vuc_int_rx = pplc_if_read8(REG_ATPL230_TXRXBUF_RX_INT);
		/* clear INT */
		phy_clear_global_interrupt();
		/* Set interrupts to attend */
		uc_reg_rx_int |= vuc_int_rx;
		uc_reg_tx_int |= (vuc_int_tx & 0x0F);
		uc_reg_ns_int |= (vuc_int_tx & 0x10);

		/* enable rx task */
		if (uc_reg_rx_int || uc_reg_ns_int) {
			_phy_rx_task();
		}

		/* enable tx task(confirm) */
		if (uc_reg_tx_int) {
			_phy_tx_result_task();
		}
	}
}

/**
 * \brief Create PHY tasks, queues and semaphores
 * Initialize physical parameters and configure ATPL230 device
 *
 */
void phy_init(uint8_t uc_ifaceEnable)
{
	/* Store local ifaces configuration */
	uc_serial_ifaces_cfg = uc_ifaceEnable;

	/* Initialize PPLC driver */
	pplc_if_init();

	/* Set handler */
	pplc_set_handler(phy_handler);

	/* Reset PHY layer */
	phy_reset(PHY_RESET_HARD_TYPE);

#ifdef PHY_OFFSET_SYMBOL_CONTROL
	/* Init phy timer */
	_init_timer_tx_offset_symb();
#endif
}

/**
 * \brief Reset PHY layer including pplc service and serial ifaces
 * Initialize physical parameters and configure ATPL230 device
 *
 */
void phy_reset(uint8_t uc_reset_type)
{
	/* Initialize PHY layer parameters */
	_init_phy_layer(uc_reset_type);

	/* Init PHY serial interface */
	if (uc_serial_ifaces_cfg & SERIAL_IF_ENABLE) {
		serial_if_init();
	} else if (uc_serial_ifaces_cfg & SNIFFER_IF_ENABLE) {
		sniffer_if_init(true);
	}
}

/**
 * \brief Dummy Phy Tester Tool Serialization Addon
 *
 */
void Dummy_serial_if_init(void)
{
}

/**
 * \brief Dummy Phy Sniffer Serialization Addon
 *
 */
void Dummy_sniffer_if_init(uint8_t uc_enable_led)
{
	UNUSED(uc_enable_led);
}

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */
