/**
 * @file stb.c
 *
 * @brief High-level security tool box
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
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifdef STB_ON_SAL

/* === Includes ============================================================ */

#include <string.h>
#include "pal.h"
#include "tal.h"
#include "ieee_const.h"
#include "stb.h"
#include "sal.h"
#include "stb_internal.h"

/* === Macros ============================================================== */
#define NO_HDR_MIC             (0x00)
/* === Types =============================================================== */

/* === Globals ============================================================= */

/* True indicates if key has to be setup. */
static bool key_change = true;
static uint8_t last_key[AES_BLOCKSIZE];
static bool stb_restart_required = false;
#if (SAL_TYPE != ATXMEGA_SAL)
/* State of trx before AES use; use to re-store trx state. */
static tal_trx_status_t prev_trx_status;

extern tal_trx_status_t tal_trx_status;
#endif
/* === Implementation ====================================================== */

/**
 * @brief STB Initialization
 *
 * This function initializes the STB.
 */
void stb_init(void)
{
	sal_init();
}

/**
 * @brief STB Restart
 *
 * This function re-starts the STB after power down.
 *
 * @ingroup apiStbApi
 */
void stb_restart(void)
{
	/*
	 * Re-use key_change flag indicating that the AES engine has been power-
	 * cycled and needs to be restarted.
	 */
	stb_restart_required = true;
}

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
 * @param[in] hdr_len Length of plain text header (will not be encrypted)
 * @param[in] pld_len Length of payload to be encrypted; if 0, then only MIC
 *                    authentication implies
 * @param[in] sec_level Security level according to IEEE 802.15.4,
 *                    7.6.2.2.1, Table 95:
 *                    - the value may be 0 ... 7;
 *                    - the two LSBs contain the MIC length in bytes
 *                      (0, 4, 8 or 16);
 *                    - bit 2 indicates whether encryption applies or not
 * @param[in] aes_dir AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecured
 *
 * @return STB CCM Status
 */
