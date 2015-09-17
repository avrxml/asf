/**
 *
 * @file mac_build_config.h
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

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

/* === Includes ============================================================= */

#ifdef MAC_USER_BUILD_CONFIG
#include "mac_user_build_config.h"
#else

/* === Macros =============================================================== */

/**
 * \ingroup group_inc
 * \defgroup group_mac_build MAC Build Configurations
 * All Build configurations Macros are defined in this module
 * @{
 */

/*
 * Standard MAC configurations according to IEEE 802.15.4:
 * (1) FFD vs. RFD build
 * (2) Pure Nonbeacon-build vs. build including support of Beacon-enabled
 *     networks
 */
#ifdef FFD

/**
 * FEATURE SET FOR FFD BUILD
 */
#define MAC_ASSOCIATION_INDICATION_RESPONSE     (1)
#define MAC_ASSOCIATION_REQUEST_CONFIRM         (1)
#define MAC_BEACON_NOTIFY_INDICATION            (1)
#define MAC_DISASSOCIATION_BASIC_SUPPORT        (1)
#define MAC_DISASSOCIATION_FFD_SUPPORT          (1)
#define MAC_GET_SUPPORT                         (1)
#define MAC_INDIRECT_DATA_BASIC                 (1)
#define MAC_INDIRECT_DATA_FFD                   (1)
#define MAC_ORPHAN_INDICATION_RESPONSE          (1)
#define MAC_PAN_ID_CONFLICT_AS_PC               (1)
#define MAC_PAN_ID_CONFLICT_NON_PC              (1)
#define MAC_PURGE_REQUEST_CONFIRM               (1)
#define MAC_RX_ENABLE_SUPPORT                   (1)
#define MAC_SCAN_ACTIVE_REQUEST_CONFIRM         (1)
#define MAC_SCAN_ED_REQUEST_CONFIRM             (1)
#define MAC_SCAN_ORPHAN_REQUEST_CONFIRM         (1)
#ifdef BEACON_SUPPORT
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (1)
#else
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (0) /* No Passive Scan in
	                                             * nonbeacon networks */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */
#define MAC_START_REQUEST_CONFIRM               (1)
#else

/**
 * FEATURE SET FOR RFD BUILD
 */
#define MAC_ASSOCIATION_INDICATION_RESPONSE     (0) /* No acceptance of
	                                             * association FROM other
	                                             * nodes */
#define MAC_ASSOCIATION_REQUEST_CONFIRM         (1)
#define MAC_BEACON_NOTIFY_INDICATION            (1)
#define MAC_DISASSOCIATION_BASIC_SUPPORT        (1)
#define MAC_DISASSOCIATION_FFD_SUPPORT          (0) /* No indirect tx of
	                                             * disassocation
	                                             * notification frames */
#define MAC_GET_SUPPORT                         (1)
#define MAC_INDIRECT_DATA_BASIC                 (1)
#define MAC_INDIRECT_DATA_FFD                   (0) /* No Transmission of
	                                             * indirect data */
#define MAC_ORPHAN_INDICATION_RESPONSE          (0) /* No Handling of Orphan
	                                             * Notification */
#define MAC_PAN_ID_CONFLICT_AS_PC               (0) /* No PAN-Id conflict
	                                             * detection as PAN
	                                             * Coordinator */
#define MAC_PAN_ID_CONFLICT_NON_PC              (1)
#define MAC_PURGE_REQUEST_CONFIRM               (0) /* No Purging of indirect
	                                             * data */
#define MAC_RX_ENABLE_SUPPORT                   (1)
#define MAC_SCAN_ACTIVE_REQUEST_CONFIRM         (1)
#define MAC_SCAN_ED_REQUEST_CONFIRM             (0) /* No Energy Detect Scan */
#define MAC_SCAN_ORPHAN_REQUEST_CONFIRM         (1)
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (1)
#define MAC_START_REQUEST_CONFIRM               (0) /* No Start or realignment
	                                             * of networks */
#endif  /* FFD / RFD */

#ifdef BEACON_SUPPORT

/**
 * FEATURE SET FOR BUILD WITH BEACON ENABLED NETWORK SUPPORT
 */
#define MAC_SYNC_REQUEST                        (1)
#else

/**
 * FEATURE SET FOR BUILD WITH PURE NONBEACON ONLY NETWORK SUPPORT
 */
#define MAC_SYNC_REQUEST                        (0) /* No Tracking of beacon
	                                             * frames for nonbeacon
	                                             * networks */
#endif /* BEACON_SUPPORT / No BEACON_SUPPORT */

#if (defined BEACON_SUPPORT) && (defined GTS_SUPPORT)

/**
 * FEATURE SET FOR BUILD WITH BEACON ENABLED NETWORK SUPPORT
 */
#define MAC_GTS_REQUEST                        (1)
#else

/**
 * FEATURE SET FOR BUILD WITH PURE NONBEACON ONLY NETWORK SUPPORT
 */
#define MAC_GTS_REQUEST                        (0)

#endif /* BEACON_SUPPORT / No BEACON_SUPPORT */

/*
 * Sync Loss Indication primitive is always required:
 *
 * In Beacon-enabled networks this is required in case
 * MAC_SYNC_REQUEST is set to 1.
 *
 * In Beacon- and Nonbeacon-enabled networks this is required in
 * case the peer node, acting as coordinator has MAC_START_REQUEST_CONFIRM
 * set to 1.
 */
#define MAC_SYNC_LOSS_INDICATION                    (1)

/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */
/* ! @} */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* #ifdef MAC_USER_BUILD_CONFIG */

/*
 * The following defines are general and thus are also valid if a
 * user_build_config.h file was available.
 */

