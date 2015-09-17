/**
 * \file
 *
 * \brief Settings and defines used in connection with CRC memory tests.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CLASSB_CRC_H_
#define CLASSB_CRC_H_

#include <asf.h>
#include "error_handler.h"

/**
 * \name EEPROM Mapping defines
 *
 * @{
 */

/**
 * \brief Enable memory mapping of EEPROM, if it is not already enabled.
 *
 * \note Use this in conjunction with CLASSB_EEMAP_END() to ensure that the
 * prior configuration of memory-mapping is preserved.
 */
#define CLASSB_EEMAP_BEGIN()                                                   \
	bool eemap_is_enabled;                                                 \
	/* Check if memory mapping of EEPROM is already enabled. */            \
	eemap_is_enabled = (NVM.CTRLB & NVM_EEMAPEN_bm) ? true : false;        \
	                                                                       \
	if (!eemap_is_enabled) {                                               \
		/* Ensure that NVM is ready before enabling memory mapping. */ \
		do {                                                           \
		} while (NVM.STATUS & NVM_NVMBUSY_bm);                         \
		NVM.CTRLB |= NVM_EEMAPEN_bm;                                   \
	}

/**
 * \brief Disable memory mapping of EEPROM, unless it was previously enabled.
 *
 * \note Use this in conjunction with CLASSB_EEMAP_BEGIN().
 */
#define CLASSB_EEMAP_END()                                                     \
	if (!eemap_is_enabled) {                                               \
		NVM.CTRLB &= ~NVM_EEMAPEN_bm;                                  \
	}

/** @} */

/**
 * \defgroup classb_crc CRC Tests
 *
 * \brief This CRC module can be used to check the Flash and EEPROM memories.
 *
 * \section crc_intro Introduction to CRC
 *
 * A cyclic redundancy check (CRC) is an error detection technique test
 * algorithm used to detect errors in data. It is commonly used to determine the
 * correctness of data stored in memory or data that is received through some
 * communication channel. The CRC algorithm takes a data stream or a block of
 * data as input and generates a 16- or 32-bit output that can be used as a
 * checksum. We suggest two possible methods to detect errors using this
 * checksum:
 *
 * -Compute checksum of the data section and compare with the previous one. If
 * they are different, there is an error.
 *
 * -Compute checksum of the extended data section (data section and original
 * checksum). The resulting checksum should be a fixed implementation-specific
 * value, otherwise there is an error.
 *
 * If there were errors in the data, the application should take corrective
 * actions, e.g., requesting the data to be sent again or simply not using the
 * incorrect data.
 *
 * The CRC Tests module implements 16-bit CCITT CRC and IEE802.3 32-bit CRC
 * algorithms in software and hardware:
 *  - \ref classb_crc_sw
 *  - \ref classb_crc_hw
 *
 * \section crc_usage Usage
 *
 * There are a number of symbols in \ref crc_conf that can be defined or
 * undefined in order to choose between software and hardware implementations,
 * and 16-bit and 32-bit CRC. The software implementation can be further
 * configured in the corresponding header file.
 *
 * To calculate an EEPROM checksum, use the functions
 *  - \c CLASSB_CRC[16/32]_EEPROM_[SW/HW]
 *
 * To calculate a Flash checksum, use the functions
 *  - \c CLASSB_CRC[16/32]_Flash_[SW/HW]
 *
 * If there are any errors, the error handler \ref CLASSB_ERROR_HANDLER_CRC() is
 * called.
 *
 * \note Interrupts must be disabled during this test.
 *
 * \section crc_speed Execution speed
 *
 * We provide an estimate for the execution speed of different CRC
 * implementations. Note that this is based on empirical results.
 * - EEPROM
 *     - CRC32 lookup table is ~45% slower than HW.
 *     - CRC32 direct calculation is ~750% slower than HW.
 *     - CRC16 lookup table is ~20% slower than HW.
 *     - CRC16 direct calculation is ~430% slower than HW.
 * - Flash
 *     - CRC32 lookup table is ~4600% slower than HW.
 *     - CRC32 direct calculation is ~15800% slower than HW.
 *     - CRC16 lookup table is ~3% slower than HW.
 *     - CRC16 direct calculation is ~275% slower than HW.
 *
 * @{
 */

