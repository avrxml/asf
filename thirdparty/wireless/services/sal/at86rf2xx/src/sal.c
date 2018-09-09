/**
 * @file sal.c
 *
 * @brief Low-level crypto API for an AES unit implemented in AT86RF2xx
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
 */

/**
 * @author
 *      Microchip Technology Inc: http://www.microchip.com
 *      Support: https://www.microchip.com/support/
 */

/*
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */

#include "trx_access.h"
#include "sal_types.h"
#if (SAL_TYPE == AT86RF2xx)
#include <string.h>
#include "sal.h"
# include "delay.h"

/* === Macros ============================================================== */

#define AES_RY_BIT                  (1)     /* AES_RY: poll on finished op */
#define AES_DIR_VOID                (AES_DIR_ENCRYPT + AES_DIR_DECRYPT + 1)
/* Must be different from both summands */

#define _SR_MASK(addr, mask, pos, val)     (((uint8_t)val << pos) & mask)
#define SR_MASK(SR, val)                   _SR_MASK(SR, val)

/* === Types =============================================================== */

/* === Globals ============================================================= */

/* True after sal_aes_setup(). */
static bool setup_flag;
/* True if decryption key is actual and was computed. */
static bool dec_initialized = false;
/* Buffer written over SPI to AES unit. */
static uint8_t aes_buf[AES_BLOCKSIZE + 2];
/* Last value of "dir" parameter in sal_aes_setup(). */
static uint8_t last_dir = AES_DIR_VOID;
/* Actual encryption key. */
static uint8_t enc_key[AES_KEYSIZE];
/* Actual decryption key (valid if last_dir == AES_DIR_DECRYPT). */
static uint8_t dec_key[AES_KEYSIZE];

/* === Implementation ====================================================== */

/**
 * @brief Initialization of SAL.
 *
 * This functions initializes the SAL.
 */
void sal_init(void)
{
}

/**
 * @brief Setup AES unit
 *
 * This function perform the following tasks as part of the setup of the
 * AES unit: key initialization, set encryption direction and encryption mode.
 *
 * In general, the contents of SRAM buffer is destroyed. When using
 * sal_aes_wrrd(), sal_aes_read() needs to be called in order to get the result
 * of the last AES operation before you may call sal_aes_setup() again.
 *
 * @param[in] key AES key or NULL (NULL: use last key)
 * @param[in] enc_mode  AES_MODE_ECB or AES_MODE_CBC
 * @param[in] dir AES_DIR_ENCRYPT or AES_DIR_DECRYPT
 *
 * @return  False if some parameter was illegal, true else
 */
bool sal_aes_setup(uint8_t *key,
		uint8_t enc_mode,
		uint8_t dir)
{
	if (key != NULL) {
		/* Setup key. */
		dec_initialized = false;

		last_dir = AES_DIR_VOID;

		/* Save key for later use after decryption or sleep. */
		memcpy(enc_key, key, AES_KEYSIZE);

		/* Set subregister AES_MODE (Bits 4:6 in AES_CON) to 1: KEY
		 * SETUP. */
		aes_buf[0] = SR_MASK(SR_AES_MODE, AES_MODE_KEY);

		/* Fill in key. */
		memcpy(aes_buf + 1, key, AES_KEYSIZE);

		/* Write to SRAM in one step. */
		trx_sram_write((AES_BASE_ADDR + RG_AES_CTRL), aes_buf,
				AES_BLOCKSIZE + 1);
	}

	/* Set encryption direction. */
	switch (dir) {
	case AES_DIR_ENCRYPT:
		if (last_dir == AES_DIR_DECRYPT) {
			/*
			 * If the last operation was decryption, the encryption
			 * key must be stored in enc_key, so re-initialize it.
			 */
			aes_buf[0] = SR_MASK(SR_AES_MODE, AES_MODE_KEY);

			/* Fill in key. */
			memcpy(aes_buf + 1, enc_key, AES_KEYSIZE);

			/* Write to SRAM in one step. */
			trx_sram_write((AES_BASE_ADDR + RG_AES_CTRL),
					aes_buf, AES_BLOCKSIZE + 1);
		}

		break;

	case AES_DIR_DECRYPT:
		if (last_dir != AES_DIR_DECRYPT) {
			aes_buf[0] = SR_MASK(SR_AES_MODE, AES_MODE_KEY);

			if (!dec_initialized) {
				uint8_t dummy[AES_BLOCKSIZE];

				/* Compute decryption key and initialize unit
				 * with it. */

				/* Dummy ECB encryption. */
				aes_buf[0] = SR_MASK(SR_AES_MODE, AES_MODE_ECB);
				aes_buf[AES_BLOCKSIZE + 1] = SR_MASK(
						SR_AES_MODE, AES_MODE_ECB) |
						SR_MASK(SR_AES_REQUEST,
						AES_REQUEST);

				setup_flag = true; /* Needed in sal_aes_wrrd().
				                   **/
				sal_aes_wrrd(dummy, NULL);

				/* Read last round key: */

				/* Set to key mode. */
				aes_buf[0] = SR_MASK(SR_AES_MODE, AES_MODE_KEY);
				trx_sram_write(
						(AES_BASE_ADDR + RG_AES_CTRL),
						aes_buf, 1);

				/* Read the key. */
				trx_sram_read((AES_BASE_ADDR +
						RG_AES_STATE_KEY_0), dec_key,
						AES_KEYSIZE);
			}

			/*
			 * Now the decryption key is computed resp. known,
			 * simply re-initialize the unit;
			 * aes_buf[0] is AES_MODE_KEY
			 */

			/* Fill in key. */
			memcpy(aes_buf + 1, dec_key, AES_KEYSIZE);

			/* Write to SRAM in one step. */
			trx_sram_write((AES_BASE_ADDR + RG_AES_CTRL),
					aes_buf, AES_BLOCKSIZE + 1);

			dec_initialized = true;
		}

		break;

	default:
		return false;
	}

	last_dir = dir;

	/* Set encryption mode. */
	switch (enc_mode) {
	case AES_MODE_ECB:
	case AES_MODE_CBC:
	{
		aes_buf[0] = SR_MASK(SR_AES_MODE, enc_mode) |
				SR_MASK(SR_AES_DIR, dir);
		aes_buf[AES_BLOCKSIZE + 1] = SR_MASK(SR_AES_MODE, enc_mode) |
				SR_MASK(SR_AES_DIR, dir) |
				SR_MASK(SR_AES_REQUEST, AES_REQUEST);
	}
	break;

	default:
		return (false);
	}

	setup_flag = true;

	return (true);
}

