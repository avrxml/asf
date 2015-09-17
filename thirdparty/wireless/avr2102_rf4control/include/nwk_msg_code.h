/**
 * @file nwk_msg_code.h
 *
 * @brief This file defines all message constants.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef NWK_MSG_CODE_H
#define NWK_MSG_CODE_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */

/* === Types ================================================================ */

/**
 * This type contains the service primitives of the Network-layer
 * as well the command-interface primitives
 *
 * DO NOT CHANGE ORDER OF ENUM!
 */
typedef enum nwk_msg_code_tag {
	NLDE_DATA_REQUEST                   = 0x24, /**< */
	NLME_AUTO_DISCOVERY_REQUEST         = 0x25, /**< */
	NLME_DISCOVERY_REQUEST              = 0x26, /**< */
	NLME_DISCOVERY_RESPONSE             = 0x27, /**< */
	NLME_PAIR_REQUEST                   = 0x28, /**< */
	NLME_PAIR_RESPONSE                  = 0x29, /**< */
	NLME_RESET_REQUEST                  = 0x2A, /**< */
	NLME_GET_REQUEST                    = 0x2B, /**< */
	NLME_RX_ENABLE_REQUEST              = 0x2C, /**< */
	NLME_SET_REQUEST                    = 0x2D, /**< */
	NLME_START_REQUEST                  = 0x2E, /**< */
	NLME_UNPAIR_REQUEST                 = 0x2F, /**< */
	NLME_UNPAIR_RESPONSE                = 0x30, /**< */
	NLME_UPDATE_KEY_REQUEST             = 0x31, /**< */
	NWK_CH_AGILITY_REQUEST              = 0x32, /**< */

	MAC_DATA_INDICATION                 = 0x33, /**< */

	NLDE_DATA_INDICATION                = 0x34, /**< */
	NLDE_DATA_CONFIRM                   = 0x35, /**< */
	NLME_AUTO_DISCOVERY_CONFIRM         = 0x36, /**< */
	NLME_COMM_STATUS_INDICATION         = 0x37, /**< */
	NLME_DISCOVERY_INDICATION           = 0x38, /**< */
	NLME_DISCOVERY_CONFIRM              = 0x39, /**< */
	NLME_GET_CONFIRM                    = 0x3A, /**< */
	NLME_PAIR_INDICATION                = 0x3B, /**< */
	NLME_PAIR_CONFIRM                   = 0x3C, /**< */
	NLME_RESET_CONFIRM                  = 0x3D, /**< */
	NLME_RX_ENABLE_CONFIRM              = 0x3E, /**< */
	NLME_SET_CONFIRM                    = 0x3F, /**< */
	NLME_START_CONFIRM                  = 0x40, /**< */
	NLME_UNPAIR_INDICATION              = 0x41, /**< */
	NLME_UNPAIR_CONFIRM                 = 0x42, /**< */
	NLME_UPDATE_KEY_CONFIRM             = 0x43, /**< */
	NWK_CH_AGILITY_INDICATION           = 0x44, /**< */
	NWK_CH_AGILITY_CONFIRM              = 0x45 /**< */
#ifdef PBP_ORG
	,
	PBP_ORG_PAIR_REQUEST                = 0x46, /**< */
	PBP_ORG_PAIR_CONFIRM                = 0x47 /**< */
#endif
#ifdef PBP_REC
	,
	PBP_REC_PAIR_REQUEST                = 0x48, /**< */
	PBP_REC_PAIR_CONFIRM                = 0x49 /**< */
#endif
#ifdef ZRC_PROFILE
	,
	ZRC_CMD_REQUEST                     = 0x4A, /**< */
	ZRC_CMD_INDICATION                  = 0x4B, /**< */
	ZRC_CMD_CONFIRM                     = 0x4C /**< */
#endif
#ifdef ZRC_CMD_DISCOVERY
	,
	ZRC_CMD_DISCOVERY_REQUEST           = 0x4D, /**< */
	ZRC_CMD_DISCOVERY_INDICATION        = 0x4E, /**< */
	ZRC_CMD_DISCOVERY_CONFIRM           = 0x4F, /**< */
	ZRC_CMD_DISCOVERY_RESPONSE          = 0x50 /**< */
#endif
#ifdef VENDOR_DATA
	,
	VENDOR_DATA_REQUEST                 = 0x51, /**< */
	VENDOR_DATA_INDICATION              = 0x52, /**< */
	VENDOR_DATA_CONFIRM                 = 0x53, /**< */
	VENDOR_DATA_RESPONSE                = 0x54 /**< */
#endif
#ifdef ZID_PROFILE
	,
	ZID_REC_CONNECT_REQUEST             = 0x55, /**< */
	ZID_CONNECT_CONFIRM                 = 0x56, /**< */
	ZID_ORG_CONNECT_REQUEST             = 0x57, /**< */
	ZID_REPORT_DATA_REQUEST             = 0x58, /**< */
	ZID_REPORT_DATA_INDICATION          = 0x59, /**< */
	ZID_REPORT_DATA_CONFIRM             = 0x5A, /**< */
	ZID_STANDBY_REQUEST                 = 0x5B, /**< */
	ZID_STANDBY_CONFIRM                 = 0x5C, /**< */
	ZID_STANDBY_LEAVE_INDICATION        = 0x5D, /**< */
	ZID_SET_ATTRIBUTE_REQUEST           = 0x5E, /**< */
	ZID_SET_ATTRIBUTE_CONFIRM           = 0x5F, /**< */
	ZID_GET_ATTRIBUTE_REQUEST           = 0x60, /**< */
	ZID_GET_ATTRIBUTE_CONFIRM           = 0x61, /**< */
	ZID_HEARTBEAT_REQUEST               = 0x62, /**< */
	ZID_HEARTBEAT_INDICATION            = 0x63, /**< */
	ZID_HEARTBEAT_CONFIRM               = 0x64, /**< */
	ZID_SET_NULL_REPORT_REQUEST         = 0x65, /**< */
	ZID_SET_NULL_REPORT_CONFIRM         = 0x66, /**< */
	ZID_SET_REPORT_REQUEST              = 0x67, /**< */
	ZID_SET_REPORT_CONFIRM              = 0x68, /**< */
	ZID_GET_REPORT_REQUEST              = 0x69, /**< */
	ZID_GET_REPORT_CONFIRM              = 0x6a, /**< */
	ZID_GET_REPORT_INDICATION           = 0x6b
#endif
} SHORTENUM nwk_msg_code_t;

