/**
 * @file mac_msg_const.h
 *
 * @brief This file defines all message constants.
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
#ifndef MAC_MSG_CONST_H
#define MAC_MSG_CONST_H

/* === Includes ============================================================= */
#include <stdint.h>
#include <stdbool.h>

/* #include "stack_config.h" */
/* === Macros =============================================================== */

/**
 * \addtogroup group_mac_def
 * @{
 */

#define MAC                                 (4)

/**
 * This type contains the service primitives of the PHY-, MAC- and Network-layer
 * as well the command-interface primitives
 */
enum msg_code {
	TAL_DATA_INDICATION                 = (0x00),

	MLME_ASSOCIATE_REQUEST              = (0x01),
	MLME_ASSOCIATE_RESPONSE             = (0x02),

	MCPS_DATA_REQUEST                   = (0x03),
	MCPS_PURGE_REQUEST                  = (0x04),

	MLME_DISASSOCIATE_REQUEST           = (0x05),
#if (HIGHEST_STACK_LAYER == MAC)
	MLME_SET_REQUEST                    = (0x06),
#endif /* (HIGHEST_STACK_LAYER == MAC) */
	MLME_ORPHAN_RESPONSE                = (0x07),
	MLME_GET_REQUEST                    = (0x08),
	MLME_RESET_REQUEST                  = (0x09),
	MLME_RX_ENABLE_REQUEST              = (0x0A),
	MLME_SCAN_REQUEST                   = (0x0B),
	MLME_GTS_REQUEST                    = (0x0C),
	MLME_START_REQUEST                  = (0x0D),
	MLME_POLL_REQUEST                   = (0x0E),
	MLME_SYNC_REQUEST                   = (0x0F),

	MCPS_DATA_CONFIRM                   = (0x10),
	MCPS_DATA_INDICATION                = (0x11),
	MCPS_PURGE_CONFIRM                  = (0x12),

	MLME_ASSOCIATE_INDICATION           = (0x13),
	MLME_ASSOCIATE_CONFIRM              = (0x14),
	MLME_DISASSOCIATE_INDICATION        = (0x15),
	MLME_DISASSOCIATE_CONFIRM           = (0x16),
	MLME_BEACON_NOTIFY_INDICATION       = (0x17),
	MLME_GTS_CONFIRM                    = (0x18),
	MLME_GTS_INDICATION                 = (0x19),
	MLME_ORPHAN_INDICATION              = (0x1A),
	MLME_SCAN_CONFIRM                   = (0x1B),
	MLME_COMM_STATUS_INDICATION         = (0x1C),
	MLME_SYNC_LOSS_INDICATION           = (0x1D),
	MLME_GET_CONFIRM                    = (0x1E),
#if (HIGHEST_STACK_LAYER == MAC)
	MLME_SET_CONFIRM                    = (0x1F),
#endif /* (HIGHEST_STACK_LAYER == MAC) */
	MLME_RESET_CONFIRM                  = (0x20),
	MLME_RX_ENABLE_CONFIRM              = (0x21),
	MLME_START_CONFIRM                  = (0x22),
	MLME_POLL_CONFIRM                   = (0x23),
	MAC_INC_FRAME                       = (0x24)
}
SHORTENUM;

/**
 * Bump this when extending the list!
 */
#define LAST_MESSAGE                    MLME_POLL_CONFIRM

/** The following defines describe the minimum length of a primitive message. */
#ifdef ENABLE_TSTAMP
#define MCPS_DATA_CON_LEN               (7)
#define MCPS_DATA_IND_LEN               (31)
#else   /* ENABLE_TSTAMP not enabled */
#define MCPS_DATA_CON_LEN               (7 - 4)
#define MCPS_DATA_IND_LEN               (31 - 4)
#endif  /* ENABLE_TSTAMP */
#define MCPS_PURGE_CONF_LEN             (3)

#define MLME_ASSOCIATE_CONF_LEN         (4)
#define MLME_ASSOCIATE_IND_LEN          (10)
#define MLME_BEACON_NOTIFY_IND_LEN      (2)
#define MLME_COMM_STATUS_IND_LEN        (22)
#define MLME_DISASSOCIATE_CONF_LEN      (13)
#define MLME_DISASSOCIATE_IND_LEN       (10)
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#define MLME_GET_CONF_LEN               (5)
#else
#define MLME_GET_CONF_LEN               (4)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
#define MLME_ORPHAN_IND_LEN             (9)
#define MLME_POLL_CONF_LEN              (2)
#define MLME_RESET_CONF_LEN             (2)
#define MLME_RX_ENABLE_CONF_LEN         (2)
#define MLME_SCAN_CONF_LEN              (10)
#if ((defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006))
#define MLME_SET_CONF_LEN               (4)
#else
#define MLME_SET_CONF_LEN               (3)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
#define MLME_START_CONF_LEN             (2)
#define MLME_SYNC_LOSS_IND_LEN          (6)
#define MLME_GTS_CONF_LEN               (3)
#define MLME_GTS_IND_LEN                (4)

#ifdef ENABLE_TSTAMP
#define WPAN_DESCRIPTOR_LEN             (21)
#else   /* ENABLE_TSTAMP not enabled */
#define WPAN_DESCRIPTOR_LEN             (21 - 4)
#endif  /* ENABLE_TSTAMP */
/* ! @} */
/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAC_MSG_CONST_H */
/* EOF */
