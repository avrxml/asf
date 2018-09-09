/**
 * \file
 *
 * \brief megaAVR ADC example 1
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
