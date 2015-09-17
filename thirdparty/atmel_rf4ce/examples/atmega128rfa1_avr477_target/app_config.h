/**
 * @file app_config.h
 *
 * @brief These are application-specific resources which are used
 *        in the example application of the coordinator in addition to the
 *        underlaying stack.
 *
 * $Id: app_config.h 27367 2011-06-21 03:13:18Z pawan.jadia $
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
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifdef STB_RCB
#ifdef RZ600
#error "Both board types added as build switches,remove one."
#endif
#endif

/* === Includes ============================================================= */

#include "stack_config.h"
#include "rf4ce.h"
#include "zrc.h"

/* === Macros =============================================================== */

/** @brief This is the first timer identifier of the application.
 *
 *  The value of this identifier is an increment of the largest identifier
 *  value used by the MAC.
 */
#if (NUMBER_OF_TOTAL_STACK_TIMERS == 0)
#define APP_FIRST_TIMER_ID          (0)
#else
#define APP_FIRST_TIMER_ID          (LAST_STACK_TIMER_ID + 1)
#endif

/* === Types ================================================================ */

/** Timer ID's used by the Application */
typedef enum
{
    /* App Timers start from APP_FIRST_TIMER_ID */

    /** Application timer id used to switch off LED */
    T_LED_TIMER     = (APP_FIRST_TIMER_ID)
} app_timer_t;

/** Defines the number of timers used by the application. */
#define NUMBER_OF_APP_TIMERS        (1) /* Do not change this value */

/** Defines the largest timer identifier used by the application */
#define APP_LAST_TIMER_ID           (LAST_STACK_TIMER_ID + NUMBER_OF_APP_TIMERS)

/** Defines the total number of timers used by the application and the layers below. */
#define TOTAL_NUMBER_OF_TIMERS      (NUMBER_OF_APP_TIMERS + NUMBER_OF_TOTAL_STACK_TIMERS)



#define NWKC_MAX_PAIRING_TABLE_ENTRIES  (1) /* Do not set this value >5 */
#define NWKC_VENDOR_IDENTIFIER           (0x1014)
#define NWKC_VENDOR_STRING               "ATMEL  "          /* 7 characters ! */
#define APP_USER_STRING             "Terminal Target"       /* 15 characters ! */
#define USER_STRING_AVAILABLE       true
#define NUM_SUPPORTED_DEV_TYPES         (1)
#define NUM_SUPPORTED_PROFILES          (1)
#define SUPPORTED_DEV_TYPE_0        DEV_TYPE_TELEVISION
#define SUPPORTED_DEV_TYPE_1        DEV_TYPE_TELEVISION
#define SUPPORTED_DEV_TYPE_2        DEV_TYPE_TELEVISION
#define SUPPORTED_PROFILE_ID_0      PROFILE_ID_ZRC
#define SUPPORTED_PROFILE_ID_1      PROFILE_ID_ZRC
#define SUPPORTED_PROFILE_ID_2      PROFILE_ID_ZRC
#define SUPPORTED_PROFILE_ID_3      PROFILE_ID_ZRC
#define SUPPORTED_PROFILE_ID_4      PROFILE_ID_ZRC
#define SUPPORTED_PROFILE_ID_5      PROFILE_ID_ZRC
#define SUPPORTED_PROFILE_ID_6      PROFILE_ID_ZRC
#if (USER_STRING_AVAILABLE == true)
#define APP_CAPABILITIES                (0x01 | (NUM_SUPPORTED_DEV_TYPES << 1) | (NUM_SUPPORTED_PROFILES << 4))
#else
#define APP_CAPABILITIES                ((NUM_SUPPORTED_DEV_TYPES << 1) | (NUM_SUPPORTED_PROFILES << 4))
#endif

#define SUPPORTED_CEC_CMDS          {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

/* QTouch related parameters */
#define USED_QT_CHANNELS            0xF1F0F   //each bit represents one QT channel;should be same as on AVR477 Remote
#define QT_USED_CHANNELS_LENGTH     0x2D //in case of QT_SIGNALS and QT_REFERENCES for current number of used channels
#define QT_STATE_LENGTH	            0x0F
#define QT_DELTA_LENGTH	            0x2D 	
#define QT_SIGNAL_LENGTH            0x45
#define QT_REFERENCES_LENGTH        0x45

/* === Externals ============================================================ */


/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_CONFIG_H */
/* EOF */
