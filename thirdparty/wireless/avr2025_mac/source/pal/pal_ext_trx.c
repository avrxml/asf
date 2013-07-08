/*
 * @file pal.c
 *
 * @brief Performs interface functionalities between the TAL layer and ASF
 *drivers
 *  Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "board.h"
#include "spi_master.h"
#include "pal_ext_trx.h"
#include "pal.h"
#include "delay.h"
#include "interrupt.h"
#include "conf_pal.h"

static irq_handler_t irq_hdl_trx = NULL;
struct spi_device SPI_AT86RFX_DEVICE = {
	/* ! Board specific select id */
	.id = AT86RFX_SPI_CS
};

AT86RFX_ISR()
{
	/*Clearing the RF interrupt*/
	pal_trx_irq_flag_clr();

	/*Calling the interrupt routines*/
	if (irq_hdl_trx) {
		irq_hdl_trx();
	}
}

void pal_spi_init(void)
{
	/* Initialize SPI in master mode to access the transceiver */
	spi_master_init(AT86RFX_SPI);
	spi_master_setup_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE, SPI_MODE_0,
			AT86RFX_SPI_BAUDRATE, 0);
	spi_enable(AT86RFX_SPI);
	AT86RFX_INTC_INIT();
}

uint8_t pal_trx_reg_read(uint8_t addr)
{
	uint8_t register_value = 0;

	/*Saving the current interrupt status & disabling the global interrupt
	 **/
	ENTER_CRITICAL_REGION();

	/* Prepare the command byte */
	addr |= READ_ACCESS_COMMAND;

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &addr, 1);

	/* Do dummy read for initiating SPI read */
	spi_read_packet(AT86RFX_SPI, &register_value, 1);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/*Restoring the interrupt status which was stored & enabling the global
	 *interrupt */
	LEAVE_CRITICAL_REGION();

	return register_value;
}

void pal_trx_reg_write(uint8_t addr, uint8_t data)
{
	/*Saving the current interrupt status & disabling the global interrupt
	 **/
	ENTER_CRITICAL_REGION();

	/* Prepare the command byte */
	addr |= WRITE_ACCESS_COMMAND;

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/* Send the Read command byte */
	spi_write_packet(AT86RFX_SPI, &addr, 1);

	/* Write the byte in the transceiver data register */
	spi_write_packet(AT86RFX_SPI, &data, 1);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/*Restoring the interrupt status which was stored & enabling the global
	 *interrupt */
	LEAVE_CRITICAL_REGION();
}

void pal_trx_irq_init(FUNC_PTR trx_irq_cb)
{
	/*
	 * Set the handler function.
	 * The handler is set before enabling the interrupt to prepare for
	 *spurious
	 * interrupts, that can pop up the moment they are enabled
	 */
	irq_hdl_trx = (irq_handler_t)trx_irq_cb;
}

uint8_t pal_trx_bit_read(uint8_t addr, uint8_t mask, uint8_t pos)
{
	uint8_t ret;
	ret = pal_trx_reg_read(addr);
	ret &= mask;
	ret >>= pos;
	return ret;
}

void pal_trx_bit_write(uint8_t reg_addr, uint8_t mask, uint8_t pos,
		uint8_t new_value)
{
	uint8_t current_reg_value;
	current_reg_value = pal_trx_reg_read(reg_addr);
	current_reg_value &= ~mask;
	new_value <<= pos;
	new_value &= mask;
	new_value |= current_reg_value;
	pal_trx_reg_write(reg_addr, new_value);
}

void pal_trx_frame_read(uint8_t *data, uint8_t length)
{
	uint8_t temp;

	/*Saving the current interrupt status & disabling the global interrupt
	 **/
	ENTER_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	temp = TRX_CMD_FR;

	/* Send the command byte */
	spi_write_packet(AT86RFX_SPI, &temp, 1);

	spi_read_packet(AT86RFX_SPI, data, length);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/*Restoring the interrupt status which was stored & enabling the global
	 *interrupt */
	LEAVE_CRITICAL_REGION();
}

void pal_trx_frame_write(uint8_t *data, uint8_t length)
{
	uint8_t temp;
	/*Saving the current interrupt status & disabling the global interrupt
	 **/
	ENTER_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	temp = TRX_CMD_FW;

	/* Send the command byte */
	spi_write_packet(AT86RFX_SPI, &temp, 1);

	spi_write_packet(AT86RFX_SPI, data, length);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/*Restoring the interrupt status which was stored & enabling the global
	 *interrupt */
	LEAVE_CRITICAL_REGION();
}

