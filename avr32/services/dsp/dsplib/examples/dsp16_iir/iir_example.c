/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit IIR filtering
 *
 * This file show a way to use the 16 bits IIR filter function and print the output signal
 * through the USART.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the 16-bit IIR Filter function.\n
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

A_ALIGNED dsp16_t num[NUM_SIZE] = {
  DSP16_Q(0.6537018  / (1 << NUM_PREDIV)),
  DSP16_Q(-3.2685088 / (1 << NUM_PREDIV)),
  DSP16_Q(6.5370176  / (1 << NUM_PREDIV)),
  DSP16_Q(-6.5370176 / (1 << NUM_PREDIV)),
  DSP16_Q(3.2685088  / (1 << NUM_PREDIV)),
  DSP16_Q(-0.6537018 / (1 << NUM_PREDIV))
};

A_ALIGNED dsp16_t den[DEN_SIZE] = {
  DSP16_Q(-4.1534907 / (1 << DEN_PREDIV)),
  DSP16_Q(6.9612765  / (1 << DEN_PREDIV)),
  DSP16_Q(-5.877997  / (1 << DEN_PREDIV)),
  DSP16_Q(2.498365   / (1 << DEN_PREDIV)),
  DSP16_Q(-0.427326  / (1 << DEN_PREDIV))
};

//! The output buffer
A_ALIGNED dsp16_t y[SIZE + DEN_SIZE] = {
  DSP16_Q(0.)
};

//! The input signal (sampling rate: 48KHz)
//! A mix between a 4000 Hz and a 1000 Hz signal
//! (mixed by addition)
A_ALIGNED dsp16_t x[SIZE + NUM_SIZE - 1] = {
    // ----Previous Data---
    DSP16_Q(-0.00363360317),
    DSP16_Q(0.01830127019),
    DSP16_Q(0.04619397663),
    DSP16_Q(0.07329629131),
    DSP16_Q(0.09287351326),
    // --------------------
    DSP16_Q(0.10000000000),
    DSP16_Q(0.09287351326),
    DSP16_Q(0.07329629131),
    DSP16_Q(0.04619397663),
    DSP16_Q(0.01830127019),
    DSP16_Q(-0.00363360317),
    DSP16_Q(-0.01464466094),
    DSP16_Q(-0.01286319874),
    DSP16_Q(-0.00000000000),
    DSP16_Q(0.01913417162),
    DSP16_Q(0.03794095226),
    DSP16_Q(0.04982757980),
    DSP16_Q(0.05000000000),
    DSP16_Q(0.03677496058),
    DSP16_Q(0.01205904774),
    DSP16_Q(-0.01913417162),
    DSP16_Q(-0.05000000000),
    DSP16_Q(-0.07373934164),
    DSP16_Q(-0.08535533906),
    DSP16_Q(-0.08296893720),
    DSP16_Q(-0.06830127019),
    DSP16_Q(-0.04619397663),
    DSP16_Q(-0.02329629131),
    DSP16_Q(-0.00627097288),
    DSP16_Q(0.00000000000),
    DSP16_Q(-0.00627097288),
    DSP16_Q(-0.02329629131),
    DSP16_Q(-0.04619397663),
    DSP16_Q(-0.06830127019),
    DSP16_Q(-0.08296893720),
    DSP16_Q(-0.08535533906),
    DSP16_Q(-0.07373934164),
    DSP16_Q(-0.05000000000),
    DSP16_Q(-0.01913417162),
    DSP16_Q(0.01205904774),
    DSP16_Q(0.03677496058),
    DSP16_Q(0.05000000000),
    DSP16_Q(0.04982757980),
    DSP16_Q(0.03794095226),
    DSP16_Q(0.01913417162),
    DSP16_Q(0.00000000000),
    DSP16_Q(-0.01286319874),
    DSP16_Q(-0.01464466094),
    DSP16_Q(-0.00363360317),
    DSP16_Q(0.01830127019),
    DSP16_Q(0.04619397663),
    DSP16_Q(0.07329629131),
    DSP16_Q(0.09287351326)
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
  for(i=0; i<1000; i++)
  {
    // Perform a IIR filter
    dsp16_filt_iir(&y[DEN_SIZE], &x[NUM_SIZE-1], SIZE, num, NUM_SIZE, den, DEN_SIZE, NUM_PREDIV, DEN_PREDIV);
    // Update the output signal
    memmove(y, &y[SIZE], (DEN_SIZE)*sizeof(dsp16_t));

  }

  // Print the  output signal
  dsp16_debug_print_vect(&y[DEN_SIZE], SIZE);

  while(1);
}
