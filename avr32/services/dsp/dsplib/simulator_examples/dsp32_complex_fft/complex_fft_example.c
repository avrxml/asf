/**
 * \file
 *
 * \brief 32 bits complex FFT program test
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
