/**
 * @file mac_config.h
 *
 * @brief These are all timer IDs which are used in MAC.
 *
 * $Id: mac_config.h 26685 2011-05-16 11:38:33Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

/* Prevent double inclusion */
#ifndef MAC_CONFIG_H
#define MAC_CONFIG_H

/* === Includes ============================================================= */

#ifdef ENABLE_RTB
    #include "rtb_config.h"
#else
#include "tal_config.h"
#endif  /* ENABLE_RTB */
#include "mac_build_config.h"

/* === Macros =============================================================== */

#ifdef ENABLE_RTB
    #if (RTB_LAST_TIMER_ID == 0)
    #define MAC_FIRST_TIMER_ID          (0)
    #else
    #define MAC_FIRST_TIMER_ID          (RTB_LAST_TIMER_ID + 1)
    #endif
#else
#if (NUMBER_OF_TAL_TIMERS == 0)
#define MAC_FIRST_TIMER_ID          (0)
#else
#define MAC_FIRST_TIMER_ID          (TAL_LAST_TIMER_ID + 1)
#endif
#endif

/* === Types ================================================================ */


/* Timer ID's used by MAC */

/* DO NOT CHANGE ORDER OF ANY OF THE DEFINES OR TIMERS BELOW! */

// Number of timers required for beacon support
#ifdef BEACON_SUPPORT
    #if (MAC_START_REQUEST_CONFIRM == 1)
        #define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (5)
    #else   /* RFD */
        #define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (3)
    #endif /* MAC_START_REQUEST_CONFIRM */
#else   /* No BEACON_SUPPORT */
        #define NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS         (0)
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */


// Number of timers required to support indirect data transmission
#if (MAC_INDIRECT_DATA_BASIC == 1)
    #if (MAC_INDIRECT_DATA_FFD == 1)
        #define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (2)
    #else
        #define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (1)
    #endif /* MAC_INDIRECT_DATA_FFD */
#else   /* No indirect data support */
        #define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (0)
#endif  /* MAC_INDIRECT_DATA_BASIC */


// Number of timers required to support scanning
#if (MAC_SCAN_SUPPORT == 1)
        #define NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS           (1)
#else
        #define NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS           (0)
#endif  /* MAC_SCAN_SUPPORT */


// Number of timers required to support Rx-Enable
#if (MAC_RX_ENABLE_SUPPORT == 1)
        #define NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS      (1)
#else
        #define NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS      (0)
#endif  /* MAC_RX_ENABLE_SUPPORT */

// Total numbers of timers used in MAC layer
#define NUMBER_OF_MAC_TIMERS        (NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS + \
                                     NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS + \
                                     NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS + \
                                     NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS)


/* DO NOT CHANGE ORDER OF ANY OF THE DEFINES OR TIMERS BELOW! */

#if (NUMBER_OF_MAC_TIMERS > 0)
// Timer type definition only in case there is any MAC timer
typedef enum mac_timer_id_tag
{
         /* MAC Timers start from MAC_FIRST_TIMER_ID */

#ifdef BEACON_SUPPORT
        /* Beacon tracking period to wake up radio before beacon is expected */
        T_Beacon_Tracking_Period = (MAC_FIRST_TIMER_ID),

        /* Superframe timer */
        T_Superframe = (MAC_FIRST_TIMER_ID + 1),

        /* Beacon tracking period to count missed beacon frames */
        T_Missed_Beacon = (MAC_FIRST_TIMER_ID + 2),
    #if (MAC_START_REQUEST_CONFIRM == 1)
        /* Beacon timer */
        T_Beacon = (MAC_FIRST_TIMER_ID + 3),

        /* Beacon preperation timer */
        T_Beacon_Preparation = (MAC_FIRST_TIMER_ID + 4),
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */


#if (MAC_INDIRECT_DATA_BASIC == 1)
        /* Maximum frame response time, also used for association process */
        T_Poll_Wait_Time = (MAC_FIRST_TIMER_ID + NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS),
    #if (MAC_INDIRECT_DATA_FFD == 1)
        /* Indirect Data persistence timer */
        T_Data_Persistence = (MAC_FIRST_TIMER_ID + NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS + 1),
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */


#if (MAC_SCAN_SUPPORT == 1)
        /* Scan duration timer for SCAN */
        T_Scan_Duration = (MAC_FIRST_TIMER_ID + \
                           NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS + \
                           NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS),
#endif  /* MAC_SCAN_SUPPORT */


#if (MAC_RX_ENABLE_SUPPORT == 1)
        /* Receiver Enable timer */
        T_Rx_Enable = (MAC_FIRST_TIMER_ID + \
                       NUMBER_OF_MAC_BEACON_SUPPORT_TIMERS + \
                       NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS + \
                       NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS)
#endif  /* MAC_RX_ENABLE_SUPPORT */
} SHORTENUM mac_timer_id_t;
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */

#ifdef ENABLE_RTB
    #define MAC_LAST_TIMER_ID                   (RTB_LAST_TIMER_ID + NUMBER_OF_MAC_TIMERS)
#else
#if (NUMBER_OF_TAL_TIMERS == 0)
    #if (NUMBER_OF_MAC_TIMERS == 0)
        #define MAC_LAST_TIMER_ID           (0)
    #else
        #define MAC_LAST_TIMER_ID           (NUMBER_OF_MAC_TIMERS - 1)
    #endif
#else
    #define MAC_LAST_TIMER_ID               (TAL_LAST_TIMER_ID + NUMBER_OF_MAC_TIMERS)
#endif
#endif


#ifdef ENABLE_QUEUE_CAPACITY
/**
 * Macro configuring the queue capacities.
 */
#define TAL_MAC_QUEUE_CAPACITY              (255)
#define NHLE_MAC_QUEUE_CAPACITY             (255)
#define INDIRECT_DATA_QUEUE_CAPACITY        (255)
#define BROADCAST_QUEUE_CAPACITY            (255)
#endif /* ENABLE_QUEUE_CAPACITY */

/* === Externals ============================================================ */


/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAC_CONFIG_H */
/* EOF */
