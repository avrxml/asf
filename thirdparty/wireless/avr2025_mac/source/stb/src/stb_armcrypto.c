/**
 * @file stb.c
 *
 * @brief High-level security tool box for ARM7 with crypto engine (SAM7XC)
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

#ifdef STB_ARMCRYPTO

/* === Includes ============================================================ */

#include <string.h>
#include "pal.h"
#include "ieee_const.h"
#include "stb.h"

/* === Macros ============================================================== */

#define MAX_FRAMELEN    aMaxPHYPacketSize - 2
#define FULL_BLOCK(l)   ((l + AES_BLOCKSIZE - \
	1) / AES_BLOCKSIZE * AES_BLOCKSIZE)

/* === Types =============================================================== */

/* === Globals ============================================================= */

/* True indicates if key has to be setup. */
static bool firstcall = true;

static void ctr_crypt(uint8_t *nonce, uint8_t *buf, uint8_t len);
static void compute_mic(uint8_t *nonce, uint8_t *buf,
		uint8_t len, uint8_t off);

/* === Implementation ====================================================== */

/* Dummy function, not required for ARM crypto */
void sal_aes_restart(void)
{
}

/* --- Helper functions ---------------------------------------------------- */

/**
 * @brief CTR en/decryption of buffer 'buf'
 *
 * @param[in,out]       nonce     The nonce (as "IV"), 1st and last byte
 *                                will be changed!
 * @param[in,out]       buf       Plain resp. ciphertext, MIC is first block
 * @param[in]           len       Number of encrypted bytes
 */
static void ctr_crypt(uint8_t *nonce, uint8_t *buf, uint8_t len)
{
	nonce[0] = 1;
	nonce[AES_BLOCKSIZE - 1] = 0;   /* counter value for MIC */

	/* Set mode */
	AT91C_BASE_AES->AES_MR = AT91C_AES_OPMOD_CTR | AT91C_AES_CIPHER |
			AT91C_AES_SMOD_PDC;

	/* Init CTR*/
	memcpy((void *)(AT91C_BASE_AES->AES_IVxR), nonce, AES_BLOCKSIZE);

	/* Set PDC */
	AT91C_BASE_AES->AES_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;
	AT91C_BASE_AES->AES_TPR = AT91C_BASE_AES->AES_RPR = (uint32_t)(buf);
	AT91C_BASE_AES->AES_TCR = AT91C_BASE_AES->AES_RCR = FULL_BLOCK(len) / 4;

	/* Start encryption */
	AT91C_BASE_AES->AES_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;

	/* Wait for end */
	while (!(AT91C_BASE_AES->AES_ISR & AT91C_AES_ENDRX)) {
	}
}

/**
 * @brief Compute MIC of buffer
 *
 * @param[in]       nonce     The nonce (as "IV")
 * @param[in,out]   buf       Buffer with plaintext, nonce will be
 *                            stored in first block
 * @param[in]       len       Number of plaintext bytes (with nonce)
 * @param[in]       micoff    Offset of 'buf' where MIC is stored
 */
static void compute_mic(uint8_t *nonce,
		uint8_t *buf,
		uint8_t len,
		uint8_t micoff)
{
	memcpy(buf, nonce, AES_BLOCKSIZE);

	/* Set mode */
	AT91C_BASE_AES->AES_MR = AT91C_AES_LOD | AT91C_AES_CIPHER |
			AT91C_AES_OPMOD_CBC | AT91C_AES_SMOD_PDC;

	memset((void *)(AT91C_BASE_AES->AES_IVxR), 0, AES_BLOCKSIZE);

	/* Set PDC */
	AT91C_BASE_AES->AES_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;
	AT91C_BASE_AES->AES_TPR = (uint32_t)buf;
	AT91C_BASE_AES->AES_TCR = FULL_BLOCK(len) / 4;

	/* Start encryption */
	AT91C_BASE_AES->AES_PTCR = AT91C_PDC_TXTEN;

	/* Wait for finishing */
	while (AT91C_BASE_AES->AES_TCR) {
	}
	while (!(AT91C_BASE_AES->AES_ISR & AT91C_AES_DATRDY)) {
	}

	/*
	 * Get result - write MIC before payload, so CTR encryption in ZigBee
	 * can be done in one step (the MIC is encrypted with counter value 0,
	 * the payload with 1,2,...)
	 */
	memcpy(buf + micoff,
			(void *)(AT91C_BASE_AES->AES_ODATAxR),
			AES_BLOCKSIZE);
}

/* --- End of Helper functions --------------------------------------------- */

/**
 * @brief STB Initialization
 *
 * This function initializes the STB.
 */
