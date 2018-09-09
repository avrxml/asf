/**
 * \file
 *
 * \brief Definitions and settings for the CRC implemented in software.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef CLASSB_CRC_SW_H_
#define CLASSB_CRC_SW_H_

#include <asf.h>
#include "classb_crc.h"

/**
 * \ingroup classb_crc
 * \defgroup classb_crc_sw CRC software implementation
 *
 * \brief CRC computations implemented in software.
 *
 * The CPU reads the data and computes the CRC checksum. There are two available
 * methods:
 * - Lookup table: this uses a CRC lookup table to speed up the computations.
 * The lookup table requires 512 (for 16 bit) or 1024 (for 32 bit) bytes of
 * flash memory.
 * - Direct computation: this calculates the checksum for each byte using a
 * polynomial division each time it is called. This version occupies no space in
 * flash memory, but is 3.5-4x slower than lookup table method.
 *
 * The CRC32-polynomial is in the reflected form (0xEDB88320) in the software
 * implementation. The initial remainder is 0xFFFFFFFF, and the generated
 * checksum is bit-reversed and complemented (in compliance with IEE802.3). The
 * CCITT polynomial used is 0x1021, with 0x0000 as initial remainder. In this
 * case the checksum is neither bit reversed nor complemented.
 *
 * @{
 */

/**
 * \name Configuration settings for CRC software implementation
 *
 * \brief Symbols used to choose between lookup table or direct computation.
 *
 * They should be defined for lookup table implementation and commented out
 * otherwise.
 *
 * @{
 */

#ifdef __DOXYGEN__
/** Select the lookup table method for 16-bit CRC */
#  define CRC_USE_16BIT_LOOKUP_TABLE
/** Select the lookup table method for 32-bit CRC */
#  define CRC_USE_32BIT_LOOKUP_TABLE
#else
/* #define CRC_USE_16BIT_LOOKUP_TABLE */
/* #define CRC_USE_32BIT_LOOKUP_TABLE */
#endif

/** @} */

/**
 * \name Constants for internal use
 *
 * \attention These should not be modified by user!
 *
 * \brief This options make sure that the CRC computation complies with the
 * standard 16-bit CCITT CRC and IEE802.3 32-bit CRC.
 *
 * @{
 */

/** Polynomial used for computation of CRC16 (software, not lookup table) */
#define CRC16_POLYNOMIAL      (uint16_t)0x1021
/** Polynomial used for computation of CRC32 (software, not lookup table) */
#define CRC32_POLYNOMIAL      (uint32_t)0xEDB88320UL
/** Final XOR Value for CRC32 */
#define CRC32_FINAL_XOR_VALUE (uint32_t)0xFFFFFFFF

/** @} */

/**
 * \name Internal macros used for CRC computation
 *
 * @{
 */

/**
 * \brief Update CRC value for one input byte.
 *
 * \note This macro works for both 16 and 32 bit CRC.
 *
 * \param data Input data byte.
 * \param crc Variable that holds the CRC value.
 * \param poly CRC polynomial.
 * \param crcbits Number of CRC bits.
 */
#define CLASSB_CRC(data, crc, poly, crcbits) do {                              \
	/* Bring the next byte into the checksum. */                           \
	crc ^= ((uint32_t)data << (crcbits - 8));                              \
	                                                                       \
	uint8_t bit;                                                           \
	                                                                       \
	/* Perform modulo-2 division. */                                       \
	for (bit = 8; bit > 0; --bit) {                                        \
		/* ..only if MSB is set. */                                    \
		if (crc & ((uint32_t)1 << (crcbits - 1))) {                    \
			crc = (crc << 1) ^ poly;                               \
		} else {                                                       \
			crc <<= 1;                                             \
		}                                                              \
	}                                                                      \
} while (0)

/**
 * \brief Update CRC value for one input byte (reflected polynomial).
 *
 * \note This macro works for both 16 and 32 bit CRC.
 *
 * \param data Input data byte.
 * \param crc Variable that holds the CRC value.
 * \param poly CRC polynomial.
 * \param crcbits Number of CRC bits.
 */
#define CLASSB_CRC_REFL(data, crc, poly, crcbits) do {                         \
	/* Bring next byte into the checksum. */                               \
	crc ^= (uint32_t)data;                                                 \
	                                                                       \
	uint8_t bit;                                                           \
	                                                                       \
	/* Perform modulo-2 division. */                                       \
	for (bit = 8; bit > 0; --bit) {                                        \
		/* ..only if LSB is set. */                                    \
		if (crc & 0x01) {                                              \
			crc = (crc >> 1) ^ poly;                               \
		} else {                                                       \
			crc >>= 1;                                             \
		}                                                              \
	}                                                                      \
} while (0)

/**
 * \brief Update 32-bit CRC value for one input byte (reflected polynomial).
 *
 * \note This macro assumes that the CRC lookup table is located in the lower
 * 64 kB address range of Flash.
 *
 * \param data Input data byte.
 * \param crc Variable that holds the CRC value.
 * \param table Table that contains pre-calculated CRC values.
 */
#define CLASSB_CRC_REFL_TABLE_32(data, crc, table) do {                        \
	data ^= crc & 0xFF;                                                    \
	crc = PROGMEM_READ_DWORD(&table[data]) ^ (crc >> 8);                   \
} while (0)

/**
 * \brief Update 16-bit CRC value for one input byte.
 *
 * \note This macro assumes that the CRC lookup table is located in the lower
 * 64 kB address range of Flash.
 *
 * \param data Input data byte.
 * \param crc Variable that holds the CRC value.
 * \param table Table that contains pre-calculated CRC values.
 */
#define CLASSB_CRC_TABLE_16(data, crc, table) do {                             \
	data ^= crc >> (16 - 8);                                               \
	crc = PROGMEM_READ_WORD(&table[data]) ^ (crc << 8);                    \
} while (0)

/** @} */

/**
 * \name CRC tests
 *
 * \brief Invariant memory tests based on CRC that are compliant with IEC60730
 * Class B.
 *
 * @{
 */

uint16_t CLASSB_CRC16_EEPROM_SW (eepromptr_t dataptr,
		const crcbytenum_t numBytes, eeprom_uint16ptr_t pchecksum);
uint16_t CLASSB_CRC16_Flash_SW (flashptr_t dataptr, const crcbytenum_t numBytes,
		eeprom_uint16ptr_t pchecksum);
uint32_t CLASSB_CRC32_EEPROM_SW (eepromptr_t dataptr,
		const crcbytenum_t numBytes, eeprom_uint32ptr_t pchecksum);
uint32_t CLASSB_CRC32_Flash_SW (flashptr_t dataptr, const crcbytenum_t numBytes,
		eeprom_uint32ptr_t pchecksum);

/** @} */

/** @} */

#endif /* CLASSB_CRC_SW_H_ */
