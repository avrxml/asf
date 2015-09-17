/**
 * @file app_config.h
 *
 * @brief These are application-specific resources which are used
 *        in the example application of the coordinator in addition to the
 *        underlaying stack.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

/* === Macros =============================================================== */

/** Defines the number of timers used by the application. */
#define NUMBER_OF_APP_TIMERS        (4)

/** Defines the total number of timers used by the application and the layers
 * below. */
#define TOTAL_NUMBER_OF_TIMERS      (NUMBER_OF_APP_TIMERS + \
	NUMBER_OF_TOTAL_STACK_TIMERS)

/** Defines the number of additional large buffers used by the application */
#define NUMBER_OF_LARGE_APP_BUFS    (10)

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

/* Offset of IEEE address storage location within EEPROM */
#define EE_IEEE_ADDR                (0)

#ifdef MAC_SECURITY_ZIP

/**
 * The maximum number of entries supported in the macKeyTable.
 * This value is implementation specific.
 */
#define MAC_ZIP_MAX_KEY_TABLE_ENTRIES           (4)

/**
 * The maximum number of entries supported in the macDeviceTable.
 * This value is implementation specific.
 */
#if (MAC_START_REQUEST_CONFIRM == 1)    /* FFD like device */
#define MAC_ZIP_MAX_DEV_TABLE_ENTRIES           (10)
#else
#define MAC_ZIP_MAX_DEV_TABLE_ENTRIES           (3)
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

/**
 * The maximum number of entries supported in the macSecurityLevelTable.
 * This value is implementation specific.
 */
#define MAC_ZIP_MAX_SEC_LVL_TABLE_ENTRIES       (2)

/**
 * The maximum number of entries supported in the KeyIdLookupList
 */
#define MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES  (1)

/**
 * The maximum number of entries supported in the KeyDeviceList
 */
#define MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES        (4)

/**
 * The maximum number of entries supported in the KeyUsageList
 */
#define MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES      (1)
#endif /* MAC_SECURITY_ZIP */
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
