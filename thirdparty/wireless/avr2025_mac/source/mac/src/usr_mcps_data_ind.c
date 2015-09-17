/**
 * @file usr_mcps_data_ind.c
 *
 * @brief This file contains user call back function for MCPS-DATA.indication.
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

/* === Includes ============================================================= */

#include <stdint.h>
#include <stdbool.h>
#include "mac_api.h"

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */
#if (defined __GNUC__)
void __attribute__((weak))
#ifdef MAC_SECURITY_ZIP
usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN,
    #ifdef ENABLE_TSTAMP
		uint32_t Timestamp,
    #endif  /* ENABLE_TSTAMP */
		uint8_t SecurityLevel,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else   /* No MAC_SECURITY */
usr_mcps_data_ind(wpan_addr_spec_t * SrcAddrSpec,
		wpan_addr_spec_t * DstAddrSpec,
		uint8_t msduLength,
		uint8_t * msdu,
		uint8_t mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
		uint8_t DSN,
		uint32_t Timestamp)
    #else
		uint8_t DSN)
    #endif  /* ENABLE_TSTAMP */
#endif  /* MAC_SECURITY */
#else
#ifdef MAC_SECURITY_ZIP
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN,
    #ifdef ENABLE_TSTAMP
		uint32_t Timestamp,
    #endif  /* ENABLE_TSTAMP */
		uint8_t SecurityLevel,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else   /* No MAC_SECURITY */
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
		uint8_t DSN,
		uint32_t Timestamp)
    #else
		uint8_t DSN)
    #endif  /* ENABLE_TSTAMP */
#endif  /* MAC_SECURITY */
#endif
{
	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
	msduLength = msduLength;
	msdu = msdu;
	mpduLinkQuality = mpduLinkQuality;
	DSN = DSN;
#ifdef ENABLE_TSTAMP
	Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
#ifdef MAC_SECURITY_ZIP
	SecurityLevel = SecurityLevel;
	KeyIdMode = KeyIdMode;
	KeyIndex = KeyIndex;
#endif  /* MAC_SECURITY */
}

/* EOF */
