/**
 * @file mac_user_build_config.h
 *
 * @brief This header file declares macros for various build configurations
 *
 * $Id: mac_user_build_config.h 20338 2010-02-13 22:43:25Z uwalter $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

/* Prevent double inclusion */
#ifndef MAC_USER_BUILD_CONFIG_H
#define MAC_USER_BUILD_CONFIG_H

/* === Includes ============================================================= */

/* === Types ================================================================ */

/* === Macros =============================================================== */

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

#endif

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif



#endif  /* MAC_USER_BUILD_CONFIG_H */

#endif /* #ifdef MAC_USER_BUILD_CONFIG */

/* EOF */

