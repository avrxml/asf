/**
 *
 * @file mac_config.h
 *
 * @brief These are all timer IDs which are used in MAC.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef MAC_CONFIG_H
#define MAC_CONFIG_H

/* === Includes ============================================================= */

#ifdef ENABLE_RTB
#include "rtb_config.h"
#else
#include "tal_build_config.h"
#endif  /* ENABLE_RTB */
#include "mac_build_config.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/**
 * \addtogroup group_mac_def
 * @{
 */

/* Timer ID's used by MAC */

/* DO NOT CHANGE ORDER OF ANY OF THE DEFINES OR TIMERS BELOW! */

/* Number of timers required for beacon support */
#ifdef BEACON_SUPPORT
#ifdef GTS_SUPPORT
    #if (MAC_START_REQUEST_CONFIRM == 1)
    #define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (6)
    #else   /* RFD */
    #define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (4)
    #endif /* MAC_START_REQUEST_CONFIRM */
#else /* GTS_SUPPORT */
    #if (MAC_START_REQUEST_CONFIRM == 1)
	#define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (5)
    #else   /* RFD */
	#define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (3)
    #endif /* MAC_START_REQUEST_CONFIRM */
#endif /* GTS_SUPPORT */
#else   /* No BEACON_SUPPORT */
	#define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (0)
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

/* Number of timers required to support indirect data transmission */
#if (MAC_INDIRECT_DATA_BASIC == 1)
    #if (MAC_INDIRECT_DATA_FFD == 1)
	#define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (2)
    #else
	#define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (1)
    #endif /* MAC_INDIRECT_DATA_FFD */
#else   /* No indirect data support */
	#define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (0)
#endif  /* MAC_INDIRECT_DATA_BASIC */

/* Number of timers required to support scanning */
#if (MAC_SCAN_SUPPORT == 1)
	#define NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS           (1)
#else
	#define NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS           (0)
#endif  /* MAC_SCAN_SUPPORT */

/* Number of timers required to support Rx-Enable */
#if (MAC_RX_ENABLE_SUPPORT == 1)
	#define NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS      (1)
#else
	#define NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS      (0)
#endif  /* MAC_RX_ENABLE_SUPPORT */

/* Total numbers of timers used in MAC layer */
#define NUMBER_OF_MAC_TIMERS        (NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS + \
	NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS + \
	NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS + \
	NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS)

/* DO NOT CHANGE ORDER OF ANY OF THE DEFINES OR TIMERS BELOW! */

#if (NUMBER_OF_MAC_TIMERS > 0)
/* Timer type definition only in case there is any MAC timer */

#ifdef BEACON_SUPPORT
/* Beacon tracking period to wake up radio before beacon is expected */
extern uint8_t T_Beacon_Tracking_Period;

/* Superframe timer */
extern uint8_t T_Superframe;

/* Beacon tracking period to count missed beacon frames */
extern uint8_t T_Missed_Beacon;
    #if (MAC_START_REQUEST_CONFIRM == 1)
/* Beacon timer */
extern uint8_t T_Beacon;

/* Beacon preperation timer */
extern uint8_t T_Beacon_Preparation;
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#ifdef GTS_SUPPORT
/* GTS timer */
extern uint8_t T_CAP;
#endif  /* GTS_SUPPORT */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_BASIC == 1)
/* Maximum frame response time, also used for association process */
extern uint8_t T_Poll_Wait_Time;
    #if (MAC_INDIRECT_DATA_FFD == 1)
/* Indirect Data persistence timer */
extern uint8_t T_Data_Persistence;
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
/* Scan duration timer for SCAN */
extern uint8_t T_Scan_Duration;
#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
/* Receiver Enable timer */
extern uint8_t T_Rx_Enable;
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */

#ifdef ENABLE_QUEUE_CAPACITY

/**
 * Macro configuring the queue capacities.
 */
#define TAL_MAC_QUEUE_CAPACITY              (255)
#define NHLE_MAC_QUEUE_CAPACITY             (255)
#define INDIRECT_DATA_QUEUE_CAPACITY        (255)
#define BROADCAST_QUEUE_CAPACITY            (255)
#define GTS_QUEUE_CAPACITY                  (255)
#endif /* ENABLE_QUEUE_CAPACITY */

/* ! @} */
/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

retval_t mac_timers_init(void);
retval_t mac_timers_stop(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAC_CONFIG_H */
/* EOF */
