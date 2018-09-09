/**
 * @file tal_pib.h
 *
 * @brief This file contains the prototypes for TAL PIB functions.
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
