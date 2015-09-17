/**
 * @file node_config.h
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
#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

/* === Includes ============================================================= */

#include "stack_config.h"
#include "rf4ce.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/* Offset of IEEE address storage location within EEPROM */
#define EE_IEEE_ADDR                (0)

/* Configure RF4CE node capabilities */

/* The capabilities of this node. Implementation specific according to the
 * format
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
#define NWKC_NODE_CAPABILITIES   (NodeTypeTarget | PowerSourceMainsFalse \
	| SecurityCapableTrue | ChannelNormalizationCapableTrue)
#else
#define NWKC_NODE_CAPABILITIES   (NodeTypeTarget | PowerSourceMainsFalse \
	| SecurityCapableFalse | ChannelNormalizationCapableTrue)
#endif

#define NWKC_MAX_PAIRING_TABLE_ENTRIES    (5)
#define NWKC_VENDOR_IDENTIFIER           (0x1014)
#define NWKC_VENDOR_STRING               "ATMEL  "          /* 7 characters ! */

/* Implementation specific ZRC constant defines */

#define NWK_ACTIVE_PERIOD           nwkcMinActivePeriod /* 16.8 ms */

#define NWK_DUTY_CYCLE              nwkcMaxDutyCycle   /* 62500 symbols = 1s */
#define NWK_DISCOVERY_LQI_THRESHOLD (0x00)

/* Implementation specific ZRC constant defines */
#define APL_ZID_PROFILE_VERSION             aplZIDProfileVersion_def
#define APL_KEY_EXCHANGE_TRANSFER_COUNT     aplcKeyExchangeTransferCount_def
#define APL_DEVICE_IDLE_RATE                aplDeviceIdleRate_def
/* #define APL_INT_PIPE_MAX_SAFE_TXS           aplcMinIntPipeMaxSafeTxs */
#define APL_INT_PIPE_UNSAFE_TXS_WINDOW      aplcMinIntPipeUnsafeTxWindowTime
#define APL_REPORT_REPEAT_INTERVAL          (aplcMaxReportRepeatInterval / 2)
#define APL_HEARTBEAT_INTERVAL              HEARTBEAT_INTERVAL_DEF
#define APL_HID_PARSER_VERSION              HID_PARSER_VERSION
#define APL_HID_DEVICE_SUB_CLASS            HID_DEVICE_SUB_CLASS
#define APL_HID_PROTOCOL_CODE               HID_PROTOCOL_CODE
#define APL_HID_COUNTRY_CODE                HID_COUNTRY_CODE
#define APL_HID_DEVICE_RELEASE_NUMBER       HID_DEVICE_RELEASE_NUMBER
#define APL_HID_VENDOR_ID                   HID_VENDOR_ID
#define APL_HID_PRODUCT_ID                  HID_PRODUCT_ID
#define APL_HID_NUMBER_ENDPOINTS            HID_NUMBER_ENDPOINTS
#define APL_HID_POLL_INTERVAL               HID_POLL_INTERVAL
#define APL_HID_NUM_STD_DESCRIPTORS         HID_NUM_STD_DESCRIPTORS
#define APL_HID_STD_DESCRIPTOR_LIST_0       HID_STD_DESCRIPTOR_LIST_0
#define APL_HID_STD_DESCRIPTOR_LIST_1       HID_STD_DESCRIPTOR_LIST_1
#define APL_HID_STD_DESCRIPTOR_LIST_2       HID_STD_DESCRIPTOR_LIST_2
#define APL_HID_STD_DESCRIPTOR_LIST_3       HID_STD_DESCRIPTOR_LIST_3
#define APL_HID_NUM_DESCRIPTORS             HID_NUM_DESCRIPTORS
#define APL_HID_NUM_NULL_REPORTS            HID_NUM_NULL_REPORTS

/* Set storage location/addresss of flash memory;
 * ensure that NIB storage location, firmware image and bootloader area do not
 * overlap; check if nib is sufficient while changing pairing table size. */
#define NIB_SIZE                    (6 * 1024) /*for ZID and NWK nib *//* bytes
	                                       **/

/* Fixed size of the boot loader area.
 * If the boot loader area size is changed,
 * the linker information needs to be updated as well. */
#define BOOT_LOADER_SIZE            4096 /* bytes */
#if (PAL_TYPE == ATMEGA128RFA1)
#define PAGE_SIZE 256
#endif
#define NIB_FLASH_ADDR              (FLASHEND - BOOT_LOADER_SIZE - NIB_SIZE + 1)

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NODE_CONFIG_H */
/* EOF */
