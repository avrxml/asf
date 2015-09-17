/**
 * \file
 *
 * \brief megaAVR ADC example 3, measure bandgap voltage
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
 * This example calculates the AVCC voltage by measuring the internal bandgap
 * voltage.
 *
 * \section files Files:
 * - adc_example3.c: megaAVR ADC example 3
 * - adc.h: \ref mega_adc_group
 *
 * \section exampledescription Brief description
 * This application will measure the internal bandgap voltage using AVCC as
 * the reference and then calculate AVCC based on the measurement.
 * Add the supply_voltage variable to your watch window to observe the
 * calculated value. To save size and speed the calculation is done with integer
 * math and not floating point. The resulting value is in millivolts.
 *
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

/* Internal bandgap voltages in mV. */
#if MEGA_RF
#       define BG_VOLTAGE       1200
#       define BG_MUX       ADC_MUX_1V2
#elif MEGA_XX8 || MEGA_XX0_1 || MEGA_XX4 || AVR8_PART_IS_DEFINED(ATmega169)
#       define BG_VOLTAGE   1100
#       define BG_MUX       ADC_MUX_1V1
#elif MEGA_XX
#       define BG_VOLTAGE   1220
#       define BG_MUX       ADC_MUX_1V22
#endif

int main(void)
{
	uint8_t i;
	uint16_t bandgap;
	uint16_t supply_voltage;
	volatile uint16_t dummy;

	/* initialize and enable the ADC */
	adc_init(ADC_PRESCALER_DIV128);

	/* Do some dummy reads to make sure the reference is stable */
	for (i = 0; i < 5; i++) {
		bandgap = adc_read_10bit(BG_MUX, ADC_VREF_AVCC);
	}

	/* measure bandgap voltage using AVCC as reference */
	bandgap = adc_read_10bit(BG_MUX, ADC_VREF_AVCC);

	/*
	 * Find AVCC based on the fact that we know the bandgap voltage.
	 * To avoid excessive rounding we multiply 1024 by 1000 before dividing
	 * with the ADC result and then divide the result by 1000 afterwards.
	 * the final result is the supply voltage in millivolts.
	 */
	supply_voltage = ((BG_VOLTAGE * ((1024000) / bandgap)) / 1000);

	/* add breakpoint here; dummy volatile to prevent the optimizer
	 * from throwing away the result before we can look at it.
	 */
	while (1) {
		dummy = supply_voltage;
	}
}
