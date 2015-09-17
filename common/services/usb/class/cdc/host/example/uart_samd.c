/**
 * \file
 *
 * \brief UART functions
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "conf_example.h"
#include "conf_usb_host.h"
#include <asf.h>

/* Structure for USART module connected to EDBG */
struct usart_module usart_module_edbg;

/* Structure for USART parameter configuration */
struct usart_config usart_conf;

/* Data for EDBG communication */
static uint8_t tx_data;
static uint8_t rx_data;

/**
 * \internal
 * \brief USART interrupt callback function
 *
 * Called by USART driver when transmit is complete.
 *
 * * \param module USART module causing the interrupt (not used)
 */
static void usart_tx_callback(struct usart_module *const module)
{
	/* Data ready to be sent */
	if (uhi_cdc_is_rx_ready(0)) {
		/* Transmit next data */
		ui_com_rx_start();
		tx_data = uhi_cdc_getc(0);
		usart_write_buffer_job(&usart_module_edbg, &tx_data, 1);
	} else {
		/* Fifo empty then Stop UART transmission */
		usart_disable_callback(&usart_module_edbg, USART_CALLBACK_BUFFER_TRANSMITTED);
		ui_com_rx_stop();
	}
}

/**
 * \internal
 * \brief USART interrupt callback function
 *
 * Called by USART driver when receiving is complete.
 *
 * * \param module USART module causing the interrupt (not used)
 */
static void usart_rx_callback(struct usart_module *const module)
{
	/* Data received */
	ui_com_tx_start();

	/* Transfer UART RX fifo to CDC TX */
	if (!uhi_cdc_is_tx_ready(0)) {
		/* Fifo full */
		ui_com_overflow();
	} else {
		uhi_cdc_putc(0, rx_data);
	}

	ui_com_tx_stop();

	usart_read_buffer_job(&usart_module_edbg, &rx_data, 1);

	return;
}

void uart_rx_notify(void)
{
	/* Transmit first data */
	ui_com_rx_start();
	usart_enable_callback(&usart_module_edbg, USART_CALLBACK_BUFFER_TRANSMITTED);
	tx_data = uhi_cdc_getc(0);
	usart_write_buffer_job(&usart_module_edbg, &tx_data, 1);
}

void uart_config(usb_cdc_line_coding_t *cfg)
{
	/* Configure USART for unit test output */
	usart_get_config_defaults(&usart_conf);

	switch (cfg->bCharFormat) {
	case CDC_STOP_BITS_2:
		usart_conf.stopbits = USART_STOPBITS_2;
		break;

	case CDC_STOP_BITS_1_5:
		usart_conf.stopbits = USART_STOPBITS_1;
		break;

	case CDC_STOP_BITS_1:
	default:
		/* Default stop bit = 1 stop bit */
		usart_conf.stopbits = USART_STOPBITS_1;
		break;
	}

	switch (cfg->bParityType) {
	case CDC_PAR_EVEN:
		usart_conf.parity = USART_PARITY_EVEN;
		break;

	case CDC_PAR_ODD:
		usart_conf.parity = USART_PARITY_ODD;
		break;

	case CDC_PAR_MARK:
		usart_conf.parity = USART_PARITY_NONE;
		break;

	case CDC_PAR_SPACE:
		usart_conf.parity = USART_PARITY_NONE;
		break;

	default:
	case CDC_PAR_NONE:
		usart_conf.parity = USART_PARITY_NONE;
		break;
	}

	switch(cfg->bDataBits) {
	case 5:
		usart_conf.character_size = USART_CHARACTER_SIZE_5BIT;
		break;
	case 6:
		usart_conf.character_size = USART_CHARACTER_SIZE_6BIT;
		break;
	case 7:
		usart_conf.character_size = USART_CHARACTER_SIZE_7BIT;
		break;
	default:
	case 8:
		usart_conf.character_size = USART_CHARACTER_SIZE_8BIT;
		break;
	}

	/* Options for USART. */
	usart_conf.baudrate = LE32_TO_CPU(cfg->dwDTERate);
	usart_conf.mux_setting = CONF_USART_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_USART_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_USART_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_USART_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_USART_PINMUX_PAD3;
	usart_disable(&usart_module_edbg);
	usart_init(&usart_module_edbg, CONF_USART_BASE, &usart_conf);
	usart_enable(&usart_module_edbg);

	/* Enable interrupts */
	usart_register_callback(&usart_module_edbg, usart_tx_callback,
			USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_module_edbg, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_module_edbg, usart_rx_callback,
			USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_module_edbg, USART_CALLBACK_BUFFER_RECEIVED);
	usart_read_buffer_job(&usart_module_edbg, &rx_data, 1);
}

void uart_open(void)
{
	usart_enable(&usart_module_edbg);
}

void uart_close(void)
{
	/* Close RS232 communication */
	usart_disable(&usart_module_edbg);
}
