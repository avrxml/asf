/**
 * @file tal_4g_utils.c
 *
 * @brief This file supports the TAL PIB attributes by providing 4g specific
 *        information.
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

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

#define OQPSK_SYMBOL_DURATION_TABLE \
        /* symbol duration depends on chip rate */ \
	{ 320, 160, 64, 64 }

#define OQPSK_SYMBOL_DURATION_TABLE_SIZE 4
#define OQPSK_SYMBOL_DURATION_TABLE_DATA_TYPE  uint16_t

#define OQPSK_SYMBOL_LENGTH_TABLE \
        /* symbol length in chips depends on chip rate */ \
        /* 100, 200, 1000, 2000 kchip */ \
	{ 32, 32, 64, 128 }

#define OQPSK_SYMBOL_LENGTH_TABLE_SIZE 4
#define OQPSK_SYMBOL_LENGTH_TABLE_DATA_TYPE  uint8_t

#define OQPSK_CHIP_RATE_TABLE \
        /* kbit * 10 */	\
	{ 10, 20, 100, 200 }

#define OQPSK_CHIP_RATE_TABLE_SIZE 4
#define OQPSK_CHIP_RATE_TABLE_DATA_TYPE  uint8_t

#define OQPSK_ACK_TIMING_OFFSET_TABLE \
        /* Offset for ACK timing depending on chip rate */ \
        /* 100, 200, 1000, 2000 */ \
	{ 90, 50, 25, 20 }

#define OQPSK_ACK_TIMING_OFFSET_TABLE_SIZE 4
#define OQPSK_ACK_TIMING_OFFSET_TABLE_DATA_TYPE  uint8_t

#define OQPSK_CHIP_RATE_FREQ_TABLE \
        /* rows: freq band ; column: chip rate (kchip/s) */ \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 100 }, \
	{ CHINA_780, 1000 }, \
	{ EU_863, 100 }, \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 1000 }, \
	{ KOREA_917, 1000 }, \
	{ JAPAN_920, 100 }, \
        /* US_928 */ \
        /* { JAPAN_950, 320 }, */ \
        /* US_1427 */ \
	{ WORLD_2450, 2000 }

#define OQPSK_CHIP_RATE_FREQ_TABLE_ROW_SIZE 7
#define OQPSK_CHIP_RATE_FREQ_TABLE_COL_SIZE 2
#define OQPSK_CHIP_RATE_FREQ_TABLE_DATA_TYPE  uint16_t

#define FSK_CCA_THRES_50_KBIT   (-91 + CCA_OFFSET) /* dBm */
#define FSK_CCA_THRES_100_KBIT  (-88 + CCA_OFFSET) /* dBm */
#define FSK_CCA_THRES_150_KBIT  (-86 + CCA_OFFSET) /* dBm */
#define FSK_CCA_THRES_200_KBIT  (-85 + CCA_OFFSET) /* dBm */
#define FSK_CCA_THRES_300_KBIT  (-83 + CCA_OFFSET) /* dBm */
#define FSK_CCA_THRES_400_KBIT  (-82 + CCA_OFFSET) /* dBm */
#define FK_CCA_THRES_FEC_OFFSET 6   /* dBm */

#define FSK_CCA_THRES_TABLE \
	{ FSK_CCA_THRES_50_KBIT, FSK_CCA_THRES_100_KBIT, FSK_CCA_THRES_150_KBIT, \
	  FSK_CCA_THRES_200_KBIT, FSK_CCA_THRES_300_KBIT, \
	  FSK_CCA_THRES_400_KBIT  }
#define FSK_CCA_THRES_TABLE_SIZE    6
#define FSK_CCA_THRES_DATA_TYPE     int8_t

#define OQPSK_CCA_DURATION_TABLE \
        /* aCCATime in symbols depending von chip rate */ \
        /* 100, 200, 1000, 2000 kchip */ \
	{ 4, 4, 8, 8 }

#define OQPSK_CCA_DURATION_TABLE_SIZE 4
#define OQPSK_CCA_DURATION_TABLE_DATA_TYPE  uint8_t

#define OQPSK_CCA_THRES  (-90)   /* dBm */
#define CCA_OFFSET  10
/* CCA threshold table is based on table 159, pg. 92 */
#define OFDM_CCA_THRES_TABLE \
        /* rows: MCSn; column: option n */ \
	{ (-103 + CCA_OFFSET), (-105 + CCA_OFFSET), (-105 + CCA_OFFSET), \
	  (-105 + CCA_OFFSET) }, \
	{ (-100 + CCA_OFFSET), (-103 + CCA_OFFSET), (-105 + CCA_OFFSET), \
	  (-105 + CCA_OFFSET) }, \
	{ (-97 + CCA_OFFSET), (-100 + CCA_OFFSET), (-103 + CCA_OFFSET),	\
	  (-105 + CCA_OFFSET) }, \
	{ (-94 + CCA_OFFSET), (-97 + CCA_OFFSET), (-100 + CCA_OFFSET), \
	  (-103 + CCA_OFFSET) }, \
	{ (-91 + CCA_OFFSET), (-94 + CCA_OFFSET), (-97 + CCA_OFFSET), \
	  (-100 + CCA_OFFSET) }, \
	{ (-88 + CCA_OFFSET), (-91 + CCA_OFFSET), (-94 + CCA_OFFSET), \
	  (-97 + CCA_OFFSET) },	\
	{ (-85 + CCA_OFFSET), (-88 + CCA_OFFSET), (-91 + CCA_OFFSET), \
	  (-94 + CCA_OFFSET) }

#define OFDM_CCA_THRES_TABLE_ROW_SIZE 7
#define OFDM_CCA_THRES_TABLE_COL_SIZE 4
#define OFDM_CCA_THRES_TABLE_DATA_TYPE  int8_t

/* OFDM processing delay for ACK timing */
#define OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE \
	{ 120, 120, 110, 150, 80, 160, 140 }, /* Opt. 1 */ \
	{  15, 100, 150, 100, 80, 140, 55 }, /* Opt. 2 */ \
	{   0, 35, 100, 140, 120, 100, 75 }, /* Opt. 3 */ \
	{ 130, 130, 130, 130, 140, 140, 100 } /* Opt. 4 */

#define OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE_ROW_SIZE 7
#define OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE_COL_SIZE 4
#define OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE_DATA_TYPE  uint8_t

