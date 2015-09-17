/**
 * @file trx_access_2.c
 *
 * @brief Performs interface functionalities between the PHY layer and ASF
 * drivers
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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
 *
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "board.h"
#if SAMD || SAMR21
#include "spi.h"
#else
#include "spi_master.h"
#endif
#include "trx_access_2.h"
#include "tal.h"
#include "usart.h"
#include "sio2ncp.h"
#include "conf_sio2ncp.h"
#include "delay.h"
#include "interrupt.h"
#include "conf_pal.h"
static irq_handler_t irq_hdl_trx = NULL;

#if SAMD || SAMR21
struct spi_slave_inst_config slave_dev_config;
struct spi_config config;
struct spi_module master;
struct spi_slave_inst slave;
uint16_t dummy_read;
#else
struct spi_device SPI_AT86RFX_DEVICE = {
	/* ! Board specific select id */
	.id = AT86RFX_SPI_CS
};
#endif

#if SAMD || SAMR21
void AT86RFX_ISR(void);

void AT86RFX_ISR(void)
#else
AT86RFX_ISR()
#endif

{
	/*Clearing the RF interrupt*/
	trx_irq_flag_clr();

	/*Calling the interrupt routines*/
	if (irq_hdl_trx) {
		irq_hdl_trx();
	}
}

void trx_spi_init(void)
{
	/* Initialize SPI in master mode to access the transceiver */
	#if SAMD || SAMR21
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = AT86RFX_SPI_CS;
	spi_attach_slave(&slave, &slave_dev_config);
	spi_get_config_defaults(&config);
	config.mux_setting = AT86RFX_SPI_SERCOM_MUX_SETTING;
	config.mode_specific.master.baudrate = AT86RFX_SPI_BAUDRATE;
	config.pinmux_pad0 = AT86RFX_SPI_SERCOM_PINMUX_PAD0;
	config.pinmux_pad1 = AT86RFX_SPI_SERCOM_PINMUX_PAD1;
	config.pinmux_pad2 = AT86RFX_SPI_SERCOM_PINMUX_PAD2;
	config.pinmux_pad3 = AT86RFX_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&master, AT86RFX_SPI, &config);
	spi_enable(&master);

	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);
	eint_chan_conf.gpio_pin = AT86RFX_IRQ_PIN;
	eint_chan_conf.gpio_pin_mux = AT86RFX_IRQ_PINMUX;
	eint_chan_conf.gpio_pin_pull      = EXTINT_PULL_DOWN;
	eint_chan_conf.wake_if_sleeping    = true;
	eint_chan_conf.filter_input_signal = false;
	eint_chan_conf.detection_criteria  = EXTINT_DETECT_RISING;
	extint_chan_set_config(AT86RFX_IRQ_CHAN, &eint_chan_conf);
	extint_register_callback(AT86RFX_ISR, AT86RFX_IRQ_CHAN,
			EXTINT_CALLBACK_TYPE_DETECT);

	#else
	spi_master_init(AT86RFX_SPI);
	spi_master_setup_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE, SPI_MODE_0,
			AT86RFX_SPI_BAUDRATE, 0);
	spi_enable(AT86RFX_SPI);
	AT86RFX_INTC_INIT();
	#endif
}

void PhyReset(void)
{
	/* Ensure control lines have correct levels. */
	RST_HIGH();
	SLP_TR_LOW();

	/* Wait typical time of timer TR1. */
	delay_us(330);

	RST_LOW();
	delay_us(10);
	RST_HIGH();
}

#define TAL_DEFAULT_BB_IRQ_MASK     (BB_IRQ_TXFE | BB_IRQ_RXFE)
#define TAL_DEFAULT_RF_IRQ_MASK     RF_IRQ_ALL_IRQ

/*Temporary set of definitions to test parallel PER Test in 215*/
/* todo : Implement spi using dma to reduce cpu dependency for writing 2000bytes
 * payload for spi */
#define DISABLE_TRX_INTERRUPT()     trx_reg_write( RG_BBC0_IRQM, 0); \
	trx_reg_write(BB_BASE_ADDR_OFFSET + RG_BBC0_IRQM, 0); \
	trx_reg_write( RG_RF09_IRQM, 0); \
	trx_reg_write(RF_BASE_ADDR_OFFSET + RG_RF09_IRQM, 0);

#define ENABLE_TRX_INTERRUPT()         trx_reg_write( RG_BBC0_IRQM, \
		TAL_DEFAULT_BB_IRQ_MASK); \
	trx_reg_write(BB_BASE_ADDR_OFFSET + RG_BBC0_IRQM, \
		TAL_DEFAULT_BB_IRQ_MASK); \
	trx_reg_write( RG_RF09_IRQM, TAL_DEFAULT_RF_IRQ_MASK); \
	trx_reg_write(RF_BASE_ADDR_OFFSET + RG_RF09_IRQM, \
		TAL_DEFAULT_RF_IRQ_MASK);

