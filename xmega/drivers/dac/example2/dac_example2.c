/**
 * \file
 *
 * \brief AVR XMEGA Digital to Analog Converter Driver Example 2
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
 *
 * \section intro Introduction
 * This example demonstrates use of the \ref dac_group for dual-channel output
 * with automatic refresh.
 *
 * \section files Main files:
 * - dac_example2.c: DAC driver example application
 * - conf_board.h: board initialization configuration
 * - conf_example.h: example application configuration
 * - conf_clock.h: system clock configuration
 *
 * \section apiinfo DAC driver API
 * The DAC driver API can be found \ref dac_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with a DAC can be used.
 *
 * \section exampledescription Description of the example
 * This example sets up the configured DAC (\ref OUTPUT_DAC) to perform DA
 * conversions on both channels, with output to their respective pins,
 * every time it receives events in event channel 0. Timer/counter 0 on port C
 * is used to generate the events on overflow. This setup gives a constant
 * conversion rate.
 *
 * The main loop in the example waits for the DAC channels to get ready for new
 * data, then alternately feeds them with values that are 10% and 90% of maximum
 * to demonstrate settling and hold time of the DAC.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

//! \name Example configuration
//@{

/**
 * \def OUTPUT_DAC
 * \brief DAC to use in example
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//@}

#include <conf_example.h>
#include <asf.h>

//! Rate at which DA conversions should be done.
#define RATE_OF_CONVERSION    500

__always_inline static void wait_for_timer(void)
{
#if !XMEGA_E
	do { } while (!(TCC0.INTFLAGS & TC0_OVFIF_bm));
	TCC0.INTFLAGS = TC0_OVFIF_bm;
#else
	do { } while (!(TCC4.INTFLAGS & TC4_OVFIF_bm));
	TCC4.INTFLAGS = TC4_OVFIF_bm;
#endif
}

int main(void)
{
	struct dac_config conf;

	board_init();
	sysclk_init();

	// Initialize the dac configuration.
	dac_read_configuration(&OUTPUT_DAC, &conf);

	/* Create configuration:
	 * - AVCC as reference, right adjusted channel value
	 * - both DAC channels active, no internal output
	 * - manually triggered conversions on both channels
	 * - 2 us conversion intervals
	 * - 10 us refresh intervals
	 */
	dac_set_conversion_parameters(&conf, DAC_REF_AVCC, DAC_ADJ_RIGHT);
	dac_set_active_channel(&conf, DAC_CH0 | DAC_CH1, 0);
	dac_set_conversion_trigger(&conf, 0, 0);
#if XMEGA_DAC_VERSION_1
	dac_set_conversion_interval(&conf, 10);
	dac_set_refresh_interval(&conf, 20);
#endif
	dac_write_configuration(&OUTPUT_DAC, &conf);
	dac_enable(&OUTPUT_DAC);

	dac_wait_for_channel_ready(&OUTPUT_DAC, DAC_CH0 | DAC_CH1);
	dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 0);
	dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 0);
	dac_wait_for_channel_ready(&OUTPUT_DAC, DAC_CH0 | DAC_CH1);

#if !XMEGA_E
	// Configure timer/counter to generate events at conversion rate.
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC0);
	TCC0.PER = (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1;

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;

	// Start the timer/counter.
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
#else
	// Configure timer/counter to generate events at conversion rate.
	sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC4);
	TCC4.PER = (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1;

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC4_OVF_gc;

	// Start the timer/counter.
	TCC4.CTRLA = TC45_CLKSEL_DIV1_gc;
	
#endif
	/* Write samples to the DAC channel every time it is ready.
	 * Conversions are triggered by the timer/counter.
	 */
	do {
		/* Wait for channels to get ready for new values, then set the
		 * value of one to 10% and the other to 90% of maximum.
		 */
		wait_for_timer();
		dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 410);
		dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 3686);

		wait_for_timer();
		dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 3686);
		dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 410);
	} while (1);
}
