/**
 * @file tal_generic.h
 *
 * @brief This file contains defines for TAL types.
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

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_GENERIC_H
#define TAL_GENERIC_H

/* === INCLUDES ============================================================ */
#include <stdint.h>
#include <stdbool.h>

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/**
 * PIB attribute value type
 */
__PACK__DATA__
typedef union {
	/** PIB Attribute Bool */
	bool pib_value_bool;
	/** PIB Attribute 8-bit */
	uint8_t pib_value_8bit;
	/** PIB Attribute 16-bit */
	uint16_t pib_value_16bit;
	/** PIB Attribute 32-bit */
	uint32_t pib_value_32bit;
	/** PIB Attribute 64-bit */
	uint64_t pib_value_64bit;
} pib_value_t;
__PACK__RST_DATA__
/* === MACROS ============================================================== */

/* RF bands: */

/**
 * 868 / 910 MHz (channels 0 through 10)
 * using BPSK
 */
#define BAND_900                            (0)

/**
 * 2.4 GHz (channels 11 through 26)
 */
#define BAND_2400                           (1)

/** AES core operation direction: Encryption (ECB, CBC) */
#define AES_DIR_ENCRYPT   (0)

/** AES core operation direction: Decryption (ECB) */
#define AES_DIR_DECRYPT   (1)

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TAL task handling
 *
 * This function
 * - Checks and allocates the receive buffer.
 * - Processes the TAL incoming frame queue.
 * - Implements the TAL state machine.
 * @ingroup apiTalApi
 */
/* void tal_task(void); */

uint32_t tal_convert_symbols_to_us_def(uint32_t symbols);

uint32_t tal_convert_us_to_symbols_def(uint32_t us);

/**
 * @brief Generates a 16-bit random number used as initial seed for srand()
 *
 * This function generates a 16-bit random number by means of using the
 * Random Number Generator from the transceiver.
 * The Random Number Generator generates 2-bit random values. These 2-bit
 * random values are concatenated to the required 16-bit random seed.
 * The generated random 16-bit number is fed into function srand()
 * as initial seed.
 * The transceiver state is initally set to RX_ON.
 * After the completion of the random seed generation, the
 * tranceiver is set to TRX_OFF.
 *
 * @ingroup apiTalApi
 */
/* void tal_generate_rand_seed(void); */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_GENERIC_H */
/* EOF */
