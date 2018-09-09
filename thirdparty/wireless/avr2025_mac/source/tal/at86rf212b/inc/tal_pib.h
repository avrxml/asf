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
        /* Tx power, dBm        11     10     9     8     7     6     5     4
         *    3     2     1     0    -1    -2    -3    -4    -5    -6    -7
         *   -8    -9   -10   -11   -12   -13   -14   -15   -16   -17   -18
         *  -19   -20   -21   -22   -23   -24   -25 */ \
        /* Register value */ 0xc0, 0xc1, 0x80, 0x82, 0x83, 0x84, 0x40, 0x86, \
	0x00, 0x01, 0x02, 0x03, 0x04, 0x27, 0x05, 0x07, 0x08, \
	0x91, 0x09, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, \
	0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x19, 0x1a, 0x1b, \
	0x1c, 0x1d

#define TX_PWR_TABLE_EU	\
        /* Tx power, dBm        11     10     9     8     7     6     5     4
         *    3     2     1     0    -1    -2    -3    -4    -5    -6    -7
         *   -8    -9   -10   -11   -12   -13   -14   -15   -16   -17   -18
         *  -19   -20   -21   -22   -23   -24   -25 */ \
        /* Register value */ 0xa0, 0x80, 0xe4, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, \
	0xcb, 0xcc, 0xcd, 0xad, 0x47, 0x48, 0x49, 0x29, 0x90, \
	0x91, 0x93, 0x94, 0x2f, 0x30, 0x31, 0x0f, 0x10, 0x11, \
	0x12, 0x13, 0x14, 0x15, 0x17, 0x18, 0x19, 0x1a, 0x1b, \
	0x1c, 0x1d

#define TX_PWR_TABLE_CHINA \
        /* Tx power, dBm        11     10     9     8     7     6     5     4
         *    3     2     1     0    -1    -2    -3    -4    -5    -6    -7
         *   -8    -9   -10   -11   -12   -13   -14   -15   -16   -17   -18
         *  -19   -20   -21   -22   -23   -24   -25 */ \
        /* Register value */ 0xc1, 0xe3, 0xe4, 0xc5, 0xe7, 0xe8, 0xe9, 0xea, \
	0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xaf, 0x26, 0x27, 0x28, \
	0x29, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, \
	0x0f, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, \
	0x19, 0x1a

#define TX_PWR_TABLE_SIZE           (37)

#define MAX_TX_PWR                  (11)
#define MIN_TX_PWR                  (-25)
#define DEFAULT_TX_PWR_CHINA_CH_0_3     (4)

/* \todo use one max power for each band (EU, NA, China) */
/* max power of standard compliant modes */
#define MAX_TX_PWR_BPSK_40          MAX_TX_PWR  /* NA, band: 915MHz */
#define MAX_TX_PWR_OQPSK_SIN_250    MAX_TX_PWR  /* NA, band: 915MHz */
#define MAX_TX_PWR_BPSK_20         (MAX_TX_PWR)         /* EU, band: 868MHz */
#define DEFAULT_TX_PWR_BPSK_20      (6)         /* EU, band: 868MHz */
#define MAX_TX_PWR_OQPSK_SIN_RC_100 (MAX_TX_PWR)         /* EU, band: 868MHz */
#define MAX_TX_PWR_OQPSK_RC_250     (MAX_TX_PWR)        /* China, band: 780MHz
	                                                **/
#define DEFAULT_TX_PWR_OQPSK_RC_250_500     (10)        /* China, band: 780MHz
	                                                **/
#define DEFAULT_TX_PWR_OQPSK_SIN_RC_100_200_400 (3)         /* EU, band: 868MHz
	                                                    **/

/* max power of proprietary modes */
#ifdef HIGH_DATA_RATE_SUPPORT
#define MAX_TX_PWR_OQPSK_SIN_500    MAX_TX_PWR  /* NA, band: 915MHz */
#define MAX_TX_PWR_OQPSK_SIN_1000   MAX_TX_PWR  /* NA, band: 915MHz */
#define MAX_TX_PWR_OQPSK_SIN_RC_200 (MAX_TX_PWR)         /* EU, band: 868MHz */
#define MAX_TX_PWR_OQPSK_SIN_RC_400 (MAX_TX_PWR)         /* EU, band: 868MHz */
#define MAX_TX_PWR_OQPSK_RC_500     (MAX_TX_PWR)        /* China, band: 780MHz
	                                                **/
#define MAX_TX_PWR_OQPSK_RC_1000    (MAX_TX_PWR)        /* China, band: 780MHz
	                                                **/

/* old stuff
 * #define MAX_TX_PWR_OQPSK_200        MAX_TX_PWR_OQPSK_100
 * #define MAX_TX_PWR_OQPSK_400        MAX_TX_PWR_OQPSK_100
 * #define MAX_TX_PWR_OQPSK_SIN_500    (7)
 * #define MAX_TX_PWR_OQPSK_SIN_1000   (4)
 */
#endif /* #ifdef HIGH_DATA_RATE_SUPPORT */

#endif  /* #ifndef CUSTOM_PWR_TABLE */

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup group_tal_pib_212b
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