#define CH_SPAC_FSK_MOD_1 200000 /* Hz */
#define CH_SPAC_FSK_MOD_2 400000 /* Hz */
#define CH_SPAC_FSK_MOD_3 400000 /* Hz */
#define CH_SPAC_FSK_MOD_4 600000 /* Hz */
#define CH_SPAC_FSK_TABLE   CH_SPAC_FSK_MOD_1, CH_SPAC_FSK_MOD_2, \
	CH_SPAC_FSK_MOD_3, CH_SPAC_FSK_MOD_4

#define CH_SPAC_OFDM_OPT_1 1200000 /* Hz */
#define CH_SPAC_OFDM_OPT_2 800000 /* Hz */
#define CH_SPAC_OFDM_OPT_3 400000 /* Hz */
#define CH_SPAC_OFDM_OPT_4 200000 /* Hz */
#define CH_SPAC_OFDM_TABLE  CH_SPAC_OFDM_OPT_1, CH_SPAC_OFDM_OPT_2, \
	CH_SPAC_OFDM_OPT_3, CH_SPAC_OFDM_OPT_4

#define OQPSK_CH_SPAC_TABLE \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 400000 }, \
	{ CHINA_780, 2000000 },	\
	{ EU_863, 0 }, \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 2000000 }, \
	{ KOREA_917, 2000000 },	\
	{ JAPAN_920, 200000 }, \
        /* US_928 */ \
        /* { JAPAN_950, 951000000, 951100000, 951200000, 951200000 }, */ \
        /* US_1427 */ \
	{ WORLD_2450, 5000000 }

#define OQPSK_CH_SPAC_TABLE_ROW_SIZE 7
#define OQPSK_CH_SPAC_TABLE_COL_SIZE 2
#define OQPSK_CH_SPAC_TABLE_DATA_TYPE  uint32_t

/* FSK symbol rate divided by 10 */
#define FSK_SYM_RATE_TABLE \
	{ \
		5, /* FSK_SYM_RATE_50 */ \
		10, /* FSK_SYM_RATE_100 */ \
		15, /* FSK_SYM_RATE_150 */ \
		20, /* FSK_SYM_RATE_200 */ \
		30, /* FSK_SYM_RATE_300 */ \
		40 /* FSK_SYM_RATE_400 */ \
	}
#define FSK_SYM_RATE_TABLE_SIZE        6
#define FSK_SYM_RATE_TABLE_DATA_TYPE   uint8_t

/* Rate dependent ACK processing delay */
#define FSK_PROCESSING_DELAY_ACK_TIMING	\
	{ \
		160, /* FSK_DATA_RATE_50 */ \
		90, /* FSK_DATA_RATE_100 */ \
		60, /* FSK_DATA_RATE_150 */ \
		70, /* FSK_DATA_RATE_200 */ \
		50, /* FSK_DATA_RATE_300 */ \
		40 /* FSK_DATA_RATE_400 */ \
	}

/* Rate dependent ACK processing delay for auto modes */
#define FSK_PROCESSING_DELAY_ACK_TIMING_AUTO_MODES \
	{ \
		128, /* FSK_DATA_RATE_50 */ \
		70, /* FSK_DATA_RATE_100 */ \
		60, /* FSK_DATA_RATE_150 */ \
		60, /* FSK_DATA_RATE_200 */ \
		30, /* FSK_DATA_RATE_300 */ \
		25 /* FSK_DATA_RATE_400 */ \
	}

#define FSK_PROCESSING_DELAY_ACK_TABLE_ROW_SIZE  6
#define FSK_PROCESSING_DELAY_ACK_TABLE_DATA_TYPE  uint8_t

#define OFDM_CH_CENTER_FREQ0_MAP \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 0, 0, 0, 470020000 }, \
	{ CHINA_780, 780200000, 779800000, 779400000, 779200000 }, \
	{ EU_863, 863625000, 863425000, 863225000, 863125000 },	\
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 903200000, 902800000, 902400000, 902200000 },	\
	{ KOREA_917, 917900000, 917500000, 917300000, 917100000 }, \
	{ JAPAN_920, 921200000, 920800000, 920400000, 920200000 }, \
        /* US_928 */ \
        /* { JAPAN_950, 0, 951300000, 951100000, 951000000 }, */ \
        /* US_1427 */ \
	{ WORLD_2450, 2401200000UL, 2400800000UL, 2400400000UL, 2400200000UL }

#define OFDM_CH_CENTER_FREQ0_MAP_ROW_SIZE    7
#define OFDM_CH_CENTER_FREQ0_MAP_COL_SIZE    5

#define OFDM_DATA_RATE_TABLE \
        /* see table 148, pg. 80 */ \
        /* OFDM option 1, OFDM option 2, OFDM option 3, OFDM option 4 */ \
        /* row contains value for a MCS value */ \
        /* MCS0: */ {100, 50, 25, 12}, \
        /* MCS1: */ {200, 100, 50, 25},	\
        /* MCS2: */ {400, 200, 100, 50}, \
        /* MCS3: */ {800, 400, 200, 100}, \
        /* MCS4: */ {1200, 600, 300, 150}, \
        /* MCS5: */ {1600, 800, 400, 200}, \
        /* MCS6: */ {2400, 1200, 600, 300}

#define OFDM_DATA_RATE_TABLE_ROW_SIZE 7
#define OFDM_DATA_RATE_TABLE_COL_SIZE 4
#define OFDM_DATA_RATE_TABLE_DATA_TYPE  uint16_t

#define OFDM_TOTAL_CHANNELS_MAP	\
        /* frequency band, op1 ,opt2 ,opt3 ,opt4total channels */ \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 0, 0, 0, 199 }, \
	{ CHINA_780, 6, 9, 19, 39 }, \
	{ EU_863, 5, 8, 17, 34 }, \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 20, 31, 64, 129 }, \
	{ KOREA_917, 5, 8, 16, 32 }, \
	{ JAPAN_920, 6, 9, 19, 39 }, \
        /* US_928 */ \
        /* { JAPAN_950, 951100000, 400000 }, */	\
        /* US_1427 */ \
	{ WORLD_2450, 64, 97, 207, 416 }

#define OFDM_TOTAL_CHANNELS_MAP_ROW_SIZE    7
#define OFDM_TOTAL_CHANNELS_MAP_COL_SIZE    5

