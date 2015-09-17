/**
 *
 * \file
 *
 * \brief FreeRTOS TWI EEPROM test tasks
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
#include "semphr.h"

/* Atmel library includes. */
#include <freertos_twi_master.h>

/* Demo includes. */
#include "demo-tasks.h"

#if (defined confINCLUDE_TWI_EEPROM_TASK)

/*-----------------------------------------------------------*/

/* Clock speed to use. */
#define TWI_CLOCK_HZ            400000

/* The size of the EEPROM in bytes. */
#define TOTAL_EEPROM_SIZE       65536

/* The size of each page in the EEPROM. */
#define PAGE_SIZE               128

/* The maximum time the EEPROM takes to commit its RAM buffer. */
#define INTERNAL_WRITE_DELAY    (7 / portTICK_RATE_MS)

/*-----------------------------------------------------------*/

/*
 * Function that implements the task.  The task fills an EEPROM with a known
 * pattern, then reads back from the EEPROM to ensure the data read back matches
 * that written.
 */
static void twi_eeprom_task(void *pvParameters);

/*
 * Write the data held in data_buffer to the page passed in as the function
 * parameter.  The entire page is written.
 */
static void write_page_to_eeprom(freertos_twi_if freertos_twi, uint16_t page);

/*
 * Read EEPROM data from the page passed in the function parameter into
 * data_buffer.  The entire page is read.
 */
static void read_page_from_eeprom(freertos_twi_if freertos_twi,
		uint16_t page);

/* A buffer large enough to hold a complete page of data. */
static uint8_t data_buffer[PAGE_SIZE];

/* Used to latch errors found during the execution of the example. */
static uint32_t error_detected = pdFALSE;

/* eeprom_test_passed is set to pdPASS if the test completes without an error
being latched in error_detected.  This is used by other tasks to determine if
the test was successful or not. */
static uint32_t eeprom_test_passed = pdFALSE;

/* If twi_use_asynchronous_api is set to pdTRUE, then the example will use the
fully asynchronous FreeRTOS API.  Otherwise the example will use the blocking
FreeRTOS API (other tasks will execute while the application task is blocked). */
portBASE_TYPE twi_use_asynchronous_api;

/* The notification semaphore is only created and used when the fully
asynchronous FreeRTOS API is used. */
xSemaphoreHandle twi_notification_semaphore = NULL;

/*-----------------------------------------------------------*/

void create_twi_eeprom_test_task(Twi *twi_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority,
		portBASE_TYPE set_asynchronous_api)
{
	freertos_twi_if freertos_twi;

	/* blocking_driver_options is used if set_asynchronous_api is passed in
	as 0. */
	const freertos_peripheral_options_t blocking_driver_options = {
		NULL,											/* This peripheral does not need a receive buffer, so this parameter is just set to NULL. */
		0,												/* There is no Rx buffer, so the rx buffer size is not used. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the TWI interrupts. */
		TWI_I2C_MASTER,									/* Communicating with the EEPROM requires the TWI to be configured as an I2C master. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX | WAIT_TX_COMPLETE | WAIT_RX_COMPLETE)	/* The blocking driver is to be used, so WAIT_TX_COMPLETE and WAIT_RX_COMPLETE are set. */
	};

	/* asynchronous_driver_options is used if set_asynchronous_api is passed
	in as 1. */
	const freertos_peripheral_options_t asynchronous_driver_options = {
		NULL,											/* This peripheral does not need a receive buffer, so this parameter is just set to NULL. */
		0,												/* There is no Rx buffer, so the rx buffer size is not used. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the TWI interrupts. */
		TWI_I2C_MASTER,									/* Communicating with the EEPROM requires the TWI to be configured as an I2C master. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX)	/* The asynchronous driver is to be used, so WAIT_TX_COMPLETE and WAIT_RX_COMPLETE are not set. */
	};

	/* Remember if the asynchronous or blocking API is being used. */
	twi_use_asynchronous_api = set_asynchronous_api;

	/* The freertos_peripheral_options_t structure used to initialize the
	FreeRTOS driver differs depending on the twi_use_asynchronous_api setting. */
	if (twi_use_asynchronous_api == pdFALSE) {
		/* Initialize the FreeRTOS driver for blocking operation.  The
		peripheral clock is configured in this function call. */
		freertos_twi = freertos_twi_master_init(twi_base,
				&blocking_driver_options);
	} else {
		/* Initialize the FreeRTOS driver for asynchronous operation.  The
		peripheral clock is configured in this function call. */
		freertos_twi = freertos_twi_master_init(twi_base,
				&asynchronous_driver_options);

		/* Asynchronous operation requires a notification semaphore.  First,
		create the semaphore. */
		vSemaphoreCreateBinary(twi_notification_semaphore);

		/* Check the semaphore was created. */
		configASSERT(twi_notification_semaphore);

		/* Then set the semaphore into the correct initial state. */
		xSemaphoreTake(twi_notification_semaphore, 0);
	}

	/* Check the port was initialized successfully. */
	configASSERT(freertos_twi);

	/* Configure the TWI bus parameters.  Do this after calling
	freertos_twi_master_init(). */
	twi_set_speed(twi_base, TWI_CLOCK_HZ, sysclk_get_cpu_hz());

	/* Create the task as described above. */
	xTaskCreate(twi_eeprom_task, (const signed char *const) "Tx",
			stack_depth_words, (void *) freertos_twi, task_priority,
			NULL);
}

