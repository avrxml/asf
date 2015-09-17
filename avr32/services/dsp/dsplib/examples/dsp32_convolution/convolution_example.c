/*****************************************************************************
 *
 * \file
 *
 * \brief 32 bits Convolution
 *
 * This file show a way to use the 32 bits Convolution function and print the output signal
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
 * This example demonstrates how to use the 32-bit convolution function.\n
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
#define VECT2_SIZE  64
//! The number of tap
#define VECT3_SIZE  25

//! The output buffer
A_ALIGNED dsp32_t vect1[VECT2_SIZE + 2*(VECT3_SIZE - 1) + 4];  // <- "+4" for avr32-uc3 optimized FIR version

//! First input signal
A_ALIGNED dsp32_t vect2[VECT2_SIZE] = {
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

//! Second input signal
A_ALIGNED dsp32_t vect3[VECT3_SIZE] = {
  DSP32_Q(0.01559148806),
  DSP32_Q(0.00894210585),
  DSP32_Q(-0.00000000000),
  DSP32_Q(-0.01092924048),
  DSP32_Q(-0.02338723209),
  DSP32_Q(-0.03678830106),
  DSP32_Q(-0.05045511524),
  DSP32_Q(-0.06366197724),
  DSP32_Q(-0.07568267286),
  DSP32_Q(-0.08583936913),
  DSP32_Q(-0.09354892838),
  DSP32_Q(-0.09836316431),
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
  unsigned int cycle_count;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  // Get the actual cycle count
  cycle_count = Get_sys_count();

  // Perform a convolution
  dsp32_vect_conv(vect1, vect2, VECT2_SIZE, vect3, VECT3_SIZE);

  // Calculate the number of cycles
  cycle_count = Get_sys_count() - cycle_count;

  // Print the number of cycles
  dsp32_debug_printf("Cycle count: %d\n", cycle_count);
  // Print the  output signal
  dsp32_debug_print_vect(vect1, VECT2_SIZE + VECT3_SIZE - 1);

  while(1);
}

