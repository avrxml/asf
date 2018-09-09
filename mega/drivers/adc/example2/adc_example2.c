/**
 * \file
 *
 * \brief megaAVR ADC example 2 using interrupts
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
