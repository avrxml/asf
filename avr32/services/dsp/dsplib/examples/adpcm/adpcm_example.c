/*****************************************************************************
 *
 * \file
 *
 * \brief IMA/DVI ADPCM functions usage
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
 * This example demonstrates how to use IMA/DVI ADPCM coding and decoding functions.\n
 * To visualize the result, you may use a terminal configured as described in the section \ref configinfo.\n
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


#define NSAMPLES 100

short sbuf[NSAMPLES] = {
    0,
    10,
    37,
    89,
    158,
    345,
    869,
    1920,
    3726,
    7405,
    10987,
    14427,
    17679,
    20703,
    23457,
    25907,
    28021,
    29771,
    31135,
    32094,
    32638,
    32757,
    32452,
    31725,
    30587,
    29052,
    27139,
    24875,
    22287,
    19411,
    16282,
    12943,
    9435,
    5805,
    2099,
  - 1633,
  - 5344,
  - 8987,
  - 12512,
  - 15876,
  - 19033,
  - 21943,
  - 24568,
  - 26875,
  - 28833,
  - 30416,
  - 31605,
  - 32384,
  - 32742,
  - 32676,
  - 32185,
  - 31277,
  - 29963,
  - 28260,
  - 26190,
  - 23781,
  - 21062,
  - 18071,
  - 14845,
  - 11426,
  - 7859,
  - 4190,
  - 466,
    3262,
    6949,
    10546,
    14006,
    17285,
    20339,
    23129,
    25619,
    27776,
    29573,
    30986,
    31997,
    32593,
    32766,
    32513,
    31839,
    30751,
    29265,
    27398,
    25176,
    22627,
    19785,
    16686,
    13370,
    9881,
    6263,
    2565,
  - 1166,
  - 4883,
  - 8537,
  - 12080,
  - 15466,
  - 18651,
  - 21594,
  - 24257,
  - 26605,
  - 28608
};
char cbuf[NSAMPLES];
short new_sbuf[NSAMPLES];

//! The main function
int main(int argc, char *argv[])
{
  unsigned int cycle_count;
  int i;
  short predicted_value, step_index;
  int diff;
  dsp16_t ratio;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  // Get the actual cycle count
  cycle_count = Get_sys_count();

  predicted_value = 0;
  step_index = 0;
  // Encode
  dsp_adpcm_ima_encode(cbuf, sbuf, NSAMPLES, &step_index, &predicted_value);

  predicted_value = 0;
  step_index = 0;
  // Decode
  dsp_adpcm_ima_decode(new_sbuf, cbuf, NSAMPLES, &step_index, &predicted_value);

  // Calculate the number of cycles
  cycle_count = Get_sys_count() - cycle_count;

  // Error calculation
  diff = 0;
  for(i=0; i<NSAMPLES; i++)
    diff += ((new_sbuf[i]-sbuf[i]) > 0)?(new_sbuf[i]-sbuf[i]):(sbuf[i]-new_sbuf[i]);
  ratio = DSP16_Q((diff/NSAMPLES)/(((float) (1 << 15))));

  // Print the number of cycles
  dsp16_debug_printf("Cycle count: %d\n", cycle_count);
  // Print the error average
  dsp16_debug_printf("Error average ratio: %f\n", ratio);

  while(1);
}

