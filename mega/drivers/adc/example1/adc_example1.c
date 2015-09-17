/**
 * \file
 *
 * \brief megaAVR ADC example 1
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 * \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the \ref mega_adc_group to do a simple
 * ADC conversion.
 *
 * \section files Files:
 * - adc_example1.c: megaAVR ADC example 1
 * - adc.h: \ref mega_adc_group
 *
 * \section exampledescription Brief description
 * This application will read the analog value on the ADC0 pin and output the 8-bit
 * result on PORTB. in an infinite loop. If using an STK600, connect PORTB to the
 * STK600 LEDs.
 *
 * \note You must connect an analog voltage source to the ADC0 pin. If using
 * the STK600, you can use AREF0 or AREF1. Do not apply a voltage above the
 * ADC voltage reference.
 *
 * \note As the values in this example is not used for any meaningful
 * calculation we do not throw away the first reading after setting the
 * reference. You should always discard the first conversion after changing
 * the voltage reference as the reference may not yet be stable. You can find
 * more information about this in the ADC chapter of your device datasheet.
 *
 * \section compinfo Compilation Info
 * This software was written for the <A href="http://gcc.gnu.org/">GNU GCC</A>
 * for AVR, and <A href="http://iar.se">IAR Embedded Workbench</A>.\n
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "adc.h"

#define ADC_INPUT      ADC_MUX_ADC0
#define ADC_VREF           ADC_VREF_AVCC

int main(void)
{
	// set PORTB as output
	DDRB = 0xFF;

	// set prescaler and enable ADC
	adc_init(ADC_PRESCALER_DIV128);

	// output inverted ADC value on PORTB
	while (1) {
		PORTB = ~(adc_read_8bit(ADC_INPUT, ADC_VREF));
	}
}
