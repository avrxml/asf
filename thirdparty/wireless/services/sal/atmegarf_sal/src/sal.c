/**
 * @file sal.c
 *
 * @brief Low-level crypto API for an AES unit implemented in AT86RFA1
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
 */

/**
 * @author
 *      Atmel Corporation: http://www.atmel.com
 *      Support: http://www.atmel.com/design-support/
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */

#include "sal_types.h"
#if (SAL_TYPE == ATMEGARF_SAL)
#include "sal.h"
#include <string.h>

/* === Macros ============================================================== */

/* values for SR_AES_DIR */
#define AES_DIR_VOID      (AES_DIR_ENCRYPT + AES_DIR_DECRYPT + 1)
/* Must be different from both summands */

/* value for SR_AES_CON */
#define AES_START         (1)

#define _SR_MASK(addr, mask, pos, val)     (((uint8_t)val << pos) & mask)
#define SR_MASK(SR, val)                   _SR_MASK(SR, val)

/* === Types =============================================================== */

/* === Data ============================================================= */

/* True if decryption key is actual and was computed. */
static bool dec_initialized = false;
/* Encryption mode with flags. */
static uint8_t mode_byte;
/* Last value of "dir" parameter in sal_aes_setup(). */
static uint8_t last_dir = AES_DIR_VOID;
/* Actual encryption key. */
static uint8_t enc_key[AES_KEYSIZE];
/* Actual decryption key (valid if last_dir == AES_DIR_DECRYPT). */
static uint8_t dec_key[AES_KEYSIZE];
/** Offset for register AES_CTRL */
#define RG_AES_CTRL                     (0x13C)
/** Sub-registers of Register @ref RG_AES_CTRL */
/** Access parameters for sub-register AES_IM in register @ref RG_AES_CTRL */
#define SR_AES_IM                       0x13C, 0x04, 2
/** Access parameters for sub-register AES_DIR in register @ref RG_AES_CTRL */
#define SR_AES_DIR                      0x13C, 0x08, 3

/** Constant AES_DIR_DECRYPT for sub-register @ref SR_AES_DIR in register
 * AES_CTRL */
#define AES_DIR_DECRYPT                 (1)

/** Constant AES_DIR_ENCRYPT for sub-register @ref SR_AES_DIR in register
 * AES_CTRL */
#define AES_DIR_ENCRYPT                 (0)
/** Access parameters for sub-register AES_MODE in register @ref RG_AES_CTRL */
#define SR_AES_MODE                     0x13C, 0x20, 5

/** Access parameters for sub-register AES_REQUEST in register @ref RG_AES_CTRL
**/
#define SR_AES_REQUEST                  0x13C, 0x80, 7

/** Constant AES_START for sub-register @ref SR_AES_REQUEST in register AES_CTRL
**/
#define AES_START                       (1)

/** Offset for register AES_STATUS */
#define RG_AES_STATUS                   (0x13D)
/** Sub-registers of Register @ref RG_AES_STATUS */

/** Access parameters for sub-register AES_DONE in register @ref RG_AES_STATUS
**/
#define SR_AES_DONE                     0x13D, 0x01, 0

/** Constant AES_COMPLETED for sub-register @ref SR_AES_DONE in register
 * AES_STATUS */
#define AES_COMPLETED                   (1)

/** Constant AES_NOT_COMPLETED for sub-register @ref SR_AES_DONE in register
 * AES_STATUS */
#define AES_NOT_COMPLETED               (0)
/** Access parameters for sub-register AES_ER in register @ref RG_AES_STATUS */
#define SR_AES_ER                       0x13D, 0x80, 7

/** Offset for register AES_STATE */
#define RG_AES_STATE                    (0x13E)

/** Offset for register AES_KEY */
#define RG_AES_KEY                      (0x13F)

/** Offset for register TRX_STATUS */
#define RG_TRX_STATUS                   (0x141)

/* State of trx before AES use; use to re-store trx state. */

/**
 * \brief Writes data into a transceiver register
 *
 * This macro writes a value into transceiver register.
 *
 * \param addr Address of the trx register
 * \param data Data to be written to trx register
 * \ingroup group_trx_access
 */
#define trx_reg_write(addr, data) \
	(*(volatile uint8_t *)(addr)) = (data)

#define _trx_bit_read(addr, mask, pos) \
	(((*(volatile uint8_t *)(addr)) & (mask)) >> (pos))

/**
 * \brief Subregister read
 *
 * \param   arg Subregister
 *
 * \return  Value of the read subregister
 * \ingroup group_trx_access
 */
