/**
 * @file tal_config.h
 *
 * @brief File contains TAL configuration parameters.
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
#ifndef TAL_CONFIG_H
#define TAL_CONFIG_H

/* === INCLUDES ============================================================ */

/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */

#define TAL_FIRST_TIMER_ID              (0)

/**
 * To configure the clock output configuration to the build,
 * set the following define TRX_CLOCK_OUTPUT_SELECTION to the required value.
 * Value - meaning
 * 0       off
 * 1       26 MHz (default/reset)
 * 2       32 MHz
 * 3       16 MHz
 * 4       8 MHz
 * 5       4 MHz
 * 6       2 MHz
 * 7       1 MHz
 */
#ifndef TRX_CLOCK_OUTPUT_SELECTION
#define TRX_CLOCK_OUTPUT_SELECTION  0
#endif

/**
 * To change the transceiver's interrupt polarity,
 * set the following define TRX_IRQ_POLARITY to the required value.
 * Value - meaning
 * 0 = Active HIGH (default/reset)
 * 1 = Active LOW
 */
#ifndef TRX_IRQ_POLARITY
#define TRX_IRQ_POLARITY          0
#endif

#ifdef TAL_SUPPORT_ALL_FEATURES

/**
 * To include the filter tuning functions to the build uncomment the define
 * ENABLE_FTN_PLL_CALIBRATION.
 */
#   ifndef ENABLE_FTN_PLL_CALIBRATION
/* #       define ENABLE_FTN_PLL_CALIBRATION */
#   endif

/**
 * To include the functionality of measuring the on air duration to the build,
 * uncomment the define MEASURE_ON_AIR_DURATION.
 * The on-air-duration can be read/written using the PIB attribute
 * phyOnAirDuration.
 */
#   ifndef MEASURE_ON_AIR_DURATION
/* #       define MEASURE_ON_AIR_DURATION */
#   endif

/**
 * For applications such as sniffer or beacon-enabled applications a time stamp
 * for each frame is required.
 * This feature can be enabled during compile time by uncomment the define
 * ENABLE_TSTAMP.
 */
#   ifndef ENABLE_TSTAMP
/* #       define ENABLE_TSTAMP */
#   endif

/**
 * To upload the CRC field and append it to the PSDU, uncomment the
 * following define UPLOAD_CRC.
 */
#    ifndef UPLOAD_CRC
#        define UPLOAD_CRC
#    endif

/**
 * To include the promiscuous mode functionality to the build, uncomment the
 * following define PROMISCUOUS_MODE.
 * The actual use of this feature can be enabled/disabled during runtime
 * using the PIB attribute macPromiscuousMode.
 */
#   ifndef PROMISCUOUS_MODE
/* #       define PROMISCUOUS_MODE */
#   endif

/**
 * To include the frame filter configuration functionality to the build,
 * uncomment the following define FRAME_FILTER_CONFIGURATION.
 * The actual use of this feature can be controlled during runtime
 * using the PIB attributes macFrameFilterFrameTypes and
 * macFrameFilterFrameVersion.
 */
#   ifndef SUPPORT_FRAME_FILTER_CONFIGURATION
#       define SUPPORT_FRAME_FILTER_CONFIGURATION
#   endif

/**
 * To use the IQ radio mode functionality,
 * uncomment the following define IQ_RADIO.
 */
/* #define IQ_RADIO */

/**
 * To include the basic mode functionality to the build,
 * uncomment the following define BASIC_MODE.
 */
/* #define BASIC_MODE */

/**
 * To include the legacy O-QPSK support,
 * uncomment the following define SUPPORT_LEGACY_OQPSK.
 */
#   ifndef SUPPORT_LEGACY_OQPSK
#       define SUPPORT_LEGACY_OQPSK
#   endif

/**
 * Enable support of MR-OQPSK modulation
 * To include the MR-OQSK support to the build,
 * uncomment the following define SUPPORT_OQPSK.
 */
#   ifndef SUPPORT_OQPSK
#       define SUPPORT_OQPSK
#   endif

/**
 * Enable support of MR-OFDM modulation
 * To include the MR-OFDM support to the build,
 * uncomment the following define SUPPORT_OFDM.
 */
#   ifndef SUPPORT_OFDM
#       define SUPPORT_OFDM
#   endif

/**
 * To include the functionality of adaption the data rate of the incoming frame
 * to the ACK frame, uncomment the define SUPPORT_ACK_RATE_MODE_ADAPTION.
 * The actual use of this feature can be enabled/disabled during runtime
 * using the PIB attribute macAdaptDataRateForACK.
 */
