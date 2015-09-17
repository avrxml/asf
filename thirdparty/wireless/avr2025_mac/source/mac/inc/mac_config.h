/**
 *
 * @file mac_config.h
 *
 * @brief These are all timer IDs which are used in MAC.
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

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
