/**
 * @file nwk_build_config.h
 *
 * @brief This header file declares macros for various build configurations
 *
 * $Id: nwk_build_config.h 23558 2010-09-24 09:18:45Z uwalter $
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
#ifndef NWK_BUILD_CONFIG_H
#define NWK_BUILD_CONFIG_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */


/* NWK primitive configuration */

#ifdef NWK_USER_BUILD_CONFIG
#include "nwk_user_build_config.h"
#else   /* Include all NWK primitives */
#define NWK_SET                                 (1)
#define NWK_GET                                 (1)
#define NWK_UNPAIR_REQ_CONF                     (1)
#define NWK_UNPAIR_IND_RESP                     (1)
#define NWK_UPDATE_KEY                          (1)
#define NWK_DISCOVERY_REQ_CONF                  (1)
#define NWK_DISCOVERY_IND_RESP                  (1)
#define NWK_PAIR_REQ_CONF                       (1)
#define NWK_PAIR_IND_RESP                       (1)
#endif


/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif



#endif  /* NWK_BUILD_CONFIG_H */
/* EOF */