void stb_init(void)
{
	/* PMC init */
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_AES);
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
	/* Nothing to be done here. */
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
	uint8_t mic_len;
	uint8_t enc_flag;
	uint8_t off_pld; /* offset for payload in padbuf */
	uint8_t off_mic; /* offset for MIC in padbuf, before payload */
	uint8_t padbuf[MAX_FRAMELEN + 4 * AES_BLOCKSIZE];
	/* padded buffer with leading nonce and appended MIC */

	if (sec_level & 3) {
		mic_len = 1 << ((sec_level & 3) + 1);
	} else {
		mic_len = 0;
	}

	enc_flag = sec_level & 4;

	/* Test on correct parameters. */

	if ((sec_level & ~0x7) ||
			(buffer == NULL) ||
			(nonce == NULL) ||
			((uint16_t)pld_len + (uint16_t)hdr_len +
			(uint16_t)mic_len >
			aMaxPHYPacketSize)
			) {
		return (STB_CCM_ILLPARM);
	}

	if (firstcall) {
		if (key == NULL) {
			return (STB_CCM_KEYMISS); /* Initial call, but no key
			                           * given. */
		}

		/* Key must be non-NULL because of test above */
		firstcall = false;

		memcpy((void *)(AT91C_BASE_AES->AES_KEYWxR), key, AES_KEYSIZE);
	}

	/* Prepare nonce. */

	nonce[0] = 1; /* Always 2 bytes for length field. */

	if (mic_len > 0) {
		nonce[0] |= ((mic_len - 2) >> 1) << 3;
	}

	if (hdr_len) {
		nonce[0] |= 1 << 6;
	}

	nonce_0 = nonce[0];
	nonce[AES_BLOCKSIZE -  2] = 0;

	/*
	 * Fill padbuf
	 * first block for nonce (filled by compute_mic()),
	 * then optional space for header + 2 byte length, with padding,
	 * then space for payload with padding,
	 * then MIC
	 */

	off_pld = AES_BLOCKSIZE;

	memset(padbuf, 0, sizeof(padbuf));

	if (hdr_len) {
		off_pld += FULL_BLOCK(hdr_len + 2);
	}

	off_mic = off_pld - AES_BLOCKSIZE;

	if (pld_len) {
		memcpy(padbuf + off_pld, buffer + hdr_len, pld_len);
	}

	if (aes_dir == AES_DIR_ENCRYPT) {
		/*
		 * Authenticate -
		 * place header and result (16byte-MIC) before payload
		 */

		if (hdr_len) {
			memcpy(padbuf + AES_BLOCKSIZE + 2, buffer, hdr_len);
			padbuf[AES_BLOCKSIZE + 1] = hdr_len;
		}

		if (mic_len > 0) {
			nonce[AES_BLOCKSIZE - 1] = pld_len;
			compute_mic(nonce, padbuf, off_pld + pld_len, off_mic);
		}

		/* Encrypt payload and MIC */
		if (enc_flag) {
			ctr_crypt(nonce, padbuf + off_mic,
					pld_len + AES_BLOCKSIZE);

			/*
			 * Copy back payload - consider case that sec_level
			 * requires encryption but pld_len is 0, hence only
			 * MIC was encrypted, no payload
			 */
			if (pld_len) {
				memcpy(buffer + hdr_len, padbuf + off_pld,
						pld_len);
			}
		}

		/* Copy back encrypted MIC */
		if (mic_len) {
			memcpy(buffer + hdr_len + pld_len, padbuf + off_mic,
					mic_len);
		}
	} else {                /* Decrypt payload and MIC. */
		if (enc_flag) {
			/*
			 * Place encrypted MIC before encrypted payload
			 * (first block is still null)
			 */
			if (mic_len) {
				memcpy(padbuf + off_mic,
						buffer + hdr_len + pld_len,
						mic_len);
			}

			/* Decrypt */
			ctr_crypt(nonce, padbuf + off_mic,
					pld_len + AES_BLOCKSIZE);

			/* Copy plaintext back to original buffer */
			memcpy(buffer + hdr_len, padbuf + off_pld, pld_len);

			/* Copy MIC to end (just a buffer) */
			if (mic_len) {
				memcpy(padbuf + off_pld + FULL_BLOCK(pld_len),
						padbuf + off_mic,
						mic_len);
			}
		} else {
			memcpy(padbuf + off_pld + FULL_BLOCK(pld_len),
					buffer + hdr_len + pld_len, mic_len);
		}

		/* Check MIC. */
		if (mic_len > 0) {
			nonce[0] = nonce_0;
			nonce[AES_BLOCKSIZE - 1] = pld_len;
			memcpy(padbuf, nonce, AES_BLOCKSIZE);

			/* Copy hdr */
			if (hdr_len) {
				memset(padbuf + AES_BLOCKSIZE, 0,
						FULL_BLOCK(hdr_len + 2));
				memcpy(padbuf + AES_BLOCKSIZE + 2, buffer,
						hdr_len);
				padbuf[AES_BLOCKSIZE + 1] = hdr_len;
			}

			/* Pad pld */
			if (pld_len > 0) {
				memset(padbuf + off_pld + pld_len, 0, FULL_BLOCK(
						pld_len) - pld_len);
			}

			compute_mic(nonce, padbuf, off_pld + pld_len, off_mic);

			if (memcmp(padbuf + off_mic,
					padbuf + off_pld + FULL_BLOCK(pld_len),
					mic_len)) {
				return STB_CCM_MICERR;
			}
		}
	}

	return (STB_CCM_OK);
}

#endif /* #ifdef STB_ARMCRYPTO */

/* EOF */