#define OQPSK_TOTAL_CHANNELS_MAP \
        /* frequency band, total channels */ \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 99 }, \
	{ CHINA_780, 4 }, \
	{ EU_863, 3 }, \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 12 },	\
	{ KOREA_917, 3 }, \
	{ JAPAN_920, 38 }, \
        /* US_928 */ \
        /* { JAPAN_950, 951100000, 400000 }, */	\
        /* US_1427 */ \
	{ WORLD_2450, 16 }

#define OQPSK_TOTAL_CHANNELS_MAP_ROW_SIZE    7
#define OQPSK_TOTAL_CHANNELS_MAP_COL_SIZE    2

#define OQPSK_CH_CENTER_FREQ0_MAP \
        /* frequency band, channel center freq0, channel spacing */ \
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 470040000, 400000 }, \
	{ CHINA_780, 780000000, 2000000 }, \
	{ EU_863, 868300000, 0 }, \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 904000000, 2000000 },	\
	{ KOREA_917, 918100000, 2000000 }, \
	{ JAPAN_920, 920600000, 200000 }, \
        /* US_928 */ \
        /* { JAPAN_950, 951100000, 400000 }, */	\
        /* US_1427 */ \
	{ WORLD_2450, 2405000000UL, 5000000 }

#define OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE    7
#define OQPSK_CH_CENTER_FREQ0_MAP_COL_SIZE    3

#define OQPSK_EU_863_FREQ_TABLE   868300000, 868950000, 869525000
#define OQPSK_EU_863_FREQ_TABLE_SIZE  3
#define OQPSK_EU_863_FREQ_TABLE_DATA_TYPE  uint32_t

#define OQPSK_SHR_DURATION_TABLE \
        /* SHR duration depend of chip rate, shrDuration table 148, pg. 119 */ \
	{ 48, 48, 72, 72 }

#define OQPSK_SHR_DURATION_TABLE_SIZE  4
#define OQPSK_SHR_DURATION_TABLE_DATA_TYPE  uint8_t

#define LEG_OQPSK_CH_CENTER_FREQ0_MAP \
        /* frequency band, channel center freq0, channel spacing */ \
        /* EU_169 */ \
        /* US_450 */ \
        /* CHINA_470 */	\
	{ CHINA_780, 780000000, 2000000 }, \
        /*EU_863*/ \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 906000000, 2000000 },	\
        /*KOREA_917*/ \
        /* JAPAN_920*/ \
        /* US_928 */ \
        /*  JAPAN_950 */ \
        /* US_1427 */ \
	{ WORLD_2450, 2405000000UL, 5000000 }

#define LEG_OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE    3
#define LEG_OQPSK_CH_CENTER_FREQ0_MAP_COL_SIZE    3

#define FSK_CH_CENTER_FREQ0_MAP	\
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 470020000, 470040000, 470020000, 0 }, \
	{ CHINA_780, 779200000, 779400000, 779400000, 0 }, \
	{ EU_863, 863125000, 863225000, 863225000, 0 },	\
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 902200000, 902400000, 902400000, 0 },	\
	{ KOREA_917, 917100000, 917300000, 917300000, 0 }, \
	{ JAPAN_920, 920600000, 920900000, 920800000, 920800000 }, \
        /* US_928 */ \
        /* { JAPAN_950, 0, 951300000, 951100000, 951000000 }, */ \
        /* US_1427 */ \
	{ WORLD_2450, 2400200000UL, 2400400000UL, 2400400000UL, 0 }

#define FSK_CH_CENTER_FREQ0_MAP_ROW_SIZE    7
#define FSK_CH_CENTER_FREQ0_MAP_COL_SIZE    5

#define FSK_TOTAL_CHANNELS_MAP \
        /* frequency band, opmode1 ,opmode2 ,opmode3 ,opmode4 total channels */	\
        /* EU_169 */ \
        /* US_450 */ \
	{ CHINA_470, 199, 99, 99, 0 }, \
	{ CHINA_780, 39, 19, 19, 0 }, \
	{ EU_863, 34, 17, 17, 0 }, \
        /* US_896 */ \
        /* US_901 */ \
	{ US_915, 129, 64, 64, 0 }, \
	{ KOREA_917, 32, 16, 16, 0 }, \
	{ JAPAN_920, 38, 18, 12, 12 }, \
        /* US_928 */ \
        /* { JAPAN_950, 951100000, 400000 }, */	\
        /* US_1427 */ \
	{ WORLD_2450, 416, 207, 207, 0 }

#define FSK_TOTAL_CHANNELS_MAP_ROW_SIZE    7
#define FSK_TOTAL_CHANNELS_MAP_COL_SIZE    5

#define FPGA_CCA_THRES_VALUE    -105

/* === GLOBALS ============================================================= */

#if (defined SUPPORT_OQPSK) || (defined SUPPORT_LEGACY_OQPSK)
FLASH_DECLARE(OQPSK_SYMBOL_DURATION_TABLE_DATA_TYPE
		oqpsk_sym_duration_table[OQPSK_SYMBOL_DURATION_TABLE_SIZE])
	= OQPSK_SYMBOL_DURATION_TABLE;

FLASH_DECLARE(OQPSK_CCA_DURATION_TABLE_DATA_TYPE
		oqpsk_cca_dur_table[OQPSK_CCA_DURATION_TABLE_SIZE])
	= OQPSK_CCA_DURATION_TABLE;

FLASH_DECLARE(OQPSK_SHR_DURATION_TABLE_DATA_TYPE
		oqpsk_shr_duration_table[OQPSK_SHR_DURATION_TABLE_SIZE])
	= OQPSK_SHR_DURATION_TABLE;

FLASH_DECLARE(OQPSK_SYMBOL_LENGTH_TABLE_DATA_TYPE
		oqpsk_sym_len_table[OQPSK_SYMBOL_LENGTH_TABLE_SIZE])
	= OQPSK_SYMBOL_LENGTH_TABLE;

FLASH_DECLARE(OQPSK_CHIP_RATE_TABLE_DATA_TYPE
		oqpsk_chip_rate_table[OQPSK_CHIP_RATE_TABLE_SIZE])
	= OQPSK_CHIP_RATE_TABLE;

FLASH_DECLARE(OQPSK_ACK_TIMING_OFFSET_TABLE_DATA_TYPE
		oqpsk_ack_timing_offset_table[OQPSK_ACK_TIMING_OFFSET_TABLE_SIZE])
	= OQPSK_ACK_TIMING_OFFSET_TABLE;

