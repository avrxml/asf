/**
 * \file
 *
 * \brief MAC Layer Configuration
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