/**
 * @brief Re-inits key and state after a sleep or TRX reset
 *
 * This function re-initializes the AES key and the state of the
 * AES engine after TRX sleep or reset.
 * The contents of AES registers AES_CON and AES_CON_MIRROR
 * are restored, the next AES operation started with sal_aes_wrrd()
 * will be executed correctly.
 * However, the contents of SRAM buffers is destroyed, in general.
 * When using sal_aes_wrrd(), call sal_aes_read() to get the result
 * of the last AES operation BEFORE you put the transceiver unit to
 * sleep state!
 */
void sal_aes_restart(void)
{
	/*
	 * AT86RF212 & AT86RF231 loose the key information after a sleep
	 * cycle, so the security information needs to be re-written
	 * to the transceiver.
	 * This is not required anymore for SPI transceivers beyond
	 * these two mentioned transceivers.
	 */
	uint8_t *keyp;
	uint8_t save_cmd;
	if (last_dir == AES_DIR_ENCRYPT) {
		keyp = enc_key;
	} else {
		keyp = dec_key;
	}

	save_cmd = aes_buf[0];
	aes_buf[0] = SR_MASK(SR_AES_MODE, AES_MODE_KEY);

	/* Fill in key. */
	memcpy(aes_buf + 1, keyp, AES_KEYSIZE);

	/* Write to SRAM in one step. */
	trx_sram_write((AES_BASE_ADDR + RG_AES_CTRL), aes_buf,
			AES_BLOCKSIZE + 1);

	aes_buf[0] = save_cmd;
	setup_flag = true;
}

/**
 * @brief Cleans up the SAL/AES after STB has been finished
 */
void _sal_aes_clean_up(void)
{
}

/**
 * @brief Writes data, reads previous result and does the AES en/decryption
 *
 * The function returns after the AES operation is finished.
 *
 * When sal_aes_wrrd() is called several times in sequence, from the
 * second call onwards, odata contains the result of the previous operation.
 * To obtain the last result, you must call sal_aes_read() at the end.
 * Please note that any call of sal_aes_setup() as well as putting
 * the transceiver to sleep state destroys the SRAM contents,
 * i.e. the next call of sal_aes_wrrd() yields no meaningful result.
 *
 * @param[in]  idata  AES block to be en/decrypted
 * @param[out] odata  Result of previous operation
 *                    (odata may be NULL or equal to idata)
 */
void sal_aes_wrrd(uint8_t *idata, uint8_t *odata)
{
	uint8_t save_cmd;

	/*
	 * Write data and start the operation.
	 * AES_MODE in aes_buf[0] and aes_buf[AES_BLOCKSIZE+1] as well as
	 * AES_REQUEST in aes_buf[AES_BLOCKSIZE+1]
	 * were set before in sal_aes_setup()
	 */
	memcpy(aes_buf + 1, idata, AES_BLOCKSIZE);

	/* trx_aes_wrrd() overwrites aes_buf, the last byte must be saved.
	**/
	save_cmd = aes_buf[AES_BLOCKSIZE + 1];

	if (setup_flag) {
		trx_aes_wrrd((AES_BASE_ADDR + RG_AES_CTRL), aes_buf,
				AES_BLOCKSIZE + 2);

		setup_flag = false;
	} else {
		trx_aes_wrrd((AES_BASE_ADDR + RG_AES_STATE_KEY_0),
				aes_buf + 1, AES_BLOCKSIZE + 1);
	}

	/* Restore the result. */
	if (odata != NULL) {
		memcpy(odata, aes_buf + 1, AES_BLOCKSIZE);
	}

	aes_buf[AES_BLOCKSIZE + 1] = save_cmd;

	/* Wait for the operation to finish for 24 us. */
	delay_us(24);
}

/**
 * @brief Reads the result of previous AES en/decryption
 *
 * This function returns the result of the previous AES operation,
 * so this function is needed in order to get the last result
 * of a series of sal_aes_wrrd() calls.
 *
 * @param[out] data     - result of previous operation
 */
void sal_aes_read(uint8_t *data)
{
	trx_sram_read((AES_BASE_ADDR + RG_AES_STATE_KEY_0), data,
			AES_BLOCKSIZE);
}

#endif /* AT86RF2xx */

/* EOF */
