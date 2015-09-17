/**
 * \file
 *
 * \brief Cyclic Redundancy Check module driver
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdbool.h>
#include <assert.h>
#include "crc.h"

//! Initial CRC value to use on next CRC calculation.
uint32_t crc_initial_value = 0;

/**
 * \brief Check if the CRC module is busy
 *
 * This function can be used to see if a CRC checksum generation is completed.
 *
 * \retval true if module is busy
 * \retval false if module is not busy
 *
 */
static inline bool crc_is_busy(void)
{
	return ((CRC_STATUS & CRC_BUSY_bm) == CRC_BUSY_bm);
}

/**
 * \brief Add one byte of data to CRC performed on I/O interface
 *
 * This function writes a byte to the DATAIN register. Each time this function
 * is called, the checksum is calculated for the new data appended to all
 * previous data written since the \ref crc_io_checksum_byte_start was called.
 * A new checksum is ready one clock cycle after the DATAIN register is
 * written.
 *
 * \param data  data to perform CRC on
 *
 */
void crc_io_checksum_byte_add(uint8_t data)
{
	CRC_DATAIN = data;
}

/**
 * \internal
 * \brief Bit reverse the given value
 *
 * \param value the value to bit revert
 * \return bit reverted value
 *
 */
static inline uint32_t crc_bit_revert(uint32_t value)
{
	uint32_t result;
	uint8_t i;

	for (result = i = 0; i < 32; i++) {
		result = (result << 1) + (value & 1);
		value >>= 1;
	}

	return result;
}

/**
 * \internal
 * \brief Reset CRC module and set CHECKSUM to initial value
 *
 * The CRC registers will be reset one peripheral clock cycle after the
 * RESET[1] bit is set. The initial value is reset to 0 after loading it into
 * the CHECKSUM registers.
 *
 */
static inline void crc_reset(void)
{
	// Reset module
	CRC_CTRL |= CRC_RESET_RESET0_gc;

	// Set initial checksum value
	CRC.CHECKSUM0 = crc_initial_value & 0xFF;
	CRC.CHECKSUM1 = (crc_initial_value >> 8) & 0xFF;
	CRC.CHECKSUM2 = (crc_initial_value >> 16) & 0xFF;
	CRC.CHECKSUM3 = (crc_initial_value >> 24) & 0xFF;

	crc_initial_value = 0;
}

/**
 * \internal
 * \brief Read CRC-16 checksum
 *
 * This function returns the computed CRC-16 checksum.
 *
 * \return checksum checksum value
 */
static inline uint16_t crc16_checksum_read(void)
{
	uint16_t checksum;
	checksum = 0;
	checksum = ((uint16_t)CRC_CHECKSUM0 & 0x00FF);
	checksum |= (((uint16_t)CRC_CHECKSUM1 << 8) & 0xFF00);

	return checksum;
}

/**
 * \internal
 * \brief Read CRC-32 checksum
 *
 * This function extracts the CRC-32 checksum from the registers.
 *
 * \note To comply with IEEE 802.3 CRC-32, the checksum is bit reversed
 *       and complemented.
 *
 * \return checksum checksum value
 */
static inline uint32_t crc32_checksum_read(void)
{
	uint32_t checksum;

	checksum = ((uint32_t)CRC_CHECKSUM0 & 0x000000FF);
	checksum |= (((uint32_t)CRC_CHECKSUM1 << 8) & 0x0000FF00);
	checksum |= (((uint32_t)CRC_CHECKSUM2 << 16) & 0x00FF0000);
	checksum |= (((uint32_t)CRC_CHECKSUM3 << 24) & 0xFF000000);

	// Bit reverse the result (to comply with IEEE 802.3 CRC-32)
	checksum = crc_bit_revert(checksum);

	// Complement the result (to comply with IEEE 802.3 CRC-32)
	checksum = ~checksum;

	return checksum;
}

/**
 * \internal
 * \brief Read checksum
 *
 * This function waits until the CRC conversion is complete and returns
 * either the 32 bit or 16 bit checksum as a 32 bit value.
 *
 * \note This function must not be called before a IO CRC calculation is
 * completed or a FLASH or DMA CRC calculation is started, as this will cause
 * it to wait for the BUSY flag to be cleared forever.
 *
 * \return checksum checksum value
 */
static inline uint32_t crc_checksum_read(void)
{
	// Wait until the CRC conversion is finished
	while (crc_is_busy()) {
		// Do nothing
	}

	// Check if we have a 32 or 16 bit checksum and return the correct one
	if ((CRC_CTRL & CRC_CRC32_bm) == CRC_CRC32_bm) {
		return crc32_checksum_read();
	} else {
		return crc16_checksum_read();
	}
}

