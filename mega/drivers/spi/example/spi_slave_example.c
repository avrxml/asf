/**
 * \file
 *
 * \brief AVR MEGARF SPI Slave example
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use MEGARF SPI module in slave mode.
 *
 * \section files Main Files
 * - spi_slave_example.c: the example application.
 * - conf_spi_slave_example.h: configuration of the example
 *
 * \section spi_apiinfo drivers/spi API
 * The SPI driver API can be found \ref group_megarf_drivers_spi "here".
 *
 * \section deviceinfo Device Info
 * All AVR MEGARF devices can be used. This example has been tested
 * with the following setup:
 *   - STK600 with ATmega128RFA1 on STK600-RC128X_RFX.
 *
 * \section exampledescription Description of the example
 * In This example Usart is configured in SPI master mode and SPI module
 * is configured in slave mode. Usart SPI master will transfer the data
 * to slave. SPI slave also sends data to Usart master when master sends
 * the read command to slave. Received data validation is done on both the
 * sides. If the received data matches with the transmit data then LED will
 * glow.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <conf_spi_slave_example.h>
#include <asf.h>

/* Slave Read Command */
#define SLAVE_RD_CMD             0x9F

/* Buffer size */
#define DATA_BUFFER_SIZE         0x04

/* Define SPI Module pins */
#define SPI_SCK       IOPORT_CREATE_PIN(PORTB, 1)
#define SPI_MOSI      IOPORT_CREATE_PIN(PORTB, 2)
#define SPI_MISO      IOPORT_CREATE_PIN(PORTB, 3)
#define SPI_SS        IOPORT_CREATE_PIN(PORTB, 0)

/* Data master tx buffer */
uint8_t data_master_tx[DATA_BUFFER_SIZE] = {0x11, 0x22, 0x33, 0x44};

/* Data master tx buffer */
uint8_t data_master_rx[DATA_BUFFER_SIZE];

/* Data slave rx buffer */
uint8_t data_slave_rx[DATA_BUFFER_SIZE];

/* Data slave tx buffer */
uint8_t data_slave_tx[DATA_BUFFER_SIZE] = {0x55, 0x66, 0x77, 0x88};

uint8_t count = 0;

bool data_transfer = false;

struct usart_spi_device USART_SPI_DEVICE_EXAMPLE = {
	/* Board specific select ID. */
	.id = USART_SPI_DEVICE_EXAMPLE_ID
};

/**
 * \brief Interrupt callback for SPI interrupt
 *  \param none
 */
static void spi_interrupt_callback(void)
{
	uint8_t data = spi_get(SPI_SLAVE_EXAMPLE);

	if (data_transfer == true) {
		spi_put(SPI_SLAVE_EXAMPLE, data_slave_tx[count++]);
	} else {
		data_slave_rx[count++] = data;
		if (data_slave_rx[0] == SLAVE_RD_CMD) {
			data_transfer = true;
			count = 0;
			spi_put(SPI_SLAVE_EXAMPLE, data_slave_tx[count++]);
		}
	}
}

/*! \brief Initialize the SPI in slave mode and enable the spi interrupt.
 */
static void spi_slave_init(volatile void *spi, uint8_t mode)
{
	/* Enable Clock for SPI module */
	sysclk_enable_module(POWER_RED_REG0, PRSPI_bm);

	/* Configure SPI pins for slave */
	/* Set MISO as output high, and set MOSI, SS , SCK as input */

	gpio_configure_pin(SPI_SCK, IOPORT_DIR_INPUT);
	gpio_configure_pin(SPI_MOSI, IOPORT_DIR_INPUT);
	gpio_configure_pin(SPI_SS, IOPORT_DIR_INPUT);
	gpio_configure_pin(SPI_MISO, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);

	/* Set the clock mode */
	spi_set_clock_mode(spi, mode);

	/* Enable SPI as slave */
	spi_enable_slave_mode(spi);
        
        /* Set the interrupt call back */
        spi_set_interrupt_callback(spi_interrupt_callback);
          
	/* Enable SPI interrupt */
	spi_enable_interrupt(spi);
}

/*! \brief Usart configured as SPI master transfers data to SPI slave.
 */
static bool spi_usart_master_transfer(void)
{
	/* Select the Slave device */
	usart_spi_select_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE);

	/* Send the data to slave */
	usart_spi_write_packet(USART_SPI_EXAMPLE, data_master_tx,
			DATA_BUFFER_SIZE);

	/* Deselect the Slave device  */
	usart_spi_deselect_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE);

	/* Check the Slave received data with master tx data */
	for (uint8_t i = 0; i < DATA_BUFFER_SIZE; i++) {
		if (data_master_tx[i] == data_slave_rx[i]) {
			continue;
		} else {
			return false;
		}
	}

	return true;
}

/*! \brief SPI slave tranfers data to Usart SPI master
 */
static bool spi_slave_transfer(void)
{
	/* Select the Slave device */
	usart_spi_select_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE);

	/* Put the slave read command in master tx buffer */
	data_master_tx[0] = SLAVE_RD_CMD;

	count = 0;

	/* Send the Read command */
	usart_spi_write_packet(USART_SPI_EXAMPLE, data_master_tx, 1);

	/* Receive data from slave */
	usart_spi_read_packet(USART_SPI_EXAMPLE, data_master_rx,
			DATA_BUFFER_SIZE);

	/* Deselect the Slave device  */
	usart_spi_deselect_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE);

	/* Check the master received data with slave tx data */
	for (uint8_t i = 0; i < DATA_BUFFER_SIZE; i++) {
		if (data_master_rx[i] == data_slave_tx[i]) {
			continue;
		} else {
			return false;
		}
	}

	return true;
}

/*! \brief Main function.
 */
int main(void)
{
	sysclk_init();
	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();

	/* Config the USART_SPI in master mode. */
	usart_spi_init(USART_SPI_EXAMPLE);
	usart_spi_setup_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE,
			SPI_MODE_0, USART_SPI_EXAMPLE_BAUDRATE, 0);
	usart_spi_enable(USART_SPI_EXAMPLE);

	/* Config the SPI module in slave mode. */
	spi_slave_init(SPI_SLAVE_EXAMPLE, SPI_MODE_0);
	spi_enable(SPI_SLAVE_EXAMPLE);

	/* Enable global interrupt */
	cpu_irq_enable();

	/* Show the test result by LED. */
	if (spi_usart_master_transfer() == true && spi_slave_transfer() ==
			true) {
		ioport_set_pin_level(SPI_SLAVE_EXAMPLE_LED_PIN,
				IOPORT_PIN_LEVEL_LOW);
	} else {
		ioport_set_pin_level(SPI_SLAVE_EXAMPLE_LED_PIN,
				IOPORT_PIN_LEVEL_HIGH);
	}

	while (1) {
		/* Do nothing */
	}
}
