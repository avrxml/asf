/**
 * \file
 *
 * \brief PARC example.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include <string.h>
#include <asf.h>
#include <conf_board.h>
#include <conf_clock.h>
#include <conf_example.h>
#include <ioport.h>
#include <sysclk.h>

/** Size of the receive buffer used by the PDCA, in bytes. */
#define BUFFER_SIZE         64

/** Timer counter frequency in Hz. */
#define TC_FREQ             10

#define STRING_EOL    "\r"
#define STRING_HEADER "-- SAM PARC Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#define PDCA_PARC_CHANNEL     0
#define PDCA_PID_PARC         16

/** Receive buffer. */
static uint8_t gs_puc_buffer[BUFFER_SIZE];

/** Next Receive buffer. */
static uint8_t gs_puc_nextbuffer[BUFFER_SIZE];

/** PARC configure stuct */
struct parc_config parc_configex;

/** PDCA channel options. */
pdca_channel_config_t PDCA_PARC_OPTIONS = {
	.addr = (void *)gs_puc_buffer, /* memory address */
	.pid = PDCA_PID_PARC, /* select peripheral - PARC line.*/
	.size = BUFFER_SIZE, /* transfer counter */
	.r_addr = (void *)gs_puc_nextbuffer, /* next memory address */
	.r_size = BUFFER_SIZE, /* next transfer counter */
	.transfer_size = PDCA_MR_SIZE_BYTE /* select size of the transfer */
};

/**
 * \brief Interrupt handler for TC00. Record the number of bytes received,
 * and then restart a read transfer on the USART if the transfer was stopped.
 */
void TC00_Handler(void)
{
	static int32_t input_data = 0;
	static int32_t cnt = 0;

	/* Read TC0 Status. */
	tc_get_status(TC0, 0);

	/* Toggel I/O pin to simulate the PCCK */
	ioport_toggle_pin_level(PIN_PCCK_INPUT);

	/* PCDATA changes every two PCCK level change*/
	cnt++;
	if (cnt == 1) {
		/* Simulate PCD data */
		place_data_to_port(input_data++);
		if (input_data == BUFFER_SIZE ) {
			input_data = 0;
		}
	} else if (cnt == 2) {
		cnt =0;
	}
}

/**
 * \brief Configure Timer Counter 0 (TC0) to generate an interrupt every 200ms.
 * This interrupt will be used to flush USART input and echo back.
 */
static void configure_tc(void)
{
	uint32_t ul_div;
	uint32_t ul_tcclks;
	static uint32_t ul_pbaclk;

	/* Configure clock service. */
	sysclk_enable_peripheral_clock(TC0);

	/* Get system clock. */
	ul_pbaclk = sysclk_get_peripheral_bus_hz(TC0);

	/* Configure TC for a 1Hz frequency and trigger on RC compare. */
	tc_find_mck_divisor(TC_FREQ, ul_pbaclk, &ul_div, &ul_tcclks, ul_pbaclk);
	tc_init(TC0, 0, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC0, 0, (ul_pbaclk / ul_div) / TC_FREQ/10);

	/* Configure and enable interrupt on RC compare. */
	NVIC_EnableIRQ(TC00_IRQn);
	tc_enable_interrupt(TC0, 0, TC_IER_CPCS);
}

/**
 * \brief Configure UART for debug message output.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief The callback function for PDCA channel of PARC
 *
 * \param status  PDCA channel status
 */
static void pdca_parc_callback(enum pdca_channel_status status)
{
	UNUSED(status);
	printf("End of capture.\r\n");
	for (uint32_t uc_i = 0; uc_i < BUFFER_SIZE; uc_i++) {
		printf("0x%02X ", gs_puc_buffer[uc_i]);
	}
	printf("\r\n");

	/* disable interrupt for pdca channel of PARC*/
	pdca_channel_disable_interrupt(PDCA_PARC_CHANNEL,PDCA_IER_RCZ);
	
	pdca_channel_disable(PDCA_PARC_CHANNEL);
	puts("\n\rThe example is done!\n\r");
}

/**
 * \brief Application entry point for PARC example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint32_t uc_key;

	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Configure UART for debug message output. */
	configure_console();
	parc_port_source_simulation_config();

	//! [parc_variables]	
	struct parc_module module_inst;
	struct parc_config config;
	//! [parc_variables]	

	/* Output example information. */
	puts(STRING_HEADER);

	/* Configure TC. */
	configure_tc();
	/* Start timer. */
	tc_start(TC0, 0);

	//! [parc_get_defaults]	
	// Get default configuration
	parc_get_config_defaults(&config);
	//! [parc_get_defaults]	
	printf("Press y to sample the data when both data enable pins are enabled.\r\n");
	printf("Press n to sample the data, don't care the status of the data enable pins.\r\n");
	uc_key = 0;
	while ((uc_key != 'y') && (uc_key != 'n')) {
		usart_read(CONF_UART, &uc_key);
	}
	if (uc_key == 'y') {
		/* Sample the data when both data enable pins are enabled. */
		config.smode = PARC_SMODE_PCEN1_AND_PCEN2_H;
		ioport_set_pin_level(PIN_PCEN1_INPUT, IOPORT_PIN_LEVEL_HIGH);
		ioport_set_pin_level(PIN_PCEN2_INPUT, IOPORT_PIN_LEVEL_HIGH);
		printf("Receive data when both data enable pins are enabled.\r\n");
	} else {
		/* Sample the data, don't care the status of the data enable pins. */
		config.smode = PARC_SMODE_ALWAYS;
		printf("Receive data, don't care the status of the data enable pins.\r\n");
	}

	printf("Press y to sample all the data.\r\n");
	printf("Press n to sample the data only one out of two.\r\n");
	uc_key = 0;
	while ((uc_key != 'y') && (uc_key != 'n')) {
		usart_read(CONF_UART, &uc_key);
	}
	if (uc_key == 'y') {
			/* Sample all the data. */
		config.capture_mode = PARC_BOTH_CAPTURE;
		printf("All data are sampled.\r\n");
	} else {
		/* Sample the data only one out of two. */
		config.capture_mode = PARC_EVEN_CAPTURE;
		printf("Only one out of two data is sampled, with an even index.\r\n");
	}

	//! [parc_init_enable_and_start]
	//! [parc_init_enable_and_start_1]
	// Initialize PARC.
	parc_init(&module_inst, PARC, &config);
	//! [parc_init_enable_and_start_1]
	
	//! [parc_init_enable_and_start_2]
	// Enable the PARC
	parc_enable(&module_inst);
	
	// Start capture.
	parc_start_capture(&module_inst);
	//! [parc_init_enable_and_start_2]
	//! [parc_init_enable_and_start]

	/* Enable PDCA module clock */
	pdca_enable(PDCA);
	/* Init PDCA channel with the pdca_options.*/
	pdca_channel_set_config(PDCA_PARC_CHANNEL, &PDCA_PARC_OPTIONS);

	/* Set callback for PDCA interrupt. */
	pdca_channel_set_callback(PDCA_PARC_CHANNEL,
			pdca_parc_callback,PDCA_0_IRQn,1,PDCA_IER_RCZ);

	/* Enable PDCA channel, start receiving data. */
	pdca_channel_enable(PDCA_PARC_CHANNEL);
	/* Start read PARC data capture via PDCA. */
	pdca_channel_write_load(PDCA_PARC_CHANNEL,
			(void *)gs_puc_buffer, BUFFER_SIZE);
	/* Main loop. */
	while(1) {
	}
}