FLASH_DECLARE(uint32_t oqpsk_freq0_map[OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE][
			OQPSK_CH_CENTER_FREQ0_MAP_COL_SIZE])
	= { OQPSK_CH_CENTER_FREQ0_MAP };

FLASH_DECLARE(uint32_t leg_oqpsk_freq0_map[
			LEG_OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE][
			LEG_OQPSK_CH_CENTER_FREQ0_MAP_COL_SIZE])
	= { LEG_OQPSK_CH_CENTER_FREQ0_MAP };

FLASH_DECLARE(OQPSK_CH_SPAC_TABLE_DATA_TYPE oqpsk_ch_spac_table[
			OQPSK_CH_SPAC_TABLE_ROW_SIZE][
			OQPSK_CH_SPAC_TABLE_COL_SIZE])
	= { OQPSK_CH_SPAC_TABLE };

FLASH_DECLARE(uint32_t oqpsk_max_ch_map[OQPSK_TOTAL_CHANNELS_MAP_ROW_SIZE][
			OQPSK_TOTAL_CHANNELS_MAP_COL_SIZE])
	= { OQPSK_TOTAL_CHANNELS_MAP };

FLASH_DECLARE(OQPSK_CHIP_RATE_FREQ_TABLE_DATA_TYPE oqpsk_chip_rate_freq_table[
			OQPSK_CHIP_RATE_FREQ_TABLE_ROW_SIZE][
			OQPSK_CHIP_RATE_FREQ_TABLE_COL_SIZE])
	= {OQPSK_CHIP_RATE_FREQ_TABLE};
#endif

#ifdef SUPPORT_OFDM
FLASH_DECLARE(uint32_t ofdm_freq0_map[OFDM_CH_CENTER_FREQ0_MAP_ROW_SIZE][
			OFDM_CH_CENTER_FREQ0_MAP_COL_SIZE])
	= { OFDM_CH_CENTER_FREQ0_MAP };

FLASH_DECLARE(uint32_t ofdm_max_ch_map[OFDM_TOTAL_CHANNELS_MAP_ROW_SIZE][
			OFDM_TOTAL_CHANNELS_MAP_COL_SIZE])
	= { OFDM_TOTAL_CHANNELS_MAP };

FLASH_DECLARE(uint32_t ofdm_ch_spacing_table[]) = { CH_SPAC_OFDM_TABLE };

FLASH_DECLARE(OFDM_CCA_THRES_TABLE_DATA_TYPE
		ofdm_cca_thres[OFDM_CCA_THRES_TABLE_ROW_SIZE][
			OFDM_CCA_THRES_TABLE_COL_SIZE]) =
{OFDM_CCA_THRES_TABLE};

FLASH_DECLARE(OFDM_DATA_RATE_TABLE_DATA_TYPE
		ofdm_data_rate_table[OFDM_DATA_RATE_TABLE_ROW_SIZE][
			OFDM_DATA_RATE_TABLE_COL_SIZE]) =
{OFDM_DATA_RATE_TABLE};

FLASH_DECLARE(OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE_DATA_TYPE
		ofdm_processing_delay_ack_timing[
			OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE_COL_SIZE][
			OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE_ROW_SIZE]) =
{OFDM_PROCESSING_DELAY_ACK_TIMING_TABLE};
#endif

#ifdef SUPPORT_FSK
FLASH_DECLARE(uint32_t fsk_freq0_map[FSK_CH_CENTER_FREQ0_MAP_ROW_SIZE][
			FSK_CH_CENTER_FREQ0_MAP_COL_SIZE])
	= { FSK_CH_CENTER_FREQ0_MAP };

FLASH_DECLARE(uint32_t fsk_max_ch_map[FSK_TOTAL_CHANNELS_MAP_ROW_SIZE][
			FSK_TOTAL_CHANNELS_MAP_COL_SIZE])
	= { FSK_TOTAL_CHANNELS_MAP };

FLASH_DECLARE(uint32_t fsk_ch_spacing_table[]) = { CH_SPAC_FSK_TABLE };

FLASH_DECLARE(FSK_SYM_RATE_TABLE_DATA_TYPE fsk_sym_rate_table[
			FSK_SYM_RATE_TABLE_SIZE])
	= FSK_SYM_RATE_TABLE;
FLASH_DECLARE(FSK_CCA_THRES_DATA_TYPE fsk_cca_thres_table[
			FSK_CCA_THRES_TABLE_SIZE])
	= FSK_CCA_THRES_TABLE;

#ifndef BASIC_MODE
FLASH_DECLARE(FSK_PROCESSING_DELAY_ACK_TABLE_DATA_TYPE
		fsk_processing_delay_ack_timing[
			FSK_PROCESSING_DELAY_ACK_TABLE_ROW_SIZE])
	= FSK_PROCESSING_DELAY_ACK_TIMING_AUTO_MODES;
#else
FLASH_DECLARE(FSK_PROCESSING_DELAY_ACK_TABLE_DATA_TYPE
		fsk_processing_delay_ack_timing[
			FSK_PROCESSING_DELAY_ACK_TABLE_ROW_SIZE])
	= FSK_PROCESSING_DELAY_ACK_TIMING;
#endif
#endif

/* === PROTOTYPES ========================================================== */

#ifdef SUPPORT_OQPSK
static uint16_t oqpsk_ack_psdu_duration_sym(trx_id_t trx_id);
static uint8_t oqpsk_spreading(oqpsk_chip_rate_t chip_rate,
		oqpsk_rate_mode_t rate_mode);
static uint16_t oqpsk_get_chip_rate(trx_id_t trx_id);

#endif

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Gets the symbol duration in us
 *
 * @param trx_id Transceiver identifier
 *
 * @return Duration of a symbol in us
 */
uint16_t tal_get_symbol_duration_us(trx_id_t trx_id)
{
	uint16_t ret_val = 0; /* 0: indicator for wrong parameter */

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		ret_val = 20; /* table 0, pg. 7 */
		break;

#endif
#ifdef SUPPORT_OFDM
	case OFDM:
		ret_val = 120;
		break;

#endif
#ifdef SUPPORT_OQPSK
	case OQPSK: /* table 183, pg. 118 */
		ret_val
			= (uint16_t)PGM_READ_WORD(&oqpsk_sym_duration_table[
					tal_pib
					[trx_id].phy.phy_mode.oqpsk.chip_rate]);
		break;

#endif
#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		ret_val = 16;
		break;

#endif
	default:
		break;
	}

	return ret_val;
}

