/**
 *
 * \file
 *
 * \brief Serial Bridge.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#include "usart_stream.h"
#include <string.h>

static uint8_t usart_buffer[USART_BUFFER_MAX];
static uint32_t usart_recv_size = 0;

void configure_usart(uint32_t baudrate)
{
	usart_serial_options_t uart_serial_options = {
		.baudrate = baudrate,
		.paritytype = CONF_UART_PARITY,
		.charlength = US_MR_CHRL_8_BIT,
		.stopbits = 0,
	};

	/* Configure the UART console. */
	usart_serial_init((usart_if)CONF_UART, &uart_serial_options);

	usart_stream_reset();
}

void usart_stream_reset(void)
{
#ifdef SAMG55
	usart_reset_rx((Usart *)CONF_UART);
	usart_enable_rx((Usart *)CONF_UART);
#else
	uart_reset((Uart *)CONF_UART);
	uart_enable((Uart *)CONF_UART);
#endif
	usart_recv_size = 0;
}

void usart_stream_write(uint8_t data)
{
	usart_serial_putchar((usart_if)CONF_UART, data);
#ifdef SAMG55
	while (!usart_is_tx_empty((Usart *)CONF_UART)) {
#else
	while (!uart_is_tx_buf_empty((Uart *)CONF_UART)) {
#endif
	}
}

void usart_stream_write_buffer(uint8_t *data, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++) {
		usart_serial_putchar((usart_if)CONF_UART, data[i]);
	}
}

int usart_stream_read(uint8_t **data, uint32_t *size)
{
#ifdef SAMG55
	uint32_t val;
	
	while (usart_recv_size < sizeof(usart_buffer) && usart_read((Usart *)CONF_UART, &val) == 0) {
#else
	uint8_t val;

	while (usart_recv_size < sizeof(usart_buffer) && uart_read((Uart *)CONF_UART, &val) == 0) {
#endif
		usart_buffer[usart_recv_size++] = val;
	}

	*data = usart_buffer;
	*size = usart_recv_size;

	return 0;
}

void usart_stream_move(uint32_t offset)
{
	usart_recv_size -= offset;
	if (usart_recv_size > 0) {
		memmove(usart_buffer, usart_buffer + offset, usart_recv_size);
	} else {
		usart_recv_size = 0;
	}
}
