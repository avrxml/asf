/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit IIR filtering
 *
 * This file show a way to use the 32 bits IIR filter function and print the output signal
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
 * This example demonstrates how to use the 32-bit IIR Filter function.\n
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

//! The size of the input signal
#define SIZE  48

// A High-pass IIR Filter
// Butterworth model
// Fsample = 48 KHz
// Fcut-off = 2000 Hz
// Order 5

#define NUM_SIZE  6
#define NUM_PREDIV  3
#define DEN_SIZE  5
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

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  // Number of recursions
  for(i=0; i<100000; i++)
  {
    // Perform a IIR filter
    dsp32_filt_iir(&y[DEN_SIZE], &x[NUM_SIZE-1], SIZE, num, NUM_SIZE, den, DEN_SIZE, NUM_PREDIV, DEN_PREDIV);
    // Update the output signal
    memmove(y, &y[SIZE], (DEN_SIZE)*sizeof(dsp32_t));

  }
  // Print the  output signal
  dsp32_debug_print_vect(&y[DEN_SIZE], SIZE);

  while(1);
}
