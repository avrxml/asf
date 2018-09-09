/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit Partial IIR filtering
 *
 * This file show a way to use the 32-bit partial IIR filter function and print the output signal
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
