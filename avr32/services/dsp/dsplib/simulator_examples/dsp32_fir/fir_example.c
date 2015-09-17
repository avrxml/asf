/**
 * \file
 *
 * \brief 32 bits FIR filtering
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
 * This example demonstrates how to use the 32-bit FIR Filter function.\n
 * To visualize the result, you may set a breakpoint after the sprintf and
 * check the string variable output through Watch Window in AVR Studio 5.
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section files Main Files
 * - fir_example.c : DSPLIB FIR Filter example
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

//! The size of the input signal
#define SIZE  64
//! The number of tap
#define FIR_COEF_SIZE  25

//! The output buffer
A_ALIGNED dsp32_t y[SIZE - FIR_COEF_SIZE + 1];

//! Input signal resulting from a multiplication between a cosine and a sine.
A_ALIGNED dsp32_t x[SIZE] = {
	DSP32_Q(0.00000000000),
	DSP32_Q(0.05971733275),
	DSP32_Q(0.10139671590),
	DSP32_Q(0.11013997328),
	DSP32_Q(0.07684940150),
	DSP32_Q(0.00000000009),
	DSP32_Q(-0.11375674268),
	DSP32_Q(-0.25026678813),
	DSP32_Q(-0.38974690925),
	DSP32_Q(-0.50960156480),
	DSP32_Q(-0.58778525220),
	DSP32_Q(-0.60622623913),
	DSP32_Q(-0.55381024235),
	DSP32_Q(-0.42847700897),
	DSP32_Q(-0.23810168698),
	DSP32_Q(-0.00000000072),
	DSP32_Q(0.26091167702),
	DSP32_Q(0.51508014225),
	DSP32_Q(0.73202046178),
	DSP32_Q(0.88426998520),
	DSP32_Q(0.95105651622),
	DSP32_Q(0.92117732728),
	DSP32_Q(0.79468707991),
	DSP32_Q(0.58315039108),
	DSP32_Q(0.30840722126),
	DSP32_Q(0.00000000147),
	DSP32_Q(-0.30840721848),
	DSP32_Q(-0.58315038876),
	DSP32_Q(-0.79468707829),
	DSP32_Q(-0.92117732653),
	DSP32_Q(-0.95105651640),
	DSP32_Q(-0.88426998626),
	DSP32_Q(-0.73202046355),
	DSP32_Q(-0.51508014451),
	DSP32_Q(-0.26091167948),
	DSP32_Q(-0.00000000167),
	DSP32_Q(0.23810168494),
	DSP32_Q(0.42847700747),
	DSP32_Q(0.55381024151),
	DSP32_Q(0.60622623898),
	DSP32_Q(0.58778525267),
	DSP32_Q(0.50960156576),
	DSP32_Q(0.38974691050),
	DSP32_Q(0.25026678946),
	DSP32_Q(0.11375674388),
	DSP32_Q(0.00000000082),
	DSP32_Q(-0.07684940098),
	DSP32_Q(-0.11013997317),
	DSP32_Q(-0.10139671616),
	DSP32_Q(-0.05971733325),
	DSP32_Q(-0.00000000059),
	DSP32_Q(0.05971733225),
	DSP32_Q(0.10139671565),
	DSP32_Q(0.11013997338),
	DSP32_Q(0.07684940203),
	DSP32_Q(0.00000000100),
	DSP32_Q(-0.11375674148),
	DSP32_Q(-0.25026678680),
	DSP32_Q(-0.38974690800),
	DSP32_Q(-0.50960156384),
	DSP32_Q(-0.58778525172),
	DSP32_Q(-0.60622623927),
	DSP32_Q(-0.55381024319),
	DSP32_Q(-0.42847701047)
};

//! The impulse response coefficients of the filter
A_ALIGNED dsp32_t fir_coef[FIR_COEF_SIZE] = {
	DSP32_Q(-0.01559148806),
	DSP32_Q(-0.00894210585),
	DSP32_Q(0.00000000000),
	DSP32_Q(0.01092924048),
	DSP32_Q(0.02338723209),
	DSP32_Q(0.03678830106),
	DSP32_Q(0.05045511524),
	DSP32_Q(0.06366197724),
	DSP32_Q(0.07568267286),
	DSP32_Q(0.08583936913),
	DSP32_Q(0.09354892838),
	DSP32_Q(0.09836316431),
	DSP32_Q(0.10000000000),
	DSP32_Q(0.09836316431),
	DSP32_Q(0.09354892838),
	DSP32_Q(0.08583936913),
	DSP32_Q(0.07568267286),
	DSP32_Q(0.06366197724),
	DSP32_Q(0.05045511524),
	DSP32_Q(0.03678830106),
	DSP32_Q(0.02338723209),
	DSP32_Q(0.01092924048),
	DSP32_Q(0.00000000000),
	DSP32_Q(-0.00894210585),
	DSP32_Q(-0.01559148806)
};


//! The main function
int main(int argc, char *argv[])
{
	unsigned int cycle_count, i;
	char fir_vect_result[SIZE][15];

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

	// Perform a 25-taps FIR
	dsp32_filt_fir(y, x, SIZE, fir_coef, FIR_COEF_SIZE);

	// Calculate the number of cycles
	cycle_count = Get_sys_count() - cycle_count;

	// Print the  output signal
	for (i = 0; i < SIZE; i++) {
		dsp32_debug_sprintf(fir_vect_result[i],"%f", y[i]);
	}
	/*
	 * Place a breakpoint here and check the ASCII output in fir_vect_result
	 * in Memory Window.
	 * Note: Find the variable address in Watch Window and enter it in
	 * Memory Window
	 */

	while (1) {
		// Intentionally left blank.
	}
}