/*-----------------------------------------------------------*/

static void twi_eeprom_task(void *pvParameters)
{
	freertos_twi_if freertos_twi;
	portBASE_TYPE page;
	const portBASE_TYPE max_page = TOTAL_EEPROM_SIZE / PAGE_SIZE;
	size_t byte_in_page;
	uint8_t byte_value;

	/* The already open TWI port is passed in as the task parameter. */
	freertos_twi = (freertos_twi_if) pvParameters;

	/* Fill the EEPROM with 0xaa, one page at a time. */
	memset(data_buffer, 0xaa, PAGE_SIZE);
	for (page = 0; page < max_page; page++) {
		write_page_to_eeprom(freertos_twi, page);
	}

	/* Check that the data is read back from the EEPROM as 0xaa.  First make
	 * sure that the buffer contains a value other than 0xaa to be sure that
	 * it
	 * does not coincidentally already hold the expected data. */
	for (page = 0; page < max_page; page++) {
		memset((void *) data_buffer, 0xff, PAGE_SIZE);
		read_page_from_eeprom(freertos_twi, page);

		/* Check each byte in the page is as expected. */
		for (byte_in_page = 0; byte_in_page < PAGE_SIZE;
				byte_in_page++) {
			if (data_buffer[byte_in_page] != 0xaa) {
				error_detected = pdTRUE;
			}
		}
	}

	/* Fill the EEPROM with 0x55, one page at a time. */
	memset(data_buffer, 0x55, PAGE_SIZE);
	for (page = 0; page < max_page; page++) {
		write_page_to_eeprom(freertos_twi, page);
	}

	/* Check that the data is read back from the EEPROM as 0x55.  First make
	sure that the buffer contains a value other than 0x55 to be sure that it
	does not coincidentally already hold the expected data. */
	for (page = 0; page < max_page; page++) {
		memset((void *) data_buffer, 0xff, PAGE_SIZE);
		read_page_from_eeprom(freertos_twi, page);

		/* Check each byte in the page is as expected. */
		for (byte_in_page = 0; byte_in_page < PAGE_SIZE;
				byte_in_page++) {
			if (data_buffer[byte_in_page] != 0x55) {
				error_detected = pdTRUE;
			}
		}
	}

	/* Fill the EEPROM using different values in each position (the value
	 * will wrap). */
	byte_value = 0;
	for (page = 0; page < max_page; page++) {
		for (byte_in_page = 0; byte_in_page < PAGE_SIZE;
				byte_in_page++) {
			data_buffer[byte_in_page] = byte_value;
			byte_value++;
		}

		write_page_to_eeprom(freertos_twi, page);
	}

	/* Check each byte read back matches that expected once more. */
	byte_value = 0;
	for (page = 0; page < max_page; page++) {
		memset((void *) data_buffer, 0x00, PAGE_SIZE);
		read_page_from_eeprom(freertos_twi, page);

		/* Check each byte in the page is as expected. */
		for (byte_in_page = 0; byte_in_page < PAGE_SIZE;
				byte_in_page++) {
			if (data_buffer[byte_in_page] != byte_value) {
				error_detected = pdTRUE;
			}

			byte_value++;
		}
	}

	/* Complete.  To prevent wearing out the EEPROM, the task does not iterate,
	but instead deletes itself. */
	if (error_detected == pdFALSE) {
		eeprom_test_passed = pdTRUE;
	}

	/* A FreeRTOS task must not attempt to return from the function that
	implements it, but can delete itself. */
	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

static void write_page_to_eeprom(freertos_twi_if freertos_twi, uint16_t page)
{
	twi_packet_t write_parameters;
	uint16_t calculated_address;
	const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	write_parameters.chip = BOARD_AT24C_ADDRESS;
	write_parameters.buffer = data_buffer;
	write_parameters.length = PAGE_SIZE;

	/* The internal EEPROM address being written to is calculated from the page
	number. */
	calculated_address = page * PAGE_SIZE;
	write_parameters.addr[0] = (uint8_t) ((calculated_address >> 8) & 0xff);
	write_parameters.addr[1] = (uint8_t) (calculated_address & 0xff);
	write_parameters.addr_length = 2;

	if (twi_use_asynchronous_api == pdFALSE) {
		/* The blocking API is being used.  Other tasks will execute while the
		write operation is in progress. */
		if (freertos_twi_write_packet(freertos_twi, &write_parameters,
				max_block_time_ticks) != STATUS_OK) {
			error_detected = pdTRUE;
		}
	} else {
		/* The fully asynchronous API is being used.  The notification semaphore
		is used to indicate when the TWI transfer is complete. */
		if (freertos_twi_write_packet_async(freertos_twi,
				&write_parameters, max_block_time_ticks,
				twi_notification_semaphore) != STATUS_OK) {
			error_detected = pdTRUE;
		}

		/* ... other processing can be done here while the TWI write is in
		progress ... */

		/* Ensure the transaction is complete before proceeding further.  Other
		tasks will execute if this call causes this task to enter the Blocked
		state. */
		if (xSemaphoreTake(twi_notification_semaphore,
				max_block_time_ticks) != pdPASS) {
			error_detected = pdTRUE;
		}
	}

	/* Wait for the EEPROM to complete programming its internal memory.
	Note this is a very crude way of doing this, the EEPROM status
	register could be polled to determine when it was finished. */
	vTaskDelay(INTERNAL_WRITE_DELAY);
}

/*-----------------------------------------------------------*/

static void read_page_from_eeprom(freertos_twi_if freertos_twi, uint16_t page)
{
	twi_packet_t read_parameters;
	uint16_t calculated_address;
	const portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	read_parameters.chip = BOARD_AT24C_ADDRESS;
	read_parameters.buffer = data_buffer;
	read_parameters.length = PAGE_SIZE;

	/* The internal EEPROM address being read from is calculated from the page
	number. */
	calculated_address = page * PAGE_SIZE;
	read_parameters.addr[0] = (uint8_t)((calculated_address >> 8) & 0xff);
	read_parameters.addr[1] = (uint8_t)(calculated_address & 0xff);
	read_parameters.addr_length = 2;

	if (twi_use_asynchronous_api == pdFALSE) {
		/* The blocking API is being used.  Other tasks will execute while the
		read operation is in progress. */
		if (freertos_twi_read_packet(freertos_twi, &read_parameters,
				max_block_time_ticks) != STATUS_OK) {
			error_detected = pdTRUE;
		}
	} else {
		/* The fully asynchronous API is being used.  The notification semaphore
		is used to indicate when the TWI transfer is complete. */
		if (freertos_twi_read_packet_async(freertos_twi,
				&read_parameters, max_block_time_ticks,
				twi_notification_semaphore) != STATUS_OK) {
			error_detected = pdTRUE;
		}

		/* ... other processing can be done here while the TWI read is in
		progress ... */

		/* Ensure the transaction is complete before proceeding further.  Other
		tasks will execute if this call causes this task to enter the Blocked
		state. */
		if (xSemaphoreTake(twi_notification_semaphore,
				max_block_time_ticks) != pdPASS) {
			error_detected = pdTRUE;
		}
	}
}

/*-----------------------------------------------------------*/

portBASE_TYPE did_twi_eeprom_test_pass(void)
{
	portBASE_TYPE return_value;
	const portTickType twenty_five_seconds = 25000 / portTICK_RATE_MS;

	if (eeprom_test_passed == pdTRUE) {
		return_value = pdPASS;
	} else {
		/* Ensure enough time has passed for the EEPROM test to fail or stall
		before returning an error. */
		if (xTaskGetTickCount() > twenty_five_seconds) {
			return_value = pdFAIL;
		} else {
			return_value = pdPASS;
		}
	}

	return return_value;
}

/*-----------------------------------------------------------*/

#endif