/** The following defines describe the minimum length of a primitive message. */
#define NLDE_DATA_REQ_LEN             7 /**< */
#define NLME_AUTO_DISCOVERY_REQ_LEN   16 /**< */
#define NLME_DISCOVERY_REQ_LEN        29 /**< */
#define NLME_DISCOVERY_RESP_LEN       22 /**< */
#define NLME_PAIR_REQ_LEN             24 /**< */
#define NLME_PAIR_RESP_LEN            24 /**< */
#define NLME_RESET_REQ_LEN            2 /**< */
#define NLME_GET_REQ_LEN              3 /**< */
#define NLME_RX_ENABLE_REQ_LEN        5 /**< */
#define NLME_SET_REQ_LEN              4 /**< */
#define NLME_START_REQ_LEN            1 /**< */
#define NLME_UNPAIR_REQ_LEN           2 /**< */
#define NLME_UNPAIR_RESP_LEN          2 /**< */
#define NLME_UPDATE_KEY_REQ_LEN       18 /**< */
#define NWK_CH_AGILITY_REQ_LEN        2 /**< */

#define MAC_DATA_IND_LEN             /**< */

#define NLDE_DATA_IND_LEN               8   /**< */
#define NLDE_DATA_CONF_LEN              4   /**< */
#define NLME_AUTO_DISCOVERY_CONF_LEN    10  /**< */
#define NLME_COMM_STATUS_IND_LEN        14  /**< */
#define NLME_DISCOVERY_IND_LEN          48  /**< */
#define NLME_DISCOVERY_CONF_LEN         4   /**< */
#define NLME_GET_CONF_LEN               5   /**< */
#define NLME_PAIR_IND_LEN               50  /**< */
#define NLME_PAIR_CONF_LEN              38  /**< */
#define NLME_RESET_CONF_LEN             2   /**< */
#define NLME_RX_ENABLE_CONF_LEN         2   /**< */
#define NLME_SET_CONF_LEN               4   /**< */
#define NLME_START_CONF_LEN             2   /**< */
#define NLME_UNPAIR_IND_LEN             2   /**< */
#define NLME_UNPAIR_CONF_LEN            3   /**< */
#define NLME_UPDATE_KEY_CONF_LEN        3   /**< */
#define NWK_CH_AGILITY_IND_LEN          2   /**< */
#define NWK_CH_AGILITY_CONF_LEN         4   /**< */
#define UNSUPPORTED_CMD_LEN             1   /**<*/

#define PBP_ORG_PAIR_REQ_LEN            21
#define PBP_REC_PAIR_REQ_LEN            12
#define ZRC_CMD_REQ_LEN                 7
#define ZRC_CMD_DISCOVERY_REQ_LEN       2
#define ZRC_CMD_DISCOVERY_RESP_LEN      35
#define VENDOR_DATA_REQ_LEN             7

#define PBP_ORG_PAIR_CON_LEN            3
#define PBP_REC_PAIR_CONF_LEN           3
#define ZRC_CMD_IND_LEN                 5
#define ZRC_CMD_CONF_LEN                4
#define ZRC_CMD_DISCOVERY_IND_LEN       2
#define ZRC_CMD_DISCOVERY_CONF_LEN      34
#define VENDOR_DATA_IND_LEN             8
#define VENDOR_DATA_CONF_LEN            3
/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWK_MSG_CODE_H */
/* EOF */
