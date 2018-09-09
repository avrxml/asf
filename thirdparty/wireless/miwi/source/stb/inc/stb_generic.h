/**
* \file  stb_generic.h
*
* \brief Declarations for security tool box
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

/* Prevent double inclusion */
#ifndef STB_GENERIC_H
#define STB_GENERIC_H

#if defined(STB_ON_SAL) || defined(STB_ARMCRYPTO) || defined(__DOXYGEN__)

/* === Includes =========================================================== */
#include "compiler.h"
#include "sal_generic.h"

/* === Macros ============================================================= */

/* === Types ============================================================== */

/**
 * CCM status values
 */
typedef enum {
	/** CCM en/decryption OK */
	STB_CCM_OK = 0,
	/** Illegal parameter value in stb_ccm_secure() */
	STB_CCM_ILLPARM,
	/** No previous key init in stb_ccm_secure() */
	STB_CCM_KEYMISS,
	/** MIC error detected in stb_ccm_secure() */
	STB_CCM_MICERR
} SHORTENUM stb_ccm_t;

typedef enum {
	/** Security Level 0 */
	SECURITY_00_LEVEL = 0x00,
	/** Security Level 1 */
	SECURITY_01_LEVEL,
	/** Security Level 2 */
	SECURITY_02_LEVEL,
	/** Security Level 3 */
	SECURITY_03_LEVEL,
	/** Security Level 4 */
	SECURITY_04_LEVEL,
	/** Security Level 5 */
	SECURITY_05_LEVEL,
	/** Security Level 6 */
	SECURITY_06_LEVEL,
	/** Security Level 7 */
	SECURITY_07_LEVEL
} SHORTENUM security_level_t;

/** Macros for MIC Calculation and Security */
#define ENCRYPTION_NOT_REQD     (0x00)
#define ENCRYPTION_REQD         (0x01)
#define PLAINTEXT_FLAG          (0x01)
#define LEN_FIELD                       (0x01)
#define ADATA                           (0x40)

#define LEN_MIC_00                      (0x00)
#define LEN_MIC_32                      (0x04)
#define LEN_MIC_64                      (0x08)
#define LEN_MIC_128                     (0x10)

/* === Externals ========================================================== */

/* === Prototypes ========================================================= */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief STB Initialization
 *
 * This function initializes the STB.
 *
 * @ingroup group_StbApi
 */
void stb_init(void);

/**
 * @brief Secure one block with CCM*
 *
 * This functions secures one block with CCM* according to 802.15.4.
 *
 * @param[in,out] buffer Input: plaintext header and payload concatenated;
 *                       for encryption: MUST HAVE 'AES_BLOCKSIZE'
 *                       BYTES SPACE AT THE END FOR THE MIC!
 *                       Output: frame secured (with MIC at end)/unsecured
 * @param[in]  nonce   The nonce: Initialization Vector (IV) as used in
 *                     cryptography; the ZigBee nonce (13 bytes long)
 *                     are the bytes 2...14 of this nonce
 * @param[in] key The key to be used; if NULL, use the current key
 * @param[in] hdr_len Length of plaintext header (will not be encrypted)
 * @param[in] pld_len Length of payload to be encrypted; if 0, then only MIC
 *                    authentication implies
 * @param[in] sec_level Security level according to IEEE 802.15.4,
 *                    7.6.2.2.1, Table 95:
 *                    - the value may be 0 ... 7;
 *                    - the two LSBs contain the MIC length in bytes
 *                      (0, 4, 8 or 16);
 *                    - bit 2 indicates whether encryption applies or not
 * @param[in] aes_dir AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecure
 *
 * @return STB CCM Status
 *
 * @ingroup group_StbApi
 */
stb_ccm_t stb_ccm_secure(uint8_t * buffer,
uint8_t nonce[AES_BLOCKSIZE],
uint8_t * key,
uint8_t hdr_len,
uint8_t pld_len,
uint8_t sec_level,
uint8_t aes_dir);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #if defined(STB_ON_SAL) || defined(STB_ARMCRYPTO) ||
        * defined(__DOXYGEN__) */

#endif /* STB_GENERIC_H */
/* EOF */
