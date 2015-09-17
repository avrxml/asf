/**
 * \file
 *
 * \brief 32-bit IIR filtering
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
 * This example demonstrates how to use the 32-bit IIR Filter function.\n
 * To visualize the result, you may set a breakpoint after the sprintf and
 * check the string variable output through Watch Window in AVR Studio 5.
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section files Main Files
 * - iir_example.c : DSPLIB IIR Filter example
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

// The size of the input signal
#define SIZE  48

// A High-pass IIR Filter
// Butterworth model
// Fsample = 48 KHz
// Fcut-off = 2000 Hz
// Order 5

#define NUM_SIZE    6
#define NUM_PREDIV  3
#define DEN_SIZE    5
#define DEN_PREDIV  3

A_ALIGNED dsp32_t num[NUM_SIZE] = {
	DSP32_Q(0.6537018  / (1 << NUM_PREDIV)),
	DSP32_Q(-3.2685088 / (1 << NUM_PREDIV)),
	DSP32_Q(6.5370176  / (1 << NUM_PREDIV)),
	DSP32_Q(-6.5370176 / (1 << NUM_PREDIV)),
	DSP32_Q(3.2685088  / (1 << NUM_PREDIV)),
	DSP32_Q(-0.6537018 / (1 << NUM_PREDIV))
};

A_ALIGNED dsp32_t den[DEN_SIZE] = {
	DSP32_Q(-4.1534907 / (1 << DEN_PREDIV)),
	DSP32_Q(6.9612765  / (1 << DEN_PREDIV)),
	DSP32_Q(-5.877997  / (1 << DEN_PREDIV)),
	DSP32_Q(2.498365   / (1 << DEN_PREDIV)),
	DSP32_Q(-0.427326  / (1 << DEN_PREDIV))
};

//! The output buffer
A_ALIGNED dsp32_t y[SIZE + DEN_SIZE] = {
	DSP32_Q(0.)
};

//! The input signal (sampling rate: 48KHz)
//! A mix between a 4000 Hz and a 1000 Hz signal
//! (mixed by addition)
A_ALIGNED dsp32_t x[SIZE + NUM_SIZE - 1] = {
	// ----Previous Data---
	DSP32_Q(-0.00363360317),
	DSP32_Q(0.01830127019),
	DSP32_Q(0.04619397663),
	DSP32_Q(0.07329629131),
	DSP32_Q(0.09287351326),
	// --------------------
	DSP32_Q(0.10000000000),
	DSP32_Q(0.09287351326),
	DSP32_Q(0.07329629131),
	DSP32_Q(0.04619397663),
	DSP32_Q(0.01830127019),
	DSP32_Q(-0.00363360317),
	DSP32_Q(-0.01464466094),
	DSP32_Q(-0.01286319874),
	DSP32_Q(-0.00000000000),
	DSP32_Q(0.01913417162),
	DSP32_Q(0.03794095226),
	DSP32_Q(0.04982757980),
	DSP32_Q(0.05000000000),
	DSP32_Q(0.03677496058),
	DSP32_Q(0.01205904774),
	DSP32_Q(-0.01913417162),
	DSP32_Q(-0.05000000000),
	DSP32_Q(-0.07373934164),
	DSP32_Q(-0.08535533906),
	DSP32_Q(-0.08296893720),
	DSP32_Q(-0.06830127019),
	DSP32_Q(-0.04619397663),
	DSP32_Q(-0.02329629131),
	DSP32_Q(-0.00627097288),
	DSP32_Q(0.00000000000),
	DSP32_Q(-0.00627097288),
	DSP32_Q(-0.02329629131),
	DSP32_Q(-0.04619397663),
	DSP32_Q(-0.06830127019),
	DSP32_Q(-0.08296893720),
	DSP16_Q(-0.08535533906),
	DSP32_Q(-0.07373934164),
	DSP32_Q(-0.05000000000),
	DSP32_Q(-0.01913417162),
	DSP32_Q(0.01205904774),
	DSP32_Q(0.03677496058),
	DSP32_Q(0.05000000000),
	DSP32_Q(0.04982757980),
	DSP32_Q(0.03794095226),
	DSP32_Q(0.01913417162),
	DSP32_Q(0.00000000000),
	DSP32_Q(-0.01286319874),
	DSP32_Q(-0.01464466094),
	DSP32_Q(-0.00363360317),
	DSP32_Q(0.01830127019),
	DSP32_Q(0.04619397663),
	DSP32_Q(0.07329629131),
	DSP32_Q(0.09287351326)
};

//! The main function
int main(int argc, char *argv[])
{
	int i;
	char iir_vect_result[SIZE][15];

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

	// Number of recursions - Reduced to 5 decrease the time in Simulator
	for (i = 0; i < 5; i++) {
		// Perform a IIR filter
		dsp32_filt_iir(&y[DEN_SIZE], &x[NUM_SIZE - 1], SIZE, num, NUM_SIZE,
				den, DEN_SIZE, NUM_PREDIV, DEN_PREDIV);
		// Update the output signal
		memmove(y, &y[SIZE], (DEN_SIZE)*sizeof(dsp32_t));
	}
	// Print the  output signal
	for (i = 0; i < SIZE; i++) {
		dsp32_debug_sprintf(iir_vect_result[i],"%f", y[i]);
	}
	/*
	 * Place a breakpoint here and check the ASCII output in iir_vect_result
	 * in Memory Window.
	 * Note: Find the variable address in Watch Window and enter it in
	 * Memory Window
	 */

	while (1) {
		// Intentionally left blank.
	}
}
