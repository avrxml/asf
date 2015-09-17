/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit Partial IIR filtering
 *
 * This file show a way to use the 16-bit partial IIR filter function and print the output signal
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
 * This example demonstrates how to use the 16-bit partial IIR Filter function.\n
 * To visualize the result, you may use a terminal configured as described in the section <a href="#configinfo">Configuration Information</a>.\n
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for 32-bit AVRs and for IAR Embedded Workbench
 * for Atmel 32-bit AVRs. Other compilers may or may not work.
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
#define SIZE  72

//! The output buffer
A_ALIGNED dsp16_t vect1[SIZE];

//! The input signal
A_ALIGNED dsp16_t vect2[SIZE] = {
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.19510565163),
      DSP16_Q(0.00000000000),
      DSP16_Q(0.19510565163),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.13090169944),
      DSP16_Q(0.04122147477),
      DSP16_Q(-0.18090169944),
      DSP16_Q(0.00489434837),
      DSP16_Q(-0.20000000000),
      DSP16_Q(0.00489434837),
      DSP16_Q(-0.18090169944),
      DSP16_Q(0.04122147477),
      DSP16_Q(-0.13090169944),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.19510565163),
      DSP16_Q(0.00000000000),
      DSP16_Q(0.19510565163),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.13090169944),
      DSP16_Q(0.04122147477),
      DSP16_Q(-0.18090169944),
      DSP16_Q(0.00489434837),
      DSP16_Q(-0.20000000000),
      DSP16_Q(0.00489434837),
      DSP16_Q(-0.18090169944),
      DSP16_Q(0.04122147477),
      DSP16_Q(-0.13090169944),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.19510565163),
      DSP16_Q(0.00000000000),
      DSP16_Q(0.19510565163),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.13090169944),
      DSP16_Q(0.04122147477),
      DSP16_Q(-0.18090169944),
      DSP16_Q(0.00489434837),
      DSP16_Q(-0.20000000000),
      DSP16_Q(0.00489434837),
      DSP16_Q(-0.18090169944),
      DSP16_Q(0.04122147477),
      DSP16_Q(-0.13090169944),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.19510565163),
      DSP16_Q(0.00000000000),
      DSP16_Q(0.19510565163),
      DSP16_Q(-0.01909830056),
      DSP16_Q(0.15877852523),
      DSP16_Q(-0.06909830056),
      DSP16_Q(0.10000000000),
      DSP16_Q(-0.13090169944)
};

// A Low-pass IIR Filter
// Butterworth model
// Fsample = 8 KHz
// Fcut-off = 2000 Hz
// Order 4

#define NUM_SIZE  5
#define DEN_SIZE  4

A_ALIGNED dsp16_t num[NUM_SIZE] = {
  DSP16_Q(0.0939809),
  DSP16_Q(0.375923),
  DSP16_Q(0.5638851),
  DSP16_Q(0.3759234),
  DSP16_Q(0.0939809)
};

A_ALIGNED dsp16_t den[DEN_SIZE] = {
  DSP16_Q(-0.00000000000000003469),
  DSP16_Q(0.4860288),
  DSP16_Q(-0.00000000000000001549),
  DSP16_Q(0.0176648)
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
  dsp16_filt_iirpart(vect1, vect2, SIZE, num, NUM_SIZE, den, DEN_SIZE, 0, 0);

  // Calculate the number of cycles
  cycle_count = Get_sys_count() - cycle_count;

  // Print the number of cycles
  dsp16_debug_printf("Cycle count: %d\n", cycle_count);

  // Print the  output signal
  dsp16_debug_print_vect(vect1, SIZE - NUM_SIZE + 1);

  while(1);
}
