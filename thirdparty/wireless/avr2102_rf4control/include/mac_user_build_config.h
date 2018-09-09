/**
 * @file mac_user_build_config.h
 *
 * @brief This header file declares macros for various build configurations
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

/* Prevent double inclusion */
#ifndef MAC_USER_BUILD_CONFIG_H
#define MAC_USER_BUILD_CONFIG_H

/* === Includes ============================================================= */

/* === Types ================================================================ */

/* === Macros =============================================================== */

/**
 * \ingroup group_rf4control
 * \defgroup group_mac_user_build MAC User Build Configurations
 * All Build configuration Macros are defined in this module
 * @{
 */
/* MAC primitive configuration */

#ifdef MAC_USER_BUILD_CONFIG

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)

#define MAC_ASSOCIATION_INDICATION_RESPONSE     (0)
#define MAC_ASSOCIATION_REQUEST_CONFIRM         (0)
#define MAC_BEACON_NOTIFY_INDICATION            (1)
#define MAC_DISASSOCIATION_BASIC_SUPPORT        (0)
#define MAC_DISASSOCIATION_FFD_SUPPORT          (0)
#define MAC_GET_SUPPORT                         (0)
#define MAC_INDIRECT_DATA_BASIC                 (0)
#define MAC_INDIRECT_DATA_FFD                   (0)
#define MAC_ORPHAN_INDICATION_RESPONSE          (0)
#define MAC_PAN_ID_CONFLICT_AS_PC               (0)
#define MAC_PAN_ID_CONFLICT_NON_PC              (0)
#define MAC_PURGE_REQUEST_CONFIRM               (0)
#define MAC_RX_ENABLE_SUPPORT                   (1)
#define MAC_SCAN_ACTIVE_REQUEST_CONFIRM         (1)
#define MAC_SCAN_ED_REQUEST_CONFIRM             (1)
#define MAC_SCAN_ORPHAN_REQUEST_CONFIRM         (0)
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (0)
#define MAC_START_REQUEST_CONFIRM               (1)
#define MAC_SYNC_LOSS_INDICATION                (0)
#define MAC_SYNC_REQUEST                        (0)
#define MAC_GTS_REQUEST                         (0)

#else   /* controller */

#define MAC_ASSOCIATION_INDICATION_RESPONSE     (0)
#define MAC_ASSOCIATION_REQUEST_CONFIRM         (0)
#define MAC_BEACON_NOTIFY_INDICATION            (0)
#define MAC_DISASSOCIATION_BASIC_SUPPORT        (0)
#define MAC_DISASSOCIATION_FFD_SUPPORT          (0)
#define MAC_GET_SUPPORT                         (0)
#define MAC_INDIRECT_DATA_BASIC                 (0)
#define MAC_INDIRECT_DATA_FFD                   (0)
#define MAC_ORPHAN_INDICATION_RESPONSE          (0)
#define MAC_PAN_ID_CONFLICT_AS_PC               (0)
#define MAC_PAN_ID_CONFLICT_NON_PC              (0)
#define MAC_PURGE_REQUEST_CONFIRM               (0)
#define MAC_RX_ENABLE_SUPPORT                   (1)
#define MAC_SCAN_ACTIVE_REQUEST_CONFIRM         (0)
#define MAC_SCAN_ED_REQUEST_CONFIRM             (0)
#define MAC_SCAN_ORPHAN_REQUEST_CONFIRM         (0)
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (0)
#define MAC_START_REQUEST_CONFIRM               (0)
#define MAC_SYNC_LOSS_INDICATION                (0)
#define MAC_SYNC_REQUEST                        (0)
#define MAC_GTS_REQUEST                         (0)

#endif

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */
/* ! @} */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* MAC_USER_BUILD_CONFIG_H */

#endif /* #ifdef MAC_USER_BUILD_CONFIG */

/* EOF */
