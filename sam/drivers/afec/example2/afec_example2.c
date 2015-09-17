/**
 * \file
 *
 * \brief AFEC enhanced resolution mode example for SAM.
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

/**
 * \mainpage AFEC Enhanced Resolution Example
 *
 * \section Purpose
 *
 * The example demonstrates how to use the enhanced resolution mode
 * inside the microcontroller to sample analog voltage.
 *
 * \section Requirements
 *
 * This example can be used on SAM4E-EK boards.
 *
 * \section Description
 *
 * The example is aimed to demonstrate the enhanced resolution mode
 * inside the microcontroller. To use this feature, the channel 5 which is connected to
 * the potentiometer should be enabled. Users can select different resolution modes
 * by configuration menu in the terminal.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the
 *    following text should appear (values depend on the board and the chip used):
 *    \code
	-- AFEC Enhanced Resolution Examplexxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	=========================================================
	Menu: press a key to change the resolution mode.
	---------------------------------------------------------
	-- n: Normal Resolution Mode--
	-- e: Enhanced Resolution Mode--
	-- q: Quit Configuration--
\endcode
 * -# The application will output current voltage of potentiometer on the terminal.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "asf.h"

/** Reference voltage for AFEC in mv. */
#define VOLT_REF        (3300)

/** The maximal digital value */
#define MAX_DIGITAL_12_BIT     (4095UL)

#define STRING_EOL    "\r"
#define STRING_HEADER "-- AFEC Enhanced Resolution Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

#define MENU_HEADER "\n\r-- press a key to change the resolution mode--\n\r" \
		"-- n: Normal Resolution Mode--\n\r" \
		"-- e: Enhanced Resolution Mode--\n\r" \
		"-- q: Quit Configuration--\n\r"

/** AFEC sample data */
struct {
	uint32_t value;
	bool is_done;
} g_afec_sample_data;

/** The maximal digital value */
static uint32_t g_max_digital;

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Display main menu.
 */
static void display_menu(void)
{
	puts(MENU_HEADER);
}

/**
 * \brief Set AFEC resolution mode.
 */
static void set_afec_resolution(void)
{
	uint8_t uc_key;
	uint8_t uc_done = 0;

	display_menu();

	while (!uc_done) {
		while (uart_read(CONF_UART, &uc_key));

		switch (uc_key) {
		case 'n':
			g_max_digital = MAX_DIGITAL_12_BIT;
			afec_set_resolution(AFEC0, AFEC_12_BITS);
			puts(" Set Resolution to Normal \n\r");
			break;
		case 'e':
			g_max_digital = MAX_DIGITAL_12_BIT * 16;
			afec_set_resolution(AFEC0, AFEC_16_BITS);
			puts(" Set Resolution to Enhanced \n\r");
			break;
		case 'q':
			uc_done = 1;
			puts(" Quit Configuration \n\r");
			break;
		default:
			break;
		}
	}
}

/**
 * \brief AFEC interrupt callback function.
 */
static void afec_data_ready(void)
{
	g_afec_sample_data.value = afec_get_latest_value(AFEC0);
	g_afec_sample_data.is_done = true;
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t uc_key;

	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	configure_console();

	/* Output example information. */
	puts(STRING_HEADER);

	g_afec_sample_data.value = 0;
	g_afec_sample_data.is_done = false;
	g_max_digital = MAX_DIGITAL_12_BIT;

	afec_enable(AFEC0);

	struct afec_config afec_cfg;

	afec_get_config_defaults(&afec_cfg);
	afec_init(AFEC0, &afec_cfg);

	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_POTENTIOMETER, &afec_ch_cfg);

	/*
	 * Because the internal ADC offset is 0x800, it should cancel it and shift
	 * down to 0.
	 */
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_POTENTIOMETER, 0x800);

	afec_set_trigger(AFEC0, AFEC_TRIG_SW);

	/* Enable channel for potentiometer. */
	afec_channel_enable(AFEC0, AFEC_CHANNEL_POTENTIOMETER);

	afec_set_callback(AFEC0, AFEC_INTERRUPT_DATA_READY, afec_data_ready, 1);

	display_menu();

	afec_start_calibration(AFEC0);
	while((afec_get_interrupt_status(AFEC0) & AFEC_ISR_EOCAL) != AFEC_ISR_EOCAL);

	while (1) {
		afec_start_software_conversion(AFEC0);
		delay_ms(1000);

		/* Check if AFEC sample is done. */
		if (g_afec_sample_data.is_done == true) {
			printf("Potentiometer Voltage: %4d mv.",
					(int)(g_afec_sample_data.value * VOLT_REF
					/ g_max_digital));
			puts("\r");
			g_afec_sample_data.is_done = false;
		}

		/* Check if the user enters a key. */
		if (!uart_read(CONF_UART, &uc_key)) {
			/* Disable all afec interrupt. */
			afec_disable_interrupt(AFEC0, AFEC_INTERRUPT_ALL);
			set_afec_resolution();
			afec_enable_interrupt(AFEC0, AFEC_INTERRUPT_DATA_READY);
		}
	}
}