/**
 * @brief Gets the CCA duration in us
 *
 * @param trx_id Transceiver identifier
 *
 * @return CCA duration in us
 */
uint16_t get_cca_duration_us(trx_id_t trx_id)
{
	uint16_t ret_val = 0;

	if (tal_pib[trx_id].phy.freq_band == JAPAN_920) {
		ret_val = calculate_cca_duration_us(trx_id);
	} else {
		switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
		case FSK:
			ret_val = 8;
			break;

#endif
#ifdef SUPPORT_OFDM
		case OFDM:
			ret_val = 8; /* symbols */
			break;

#endif
#ifdef SUPPORT_OQPSK
		case OQPSK:
			/* Check CCA table for entry */
			ret_val
				= (uint8_t)PGM_READ_BYTE(&oqpsk_cca_dur_table[
						tal_pib
						[trx_id].phy.phy_mode.oqpsk.
						chip_rate]);                                                          /*
			                                                                                               *symbols
			                                                                                               **/
			break;

#endif
#ifdef SUPPORT_LEGACY_OQPSK
		case LEG_OQPSK:
			ret_val = 8;
			break;

#endif
		default:
			break;
		}

		/* Change value from symbols to us */
		ret_val = ret_val * tal_pib[trx_id].SymbolDuration_us;
	}

	return ret_val;
}

/**
 * @brief Gets the CCA threshold
 *
 * @param trx_id Transceiver identifier
 *
 * @return CCA threshold in dBm
 */
int8_t get_cca_thres(trx_id_t trx_id)
{
	int8_t thres = 0; /* 0: indicator for wrong parameter */

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		thres
			= (int8_t)PGM_READ_BYTE(&fsk_cca_thres_table[tal_pib[
					trx_id
				].phy.phy_mode.fsk.sym_rate]);
		if (tal_pib[trx_id].FSKFECEnabled) {
			thres -= FK_CCA_THRES_FEC_OFFSET;
		}
		break;

#endif
#ifdef SUPPORT_OFDM
	case OFDM:
		/* rows: MCSn; column: option n */
		thres
			= (int8_t)PGM_READ_BYTE(&ofdm_cca_thres[tal_pib[trx_id].
				OFDMMCS] \
				[tal_pib[trx_id].phy.phy_mode.ofdm.option ]);
		break;

#endif
#ifdef SUPPORT_OQPSK
	case OQPSK:
		thres = (int8_t)OQPSK_CCA_THRES;
		break;

#endif
#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		thres = -85 + 10;
		break;
#endif

	default:
		break;
	}

	return thres;
}

/**
 * @brief Gets the AckWaitDuration in us
 *
 * @param trx_id Transceiver identifier
 *
 * @return AckWaitDuration in us
 */
/* see section 6.4.3, pg. 30 */
uint16_t get_AckWaitDuration_us(trx_id_t trx_id)
{
	uint16_t AckWaitDuration;

#ifdef SUPPORT_LEGACY_OQPSK
	if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
		AckWaitDuration = 54; /* symbols */
	} else
#endif
	{
#if ((defined SUPPORT_OFDM) || (defined SUPPORT_FSK))
		uint8_t ack_len = 3 + tal_pib[trx_id].FCSLen;
#endif
		/* aUnitBackoffPeriod + aTurnaroundTime */
		AckWaitDuration
			= (2 *
				ceiling_sym(trx_id, aMinTurnaroundTimeSUNPHY)) +
				tal_pib[trx_id].CCADuration_sym;
		/* phySHRDuration */
		AckWaitDuration += shr_duration_sym(trx_id);

		switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
		case FSK:
			/* PHR uses same data rate as PSDU */
			ack_len += 2;
			if (tal_pib[trx_id].phy.phy_mode.fsk.mod_type ==
					F4FSK) {
				ack_len /= 2;
			}

			if (tal_pib[trx_id].FSKFECEnabled) {
				ack_len *= 2;
			}

			AckWaitDuration += ack_len * 8;
			break;

#endif
#ifdef SUPPORT_OFDM
		case OFDM:
			/* phyPHRDuration */
			AckWaitDuration += phr_duration_sym(trx_id);
			/* PSDU len = 3 + FCS = 5 or 7; add TAIL and PAD; */
			AckWaitDuration += ceiling_sym(trx_id,
					ack_len *
					tal_pib[trx_id].OctetDuration_us);
			break;

#endif
#ifdef SUPPORT_OQPSK
		case OQPSK:
			/* phyPHRDuration */
			AckWaitDuration += phr_duration_sym(trx_id);
			/* PSDU duration */
			AckWaitDuration += oqpsk_ack_psdu_duration_sym(trx_id);
			break;

#endif
		default:
			AckWaitDuration = 0;
			break;
		}
	}

	/* Convert from symbols to us */
	AckWaitDuration *= tal_pib[trx_id].SymbolDuration_us;

	return AckWaitDuration;
}

/**
 * @brief Gets the ACK timing in us
 *
 * @param trx_id Transceiver identifier
 *
 * @return ACK timing in us
 */
uint16_t get_AckTiming_us(trx_id_t trx_id)
{
	uint16_t ack;

#ifdef SUPPORT_LEGACY_OQPSK
	if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
		ack = aTurnaroundTime; /* = 12 */
	} else
#endif
	{
		ack = ceiling_sym(trx_id, aMinTurnaroundTimeSUNPHY);
	}

	/* Convert symbols to usec */
	ack *= tal_pib[trx_id].SymbolDuration_us;

	/*
	 * ACK timing correction - processing delay:
	 * RX to RXFE IRQ delay - PHY dependent
	 * Txprep to Tx delay
	 * MCU processing delay - MCU dependent
	 * PA ramping delay
	 */
	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		ack
			-= (uint8_t)PGM_READ_BYTE(&fsk_processing_delay_ack_timing[
					tal_pib
					[trx_id].phy.phy_mode.fsk.sym_rate]);
		break;

#endif
#ifdef SUPPORT_OFDM
	case OFDM:
		ack -= 100;
		break;

#endif
#if (defined SUPPORT_OQPSK) && (!defined BASIC_MODE)
	case OQPSK:
		ack
			-= (uint8_t)PGM_READ_BYTE(&oqpsk_ack_timing_offset_table[
					tal_pib
					[trx_id].phy.phy_mode.oqpsk.chip_rate]);
		break;

