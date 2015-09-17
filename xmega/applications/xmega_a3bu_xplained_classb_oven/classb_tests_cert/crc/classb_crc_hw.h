/**
 * \file
 *
 * \brief Definitions and settings for the CRC hardware module driver
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

#ifndef CLASSB_CRC_HW_H_
#define CLASSB_CRC_HW_H_

#include <asf.h>
#include "classb_crc.h"

/**
 * \ingroup classb_crc
 *
 * \defgroup classb_crc_hw CRC hardware implementation
 *
 * \brief Low-level driver implementation for the AVR XMEGA AU CRC module.
 *
 * The CRC computation is done by the hardware CRC module. Even if this hardware
 * module computes the checksum, the CPU can still be required to read the data.
 * In the case of 32-bit CRC computation for flash memory, the whole process can
 * be run without support from the CPU. However, for the 16-bit computation the
 * CPU would have to read from Flash memory and transfer data to the CRC module.
 * For CRC computations of data in SRAM, EEPROM or peripherals, the CPU will have
 * to transfer data to the CRC module unless a DMA transaction is setup. The
 * CRC-CCITT polynomial is 0x1021 and the IEEE 802.3 polynomial is 0x04C11DB7.
 * More details on the CRC implementation can be found in the XMEGA AU manual.
 *
 * \note \li The CRC module driver requires one of the new  XMEGA AU devices.
 * \li The CPU is locked while the CRC module is set to compute a checksum from
 * Flash.
 * \li CRC32 in Flash can only check an even number of bytes.
 * \li When specifying Flash or EEPROM addresses, 0x000000 denotes the start of
 * physical memory.
 *
 * @{
 */

/**
 * \name NVM Commands
 *
 * \brief NVM commands for 32-bit CRC of flash memory.
 *
 * These are only used by \ref CLASSB_CRC32_Flash_HW.
 *
 * @{
 */

/** Compute a CRC checksum on an address range */
#define CRC_FLASH_RANGE    NVM_CMD_FLASH_RANGE_CRC_gc
/** Compute a CRC checksum on the application section */
#define CRC_BOOT           NVM_CMD_BOOT_CRC_gc
/** Compute a CRC checksum on the boot section */
#define CRC_APP            NVM_CMD_APP_CRC_gc

/** @} */

/**
 * \name CRC tests
 *
 * \brief Invariant memory tests based on CRC that are compliant with IEC60730 Class B.
 *
 * @{
 */

uint16_t CLASSB_CRC16_EEPROM_HW (eepromptr_t dataptr,
		const crcbytenum_t numBytes, eeprom_uint16ptr_t pchecksum);
uint16_t CLASSB_CRC16_Flash_HW (flashptr_t dataptr, const crcbytenum_t numBytes,
		eeprom_uint16ptr_t pchecksum);
uint32_t CLASSB_CRC32_EEPROM_HW (eepromptr_t dataptr,
		const crcbytenum_t numBytes, eeprom_uint32ptr_t pchecksum);
uint32_t CLASSB_CRC32_Flash_HW (NVM_CMD_t crc_type, flashptr_t dataptr,
		const crcbytenum_t numBytes, eeprom_uint32ptr_t pchecksum);

/** @} */

/** Enum for indicating whether to use 16- or 32-bits CRC */
enum crc_16_32_t {
	CRC_16BIT,
	CRC_32BIT
};

/**
 * \name Internal functions
 *
 * \brief These are functions from the CRC module driver.
 *
 * @{
 */

void crc_set_initial_value(uint32_t value);
void crc32_append_value(uint32_t value, void *ptr);
void crc16_append_value(uint16_t value, void *ptr);
uint32_t crc_flash_checksum(NVM_CMD_t crc_type, flash_addr_t flash_addr,
		uint32_t len);
uint32_t crc_io_checksum(void *data, uint16_t len, enum crc_16_32_t crc_16_32);
void crc_io_checksum_byte_start(enum crc_16_32_t crc_16_32);
void crc_io_checksum_byte_add(uint8_t data);
uint32_t crc_io_checksum_byte_stop(void);
void nvm_issue_flash_range_crc(flash_addr_t start_addr, flash_addr_t end_addr);

/** @} */

/** @} */

#endif /* CLASSB_CRC_HW_H_ */
