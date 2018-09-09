/**
 * \file
 *
 * \brief 32-bit NLMS filtering
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
 * This example demonstrates a way to use the 32-bit NLMS Filter function.\n
 * To visualize the result, you may set a breakpoint after the sprintf and
 * check the string variable output through Watch Window in AVR Studio 5.
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section files Main Files
 * - nlms_example.c : DSPLIB NLMS Filter example
 * - conf_board.h: Simulator configuration for example
 * - conf_clock.h: Clock configuration for example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - AVR Simulator
 * - CPU clock: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/design-centers/32-bit /">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"
#include "dsp_debug.h"

#include "sysclk.h"
#include "cycle_counter.h"

//! The size of the input signal
#define SIZE  64

//! Buffer containing input data
A_ALIGNED dsp32_t input_x[SIZE] = {
	DSP32_Q(0.00000000000),
	DSP32_Q(0.03718075139),
	DSP32_Q(0.07131184859),
	DSP32_Q(0.09963983090),
	DSP32_Q(0.11997464035),
	DSP32_Q(0.13090169944),
	DSP32_Q(0.13191810690),
	DSP32_Q(0.12347962859),
	DSP32_Q(0.10695389264),
	DSP32_Q(0.08448437894),
	DSP32_Q(0.05877852523),
	DSP32_Q(0.03284069954),
	DSP32_Q(0.00967618536),
	DSP32_Q(-0.00800483670),
	DSP32_Q(-0.01805432735),
	DSP32_Q(-0.01909830056),
	DSP32_Q(-0.01067285908),
	DSP32_Q(0.00672896857),
	DSP32_Q(0.03170418002),
	DSP32_Q(0.06207594915),
	DSP32_Q(0.09510565163),
	DSP32_Q(0.12776001555),
	DSP32_Q(0.15700725030),
	DSP32_Q(0.18011316957),
	DSP32_Q(0.19490832447),
	DSP32_Q(0.20000000000),
	DSP32_Q(0.19490832447),
	DSP32_Q(0.18011316957),
	DSP32_Q(0.15700725030),
	DSP32_Q(0.12776001555),
	DSP32_Q(0.09510565163),
	DSP32_Q(0.06207594915),
	DSP32_Q(0.03170418002),
	DSP32_Q(0.00672896857),
	DSP32_Q(-0.01067285908),
	DSP32_Q(-0.01909830056),
	DSP32_Q(-0.01805432735),
	DSP32_Q(-0.00800483670),
	DSP32_Q(0.00967618536),
	DSP32_Q(0.03284069954),
	DSP32_Q(0.05877852523),
	DSP32_Q(0.08448437894),
	DSP32_Q(0.10695389264),
	DSP32_Q(0.12347962859),
	DSP32_Q(0.13191810690),
	DSP32_Q(0.13090169944),
	DSP32_Q(0.11997464035),
	DSP32_Q(0.09963983090),
	DSP32_Q(0.07131184859),
	DSP32_Q(0.03718075139),
	DSP32_Q(0.00000000000),
	DSP32_Q(-0.03718075139),
	DSP32_Q(-0.07131184859),
	DSP32_Q(-0.09963983090),
	DSP32_Q(-0.11997464035),
	DSP32_Q(-0.13090169944),
	DSP32_Q(-0.13191810690),
	DSP32_Q(-0.12347962859),
	DSP32_Q(-0.10695389264),
	DSP32_Q(-0.08448437894),
	DSP32_Q(-0.05877852523),
	DSP32_Q(-0.03284069954),
	DSP32_Q(-0.00967618536),
	DSP32_Q(0.00800483670)
};

//! Buffer containing filtered input data
A_ALIGNED dsp32_t input_d[SIZE] = {
	DSP32_Q(0.0000),
	DSP32_Q(0.0005),
	DSP32_Q(0.0017),
	DSP32_Q(0.0045),
	DSP32_Q(0.0100),
	DSP32_Q(0.0191),
	DSP32_Q(0.0320),
	DSP32_Q(0.0480),
	DSP32_Q(0.0655),
	DSP32_Q(0.0823),
	DSP32_Q(0.0960),
	DSP32_Q(0.1043),
	DSP32_Q(0.1061),
	DSP32_Q(0.1014),
	DSP32_Q(0.0907),
	DSP32_Q(0.0756),
	DSP32_Q(0.0581),
	DSP32_Q(0.0403),
	DSP32_Q(0.0245),
	DSP32_Q(0.0127),
	DSP32_Q(0.0064),
	DSP32_Q(0.0066),
	DSP32_Q(0.0137),
	DSP32_Q(0.0272),
	DSP32_Q(0.0463),
	DSP32_Q(0.0692),
	DSP32_Q(0.0940),
	DSP32_Q(0.1183),
	DSP32_Q(0.1401),
	DSP32_Q(0.1573),
	DSP32_Q(0.1683),
	DSP32_Q(0.1721),
	DSP32_Q(0.1683),
	DSP32_Q(0.1573),
	DSP32_Q(0.1401),
	DSP32_Q(0.1183),
	DSP32_Q(0.0940),
	DSP32_Q(0.0692),
	DSP32_Q(0.0463),
	DSP32_Q(0.0272),
	DSP32_Q(0.0137),
	DSP32_Q(0.0066),
	DSP32_Q(0.0064),
	DSP32_Q(0.0127),
	DSP32_Q(0.0245),
	DSP32_Q(0.0403),
	DSP32_Q(0.0581),
	DSP32_Q(0.0756),
	DSP32_Q(0.0907),
	DSP32_Q(0.1014),
	DSP32_Q(0.1061),
	DSP32_Q(0.1039),
	DSP32_Q(0.0943),
	DSP32_Q(0.0778),
	DSP32_Q(0.0555),
	DSP32_Q(0.0289),
	DSP32_Q(0.0000),
	DSP32_Q(-0.0289),
	DSP32_Q(-0.0555),
	DSP32_Q(-0.0778),
	DSP32_Q(-0.0943),
	DSP32_Q(-0.1039),
	DSP32_Q(-0.1061),
	DSP32_Q(-0.1014)
};

#define FIR_COEF_SIZE   12
A_ALIGNED dsp32_t unknown_coef_filter[FIR_COEF_SIZE] = {
	DSP32_Q(0.0132),
	DSP32_Q(0.0254),
	DSP32_Q(0.0579),
	DSP32_Q(0.1006),
	DSP32_Q(0.1398),
	DSP32_Q(0.1632),
	DSP32_Q(0.1632),
	DSP32_Q(0.1398),
	DSP32_Q(0.1006),
	DSP32_Q(0.0579),
	DSP32_Q(0.0254),
	DSP32_Q(0.0132)
};

A_ALIGNED dsp32_t x[FIR_COEF_SIZE];

//! Coefficients of the filter
A_ALIGNED dsp32_t w[FIR_COEF_SIZE];

//! The main function
int main(int argc, char *argv[])
{
	dsp32_t sample_x,sample_d;
	dsp32_t y, e;
	int i;
	char nlms_vect_result[FIR_COEF_SIZE][15];

	/**
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();

		// Initialize the DSP debug USART module when running in external board
#if BOARD != AVR_SIMULATOR_UC3
	dsp_debug_initialization(FOSC0);
#endif

	// Initialization of the buffers
	for (i = 0; i < FIR_COEF_SIZE; i++) {
		w[i] = 0;
		x[i] = 0;
	}

	for (i = 0; i < SIZE; i++) {
		// Compute a new sample
		sample_x = input_x[i];
		sample_d = input_d[i];
		// Compute the NLMS filter
		dsp32_filt_nlms(x, w, FIR_COEF_SIZE, sample_x, sample_d, &y, &e);
	}

	// Print the  output signal
	for (i = 0; i < FIR_COEF_SIZE; i++) {
		dsp32_debug_sprintf(nlms_vect_result[i],"%f", w[i]);
	}
	/*
	 * Place a breakpoint here and check the ASCII output in nlms_vect_result
	 * in Memory Window.
	 * Note: Find the variable address in Watch Window and enter it in
	 * Memory Window
	 */
	while (1) {
		// Intentionally left blank.
	}
}
