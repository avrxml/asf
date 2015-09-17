/**
 * \file
 *
 * \brief Interprocessor Communication (IPC) example on core 1 for SAM.
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

/**
 * \mainpage Interprocessor Communication (IPC) example on core 1
 *
 * \section Purpose
 *
 * This example demostrates how core 1 communicates with core 0 via IPC.
 *
 * \section Requirements
 *
 * This package can be used with SAM dual-core device with IPC. The core 0
 * example must be loaded before.
 *
 * \section Descriptions
 *
 * The program initializes the console with given parameters and configures IPC1
 * IRQ0 handler to deal with the interrupt command from core 0. Then it triggers
 * IPC0 IRQ0 interrupt to start the interprocessor communication. When the
 * interrupt is triggerred, the console will output the information and one LED
 * will be turned on.
 * Everytime the interrupt is triggerred, the variable in the SHARED_MEMORY_ADDR
 * will be updated.
 *
 * \section Usage
 *
 * -# Compile the application.
 * -# Connect the USART port of the evaluation board to the computer and open
 * it in a terminal.
 *    - Settings: 115200 bauds, 8 bits, 1 stop bit, no parity, no flow control.
 * -# Download the core 0 example into the evaluation board and run it.
 * -# Upon startup, the application will output the following lines on console:
 *    \code
 *     -- Interprocessor Communication Example (Core 1) --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 * -# When the IPC1 IRQ0 is set by core 0:
 *    \code
 *     Got IRQ signal from core0
 *    \endcode
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include <conf_board.h>
#include <conf_example.h>
#include <led.h>
#include <conf_uart_serial.h>

#define STRING_EOL    "\r\n\0"
#define STRING_HEADER "-- Interprocessor Communication Example (Core 1) --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

static volatile bool is_core0_signal = false;
static volatile SHARED_MEMORY_TYPE *cnt = SHARED_MEMORY_ADDR;

/**
 *  \brief Handler for IPC interrupt request.
 */
static void ipc_irq_handler(Ipc *p, enum ipc_interrupt_source mask)
{
	is_core0_signal = true;
	(*cnt)++;
	ipc_clear_command(p, mask);
}

/**
 *  \brief Configure the IPC.
 */
static void configure_ipc(void)
{
	ipc_set_handler(IPC1, IPC_INTERRUPT_SRC_IRQ0, ipc_irq_handler);
	ipc_enable_interrupt(IPC1, IPC_INTERRUPT_SRC_IRQ0);
	NVIC_EnableIRQ(IPC1_IRQn);
}

/**
 *  \brief Configure the Console.
 */
static void configure_console(void)
{
	const sam_usart_opt_t usart_console_settings = {
		CONF_UART_BAUDRATE,
		CONF_UART_CHAR_LENGTH,
		CONF_UART_PARITY,
		CONF_UART_STOP_BITS,
		US_MR_CHMODE_NORMAL,
		/* This field is only used in IrDA mode. */
		0
	};

	/* Enable the peripheral clock in the PMC. */
	sysclk_enable_peripheral_clock(CONF_UART_ID);

	/* Configure USART in serial mode. */
	usart_init_rs232(CONF_UART, &usart_console_settings,
			sysclk_get_cpu_hz());

	/* Enable the transmitter. */
	usart_enable_tx(CONF_UART);
}

/**
 * \brief Application entry point for IPC example on core 1.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	*cnt = 0;
	configure_ipc();
	configure_console();

	/* Output example information. */
	usart_write_line(CONF_UART, STRING_HEADER);

	ipc_set_command(IPC0, IPC_INTERRUPT_SRC_IRQ0);

	while (1) {
		if (is_core0_signal == true) {
			is_core0_signal = false;
			LED_On(LED0);
			usart_write_line(CONF_UART, "Got IRQ signal from core0\r\n\0");
			delay_s(2);
			ipc_set_command(IPC0, IPC_INTERRUPT_SRC_IRQ0);
		}
	}
}