/*
 * Communication Status Indication primitive is only required for an FDD
 * handling also Association Response or Orphan Response Primitive.
 */
#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))
#define MAC_COMM_STATUS_INDICATION              (1)
#else
#define MAC_COMM_STATUS_INDICATION              (0)
#endif

/*
 * In case any of the scan features is required, the general portions
 * supporting scanning have to be enabled.
 */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
#define MAC_SCAN_SUPPORT                        (1)
#else
#define MAC_SCAN_SUPPORT                        (0)
#endif

/*
 * Some sanity checks are done here to prevent having configurations
 * that might violate the standard or lead to undefined behaviour
 * (e.g. by having unreasonable combinations in file user_build_config.h).
 */

/*
 * 1) If Sync Request is used, also Sync Loss Indication is required.
 */
#if ((MAC_SYNC_REQUEST == 1) && (MAC_SYNC_LOSS_INDICATION == 0))
#error  ("MAC_SYNC_LOSS_INDICATION needs to be set to 1 in \
	user_build_config.h since MAC_SYNC_REQUEST is 1")
#endif

/*
 * 2) If Association Response is used, also Indirect Data Tx Support is
 * required.
 */
#if ((MAC_ASSOCIATION_INDICATION_RESPONSE == 1) && (MAC_INDIRECT_DATA_FFD == 0))
#error  ("MAC_INDIRECT_DATA_FFD needs to be set to 1 in	\
	user_build_config.h since MAC_ASSOCIATION_INDICATION_RESPONSE is 1")
#endif

/*
 * 3) If Association Request is used, also Indirect Data Rx Support is required.
 */
#if ((MAC_ASSOCIATION_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_ASSOCIATION_REQUEST_CONFIRM is 1")
#endif

/*
 * 4) If Disassociation is used as basic feature, also Indirect Data Rx Support
 *    is required.
 */
#if ((MAC_DISASSOCIATION_BASIC_SUPPORT == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_DISASSOCIATION_BASIC_SUPPORT is 1")
#endif

/*
 * 5) If Disassociation is used as FFD (allow tx of indirect disassociation
 * frames),
 *    also Indirect Data Rx Support is required.
 */
#if ((MAC_DISASSOCIATION_FFD_SUPPORT == 1) && (MAC_INDIRECT_DATA_FFD == 0))
#error  ("MAC_INDIRECT_DATA_FFD needs to be set to 1 in	\
	user_build_config.h since MAC_DISASSOCIATION_FFD_SUPPORT is 1")
#endif

/*
 * 6) If Purging is used, also Indirect Data Tx Support is required.
 */
#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_FFD == 0))
#error  ("MAC_INDIRECT_DATA_FFD needs to be set to 1 in	\
	user_build_config.h since MAC_PURGE_REQUEST_CONFIRM is 1")
#endif

/*
 * 7) If Indirect Data Tx Support is used, also Indirect Data Rx Support is
 * required.
 */
#if ((MAC_INDIRECT_DATA_FFD == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_INDIRECT_DATA_FFD is 1")
#endif

/*
 * 8) If Disassociation is used as FFD (allow tx of indirect disassociation
 * frames),
 *    also Disassociation as basic feature is required.
 */
#if ((MAC_DISASSOCIATION_FFD_SUPPORT == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_DISASSOCIATION_FFD_SUPPORT is 1")
#endif

/*
 * 9) If Sync Request is used, BEACON_SUPPORT needs to be defined.
 */
#if (MAC_SYNC_REQUEST == 1)
#ifndef BEACON_SUPPORT
#error \
	("BEACON_SUPPORT needs to be set define in the makefile since MAC_SYNC_REQUEST is 1")
#endif
#endif /* (MAC_SYNC_REQUEST == 1) */

/*
 * 10) If PAN-Id conflict detection on the PAN Coordinator side is used,
 *     also Sync Loss Indication is required.
 */
#if ((MAC_PAN_ID_CONFLICT_AS_PC == 1) && (MAC_SYNC_LOSS_INDICATION == 0))
#error  ("MAC_SYNC_LOSS_INDICATION needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_AS_PC is 1")
#endif

/*
 * 11) If PAN-Id conflict detection on the PAN Coordinator side is used,
 *     also Start Request is required.
 */
#if ((MAC_PAN_ID_CONFLICT_AS_PC == 1) && (MAC_START_REQUEST_CONFIRM == 0))
#error  ("MAC_START_REQUEST_CONFIRM needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_AS_PC is 1")
#endif

/*
 * 12) If PAN-Id conflict detection not as PAN Coordinator is used,
 *     also Sync Loss Indication is required.
 */
#if ((MAC_PAN_ID_CONFLICT_NON_PC == 1) && (MAC_SYNC_LOSS_INDICATION == 0))
#error  ("MAC_SYNC_LOSS_INDICATION needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_NON_PC is 1")
#endif

/*
 * 13) If PAN-Id conflict detection not as PAN Coordinator is used,
 *     either Association Request or Sync Request is required.
 */
#if ((MAC_PAN_ID_CONFLICT_NON_PC == 1) && \
	((MAC_ASSOCIATION_REQUEST_CONFIRM == 0) && (MAC_SYNC_REQUEST == 0)) \
)
#error \
	("MAC_ASSOCIATION_REQUEST_CONFIRM or MAC_SYNC_REQUEST needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_NON_PC is 1")
#endif

#if (defined GTS_SUPPORT) && (!defined BEACON_SUPPORT)
#error \
	"GTS can be used along with Beacon mode only. Add BEACON_SUPPORT symbol..."
#endif /* (defined GTS_SUPPORT) && (!defined BEACON_SUPPORT) */

#endif  /* BUILD_CONFIG_H */
/* EOF */
