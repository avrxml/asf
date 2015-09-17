/**
 * @file tal_pib.h
 *
 * @brief This file contains the prototypes for TAL PIB functions.
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
#ifndef TAL_PIB_H
#define TAL_PIB_H

/* === INCLUDES ============================================================ */

/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

#ifndef CUSTOM_PWR_TABLE

/**
 * Tx power table
 * Table maps tx power value to register value
 */
#define TX_PWR_TABLE_NA	\
        /* Tx power, dBm        10     9     8     7     6     5     4     3
         *    2     1     0    -1    -2    -3    -4    -5    -6    -7    -8
         *   -9   -10   -11 */ \
        /* Register value */ 0xe1, 0xa1, 0x81, 0x82, 0x83, 0x84, 0x85, 0x42, \
	0x22, 0x23, 0x24, 0x25, 0x04, 0x05, 0x06, 0x07, 0x08, \
	0x09, 0x0a, 0x0b, 0x0c, 0x0d

#define TX_PWR_TABLE_EU1 \
        /* Tx power, dBm        10     9     8     7     6     5     4     3
         *    2     1     0    -1    -2    -3    -4    -5    -6    -7    -8
         *   -9   -10   -11 */ \
        /* Register value */ 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x63, \
	0x64, 0x65, 0x66, 0x47, 0x48, 0x28, 0x29, 0x2a, 0x08, \
	0x09, 0x0a, 0x0b, 0x0c, 0x0d

#define TX_PWR_TABLE_EU2 \
        /* Tx power, dBm        10     9     8     7     6     5     4     3
         *    2     1     0    -1    -2    -3    -4    -5    -6    -7    -8
         *   -9   -10   -11 */ \
        /* Register value */ 0xe8, 0xe8, 0xe8, 0xe8, 0xe8, 0xe8, 0xe9, 0xea, \
	0xeb, 0xab, 0xac, 0xad, 0x48, 0x28, 0x29, 0x2a, 0x08, \
	0x09, 0x0a, 0x0b, 0x0c, 0x0d

#define TX_PWR_TABLE_CHINA \
        /* Tx power, dBm        10     9     8     7     6     5     4     3
         *    2     1     0    -1    -2    -3    -4    -5    -6    -7    -8
         *   -9   -10   -11 */ \
        /* Register value */ 0xe4, 0xe4, 0xe4, 0xe5, 0xe6, 0xe8, 0xe9, 0xea, \
	0xca, 0xaa, 0xab, 0xac, 0x46, 0x25, 0x03, 0x04, 0x05, \
	0x06, 0x07, 0x08, 0x09, 0x0a

#define TX_PWR_TABLE_SIZE           (22)

#define EU_TX_PWR_TABLE             (2)     /* Use power table #2 for EU mode */

#define MAX_TX_PWR                  (10)
#define MIN_TX_PWR                  (-11)

#if (EU_TX_PWR_TABLE == 1)
#define MAX_TX_PWR_BPSK_20          (4)
#define MAX_TX_PWR_OQPSK_100        (-1)
#endif
#if (EU_TX_PWR_TABLE == 2)
#define MAX_TX_PWR_BPSK_20          (5)
#define MAX_TX_PWR_OQPSK_100        (2)
#endif

#ifdef HIGH_DATA_RATE_SUPPORT
#define MAX_TX_PWR_OQPSK_200        MAX_TX_PWR_OQPSK_100
#define MAX_TX_PWR_OQPSK_400        MAX_TX_PWR_OQPSK_100
#define MAX_TX_PWR_OQPSK_500        (7)
#define MAX_TX_PWR_OQPSK_1000       (4)
#endif

#define MAX_TX_PWR_CHINA            (8)     /* MAX TX power limit for
	                                     * 250,500kbps data rates */
#define MAX_TX_PWR_CHINA_1000       (5)     /* MAX TX power limit for 1000kbps
	                                     * data rates */

#endif  /* #ifndef CUSTOM_PWR_TABLE */

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup group_tal_pib_212
 * @{
 */

/**
 * \brief Initialize the TAL PIB
 *
 * This function initializes the TAL information base attributes
 * to their default values.
 */
void init_tal_pib(void);

/**
 * \brief Write all shadow PIB variables to the transceiver
 *
 * This function writes all shadow PIB variables to the transceiver.
 * It is assumed that the radio does not sleep.
 */
void write_all_tal_pib_to_trx(void);

/* ! @} */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_PIB_H */

/* EOF */
