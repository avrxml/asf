/**
 * \file
 *
 * \brief ADC and DAC test.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "classb_analog.h"
#include "error_handler.h"

/**
 * \brief Internal function for analog test
 *
 * This test writes a value to the DAC, and checks that the ADC readouts are
 * within the expected range, which is set with \ref CLASSB_ADC_DEV.
 *
 * \param dacptr Base address for registers of DAC module to test.
 * \param adcptr Base address for registers of ADC module to test.
 * \param dac_out Output value to test the DAC with.
 * \param adc_assert Expected ADC value to measure.
 */
static void classb_dac_adc_test(DAC_t *dacptr, ADC_t *adcptr, uint16_t dac_out,
		uint16_t adc_assert)
{
	/* Deviation in measurements */
	int16_t adc_dev;

	/* Write to DAC and wait until it's stable */
	dacptr->CH0DATA = dac_out;
	while( !(dacptr->STATUS & DAC_CH0DRE_bm) );

	/* Clear interrupt flag channel 3 */
	adcptr->CH3.INTFLAGS = ADC_CH_CHIF_bm;
	/* Start AD-conversions in channels 0 to 3 */
	adcptr->CTRLA |= (ADC_CH0START_bm | ADC_CH1START_bm | ADC_CH2START_bm
			| ADC_CH3START_bm);
	/* Use channel 3 interrupt flag to detect end of conversion
	 * (channel 3 has the lowest priority)
	 */
	while( !(adcptr->CH3.INTFLAGS & ADC_CH_CHIF_bm) );

	/* Do a range check on the conversion results */
	adc_dev = adcptr->CH0RES - adc_assert;
	if (abs(adc_dev) > CLASSB_ADC_DEV)
		CLASSB_ERROR_HANDLER_ANALOG();

	adc_dev = adcptr->CH1RES - adc_assert;
	if (abs(adc_dev) > CLASSB_ADC_DEV)
		CLASSB_ERROR_HANDLER_ANALOG();

	adc_dev = adcptr->CH2RES - adc_assert;
	if (abs(adc_dev) > CLASSB_ADC_DEV)
		CLASSB_ERROR_HANDLER_ANALOG();

	adc_dev = adcptr->CH3RES - adc_assert;
	if (abs(adc_dev) > CLASSB_ADC_DEV)
		CLASSB_ERROR_HANDLER_ANALOG();

	/* Clear interrupt flags */
	adcptr->CH0.INTFLAGS = ADC_CH_CHIF_bm;
	adcptr->CH1.INTFLAGS = ADC_CH_CHIF_bm;
	adcptr->CH2.INTFLAGS = ADC_CH_CHIF_bm;
	adcptr->CH3.INTFLAGS = ADC_CH_CHIF_bm;
}

/**
 * \brief Functional test for the ADC, DAC and analog MUX.
 *
 * This function configures the DAC to output five different voltages, which are
 * each measured with the ADC using 12-bit resolution, signed conversion and
 * 1V voltage reference.
 *
 * Range checking of the conversion results is done to verify that ADC and DAC
 * are working correctly.
 *
 * \param dacptr Base address for registers of DAC module to test.
 * \param adcptr Base address for registers of ADC module to test.
 */
void classb_analog_io_test(DAC_t *dacptr, ADC_t *adcptr)
{
	/* Enable and set up the DAC
	 * Single channel, 1V reference, internal output,
	 */
	dacptr->CTRLA = DAC_IDOEN_bm | DAC_ENABLE_bm;
	dacptr->CTRLB = DAC_CHSEL_SINGLE_gc;
	dacptr->CTRLC = DAC_REFSEL_INT1V_gc;
	/* dacptr->TIMCTRL = DAC_CONINTVAL_32CLK_gc; */

	/* Set up the ADC
	 * All channels will be connected to an internal input
	 */
	adcptr->CH0.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;
	adcptr->CH1.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;
	adcptr->CH2.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;
	adcptr->CH3.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;

	/* MUX selection: measure alternately from DAC and bandgap */
	adcptr->CH0.MUXCTRL = ADC_CH_MUXINT_DAC_gc;
	adcptr->CH1.MUXCTRL = ADC_CH_MUXINT_DAC_gc;
	adcptr->CH2.MUXCTRL = ADC_CH_MUXINT_DAC_gc;
	adcptr->CH3.MUXCTRL = ADC_CH_MUXINT_DAC_gc;

	/* Enable and set up ADC
	 * Signed conversions, 12 bit resolution, 1V ref. and prescale clock by
	 * 512
	 */
	adcptr->CTRLA = ADC_ENABLE_bm;
	adcptr->CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc;
	adcptr->REFCTRL = ADC_REFSEL_INT1V_gc;
	adcptr->PRESCALER = ADC_PRESCALER_DIV512_gc;

	/* Do the tests */
	classb_dac_adc_test(dacptr, adcptr, 0x000, 0x000);
	classb_dac_adc_test(dacptr, adcptr, 0x400, 0x200);
	classb_dac_adc_test(dacptr, adcptr, 0x800, 0x400);
	classb_dac_adc_test(dacptr, adcptr, 0xC00, 0x600);
	classb_dac_adc_test(dacptr, adcptr, 0xFFF, 0x800);

	/* Disable the ADC and DAC */
	adcptr->CTRLA &= ~ADC_ENABLE_bm;
	dacptr->CTRLA &= ~DAC_ENABLE_bm;
}