/**
 * @brief Writes data into SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver
 *
 * @param addr Start address in the SRAM for the write operation
 * @param data Pointer to the data to be written into SRAM
 * @param length Number of bytes to be written into SRAM
 */
void pal_trx_sram_write(uint8_t addr, uint8_t *data, uint8_t length)
{
	uint8_t temp;
	/*Saving the current interrupt status & disabling the global interrupt
	 **/
	ENTER_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/* Send the command byte */
	temp = TRX_CMD_SW;

	/* Send the command byte */
	spi_write_packet(AT86RFX_SPI, &temp, 1);
	while (!spi_is_tx_empty(AT86RFX_SPI)) {
	}

	/* Send the address from which the write operation should start */
	spi_write_packet(AT86RFX_SPI, &addr, 1);
	while (!spi_is_tx_empty(AT86RFX_SPI)) {
	}

	spi_write_packet(AT86RFX_SPI, data, length);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/*Restoring the interrupt status which was stored & enabling the global
	 *interrupt */
	LEAVE_CRITICAL_REGION();
}

/**
 * @brief Reads data from SRAM of the transceiver
 *
 * This function reads from the SRAM of the transceiver
 *
 * @param[in] addr Start address in SRAM for read operation
 * @param[out] data Pointer to the location where data stored
 * @param[in] length Number of bytes to be read from SRAM
 */
void pal_trx_sram_read(uint8_t addr, uint8_t *data, uint8_t length)
{
	uint8_t temp;

	PAL_WAIT_1_US(); /* wap_rf4ce */

	/*Saving the current interrupt status & disabling the global interrupt
	 **/
	ENTER_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	temp = TRX_CMD_SR;

	/* Send the command byte */
	spi_write_packet(AT86RFX_SPI, &temp, 1);
	while (!spi_is_tx_empty(AT86RFX_SPI)) {
	}

	/* Send the command byte */
	spi_write_packet(AT86RFX_SPI, &addr, 1);
	while (!spi_is_tx_empty(AT86RFX_SPI)) {
	}

	/* Send the address from which the read operation should start */
	/* Upload the received byte in the user provided location */
	spi_read_packet(AT86RFX_SPI, data, length);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/*Restoring the interrupt status which was stored & enabling the global
	 *interrupt */
	LEAVE_CRITICAL_REGION();
}

/**
 * @brief Writes and reads data into/from SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver and
 * simultaneously reads the bytes.
 *
 * @param addr Start address in the SRAM for the write operation
 * @param idata Pointer to the data written/read into/from SRAM
 * @param length Number of bytes written/read into/from SRAM
 */
void pal_trx_aes_wrrd(uint8_t addr, uint8_t *idata, uint8_t length)
{
	uint8_t *odata;
	uint8_t temp;

	PAL_WAIT_1_US(); /* wap_rf4ce */

	ENTER_TRX_REGION();

#ifdef NON_BLOCKING_SPI
	while (spi_state != SPI_IDLE) {
		/* wait until SPI gets available */
	}
#endif

	/* Start SPI transaction by pulling SEL low */
	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	/* Send the command byte */
	temp = TRX_CMD_SW;
	spi_write_packet(AT86RFX_SPI, &temp, 1);
	while (!spi_is_tx_empty(AT86RFX_SPI)) {
	}

	/* write SRAM start address */
	spi_write_packet(AT86RFX_SPI, &addr, 1);
	while (!spi_is_tx_empty(AT86RFX_SPI)) {
	}

	/* now transfer data */
	odata = idata;

	/* write data byte 0 - the obtained value in SPDR is meaningless */
	spi_write_packet(AT86RFX_SPI, idata++, 1);

	/* Reading Spi Data for the length specified */
	while (length > 0) {
		spi_write_packet(AT86RFX_SPI, idata++, 1);
		while (!spi_is_tx_empty(AT86RFX_SPI)) {
		}
		spi_read_single(AT86RFX_SPI, odata++);
		length--;
	}

	/* To get the last data byte, write some dummy byte */
	spi_read_packet(AT86RFX_SPI, odata, 1);

	/* Stop the SPI transaction by setting SEL high */
	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);

	LEAVE_TRX_REGION();
}
