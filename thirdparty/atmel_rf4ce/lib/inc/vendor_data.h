/**
 * @file vendor_data.h
 *
 * @brief Declarations of API functions to access vendor specific data functionality
 *
 * $Id: vendor_data.h 27820 2011-07-24 17:20:51Z pawan.jadia $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

#if ((defined VENDOR_DATA) || (defined DOXYGEN))


/* Prevent double inclusion */
#ifndef VENDOR_DATA_H
#define VENDOR_DATA_H

/* === INCLUDES ============================================================ */

#include <stdio.h>

#include "rf4ce.h"
#include "zrc.h"

/* === MACROS ============================================================== */

/** The maximum size of a vendor-specific data frame; i.e. maximum MSDU - header
   and footer overhead: aMxMACSafePayloadSize - 12 */
#define MAX_NWK_VENDOR_DATA_SIZE    90

/* === TYPES =============================================================== */

/**
 * Enum Ids for vendor-specific commands
 */
typedef enum vendor_cmd_id_tag
{
    BATTERY_STATUS_REQ,
    BATTERY_STATUS_RESP,
    ALIVE_REQ,
    ALIVE_RESP,
    FW_VERSION_REQ,
    FW_VERSION_RESP,
    RX_ON_REQ,
    RX_ON_RESP,
    FW_DATA_REQ,
    FW_DATA_RESP,
    FW_SWAP_REQ,
    FW_SWAP_RESP
} SHORTENUM vendor_cmd_id_t;

/**
 * Structure handling firmware data frames
 */
typedef struct fw_data_frame_tag
{
    vendor_cmd_id_t cmd_id; /**< vendor command id */
    uint16_t frame_cnt; /**< firmware image frame counter */
    uint16_t total_num_frames; /**< total number of frames used for image */
    uint8_t fw_data[0]; /**< actual image data */
} fw_data_frame_t;

/**
 * Enum Ids for vendor status code
 */
typedef enum vendor_status_tag
{
    VD_SUCCESS,
    VD_NOT_SUPPORTED_ATTRIBUTE,
    VD_UNSUPPORTED_SIZE,
    VD_INVALID_CHECKSUM
} SHORTENUM vendor_status_t;

/* === IMPLEMENTATION ====================================================== */


/**
 * @brief Vendor data request
 *
 * The vendor data request allows a device to send a vendor specific data frame.
 *
 * @param PairingRef    Pairing reference of the recipient of the transmission
 * @param ProfileId     Profile id used for the transmission
 * @param VendorId      Vendor id used for the transmission
 * @param nsduLength    Length of the payload data
 * @param nsdu          Pointer to the payload data
 * @param TxOptions     Transit options
 *
 * @return true if command could be handled; else false
 *
 * @ingroup apiRF4CONTROL_VENDOR_DATA_API
 */
bool vendor_data_request(uint8_t PairingRef, profile_id_t ProfileId,
                       uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
                       uint8_t TxOptions);


/**
 * @brief Vendor data indication
 *
 * Callback function indicating incoming vendor data.
 *
 * @param PairingRef    Pairing reference of the sender
 * @param ProfileId     Profile id used for the transmission
 * @param VendorId      Vendor id used for the transmission
 * @param nsduLength    Length of the payload data
 * @param nsdu          Pointer to the payload data
 * @param RxLinkQuality LQI of the incoming frame
 * @param RxFlags       Options used for transmission
 *
 * @return true if command could be handled; else false
 *
 * @ingroup apiRF4CONTROL_VENDOR_DATA_API
 */
void vendor_data_ind(uint8_t PairingRef, profile_id_t ProfileId, uint16_t VendorId,
                     uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
                     uint8_t RxFlags);


/**
 * @brief Vendor data confirm
 *
 * Callback function indicating status of the request
 *
 * @param Status        Status of the request
 * @param PairingRef    Pairing reference used for the transmission
 *
 * @ingroup apiRF4CONTROL_VENDOR_DATA_API
 */
void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef);


/**
 * @brief Vendor data alive request
 *
 * Hook for the application indicating that an alive request command was received.
 *
 * @ingroup apiRF4CONTROL_VENDOR_DATA_API
 */
void vendor_app_alive_req(void);

#endif /* VENDOR_DATA_H */

#endif  /* #ifdef VENDOR_DATA */
