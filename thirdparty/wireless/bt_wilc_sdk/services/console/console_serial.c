/**
 * \file console_serial.c
 *
 * \brief Serial Console functionalities
 *
 * Copyright (c) 2018 Atmel Corporation. All rights reserved.
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
 */

/* === INCLUDES ============================================================ */

#include "asf.h"
#include "console_serial.h"
#include "conf_uart_serial.h"
#include "stdio_serial.h"
#include "btstack_stdin.h"
#include "btstack_config.h"

/* === TYPES =============================================================== */
static void (*stdin_handler)(char c);
static btstack_data_source_t stdin_data_source;
/* === MACROS ============================================================== */

#ifndef CONF_UART
#define CONF_UART            CONSOLE_UART
#endif

#ifndef CONF_UART_BAUDRATE
#define CONF_UART_BAUDRATE (115200UL)
#endif

#ifndef CONF_UART_CHAR_LENGTH
#define CONF_UART_CHAR_LENGTH	US_MR_CHRL_8_BIT
#endif

#ifndef CONF_UART_PARITY
#define CONF_UART_PARITY		US_MR_PAR_NO
#endif

#ifndef CONF_UART_STOP_BITS
#define CONF_UART_STOP_BITS		US_MR_NBSTOP_1_BIT
#endif

/**
 *  Configure console.
 */
void serial_console_init(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);

#ifdef CONF_MODE_WIFI_AND_BLE
	osprintf_init(CONF_UART, &uart_serial_options);
#else
	stdio_serial_init(CONF_UART, &uart_serial_options);
#endif
}

static void btstack_stdin_process(struct btstack_data_source *ds, btstack_data_source_callback_type_t callback_type)
{

	// try to read from console
	uint32_t stdin_character;
	uint32_t res = usart_read(CONF_UART, &stdin_character);
	if (res) return;

	if (stdin_handler){
		(*stdin_handler)(stdin_character & 0xff);
	}
}

void btstack_stdin_setup(void (*handler)(char c))
{
	// set handler
	stdin_handler = handler;

	// set up polling data_source
	btstack_run_loop_set_data_source_handler(&stdin_data_source, &btstack_stdin_process);
	btstack_run_loop_enable_data_source_callbacks(&stdin_data_source, DATA_SOURCE_CALLBACK_POLL);
	btstack_run_loop_add_data_source(&stdin_data_source);
}

/* EOF */
