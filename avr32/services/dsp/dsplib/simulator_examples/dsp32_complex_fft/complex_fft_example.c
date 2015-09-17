/**
 * \file
 *
 * \brief 32 bits complex FFT program test
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

/**
 * \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the 32-bit complex FFT function.\n
 * To visualize the result, you may set a breakpoint after the sprintf and
 * check the string variable output through Watch Window in AVR Studio 5.
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section files Main Files
 * - complex_fft_example.c : DSPLIB Complex FFT example
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
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"
#include "dsp_debug.h"

#include "sysclk.h"
#include "cycle_counter.h"


// The size of the signal
#define SIZE  32
// log2(SIZE)
#define NLOG  6

// The output buffer
A_ALIGNED dsp32_complex_t vect1[SIZE];
A_ALIGNED dsp32_t         fft_real[SIZE];
// Input signal resulting from a multiplication between a cosine and a sine.
A_ALIGNED dsp32_t vect2[SIZE] = {
	DSP32_Q(0.0000000000),
	DSP32_Q(0.0597173328),
	DSP32_Q(0.1013967159),
	DSP32_Q(0.1101399733),
	DSP32_Q(0.0768494015),
	DSP32_Q(0.0000000000),
	DSP32_Q(-0.1137567428),
	DSP32_Q(-0.2502667883),
	DSP32_Q(-0.3897469095),
	DSP32_Q(-0.5096015650),
	DSP32_Q(-0.5877852523),
	DSP32_Q(-0.6062262391),
	DSP32_Q(-0.5538102421),
	DSP32_Q(-0.4284770086),
	DSP32_Q(-0.2381016864),
	DSP32_Q(-0.0000000000),
	DSP32_Q(0.2609116778),
	DSP32_Q(0.5150801430),
	DSP32_Q(0.7320204624),
	DSP32_Q(0.8842699856),
	DSP32_Q(0.9510565163),
	DSP32_Q(0.9211773270),
	DSP32_Q(0.7946870792),
	DSP32_Q(0.5831503900),
	DSP32_Q(0.3084072199),
	DSP32_Q(0.0000000000),
	DSP32_Q(-0.3084072199),
	DSP32_Q(-0.5831503900),
	DSP32_Q(-0.7946870792),
	DSP32_Q(-0.9211773270),
	DSP32_Q(-0.9510565163),
	DSP32_Q(-0.8842699856)
};

// The main function
int main(int argc, char *argv[])
{
	unsigned int cycle_count, i;
	volatile  dsp32_t fft32_max;
	char *temp = " +";
	char complex_vect_result[SIZE][31];

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

	// Get the actual cycle count
	cycle_count = Get_sys_count();
	// Perform a 64-point complex FFT
	dsp32_trans_realcomplexfft(vect1, vect2, NLOG);
	// Perform the absolute value of a complex vector
	dsp32_vect_complex_abs(fft_real, vect1, SIZE);
	// Retrieves the maximum of a vector
	fft32_max = dsp32_vect_max(fft_real, SIZE);

	// Calculate the number of cycles the FFT took
	cycle_count = Get_sys_count() - cycle_count;

	// Print the  output signal
	for (i = 0; i < SIZE; i++) {
		if (vect1[i].imag >= 0) {
			temp = " +";
		} else {
			temp = " ";
		}
		dsp32_debug_sprintf(complex_vect_result[i],"%f%s%f", vect1[i].real,
				temp,vect1[i].imag);
	}
	/*
	 * Place a breakpoint here and check ASCII output in complex_vect_result
	 * in Memory Window.
	 * Note: Find the variable address in Watch Window and enter it in
	 * Memory Window
	 */

	while (1) {
		// Intentionally left blank.
	}
}
