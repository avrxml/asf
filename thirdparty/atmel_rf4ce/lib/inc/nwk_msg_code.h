/**
 * @file nwk_msg_code.h
 *
 * @brief This file defines all message constants.
 *
 * $Id: nwk_msg_code.h 27820 2011-07-24 17:20:51Z pawan.jadia $
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
typedef enum nwk_msg_code_tag
{
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
#ifdef PB_PAIRING
    ,
    PBP_REC_PAIR_REQUEST                = 0x46, /**< */
    PBP_ORG_PAIR_REQUEST                = 0x47, /**< */
    PBP_PAIR_CONFIRM                    = 0x48 /**< */
#endif
#ifdef ZRC_PROFILE
    ,
    ZRC_CMD_REQUEST                     = 0x49, /**< */
    ZRC_CMD_INDICATION                  = 0x4A, /**< */
    ZRC_CMD_CONFIRM                     = 0x4B /**< */
#endif
#ifdef ZRC_CMD_DISCOVERY
    ,
    ZRC_CMD_DISCOVERY_REQUEST           = 0x4C, /**< */
    ZRC_CMD_DISCOVERY_INDICATION        = 0x4D, /**< */
    ZRC_CMD_DISCOVERY_CONFIRM           = 0x4E, /**< */
    ZRC_CMD_DISCOVERY_RESPONSE          = 0x4F /**< */
#endif
#ifdef VENDOR_DATA
    ,
    VENDOR_DATA_REQUEST                 = 0x50, /**< */
    VENDOR_DATA_INDICATION              = 0x51, /**< */
    VENDOR_DATA_CONFIRM                 = 0x52, /**< */
    VENDOR_DATA_RESPONSE                = 0x53 /**< */
#endif
} SHORTENUM nwk_msg_code_t;


/** The following defines describe the minimum length of a primitive message. */
#define NLDE_DATA_REQ_LEN            /**< */
#define NLME_AUTO_DISCOVERY_REQ_LEN  /**< */
#define NLME_DISCOVERY_REQ_LEN       /**< */
#define NLME_DISCOVERY_RESP_LEN      /**< */
#define NLME_PAIR_REQ_LEN            /**< */
#define NLME_PAIR_RESP_LEN           /**< */
#define NLME_RESET_REQ_LEN           /**< */
#define NLME_GET_REQ_LEN             /**< */
#define NLME_RX_ENABLE_REQ_LEN       /**< */
#define NLME_SET_REQ_LEN             /**< */
#define NLME_START_REQ               /**< */
#define NLME_UNPAIR_REQ_LEN          /**< */
#define NLME_UNPAIR_RESP_LEN         /**< */
#define NLME_UPDATE_KEY_REQ_LEN      /**< */
#define NWK_CH_AGILITY_REQ_LEN       /**< */

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
