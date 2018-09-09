/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit Partial IIR filtering
 *
 * This file show a way to use the 16-bit partial IIR filter function and print the output signal
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
