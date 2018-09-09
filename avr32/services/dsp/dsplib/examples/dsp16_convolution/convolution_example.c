/*****************************************************************************
 *
 * \file
 *
 * \brief 16 bits Convolution
 *
 * This file show a way to use the 16 bits Convolution function and print the output signal
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
 * This example demonstrates how to use the 16-bit convolution function.
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
#define VECT2_SIZE  64
//! The number of tap
#define VECT3_SIZE  25

//! The output buffer
A_ALIGNED dsp16_t vect1[VECT2_SIZE + 2*(VECT3_SIZE - 1) + 4];  // <- "+4" for avr32-uc3 optimized FIR version

//! First input signal
A_ALIGNED dsp16_t vect2[VECT2_SIZE] = {
  DSP16_Q(0.000000),
  DSP16_Q(0.059717),
  DSP16_Q(0.101397),
  DSP16_Q(0.110140),
  DSP16_Q(0.076849),
  DSP16_Q(-0.000000),
  DSP16_Q(-0.113757),
  DSP16_Q(-0.250267),
  DSP16_Q(-0.389747),
  DSP16_Q(-0.509602),
  DSP16_Q(-0.587785),
  DSP16_Q(-0.606226),
  DSP16_Q(-0.553810),
  DSP16_Q(-0.428477),
  DSP16_Q(-0.238102),
  DSP16_Q(0.000000),
  DSP16_Q(0.260912),
  DSP16_Q(0.515080),
  DSP16_Q(0.732020),
  DSP16_Q(0.884270),
  DSP16_Q(0.951057),
  DSP16_Q(0.921177),
  DSP16_Q(0.794687),
  DSP16_Q(0.583150),
  DSP16_Q(0.308407),
  DSP16_Q(-0.000000),
  DSP16_Q(-0.308407),
  DSP16_Q(-0.583150),
  DSP16_Q(-0.794687),
  DSP16_Q(-0.921177),
  DSP16_Q(-0.951057),
  DSP16_Q(-0.884270),
  DSP16_Q(-0.732020),
  DSP16_Q(-0.515080),
  DSP16_Q(-0.260912),
  DSP16_Q(0.000000),
  DSP16_Q(0.238102),
  DSP16_Q(0.428477),
  DSP16_Q(0.553810),
  DSP16_Q(0.606226),
  DSP16_Q(0.587785),
  DSP16_Q(0.509602),
  DSP16_Q(0.389747),
  DSP16_Q(0.250267),
  DSP16_Q(0.113757),
  DSP16_Q(-0.000000),
  DSP16_Q(-0.076849),
  DSP16_Q(-0.110140),
  DSP16_Q(-0.101397),
  DSP16_Q(-0.059717),
  DSP16_Q(0.000000),
  DSP16_Q(0.059717),
  DSP16_Q(0.101397),
  DSP16_Q(0.110140),
  DSP16_Q(0.076849),
  DSP16_Q(-0.000000),
  DSP16_Q(-0.113757),
  DSP16_Q(-0.250267),
  DSP16_Q(-0.389747),
  DSP16_Q(-0.509602),
  DSP16_Q(-0.587785),
  DSP16_Q(-0.606226),
  DSP16_Q(-0.553810),
  DSP16_Q(-0.428477)
};

//! Second input signal
A_ALIGNED dsp16_t vect3[VECT3_SIZE] = {
  DSP16_Q(0.015591),
  DSP16_Q(0.008942),
  DSP16_Q(-0.),
  DSP16_Q(-0.010929),
  DSP16_Q(-0.023387),
  DSP16_Q(-0.036788),
  DSP16_Q(-0.050455),
  DSP16_Q(-0.063661),
  DSP16_Q(-0.075682),
  DSP16_Q(-0.085839),
  DSP16_Q(-0.093548),
  DSP16_Q(-0.098363),
  DSP16_Q(0.1),
  DSP16_Q(0.098363),
  DSP16_Q(0.093548),
  DSP16_Q(0.085839),
  DSP16_Q(0.075682),
  DSP16_Q(0.063661),
  DSP16_Q(0.050455),
  DSP16_Q(0.036788),
  DSP16_Q(0.023387),
  DSP16_Q(0.010929),
  DSP16_Q(0.),
  DSP16_Q(-0.008942),
  DSP16_Q(-0.015591)
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
  dsp16_vect_conv(vect1, vect2, VECT2_SIZE, vect3, VECT3_SIZE);

  // Calculate the number of cycles
  cycle_count = Get_sys_count() - cycle_count;

  // Print the number of cycles
  dsp16_debug_printf("Cycle count: %d\n", cycle_count);
  // Print the  output signal
  dsp16_debug_print_vect(vect1, VECT2_SIZE + VECT3_SIZE - 1);

  while(1);
}


