/**
 * \file
 *
 * \brief XMEGA ADC Oversampling Demo application Main File
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 *  This Projects demonstrates:\n
 *  - How to implement oversampling application with XMEGA ADC : \n
 *    The oversampling is used to increase the resolution of ADC hardware by
 *    implementing signal processing in firmware.
 *    In this example,configuration has been selected to increase
 *    resolution from 12 bit signed to 16 bit signed
 *  - More information can be obtained from application note
 *	  "Atmel AVR1629: XMEGA ADC Oversampling"
 *
 * \section files Main Files
 * - oversampling.c                : Oversampling demo application main file
 * - adc_oversampling.c            : ADC applications and signal processing
 *                                   functions related to oversampling
 * - conf_oversampling.h           : Configuration Macros for oversampling
 *
 * \section referenceinfo References
 *  - Refer Application Note: AVR121: Enhancing ADC resolution by oversampling
 *  - Refer Application Note: AVR120: ADC Characteristics and Calibration
 *  - Refer Application Note: AVR1300: Using the XMEGA ADC
 *  - Refer Application Note: AVR1505: XMEGA-A1 Xplained training - XMEGA ADC
 *
 * \section device_info Device Info
 * All AVR XMEGA device. This example have been tested with following setup:
 * - XMEGA-A3BU Xplained
 *
 * \section description Description of the example
 *
 * - All configurations related to oversampling level, ADC input pin, reference
 *   voltages, reference source and gain error factor are available from
 *   conf_oversampling.h file.
 * - To calculate a single result, this demo application will use number of
 *   sample as per configuration and perform signal processing to get higher
 *   resolution ADC result.
 * - After oversampling process, the converted analog input voltage and also
 *   ADC counts are displayed on LCD available in XMEGA-A3BU Xplained kit.
 * - Also, for comparison purpose, analog input voltage is calculated with only
 *   one sample(i.e. without oversampling) and this analog input voltage and
 *   corresponding ADC counts are displayed on LCD.
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <compiler.h>
#include <sleepmgr.h>
#include <sysclk.h>
#include <conf_oversampling.h>
#include <adc_oversampling.h>
#include <oversampling.h>
#include <asf.h>

/**
 * \brief Main application routine
 *  - Initializes the board and LCD display
 *  - Initialize ADC ,to read ADC offset and configure for oversampling
 *  - If number of sample Reached to  total number of oversample required,
 *    call function to start process on oversampled ADC readings
 */
int main( void )
{
	/*
	 * Initialize basic features for the AVR XMEGA family.
	 *  - PMIC is needed to enable all interrupt levels.
	 *  - Board init for setting up GPIO and board specific features.
	 *  - Sysclk init for configuring clock speed and turning off unused
	 *    peripherals.
	 *  - Sleepmgr init for setting up the basics for the sleep manager,
	 */

	board_init();
	sysclk_init();
	pmic_init();
	sleepmgr_init();

	/* Initialize ST7565R controller and LCD display */
	gfx_mono_init();

	/* Display headings on LCD for oversampled result */
	gfx_mono_draw_string("Oversampled", 0, 0, &sysfont);

	/* Display headings on LCD for normal result */
	gfx_mono_draw_string("Normal", 80, 0, &sysfont);

	/* Initialize ADC ,to read ADC offset and configure ADC for oversampling
	**/
	init_adc();

	/* Enable global interrupt */
	cpu_irq_enable();

	/* Switch ON LCD back light */
	ioport_set_pin_high(NHD_C12832A1Z_BACKLIGHT);

	/* Set LCD contrast */
	st7565r_set_contrast(ST7565R_DISPLAY_CONTRAST_MIN);

	/* Continuous Execution Loop */
	while (1) {
		/*
		 * Check if number of sample reached to  total Number of
		 * oversample required by checking status of
		 * adc_oversampled_flag
		 */
		if (adc_oversampled_flag == true) {
			/* Reset the adc_oversampled_flag */
			adc_oversampled_flag = false;

			/* Process all received ADC samples and calculate analog
			 * input */
			adc_oversampled();
		}
	}
}
