/**
 * \file
 *
 * \brief lwIP Raw HTTP basic example.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
 *  \mainpage lwIP Raw HTTP basic example
 *
 *  \section Purpose
 *  This documents data structures, functions, variables, defines, enums, and
 *  typedefs in the software for the lwIP Raw HTTP basic example.
 *
 *  The given example is a lwIP example using the current lwIP stack and MAC driver.
 *
 *  \section Requirements
 *
 *  This package can be used on SAMD20 Xplained Pro with the Ethernet1
 *  Xplained Pro extention board on EXT1.
 *
 *  \section Description
 *
 *  This example features a simple lwIP web server.
 *  - Plug the Ethernet cable directly into the evaluation kit to connect to the PC.
 *  - Configuring the PC network port to local mode to setup a 'point to point' network.
 *  - Start the example.
 *  - Launch your favorite web browser.
 *  - Type the WEB server example IP address in your browser's address bar.
 *
 *  \section Usage
 *
 *  -# Build the program and download it into the evaluation board. Please
 *     refer to the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/6421B.pdf">
 *     SAM-BA User Guide</a>, the
 *     <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *     GNU-Based Software Development</a>
 *     application note or the
 *     <a href="http://www.iar.com/website1/1.0.1.0/78/1/">
 *     IAR EWARM User and reference guides</a>,
 *     depending on the solutions that users choose.
 *  -# On the computer, open and configure a terminal application
 *     (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *    - 115200 bauds
 *    - 8 bits of data
 *    - No parity
 *    - 1 stop bit
 *    - No flow control
 *  -# In the terminal window, the
 *     following text should appear (if DHCP mode is not enabled):
 *     \code
 *      Network up IP==xxx.xxx.xxx.xxx
 *      Static IP Address Assigned
 *     \endcode
 *
 */

#include "board.h"
#include "system.h"
#include "usart.h"
#include "stdio_serial.h"
#include "delay.h"
#include "ethernet.h"
#include "httpd.h"

#define STRING_EOL    "\r"
#define STRING_HEADER "-- Raw HTTP Basic Example -- \r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Software instance of the USART upon which to transmit the results. */
static struct usart_module usart_instance;

/** Set up the USART (EDBG) communication for debug purpose. */
static void setup_usart_channel(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);

	/* Configure the USART settings and initialize the standard I/O library */
	config_usart.baudrate = 115200;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

	stdio_serial_init(&usart_instance, EDBG_CDC_MODULE, &config_usart);
	usart_enable(&usart_instance);
}

/**
 * \brief Main program function. Configure the hardware, initialize lwIP
 * TCP/IP stack, and start HTTP service.
 */
int main(void)
{
	/* Initialize the SAM system. */
	system_init();
	delay_init();

	/* Setup USART module to output results */
	setup_usart_channel();

	/* Print example information. */
	puts(STRING_HEADER);

	/* Bring up the ethernet interface & initialize timer0, channel0. */
	init_ethernet();

	/* Bring up the web server. */
	httpd_init();

	/* Program main loop. */
	while (1) {
		/* Check for input packet and process it. */
		ethernet_task();
	}
}