/**
 * \internal
 * \brief Set data source for the CRC module after reset
 *
 * The selected source is locked until either the CRC generation is completed
 * or the CRC module is reset.
 * CRC generation complete is generated and signalled from the selected source
 * when used with the DMA controller or flash memory.
 * To disable the CRC module, use CRC_SOURCE_DISABLE_gc as source.
 *
 * \param source the data source for the CRC module
 */
static inline void crc_set_source(CRC_SOURCE_t source)
{
	CRC_CTRL &= ~CRC_SOURCE_gm;
	CRC_CTRL |= source;
}

/**
 * \internal
 * \brief Enable CRC-32
 *
 * This function will set the CRC-32 polynomial to be used to generate
 * the checksum.
 *
 * \note This cannot be changed while the busy flag is set.
 *
 */
static inline void crc_32_enable(void)
{
	CRC_CTRL |= CRC_CRC32_bm;
}

/**
 * \internal
 * \brief Disable the CRC module
 *
 */
static inline void crc_disable(void)
{
	crc_set_source(CRC_SOURCE_DISABLE_gc);
}

/**
 * \internal
 * \brief Complete CRC calculation by reading checksum and disabling module
 *
 * \return checksum checksum value
 */
static inline uint32_t crc_checksum_complete(void)
{
	uint32_t checksum;

	// Read checksum
	checksum = crc_checksum_read();

	// Disable CRC module
	crc_disable();

	// Return checksum
	return checksum;
}

/**
 * \brief Get and append data to generate checksum zero with CRC32
 *
 * This function calculates checksum value to append to the data after
 * generating its checksum.
 * The value is written to given pointer as little endian.
 * Taking the checksum of the data appended will result in checksum zero.
 *
 * \note At least four bytes must be reserved at the given pointer location.
 *
 * \param value the value to convert to append value
 * \param ptr pointer to 4 byte memory location to store the value.
 *
 */
void crc32_append_value(uint32_t value, void *ptr)
{
	value = crc_bit_revert(value);

	*(uint8_t*)ptr = value & 0xFF;
	*((uint8_t*)ptr+1) = (value >> 8) & 0xFF;
	*((uint8_t*)ptr+2) = (value >> 16) & 0xFF;
	*((uint8_t*)ptr+3) = (value >> 24) & 0xFF;
}

/**
 * \brief Appends a CRC value to the given pointer
 *
 * Taking the checksum of the value and the appended data will result in zero.
 *
 * \note At least two bytes must be reserved at the given pointer location.
 *
 * \param value CRC value to append
 * \param ptr pointer to 2 byte memory location to store value.
 *
 */
void crc16_append_value(uint16_t value, void *ptr)
{
	*(uint8_t*)ptr = (value >> 8);
	*((uint8_t*)ptr+1) = value;
}

/**
 * \brief Set initial CRC value for next CRC calculation
 *
 * \note Value is reset to 0 when CRC calculation completes.
 *
 * \param value Initial CRC value
 */
void crc_set_initial_value(uint32_t value)
{
	crc_initial_value = value;
}

#ifdef DMA_SUPPORTED
/**
 * \brief Start CRC calculations on data passing through a DMA channel
 *
 * This function resets and sets up the CRC module to do CRC calculations on
 * data passing through chosen DMA channel.
 * Once a DMA channel is selected as the source, the CRC module will
 * continuously generate the CRC on the data passing through the DMA channel.
 * The checksum is available for readout once the DMA transaction is completed
 * or aborted. A CRC can be performed not only on communication data, but also
 * on data in SRAM or I/O memory by passing these data through a DMA channel.
 * If the latter is done, the destination register for the DMA data can be the
 * data input (DATAIN) register in the CRC module.
 *
 * The checksum can be read and the CRC module disabled with \ref
 * crc_dma_checksum_stop.
 *
 * \note The DMA channel must be set up separately
 *
 * \param dma_channel DMA channel to use (0 to N-1)
 * \param crc_16_32   enum to indicate whether CRC-32 or CRC-16 shall be used
 */
void crc_dma_checksum_start(uint8_t dma_channel, enum crc_16_32_t crc_16_32)
{
	// Reset CRC module
	crc_reset();

	if (crc_16_32 == CRC_32BIT) {
		// Enable CRC-32
		crc_32_enable();
	}

	// Set DMA channel as source
	switch (dma_channel) {
	case 0:
		crc_set_source(CRC_SOURCE_DMAC0_gc);
		break;
	case 1:
		crc_set_source(CRC_SOURCE_DMAC1_gc);
		break;
	#if defined(CRC_SOURCE_DMAC2_gc)
	case 2:
		crc_set_source(CRC_SOURCE_DMAC2_gc);
		break;
	#endif
	#if defined(CRC_SOURCE_DMAC3_gc)
	case 3:
		crc_set_source(CRC_SOURCE_DMAC3_gc);
		break;
	#endif
	default:
		// Nonexisting DMA channel - do nothing
		break;
	}
}
#endif

