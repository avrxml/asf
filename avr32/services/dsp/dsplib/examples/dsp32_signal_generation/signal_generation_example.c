/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bits signal generation program test
 *
 * This file show a way to use the 32 bits signal generation functions and print the output
 * results through the USART.
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
 * This example demonstrates how to use the 32-bit fixed point signal generation functions.\n
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

#define SIGNAL_SIN     1
#define SIGNAL_COS     2
#define SIGNAL_NOISE   3

#define SIGNAL_TO_USE  SIGNAL_SIN

// Size in elements of the signal to generate
#define SIZE  100

//! The main function
int main(int argc, char *argv[])
{
  dsp32_t vect1[SIZE];
  int cycle_count;
  int frequency, sample_rate;
  dsp32_t phase, amplitude;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  // 1 KHz
  frequency = 1000;
  // 10 KHz
  sample_rate = 100000;
  // phase = PI/2
  phase = DSP32_Q(0.5);
  // amplitude
  amplitude = DSP32_Q(1.);

  dsp32_debug_printf("32-bit signal generation program test\n");

  // Compute the signal to generate
  switch(SIGNAL_TO_USE)
  {
  // Sinusoidal
  case SIGNAL_SIN:
    dsp32_debug_printf("Sine\n");
    dsp32_debug_printf("Frequency: %d, Fs: %d, Phase: %f\n", frequency, sample_rate, phase);
    cycle_count = Get_sys_count();
    dsp32_gen_sin(vect1, SIZE, frequency, sample_rate, phase);
    cycle_count = Get_sys_count() - cycle_count;
    break;
  // Cosinusoidal
  case SIGNAL_COS:
    dsp32_debug_printf("Cosine\n");
    dsp32_debug_printf("Frequency: %d, Fs: %d, Phase: %f\n", frequency, sample_rate, phase);
    cycle_count = Get_sys_count();
    dsp32_gen_cos(vect1, SIZE, frequency, sample_rate, phase);
    cycle_count = Get_sys_count() - cycle_count;
    break;
  // Noise
  case SIGNAL_NOISE:
    dsp32_debug_printf("Noise\n");
    dsp32_debug_printf("Amplitude: %d\n", amplitude);
    cycle_count = Get_sys_count();
    dsp32_gen_noise(vect1, SIZE, amplitude);
    cycle_count = Get_sys_count() - cycle_count;
    break;
  }

  // Print the number of cycles
  dsp32_debug_printf("Cycle count: %d\n", cycle_count);
  // Print the signal
  dsp32_debug_print_vect(vect1, SIZE);

  while(1);
}


