/**
 * @file app_config.h
 *
 * @brief These are application-specific resources which are used
 *        in the example application of the coordinator in addition to the
 *        underlaying stack.
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

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */

/* === Types ================================================================ */

/* Offset of IEEE address storage location within EEPROM */
#define EE_IEEE_ADDR                (0)

/** Defines the number of timers used by the application. */
#define NUMBER_OF_APP_TIMERS        (1)

/** Defines the total number of timers used by the application and the layers
 * below. */
#define TOTAL_NUMBER_OF_TIMERS      NUMBER_OF_APP_TIMERS

#define APP_USER_STRING                 "My Target  "    /* 15 characters ! */
#define USER_STRING_AVAILABLE           true
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
#define APP_CAPABILITIES                (0x01 |	\
	(NUM_SUPPORTED_DEV_TYPES << \
	1) | (NUM_SUPPORTED_PROFILES << 4))
#else
#define APP_CAPABILITIES                ((NUM_SUPPORTED_DEV_TYPES << \
	1) | (NUM_SUPPORTED_PROFILES << 4))
#endif
#define SUPPORTED_CEC_CMDS          {0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, \
				     0x00, \
				     0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, \
				     0x00, \
				     0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, \
				     0x00, \
				     0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, \
				     0x00}

/* Define the periodic wakeup interval */
#define WDT_WAKEUP_INTERVAL         WDT_TO_1S
#define WDT_WAKEUP_INTERVAL_US      (1000000) /* micro seconds */
/* Define the watchdog timeout for system reset */
#define WDT_TIMEOUT_PERIOD          WDT_TO_4S

#define BOOT_LOADER_SIZE 0

#define IEEE_FLASH_OFFSET (INT_FLASH_END - STACK_FLASH_SIZE  + 1)

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
