/**
 * \file
 *
 * \brief Definitions and settings for the CRC hardware module driver
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
