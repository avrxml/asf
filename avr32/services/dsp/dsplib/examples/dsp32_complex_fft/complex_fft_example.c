/*****************************************************************************
 *
 * \file
 *
 * \brief 32 bits complex FFT program test
 *
 * This file show a way to use the 32 bits complex FFT function and print the output signal
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
 * This example demonstrates how to use the 32-bit complex FFT function.\n
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


//! The size of the signal
#define SIZE  64
//! log2(SIZE)
#define NLOG  6

//! The output buffer
A_ALIGNED dsp32_complex_t vect1[SIZE];
A_ALIGNED dsp32_t         fft_real[SIZE];
//! The input signal resulting from a multiplication between a cosine and a sine.
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
  DSP32_Q(-0.8842699856),
  DSP32_Q(-0.7320204624),
  DSP32_Q(-0.5150801430),
  DSP32_Q(-0.2609116778),
  DSP32_Q(-0.0000000000),
  DSP32_Q(0.2381016864),
  DSP32_Q(0.4284770086),
  DSP32_Q(0.5538102421),
  DSP32_Q(0.6062262391),
  DSP32_Q(0.5877852523),
  DSP32_Q(0.5096015650),
  DSP32_Q(0.3897469095),
  DSP32_Q(0.2502667883),
  DSP32_Q(0.1137567428),
  DSP32_Q(0.0000000000),
  DSP32_Q(-0.0768494015),
  DSP32_Q(-0.1101399733),
  DSP32_Q(-0.1013967159),
  DSP32_Q(-0.0597173328),
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
  DSP32_Q(-0.4284770086)
    };

//! The main function
int main(int argc, char *argv[])
{
  unsigned int cycle_count;
  volatile  dsp32_t fft32_max;

    // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

    // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

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

    // Print the number of cycles
  dsp32_debug_printf("Cycle count: %d\n", cycle_count);
    // Print the complex FFT output signal
  dsp32_debug_print_complex_vect(vect1, SIZE);

  while(1);
}
