/**
 * \file
 *
 * \brief AFEC automatic comparison example for SAM.
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
 * \mainpage AFEC Automatic Comparison Example
 *
 * \section Purpose
 *
 * The example demonstrates how to use the automatic comparison feature
 * inside the microcontroller to compare analog voltage.
 *
 * \section Requirements
 *
 * This example can be used on SAM4E-EK boards,SAMV71-Xplained-Ultra,
 * SAME70-Xplained-Pro.
 *
 * \section Description
 *
 * The example is aimed to demonstrate the automatic comparison feature
 * inside the microcontroller. To use this feature, the channel 5 which is
 * connected to the potentiometer should be enabled. Users can change the
 * potentiometer voltage to compare with threshold value.The terminal will
 * output current potentiometer voltage and configured window value.
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
	-- AFEC Automatic Comparison Examplexxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 * -# The application will output comparison result on the terminal.
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
#define MAX_DIGITAL     (4095UL)

#define STRING_EOL    "\r"
#define STRING_HEADER "-- AFEC Automatic Comparison Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Low threshold */
static uint16_t gs_us_low_threshold = 0;
/** High threshold */
static uint16_t gs_us_high_threshold = 0;

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
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
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Callback function for AFEC enter compasion window interrupt.
 */
static void afec_print_comp_result(void)
{
	uint16_t us_adc;

	/* Disable Compare Interrupt. */
	afec_disable_interrupt(AFEC0, AFEC_INTERRUPT_COMP_ERROR);

	us_adc = afec_channel_get_value(AFEC0, AFEC_CHANNEL_POTENTIOMETER);

	printf("-ISR-:Potentiometer voltage %d mv is in the comparison "
			"window:%d -%d mv!\n\r",
			(int)(us_adc * VOLT_REF / MAX_DIGITAL),
			(int)(gs_us_low_threshold * VOLT_REF / MAX_DIGITAL),
			(int)(gs_us_high_threshold * VOLT_REF / MAX_DIGITAL));
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	configure_console();

	/* Output example information. */
	puts(STRING_HEADER);

	gs_us_low_threshold = 0x0;
	gs_us_high_threshold = 0x7FF;

	afec_enable(AFEC0);

	struct afec_config afec_cfg;

	afec_get_config_defaults(&afec_cfg);
	afec_init(AFEC0, &afec_cfg);

	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);

#if (SAMV71 || SAMV70 || SAME70 || SAMS70)
	/*
	 * Because the internal AFEC offset is 0x200, it should cancel it and shift
	 * down to 0.
	 */
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_POTENTIOMETER, 0x200);

	afec_ch_cfg.gain = AFEC_GAINVALUE_0;

#else
	/*
	 * Because the internal AFEC offset is 0x800, it should cancel it and shift
	 * down to 0.
	 */
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_POTENTIOMETER, 0x800);
#endif

	afec_ch_set_config(AFEC0, AFEC_CHANNEL_POTENTIOMETER, &afec_ch_cfg);
	afec_set_trigger(AFEC0, AFEC_TRIG_SW);

	afec_set_comparison_mode(AFEC0, AFEC_CMP_MODE_2, AFEC_CHANNEL_POTENTIOMETER, 0);
	afec_set_comparison_window(AFEC0, gs_us_low_threshold, gs_us_high_threshold);

	/* Enable channel for potentiometer. */
	afec_channel_enable(AFEC0, AFEC_CHANNEL_POTENTIOMETER);

	afec_set_callback(AFEC0, AFEC_INTERRUPT_COMP_ERROR, afec_print_comp_result, 1);

	while (1) {
		afec_start_software_conversion(AFEC0);
		delay_ms(1000);
	}
}
