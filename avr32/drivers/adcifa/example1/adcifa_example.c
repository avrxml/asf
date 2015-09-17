/**
 * \file
 *
 * \brief ADCIFA driver example
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

/** \mainpage
 * \section intro Introduction
 * Example use of the ASF ADCIFA driver for UC3 devices, giving a demonstration
 * of how to set up the module and perform conversions. It also performs an
 * offset calibration before starting ADC applicative configuration.
 *
 * \section files Main Files
 * - adcifa_example.c : ADCIFA code example
 * - conf_example.h : Example configuration for the chosen board hardware
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR UC3. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR UC3 devices with a ADCIFA module can be used. This example has been
 * tested with the following setup:
 * - UC3C_EK evaluation kit
 *
 * \section setupinfo Setup Information
 * CPU speed: <i>16 MHz</i>
 * - [on UC3C_EK only] Connect a PC USB cable to the USB VCP plug (the USB plug
 * on the right) of the UC3C_EK. The PC is used as a power source. The UC3C0512C
 * USART2 is connected to the UC3B USART1. The UC3B holds a firmware that acts
 * as a USART to USB gateway. It implements a USB CDC class: when connected to a
 * PC, it will enumerate as a Virtual Com Port. Once the UC3B USB is correctly
 * installed on Windows, to communicate on this port, open a HyperTerminal
 * configured with the following settings: 57600 bps, 8 data bits, no parity
 * bit, 1 stop bit, no flow control.
 *
 * \section outputinfo Output Information
 *
 * - [on UC3C_EK only]
 *
 * - Potentiometer is connected to ADC Input 5
 * (Configured as first channel in the sequence)
 * - Mic Input is connected to ADC Input 14
 * (Configured as second channel in the sequence)
 *
 * The demo code will display the potentiometer sensor
 * (equivalent ADC Counts with gain 1)
 * and mic input (equivalent ADC Counts with gain 8) on serial terminal.
 *
 * For example rotate the potentiometer position and see the value change
 * on a serial terminal.
 *
 * There are 2 window monitor modes in ADCIFA..
 * In the code the window monitor mode 0 is used to monitor the
 * potentiometer ADC result and the window monitor mode 1 is used to monitor the
 * Mic ADC result.
 * For both channels, the window result will be active if the respective
 * ADC counts > low threshold value (0x1FF) of the respective window mode.
 *
 * For example if the potentiometer count is less than 0x1FF we can see the
 * window monitor result as "Inside Range" on serial terminal.If the ADC count
 * is greater than 0x1FF we can see the window monitor result as "Out of Range".
 *
 * If the count is less than 0x1FF, rotate the potentiometer position
 * (clock wise),we can see the change in window monitor result from
 * "Inside Range" to "Out of Range".
 *
 * If the count is greater than 0x1FF, rotate the potentiometer position
 * (anti clock wise),we can see the change in window monitor result from
 * "Out of Range" to 'Inside Range".
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com/avr">Atmel AVR</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "conf_example.h"

/* Interrupt Level 0 */
#define ADC_INTERRUPT_PRIORITY  0

/* Interrupt Group is not defined in IAR compiler's device part header file */
#ifndef AVR32_ADCIFA_IRQ_GROUP
#define AVR32_ADCIFA_IRQ_GROUP 36
#endif

unsigned char volatile window1;
unsigned char volatile window2;

/**
 * This ISR is executed if the window 1 or window 2 or both of the window
 * conditions are reached.
 */
ISR(ADCIFA_interrupt_handler, AVR32_ADCIFA_IRQ_GROUP, ADC_INTERRUPT_PRIORITY)
{
	/* Window Monitor 0 */
	if(ADCIFA_is_window_0_set()) {
		window1 = true;
		/* Acknowledge the Interrupt */
		ADCIFA_clear_window_0();
		/* Disable the Interrupt */
		adcifa_disable_interrupt(&AVR32_ADCIFA,AVR32_ADCIFA_IDR_WM0_MASK);
	}
	/* Window Monitor 1 */
	if(ADCIFA_is_window_1_set()) {
		window2 = true;
		/* Acknowledge the Interrupt */
		ADCIFA_clear_window_1();
		/* Disable the Interrupt */
		adcifa_disable_interrupt(&AVR32_ADCIFA,AVR32_ADCIFA_IDR_WM1_MASK);
	}
}
/* End of ISR routine */

/** \brief Main function to initialize the system and loop to display ADC values
 *
 */
