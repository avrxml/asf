/**
 *
 * \file
 *
 * \brief FreeRTOS+CLI task implementation example
 *
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

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+ includes. */
#include "FreeRTOS_CLI.h"

/* Atmel library includes. */
#include <udi_cdc.h>
#include <udc.h>

/* Demo includes. */
#include "demo-tasks.h"

/* Semaphore used to signal the arrival of new data. */
static xSemaphoreHandle cdc_new_data_semaphore = NULL;

/* Flag used to indicated whether or not the CDC port is connected or not. */
static volatile bool cdc_connected = false;

#if (defined confINCLUDE_CDC_CLI)

/* Dimensions the buffer into which input characters are placed. */
#define MAX_INPUT_SIZE                  50

/* The size of the buffer provided to the USART driver for storage of received
 * bytes. */
#define RX_BUFFER_SIZE_BYTES            (50)

/* The maximum number of events the queue used to pass CDC events to the CLI
 * task can hold at any one time. */
#define EVENT_QUEUE_LENGTH              (5)

/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void usb_cdc_command_console_task(void *pvParameters);

/*-----------------------------------------------------------*/


/* Const strings used by the CLI interface. */
static const uint8_t *const new_line = (uint8_t *) "\r\n";
static const uint8_t *const line_separator = (uint8_t *) "\r\n[Press ENTER to execute the previous command again]\r\n>";

/* Mutex used to get access to the CDC port for transmitting. */
static xSemaphoreHandle access_mutex = NULL;

/*-----------------------------------------------------------*/

void create_usb_cdc_cli_task(uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority)
{
	/* Register the default CLI commands. */
	vRegisterCLICommands();

	/* Create the semaphore used to signal the arrival of new data. */
	vSemaphoreCreateBinary(cdc_new_data_semaphore);
	configASSERT(cdc_new_data_semaphore);

	/* Create the semaphore used to access the CDC port as it is written to from
	more than one task. */
	access_mutex = xSemaphoreCreateMutex();
	configASSERT(access_mutex);

	/* Create the USART CLI task. */
	xTaskCreate(	usb_cdc_command_console_task,		/* The task that implements the command console. */
					(const int8_t *const) "CDC_CLI",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,					/* The size of the stack allocated to the task. */
					NULL,								/* The parameter is not used. */
					task_priority,						/* The priority allocated to the task. */
					NULL);								/* Used to store the handle to the created task - in this case the handle is not required. */
}

/*-----------------------------------------------------------*/

static void usb_cdc_command_console_task(void *pvParameters)
{
	uint8_t received_char, input_index = 0, *output_string;
	static int8_t input_string[MAX_INPUT_SIZE],
			last_input_string[MAX_INPUT_SIZE];
	portBASE_TYPE returned_value;

	/* Just to remove compiler warnings. */
	(void) pvParameters;

	udc_start();

	if (udc_include_vbus_monitoring() == false) {
		/* VBUS monitoring is not available on this product.  Assume VBUS is
		present. */
		cli_vbus_event(true);
	}

	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console
	interface will be used at any one time. */
	output_string = (uint8_t *) FreeRTOS_CLIGetOutputBuffer();

	for (;;) {
		/* Wait for new data. */
		xSemaphoreTake(cdc_new_data_semaphore, portMAX_DELAY);

		/* Ensure mutually exclusive access is obtained as other tasks can write
		to the CLI. */
		xSemaphoreTake(access_mutex, portMAX_DELAY);

		/* While there are input characters. */
		while (udi_cdc_is_rx_ready() == true) {
			received_char = (uint8_t) udi_cdc_getc();

			/* Echo the character. */
			udi_cdc_putc(received_char);

			if (received_char == '\r') {
				/* Transmit a line separator, just to make the output easier to
				read. */
				udi_cdc_write_buf((void *) new_line,
						strlen((char *) new_line));

				/* See if the command is empty, indicating that the last command
				is to be executed again. */
				if (input_index == 0) {
					strcpy((char *) input_string,
							(char *) last_input_string);
				}

				/* Pass the received command to the command interpreter.  The
				command interpreter is called repeatedly until it returns pdFALSE as
				it might generate more than one string. */
				do {
					/* Get the string to write to the UART from the command
					interpreter. */
					returned_value = FreeRTOS_CLIProcessCommand(
							input_string,
							(int8_t *) output_string,
							configCOMMAND_INT_MAX_OUTPUT_SIZE);

					/* Transmit the generated string. */
					udi_cdc_write_buf((void *) output_string, strlen(
							(char *) output_string));
				} while (returned_value != pdFALSE);

				/* All the strings generated by the input command have been sent.
				Clear the input	string ready to receive the next command.
				Remember the command that was just processed first in case it is
				to be processed again. */
				strcpy((char *) last_input_string,
						(char *) input_string);
				input_index = 0;
				memset(input_string, 0x00, MAX_INPUT_SIZE);

				/* Start to transmit a line separator, just to make the output
				easier to read. */
				udi_cdc_write_buf((void *) line_separator, strlen(
						(char *) line_separator));
			} else {
				if (received_char == '\n') {
					/* Ignore the character. */
				} else if (received_char == '\b') {
					/* Backspace was pressed.  Erase the last character in the
					string - if any. */
					if (input_index > 0) {
						input_index--;
						input_string[input_index]
							= '\0';
					}
				} else {
					/* A character was entered.  Add it to the string
					entered so far.  When a \n is entered the complete
					string will be passed to the command interpreter. */
					if (input_index < MAX_INPUT_SIZE) {
						input_string[input_index] = received_char;
						input_index++;
					}
				}
			}
		}

		/* Finished with the CDC port, return the mutex until more characters
		arrive. */
		xSemaphoreGive(access_mutex);
	}
}

/*-----------------------------------------------------------*/

void cdc_cli_output(const uint8_t const *message_string)
{
	if (cdc_connected == true) {
		/* This call is outside of the CLI task, so ensure mutually exclusive
		access is obtained. */
		xSemaphoreTake(access_mutex, portMAX_DELAY);

		udi_cdc_write_buf((void *) message_string,
				strlen((char *) message_string));

		/* Finished writing to the CDC. */
		xSemaphoreGive(access_mutex);
	}
}

/*-----------------------------------------------------------*/

#endif

void cli_vbus_event(bool b_vbus_high)
{
	if (b_vbus_high == true) {
		udc_attach();
	} else {
		udc_detach();
	}
}

/*-----------------------------------------------------------*/

bool cli_cdc_enable(uint8_t port)
{
	(void) port;

	cdc_connected = true;
	return true;
}

/*-----------------------------------------------------------*/

void cli_cdc_rx_notify(uint8_t port)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	(void) port;

	/* Sanity check the event semaphore before giving it to indicate to the
	 * task that data is available. */
	configASSERT(cdc_new_data_semaphore);
	xSemaphoreGiveFromISR(cdc_new_data_semaphore,
			&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/*-----------------------------------------------------------*/

void cli_cdc_disable(uint8_t port)
{
	(void) port;

	cdc_connected = false;
}
