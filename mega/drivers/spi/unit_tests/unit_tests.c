/**
 * \file
 *
 * \brief Unit tests for SPI driver of megaRF
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#include <asf.h>
#include <conf_test.h>

/**
 * \mainpage
 *
 * \section intro Introduction
 * This is the unit test application for the SPI driver.
 * It consists of test cases for the following functionality:
 * - Setting baudrate divisor
 * - Testing the SPI transfer
 *
 * \section files Main Files
 * - \ref unit_tests.c
 * - \ref conf_test.h
 * - \ref conf_board.h
 * - \ref conf_clock.h
 * - \ref conf_usart_serial.h
 *
 * \section device_info Device Info
 * All AVR MEGARF devices can be used.
 * This example has been tested with the following setup:
 * - atmega128rfa1_stk600-rc128x_rfx
 *
 * \section description Description of the unit tests
 * See the documentation for the individual unit test functions \ref
 * unit_tests.c "here" for detailed descriptions of the tests.
 *
 * \section dependencies Dependencies
 * This example depends directly on the following modules:
 * - \ref test_suite_group
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */

/* ! \name Unit test configuration */
/* @{ */

/**
 * \def CONF_TEST_SPI
 * \brief SPI instance to perform unit tests on
 */

/**
 * \def CONF_TEST_SPI_BAUDRATE
 * \brief SPI baudrate selection
 */

/**
 * \def CONF_TEST_SPI_MODE
 * \brief SPI clock mode selection
 */

/**
 * \def CONF_TEST_USART
 * \brief USART to redirect STDIO to
 */

/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of USART
 */

/**
 * \def CONF_TEST_CHARLENGTH
 * \brief Character length (bits) of USART
 */

/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of USART
 */

/**
 * \def CONF_TEST_STOPBITS
 * \brief Stopbit configuration of USART
 */
/* @} */

/* Buffer size. */
#define DATA_BUFFER_SIZE             0x05

/* Dummy data to initiate the SPI transfer */
#define SPI_MASTER_DUMMY             0xFF

/* Device selection IO pin */
#define SPI_DEVICE_SELECT            IOPORT_CREATE_PIN(PORTB, 0)

/* First 4 Bytes are commands for buffer write, after that data to written to
 * memory */