#endif
#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
#ifndef BASIC_MODE
		ack -= 20;
#else
		ack -= 16;
#endif
		break;

#endif /* #if (defined SUPPORT_LEGACY_OQPSK) */
	default:
		break;
	}

	return ack;
}

/**
 * @brief Implements the ceiling function for symbols
 *
 * Returns the smallest integer value in symbols greater or equal to its
 *argument value
 * see section 9.2, pg. 43
 *
 * @param trx_id Transceiver identifier
 * @param duration_us Symbol duration in us
 *
 * @return Number of symbols
 */
uint16_t ceiling_sym(trx_id_t trx_id, uint16_t duration_us)
{
	uint16_t sym;
	sym = duration_us / tal_pib[trx_id].SymbolDuration_us;
	if ((duration_us % tal_pib[trx_id].SymbolDuration_us) > 0) {
		sym++;
	}

	return sym;
}

/**
 * @brief Gets the PHR duration
 *
 * see pg. 46
 *
 * @param trx_id Transceiver identifier
 *
 * @return PHR duration in symbols
 */
uint8_t phr_duration_sym(trx_id_t trx_id)
{
	uint8_t phr;

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		phr = 2 * 8;
		break;

#endif
#ifdef SUPPORT_OFDM
	case OFDM:
		if (tal_pib[trx_id].OFDMInterleaving) {
			switch (tal_pib[trx_id].phy.phy_mode.ofdm.option) {
			case OFDM_OPT_1:
				phr = 4;
				break;

			case OFDM_OPT_2:
				phr = 8;
				break;

			default: /*  OFDM_OPT_3 and OFDM_OPT_4 */
				phr = 6;
				break;
			}
		} else { /* interleaving == 0 */
			if (tal_pib[trx_id].phy.phy_mode.ofdm.option ==
					OFDM_OPT_1) {
				phr = 3;
			} else {
				phr = 6;
			}
		}
		break;

#endif
#ifdef SUPPORT_OQPSK
	case OQPSK:
		phr = 15;
		break;

#endif
#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		phr = 2;
		break;

#endif
	default:
		phr = 0;
		break;
	}

	return phr;
}

/**
 * @brief Gets the SHR duration
 *
 * @param trx_id Transceiver identifier
 *
 * @return SHR duration in symbols
 */
uint8_t shr_duration_sym(trx_id_t trx_id)
{
	uint8_t shr = 0;

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		/* Preamble + SFD; SFD=2 */
		shr = (tal_pib[trx_id].FSKPreambleLength + 2) * 8;
		break;
#endif

#ifdef SUPPORT_OFDM
	case OFDM:
		shr = 6;
		break;
#endif

#ifdef SUPPORT_OQPSK
	case OQPSK: /* see pg. 119 */
		shr
			= (uint8_t)PGM_READ_BYTE(&oqpsk_shr_duration_table[
					tal_pib[trx_id
					].phy.phy_mode.oqpsk.chip_rate]);
		break;
#endif

#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		shr = 8 + 2; /* 8 = SHR; 2 = SFD */
		break;

#endif
	default:
		break;
	}

	return shr;
}

/**
 * @brief Gets the PSDU duration for MR-OQPSK
 *
 * pg. 119, section 18.3.2.14
 *
 * @param trx_id Transceiver identifier
 *
 * @return PSDU length in symbols
 */
#ifdef SUPPORT_OQPSK
static uint16_t oqpsk_ack_psdu_duration_sym(trx_id_t trx_id)
{
	uint8_t Ns
		= (uint8_t)PGM_READ_BYTE(&oqpsk_sym_len_table[tal_pib[trx_id].
			phy.phy_mode.oqpsk.chip_rate]);                                                      /*
	                                                                                                      * Table
	                                                                                                      * 183 */
	uint8_t Rspread = oqpsk_spreading(
			tal_pib[trx_id].phy.phy_mode.oqpsk.chip_rate,
			tal_pib[trx_id].OQPSKRateMode);
	uint16_t Npsdu = Rspread * 2 * 63; /* Nd == 63, since ACK length is 5 or
	                                    * 7 octects only */
	uint16_t duration_sym = Npsdu / Ns;
	if (Npsdu % Ns) {
		duration_sym += 1;
	}

	uint16_t temp = 16 * Ns;
	duration_sym += Npsdu / temp;
	if ((Npsdu % temp) > 0) {
		duration_sym += 1;
	}

	return duration_sym;
}

#endif

/**
 * @brief Gets the MR-OQPSK chip rate
 *
 * table 166, pg. 101
 *
 * @param trx_id Transceiver identifier
 *
 * @return Chip rate
 */
#ifdef SUPPORT_OQPSK
uint16_t oqpsk_get_chip_rate(trx_id_t trx_id)
{
	uint16_t rate = 10 *
			(uint16_t)PGM_READ_BYTE(&oqpsk_chip_rate_table[tal_pib[
				trx_id].phy.phy_mode.oqpsk.chip_rate]);

	return rate;
}

#endif

/**
 * @brief Gets the MR-OQPSK spreading
 *
 * @param chip_rate Chip rate
 * @param rate_mode Rate mode
 *
 * @return Spreading
 */
#ifdef SUPPORT_OQPSK
static uint8_t oqpsk_spreading(oqpsk_chip_rate_t chip_rate,
		oqpsk_rate_mode_t rate_mode)
{
	uint8_t spread;

	if (rate_mode == OQPSK_RATE_MOD_4) {
		spread = 1;
	} else {
		spread = 1 << (3 - rate_mode);
		switch (chip_rate) {
		case CHIP_RATE_100: /* no correction */
			break;

		case CHIP_RATE_200: /* no correction */
			break;

		case CHIP_RATE_1000:
			spread *= 2;
			break;

		case CHIP_RATE_2000:
			spread *= 4;
			break;
		}
	}

	return spread;
}

#endif

/**
 * @brief Gets PSDU data rate
 *
 * @param trx_id Transceiver identifier
 *
 * @return data rate in kbit/s
 */
