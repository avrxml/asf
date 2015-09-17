/**
 * \file
 *
 * \brief megaAVR ADC example 2 using interrupts
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
 * This example demonstrates how to use the \ref mega_adc_group to set up the ADC
 * in freerunning mode.
 *
 * \section files Files:
 * - adc_example2.c: megaAVR STK600 ADC example 2
 * - adc.h: \ref mega_adc_group

 * \section exampledescription Brief description
 * This application will read the analog value on the ADC0 pin in freerunning
 * mode. An interrupt routine accumulates the readings and does an average
 * every 16 readings; storing it in a global variable.
 *
 * \note You must connect an analog voltage source to the ADC0 pin. If using
 * the STK600 you can use AREF0 or AREF1. Do not apply a voltage above the ADC
 * voltage reference.
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

// global variable holding the average calculated in the interrupt routine
volatile uint16_t avg;

/*
 * The interrupt accumulates the sum of 16 samples in the accum variable.
 * when 16 samples has been added, the sum is divided by 16 to find the average
 * and the result is stored in the global variable avg.
 */
ISR(ADC_vect)
{
	static uint16_t accum;
	static uint8_t i = 0;

	if (i == 16){
		avg = accum / 16;
		i = 0;
		accum = 0;
	} else {
		accum += ADC;
		i++;
	}
}

/*
 * To ensure that the data is not modified by the interrupt handler
 * while we read it, we disable interrupts before reading the global variable.
 */
static inline uint16_t read_global(void)
{
	uint16_t retval;
	irqflags_t flags;

	flags = cpu_irq_save();
	retval = avg;
	cpu_irq_restore(flags);

	return retval;
}

int main(void)
{

	// set PORTB as output
	DDRB = 0xFF;

	// set prescaler and enable ADC
	adc_init(ADC_PRESCALER_DIV128);

	// set voltage reference, mux input and right adjustment
	adc_set_admux(ADC_VREF_AVCC | ADC_MUX_ADC0 | ADC_ADJUSTMENT_RIGHT);

	adc_enable_autotrigger();
	adc_enable_interrupt();

	// start the first conversion starting the freerunning mode
	adc_start_conversion();

	cpu_irq_enable();

	// output MSB of average on PORTB.
	while (1) {
		PORTB = (read_global() >> 8);
	}
}
