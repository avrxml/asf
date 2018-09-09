/**
 * @file beacon_app.h
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
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * This enum stores the current state of the application.
 */
typedef enum app_state_tag {
	APP_IDLE = 0,
	APP_SCAN_DONE,
	APP_ASSOC_IN_PROGRESS,
	APP_DEVICE_RUNNING
}
app_state_t;

/* === Macros =============================================================== */
#define DEFAULT_PAN_ID                  CCPU_ENDIAN_TO_LE16(0x1111)

/** Defines the short address of the coordinator. */
#define COORD_SHORT_ADDR                (0x0000)

#define SCAN_CHANNEL(x)                    (1ul << x)

#ifdef MAC_SECURITY_ZIP
/* MAC security macros */
#define KEY_INDEX_0                     (0)
#define KEY_INDEX_1                     (1)
#define KEY_INDEX_2                     (2)
#define KEY_INDEX_3                     (3)
#define KEY_INDEX_4                     (4)
#define LOOKUP_DATA_SIZE_1              (1) /* Size is 9 octets */
#define FRAME_TYPE_BEACON               (0)
#define FRAME_TYPE_DATA                 (1)
#define CMD_FRAME_ID_NA                 (0) /* CommandFrameIdentifier is n/a */
#define ZIP_SEC_MIN                     (0x05) /* SecurityMinimum for ZIP is 5
	                                       **/
#define DEV_OVERRIDE_SEC_MIN            (1) /* DeviceOverrideSecurityMinimum:
	                                     * True */
#define ZIP_KEY_ID_MODE                 (1) /* ZIP uses KeyIdMode 1 */

#define INDEX_0                         (0)
#define INDEX_1                         (1)
#define INDEX_2                         (2)
#define INDEX_3                         (3)
#define DEV_DESC_HANDLE_IDX_0                   (0x00)
#define EMPTY_DEV_HANDLE                (0xFF) /* key device desc is invalid */
#define KEY_INFO_FRAME                  (0xDE)
#define NO_SECURITY                     (0)
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

#endif /* APP_CONFIG_H */
/* EOF */
