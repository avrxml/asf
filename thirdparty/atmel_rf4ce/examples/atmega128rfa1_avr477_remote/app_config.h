/**
 * @file app_config.h
 *
 * @brief Single Button Controller application config
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
#define NUMBER_OF_APP_TIMERS        (1)

/** Defines the largest timer identifier used by the application */
#define APP_LAST_TIMER_ID           (LAST_STACK_TIMER_ID + NUMBER_OF_APP_TIMERS)

/** Defines the total number of timers used by the application and the layers below. */
#define TOTAL_NUMBER_OF_TIMERS      (NUMBER_OF_APP_TIMERS + NUMBER_OF_TOTAL_STACK_TIMERS)

/* Configure RF4CE node capabilities */
/* The capabilities of this node. Implementation specific according to the format
 * illustrated in Figure 26.
 * Implementation specific
 * NodeTypeTarget
 * NodeTypeController
 * PowerSourceMainsTrue
 * PowerSourceMainsFalse
 * SecurityCapableTrue
 * SecurityCapableFalse
 * ChannelNormalizationCapableTrue
 * ChannelNormalizationCapableFalse    */
#ifdef RF4CE_SECURITY
#define NWKC_NODE_CAPABILITIES   (NodeTypeController | PowerSourceMainsFalse \
                             | SecurityCapableTrue | ChannelNormalizationCapableTrue)
#else
#define NWKC_NODE_CAPABILITIES   (NodeTypeController | PowerSourceMainsFalse \
                             | SecurityCapableFalse | ChannelNormalizationCapableTrue)
#endif

#define NWKC_VENDOR_IDENTIFIER           (0x1014)
#define NWKC_VENDOR_STRING               "ATMEL  "            /* 7 characters ! */
#define APP_USER_STRING             "Single Btn Ctrl"    /* 15 characters ! */
#define USER_STRING_AVAILABLE       true
#define NUM_SUPPORTED_DEV_TYPES         (1)
#define NUM_SUPPORTED_PROFILES          (1)
#if (USER_STRING_AVAILABLE == true)
#define APP_CAPABILITIES                (0x01 | (NUM_SUPPORTED_DEV_TYPES << 1) | (NUM_SUPPORTED_PROFILES << 4))
#else
#define APP_CAPABILITIES                ((NUM_SUPPORTED_DEV_TYPES << 1) | (NUM_SUPPORTED_PROFILES << 4))
#endif

#define FW_VERSION_MAJOR            1
#define FW_VERSION_MINOR            3
#define FW_VERSION_REV              0
#define SUPPORTED_CEC_CMDS          {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, \
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define TX_OPTIONS  (TXO_UNICAST | TXO_DST_ADDR_NET | \
                     TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
                     TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)


/* Define the watchdog timeout for system reset */
#define WDT_TIMEOUT_PERIOD          WDT_TO_8S

/* Application specific active and sleep durations, can be configure by user.
   General guideline is that, the duty-cycle (sleep_duration/ 'Active+Sleep' Duration) 
   sholuld be more so that battery can last longer.
*/
#define AVR477_ACTIVE_DURATION_SEC    10 /* in Seconds */
#define AVR477_SLEEP_DURATION_SEC     10 /* In Seconds */
#define AVR477_WARM_RESET_WAIT_TIME_SEC   10 /* in Seconds */

/* EEPROM address to save the qdebug data */
#define EE_QDEBUG_OFFSET    (EE_IEEE_ADDR + 0xC8)

/* QTouch related parameters */
#define USED_QT_CHANNELS     0xF1F0F   //each bit represents one QT channel
#define QT_STATE_LENGTH	     0x0F	
#define QT_DELTA_LENGTH	     0x2D 	
#define QT_SIGNAL_LENGTH     0x45
#define QT_REFERENCES_LENGTH      0x45
#define QT_START_DELTA_DATA  0x11
#define QT_START_REF_DATA    0x1D



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
