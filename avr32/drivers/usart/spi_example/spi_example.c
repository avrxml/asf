/*****************************************************************************
 *
 * \file
 *
 * \brief SPI example application for AVR32 USART driver.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the USART in SPI modes software driver.\n It also comes
 * bundled with an example. This example is a basic Write to Slave Mode example.\n
 * <b>Example's operating mode: </b>
 * -# A message is displayed on the PC terminal ("Basic write in USART SPI Mode (press enter)")
 * -# You may then type any character other than CR(Carriage Return) and it will
 * be echoed back to the PC terminal.
 * -# If you type a CR, the SPI write mode send data through SPI
 *
 * \section files Main Files
 * - usart.c: USART driver and SPI support mode;
 * - usart.h: USART driver header file and SPI support mode;
 * - spi_example.c: SPI example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a USART module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100 evaluation kit or EVK1101 evaluation kit or STK600+RCUC3D routing card;
 * - CPU clock: 12 MHz;
 * - USART1 connected to a PC serial port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"

/*! \name USART Settings
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_USART                     (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN              AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION         AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN              AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION         AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI                 (&AVR32_USART2)
#  define EXAMPLE_USART_SPI_SCK_PIN         AVR32_USART2_CLK_0_PIN
#  define EXAMPLE_USART_SPI_SCK_FUNCTION    AVR32_USART2_CLK_0_FUNCTION
#  define EXAMPLE_USART_SPI_MISO_PIN        AVR32_USART2_RXD_0_0_PIN
#  define EXAMPLE_USART_SPI_MISO_FUNCTION   AVR32_USART2_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI_MOSI_PIN        AVR32_USART2_TXD_0_0_PIN
#  define EXAMPLE_USART_SPI_MOSI_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI_NSS_PIN         AVR32_USART2_RTS_0_0_PIN
#  define EXAMPLE_USART_SPI_NSS_FUNCTION    AVR32_USART2_RTS_0_0_FUNCTION
#elif BOARD == EVK1101
#  define EXAMPLE_USART                     (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN              AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION         AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN              AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION         AVR32_USART1_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI                 (&AVR32_USART2)
#  define EXAMPLE_USART_SPI_SCK_PIN         AVR32_USART2_CLK_0_PIN
#  define EXAMPLE_USART_SPI_SCK_FUNCTION    AVR32_USART2_CLK_0_FUNCTION
#  define EXAMPLE_USART_SPI_MISO_PIN        AVR32_USART2_RXD_0_0_PIN
#  define EXAMPLE_USART_SPI_MISO_FUNCTION   AVR32_USART2_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI_MOSI_PIN        AVR32_USART2_TXD_0_0_PIN
#  define EXAMPLE_USART_SPI_MOSI_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI_NSS_PIN         AVR32_USART2_RTS_0_PIN
#  define EXAMPLE_USART_SPI_NSS_FUNCTION    AVR32_USART2_RTS_0_FUNCTION
#elif BOARD == STK600_RCUC3D
#  define EXAMPLE_USART                     (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN              AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION         AVR32_USART1_RXD_0_0_FUNCTION
// For the RX pin, connect STK600.PORTD.PD0 to STK600.RS232 SPARE.RXD
#  define EXAMPLE_USART_TX_PIN              AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION         AVR32_USART1_TXD_0_0_FUNCTION
// For the TX pin, connect STK600.PORTC.PC7 to STK600.RS232 SPARE.TXD
#  define EXAMPLE_USART_SPI                 (&AVR32_USART2)
#  define EXAMPLE_USART_SPI_SCK_PIN         AVR32_USART2_CLK_0_PIN
#  define EXAMPLE_USART_SPI_SCK_FUNCTION    AVR32_USART2_CLK_0_FUNCTION
#  define EXAMPLE_USART_SPI_MISO_PIN        AVR32_USART2_RXD_0_0_PIN
#  define EXAMPLE_USART_SPI_MISO_FUNCTION   AVR32_USART2_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI_MOSI_PIN        AVR32_USART2_TXD_0_0_PIN
#  define EXAMPLE_USART_SPI_MOSI_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
#  define EXAMPLE_USART_SPI_NSS_PIN         AVR32_USART2_RTS_0_PIN
#  define EXAMPLE_USART_SPI_NSS_FUNCTION    AVR32_USART2_RTS_0_FUNCTION
#endif
//! @}


/*! \brief This is an example demonstrating the SPI mode of USART IP
 *         functionalities using the USART driver.
 */
int main(void)
{
#if BOARD == STK600_RCUC3D
	// Configure OSC0 in crystal mode, external crystal with a FOSC0 Hz frequency.
	scif_configure_osc_crystalmode(SCIF_OSC0, FOSC0);

	// Enable the OSC0
	scif_enable_osc(SCIF_OSC0, OSC0_STARTUP, true);

	// Set the main clock source as being OSC0.
	pm_set_mclk_source(PM_CLK_SRC_OSC0);
#else
	// Switch main clock to external oscillator 0 (crystal).
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
#endif

	static const gpio_map_t USART_GPIO_MAP =
	{
		{EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
		{EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	};

	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = 57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP,
		sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize USART in RS232 mode.
	usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FOSC0);

	static const gpio_map_t USART_SPI_GPIO_MAP =
	{
		{EXAMPLE_USART_SPI_SCK_PIN,  EXAMPLE_USART_SPI_SCK_FUNCTION },
		{EXAMPLE_USART_SPI_MISO_PIN, EXAMPLE_USART_SPI_MISO_FUNCTION},
		{EXAMPLE_USART_SPI_MOSI_PIN, EXAMPLE_USART_SPI_MOSI_FUNCTION},
		{EXAMPLE_USART_SPI_NSS_PIN,  EXAMPLE_USART_SPI_NSS_FUNCTION }
	};

	static const usart_spi_options_t USART_SPI_OPTIONS =
	{
		.baudrate     = 60000,
		.charlength   = 8,
		.spimode      = 0,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to SPI.
	gpio_enable_module(USART_SPI_GPIO_MAP,
		sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));

	// Initialize USART in SPI mode.
	usart_init_spi_master(EXAMPLE_USART_SPI, &USART_SPI_OPTIONS, FOSC0);

	// Show startup message
	usart_write_line(EXAMPLE_USART, "Basic write in USART SPI Mode (press enter)\r\n");

	// Press enter to continue.
	while (usart_get_echo_line(EXAMPLE_USART) == USART_FAILURE);  // Get and echo characters until end of line.

	usart_write_line(EXAMPLE_USART, "Writing SPI test pattern.\r\n");

	while (true)
	{
		usart_spi_selectChip(EXAMPLE_USART_SPI);
		usart_putchar(EXAMPLE_USART_SPI, 0x55);
		usart_putchar(EXAMPLE_USART_SPI, 0xAA);
		usart_spi_unselectChip(EXAMPLE_USART_SPI);
	}
}
