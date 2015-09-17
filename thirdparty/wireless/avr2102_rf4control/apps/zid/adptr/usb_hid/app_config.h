/**
 * @file app_config.h
 *
 * @brief These are application-specific resources which are used
 *        in the example application of the coordinator in addition to the
 *        underlaying stack.
 *
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* === Includes ============================================================= */

#include "stack_config.h"
#include "tal_timer_config.h"
#include "mac_config.h"
#include "node_config.h"
#include "rf4ce.h"
#include "zid.h"
#include "common_nvm.h"
#include "conf_nvm.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

#define NUMBER_OF_APP_TIMERS        (1)

#define TOTAL_NUMBER_OF_TIMERS      (NUMBER_OF_APP_TIMERS + TOTAL_STACK_TIMERS)

/** Defines the number of additional large buffers used by the application */
#define NUMBER_OF_LARGE_APP_BUFS    (0)

/** Defines the number of additional small buffers used by the application */
#define NUMBER_OF_SMALL_APP_BUFS    (0)

/**
 *  Defines the total number of large buffers used by the application and the
 *  layers below.
 */
#define TOTAL_NUMBER_OF_LARGE_BUFS  (NUMBER_OF_LARGE_APP_BUFS +	\
	NUMBER_OF_LARGE_STACK_BUFS)

/**
 *  Defines the total number of small buffers used by the application and the
 *  layers below.
 */
#define TOTAL_NUMBER_OF_SMALL_BUFS  (NUMBER_OF_SMALL_APP_BUFS +	\
	NUMBER_OF_SMALL_STACK_BUFS)

#define TOTAL_NUMBER_OF_BUFS        (TOTAL_NUMBER_OF_LARGE_BUFS + \
	TOTAL_NUMBER_OF_SMALL_BUFS)
bool main_kbd_enable(void);
void main_kbd_disable(void);
bool main_mouse_enable(void);
void main_mouse_disable(void);
void main_remotewakeup_disable(void);
void main_remotewakeup_enable(void);
void main_sof_action(void);
void main_resume_action(void);
void main_suspend_action(void);

#define APP_USER_STRING                 "My Target      "    /* 15 characters !
	                                                     **/
#define USER_STRING_AVAILABLE           true
#define NUM_SUPPORTED_DEV_TYPES         (1)
#define NUM_SUPPORTED_PROFILES          (1)
#define SUPPORTED_DEV_TYPE_0            DEV_TYPE_TELEVISION
#define SUPPORTED_DEV_TYPE_1            DEV_TYPE_TELEVISION
#define SUPPORTED_DEV_TYPE_2            DEV_TYPE_TELEVISION
#define SUPPORTED_PROFILE_ID_0          PROFILE_ID_ZID
#define SUPPORTED_PROFILE_ID_1          PROFILE_ID_ZID
#define SUPPORTED_PROFILE_ID_2          PROFILE_ID_ZID
#define SUPPORTED_PROFILE_ID_3          PROFILE_ID_ZID
#define SUPPORTED_PROFILE_ID_4          PROFILE_ID_ZID
#define SUPPORTED_PROFILE_ID_5          PROFILE_ID_ZID
#define SUPPORTED_PROFILE_ID_6          PROFILE_ID_ZID
#if (USER_STRING_AVAILABLE == true)
#define APP_CAPABILITIES                (0x01 |	\
	(NUM_SUPPORTED_DEV_TYPES << \
	1) | (NUM_SUPPORTED_PROFILES << 4))
#else
#define APP_CAPABILITIES                ((NUM_SUPPORTED_DEV_TYPES << \
	1) | (NUM_SUPPORTED_PROFILES << 4))
#endif

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
