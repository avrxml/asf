/**
 * @file nwk_build_config.h
 *
 * @brief This header file declares macros for various build configurations.
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
