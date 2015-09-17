/**
 *
 * \file
 *
 * \brief FreeRTOS SPI Flash test task
 *
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

/* Standard includes. */
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Atmel library includes. */
#include "freertos_spi_master.h"
#include "sysclk.h"

/* Demo includes. */
#include "demo-tasks.h"

#if (defined confINCLUDE_SPI_FLASH_TASK)

/*-----------------------------------------------------------*/

/* The size of the AT25 dataflash. */
#define TOTAL_FLASH_SIZE        (4 * 1024 * 1024)

/* The page size of AT25DF series is always 256. */
#define PAGE_SIZE               256

/** Test buffer size */
#define TEST_DATA_SIZE          1024

/** Test start address */
#define TEST_BLOCK_ADDR         0

/* The command always goes in the first byte of the message. */
#define COMMAND_INDEX           0

/* The address goes in bytes 2, 3 and 4 of the message. */
#define ADDRESS_INDEX           1

/* A block time used after a write to allow the flash to write the data sent
 * over the SPI bus into its internal memory. */
#define INTERNAL_WRITE_DELAY    (10 / portTICK_RATE_MS)

/* Max number when the chip selects are directly connected to peripheral device.
 **/
#define NONE_CHIP_SELECT_VALUE     0x0f

/* Instructions/commands that can be sent to the flash. */
/** Write status register command code */
#define STATUS  0x01 /* Write status register. */
#define WRITE   0x02 /* Write instruction. */
#define READ    0x03 /* Read instruction. */
#define RDSR    0x05 /* Read status register instruction. */
#define WREN    0x06 /* Write enable instruction. */
#define ERASE   0xD8 /* 64k block erase instruction. */

/* AT25 ready/busy status bit */
#define STATUS_RDYBSY           (1 << 0)

/* AT25 ready bit */
#define STATUS_RDYBSY_READY     (0 << 0)

/* SPI clock speed. */
#define BAUD_RATE               12000000

/* Chip select. */
#define CHIP_SELECT             CONF_EXAMPLE_CS

/* Clock polarity. */
#define CLOCK_POLARITY          0

/* Clock phase. */
#define CLOCK_PHASE             1

/* Delay before SPCK. */
#define DELAY_BEFORE            0x40

/* Delay between consecutive transfers. */
#define DELAY_BETWEEN           0x10

/*
 * Function that implements the task.  The task fills the flash with a known
 * pattern, then reads back from the flash to ensure the read data matches that
 * written.
 */
static void spi_flash_task(void *pvParameters);

/*
 * Send the command stored in cmd_buffer and read data_size bytes from the AT25.
 */
static void at25dfx_send_command(freertos_spi_if freertos_spi,
		uint8_t *cmd_buffer, uint32_t cmd_size,
		uint8_t *data_buffer, uint32_t data_size);

/*
 * Generates an SPI frame that contains the correct control codes to unprotect
 * the AT25.
 */
static void at25dfx_unprotect(freertos_spi_if freertos_spi);

/*
 * Generates an SPI frame that contains the correct control codes to erase a 64k
 * block.
 */
static void at25dfx_erase_block(freertos_spi_if freertos_spi,
		uint32_t address);

/*
 * Generates an SPI frame that contains the correct control codes to read the
 * AT25 status. Several attempts will be made to wait for the AT25 to be ready.
 * If passed the maximum attempt limit, return anyway (the task will be more
 * likely to fail and the user will be notified via the led status).
 */
static void at25dfx_wait_ready(freertos_spi_if freertos_spi);

/*
 * Generates an SPI frame that contains the correct control codes to read the
 * data. It calls at25dfx_spi_read() to perform the actual data transfer.
 */
static void at25dfx_read(freertos_spi_if freertos_spi, uint32_t size,
		uint32_t address);

/*
 * Read number_of_bytes of data to data_buffer on the SPI bus. This function
 * selects either the blocking or asynchronous FreeRTOS functions, depending on
 * the value of spi_use_asynchronous_api.
 */
