/*****************************************************************************
*
* \file
*
* \brief ACIFA example driver for AVR UC3.
*
* This file provides an example for the ACIFA on AVR UC3 devices.
*
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products.
* It is your responsibility to comply with third party license terms applicable
* to your use of third party software (including open source software) that
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
 * <A href="http://www.microchip.com/design-centers/32-bit">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