#define trx_bit_read(arg) \
	_trx_bit_read(arg)

/**
 * \brief Reads current value from a transceiver register
 *
 * This macro reads the current value from a transceiver register.
 *
 * \param addr Specifies the address of the trx register
 * from which the data shall be read
 * \ingroup group_trx_access
 * \return value of the register read
 */
#define trx_reg_read(addr) \
	(*(volatile uint8_t *)(addr))

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
	uint8_t i;

	if (key != NULL) {
		/* Setup key. */
		dec_initialized = false;

		last_dir = AES_DIR_VOID;

		/* Save key for later use after decryption or sleep. */
		memcpy(enc_key, key, AES_KEYSIZE);

		/* fill in key */
		for (i = 0; i < AES_BLOCKSIZE; ++i) {
			trx_reg_write(RG_AES_KEY, key[i]);
		}
	}

	/* Set encryption direction. */
	switch (dir) {
	case AES_DIR_ENCRYPT:
		if (last_dir == AES_DIR_DECRYPT) {
			/*
			 * If the last operation was decryption, the encryption
			 * key must be stored in enc_key, so re-initialize it.
			 */
			for (i = 0; i < AES_BLOCKSIZE; ++i) {
				trx_reg_write(RG_AES_KEY, enc_key[i]);
			}
		}

		break;

	case AES_DIR_DECRYPT:
		if (last_dir != AES_DIR_DECRYPT) {
			if (!dec_initialized) {
				uint8_t dummy[AES_BLOCKSIZE];

				/* Compute decryption key and initialize unit
				 * with it. */

				/* Dummy ECB encryption. */
				mode_byte = SR_MASK(SR_AES_MODE, AES_MODE_ECB) |
						SR_MASK(SR_AES_DIR,
						AES_DIR_ENCRYPT);
				trx_reg_write(RG_AES_CTRL, mode_byte);
				sal_aes_exec(dummy);

				/* Read last round key. */
				for (i = 0; i < AES_BLOCKSIZE; ++i) {
					dec_key[i]
						= trx_reg_read(RG_AES_KEY);
				}

				dec_initialized = true;
			}

			/* Initialize the key. */
			for (i = 0; i < AES_BLOCKSIZE; ++i) {
				trx_reg_write(RG_AES_KEY, dec_key[i]);
			}

			break;
		}

	default:
		return false;
	}

	last_dir = dir;

	/* Set encryption mode. */
	switch (enc_mode) {
	case AES_MODE_ECB:
	case AES_MODE_CBC:
		mode_byte
			= SR_MASK(SR_AES_MODE, enc_mode) | SR_MASK(SR_AES_DIR,
				dir);
		break;

	default:
		return (false);
	}

	/* set mode and direction */

	trx_reg_write(RG_AES_CTRL, mode_byte);

	return (true);
}

/**
 * @brief Re-inits key and state after a sleep or TRX reset
 *
 * This function re-initializes the AES key and the state of the
 * AES engine after TRX sleep or reset.
 * The contents of AES register AES_CON is restored,
 * the next AES operation started with sal_aes_exec()
 * will be executed correctly.
 */
void sal_aes_restart(void)
{
	uint8_t i;
	uint8_t *keyp;

	if (last_dir == AES_DIR_ENCRYPT) {
		keyp = enc_key;
	} else {
		keyp = dec_key;
	}

	/* fill in key */
	for (i = 0; i < AES_BLOCKSIZE; ++i) {
		trx_reg_write(RG_AES_KEY, *keyp++);
	}

	trx_reg_write(RG_AES_CTRL, mode_byte);
}

/**
 * @brief Cleans up the SAL/AES after STB has been finished
 */
void _sal_aes_clean_up(void)
{
}

/**
 * @brief En/decrypt one AES block.
 *
 * The function returns after the AES operation is finished.
 *
 * @param[in]  data  AES block to be en/decrypted
 */
void sal_aes_exec(uint8_t *data)
{
	uint8_t i;

	for (i = 0; i < AES_BLOCKSIZE; ++i) {
		trx_reg_write(RG_AES_STATE, *data++);
	}

	trx_reg_write(RG_AES_CTRL,
			mode_byte | SR_MASK(SR_AES_REQUEST, AES_START));

	/* Wait for the operation to finish - poll RG_AES_RY. */
	while (!trx_bit_read(SR_AES_DONE)) {
	}
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
	uint8_t i;

	for (i = 0; i < AES_BLOCKSIZE; ++i) {
		*data++ = trx_reg_read(RG_AES_STATE);
	}
}

#endif /* AT86RFAx */

/* EOF */
