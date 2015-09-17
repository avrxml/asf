/**
 * @file mac_user_build_config.h
 *
 * @brief This header file declares macros for various build configurations
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
