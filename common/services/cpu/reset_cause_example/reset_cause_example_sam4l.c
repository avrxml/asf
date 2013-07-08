/**
 * \file
 *
 * \brief CPU reset cause example for SAM4L
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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
 * \mainpage CPU reset cause example
 *
 * \section Purpose
 *
 * This example demonstrates how to use the reset cause interface.
 *
 * \section Requirements
 *
 *  This example can be used with SAM4L evaluation kits.
 *
 * \section Description
 *
 * The program turns LED0 on when the power-on reset happens and outputs
 * different informaiton on the terminal, depending on the reset cause.
 *
 * Type different commands to generate different reset. To generate Core BOD 
 * reset or I/O BOD reset, press reset button after typing 'c' or 'i'.
 *
 * \section Usage
 *
 * - Build the program and download it to the evaluation board. Please
 *  refer to the SAM4L main page documentation
 *  http://www.atmel.com/tools/SAM4L-EK.aspx
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 *    \code
 *     -- Reset_Cause Example xxx --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 * -# Choose the item in the following menu to test.
 *    \code
 *     Menu :
 *     ------Please select reset cause way--------
 *       c: Generate core BOD reset
 *       i: Generate I/O BOD reset
 *       w: Generate watchdog reset
 *       s: Generate system reset
 *    \endcode
 */

#include "asf.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Reset_Cause Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

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
 * \brief Main function.
 */
int main(void)
{
	uint8_t key;

	/* Initialize the SAM system. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();

	/* Output example information. */
	puts(STRING_HEADER);
	puts("------ Please select which reset is to be generated ----------\n\r"
			"  c:  Generate core BOD reset\n\r"
			"  i:  Generate I/O BOD reset\n\r"
			"  w:  Generate watchdog reset\n\r"
			"  s:  Generate system reset \n\r");

	if (reset_cause_is_power_on_reset()) {
#ifdef LED0_GPIO
		ioport_set_pin_level(LED0_GPIO, IOPORT_PIN_LEVEL_HIGH);
#endif
	}
	if (reset_cause_is_cpu_brown_out_detected()) {
		puts("-- Reset Cause is Core browm out reset --\r\n");
	}
	if (reset_cause_is_io_brown_out_detected()) {
		puts("-- Reset Cause is I/O brown out reset --\r\n");
	}
	if (reset_cause_is_ocd()) {
		puts("-- Reset Cause is system reset request --\r\n");
	}
	if (reset_cause_is_external_reset()) {
		puts("-- Reset Cause is external reset --\r\n");
	}
	if (reset_cause_is_watchdog()) {
		puts("-- Reset Cause is watchdog reset --\r\n");
	}

	while(1) {
		scanf("%c", (char *)&key);
		switch (key) {
		case 'c':
			puts("-- Please press reset button --\r\n");
			BSCIF->BSCIF_BOD18CTRL = BSCIF_BOD18CTRL_EN | BSCIF_BOD18CTRL_ACTION(1) |
					BSCIF_BOD18CTRL_HYST | BSCIF_BOD18CTRL_FCD;
			BSCIF->BSCIF_BOD18LEVEL |= BSCIF_BOD18LEVEL_VAL(0x3F);
			break;
		case 'i':
			puts("-- Please press reset button --\r\n");
			BSCIF->BSCIF_BOD33CTRL = BSCIF_BOD33CTRL_EN | BSCIF_BOD33CTRL_ACTION(1) |
					BSCIF_BOD33CTRL_HYST | BSCIF_BOD33CTRL_FCD;
			BSCIF->BSCIF_BOD33LEVEL = BSCIF_BOD33LEVEL_VAL(0x3F);
			break;
		case 'w':
			WDT->WDT_CTRL |= WDT_CTRL_EN | WDT_CTRL_DAR | WDT_CTRL_KEY(0x55u);
			WDT->WDT_CTRL |= WDT_CTRL_EN | WDT_CTRL_DAR | WDT_CTRL_KEY(0xAAu);
			break;
		case 's':
			reset_do_soft_reset();
			break;
		}
	}
}
