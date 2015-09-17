/*****************************************************************************
*
* \file
*
* \brief ACIFA example driver for AVR UC3.
*
* This file provides an example for the ACIFA on AVR UC3 devices.
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
*****************************************************************************/

/** \mainpage
 * \section intro Introduction
 * Example use of the ASF ACIFA driver for UC3 devices, giving a demonstration
 * of how to set up the module and perform comparisons.
 *
 * \section files Main Files
 * - acifa_example.c : ACIFA code example
 * - conf_example.h : Example configuration for the chosen board hardware
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR UC3 devices. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR UC3 devices with an ACIFA module can be used. This example has been
 * tested with the following setup:<BR>
 * - UC3C_EK evaluation kit
 *  - Comparator 1: Input + on pin PA06, Input - on pin PA07
 *  - Comparator 2: Input + on pin PA20, Input - on pin PA22
 *  - Comparator 3: Input + on pin PA23, Input - on pin PA21
 *
 * \section setupinfo Setup Information
 * <BR>CPU speed: <i> 16 MHz </i>
 * - [on UC3C_EK only] Connect a PC USB cable to the USB VCP plug (the USB plug
 *  on the right) of the UC3C_EK. The PC is used as a power source. The UC3C0512
 *  USART2 is connected to the UC3B USART1. The UC3B holds a firmware that acts
 *  as a USART to USB gateway. On the USB side, the UC3B firmware implements a 
 *  USB CDC class: when connected to a PC, it will enumerate as a Virtual Com 
 *  Port. Once the UC3B USB is correctly installed on Windows, to communicate on
 *  this port, open a HyperTerminal configured with the following settings:
 *  57600 bps, 8 data bits, no parity bit, 1 stop bit, no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_example.h"

/* GPIO pin/adc-function map. */
const gpio_map_t ACIFA_GPIO_MAP = {
	{AVR32_ADCREF0_PIN    , AVR32_ADCREF0_FUNCTION    },
	{AVR32_ADCREFP_PIN    , AVR32_ADCREFP_FUNCTION    },
	{AVR32_ADCREFN_PIN    , AVR32_ADCREFN_FUNCTION    },
	{EXAMPLE_AC_ACMP0_PIN , EXAMPLE_AC_ACMP0_FUNCTION },
	{EXAMPLE_AC_ACMPN0_PIN, EXAMPLE_AC_ACMPN0_FUNCTION},
	{EXAMPLE_AC_ACMP1_PIN , EXAMPLE_AC_ACMP1_FUNCTION },
	{EXAMPLE_AC_ACMPN1_PIN, EXAMPLE_AC_ACMPN1_FUNCTION},
	{EXAMPLE_AC_ACMP2_PIN , EXAMPLE_AC_ACMP2_FUNCTION },
	{EXAMPLE_AC_ACMPN2_PIN, EXAMPLE_AC_ACMPN2_FUNCTION}
};

/** \brief main function : do init and loop to display ACIFA values */
int main( void )
{
	/* Init system clocks */
	sysclk_init();

	/* init debug serial line */
	init_dbg_rs232(sysclk_get_cpu_hz());

	/* Assign and enable GPIO pins to the AC function. */
	gpio_enable_module(ACIFA_GPIO_MAP, sizeof(ACIFA_GPIO_MAP) /
			sizeof(ACIFA_GPIO_MAP[0]));

	/* Configure ACIFA0 */
	acifa_configure(&AVR32_ACIFA0,
			ACIFA_COMP_SELA,
			EXAMPLE_AC_ACMP1_INPUT,
			EXAMPLE_AC_ACMPN1_INPUT,
			FOSC0);
	acifa_configure(&AVR32_ACIFA0,
			ACIFA_COMP_SELB,
			EXAMPLE_AC_ACMP2_INPUT,
			EXAMPLE_AC_ACMPN2_INPUT,
			FOSC0);

	/* Start the ACIFA0. */
	acifa_start(&AVR32_ACIFA0,
			(ACIFA_COMP_SELA | ACIFA_COMP_SELB));

	/* Configure ACIFA1 */
	acifa_configure(&AVR32_ACIFA1,
			ACIFA_COMP_SELA,
			EXAMPLE_AC_ACMP0_INPUT,
			EXAMPLE_AC_ACMPN0_INPUT,
			FOSC0);

	/* Start the ACIFA1. */
	acifa_start(&AVR32_ACIFA1,
			ACIFA_COMP_SELA);

	/* Display a header to user */
	print_dbg("\x1B[2J\x1B[H\r\nACIFA Example\r\n");

	while (true) {
		if (acifa_is_aca_inp_higher(&AVR32_ACIFA1)) {
			print_dbg("ACMP0 > ACMPN0");
			print_dbg("\r\n");
		} else {
			print_dbg("ACMP0 < ACMPN0");
			print_dbg("\r\n");
		}

		if (acifa_is_aca_inp_higher(&AVR32_ACIFA0)) {
			print_dbg("ACMP1 > ACMPN1");
			print_dbg("\r\n");
		} else {
			print_dbg("ACMP1 < ACMPN1");
			print_dbg("\r\n");
		}

		if (acifa_is_acb_inp_higher(&AVR32_ACIFA0)) {
			print_dbg("ACMP2 > ACMPN2");
			print_dbg("\r\n");
		} else {
			print_dbg("ACMP2 < ACMPN2");
			print_dbg("\r\n");
		}

		/* Slow down display of comparison values */
		delay_ms(100);
	}
}
