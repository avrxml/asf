/**
 * @file tal_pib.h
 *
 * @brief This file contains the defines for initial TAL PIB attribute values.
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
#ifndef TAL_PIB_H
#define TAL_PIB_H

/* === INCLUDES ============================================================ */

#include "ieee_154g.h"

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/*
 * TAL PIB default values
 */

/**
 * Defines used to select the default configuration
 */
#define OFDM_DEF                                1
#define FSK_DEF                                 2
#define OQPSK_DEF                               3
#define LEG_OQPSK_DEF                           4

/**
 * Default value of maximum backoff exponent used while performing CSMA
 */
#define TAL_RF09_MAXBE_DEF                      (0x08)
#define TAL_RF24_MAXBE_DEF                      (0x08)

/**
 * Default value of PIB attribute macMaxFrameRetries
 */
#define TAL_RF09_MAXFRAMERETRIES_DEF            (0x03)
#define TAL_RF24_MAXFRAMERETRIES_DEF            (0x03)

/**
 * Default value of maximum CSMA backoffs
 */
#define TAL_RF09_MAX_CSMA_BACKOFFS_DEF          (0x04)
#define TAL_RF24_MAX_CSMA_BACKOFFS_DEF          (0x04)

/**
 * Default value of minimum backoff exponent used while performing CSMA
 */
#define TAL_RF09_MINBE_DEF                      (0x05)
#define TAL_RF24_MINBE_DEF                      (0x05)

/**
 * Default value of the maximum number of frames that are allowed to be
 * received during CSMA backoff periods for a tx transaction.
 */
#define TAL_RF09_MAX_FRAMES_DURING_BACKOFF_DEF  (0x05)
#define TAL_RF24_MAX_FRAMES_DURING_BACKOFF_DEF  (0x05)

/**
 * Value of a broadcast PAN ID
 */
#define TAL_PANID_BC_DEF                        (0xFFFF)

/**
 * Default value of short address
 */
#define TAL_SHORT_ADDRESS_DEF                   (0xFFFF)

#ifndef CUSTOM_DEFAULT_TX_PWR

/*
 * Default value of transmit power of transceiver: Use highest tx power
 */
#define TAL_TRANSMIT_POWER_DEFAULT              (0x04)
#endif

/**
 * Default value of PAN Coordinator custom TAL PIB
 */
#define TAL_RF09_PAN_COORDINATOR_DEF            (false)
#define TAL_RF24_PAN_COORDINATOR_DEF            (false)

/**
 * FCS type default value
 */
#define TAL_RF09_FCS_TYPE_DEFAULT               FCS_TYPE_4_OCTETS
#define TAL_RF24_FCS_TYPE_DEFAULT               FCS_TYPE_4_OCTETS

/**
 * Default value of PHY / modulation
 * The define DEFAULT_PHY can be set by the Makefile.
 */
#ifdef DEFAULT_RF09_PHY
#   if ((DEFAULT_RF09_PHY != OFDM_DEF) && (DEFAULT_RF09_PHY != FSK_DEF) && \
	(DEFAULT_RF09_PHY != OQPSK_DEF) && (DEFAULT_RF09_PHY != LEG_OQPSK_DEF))
#       error "Undefined PHY configuration"
#   else
#       define TAL_RF09_MODULATION_DEF          DEFAULT_RF09_PHY
#   endif
#else
#   ifdef SUPPORT_OFDM
#       define TAL_RF09_MODULATION_DEF          OFDM_DEF
#   elif (defined SUPPORT_FSK)
#       define TAL_RF09_MODULATION_DEF          FSK_DEF
#   elif (defined SUPPORT_OQPSK)
#       define TAL_RF09_MODULATION_DEF          OQPSK_DEF
#   elif (defined SUPPORT_LEGACY_OQPSK)
#       define TAL_RF09_MODULATION_DEF          LEG_OQPSK_DEF
#   else
#       error "Undefined PHY configuration"
#   endif
#endif

#ifdef DEFAULT_RF24_PHY
#   if ((DEFAULT_RF24_PHY != OFDM_DEF) && (DEFAULT_RF24_PHY != FSK_DEF) && \
	(DEFAULT_RF24_PHY != OQPSK_DEF) && (DEFAULT_RF24_PHY != LEG_OQPSK_DEF))
#       error "Undefined PHY configuration"
#   else
#       define TAL_RF24_MODULATION_DEF          DEFAULT_RF24_PHY
#   endif
#else
#   ifdef SUPPORT_OFDM
#       define TAL_RF24_MODULATION_DEF          OFDM_DEF
#   elif (defined SUPPORT_FSK)
#       define TAL_RF24_MODULATION_DEF          FSK_DEF
#   elif (defined SUPPORT_OQPSK)
#       define TAL_RF24_MODULATION_DEF          OQPSK_DEF
#   elif (defined SUPPORT_LEGACY_OQPSK)
#       define TAL_RF24_MODULATION_DEF          LEG_OQPSK_DEF
#   else
#       error "Undefined PHY configuration"
#   endif
#endif

/**
 * Default value of OFDM current channel in TAL
 */
