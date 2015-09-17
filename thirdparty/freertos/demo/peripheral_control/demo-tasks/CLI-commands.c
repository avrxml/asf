/**
 *
 * \file
 *
 * \brief FreeRTOS+CLI command examples
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "demo-tasks.h"

/*
 * Implements the run-time-stats command.
 */
static portBASE_TYPE task_stats_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString);

/*
 * Implements the task-stats command.
 */
static portBASE_TYPE run_time_stats_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString);

/*
 * Implements the echo-three-parameters command.
 */
static portBASE_TYPE three_parameter_echo_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString);

/*
 * Implements the echo-parameters command.
 */
static portBASE_TYPE multi_parameter_echo_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString);

/*
 * Implements the create-task command.
 */
static portBASE_TYPE create_task_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString);

/*
 * Implements the delete-task command.
 */
static portBASE_TYPE delete_task_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString);

/*
 * The task that is created by the create-task command.
 */
void created_task(void *pvParameters);

/*
 * Holds the handle of the task created by the create-task command.
 */
static xTaskHandle created_task_handle = NULL;

/* Structure that defines the "run-time-stats" command line command.
This generates a table that shows how much run time each task has */
static const CLI_Command_Definition_t run_time_stats_command_definition =
{
	(const int8_t *const) "run-time-stats", /* The command string to type. */
	(const int8_t *const) "run-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n\r\n",
	run_time_stats_command, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "task-stats" command line command.  This generates
a table that gives information on each task in the system. */
static const CLI_Command_Definition_t task_stats_command_definition =
{
	(const int8_t *const) "task-stats", /* The command string to type. */
	(const int8_t *const) "task-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n\r\n",
	task_stats_command, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "echo_3_parameters" command line command.  This
takes exactly three parameters that the command simply echos back one at a
time. */
static const CLI_Command_Definition_t three_parameter_echo_command_definition =
{
	(const int8_t *const) "echo-3-parameters",
	(const int8_t *const) "echo-3-parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n\r\n",
	three_parameter_echo_command, /* The function to run. */
	3 /* Three parameters are expected, which can take any value. */
};

/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
static const CLI_Command_Definition_t multi_parameter_echo_command_definition =
{
	(const int8_t *const) "echo-parameters",
	(const int8_t *const) "echo-parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n\r\n",
	multi_parameter_echo_command, /* The function to run. */
	-1 /* The user can enter any number of commands. */
};

/* Structure that defines the "create-task" command line command.  This takes a
single parameter that is passed into a newly created task.  The task then
periodically writes to the console.  The parameter must be a numerical value. */
static const CLI_Command_Definition_t create_task_command_definition =
{
	(const int8_t *const) "create-task",
	(const int8_t *const) "create-task <param>:\r\n Creates a new task that periodically writes the parameter to the CLI output\r\n\r\n",
	create_task_command, /* The function to run. */
	1 /* A single parameter should be entered. */
};

/* Structure that defines the "delete-task" command line command.  This deletes
the task that was previously created using the "create-command" command. */
static const CLI_Command_Definition_t delete_task_command_definition =
{
	(const int8_t *const) "delete-task",
	(const int8_t *const) "delete-task:\r\n Deletes the task created by the create-task command\r\n\r\n",
	delete_task_command, /* The function to run. */
	0 /* A single parameter should be entered. */
};

/*-----------------------------------------------------------*/

void vRegisterCLICommands(void)
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand(&task_stats_command_definition);
	FreeRTOS_CLIRegisterCommand(&run_time_stats_command_definition);
	FreeRTOS_CLIRegisterCommand(&three_parameter_echo_command_definition);
	FreeRTOS_CLIRegisterCommand(&multi_parameter_echo_command_definition);
	FreeRTOS_CLIRegisterCommand(&create_task_command_definition);
	FreeRTOS_CLIRegisterCommand(&delete_task_command_definition);
}

/*-----------------------------------------------------------*/

static portBASE_TYPE task_stats_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	const int8_t *const task_table_header = (int8_t *) "Task          State  Priority  Stack	#\r\n************************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *) task_table_header);
	vTaskList(pcWriteBuffer + strlen((char *) task_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE run_time_stats_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	const int8_t *const stats_table_header = (int8_t *) "Task            Abs Time      % Time\r\n****************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *) stats_table_header);
	vTaskGetRunTimeStats(pcWriteBuffer + strlen(
			(char *) stats_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE three_parameter_echo_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	int8_t *parameter_string;
	portBASE_TYPE parameter_string_length, return_value;
	static portBASE_TYPE parameter_number = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	if (parameter_number == 0) {
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf((char *) pcWriteBuffer,
				"The three parameters were:\r\n");

		/* Next time the function is called the first parameter will be echoed
		back. */
		parameter_number = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		return_value = pdPASS;
	} else {
		/* Obtain the parameter string. */
		parameter_string = (int8_t *) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										parameter_number,		/* Return the next parameter. */
										&parameter_string_length	/* Store the parameter string length. */
									);

		/* Sanity check something was returned. */
		configASSERT(parameter_string);

		/* Return the parameter string. */
		memset(pcWriteBuffer, 0x00, xWriteBufferLen);
		sprintf((char *) pcWriteBuffer, "%ld: ", parameter_number);
		strncat((char *) pcWriteBuffer, (const char *) parameter_string,
				parameter_string_length);
		strncat((char *) pcWriteBuffer, "\r\n", strlen("\r\n"));

		/* If this is the last of the three parameters then there are no more
		strings to return after this one. */
		if (parameter_number == 3L) {
			/* If this is the last of the three parameters then there are no more
			strings to return after this one. */
			return_value = pdFALSE;
			parameter_number = 0L;
		} else {
			/* There are more parameters to return after this one. */
			return_value = pdTRUE;
			parameter_number++;
		}
	}

	return return_value;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE multi_parameter_echo_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	int8_t *parameter_string;
	portBASE_TYPE parameter_string_length, return_value;
	static portBASE_TYPE parameter_number = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	if (parameter_number == 0) {
		/* The first time the function is called after the command has been
		entered just a header string is returned. */
		sprintf((char *) pcWriteBuffer, "The parameters were:\r\n");

		/* Next time the function is called the first parameter will be echoed
		back. */
		parameter_number = 1L;

		/* There is more data to be returned as no parameters have been echoed
		back yet. */
		return_value = pdPASS;
	} else {
		/* Obtain the parameter string. */
		parameter_string = (int8_t *) FreeRTOS_CLIGetParameter
									(
										pcCommandString,		/* The command string itself. */
										parameter_number,		/* Return the next parameter. */
										&parameter_string_length	/* Store the parameter string length. */
									);

		if (parameter_string != NULL) {
			/* Return the parameter string. */
			memset(pcWriteBuffer, 0x00, xWriteBufferLen);
			sprintf((char *) pcWriteBuffer, "%ld: ", parameter_number);
			strncat((char *) pcWriteBuffer, (const char *) parameter_string, parameter_string_length);
			strncat((char *) pcWriteBuffer, "\r\n", strlen("\r\n"));

			/* There might be more parameters to return after this one. */
			return_value = pdTRUE;
			parameter_number++;
		} else {
			/* No more parameters were found.  Make sure the write buffer does
			not contain a valid string. */
			pcWriteBuffer[0] = 0x00;

			/* No more data to return. */
			return_value = pdFALSE;

			/* Start over the next time this command is executed. */
			parameter_number = 0;
		}
	}

	return return_value;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE create_task_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	int8_t *parameter_string;
	portBASE_TYPE parameter_string_length;
	static const int8_t *success_message = (int8_t *) "Task created\r\n";
	static const int8_t *failure_message = (int8_t *) "Task not created\r\n";
	static const int8_t *task_already_created_message = (int8_t *) "The task has already been created. Execute the delete-task command first.\r\n";
	int32_t parameter_value;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the parameter string. */
	parameter_string = (int8_t *) FreeRTOS_CLIGetParameter(
									pcCommandString,		/* The command string itself. */
									1,						/* Return the first parameter. */
									&parameter_string_length	/* Store the parameter string length. */
								);

	/* Turn the parameter into a number. */
	parameter_value = (int32_t) atol((const char *) parameter_string);

	/* Attempt to create the task. */
	if (created_task_handle != NULL) {
		strcpy((char *) pcWriteBuffer,
				(const char *) task_already_created_message);
	} else {
		if (xTaskCreate(created_task, (const signed char *) "Created",
				configMINIMAL_STACK_SIZE,
				(void *) parameter_value, tskIDLE_PRIORITY,
				&created_task_handle) == pdPASS) {
			strcpy((char *) pcWriteBuffer,
					(const char *) success_message);
		} else {
			strcpy((char *) pcWriteBuffer,
					(const char *) failure_message);
		}
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE delete_task_command(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen,
		const int8_t *pcCommandString)
{
	static const int8_t *success_message = (int8_t *) "Task deleted\r\n";
	static const int8_t *failure_message = (int8_t *) "The task was not running.  Execute the create-task command first.\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	(void) pcCommandString;
	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* See if the task is running. */
	if (created_task_handle != NULL) {
		vTaskDelete(created_task_handle);
		created_task_handle = NULL;
		strcpy((char *) pcWriteBuffer, (const char *) success_message);
	} else {
		strcpy((char *) pcWriteBuffer, (const char *) failure_message);
	}

	/* There is no more data to return after this single string, so return
	 * pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

void created_task(void *pvParameters)
{
	int32_t parameter_value;
	static uint8_t local_buffer[60];

	/* Cast the parameter to an appropriate type. */
	parameter_value = (int32_t)pvParameters;

	memset((void *) local_buffer, 0x00, sizeof(local_buffer));
	sprintf((char *) local_buffer,
			"Created task running.  Received parameter %ld\r\n\r\n",
			(long) parameter_value);

	/* Cannot yet tell which CLI interface is in use, but both output functions
	guard check the port is initialised before it is used. */
#if (defined confINCLUDE_USART_CLI)
	usart_cli_output(local_buffer);
#endif

#if (defined confINCLUDE_CDC_CLI)
	cdc_cli_output(local_buffer);
#endif

	for (;;) {
		vTaskDelay(portMAX_DELAY);
	}
}

/*-----------------------------------------------------------*/
