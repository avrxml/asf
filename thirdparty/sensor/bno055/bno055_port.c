/**
 * \file
 *
 * \brief BNO055 extension board example for SAME70.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#include "bno055_port.h"

/* Instantiates a BNO055 software instance structure which retains
* chip ID, internal sensors IDs, I2C address and pointers
* to required functions (bus read/write and delay functions) */
struct bno055_t bno055;

void bno055_initialize(void)
{
	bno055.bus_write = bno055_i2c_bus_write;
	bno055.bus_read = bno055_i2c_bus_read;
	bno055.delay_msec = bno055_delay_msek;
	bno055.dev_addr = BNO055_I2C_SLAVE_ADDRESS;
	
#if BNO055_I2C_SLAVE_ADDRESS == BNO055_I2C_ADDR1
	ioport_set_pin_level(BNO055_PIN_SLAVE_ADDR_SELECT, false);
#else
	ioport_set_pin_level(BNO055_PIN_SLAVE_ADDR_SELECT, true);
#endif
	
	ioport_set_pin_level(BNO055_PIN_BOOT, true);
	bno055_reset();
	bno055_init(&bno055);
}

/*	
 *  \Brief: The function is used as I2C bus init
 */
void bno055_i2c_bus_init(void)
{
	twihs_options_t bno055_option;
	pmc_enable_periph_clk(BOARD_BNO055_ID_TWIHS);

	/* Configure the options of TWI driver */
	bno055_option.master_clk = sysclk_get_cpu_hz();
	bno055_option.speed      = BNO055_TWIHS_CLK;
	twihs_master_init(BOARD_BNO055_TWIHS, &bno055_option);
}

/*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
int8_t bno055_i2c_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int32_t ierror = BNO055_ZERO_U8X;

	twihs_packet_t p_packet;
	p_packet.chip = dev_addr;
	p_packet.addr[0] = reg_addr;
	p_packet.addr_length = 1;
	p_packet.buffer = reg_data;
	p_packet.length = cnt;
	
	ierror = twihs_master_write(BOARD_BNO055_TWIHS, &p_packet);

	return (int8_t)ierror;
}

 /*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
int8_t bno055_i2c_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int32_t ierror = BNO055_ZERO_U8X;
	
	twihs_packet_t p_packet;
	p_packet.chip = dev_addr;
	p_packet.addr[0] = reg_addr;
	p_packet.addr_length = 1;
	p_packet.buffer = reg_data;
	p_packet.length = cnt;
	
	ierror = twihs_master_read(BOARD_BNO055_TWIHS, &p_packet);

	return (int8_t)ierror;
}
/*	Brief : The delay routine
 *	\param : delay in ms
*/
void bno055_delay_msek(uint32_t msek)
{
	/*Here you can write your own delay routine*/
	delay_ms(msek);
}

void bno055_reset(void)
{
	ioport_set_pin_level(BNO055_PIN_RESET,  BNO055_RESET_ACTIVE);
	delay_ms(BNO055_RESET_DELAY_MS);
	ioport_set_pin_level(BNO055_PIN_RESET, !BNO055_RESET_ACTIVE);
	delay_ms(BNO055_RESET_DELAY_MS);
}

void extint_initialize(void (*handler_function)(void))
{
	/* Enable the peripheral clock for the BNO055 extension board interrupt pin. */
	pmc_enable_periph_clk(PIN_BNO055_EXT_INIERRUPT_ID);
	/* Configure PIOs as input pins. */
	pio_configure(PIN_BNO055_EXT_INIERRUPT_PIO, PIN_BNO055_EXT_INIERRUPT_TYPE,
	PIN_BNO055_EXT_INIERRUPT_MASK, PIN_BNO055_EXT_INIERRUPT_ATTR);
	/* Initialize PIO interrupt handler, interrupt on rising edge. */
	pio_handler_set(PIN_BNO055_EXT_INIERRUPT_PIO, PIN_BNO055_EXT_INIERRUPT_ID, PIN_BNO055_EXT_INIERRUPT_MASK,
	PIN_BNO055_EXT_INIERRUPT_ATTR, (void (*) (uint32_t, uint32_t))handler_function);
	/* Initialize and enable push button (PIO) interrupt. */
	pio_handler_set_priority(PIN_BNO055_EXT_INIERRUPT_PIO, PIOD_IRQn, 0);
	pio_enable_interrupt(PIN_BNO055_EXT_INIERRUPT_PIO, PIN_BNO055_EXT_INIERRUPT_MASK);
}

void bno055_gpio_config(void)
{
	/* Slave address */
	ioport_set_pin_dir(BNO055_PIN_SLAVE_ADDR_SELECT, IOPORT_DIR_OUTPUT);
	/* RGB leds */
	ioport_set_pin_dir(RGB_LED_G, IOPORT_DIR_OUTPUT);
	/* Boot */
	ioport_set_pin_dir(BNO055_PIN_BOOT, IOPORT_DIR_OUTPUT);
	/* Reset */
	ioport_set_pin_dir(BNO055_PIN_RESET, IOPORT_DIR_OUTPUT);
}