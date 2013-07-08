/**
 * \file
 *
 * \brief API driver for component AT24CXX.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
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

/**
 * \defgroup common_components_memory_eeprom_at24cxx_group EEPROM AT24Cxx Series
 *
 * Low-level driver for the AT24CXX Series EEPROM controller. This driver provides access to the main
 * features of the AT24CXX Series EEPROM.
 *
 * \{
 */

#include "at24cxx.h"
#include "ioport.h"
#include "delay.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \brief Reset AT24CXX.
 *
 * Send 9 clock cycles to reset memory state.
 *
 * \note IO mode is used in this function, so it can be used even if TWI is not
 *       enabled.
 */
void at24cxx_reset(void)
{
	int i;
	/* MEM reset
	 * a) Clock up to 9 cycles (use 100K)
	 * b) look for SDA high in each cycle while SCL is high and then
	 * c) Create a start condition as SDA is high
	 */
	/* - Enable pin output mode */
	ioport_set_pin_dir(BOARD_CLK_TWI_EEPROM, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(BOARD_CLK_TWI_EEPROM, 1);
	ioport_enable_pin(BOARD_CLK_TWI_EEPROM);
	for (i = 0; i < 10; i ++) {
		delay_us(5);
		ioport_set_pin_level(BOARD_CLK_TWI_EEPROM, 0);
		delay_us(5);
		ioport_set_pin_level(BOARD_CLK_TWI_EEPROM, 1);
	}
	/* - Restore pin peripheral mode */
	ioport_set_pin_mode(BOARD_CLK_TWI_EEPROM, BOARD_CLK_TWI_MUX_EEPROM);
	ioport_disable_pin(BOARD_CLK_TWI_EEPROM);
	/* - Start condition will do on R/W start */
}

/**
 * \brief Write single byte to AT24CXX.
 *
 * \param us_address Address of the byte to write.
 * \param uc_value Value that will be written to the specified address.
 *
 * \return AT24C_WRITE_SUCCESS if single byte was written, AT24C_WRITE_FAIL otherwise.
 */
uint32_t at24cxx_write_byte(uint16_t us_address, uint8_t uc_value)
{
	twi_package_t twi_package;

	/* Configure the data packet to be transmitted */
	twi_package.chip = BOARD_AT24C_ADDRESS;
	twi_package.addr[0] = (uint8_t)us_address;
	twi_package.addr[1] = (uint8_t)(us_address >> 8);
	twi_package.addr_length = AT24C_MEM_ADDR_LEN;
	twi_package.buffer = &uc_value;
	twi_package.length = 1;

	if (twi_master_write(BOARD_AT24C_TWI_INSTANCE, &twi_package) != TWI_SUCCESS) {
		return AT24C_WRITE_FAIL;
	}

	return AT24C_WRITE_SUCCESS;
}

/**
 * \brief Write multiple bytes continuously to AT24CXX.
 *
 * \param us_start_address Address of the first byte in transaction.
 * \param us_length Number of bytes to write.
 * \param p_wr_buffer Pointer to array where the bytes to be written are stored.
 *
 *  \return AT24C_WRITE_SUCCESS if single byte was written, AT24C_WRITE_FAIL otherwise.
 */
uint32_t at24cxx_write_continuous(uint16_t us_start_address,
		uint16_t us_length, uint8_t const *p_wr_buffer)
{
	twi_package_t twi_package;

	/* Configure the data packet to be transmitted */
	twi_package.chip = BOARD_AT24C_ADDRESS;
	twi_package.addr[0] = (uint8_t)us_start_address;
	twi_package.addr[1] = (uint8_t)(us_start_address >> 8);
	twi_package.addr_length = AT24C_MEM_ADDR_LEN;
	twi_package.buffer = (uint8_t *) p_wr_buffer;
	twi_package.length = us_length;

	if (twi_master_write(BOARD_AT24C_TWI_INSTANCE, &twi_package) != TWI_SUCCESS) {
		return AT24C_WRITE_FAIL;
	}

	return AT24C_WRITE_SUCCESS;
}

/**
 * \brief Read single byte from AT24CXX.
 *
 * \param us_address Address of the byte to read.
 * \param uc_read_byte Pointer to memory where the read byte will be stored.
 *
 * \return AT24C_READ_SUCCESS if one byte was read, AT24C_READ_FAIL otherwise.
 */
uint32_t at24cxx_read_byte(uint16_t us_address, uint8_t *p_rd_byte)
{
	twi_package_t twi_package;

	/* Configure the data packet to be received */
	twi_package.chip = BOARD_AT24C_ADDRESS;
	twi_package.addr[0] = (uint8_t)us_address;
	twi_package.addr[1] = (uint8_t)(us_address >> 8);
	twi_package.addr_length = AT24C_MEM_ADDR_LEN;
	twi_package.buffer = p_rd_byte;
	twi_package.length = 1;

	if (twi_master_read(BOARD_AT24C_TWI_INSTANCE, &twi_package) != TWI_SUCCESS) {
		return AT24C_READ_FAIL;
	}

	return AT24C_READ_SUCCESS;
}

/**
 * \brief Read multiple bytes continuously from AT24CXX.
 *
 * \param us_start_address Address of the first byte to read.
 * \param us_length Number of bytes to read.
 * \param p_rd_buffer Pointer to memory where the read bytes will be stored.
 *
 * \return AT24C_READ_SUCCESS if one byte was read, AT24C_READ_FAIL otherwise.
 */
uint32_t at24cxx_read_continuous(uint16_t us_start_address, uint16_t us_length,
		uint8_t * p_rd_buffer)
{
	twi_package_t twi_package;

	/* Configure the data packet to be received */
	twi_package.chip = BOARD_AT24C_ADDRESS;
	twi_package.addr[0] = (uint8_t)us_start_address;
	twi_package.addr[1] = (uint8_t)(us_start_address >> 8);
	twi_package.addr_length = AT24C_MEM_ADDR_LEN;
	twi_package.buffer = p_rd_buffer;
	twi_package.length = us_length;

	if (twi_master_read(BOARD_AT24C_TWI_INSTANCE, &twi_package) != TWI_SUCCESS) {
		return AT24C_READ_FAIL;
	}

	return AT24C_READ_SUCCESS;
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \}
 */