float get_data_rate(trx_id_t trx_id)
{
	float rate = 0;

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		rate = 10 *
				(uint8_t)PGM_READ_BYTE(&fsk_sym_rate_table[
					tal_pib[trx_id
					].phy.phy_mode.fsk.sym_rate]);
		if (tal_pib[trx_id].phy.phy_mode.fsk.mod_type == F4FSK) {
			rate *= 2;
		}

		if (tal_pib[trx_id].FSKFECEnabled) {
			rate /= 2;
		}
		break;

#endif
#ifdef SUPPORT_OFDM
	case OFDM:
		rate
			= (uint16_t)PGM_READ_WORD(&ofdm_data_rate_table[tal_pib[
					trx_id
				].OFDMMCS] \
				[tal_pib[trx_id].phy.phy_mode.ofdm.option]);
		break;

#endif
#ifdef SUPPORT_OQPSK
	case OQPSK:
	{
		uint16_t chip_rate = oqpsk_get_chip_rate(trx_id);
		uint8_t spread = oqpsk_spreading(
				tal_pib[trx_id].phy.phy_mode.oqpsk.chip_rate,
				tal_pib[trx_id].OQPSKRateMode);
		rate = (float)chip_rate / (float)spread / (float)2;
	}
	break;

#endif
#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		rate = 250;
		if (tal_pib[trx_id].HighRateEnabled) {
			if (tal_pib[trx_id].phy.phy_mode.leg_oqpsk.chip_rate ==
					CHIP_RATE_1000) {
				rate *= 2;
			} else {
				rate *= 4;
			}
		}
		break;
#endif

	default:
		break;
	}

	return rate;
}

/**
 * @brief Gets CCA duration
 *
 * @param trx_id Transceiver identifier
 *
 * @return CCA duration in us
 */
uint16_t calculate_cca_duration_us(trx_id_t trx_id)
{
	uint16_t ret;

	if (tal_pib[trx_id].CCATimeMethod == 0) {
		ret = tal_pib[trx_id].SymbolDuration_us *
				tal_pib[trx_id].CCADuration_sym;
	} else { /* == 1 */
		ret = 2 << tal_pib[trx_id].CCADuration_us;
	}

	return ret;
}

/**
 * @brief Gets ACK duration
 *
 * @param trx_id Transceiver identifier
 *
 * @return ACK duration in symbols
 */
#ifdef MEASURE_ON_AIR_DURATION
uint16_t get_ack_duration_sym(trx_id_t trx_id)
{
#if (defined SUPPORT_FSK) || (defined SUPPORT_OFDM)
	/* PSDU duration, len = 3 + FCS = 5 or 7 */
	uint8_t ack_len = 3 + tal_pib[trx_id].FCSLen;
#endif
	uint16_t ack_sym;

#ifdef SUPPORT_LEGACY_OQPSK
	if (tal_pib[trx_id].phy.modulation == LEG_OQPSK) {
		ack_sym = 22;
	} else
#endif
	{
		/* phySHRDuration; see pg. 49 */
		ack_sym = shr_duration_sym(trx_id);
		/* phyPHRDuration, see pg. 46 */
		ack_sym += phr_duration_sym(trx_id);

		switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
		case FSK:
		{
			if (tal_pib[trx_id].phy.phy_mode.fsk.mod_type ==
					F4FSK) {
				ack_len /= 2;
			}

			ack_sym += ack_len * 8;
		}
		break;

#endif
#ifdef SUPPORT_OFDM
		case OFDM:
			/* PSDU len = 3 + FCS = 5 or 7; add TAIL and PAD; */
			ack_sym += ceiling_sym(trx_id,
					ack_len *
					tal_pib[trx_id].OctetDuration_us);
			break;

#endif
#ifdef SUPPORT_OQPSK
		case OQPSK:
			/* PSDU duration */
			ack_sym += oqpsk_ack_psdu_duration_sym(trx_id);
			break;

#endif
		default:
			break;
		}
	}

	return ack_sym;
}

#endif

/**
 * @brief Gets the supported channels
 *
 * The lowest 16bits contain the lowest supported channel number
 * and the highest 16bits contains the highest supported channel number.
 *
 * @param[in]   trx_id Transceiver identifier
 * @param[out]  value Pointer to tuple (32bits) storage location
 *
 * @return MAC_SUCCESS, MAC_UNSUPPORTED_ATTRIBUTE, MAC_INVALID_PARAMETER
 */
retval_t get_supported_channels_tuple(trx_id_t trx_id, uint32_t *value)
{
	retval_t status = MAC_SUCCESS;

	switch (tal_pib[trx_id].phy.modulation) {
#ifdef SUPPORT_FSK
	case FSK:
		/* @ToDo */
		status = MAC_UNSUPPORTED_ATTRIBUTE;
		break;
#endif

#ifdef SUPPORT_OFDM
	case OQPSK:
		/* @ToDo */
		status = MAC_UNSUPPORTED_ATTRIBUTE;
		break;
#endif

#ifdef SUPPORT_OQPSK
	case OFDM:
		/* @ToDo */
		status = MAC_UNSUPPORTED_ATTRIBUTE;
		break;
#endif

#ifdef SUPPORT_LEGACY_OQPSK
	case LEG_OQPSK:
		if (trx_id == RF09) {
			switch (tal_pib[trx_id].phy.freq_band) {
			case CHINA_470:
			case CHINA_780:
				/* *value = (uint32_t)(0 | ((uint32_t)7 << 16));
				 **/
				*(uint32_t *)value = 0x0000000F;
				break;

			case US_915:

				/* *value = (uint32_t)(1 | ((uint32_t)10 <<
				 * 16)); */
				*(uint32_t *)value = 0x000007FE;
				break;

			default:
				status = MAC_INVALID_PARAMETER;
				break;
			}
		} else { /* RF24 */
			 /* *value = (uint32_t)(11 | ((uint32_t)26 << 16)); */
			*(uint32_t *)value = 0x07FFF800;
		}
		break;
#endif

	default:
		status = MAC_INVALID_PARAMETER;
		break;
	}

#ifndef SUPPORT_LEGACY_OQPSK
	/* Keep compiler happy */
	value = value;
#endif

	return status;
}

/**
 * @brief Gets the OFDM center frequency F0
 *
 *
 * @param[in]   trx_id Transceiver identifier
 * @param[in]   freq_band Frequency band
 * @param[in]   option OFDM option
 * @param[out]  freq  Pointer to frequency F0
 * @param[out]  spacing Pointer to Channel spacing
 *
 * @return
 */
