/**
 * @file pb_pairing.h
 *
 * @brief Declarations of API functions to access push button pairing
 * functionality
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
