/**
 * \file
 *
 * \brief 32-bit IIR filtering
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
