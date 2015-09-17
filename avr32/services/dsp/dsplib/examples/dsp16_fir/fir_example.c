/*****************************************************************************
 *
 * \file
 *
 * \brief 16 bits FIR filtering
 *
 * This file show a way to use the 16 bits FIR filter function and print the output signal
 * through the USART.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the 16-bit FIR Filter function.
 * To visualize the result, you may use a terminal configured as described in the section <a href="#configinfo">Configuration Information</a>.\n
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for 32-bit AVR and for IAR Embedded Workbench
 * for Atmel 32-bit AVR. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All 32-bit AVR devices with a USART module can be used. This example has been tested
 * with the following setup:<BR>
 * - EVK1100 evaluation kit,
 * - EVK1101 evaluation kit.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - CPU clock: 12 MHz;
 * - USART1 connected to a PC serial port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 9600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"
#include "dsp_debug.h"

#include "pm.h"
#include "cycle_counter.h"

//! The size of the input signal
#define SIZE  64
//! The number of tap
#define FIR_COEF_SIZE  25

//! The output buffer
A_ALIGNED dsp16_t y[SIZE - FIR_COEF_SIZE + 1 + 4];  // <- "+4" for avr32-uc3 optimized FIR version

//! The input signal
A_ALIGNED dsp16_t x[SIZE] = {
  DSP16_Q(0.00000000000),
  DSP16_Q(0.05971733276),
  DSP16_Q(0.10139671591),
  DSP16_Q(0.11013997327),
  DSP16_Q(0.07684940146),
  DSP16_Q(0.00000000000),
  DSP16_Q(-0.11375674283),
  DSP16_Q(-0.25026678831),
  DSP16_Q(-0.38974690945),
  DSP16_Q(-0.50960156497),
  DSP16_Q(-0.58778525229),
  DSP16_Q(-0.60622623909),
  DSP16_Q(-0.55381024215),
  DSP16_Q(-0.42847700858),
  DSP16_Q(-0.23810168641),
  DSP16_Q(-0.00000000000),
  DSP16_Q(0.26091167781),
  DSP16_Q(0.51508014302),
  DSP16_Q(0.73202046242),
  DSP16_Q(0.88426998560),
  DSP16_Q(0.95105651630),
  DSP16_Q(0.92117732697),
  DSP16_Q(0.79468707920),
  DSP16_Q(0.58315039002),
  DSP16_Q(0.30840721992),
  DSP16_Q(0.00000000000),
  DSP16_Q(-0.30840721992),
  DSP16_Q(-0.58315039002),
  DSP16_Q(-0.79468707920),
  DSP16_Q(-0.92117732697),
  DSP16_Q(-0.95105651630),
  DSP16_Q(-0.88426998560),
  DSP16_Q(-0.73202046242),
  DSP16_Q(-0.51508014302),
  DSP16_Q(-0.26091167781),
  DSP16_Q(-0.00000000000),
  DSP16_Q(0.23810168641),
  DSP16_Q(0.42847700858),
  DSP16_Q(0.55381024215),
  DSP16_Q(0.60622623909),
  DSP16_Q(0.58778525229),
  DSP16_Q(0.50960156497),
  DSP16_Q(0.38974690945),
  DSP16_Q(0.25026678831),
  DSP16_Q(0.11375674283),
  DSP16_Q(0.00000000000),
  DSP16_Q(-0.07684940146),
  DSP16_Q(-0.11013997327),
  DSP16_Q(-0.10139671591),
  DSP16_Q(-0.05971733276),
  DSP16_Q(0.00000000000),
  DSP16_Q(0.05971733276),
  DSP16_Q(0.10139671591),
  DSP16_Q(0.11013997327),
  DSP16_Q(0.07684940146),
  DSP16_Q(0.00000000000),
  DSP16_Q(-0.11375674283),
  DSP16_Q(-0.25026678831),
  DSP16_Q(-0.38974690945),
  DSP16_Q(-0.50960156497),
  DSP16_Q(-0.58778525229),
  DSP16_Q(-0.60622623909),
  DSP16_Q(-0.55381024215),
  DSP16_Q(-0.42847700858)
};

// The impulse response coefficients of the filter
A_ALIGNED dsp16_t fir_coef[FIR_COEF_SIZE] = {
  DSP16_Q(-0.01559148806),
  DSP16_Q(-0.00894210585),
  DSP16_Q(0.00000000000),
  DSP16_Q(0.01092924048),
  DSP16_Q(0.02338723209),
  DSP16_Q(0.03678830106),
  DSP16_Q(0.05045511524),
  DSP16_Q(0.06366197724),
  DSP16_Q(0.07568267286),
  DSP16_Q(0.08583936913),
  DSP16_Q(0.09354892838),
  DSP16_Q(0.09836316431),
  DSP16_Q(0.10000000000),
  DSP16_Q(0.09836316431),
  DSP16_Q(0.09354892838),
  DSP16_Q(0.08583936913),
  DSP16_Q(0.07568267286),
  DSP16_Q(0.06366197724),
  DSP16_Q(0.05045511524),
  DSP16_Q(0.03678830106),
  DSP16_Q(0.02338723209),
  DSP16_Q(0.01092924048),
  DSP16_Q(0.00000000000),
  DSP16_Q(-0.00894210585),
  DSP16_Q(-0.01559148806)
};


//! The main function
int main(int argc, char *argv[])
{
  unsigned int cycle_count;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  // Get the actual cycle count
  cycle_count = Get_sys_count();

  // Perform a 25-taps FIR
  dsp16_filt_fir(y, x, SIZE, fir_coef, FIR_COEF_SIZE);

  // Calculate the number of cycles
  cycle_count = Get_sys_count() - cycle_count;

  // Print the number of cycles
  dsp16_debug_printf("Cycle count: %d\n", cycle_count);
  // Print the  output signal
  dsp16_debug_print_vect(y, SIZE - FIR_COEF_SIZE + 1);

  while(1);
}

