/*****************************************************************************
*
* \file
*
* \brief ADC example driver for AVR UC3.
*
* This file provides an example for the ADC on AVR UC3 devices.
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
 * Example for the UC3 ADC driver. This example gives a practical demonstration
 * of the ASF UC3 ADC driver usage.
 *
 * \section files Main Files
 * - adc_example.c : ADC code example
 * - conf_example.h : Example board hardware configuration definitions
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR UC3. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR UC3 devices with a ADC module can be used. This example has been
 * tested with the following setup:
 * - EVK1100 evaluation kit,
 * - EVK1101 evaluation kit,
 * - EVK1104 evaluation kit.
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> 12 MHz </i>
 * - [on EVK1100 and EVK1101 only] Connect USART_1 to your serial port via a
 *  standard RS-232 D-SUB9 cable. Set the following settings in your terminal of
 *  choice: 57600 8N1
 * - [on EVK1104 only] Connect a PC USB cable to the USB VCP plug (the USB plug
 *  on the right) of the EVK1104. The PC is used as a power source. The UC3A3256
 *  USART1 is connected to the UC3B USART1. The UC3B holds a firmware that acts
 *  as a USART to USB gateway. On the USB side, the UC3B firmware implements a
 *  USB CDC class: when connected to a PC, it will enumerate as a Virtual Com 
 *  Port.Once the UC3B USB is correctly installed on Windows, to communicate on
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

/** GPIO pin/adc-function map. */
const gpio_map_t ADC_GPIO_MAP = {
#if defined(EXAMPLE_ADC_TEMPERATURE_CHANNEL)
	{EXAMPLE_ADC_TEMPERATURE_PIN, EXAMPLE_ADC_TEMPERATURE_FUNCTION},
#endif
#if defined(EXAMPLE_ADC_LIGHT_CHANNEL)
	{EXAMPLE_ADC_LIGHT_PIN, EXAMPLE_ADC_LIGHT_FUNCTION},
#endif
#if defined(EXAMPLE_ADC_POTENTIOMETER_CHANNEL)
	{EXAMPLE_ADC_POTENTIOMETER_PIN, EXAMPLE_ADC_POTENTIOMETER_FUNCTION}
#endif
};

/** \brief Main application entry point - init and loop to display ADC values */
int main(void)
{
#if defined(EXAMPLE_ADC_TEMPERATURE_CHANNEL)
	signed short adc_value_temp  = -1;
#endif
#if defined(EXAMPLE_ADC_LIGHT_CHANNEL)
	signed short adc_value_light = -1;
#endif
#if defined(EXAMPLE_ADC_POTENTIOMETER_CHANNEL)
	signed short adc_value_pot   = -1;
#endif

	/* Init system clocks */
	sysclk_init();

	/* init debug serial line */
	init_dbg_rs232(sysclk_get_cpu_hz());

	/* Assign and enable GPIO pins to the ADC function. */
	gpio_enable_module(ADC_GPIO_MAP, sizeof(ADC_GPIO_MAP) /
			sizeof(ADC_GPIO_MAP[0]));

	/* Configure the ADC peripheral module.
	 * Lower the ADC clock to match the ADC characteristics (because we
	 * configured the CPU clock to 12MHz, and the ADC clock characteristics are
	 *  usually lower; cf. the ADC Characteristic section in the datasheet). */
	AVR32_ADC.mr |= 0x1 << AVR32_ADC_MR_PRESCAL_OFFSET;
	adc_configure(&AVR32_ADC);

	/* Enable the ADC channels. */
#if defined(EXAMPLE_ADC_TEMPERATURE_CHANNEL)
	adc_enable(&AVR32_ADC, EXAMPLE_ADC_TEMPERATURE_CHANNEL);
#endif
#if defined(EXAMPLE_ADC_LIGHT_CHANNEL)
	adc_enable(&AVR32_ADC, EXAMPLE_ADC_LIGHT_CHANNEL);
#endif
#if defined(EXAMPLE_ADC_POTENTIOMETER_CHANNEL)
	adc_enable(&AVR32_ADC, EXAMPLE_ADC_POTENTIOMETER_CHANNEL);
#endif

	/* Display a header to user */
	print_dbg("\x1B[2J\x1B[H\r\nADC Example\r\n");

	while (true) {
		/* Start conversions on all enabled channels */
		adc_start(&AVR32_ADC);

#if defined(EXAMPLE_ADC_TEMPERATURE_CHANNEL)
		/* Get value for the temperature adc channel */
		adc_value_temp = adc_get_value(&AVR32_ADC,
				EXAMPLE_ADC_TEMPERATURE_CHANNEL);

		/* Display value to user */
		print_dbg("HEX Value for Channel temperature : 0x");
		print_dbg_hex(adc_value_temp);
		print_dbg("\r\n");
#endif

#if defined(EXAMPLE_ADC_LIGHT_CHANNEL)
		/* Get value for the light adc channel */
		adc_value_light = adc_get_value(&AVR32_ADC,
				EXAMPLE_ADC_LIGHT_CHANNEL);
		
		/* Display value to user */
		print_dbg("HEX Value for Channel light : 0x");
		print_dbg_hex(adc_value_light);
		print_dbg("\r\n");
#endif

#if defined(EXAMPLE_ADC_POTENTIOMETER_CHANNEL)
		/* Get value for the potentiometer adc channel */
		adc_value_pot = adc_get_value(&AVR32_ADC,
				EXAMPLE_ADC_POTENTIOMETER_CHANNEL);
				
		/* Display value to user */
		print_dbg("HEX Value for Channel pot : 0x");
		print_dbg_hex(adc_value_pot);
		print_dbg("\r\n");
#endif

		/* Slow down the display of converted values */
		delay_ms(500);
	}

	return 0;
}