/**
 * \brief Disable CRC module and return checksum after DMA transfer
 *
 * \note This function must be called after DMA transfer is initiated.
 *
 * \return checksum checksum value
 */
uint32_t crc_dma_checksum_stop(void)
{
	return crc_checksum_complete();
}

/**
 * \brief Perform a CRC-32 calculation on the entire flash memory, on only
 * the application section, on only the boot section, or on a selectable range
 * of the flash memory.
 *
 * This function setups the type of CRC to perform and configures the memory
 * range to perform the CRC on, and starts the CRC via the NVM module.
 * When the calculation is done, the function disables the CRC module and
 * returns the checksum.
 * For CRC on the boot or application section, start address and length are not
 * needed.
 *
 * \note In the flash range mode, an even number of bytes is read. If the user
 * selects a range with an odd number of bytes, an extra byte will be read, and
 * the checksum will not correspond to the selected range.
 *
 * \param crc_type      what kind of CRC to do perform on the NVM module: \ref
 *                      CRC_FLASH_RANGE, \ref CRC_BOOT or \ref CRC_APP
 * \param flash_addr    address of first byte in flash to perform CRC on
 * \param length        number of bytes to perform CRC on
 *
 * \return checksum    CRC-32 checksum
 */
uint32_t crc_flash_checksum(NVM_CMD_t crc_type, flash_addr_t
		flash_addr, uint32_t length)
{
	if ((length == 0) && (crc_type == CRC_FLASH_RANGE)) {
		return 0;
	}

	Assert((crc_type == CRC_FLASH_RANGE) || (crc_type == CRC_BOOT) ||
			(crc_type == CRC_APP));

	// Initialize CRC for flash
	crc_reset();
	/* Set CRC32 enable bit to ensure correct reading of the resulting
	 * checksum */
	crc_32_enable();
	crc_set_source(CRC_SOURCE_FLASH_gc);

	if (crc_type == CRC_FLASH_RANGE) {
		nvm_issue_flash_range_crc(flash_addr, flash_addr + length - 1);
	} else {
		nvm_issue_command(crc_type);
	}

	// Disable CRC and return checksum
	return crc_checksum_complete();
}

/**
 * \brief Calculate the CRC checksum for data in a buffer
 *
 * This function initializes the CRC module, calculates the checksum for the
 * selected number of bytes in a data buffer, disables the module and returns
 * the calculated checksum.
 *
 * \param data          data buffer to perform CRC on
 * \param len           the number of bytes to perform CRC on
 * \param crc_16_32     enum to indicate whether CRC-32 or CRC-16 shall be used
 *
 * \return checksum checksum
 */
uint32_t crc_io_checksum(void *data, uint16_t len, enum crc_16_32_t crc_16_32)
{
	// Initialize CRC calculations on I/O interface
	crc_io_checksum_byte_start(crc_16_32);

	// Write data to DATAIN register
	while (len--) {
		crc_io_checksum_byte_add(*(uint8_t*)data);
		data = (uint8_t*)data + 1;
	}

	// Return checksum
	return crc_io_checksum_byte_stop();
}

/**
 * \brief Reset CRC module and set source to I/O interface
 *
 * This function initializes the CRC module, and \ref crc_io_checksum_byte_add
 * can be used to add bytes to calculate checksum for. When any number of bytes
 * have been added, the \ref crc_io_checksum_byte_stop can be called to disable
 * the module and get the calculated checksum.
 *
 * \param crc_16_32     enum to indicate whether CRC-32 or CRC-16 shall be used
 */
void crc_io_checksum_byte_start(enum crc_16_32_t crc_16_32)
{
	// Initialize CRC calculations on I/O interface
	crc_reset();
	// Enable CRC-32 if chosen
	if (crc_16_32 == CRC_32BIT) {
		crc_32_enable();
	}
	// Enable CRC module using the I/O interface
	crc_set_source(CRC_SOURCE_IO_gc);
}

/**
 * \brief Disable CRC module and return checksum
 *
 * This function stops the CRC calculation for bytes added with the \ref
 * crc_io_checksum_byte_add and returns the calculated checksum.
 *
 * \retval checksum checksum
 *
 */
uint32_t crc_io_checksum_byte_stop(void)
{
	// Signal CRC complete
	CRC_STATUS |= CRC_BUSY_bm;

	// Stop CRC and return checksum
	return crc_checksum_complete();
}
