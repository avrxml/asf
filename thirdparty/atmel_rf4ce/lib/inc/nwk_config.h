/**
 * @file nwk_config.h
 *
 * @brief These are all timer IDs which are used in NWK.
 *
 * $Id: nwk_config.h 25096 2011-01-24 09:26:10Z uwalter $
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
#ifndef NWK_CONFIG_H
#define NWK_CONFIG_H

/* === Includes ============================================================= */


#include "mac_config.h"


/* === Macros =============================================================== */


#define NWK_FIRST_TIMER_ID              (MAC_LAST_TIMER_ID + 1)


/* === Types ================================================================ */


/* Timer ID's used by NWK */
typedef enum
{
    /* NWK Timers start from NWK_FIRST_TIMER_ID */
    /* T_NWK is the generic timer used for the NWK layer */
    T_NWK = (NWK_FIRST_TIMER_ID)
#if ((defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM))
    ,
    T_CH_AG     = (NWK_FIRST_TIMER_ID + 1)
#endif  /* #if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM) */
#ifdef ZRC_PROFILE
    ,
    #if ((defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM))
    T_ZRC_PROFILE = (NWK_FIRST_TIMER_ID + 2)
    #else
    T_ZRC_PROFILE = (NWK_FIRST_TIMER_ID + 1)
    #endif
#endif  /* ZRC_PROFILE */
} nwk_timer_t;


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

#define NUMBER_OF_NWK_TIMERS            1 + NUMBER_OF_CH_AG_TIMERS + NUMBER_OF_PROFILE_TIMERS

#define NWK_LAST_TIMER_ID               (MAC_LAST_TIMER_ID + NUMBER_OF_NWK_TIMERS)

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


/* === Externals ============================================================ */


/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWK_CONFIG_H */
/* EOF */
