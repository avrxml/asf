/**
 * @file tal_constants.h
 *
 * @brief This file contains constants used througthout the TAL.
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

/* Prevent double inclusion */
#ifndef TAL_CONSTANTS_H
#define TAL_CONSTANTS_H

/* === INCLUDES ============================================================ */

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/*
 * TAL PIB default values
 */

/*
 * Default value of custom TAL PIB channel page
 */
#define TAL_CURRENT_PAGE_DEFAULT            (0x00)
#ifndef HIGH_DATA_RATE_SUPPORT
#if (TAL_CURRENT_PAGE_DEFAULT != 0)
#error "unsupported channel page"
#endif
#endif

/*
 * Default value of maximum number of symbols in a frame
 */
#define TAL_MAX_FRAME_DURATION_DEFAULT      (MAX_FRAME_DURATION)

/*
 * Default value of duration of the synchronization header (SHR) in symbols
 * for the current PHY
 */
#define TAL_SHR_DURATION_DEFAULT            (NO_OF_SYMBOLS_PREAMBLE_SFD)

/*
 * Default value of number of symbols per octet for the current PHY
 */
#define TAL_SYMBOLS_PER_OCTET_DEFAULT       (SYMBOLS_PER_OCTET)

/*
 * Default value of maximum backoff exponent used while performing csma ca
 */
#define TAL_MAXBE_DEFAULT                   (0x05)

/*
 * Default value of PIB attribute macMaxFrameRetries
 */
#define TAL_MAXFRAMERETRIES_DEFAULT         (0x03)

/*
 * Default value of maximum csma ca backoffs
 */
#define TAL_MAX_CSMA_BACKOFFS_DEFAULT       (0x04)

/*
 * Default value of minimum backoff exponent used while performing csma ca
 */
#define TAL_MINBE_DEFAULT                   (0x03)

/*
 * Value of a broadcast PAN ID
 */
#define TAL_PANID_BC_DEFAULT                (0xFFFF)

/*
 * Default value of short address
 */
#define TAL_SHORT_ADDRESS_DEFAULT           (0xFFFF)

/*
 * Default value of current channel in TAL
 */
#define TAL_CURRENT_CHANNEL_DEFAULT         (0x0B)

/*
 * Default value of promiscuous mode in TAL
 */
#define TAL_PIB_PROMISCUOUS_MODE_DEFAULT    (false)

/*
 * Default value of transmit power of transceiver: Use highest tx power
 */
#define TAL_TRANSMIT_POWER_DEFAULT          (TX_PWR_TOLERANCE | 0x04)

/*
 * Default value CCA mode
 */
#define TAL_CCA_MODE_DEFAULT                (TRX_CCA_MODE1)

/*
 * Default value beacon order set to 15
 */
#define TAL_BEACON_ORDER_DEFAULT            (15)

/*
 * Default value supeframe order set to 15
 */
#define TAL_SUPERFRAME_ORDER_DEFAULT        (15)

/*
 * Default value of BeaconTxTime
 */
#define TAL_BEACON_TX_TIME_DEFAULT          (0x00000000)

/*
 * Default value of BatteryLifeExtension.
 */
#define TAL_BATTERY_LIFE_EXTENSION_DEFAULT  (false)

/*
 * Default value of PAN Coordiantor custom TAL PIB
 */
#define TAL_PAN_COORDINATOR_DEFAULT         (false)

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_CONSTANTS_H */

/* EOF */