int main( void )
{
	int16_t adc_values[EXAMPLE_ADCIFA_NUMBER_OF_SEQUENCE];
	adcifa_sequencer_opt_t adcifa_sequence_opt ;
	adcifa_sequencer_conversion_opt_t
		adcifa_sequence_conversion_opt[EXAMPLE_ADCIFA_NUMBER_OF_SEQUENCE];

	adcifa_opt_t adc_config_t ;

	adcifa_window_monitor_opt_t adcifa_window_monitor_0_opt;
	adcifa_window_monitor_opt_t adcifa_window_monitor_1_opt;


	/* GPIO pin/adc-function map. */
	const gpio_map_t ADCIFA_GPIO_MAP = {
		{AVR32_ADCREF0_PIN, AVR32_ADCREF0_FUNCTION},
		{AVR32_ADCREFP_PIN, AVR32_ADCREFP_FUNCTION},
		{AVR32_ADCREFN_PIN, AVR32_ADCREFN_FUNCTION},
		{EXAMPLE_ADC_POTENTIOMETER_PIN, EXAMPLE_ADC_POTENTIOMETER_FUNCTION},
		{EXAMPLE_ADC_MIC_PIN, EXAMPLE_ADC_MIC_FUNCTION}
	};

	/* Init system clocks */
	sysclk_init();

	/* Init debug serial line */
	init_dbg_rs232(sysclk_get_cpu_hz());

	/* Assign and enable GPIO pins to the ADC function. */
	gpio_enable_module(ADCIFA_GPIO_MAP, sizeof(ADCIFA_GPIO_MAP) /
			sizeof(ADCIFA_GPIO_MAP[0]));

	/* Configure the ADC for the application*/
	adc_config_t.frequency                = 1000000;
	adc_config_t.reference_source         = ADCIFA_ADCREF0;
	adc_config_t.sample_and_hold_disable  = false;
	adc_config_t.single_sequencer_mode    = false;
	adc_config_t.free_running_mode_enable = false;
  	adc_config_t.sleep_mode_enable        = false;
    adc_config_t.mux_settle_more_time     = false;

	/* Get ADCIFA Factory Configuration */
	adcifa_get_calibration_data(&AVR32_ADCIFA, &adc_config_t);

	/* Calibrate offset first*/
	adcifa_calibrate_offset(&AVR32_ADCIFA, &adc_config_t, sysclk_get_cpu_hz());

	/* Configure ADCIFA core */
	adcifa_configure(&AVR32_ADCIFA, &adc_config_t, sysclk_get_cpu_hz());

	/* ADCIFA sequencer 0 configuration structure*/
	adcifa_sequence_opt.convnb              = EXAMPLE_ADCIFA_NUMBER_OF_SEQUENCE;
	adcifa_sequence_opt.resolution          = ADCIFA_SRES_12B;
	adcifa_sequence_opt.trigger_selection   = ADCIFA_TRGSEL_SOFT;
	adcifa_sequence_opt.start_of_conversion = ADCIFA_SOCB_ALLSEQ;
	adcifa_sequence_opt.sh_mode             = ADCIFA_SH_MODE_OVERSAMP;
	adcifa_sequence_opt.half_word_adjustment= ADCIFA_HWLA_NOADJ;
	adcifa_sequence_opt.software_acknowledge= ADCIFA_SA_NO_EOS_SOFTACK;

	/* ADCIFA conversions for sequencer 0*/
	adcifa_sequence_conversion_opt[0].channel_p = EXAMPLE_ADC_POTENTIOMETER_INP;
	adcifa_sequence_conversion_opt[0].channel_n = EXAMPLE_ADC_POTENTIOMETER_INN;
	adcifa_sequence_conversion_opt[0].gain      = ADCIFA_SHG_1;
	adcifa_sequence_conversion_opt[1].channel_p = EXAMPLE_ADC_MIC_INP;
	adcifa_sequence_conversion_opt[1].channel_n = EXAMPLE_ADC_MIC_INN;
	adcifa_sequence_conversion_opt[1].gain      = ADCIFA_SHG_8;

	/* Window Monitor 0 Configuration */
	/* Window Mode 2 -> Active (If Result Register > Low Threshold) */
	adcifa_window_monitor_0_opt.mode           = ADCIFA_WINDOW_MODE_ABOVE;
	/* First channel in the configured sequence */
	adcifa_window_monitor_0_opt.source_index   = 0;
	/* Low Threshold Equivalent ADC counts */
	adcifa_window_monitor_0_opt.low_threshold  = 0x1FF;
	/* High Threshold Equivalent ADC counts */
	adcifa_window_monitor_0_opt.high_threshold = 0;

	/* Window Monitor 1 Configuration */
	/* Window Mode 2 -> Active (If Result Register > Low Threshold) */
	adcifa_window_monitor_1_opt.mode           = ADCIFA_WINDOW_MODE_ABOVE;
	/* Second channel in the configured sequence */
	adcifa_window_monitor_1_opt.source_index   = 1;
	/* Low Threshold Equivalent ADC counts */
	adcifa_window_monitor_1_opt.low_threshold  = 0x1FF;
	/* High Threshold Equivalent ADC counts */
	adcifa_window_monitor_1_opt.high_threshold = 0;

	/*
	 * Initialize the interrupt vectors
	 * Note: This function adds nothing for IAR as the interrupts are
	 * handled by the IAR compiler itself. It provides an abstraction
	 * between GCC & IAR compiler to use interrupts.
	 * Refer function implementation in interrupt_avr32.h
	 */
	irq_initialize_vectors();

	/*
	 * Register the ADCIFA interrupt handler
	 * Note: This function adds nothing for IAR as the interrupts are
	 * handled by the IAR compiler itself. It provides an abstraction
	 * between GCC & IAR compiler to use interrupts.
	 * Refer function implementation in interrupt_avr32.h
	 */
	irq_register_handler(&ADCIFA_interrupt_handler, AVR32_ADCIFA_WINDOW_IRQ,
			ADC_INTERRUPT_PRIORITY);

	/* Enable global Interrupts */
	Enable_global_interrupt();

	/* Configure ADCIFA Window Monitor 0 */
	adcifa_configure_window_monitor(&AVR32_ADCIFA, 0 ,
		&adcifa_window_monitor_0_opt);

	/* Configure ADCIFA Window Monitor 1 */
	adcifa_configure_window_monitor(&AVR32_ADCIFA, 1 ,
		&adcifa_window_monitor_1_opt);

	/* Enable the Window Monitor 0 Interrupt */
	adcifa_enable_interrupt(&AVR32_ADCIFA,AVR32_ADCIFA_IDR_WM0_MASK);

	/* Enable the Window Monitor 1 Interrupt */
	adcifa_enable_interrupt(&AVR32_ADCIFA,AVR32_ADCIFA_IDR_WM1_MASK);


	/* Configure ADCIFA sequencer 0 */
	adcifa_configure_sequencer(&AVR32_ADCIFA, 0, &adcifa_sequence_opt,
			adcifa_sequence_conversion_opt);

	/* Start ADCIFA sequencer 0 */
	adcifa_start_sequencer(&AVR32_ADCIFA, 0);

	while (true) {
		/* Get Values from sequencer 0 */
		if (adcifa_get_values_from_sequencer(&AVR32_ADCIFA, 0,
				&adcifa_sequence_opt, adc_values) == ADCIFA_STATUS_COMPLETED) {

			/* Display values to user */

			/* Display header */
			print_dbg("\x1B[2J\x1B[H\r\nADCIFA Example\r\n");

			print_dbg("HEX Value for Channel potentiometer: 0x");
			print_dbg_hex(adc_values[0]);
			print_dbg("\r\n");

			if(window1) {
				/* Out of Range -> Action */
				print_dbg(" Window 1 : Out of Range\r\n");
				window1=false;
				adcifa_enable_interrupt(&AVR32_ADCIFA,AVR32_ADCIFA_IDR_WM0_MASK);
			} else {
				print_dbg(" Window 1 : Inside Range\r\n");
			}

			print_dbg("HEX Value for Channel microphone: 0x");
			print_dbg_hex(~adc_values[1]);
			print_dbg("\r\n");

			if(window2) {
				/* Out of Range -> Action */
				print_dbg(" Window 2 : Out of Range\r\n");
				window2=false;
				adcifa_enable_interrupt(&AVR32_ADCIFA,AVR32_ADCIFA_IDR_WM1_MASK);
			} else {
				print_dbg(" Window 2 : Inside Range\r\n");
			}

			/*
			 * Clear end-of-sequence for sequencer 0,
			 * ready for next conversion.
			 */
			ADCIFA_clear_eos_sequencer_0();

			/* Start new ADCIFA sequencer 0 */
			adcifa_start_sequencer(&AVR32_ADCIFA, 0);
		}

		/* Slow down display of converted values */
		delay_ms(100);
	}
}