/**
 * \defgroup crc_conf Configuration settings
 *
 * \brief Symbols that can be defined to configure the CRC computation.
 *
 * These are used to avoid compiling code that will not be used.
 *
 * @{
 */

/** Compile hardware implementation */
#define CLASSB_CRC_USE_HW
/** Compile software implementation */
#define CLASSB_CRC_USE_SW
/** Compile 16-bit CRC functions */
#define CLASSB_CRC_16_BIT
/** Compile 32-bit CRC functions */
#define CLASSB_CRC_32_BIT

/** @} */

/**
 * \defgroup crc_int_conf Constants for internal use
 *
 * \brief Symbols for ensuring standard compliant CRC.
 *
 * These are used to ensure that the CRC computation complies with the
 * standard 16-bit CCITT CRC and IEE802.3 32-bit CRC.
 *
 * \attention These should not be modified by user!
 *
 * @{
 */

/** Initial remainder for the 16-bit CRC */
#define CRC16_INITIAL_REMAINDER (uint16_t)0x0000
/** Initial remainder for the 32-bit CRC */
#define CRC32_INITIAL_REMAINDER (uint32_t)0xFFFFFFFF

/** @} */

/**
 * \defgroup crc_types Type definitions for CRC functions
 *
 * \brief Data types used in connection with CRC computations.
 *
 * This includes the type for variables that stores the data size in bytes,
 * memory addresses in Flash, and pointers to memory. Both for Flash and EEPROM
 * there are void pointer types and pointers to byte. Variables can be assigned
 * a memory attribute with IAR. Therefore, pointers will be large enough to hold
 * the address of a given variable. However, with GCC pointers have 16 bits.
 * This means that if a device has a large program  memory, we have to use
 * 32-bits variables to store Flash addresses.
 *
 * @{
 */

/**
 * \brief Data type to use for byte counts in CRC computations.
 *
 * This type should be kept as small as possible to optimize for speed.
 */
typedef uint32_t crcbytenum_t;

/**
 * \name Pointer types for CRC functions
 *
 * These are needed for IAR and GCC compatibility.
 *
 * @{
 */

#if defined(__ICCAVR__)
typedef const void __eeprom * eepromptr_t;
typedef const uint8_t __eeprom * eeprom_uint8ptr_t;
typedef const uint16_t __eeprom * eeprom_uint16ptr_t;
typedef const uint32_t __eeprom * eeprom_uint32ptr_t;
typedef const void _MEMATTR * flashptr_t;
typedef const uint8_t _MEMATTR * flash_uint8ptr_t;
#elif defined (__GNUC__) || defined (__DOXYGEN__)
/** Generic pointer to EEPROM. */
typedef const void * eepromptr_t;
/** Pointer to a byte in EEPROM. */
typedef const uint8_t * eeprom_uint8ptr_t;
/** Pointer to two bytes in EEPROM. */
typedef const uint16_t * eeprom_uint16ptr_t;
/** Pointer to four bytes in EEPROM. */
typedef const uint32_t * eeprom_uint32ptr_t;
/** Generic pointer to Flash. */
typedef flash_addr_t flashptr_t;
/** Pointer to a byte in Flash. */
typedef flash_addr_t flash_uint8ptr_t;
#else
#  error Unknown compiler!
#endif

/** @} */

/** @} */

#ifdef CLASSB_CRC_USE_HW
#  include "classb_crc_hw.h"
#endif

#ifdef CLASSB_CRC_USE_SW
#  include "classb_crc_sw.h"
#endif

#if defined(__GNUC__) && !defined(__OPTIMIZE__)
#  error Optimization must be enabled to successfully write to protected \
	registers, due to timing constraints.
#endif

/** @} */

#endif /* CLASSB_CRC_H_ */