stb_ccm_t stb_ccm_secure(uint8_t *buffer,
		uint8_t nonce[AES_BLOCKSIZE],
		uint8_t *key,
		uint8_t hdr_len,
		uint8_t pld_len,
		uint8_t sec_level,
		uint8_t aes_dir)
{
	uint8_t nonce_0; /* nonce[0] for MIC computation. */
	uint8_t mic_len = 0;
	uint8_t enc_flag = ENCRYPTION_NOT_REQD;

	if (stb_restart_required) {
#if (SAL_TYPE != ATXMEGA_SAL)
		prev_trx_status = tal_trx_status;
		if (tal_trx_status == TRX_SLEEP) {
			tal_trx_wakeup();
		}
#endif
		sal_aes_restart();
		stb_restart_required = false;
	}

	switch (sec_level) {
	case SECURITY_00_LEVEL:
		/* No MIC & No Encryption at Security Level -0 */
		mic_len = LEN_MIC_00;
		break;

	case SECURITY_01_LEVEL:
		/* MIC-32 & No Encryption at Security Level -1 */
		mic_len = LEN_MIC_32;
		break;

	case SECURITY_02_LEVEL:
		/* MIC-64 & No Encryption at Security Level -2 */
		mic_len = LEN_MIC_64;
		break;

	case SECURITY_03_LEVEL:
		/* MIC-128 & No Encryption at Security Level -3 */
		mic_len = LEN_MIC_128;
		break;

	case SECURITY_04_LEVEL:
		/* No MIC & Encryption at Security Level -4 */
		mic_len = LEN_MIC_00;
		enc_flag = ENCRYPTION_REQD;
		break;

	case SECURITY_05_LEVEL:
		/* MIC-32 & Encryption at Security Level -5 */
		mic_len = LEN_MIC_32;
		enc_flag = ENCRYPTION_REQD;
		break;

	case SECURITY_06_LEVEL:
		/* MIC-64 & Encryption at Security Level -6 */
		mic_len = LEN_MIC_64;
		enc_flag = ENCRYPTION_REQD;
		break;

	case SECURITY_07_LEVEL:
		/* MIC-128 & Encryption at Security Level -7 */
		mic_len = LEN_MIC_128;
		enc_flag = ENCRYPTION_REQD;
		break;

	default:
		break;
	}

	/* Test on correct parameters. */

	if ((sec_level & ~0x7) ||
			(buffer == NULL) ||
			(nonce == NULL) ||
			((uint16_t)pld_len + (uint16_t)hdr_len +
			(uint16_t)mic_len > aMaxPHYPacketSize)
			) {
#if (SAL_TYPE != ATXMEGA_SAL)
		TRX_SLEEP();
		sal_aes_clean_up();
#endif
		return (STB_CCM_ILLPARM);
	}

	if (key_change && (key == NULL)) {
#if (SAL_TYPE != ATXMEGA_SAL)
		TRX_SLEEP();
		sal_aes_clean_up();
#endif
		/* Initial call, but no key given. */
		return (STB_CCM_KEYMISS);
	}

	/* Setup key if necessary. */

	if (!key_change && key != NULL) { /* There was some previous key. */
		uint8_t i;

		/* Test on changed key. */
		for (i = AES_BLOCKSIZE; i--; /* */) {
			key_change |= (last_key[i] ^ key[i]);
		}
	}

	if (key_change) {
		/*
		 * Key must be non-NULL because of test above, and
		 * ECB encryption is always the initial encryption mode.
		 */
		sal_aes_setup(key, AES_MODE_ECB, AES_DIR_ENCRYPT);
		memcpy(last_key, key, AES_KEYSIZE);
		key_change = false;
	}

	/* Prepare nonce. */
	nonce[0] = LEN_FIELD; /* Always 2 bytes for length field. */

	if (mic_len > 0) {
		nonce[0] |= (uint8_t)(((mic_len - 2) >> 1) << 3);
	}

	if (hdr_len) {
		nonce[0] |= ADATA;
	}

	nonce_0 = nonce[0];
	nonce[AES_BLOCKSIZE - 2] = 0;

	if (aes_dir == AES_DIR_ENCRYPT) {
		/* Authenticate. */
		if (mic_len > 0) {
			nonce[AES_BLOCKSIZE - 1] = pld_len;

			if (ENCRYPTION_REQD == enc_flag) {
				compute_mic(buffer,
						buffer + hdr_len + pld_len,
						nonce,
						hdr_len,
						pld_len);
			}
		}

		nonce[0] = PLAINTEXT_FLAG;
		encrypt_pldmic(buffer + hdr_len, nonce, mic_len,
				pld_len);
	} else {
		/* Decrypt payload and MIC. */
		if (enc_flag == ENCRYPTION_REQD) {
			nonce[0] = PLAINTEXT_FLAG;
			encrypt_pldmic(buffer + hdr_len, nonce, mic_len,
					pld_len);
		}

		/* Check MIC. */
		if (mic_len > 0) {
			/* maximal MIC size */
			uint8_t rcvd_mic[AES_BLOCKSIZE];

			nonce[0] = nonce_0;
			nonce[AES_BLOCKSIZE - 1] = pld_len;

			if (ENCRYPTION_REQD == enc_flag) {
				compute_mic(buffer,
						rcvd_mic,
						nonce,
						hdr_len,
						pld_len);
			}

			buffer += hdr_len + pld_len;

			if (memcmp(buffer, rcvd_mic, mic_len)) {
				return STB_CCM_MICERR;
			}
		}
	}

#if (SAL_TYPE != ATXMEGA_SAL)
	TRX_SLEEP();
	sal_aes_clean_up();
#endif
	return (STB_CCM_OK);
}

#endif /* #ifdef STB_ON_SAL */

/* EOF */
