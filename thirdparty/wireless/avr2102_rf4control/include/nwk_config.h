/**
 * @file nwk_config.h
 *
 * @brief This module includes the Buffer and timer configuration for RF4Control
 * stack.
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
 *
 */

/* Prevent double inclusion */
#ifndef NWK_CONFIG_H
#define NWK_CONFIG_H

/* === Includes ============================================================= */

/* #include "mac_config.h" */
#include "ieee_const.h"

/* === Macros =============================================================== */

#define RF4CE   5

/* === Types ================================================================ */

/**
 * \ingroup group_rf4control
 * \defgroup group_nwk_config  NWK layer configurations
 * All nwk layer configuration Macros are defined in this module
 * @{
 */

#if ((defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM))
#define NUMBER_OF_CH_AG_TIMERS          1
#else
#define NUMBER_OF_CH_AG_TIMERS          0
#endif

#ifdef ZRC_PROFILE
#define NUMBER_OF_PROFILE_TIMERS        1
#else
#define NUMBER_OF_PROFILE_TIMERS        0
#endif

#ifdef PBP_REC
#define PBP_TIMER                       1
#else
#define PBP_TIMER                       0
#endif

#define NUMBER_OF_NWK_TIMERS            1 + NUMBER_OF_CH_AG_TIMERS + \
	NUMBER_OF_PROFILE_TIMERS + PBP_TIMER

#if (HIGHEST_STACK_LAYER == RF4CE)
#define NUMBER_OF_LARGE_STACK_BUFS              (5)

#define NUMBER_OF_SMALL_STACK_BUFS              (0)
#define TOTAL_STACK_TIMERS (NUMBER_OF_TAL_TIMERS + NUMBER_OF_MAC_TIMERS + \
	NUMBER_OF_NWK_TIMERS)
#endif
#if (HIGHEST_STACK_LAYER == RF4CE)

/* Reduce the header file dependency by using hard-coded values */
#if (MEGA || XMEGA)
#define MCPS_DATA_IND_SIZE  (32)        /* Size of mcps_data_ind_t w/o MAC
	                                 * security */
#define LARGE_BUFFER_SIZE           (MCPS_DATA_IND_SIZE + \
	aMaxMACPayloadSize + \
	FCS_LEN + \
	LQI_LEN + ED_VAL_LEN)
#elif (UC3)

/* The discovery confirm message needs to be held by the buffer; its size is
 * 180. */
#define LARGE_BUFFER_SIZE            180
#elif (SAM)

/* The discovery confirm message needs to be held by the buffer; its size is
 * 180. */
#define LARGE_BUFFER_SIZE            176
#else
#error "Unknown platform type"
#endif
#endif  /*#if (HIGHEST_STACK_LAYER == RF4CE)  */

#ifdef ENABLE_QUEUE_CAPACITY

/**
 * Macro configuring the queue capacities.
 */
#define NHLE_NWK_QUEUE_CAPACITY         (255)
#define NWK_NHLE_QUEUE_CAPACITY         (255)
#define MAC_NWK_QUEUE_CAPACITY          (255)
#endif  /* ENABLE_QUEUE_CAPACITY */

#define DEVICE_TYPE_LIST_SIZE           3
#define PROFILE_ID_LIST_SIZE            7
#define NWKC_VENDOR_IDENTIFIER           (0x1014)
#define NWKC_VENDOR_STRING               "ATMEL  "          /* 7 characters ! */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */
/* ! @} */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWK_CONFIG_H */
/* EOF */
