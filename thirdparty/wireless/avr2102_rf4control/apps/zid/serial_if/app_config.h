/**
 * \file *********************************************************************
 *
 * \brief app_config.h
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

/** Defines the number of timers used by the application. */
#define NUMBER_OF_APP_TIMERS        (1)

/** Defines the total number of timers used by the application and the layers
 * below. */
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
