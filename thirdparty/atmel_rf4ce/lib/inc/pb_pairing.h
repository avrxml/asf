/**
 * @file pb_pairing.h
 *
 * @brief Declarations of API functions to access push button pairing functionality
 *
 * $Id: pb_pairing.h 24049 2010-10-29 14:20:25Z uwalter $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

#if ((defined PB_PAIRING) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef PB_PAIRING_H
#define PB_PAIRING_H


/* === Includes ============================================================= */

#include "rf4ce.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */


/* Target */


/**
 * @brief Recipient pair request; target use
 *
 * The push button pairing recipient pair request allows a target to start the
 * push button pairing procedure; auto-discovery and incoming pairing handling.
 *
 * @param RecAppCapabilities    Application capabilities of the device issuing
 *                              the request
 * @param RecDevTypeList        Supported device type list
 * @param RecProfileIdList      Supported profile id list
 *
 * @return true if command could be handled; else false
 *
 * @ingroup apiRF4CONTROL_PBP_API
 */
#if ((defined RF4CE_PLATFORM) || (defined RF4CE_TARGET) || (defined DOXYGEN))
bool pbp_rec_pair_request(uint8_t RecAppCapabilities,
                          dev_type_t RecDevTypeList[3],
                          profile_id_t RecProfileIdList[7]);
#endif


/**
 * @brief Allow pairing from application; target use
 *
 * The allow pairing function provides information about the incoming pairing
 * request. It is a hook to handle application specific requirements during the
 * pairing procedure.
 *
 * @param Status            Status of the pair indication; here NWK_SUCCESS or
                            NWK_DUPLICATE_PAIRING
 * @param SrcIEEEAddr       IEEE address of the device requesting the pair
 * @param OrgVendorId       Vendor id  of the device requesting the pair
 * @param OrgVendorString   Vendor string of the device requesting the pair
 * @param OrgUserString     User string of the device requesting the pair
 * @param KeyExTransferCount Key exchange transfer count of the incoming pair request
 *
 * @return true if pairing is granted; else false
 *
 * @ingroup apiRF4CONTROL_PBP_API
 */
#if ((defined RF4CE_PLATFORM) || (defined RF4CE_TARGET) || (defined DOXYGEN))
bool pbp_allow_pairing(nwk_enum_t Status, uint64_t SrcIEEEAddr,
                      uint16_t OrgVendorId, uint8_t OrgVendorString[7],
                      uint8_t OrgUserString[15], uint8_t KeyExTransferCount);
#endif


/* Controller */


/**
 * @brief Originator push pairing request; controller use
 *
 * The push button pairing request allows a controller to start the push pairing
 * procedure; discovery and pair request handling.
 *
 * @param OrgAppCapabilities    Application capabilities of the device issuing
 *                              the request
 * @param OrgDevTypeList        Supported device type list
 * @param OrgProfileIdList      Supported profile id list
 * @param SearchDevType         Device type that should be searched for
 * @param DiscProfileIdListSize Profile id list size
 * @param DiscProfileIdList     Profile id list
 *
 * @return true if command could be handled; else false
 *
 * @ingroup apiRF4CONTROL_PBP_API
 */
#if (((!defined RF4CE_PLATFORM) && (!defined RF4CE_TARGET)) || (defined DOXYGEN))
bool pbp_org_pair_request(uint8_t OrgAppCapabilities,
                          dev_type_t OrgDevTypeList[3],
                          profile_id_t OrgProfileIdList[7],
                          dev_type_t SearchDevType, uint8_t DiscProfileIdListSize,
                          profile_id_t DiscProfileIdList[7]);
#endif


/**
 * @brief Push button pairing confirm; target and controller use
 *
 * The push button pairing confirm is a callback that provides the status of the
 * push pairing request.
 *
 * @param Status        Status of the last pairing transaction
 * @param PairingRef    If pairing was successful, PairingRef contains assigned
 *                      pairing reference.
 *
 * @ingroup apiRF4CONTROL_PBP_API
 */
void pbp_pair_confirm(nwk_enum_t Status, uint8_t PairingRef);



#endif /* PB_PAIRING_H */

#endif  /* #ifdef PB_PAIRING */

/* EOF */
