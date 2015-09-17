/**
 * \file
 *
 * \brief Frequency Meter Example for SAM4L.
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
 * \mainpage Frequency Meter Example
 *
 * \section Purpose
 *
 * The freqm_example demonstrates how to use frequency meter.
 *
 * \section Requirements
 *
 * This example can be used on SAM4L devices.
 *
 * \section Description
 *
 * The SAM4L controller select refclk and start frequency meter to measure the
 * selected clock.
 *
 * \section Usage
 *
 * - Build the program and download it to the evaluation board. Please
 *  refer to the SAM4L main page documentation
 *  http://www.atmel.com/products/microcontrollers/ARM/SAM4L.aspx
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
 *    \code
	-- Freqm Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 * -# The sent text should appear.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Freqm Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** GENCLK Configuration Value */
#define GCLK_ID         0
#define GCLK_SOURCE     GENCLK_SRC_CLK_CPU
#define GCLK_DIV        1

/** FREQM instance */
struct freqm_dev_inst g_freqm_inst;

/** FREQM configuration */
struct freqm_config  g_freqm_cfg;

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
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Application entry point for freqm example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint32_t clk, result;
	struct pll_config pcfg;
	
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	
	/* Initialize the UART console. */
	configure_console();

	/* Output example information. */
	puts(STRING_HEADER);

	/* Measure default clock. */
	freqm_get_config_defaults(&g_freqm_cfg);
	freqm_init(&g_freqm_inst, FREQM, &g_freqm_cfg);
	freqm_enable(&g_freqm_inst);
	freqm_start_measure(&g_freqm_inst);

	if (freqm_get_result_blocking(&g_freqm_inst, &result) == STATUS_OK) {
		clk = (result / FREQM_DURATION_DEFAULT) * OSC_RC32K_NOMINAL_HZ;
		printf(" Current CPU clock mesurement result is %d Hz. \n\r",
				(int)clk);
	} else {
		printf(" Current CPU clock mesurement Failed. \n\r");
	}

	/* Measure clock from external OSC0. */
	osc_enable(OSC_ID_OSC0);
	osc_wait_ready(OSC_ID_OSC0);
	g_freqm_cfg.msr_clk = FREQM_OSC0;
	freqm_init(&g_freqm_inst, FREQM, &g_freqm_cfg);
	freqm_start_measure(&g_freqm_inst);

	if (freqm_get_result_blocking(&g_freqm_inst, &result) == STATUS_OK) {
		clk = (result / FREQM_DURATION_DEFAULT) * OSC_RC32K_NOMINAL_HZ;
		printf(" Current OSC0 output clock mesurement result is %d Hz. \n\r",
				(int)clk);
	} else {
		printf(" Current OSC0 output clock mesurement Failed. \n\r");
	}

	/* Measure clock from PLL0. */
	pll_config_init(&pcfg, PLL_SRC_OSC0, 1, 96000000 /
			BOARD_OSC0_HZ);
	pll_enable(&pcfg, 0);
	pll_wait_for_lock(0);
	g_freqm_cfg.msr_clk = FREQM_PLL0;
	freqm_init(&g_freqm_inst, FREQM, &g_freqm_cfg);
	freqm_start_measure(&g_freqm_inst);

	if (freqm_get_result_blocking(&g_freqm_inst, &result) == STATUS_OK) {
		clk = (result / FREQM_DURATION_DEFAULT) * OSC_RC32K_NOMINAL_HZ;
		printf(" Current PLL0 output clock mesurement result is %d Hz. \n\r",
				(int)clk);
	} else {
		printf(" Current PLL0 output clock mesurement Failed. \n\r");
	}

	/* Change CLK_REF source to RCSYS and Measure clock from GENCLK0. */
	osc_disable(OSC_ID_OSC0);
	osc_enable(OSC_ID_RCSYS);
	osc_wait_ready(OSC_ID_RCSYS);
	genclk_enable_config(GCLK_ID, GCLK_SOURCE, GCLK_DIV);
	g_freqm_cfg.msr_clk = FREQM_GENCLK0;
	g_freqm_cfg.ref_clk = FREQM_REF_RCOSC;
	g_freqm_cfg.duration= FREQM_DURATION_DEFAULT;
	freqm_init(&g_freqm_inst, FREQM, &g_freqm_cfg);
	freqm_start_measure(&g_freqm_inst);

	if (freqm_get_result_blocking(&g_freqm_inst, &result) == STATUS_OK) {
		clk = (result / FREQM_DURATION_DEFAULT) * OSC_RCSYS_NOMINAL_HZ;
		printf(" Current GENCLK0 output clock mesurement result is %d Hz. \n\r",
				(int)clk);
	} else {
		printf(" Current GENCLK0 output clock mesurement Failed. \n\r");
	}
	
	while (1) {
	}
}
