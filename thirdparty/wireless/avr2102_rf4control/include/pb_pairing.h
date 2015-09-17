/**
 * @file pb_pairing.h
 *
 * @brief Declarations of API functions to access push button pairing
 * functionality
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
 *
 */

#if (defined PBP_ORG) || (defined PBP_REC) || (defined DOXYGEN)

/* Prevent double inclusion */
#ifndef PB_PAIRING_H
#define PB_PAIRING_H

/* === Includes ============================================================= */

#include "rf4ce.h"
#ifdef ZID_PROFILE
#include "zid.h"
#endif

/* === Macros =============================================================== */

/* === Types ================================================================ */

#if (defined PBP_ORG)
typedef void (*pbp_org_pair_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);
#endif
#if (defined PBP_REC)
typedef void (*pbp_rec_pair_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);
#endif

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

/* Recipient of the pairing */

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
 * @param confirm_cb            Call back pointer for the confirmation
 *
 * @return true if command could be handled; else false
 *
 * @ingroup group_RF4CONTROL_PBP_API
 */
#if (defined PBP_REC) || (defined DOXYGEN)
bool pbp_rec_pair_request(uint8_t RecAppCapabilities,
dev_type_t RecDevTypeList[3],
profile_id_t RecProfileIdList[7],
FUNC_PTR confirm_cb
);
#endif

/**
 * @brief Allow pairing from application; target use
 *
 * The allow pairing function provides information about the incoming pairing
 * request. It is a hook to handle application specific requirements during the
 * pairing procedure.
 *
 * @param Status            Status of the pair indication; here NWK_SUCCESS or
 *                          NWK_DUPLICATE_PAIRING
 * @param SrcIEEEAddr       IEEE address of the device requesting the pair
 * @param OrgVendorId       Vendor id  of the device requesting the pair
 * @param OrgVendorString   Vendor string of the device requesting the pair
 * @param OrgUserString     User string of the device requesting the pair
 * @param KeyExTransferCount Key exchange transfer count of the incoming pair
 * request
 *
 * @return true if pairing is granted; else false
 *
 * @ingroup group_RF4CONTROL_PBP_API
 */
#if (defined PBP_REC) || (defined DOXYGEN)
bool pbp_allow_pairing(nwk_enum_t Status, uint64_t SrcIEEEAddr,
uint16_t OrgVendorId, uint8_t OrgVendorString[7],
uint8_t OrgUserString[15], uint8_t KeyExTransferCount);
#endif

/* Originator of the pairing */

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
 * @param confirm_cb            Call back pointer for the confirmation
 *
 * @return true if command could be handled; else false
 *
 * @ingroup group_RF4CONTROL_PBP_API
 */
#if (defined PBP_ORG) || (defined DOXYGEN)
bool pbp_org_pair_request(uint8_t OrgAppCapabilities,
dev_type_t OrgDevTypeList[3],
profile_id_t OrgProfileIdList[7],
dev_type_t SearchDevType, uint8_t DiscProfileIdListSize,
profile_id_t DiscProfileIdList[7],
FUNC_PTR confirm_cb
);
#endif

#endif /* PB_PAIRING_H */

#endif  /* #if ((defined PBP_ORG) || (defined PBP_REC) || (defined DOXYGEN)) */

/* EOF */