#define TAL_RF09_OFDM_CURRENT_CHANNEL_DEF       (0x00)
#define TAL_RF24_OFDM_CURRENT_CHANNEL_DEF       (0x00)

/**
 * Default value of OFDM PHY / frequency band
 */
#define TAL_RF09_OFDM_FRQ_BAND_DEF              US_915
#define TAL_RF24_OFDM_FRQ_BAND_DEF              WORLD_2450

/**
 * Default value of OFDM default frequency F0
 */
#define TAL_RF09_OFDM_FRQ_F0_DEF                OFDM_915_OPT1_F0
#define TAL_RF24_OFDM_FRQ_F0_DEF                OFDM_2450_OPT1_F0

/**
 * Default value of OFDM default channel spacing
 */
#define TAL_RF09_OFDM_CH_SPACING_DEF            OFDM_915_OPT1_CH_SPAC
#define TAL_RF24_OFDM_CH_SPACING_DEF            OFDM_2450_OPT1_CH_SPAC

/**
 * Default value of PHY / OFDM option
 */
#define TAL_RF09_OFDM_OPT_DEF                   OFDM_OPT_1
#define TAL_RF24_OFDM_OPT_DEF                   OFDM_OPT_1

/**
 * Default value of PHY / OFMD MCS value
 */
#define TAL_RF09_OFDM_MCS_VAL_DEF               MCS3
#define TAL_RF24_OFDM_MCS_VAL_DEF               MCS3

/**
 * Default value of PHY / OFMD interleaving enable value
 */
#define TAL_RF09_OFDM_INTERL_DEF                false
#define TAL_RF24_OFDM_INTERL_DEF                false

/**
 * Default value of FSK current channel in TAL
 */
#define TAL_RF09_FSK_CURRENT_CHANNEL_DEF        (0x00)
#define TAL_RF24_FSK_CURRENT_CHANNEL_DEF        (0x00)

/**
 * Default value of FSK PHY / frequency band
 */
#define TAL_RF09_FSK_FRQ_BAND_DEF               US_915
#define TAL_RF24_FSK_FRQ_BAND_DEF               WORLD_2450

/**
 * Default value of FSK default frequency F0
 */
#define TAL_RF09_FSK_FRQ_F0_DEF                 FSK_915_MOD1_F0
#define TAL_RF24_FSK_FRQ_F0_DEF                 FSK_2450_MOD1_F0

/**
 * Default value of FSK default channel spacing
 */
#define TAL_RF09_FSK_CH_SPACING_DEF             FSK_915_MOD1_CH_SPAC
#define TAL_RF24_FSK_CH_SPACING_DEF             FSK_2450_MOD1_CH_SPAC

/**
 * Default value for FSK data rate
 */
#define TAL_RF09_FSK_DATA_RATE_DEF              FSK_DATA_RATE_50
#define TAL_RF24_FSK_DATA_RATE_DEF              FSK_DATA_RATE_50

/**
 * Default value for FSK modulation index
 */
#define TAL_RF09_FSK_MOD_IDX_DEF                MOD_IDX_1_0
#define TAL_RF24_FSK_MOD_IDX_DEF                MOD_IDX_1_0

/**
 * Default value for FSK modulation type
 */
#define TAL_RF09_FSK_TYPE_DEF                   F2FSK
#define TAL_RF24_FSK_TYPE_DEF                   F2FSK

/**
 * Default value for FSK operation mode
 */
#define TAL_RF09_FSK_OP_DEF                     FSK_OP_MOD_1
#define TAL_RF24_FSK_OP_DEF                     FSK_OP_MOD_1

/**
 * Default value of OQPSK current channel in TAL
 */
#define TAL_RF09_OQPSK_CURRENT_CHANNEL_DEF      (0x00)
#define TAL_RF24_OQPSK_CURRENT_CHANNEL_DEF      (0x00)

/**
 * Default value of OQPSK PHY / frequency band
 */
#define TAL_RF09_OQPSK_FRQ_BAND_DEF             US_915
#define TAL_RF24_OQPSK_FRQ_BAND_DEF             WORLD_2450

/**
 * Default value of OQPSK default frequency F0
 */
#define TAL_RF09_OQPSK_FRQ_F0_DEF               OQPSK_915_F0
#define TAL_RF24_OQPSK_FRQ_F0_DEF               OQPSK_2450_F0

/**
 * Default value of OQPSK default channel spacing
 */
#define TAL_RF09_OQPSK_CH_SPACING_DEF           OQPSK_915_CH_SPAC
#define TAL_RF24_OQPSK_CH_SPACING_DEF           OQPSK_2450_CH_SPAC

/**
 * Default value for OQPSK rate mode
 */
#define TAL_RF09_OQPSK_RATE                     OQPSK_RATE_MOD_0
#define TAL_RF24_OQPSK_RATE                     OQPSK_RATE_MOD_0

/**
 * Default value of LEG OQPSK current channel in TAL
 */
#define TAL_RF09_LEG_OQPSK_CURRENT_CHANNEL_DEF  (0x01)
#define TAL_RF24_LEG_OQPSK_CURRENT_CHANNEL_DEF  (0x0B)