void trx_read(uint16_t addr, uint8_t *data, uint16_t length)
{
	/*Saving the current interrupt status & disabling the global interrupt
	**/
	DISABLE_TRX_INTERRUPT();

	/* Prepare the command byte */
	addr |= READ_ACCESS_COMMAND; /* Read Command */

	uint8_t reg_addr;

#if SAMD || SAMR21
	/* Start SPI transaction by pulling SEL low */
	spi_select_slave(&master, &slave, true);

	reg_addr = addr >> 8;

	/* Send the command byte */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	reg_addr = addr;
	/* Send the command byte */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	while (length--) {
		while (!spi_is_ready_to_write(&master)) {
		}
		spi_write(&master, 0);
		while (!spi_is_write_complete(&master)) {
		}
		/* Dummy read since SPI RX is double buffered */
		while (!spi_is_ready_to_read(&master)) {
		}
		spi_read(&master, &temp);
		*data = temp;
		data++;
	}
	/* Stop the SPI transaction by setting SEL high */
	spi_select_slave(&master, &slave, false);

#else
	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	reg_addr = addr >> 8;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	reg_addr = addr;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	/* Do dummy read for initiating SPI read */
	spi_read_packet(AT86RFX_SPI, data, length);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);
#endif

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */

	ENABLE_TRX_INTERRUPT();
}

void trx_write(uint16_t addr, uint8_t *data, uint16_t length)
{
	/*Saving the current interrupt status & disabling the global interrupt
	**/

	DISABLE_TRX_INTERRUPT();

	/* Prepare the command byte */
	addr |= WRITE_ACCESS_COMMAND; /* Write Command */

	uint8_t reg_addr;

#if SAMD || SAMR21
	/* Start SPI transaction by pulling SEL low */
	spi_select_slave(&master, &slave, true);

	reg_addr = addr >> 8;

	/* Send the command byte */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	reg_addr = addr;
	/* Send the command byte */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	while (length--) {
		while (!spi_is_ready_to_write(&master)) {
		}
		spi_write(&master, *data++);
		while (!spi_is_write_complete(&master)) {
		}
		/* Dummy read since SPI RX is double buffered */
		while (!spi_is_ready_to_read(&master)) {
		}
		spi_read(&master, &dummy_read);
	}

	/* Stop the SPI transaction by setting SEL high */
	spi_select_slave(&master, &slave, false);
#else
	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	reg_addr = addr >> 8;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	reg_addr = addr;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	uint32_t i = 0;
	uint8_t val;

	while (length) {
		val = data[i];
		while (!spi_is_tx_ready(AT86RFX_SPI)) {
		}
		spi_write_single(AT86RFX_SPI, val);
		i++;
		length--;
	}
	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);
#endif

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	ENABLE_TRX_INTERRUPT();
}

uint8_t trx_reg_read(uint16_t addr)
{
#if SAMD || SAMR21
	uint16_t register_value = 0;
#else
	uint8_t register_value = 0;
#endif

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_CRITICAL_REGION();

	/* Prepare the command byte */
	addr |= READ_ACCESS_COMMAND; /* Read Command */

	uint8_t reg_addr;

#if SAMD || SAMR21
	/* Start SPI transaction by pulling SEL low */
	spi_select_slave(&master, &slave, true);

	reg_addr = addr >> 8;

	/* Send the Read command byte */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	reg_addr = addr;

	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, 0);
	while (!spi_is_write_complete(&master)) {
	}
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &register_value);

	/* Stop the SPI transaction by setting SEL high */
	spi_select_slave(&master, &slave, false);

#else
	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	reg_addr = addr >> 8;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	reg_addr = addr;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	/* Do dummy read for initiating SPI read */
	spi_read_packet(AT86RFX_SPI, &register_value, 1);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);
#endif

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_CRITICAL_REGION();

	return register_value;
}

void trx_reg_write(uint16_t addr, uint8_t data)
{
	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_CRITICAL_REGION();

	/* Prepare the command byte */
	addr |= WRITE_ACCESS_COMMAND; /* Write Command */

	uint8_t reg_addr;

#if SAMD || SAMR21
	/* Start SPI transaction by pulling SEL low */
	spi_select_slave(&master, &slave, true);

	reg_addr = addr >> 8;
	/* Send the Read command byte */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);
	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	reg_addr = addr;
	/* Write the byte in the transceiver data register */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, reg_addr);

	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);
	/* Write the byte in the transceiver data register */
	while (!spi_is_ready_to_write(&master)) {
	}
	spi_write(&master, data);

	while (!spi_is_write_complete(&master)) {
	}
	/* Dummy read since SPI RX is double buffered */
	while (!spi_is_ready_to_read(&master)) {
	}
	spi_read(&master, &dummy_read);

	/* Stop the SPI transaction by setting SEL high */
	spi_select_slave(&master, &slave, false);
#else
	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	reg_addr = addr >> 8;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	reg_addr = addr;

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &reg_addr, 1);

	/* Write the byte in the transceiver data register */
	spi_write_packet(AT86RFX_SPI, &data, 1);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);
#endif

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_CRITICAL_REGION();
}

void trx_irq_init(FUNC_PTR trx_irq_cb)
{
	/*
	 * Set the handler function.
	 * The handler is set before enabling the interrupt to prepare for
	 * spurious
	 * interrupts, that can pop up the moment they are enabled
	 */
	irq_hdl_trx = (irq_handler_t)trx_irq_cb;
}

uint8_t trx_bit_read(uint16_t addr, uint8_t mask, uint8_t pos)
{
	uint8_t ret;
	ret = trx_reg_read(addr);
	ret &= mask;
	ret >>= pos;
	return ret;
}

void trx_bit_write(uint16_t reg_addr, uint8_t mask, uint8_t pos,
		uint8_t new_value)
{
	uint8_t current_reg_value;
	current_reg_value = trx_reg_read(reg_addr);
	current_reg_value &= ~mask;
	new_value <<= pos;
	new_value &= mask;
	new_value |= current_reg_value;
	trx_reg_write(reg_addr, new_value);
}
