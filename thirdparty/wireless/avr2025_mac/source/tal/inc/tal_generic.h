/**
 * @file tal_generic.h
 *
 * @brief This file contains defines for TAL types.
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

/*
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
