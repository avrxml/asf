/**
 * \file
 *
 * \brief megaAVR ADC example 3, measure bandgap voltage
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
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