#   ifndef SUPPORT_ACK_RATE_MODE_ADAPTION
#       define SUPPORT_ACK_RATE_MODE_ADAPTION
#   endif

/**
 * Enable support of FSK modulation
 * To include the FSK support to the build,
 * uncomment the following define SUPPORT_FSK.
 */
#   ifndef SUPPORT_FSK
#       define SUPPORT_FSK
#   endif

/**
 * Enable FSK mode switch support
 * To include the mode switch support to the build,
 * uncomment the following define SUPPORT_MODE_SWITCH.
 */
#   ifndef SUPPORT_MODE_SWITCH
/* #       define SUPPORT_MODE_SWITCH */
#   endif

/**
 * Feature to enable FSK raw mode handling
 * To include the FSK raw mode handling,
 * uncomment the following define SUPPORT_FSK_RAW_MODE
 */
#   ifndef SUPPORT_FSK_RAW_MODE
/* #       define SUPPORT_FSK_RAW_MODE */
#   endif

/**
 * Enable Time of Flight measurement
 * To include the measurement to the build,
 * uncomment the following define MEASURE_TIME_OF_FLIGHT.
 */
#   ifndef MEASURE_TIME_OF_FLIGHT
#       ifndef BASIC_MODE
/* #           define MEASURE_TIME_OF_FLIGHT */
#       endif
#   endif

/**
 * Support all TFA features
 * To include all TFA features to the build,
 * uncomment the following define ENABLE_TFA.
 */
#   ifndef ENABLE_TFA
#       define ENABLE_TFA
#   endif

/**
 * Support all battery monitor features
 * To include all battery monitor features to the build,
 * uncomment the following define TFA_BAT_MON.
 * It is a subset of the configuration ENABLE_TFA.
 */
#   ifndef TFA_BAT_MON
#       define TFA_BAT_MON
#   endif

/**
 * Support the battery monitor read feature
 * To include the battery monitor read feature to the build,
 * uncomment the following define TFA_BAT_MON_READ.
 * It is a subset of the configuration TFA_BAT_MON.
 */
#   ifndef TFA_BAT_MON_READ
#       define TFA_BAT_MON_READ
#   endif

/**
 * Support the battery monitor interrupt feature
 * To include the battery monitor interrupt feature to the build,
 * uncomment the following define TFA_BAT_MON_IRQ.
 * It is a subset of the configuration TFA_BAT_MON.
 */
#   ifndef TFA_BAT_MON_IRQ
#       define TFA_BAT_MON_IRQ
#   endif

/**
 * Support the continuous transmission feature
 * To include the continuous transmission feature to the build,
 * uncomment the following define TFA_CW.
 * It is a subset of the configuration ENABLE_TFA.
 */
#   ifndef TFA_CW
#       define TFA_CW
#   endif

/**
 * Support the TFA CCA feature
 * To include the TFA CCA to the build,
 * uncomment the following define TFA_CCA.
 * It is a subset of the configuration ENABLE_TFA.
 */
#   ifndef TFA_CCA
#       define TFA_CCA
#   endif

/**
 * Use direct modulation for all devices except version 1
 */
#   if (!defined RF215v1)
#       define DIRECT_MODULATION
#   endif

#endif /* TAL_SUPPORT_ALL_FEATURES */

/**
 * Check for unsupported build combinations.
 */
#if ((!defined SUPPORT_LEGACY_OQPSK) && (!defined SUPPORT_OQPSK) && \
	(!defined SUPPORT_OFDM) && (!defined SUPPORT_FSK))
#   error "Missing modulation support, enable at least one modulation support."
#endif

#if (defined SUPPORT_ACK_RATE_MODE_ADAPTION) && (!defined SUPPORT_OFDM) && \
	(!defined SUPPORT_OQPSK)
#   error "Invalid combination"
#endif

#if (defined SUPPORT_MODE_SWITCH) && (!defined SUPPORT_FSK)
#   error "FSK suport missing"
#endif

#if (defined SUPPORT_FSK_RAW_MODE) && (!defined SUPPORT_FSK)
#   error "FSK support missing"
#endif

#if (!defined BASIC_MODE) && (defined IQ_RADIO)
# error "undefined combination"
#endif

#if (defined MEASURE_TIME_OF_FLIGHT) && (defined BASIC_MODE)
# error "undefined combination"
#endif

/* === TYPES =============================================================== */

/* === PROTOTYPES ========================================================== */

#endif /* TAL_CONFIG_H */