static void at25dfx_spi_read(freertos_spi_if freertos_spi, uint8_t *buffer,
		uint32_t number_of_bytes);

/*
 * Generates an SPI frame that contains the correct control codes and the data
 * to be written. It calls at25dfx_spi_write() to perform the actual data
 * transfer.
 */
static void at25dfx_write(freertos_spi_if freertos_spi, uint32_t size,
		uint32_t address);

/*
 * Writes number_of_bytes of data from data_buffer on the SPI bus. This function
 * selects either the blocking or asynchronous FreeRTOS functions, depending on
 * the value of spi_use_asynchronous_api.
 */
static void at25dfx_spi_write(freertos_spi_if freertos_spi, uint8_t *buffer,
		uint32_t number_of_bytes);

/* Command buffer (1 byte for cmd and 3 bytes for address) */
static uint8_t cmd_buffer[4];

/* Data buffer */
static uint8_t data_buffer[TEST_DATA_SIZE];

/* Used to latch error found during the execution of the example. */
static uint32_t error_detected = pdFALSE;

/* flash_test_passed is set to pdPASS if the test completes without an error
being latched in error_detected.  This is used by other tasks to determine if
the test was successful or not. */
static uint32_t flash_test_passed = pdFALSE;

/* If spi_use_asynchronous_api is set to pdTRUE, then the example will use the
fully asynchronous FreeRTOS API.  Otherwise the example will use the blocking
FreeRTOS API (other tasks will execute while the application task is blocked). */
portBASE_TYPE spi_use_asynchronous_api;

/* The notification semaphore is only created and used when the fully
asynchronous FreeRTOS API is used. */
xSemaphoreHandle spi_notification_semaphore = NULL;

/*-----------------------------------------------------------*/

void create_spi_flash_test_task(Spi *spi_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority,
		portBASE_TYPE set_asynchronous_api)
{
	freertos_spi_if freertos_spi;

	/* blocking_driver_options is used if set_asynchronous_api is passed in as 0. */

	freertos_peripheral_options_t blocking_driver_options = {
		NULL,											/* This peripheral does not need a receive buffer, so this parameter is just set to NULL. */
		0,												/* There is no Rx buffer, so the rx buffer size is not used. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the SPI interrupts. */
		SPI_MASTER,										/* Communicating with the flash requires the peripheral to be configured as an SPI master. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX | WAIT_TX_COMPLETE | WAIT_RX_COMPLETE)	/* The blocking driver is to be used, so WAIT_TX_COMPLETE and WAIT_RX_COMPLETE are set. */
	};

	/* async_driver_options is used if set_asynchronous_api is passed in as 0. */
	freertos_peripheral_options_t async_driver_options = {
		NULL,											/* This peripheral does not need a receive buffer, so this parameter is just set to NULL. */
		0,												/* There is no Rx buffer, so the rx buffer size is not used. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the SPI interrupts. */
		SPI_MASTER,										/* Communicating with the flash requires the peripheral to be configured as an SPI master. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX)	/* The asynchronous driver is to be used, so WAIT_TX_COMPLETE and WAIT_RX_COMPLETE are clear. */
	};

	/* Remember if the asynchronous or blocking API is being used. */
	spi_use_asynchronous_api = set_asynchronous_api;

	/* The freertos_peripheral_options_t structure used to initialize the
	FreeRTOS driver differs depending on the spi_use_asynchronous_api setting. */
	if (spi_use_asynchronous_api == pdFALSE) {
		/* Initialize the FreeRTOS driver for blocking operation.  The
		 * peripheral clock is configured in this function call. */
		freertos_spi = freertos_spi_master_init(spi_base,
				&blocking_driver_options);
	} else {
		/* Initialize the FreeRTOS driver for asynchronous operation.  The
		peripheral clock is configured in this function call. */
		freertos_spi = freertos_spi_master_init(spi_base,
				&async_driver_options);

		/* Asynchronous operation requires a notification semaphore.  First,
		create the semaphore. */
		vSemaphoreCreateBinary(spi_notification_semaphore);

		/* Check the semaphore was created. */
		configASSERT(spi_notification_semaphore);

		/* Then set the semaphore into the correct initial state. */
		xSemaphoreTake(spi_notification_semaphore, 0);
	}

	/* Check the port was initialized successfully. */
	configASSERT(freertos_spi);

	spi_disable(spi_base);
	spi_set_clock_polarity(spi_base, CHIP_SELECT, CLOCK_POLARITY);
	spi_set_clock_phase(spi_base, CHIP_SELECT, CLOCK_PHASE);
	spi_set_bits_per_transfer(spi_base, CHIP_SELECT, SPI_CSR_BITS_8_BIT);
	spi_set_baudrate_div(spi_base, CHIP_SELECT,
			(sysclk_get_cpu_hz() / BAUD_RATE));
	spi_set_transfer_delay(spi_base, CHIP_SELECT, DELAY_BEFORE,
			DELAY_BETWEEN);
	spi_configure_cs_behavior(spi_base, CHIP_SELECT, SPI_CS_KEEP_LOW);

	spi_set_peripheral_chip_select_value(spi_base, spi_get_pcs(CHIP_SELECT));

	spi_enable(spi_base);

	/* Create the task as described above. */
	xTaskCreate(spi_flash_task, (const signed char *const)"Tx",
			stack_depth_words, (void *)freertos_spi, task_priority,
			NULL);
}

