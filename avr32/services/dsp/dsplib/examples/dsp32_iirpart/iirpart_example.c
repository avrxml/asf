/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit Partial IIR filtering
 *
 * This file show a way to use the 32-bit partial IIR filter function and print the output signal
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
 * This example demonstrates how to use the 32-bit partial IIR Filter function.\n
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
#define SIZE  144

// A Low-pass IIR Filter
// Butterworth model
// Fsample = 8 KHz
// Fcut-off = 2000 Hz
// Order 18

#define NUM_SIZE  19
#define DEN_SIZE  18

#define PREDIV  2


A_ALIGNED dsp32_t num[NUM_SIZE] = {
  DSP32_Q(0.0000274/(1 << PREDIV)),
  DSP32_Q(0.0004939/(1 << PREDIV)),
  DSP32_Q(0.0041985/(1 << PREDIV)),
  DSP32_Q(0.0223922/(1 << PREDIV)),
  DSP32_Q(0.0839706/(1 << PREDIV)),
  DSP32_Q(0.2351178/(1 << PREDIV)),
  DSP32_Q(0.5094219/(1 << PREDIV)),
  DSP32_Q(0.8732946/(1 << PREDIV)),
  DSP32_Q(1.2007801/(1 << PREDIV)),
  DSP32_Q(1.3342001/(1 << PREDIV)),
  DSP32_Q(1.2007801/(1 << PREDIV)),
  DSP32_Q(0.8732946/(1 << PREDIV)),
  DSP32_Q(0.5094219/(1 << PREDIV)),
  DSP32_Q(0.2351178/(1 << PREDIV)),
  DSP32_Q(0.0839706/(1 << PREDIV)),
  DSP32_Q(0.0223922/(1 << PREDIV)),
  DSP32_Q(0.0041985/(1 << PREDIV)),
  DSP32_Q(0.0004939/(1 << PREDIV)),
  DSP32_Q(0.0000274/(1 << PREDIV))
};

A_ALIGNED dsp32_t den[DEN_SIZE] = {
  DSP32_Q(0.0000000000000001427/(1 << PREDIV)),
  DSP32_Q(2.4446755/(1 << PREDIV)),
  DSP32_Q(0.0000000000000002630/(1 << PREDIV)),
  DSP32_Q(2.3222667/(1 << PREDIV)),
  DSP32_Q(0.0000000000000002022/(1 << PREDIV)),
  DSP32_Q(1.1033779/(1 << PREDIV)),
  DSP32_Q(0.0000000000000000792/(1 << PREDIV)),
  DSP32_Q(0.2817442/(1 << PREDIV)),
  DSP32_Q(0.00000000000000001608/(1 << PREDIV)),
  DSP32_Q(0.0387225/(1 << PREDIV)),
  DSP32_Q(0.000000000000000001608/(1 << PREDIV)),
  DSP32_Q(0.0027201/(1 << PREDIV)),
  DSP32_Q(0.00000000000000000007254/(1 << PREDIV)),
  DSP32_Q(0.00008640036584533501/(1 << PREDIV)),
  DSP32_Q(0.000000000000000000001232/(1 << PREDIV)),
  DSP32_Q(0.0000009/(1 << PREDIV)),
  DSP32_Q(-0.00000000000000000000/(1 << PREDIV)),
  DSP32_Q(0.000000001506/(1 << PREDIV))
};

//! The output buffer
A_ALIGNED dsp32_t y[SIZE + DEN_SIZE];

//! The input signal
A_ALIGNED dsp32_t x[SIZE + NUM_SIZE] = {
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01951056516),
    DSP32_Q(0.00000000000),
    DSP32_Q(0.01951056516),
    DSP32_Q(-0.00190983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.02000000000),
    DSP32_Q(0.00048943484),
    DSP32_Q(-0.01809016994),
    DSP32_Q(0.00412214748),
    DSP32_Q(-0.01309016994),
    DSP32_Q(0.01000000000),
    DSP32_Q(-0.00690983006),
    DSP32_Q(0.01587785252),
    DSP32_Q(-0.00190983006)
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
  dsp32_filt_iirpart(y, x, SIZE, num, NUM_SIZE, den, DEN_SIZE, PREDIV, PREDIV);

  // Calculate the number of cycles
  cycle_count = Get_sys_count() - cycle_count;

  // Print the number of cycles
  dsp32_debug_printf("Cycle count: %d\n", cycle_count);

  // Print the  output signal
  dsp32_debug_print_vect(y, SIZE - NUM_SIZE + 1);

  while(1);
}
