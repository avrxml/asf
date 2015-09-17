/**
 * @file node_config.h
 *
 * @brief These are application-specific resources which are used
 *        in the example application of the coordinator in addition to the
 *        underlaying stack.
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
#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

/* === Includes ============================================================= */

#include "nwk_config.h"
#include "rf4ce.h"
#include "zrc.h"

/* === Macros =============================================================== */

/* === Types ================================================================ */

/* Configure RF4CE node capabilities */

/* The capabilities of this node. Implementation specific according to the
 * format
 * illustrated in Figure 26.
 * Implementation specific
 * NodeTypeTarget
 * NodeTypeController
 * PowerSourceMainsTrue
 * PowerSourceMainsFalse
 * SecurityCapableTrue
 * SecurityCapableFalse
 * ChannelNormalizationCapableTrue
 * ChannelNormalizationCapableFalse    */
#ifdef RF4CE_SECURITY
#define NWKC_NODE_CAPABILITIES   (NodeTypeTarget | PowerSourceMainsTrue	\
	| SecurityCapableTrue | ChannelNormalizationCapableTrue)
#else
#define NWKC_NODE_CAPABILITIES   (NodeTypeTarget | PowerSourceMainsTrue	\
	| SecurityCapableFalse | ChannelNormalizationCapableTrue)
#endif

#define NWKC_MAX_PAIRING_TABLE_ENTRIES    (5)
/* #define NWKC_VENDOR_IDENTIFIER           (0x1014) */

/* #define NWKC_VENDOR_STRING               "ATMEL  "          / * 7 characters
 * ! * / */

/* Implementation specific ZRC constant defines */
#define NWK_ACTIVE_PERIOD           nwkcMinActivePeriod /* 16.8 ms */
#define NWK_DUTY_CYCLE              nwkcMaxDutyCycle   /* 62500 symbols = 1s */
#define NWK_DISCOVERY_LQI_THRESHOLD (0x00)

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NODE_CONFIG_H */
/* EOF */