/*-----------------------------------------------------------*/

static void spi_flash_task(void *pvParameters)
{
	freertos_spi_if freertos_spi;

	/* The already open SPI port is passed in as the task parameter. */
	freertos_spi = (freertos_spi_if)pvParameters;

	/* AT25 is protected by default after init, so unprotect it. */
	at25dfx_unprotect(freertos_spi);

	/* Prepare the buffer in order to fill the AT25 with 0xaa, one page at a
	time. */
	memset(data_buffer, 0xaa, sizeof(data_buffer));

	/* Erase a 64k block at address TEST_BLOCK_ADDR. */
	at25dfx_erase_block(freertos_spi, TEST_BLOCK_ADDR);

	/* Write the data to the SerialFlash */
	at25dfx_write(freertos_spi, TEST_DATA_SIZE, TEST_BLOCK_ADDR);

	/* Reset the internal buffer to ensure data will be read from the AT25. */
	memset(data_buffer, 0xff, sizeof(data_buffer));

	/* Read back the same memory area from the AT25 and compare with the
	expected value. */
	at25dfx_read(freertos_spi, TEST_DATA_SIZE, TEST_BLOCK_ADDR);

	for (uint32_t i = 0; i < TEST_DATA_SIZE; i++) {
		if (data_buffer[i] != 0xaa) {
			error_detected = pdTRUE;
		}
	}

	/* Complete. To prevent warning out the flash, the task does not iterate,
	but instead deletes itself. */
	if (error_detected == pdFALSE) {
		flash_test_passed = pdTRUE;
	}

	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

portBASE_TYPE did_spi_flash_test_pass(void)
{
	portBASE_TYPE return_value;
	const portTickType one_second = 1000 / portTICK_RATE_MS;

	if (flash_test_passed == pdTRUE) {
		return_value = pdPASS;
	} else {
		/* Ensure enough time has passed for the flash test to fail or stall
		before returning an error. */
		if (xTaskGetTickCount() > one_second) {
			return_value = pdFAIL;
		} else {
			return_value = pdPASS;
		}
	}

	return return_value;
}

/*-----------------------------------------------------------*/

static void at25dfx_send_command(freertos_spi_if freertos_spi,
		uint8_t *cmd_buffer, uint32_t cmd_size,
		uint8_t *data_buffer, uint32_t data_size)
{
	/* Select the AT25 chip. */
	spi_set_peripheral_chip_select_value(freertos_spi,
			(~(1U << CHIP_SELECT)));

	/* Send actual command. */
	at25dfx_spi_write(freertos_spi, cmd_buffer, cmd_size);

	if (data_size > 0) {
		if (cmd_buffer[COMMAND_INDEX] == WRITE ||
				cmd_buffer[COMMAND_INDEX] == STATUS) {
			at25dfx_spi_write(freertos_spi, data_buffer, data_size);
		} else {
			at25dfx_spi_read(freertos_spi, data_buffer, data_size);
		}
	}

	/* Deselect the AT25 chip - assert all lines; no peripheral is selected. */
	spi_set_peripheral_chip_select_value(freertos_spi, NONE_CHIP_SELECT_VALUE);
	/* Last transfer, so de-assert the current NPCS if CSAAT is set. */
	spi_set_lastxfer(freertos_spi);
}

/*-----------------------------------------------------------*/

static void at25dfx_unprotect(freertos_spi_if freertos_spi)
{
	uint8_t status = 0;

	/* Set the write enable latch (1 byte command). */
	cmd_buffer[COMMAND_INDEX] = WREN;
	at25dfx_send_command(freertos_spi, cmd_buffer, 1, data_buffer, 0);

	/* Set the STATUS command. */
	cmd_buffer[COMMAND_INDEX] = STATUS;
	at25dfx_send_command(freertos_spi, cmd_buffer, 1, &status, 1);

	/* Wait for the AT25 to be ready. */
	at25dfx_wait_ready(freertos_spi);
}

/*-----------------------------------------------------------*/

static void at25dfx_erase_block(freertos_spi_if freertos_spi,
		uint32_t address)
{
	/* Set the write enable latch (1 byte command). */
	cmd_buffer[COMMAND_INDEX] = WREN;
	at25dfx_send_command(freertos_spi, cmd_buffer, 1, data_buffer, 0);

	/* Set the ERASE command and address. (4 bytes command). */
	cmd_buffer[COMMAND_INDEX] = ERASE;
	cmd_buffer[ADDRESS_INDEX] = (uint8_t)((address & 0xff0000U) >> 16);
	cmd_buffer[ADDRESS_INDEX + 1] = (uint8_t)((address & 0xff00U) >> 8);
	cmd_buffer[ADDRESS_INDEX + 2] = (uint8_t)((address & 0xffU));
	at25dfx_send_command(freertos_spi, cmd_buffer, 4, data_buffer, 0);

	/* Wait for the AT25 to be ready. */
	at25dfx_wait_ready(freertos_spi);
}

/*-----------------------------------------------------------*/

static void at25dfx_wait_ready(freertos_spi_if freertos_spi)
{
	const portTickType xDelay = 100 / portTICK_RATE_MS;
	uint32_t wait_timeout = 30;
	uint8_t status = 0;

	/* Wait for the AT25 to be ready, if not delay the task and try again. */
	/* If still not ready after several attempt, forget it. */
	while (wait_timeout) {
		/* Set the READ Status command. (1 byte command). */
		cmd_buffer[COMMAND_INDEX] = RDSR;
		at25dfx_send_command(freertos_spi, cmd_buffer, 1, &status, 1);

		/* Exit when the device is ready */
		if ((status & STATUS_RDYBSY) == STATUS_RDYBSY_READY) {
			break;
		}

		/* Give some rest to the AT25 and let the scheduler decide what
		to do. */
		vTaskDelay(xDelay);

		/* Decrease the number of attempts (timeout). */
		wait_timeout -= 1;
	}
}

/*-----------------------------------------------------------*/

static void at25dfx_read(freertos_spi_if freertos_spi, uint32_t size,
		uint32_t address)
{
	/* Set the READ command and address. (4 bytes command). */
	cmd_buffer[COMMAND_INDEX] = READ;
	cmd_buffer[ADDRESS_INDEX] = (uint8_t)((address & 0xff0000U) >> 16);
	cmd_buffer[ADDRESS_INDEX + 1] = (uint8_t)((address & 0xff00U) >> 8);
	cmd_buffer[ADDRESS_INDEX + 2] = (uint8_t)((address & 0xffU));
	at25dfx_send_command(freertos_spi, cmd_buffer, 4, data_buffer, size);
}

/*-----------------------------------------------------------*/

static void at25dfx_spi_read(freertos_spi_if freertos_spi, uint8_t *buffer,
		uint32_t number_of_bytes)
{
	const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	if (spi_use_asynchronous_api == pdFALSE) {
		/* The blocking API is being used.  Other tasks will run while the SPI
		read is in progress. */
		if (freertos_spi_read_packet(freertos_spi, buffer,
				number_of_bytes,
				max_block_time_ticks) != STATUS_OK) {
			error_detected = pdTRUE;
		}
	} else {
		/* The fully asynchronous API is being used.  The notification
		semaphore is used to indicate when the SPI read is complete. */
		if (freertos_spi_read_packet_async(freertos_spi, buffer,
				number_of_bytes, max_block_time_ticks,
				spi_notification_semaphore) != STATUS_OK) {
			error_detected = pdTRUE;
		}

		/* ... other processing can be done here while the SPI read is
		in progress ... */

		/* Ensure the transaction is complete before proceeding further.
		Other tasks will execute if this call causes this task to enter the
		Blocked state. */
		if (xSemaphoreTake(spi_notification_semaphore,
				max_block_time_ticks) != pdPASS) {
			error_detected = pdTRUE;
		}
	}
}

/*-----------------------------------------------------------*/

static void at25dfx_write(freertos_spi_if freertos_spi, uint32_t size,
		uint32_t address)
{
	uint32_t write_size;
	uint32_t written = 0;

	while (size > 0) {
		/* Compute the number of bytes to program in page */
		write_size = Min(size, PAGE_SIZE - (address % PAGE_SIZE));

		/* Set the write enable latch (1 byte command). */
		cmd_buffer[COMMAND_INDEX] = WREN;
		at25dfx_send_command(freertos_spi, cmd_buffer, 1, data_buffer, 0);

		/* Set the WRITE command and address (4 bytes command). */
		cmd_buffer[COMMAND_INDEX] = WRITE;
		cmd_buffer[ADDRESS_INDEX] = (uint8_t) ((address & 0xff0000U) >> 16);
		cmd_buffer[ADDRESS_INDEX + 1] = (uint8_t) ((address & 0xff00U) >> 8);
		cmd_buffer[ADDRESS_INDEX + 2] = (uint8_t) ((address & 0xffU));
		at25dfx_send_command(freertos_spi, cmd_buffer, 4,
				data_buffer + written, write_size);

		/* Compute remaining size and update pointer values. */
		written += write_size;
		size -= write_size;
		address += write_size;

		/* Wait for the AT25 to be ready. */
		at25dfx_wait_ready(freertos_spi);
	}
}

/*-----------------------------------------------------------*/

static void at25dfx_spi_write(freertos_spi_if freertos_spi, uint8_t *buffer,
		uint32_t number_of_bytes)
{
	const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	if (spi_use_asynchronous_api == pdFALSE) {
		/* The blocking API is being used.  Other tasks will execute while the
		SPI write is in progress. */
		if (freertos_spi_write_packet(freertos_spi, buffer,
				number_of_bytes,
				max_block_time_ticks) != STATUS_OK) {
			error_detected = pdTRUE;
		}
	} else {
		/* The fully asynchronous API is being used.  The notification
		semaphore is used to indicate when the SPI write is complete. */
		if (freertos_spi_write_packet_async(freertos_spi, buffer,
				number_of_bytes, max_block_time_ticks,
				spi_notification_semaphore) != STATUS_OK) {
			error_detected = pdTRUE;
		}

		/* ... other processing can be done here while the SPI write is in
		progress ... */

		/* Ensure the transaction is complete before proceeding further.  Other
		tasks will execute if this call causes this task to enter the Blocked
		state. */
		if (xSemaphoreTake(spi_notification_semaphore,
				max_block_time_ticks) != pdPASS) {
			error_detected = pdTRUE;
		}
	}
}

/*-----------------------------------------------------------*/

#endif
