/**
 * @file nwk_build_config.h
 *
 * @brief This header file declares macros for various build configurations.
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
