/**
 * \file
 *
 * \brief MAC Layer Configuration
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

#include "asf.h"
#include "conf_mac.h"

/* Check MAC capabilities */
#ifdef NODE_CAPABLE_CFP
	#pragma message("PRIME_MAC_CFG: NODE_CAPABLE_CFP")
const uint8_t cfgMacCFPCapable = 1;
#else
const uint8_t cfgMacCFPCapable = 0;
#endif

#ifdef NODE_CAPABLE_PRM
	#pragma message("PRIME_MAC_CFG: NODE_CAPABLE_PRM")
const uint8_t cfgMacPRMCapable = 1;
#else
const uint8_t cfgMacPRMCapable = 0;
#endif

#ifdef NODE_CAPABLE_ARQ
	#pragma message("PRIME_MAC_CFG: NODE_CAPABLE_ARQ")
const uint8_t cfgMacARQCapable = 1;
#else
const uint8_t cfgMacARQCapable = 0;
#endif

#ifdef NODE_CAPABLE_DIR_SW
	#pragma message("PRIME_MAC_CFG: NODE_CAPABLE_DIR_SW")
const uint8_t cfgMacDirConSwCapable = 1;
#else
const uint8_t cfgMacDirConSwCapable = 0;
#endif

/* Minimum SNR valid for a registration */
#ifdef NOT_USE_MINIMUM_SNR_VALID
	#pragma message("PRIME_MAC_CFG: NOT_USE_MINIMUM_SNR_VALID")
const uint8_t cfgMacMinSnrValid = 0;
#else
	#pragma message("PRIME_MAC_CFG: USE_MINIMUM_SNR_VALID")
const uint8_t cfgMacMinSnrValid = MAC_MIN_SNR_VALID;
#endif

/* Send PRIME messages through USI */
#if defined(PRIME_SERIAL_SERIALIZATION)
	#pragma message("PRIME_MAC_CFG: PRIME_SERIAL_SERIALIZATION")
const uint8_t cfgPrimeSerialEnable = 1;
#else
const uint8_t cfgPrimeSerialEnable = 0;
#endif

/* CFP reservation in OFDM symbols. Only available for Base Node */
#if defined(MAC_CFP_RESERVE)
	#pragma message("PRIME_MAC_CFG: MAC_CFP_SYMBOLS_RESERVED")
const uint8_t cfgMacCfpReserved = MAC_CFP_SYMBOLS_RESERVED;
#else
const uint8_t cfgMacCfpReserved = 0;
#endif

const uint8_t cfgMacEncryptSp1 = 0;
uint8_t const *cfgMacAesPtr = NULL;