static const uint8_t data_buffer_write[]
	= {0x84, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

/* Data to be written in memory */
static const uint8_t data_sent[] = {0x01, 0x02, 0x03, 0x04, 0x05};

/* Command for flash write */
static const uint8_t data_flash_write[] = {0x83, 0x00, 0x00, 0x00};

/* Command for flash read */
static const uint8_t data_flash_read[]
	= {0xd2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Flash read data buffer*/
static uint8_t data_received[DATA_BUFFER_SIZE];

/**
 * \brief Get SPI module baudrate divisor value
 *
 * \return The divisor value the SPI module is configured to
 */
static uint8_t get_spi_divisor(void)
{
	uint8_t divisor = 0;

	switch (SPCR & SPI_PRESCALER_gm) {
	case SPI_PRESCALER_DIV4_gc:
		divisor = 4;
		break;

	case SPI_PRESCALER_DIV16_gc:
		divisor = 16;
		break;

	case SPI_PRESCALER_DIV64_gc:
		divisor = 64;
		break;

	case SPI_PRESCALER_DIV128_gc:
		divisor = 128;
		break;
	}

	if (SPSR & SPI_CLK2X_bm) {
		divisor >>= 1;
	}

	return divisor;
}

/**
 * \brief Test baudrate setting
 *
 * This test sets different baudrates and test if the divisor is set as
 * expected.
 *
 * \param test Current test case.
 */
static void run_baudrate_set_test(const struct test_case *test)
{
	/* A struct list of test sets to perform */
	struct test_set {
		uint32_t baudrate;
		uint8_t divisor;
		int8_t ret;
	}
	test_set[] = {
		/*  First we test the possible divisors*/
		{ 4000000, 2, 1},
		{ 2000000, 4, 1},
		{ 1000000, 8, 1},
		{  500000, 16, 1},
		{  250000, 32, 1},
		{  125000, 64, 1},
		{   62500, 128, 1},
		/* Test baudrate very close to one higher divisor */
		{ 3999999, 4, 1},
		/* Test that too low speed requested fails */
		{  10000, 128, -1}
	};
	int8_t ret;
	uint8_t i;

	/* Enable SPI clock and module */
	sysclk_enable_peripheral_clock(&CONF_TEST_SPI);
	spi_enable(&CONF_TEST_SPI);

	/* Loop through the test set and test each case */
	for (i = 0; i < (sizeof(test_set) / sizeof(test_set[0])); i++) {
		ret = spi_set_baud_div(&CONF_TEST_SPI,
				test_set[i].baudrate, 8000000);
		test_assert_true(test, ret == test_set[i].ret,
				"For baudrate %ld got unexpected return value "
				"%d, expected %d",
				test_set[i].baudrate, ret, test_set[i].ret);

		/* No need to test divisor value when it fails */
		if (ret <= 0) {
			continue;
		}

		test_assert_true(test, get_spi_divisor() == test_set[i].divisor,
				"For baudrate %ld read divisor %d, expected %d",
				test_set[i].baudrate, get_spi_divisor(),
				test_set[i].divisor);
	}
}

/**
 * \brief Select given device on the SPI bus
 */
static void spi_select_device(void)
{
	ioport_set_pin_low(SPI_DEVICE_SELECT);
}

/**
 * \brief Deselect given device on the SPI bus
 */
static void spi_deselect_device(void)
{
	ioport_set_pin_high(SPI_DEVICE_SELECT);
}

/**
 * \brief Send a sequence of bytes to a SPI device
 *
 * Received bytes on the SPI bus are discarded.
 *
 * \param spi Base address of the SPI instance.
 * \param data   data buffer to write
 * \param len    Length of data
 *
 * \pre SPI device must be selected with spi_select_device() first
 */
static status_code_t spi_write(volatile void *spi, const uint8_t *data,
		size_t len)
{
	while (len) {
		spi_put(spi, *data++);

		while (!spi_is_tx_ok(spi)) {
		}

		len--;
	}

	return STATUS_OK;
}

/**
 * \brief Receive a sequence of bytes from a SPI device
 *
 * All bytes sent out on SPI bus are sent as value 0.
 *
 * \param spi Base address of the SPI instance.
 * \param data   data buffer to read
 * \param len    Length of data
 *
 * \pre SPI device must be selected with spi_select_device() first
 */
static status_code_t spi_read(volatile void *spi, uint8_t *data, size_t len)
{
	while (len) {
		spi_put(spi, SPI_MASTER_DUMMY); /* Dummy write */

		while (!spi_is_tx_ok(spi)) {
		}

		*data = spi_get(spi);

		data++;
		len--;
	}

	return STATUS_OK;
}

/**
 * \brief Read data from flash
 *
 * Read command is sent to read the data from specific page of the flash
 *
 * \param test Current test case.
 */
static void flash_read_data(void)
{
	/* Select the DF memory */
	spi_select_device();

	/* Send the flash read command */
	spi_write(&CONF_TEST_SPI, data_flash_read, 8);

	/* Get the data from flash */
	spi_read(&CONF_TEST_SPI, data_received, DATA_BUFFER_SIZE);

	/* Deselect the checked DF memory. */
	spi_deselect_device();
}

/**
 * \brief Test SPI transfer
 *
 * In this test SPI master sends write command to write set of data and send
 * read command to get the data back from external flash.
 *
 * \param test Current test case.
 */
static void run_spi_transfer_test(const struct test_case *test)
{
	bool write_enable = false;

	/* Enable SPI peripheral clock */
	sysclk_enable_peripheral_clock(&CONF_TEST_SPI);

	/* Enable SPI is master mode */
	spi_enable_master_mode(&CONF_TEST_SPI);

	/* Set the baud rate */
	spi_set_baud_div(&CONF_TEST_SPI, CONF_TEST_SPI_BAUDRATE,
			sysclk_get_cpu_hz());

	/* Set the clock mode */
	spi_set_clock_mode(&CONF_TEST_SPI, CONF_TEST_SPI_MODE);

	/* Enable SPI */
	spi_enable(&CONF_TEST_SPI);

	/* Read the data from Page 0 of flash. This is to avoid write of same
	 * data again and to reduce the flash write during testing */
	flash_read_data();

	/* Check the read data */
	for (uint8_t i = 0; i < DATA_BUFFER_SIZE; i++) {
		if (data_received[i] != data_sent[i]) {
			write_enable = true;
		}

		/* Clear receive data buffer */
		data_received[i] = 0;
	}

	/* If first time flash is getting used data will be 0xff. So write known
	 * data */
	if (write_enable) {
		/* Select the DF memory */
		spi_select_device();

		/* Send the buffer write command followed by data to written */
		spi_write(&CONF_TEST_SPI, data_buffer_write, 9);

		/* Deselect the DF memory */
		spi_deselect_device();

		/* Flash delay */
		for (volatile uint16_t i = 0; i < 30000; i++) {
		}

		/* Select the DF memory */
		spi_select_device();

		/* Send the flash write command to write the previously sent
		 * data to flash */
		spi_write(&CONF_TEST_SPI, data_flash_write, 4);

		/* Deselect the DF memory */
		spi_deselect_device();

		/* Flash delay */
		for (volatile uint16_t i = 0; i < 30000; i++) {
		}
	}

	/* Read the data from flash. */
	flash_read_data();

	/* Check the read data */
	for (uint8_t i = 0; i < DATA_BUFFER_SIZE; i++) {
		test_assert_true(test, data_received[i] == data_sent[i],
				"read spi data %d, expected %d",
				data_received[i],
				data_sent[i]);
	}
}

/**
 * \brief Run SPI driver unit tests
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_TEST_BAUDRATE,
		.charlength   = CONF_TEST_CHARLENGTH,
		.paritytype   = CONF_TEST_PARITY,
		.stopbits     = CONF_TEST_STOPBITS,
	};

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define all the test cases */
	DEFINE_TEST_CASE(baudrate_set_test, NULL, run_baudrate_set_test, NULL,
			"Baudrate set test");

	DEFINE_TEST_CASE(data_transfer_test, NULL, run_spi_transfer_test, NULL,
			"SPI data transfer test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(spi_tests) = {
		&baudrate_set_test,
		&data_transfer_test,
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(spi_suite, spi_tests, "MEGARF SPI driver test suite");

	/* Run all tests in the test suite */
	test_suite_run(&spi_suite);

	while (1) {
		/* Busy-wait forever. */
	}
}