/**
 * Default value of LEG OQPSK PHY / frequency band
 */
#define TAL_RF09_LEG_OQPSK_FRQ_BAND_DEF         US_915
#define TAL_RF24_LEG_OQPSK_FRQ_BAND_DEF         WORLD_2450

/**
 * Default value of LEG OQPSK default frequency F0
 */
#define TAL_RF09_LEG_OQPSK_FRQ_F0_DEF           (LEG_915_F0 - \
	(TAL_RF09_LEG_OQPSK_CURRENT_CHANNEL_DEF * \
	LEG_915_CH_SPAC))
#define TAL_RF24_LEG_OQPSK_FRQ_F0_DEF           (LEG_2450_F0 - \
	(TAL_RF24_LEG_OQPSK_CURRENT_CHANNEL_DEF * \
	LEG_2450_CH_SPAC))

/**
 * Default value of LEG OQPSK default channel spacing
 */
#define TAL_RF09_LEG_OQPSK_CH_SPACING_DEF       LEG_915_CH_SPAC
#define TAL_RF24_LEG_OQPSK_CH_SPACING_DEF       LEG_2450_CH_SPAC

/**
 * Default value for LEG OQPSK rate mode
 */
#define TAL_RF09_LEG_OQPSK_RATE                 CHIP_RATE_1000
#define TAL_RF24_LEG_OQPSK_RATE                 CHIP_RATE_2000

/**
 * Default value of adapt data rate of the incoming frame for ACK transmission
 */
#define TAL_RF09_ADAPT_DATA_RATE_FOR_ACK        true
#define TAL_RF24_ADAPT_DATA_RATE_FOR_ACK        true

/**
 * Default values for FSK
 */
#define TAL_RF09_FSK_FEC_ENABLE                 false
#define TAL_RF09_FSK_FEC_INTERLEAVING_RSC       false
#define TAL_RF09_FSK_FEC_SCHEME                 FEC_SCHEME_NRNSC
#define TAL_RF09_FSK_PREAMBLE_LEN               8
#define TAL_RF09_FSK_SFD                        0
#define TAL_RF09_FSK_SCRAMBLE_PSDU              true
#define TAL_RF24_FSK_FEC_ENABLE                 false
#define TAL_RF24_FSK_FEC_INTERLEAVING_RSC       false
#define TAL_RF24_FSK_FEC_SCHEME                 FEC_SCHEME_NRNSC
#define TAL_RF24_FSK_PREAMBLE_LEN               8
#define TAL_RF24_FSK_SFD                        0
#define TAL_RF24_FSK_SCRAMBLE_PSDU              true

/**
 * Default values for Mode Switch
 */
#define TAL_RF09_MODE_SWITCH_ENABLE             false
#define TAL_RF24_MODE_SWITCH_ENABLE             false
#define TAL_RF09_MODE_SWITCH_SETTLING_DELAY     100
#define TAL_RF24_MODE_SWITCH_SETTLING_DELAY     100
#define TAL_RF09_NEW_MODE_MODULATION            OFDM
#define TAL_RF24_NEW_MODE_MODULATION            OFDM
#define TAL_RF09_NEW_MODE_OPTION                OFDM_OPT_2
#define TAL_RF24_NEW_MODE_OPTION                OFDM_OPT_2
#define TAL_RF09_NEW_MODE_RATE                  MCS5
#define TAL_RF24_NEW_MODE_RATE                  MCS5
#define TAL_RF09_NEW_MODE_FEC_ENABLED           false
#define TAL_RF24_NEW_MODE_FEC_ENABLED           false
#define TAL_RF09_MODE_SWITCH_DURATION           10000
#define TAL_RF24_MODE_SWITCH_DURATION           10000

/**
 * Default values for FSK raw mode
 */
#define TAL_RF09_FSK_RAW_MODE_ENABLED           false
#define TAL_RF09_FSK_RAW_MODE_RX_LENGTH         250
#define TAL_RF24_FSK_RAW_MODE_ENABLED           false
#define TAL_RF24_FSK_RAW_MODE_RX_LENGTH         250

/**
 * Default values for reduced power consumption setting
 */
#define TAL_RF09_RPC_DEF                        false
#define TAL_RF24_RPC_DEF                        false

/**
 * Default value of frame filter settings
 */
#define TAL_RF09_FRAMES_TYPES       ((1 << FCF_FRAMETYPE_BEACON) | \
	(1 << FCF_FRAMETYPE_DATA) | \
	(1 << FCF_FRAMETYPE_MAC_CMD))
#define TAL_RF09_FRAME_VERSIONS     ((1 << FCF1_FV_2003) | (1 << FCF1_FV_2006))
#define TAL_RF24_FRAMES_TYPES       ((1 << FCF_FRAMETYPE_BEACON) | \
	(1 << FCF_FRAMETYPE_DATA) | \
	(1 << FCF_FRAMETYPE_MAC_CMD))
#define TAL_RF24_FRAME_VERSIONS     ((1 << FCF1_FV_2003) | (1 << FCF1_FV_2006))

/* === PROTOTYPES ========================================================== */

#endif /* TAL_PIB_H */

/* EOF */
