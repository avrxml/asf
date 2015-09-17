/**
 * @file
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
#ifndef BEACON_APP_H
#define BEACON_APP_H

/* === Includes ============================================================= */

/* === TYPES =============================================================== */

/** This type definition of a structure can store the short address and the
 *  extended address of a device.
 */
typedef struct associated_device_tag {
	uint16_t short_addr;
	uint64_t ieee_addr;
} associated_device_t;

/**
 * This enum store the current state of the coordinator.
 */
typedef enum coord_state_tag {
	COORD_STARTING = 0,
	COORD_RUNNING
} coord_state_t;

/* === Macros =============================================================== */
#define DEFAULT_PAN_ID                  CCPU_ENDIAN_TO_LE16(0x1111)

/** Defines the short address of the coordinator. */
#define COORD_SHORT_ADDR                (0x0000)

/** Defines the maximum number of devices this coordinator will handle. */
#define MAX_NUMBER_OF_DEVICES           (100)

/**
 * Defines the length of the beacon payload delivered to the devices.
 * This is the text "Atmel beacon demo" + one space + one uin8t_t variable.
 */
#define BEACON_PAYLOAD_LEN              (17 + 1 + 1)

#define SCAN_CHANNEL(x)                    (1ul << x)

/** Defines the scan duration time. */
#define SCAN_DURATION_COORDINATOR       (5)

#ifdef MAC_SECURITY_ZIP

/* MAC security macros */
#define KEY_INDEX_0                     (0)
#define KEY_INDEX_1                     (1)
#define KEY_INDEX_2                     (2)
#define KEY_INDEX_3                     (3)
#define KEY_INDEX_4                     (4)

/*Look up Data Size is 9 octets */
#define LOOKUP_DATA_SIZE_1              (1)

/* Type of frame */
#define FRAME_TYPE_BEACON               (0)
#define FRAME_TYPE_DATA                 (1)

/* CommandFrameIdentifier is n/a */
#define CMD_FRAME_ID_NA                 (0)

/* Default Minimum Security Level for ZIP */
#define ZIP_SEC_MIN                     (0x05)

/* Override the  minimum security level */
#define DEV_OVERRIDE_SEC_MIN            (1)

/* ZIP uses KeyIdMode 1 */
#define ZIP_KEY_ID_MODE                 (1)

/* mac key table index entry macros */
#define INDEX_0                         (0)
#define INDEX_1                         (1)
#define INDEX_2                         (2)
#define INDEX_3                         (3)

/* Device Descriptor Handle Index 0 */
#define DEV_DESC_HANDLE_IDX_0                   (0x00)

/* key device desc is invalid */
#define EMPTY_DEV_HANDLE                (0xFF)

/* Key Info frame type macro */
#define KEY_INFO_FRAME                  (0xDE)

/* Security Not used */
#define NO_SECURITY                     (0)

/* Security PIB Initialize Macro */
#define SEC_PIB_INIT                                    (1)

/* Security Default Device Table Entries */
#define DEFAULT_MAC_DEV_TABLE_ENTRIES (0x04)

/* Security Default Key Table Entries */
#define DEFAULT_MAC_KEY_TABLE_ENTRIES (0x04)

/* Security Default Security Level Table Entries */
#define DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES (0x02)

#endif

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BEACON_APP_H */
/* EOF */