void get_ofdm_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		ofdm_option_t option, uint32_t *freq, uint32_t *spacing)
{
	for (uint8_t i = 0; i < OFDM_CH_CENTER_FREQ0_MAP_ROW_SIZE; i++) {
		if (freq_band ==
				(uint32_t)PGM_READ_DWORD(&ofdm_freq0_map[i][0]))
		{
			*freq
				= (uint32_t)PGM_READ_DWORD(&ofdm_freq0_map[i][
						option
						+ 1]);
			break;
		}
	}
	*spacing = (uint32_t)PGM_READ_DWORD(&ofdm_ch_spacing_table[option]);
}

/**
 * @brief Gets the Legacy OQPSK center frequency F0
 *
 *
 * @param[in]   trx_id Transceiver identifier
 * @param[in]   freq_band Frequency band
 * @param[out]  freq  Pointer to frequency F0
 * @param[out]  spacing Pointer to Channel spacing
 *
 * @return
 */
void get_leg_oqpsk_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		uint32_t *freq, uint32_t *spacing)
{
	for (uint8_t i = 0; i < LEG_OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE; i++) {
		if (freq_band ==
				(uint32_t)PGM_READ_DWORD(&leg_oqpsk_freq0_map[i]
				[0])) {
			*freq = (uint32_t)PGM_READ_DWORD(
					&leg_oqpsk_freq0_map[i][1]);

			*spacing = (uint32_t)PGM_READ_DWORD(
					&leg_oqpsk_freq0_map[i][2]);
			break;
		}
	}
}

/**
 * @brief Gets the OQPSK center frequency F0
 *
 *
 * @param[in]   trx_id Transceiver identifier
 * @param[in]   freq_band Frequency band
 * @param[out]  freq  Pointer to frequency F0
 * @param[out]  spacing Pointer to Channel spacing
 *
 * @return
 */
void get_oqpsk_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		uint32_t *freq, uint32_t *spacing)
{
	for (uint8_t i = 0; i < OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE; i++) {
		if (freq_band ==
				(uint32_t)PGM_READ_DWORD(&oqpsk_freq0_map[i][0]))
		{
			*freq
				= (uint32_t)PGM_READ_DWORD(
					&oqpsk_freq0_map[i][1]);
			if (freq_band == EU_863) {
				*spacing = 0; /* ? */
			} else {
				*spacing = (uint32_t)PGM_READ_DWORD(
						&oqpsk_ch_spac_table[i][1]);
			}

			break;
		}
	}
}

/**
 * @brief Gets the FSK center frequency F0
 *
 *
 * @param[in]   trx_id Transceiver identifier
 * @param[in]   freq_band Frequency band
 * @param[in]   option FSK operating mode
 * @param[out]  freq  Pointer to frequency F0
 * @param[out]  spacing Pointer to Channel spacing
 *
 * @return
 */
void get_fsk_freq_f0(trx_id_t trx_id, sun_freq_band_t freq_band,
		fsk_op_mode_t option, uint32_t *freq, uint32_t *spacing)
{
	for (uint8_t i = 0; i < FSK_CH_CENTER_FREQ0_MAP_ROW_SIZE; i++) {
		if (freq_band ==
				(uint32_t)PGM_READ_DWORD(&fsk_freq0_map[i][0]))
		{
			*freq
				= (uint32_t)PGM_READ_DWORD(
					&fsk_freq0_map[i][option]);
			break;
		}
	}
	*spacing = (uint32_t)PGM_READ_DWORD(&fsk_ch_spacing_table[option - 1]);
}

/**
 * @brief Gets the OQPSK Chiprate
 *
 *
 * @param[in]   trx_id Transceiver identifier
 * @param[in]   freq_band Frequency band
 *
 * @return     oqpsk chiprate
 */
oqpsk_chip_rate_t get_oqpsk_chip_rate(trx_id_t trx_id,
		sun_freq_band_t freq_band)
{
	uint16_t rate = 0;
	uint8_t rate_mode = 0;

	for (uint8_t i = 0; i < OQPSK_CHIP_RATE_FREQ_TABLE_ROW_SIZE; i++) {
		if (freq_band ==
				(uint16_t)PGM_READ_WORD(&
				oqpsk_chip_rate_freq_table
				[i][0])) {
			rate = (uint16_t)PGM_READ_WORD(
					&oqpsk_chip_rate_freq_table[i][1]);
			break;
		}
	}
	if (rate == 2000) {
		rate_mode = CHIP_RATE_2000;
	} else if (rate == 1000) {
		rate_mode = CHIP_RATE_1000;
	} else if (rate == 200) {
		rate_mode = CHIP_RATE_200;
	} else if (rate == 100) {
		rate_mode = CHIP_RATE_100;
	}

	return rate_mode;
}

/**
 * @brief Gets Maximum no of channel supported by each modulation
 *
 *
 * @param[in]   trx_id Transceiver identifier
 *
 * @return      No of channels supported
 */
uint16_t get_sun_max_ch_no(trx_id_t trx_id)
{
	uint16_t max_ch = 0;
	switch (tal_pib[trx_id].phy.modulation) {
	case OFDM:
		for (uint8_t i = 0; i < OFDM_CH_CENTER_FREQ0_MAP_ROW_SIZE;
				i++) {
			if (tal_pib[trx_id].phy.freq_band ==
					(uint32_t)PGM_READ_DWORD(&ofdm_freq0_map
					[i][0])) {
				max_ch = (uint32_t)PGM_READ_DWORD(
						&ofdm_max_ch_map[i][tal_pib[
							trx_id].phy.phy_mode.
						ofdm.option
						+ 1]);
				break;
			}
		}
		break;

	case OQPSK:
		for (uint8_t i = 0; i < OQPSK_CH_CENTER_FREQ0_MAP_ROW_SIZE;
				i++) {
			if (tal_pib[trx_id].phy.freq_band ==
					(uint32_t)PGM_READ_DWORD(&
					oqpsk_freq0_map[i][0])) {
				max_ch = (uint32_t)PGM_READ_DWORD(
						&oqpsk_max_ch_map[i][1]);
				break;
			}
		}
		break;

	case FSK:
		for (uint8_t i = 0; i < FSK_CH_CENTER_FREQ0_MAP_ROW_SIZE; i++) {
			if (tal_pib[trx_id].phy.freq_band ==
					(uint32_t)PGM_READ_DWORD(&fsk_freq0_map[
						i][0])) {
				max_ch = (uint32_t)PGM_READ_DWORD(
						&fsk_max_ch_map[i][tal_pib[
							trx_id].phy.phy_mode.fsk
						.op_mode]);
				break;
			}
		}
		break;

	default:
		break;
	}
	return max_ch;
}

/* EOF */
