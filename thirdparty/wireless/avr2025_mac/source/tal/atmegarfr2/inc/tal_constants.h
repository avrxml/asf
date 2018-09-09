/**
 * @file tal_constants.h
 *
 * @brief This file contains constants used througthout the TAL.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 */

/*
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
