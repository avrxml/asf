/**
 * \file
 *
 * \brief AT30TSE75X driver.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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

#include "asf.h"
#include "at30tse75x.h"

void at30tse_init(void)
{
	twi_options_t opts = {
		.master_clk = sysclk_get_main_hz(),
		.speed = 10000,
		.smbus = 0
	};

	sysclk_enable_peripheral_clock(ID_TWI0);
	twi_master_init(TWI0, &opts);
}

uint8_t at30tse_eeprom_write(uint8_t *data, uint8_t length, uint8_t word_addr, uint8_t page)
{
	twi_packet_t packet = {
		// Internal chip addr
		// Byte addr in page (0-15) + 4 lower bits of page addr in EEPROM
		.addr[0] = (word_addr & 0x0F) | ((0x0F & page) << 4),
		.addr_length = 1,
		// Data buffer
		.buffer = data,
		.length = length,
		// Chip addr
		// TWI addr + 2 upper bytes of page addr.
		.chip = AT30TSE758_EEPROM_TWI_ADDR | ( (0x30 & page) >> 4 ),
	};

	return twi_master_write(TWI0, &packet);
}

uint8_t at30tse_eeprom_read(uint8_t *data, uint8_t length, uint8_t word_addr, uint8_t page)
{
	twi_packet_t packet = {
		// Internal chip addr
		// Byte addr in page (0-15) + 4 lower bits of page addr in EEPROM
		.addr[0] = (word_addr & 0x0F) | ((0x0F & page) << 4),
		.addr_length = 1,
		// Data buffer
		.buffer = data,
		.length = length,
		// Chip addr
		// TWI addr + 2 upper bytes of page addr.
		.chip = AT30TSE758_EEPROM_TWI_ADDR | ( (0x30 & page) >> 4 ),
	};

	return twi_master_read(TWI0, &packet);
}

volatile uint8_t resolution = AT30TSE_CONFIG_RES_9_bit;

uint8_t at30tse_read_register(uint8_t reg, uint8_t reg_type, uint8_t reg_size, uint8_t* buffer)
{
	twi_packet_t packet = {
		// Internal chip addr
		.addr[0] = reg | reg_type,
		.addr_length = 1,
		// Data buffer
		.buffer = buffer,
		.length = reg_size,
		// Chip addr
		.chip = AT30TSE_TEMPERATURE_TWI_ADDR
	};

	return twi_master_read(TWI0, &packet);
}

uint8_t at30tse_write_register(uint8_t reg, uint8_t reg_type, uint8_t reg_size, uint16_t reg_value)
{
	uint8_t data[2];
	twi_packet_t packet = {
		// Internal chip addr
		.addr[0] = reg | reg_type,
		.addr_length = 1,
		// Data buffer
		.buffer = data,
		.length = reg_size,
		// Chip addr
		.chip = AT30TSE_TEMPERATURE_TWI_ADDR
	};

	// cppcheck-suppress unreadVariable
	data[0] = 0x00FF & (reg_value >> 8);
	// cppcheck-suppress unreadVariable
	data[1] = 0x00FF & reg_value;

	return twi_master_write(TWI0, &packet);
}

uint8_t at30tse_write_config_register(uint16_t value)
{
	uint8_t error_code = at30tse_write_register(AT30TSE_CONFIG_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_CONFIG_REG_SIZE - 1, value);

	if (error_code == TWI_SUCCESS)
	{
		resolution = (value >> AT30TSE_CONFIG_RES_Pos) & (AT30TSE_CONFIG_RES_Msk >> AT30TSE_CONFIG_RES_Pos);
	}

	return error_code;
}

uint8_t at30tse_read_temperature(double *temperature)
{
	// Placeholder buffer to put temperature data in.
	uint8_t buffer[2];
	uint8_t error_code = 0;
	buffer[0] = 0;
	buffer[1] = 0;

	// Read the 16-bit temperature register.
	error_code = at30tse_read_register(AT30TSE_TEMPERATURE_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_TEMPERATURE_REG_SIZE, buffer);

	// Only convert temperature data if read success.
	if (error_code == TWI_SUCCESS)
	{
		uint16_t data = (buffer[0] << 8) | buffer[1];
		int8_t sign = 1;

		// Check if negative and clear sign bit.
		if (data & (1 << 15))
		{
			sign *= -1;
			data &= ~(1 << 15);
		}

		// Convert to temperature.
		switch (resolution)
		{
			case AT30TSE_CONFIG_RES_9_bit:
			data = (data >> 7);
			*(temperature) = data * sign * 0.5;
			break;
			case AT30TSE_CONFIG_RES_10_bit:
			data = (data >> 6);
			*(temperature) = data * sign * 0.25;
			break;
			case AT30TSE_CONFIG_RES_11_bit:
			data = (data >> 5);
			*(temperature) = data * sign * 0.125;
			break;
			case AT30TSE_CONFIG_RES_12_bit:
			data = (data >> 4);
			*(temperature) = data * sign * 0.0625;
			break;
			default:
			break;
		}
	}

	return error_code;
}
