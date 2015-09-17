/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit LMS filtering
 *
 * This file show a way to use the 32-bit LMS filter function and print the output signal
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
 * This example demonstrates a way to use the 32-bit LMS Filter function.\n
 * This is an adaptive filter, therefore the output of the function will be a data stream.
 * To change the signal frequency and to see the adaptation of the filter, press the push button
 * PB0 (on EVK1100) or PB2 (on EVK1101).\n
 * To visualize the result, you may use a terminal configured as described in the section <a href="#configinfo">Configuration Information</a>.\n
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for 32-bit AVR and for IAR Embedded Workbench
 * for Atmel 32-bit AVR. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All 32-bit AVR devices with a USART module can be used. This example has been tested
 * with the following setup:<BR>
 * - EVK1100 evaluation kit,
 * - EVK1101 evaluation kit..
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
#include "gpio.h"

//! \brief The size of the input signal
#define SIZE  64

//! \brief Buffer containing input data
A_ALIGNED dsp32_t input_x[SIZE] = {
    DSP32_Q(0.00000000000),
    DSP32_Q(0.03718075139),
    DSP32_Q(0.07131184859),
    DSP32_Q(0.09963983090),
    DSP32_Q(0.11997464035),
    DSP32_Q(0.13090169944),
    DSP32_Q(0.13191810690),
    DSP32_Q(0.12347962859),
    DSP32_Q(0.10695389264),
    DSP32_Q(0.08448437894),
    DSP32_Q(0.05877852523),
    DSP32_Q(0.03284069954),
    DSP32_Q(0.00967618536),
    DSP32_Q(-0.00800483670),
    DSP32_Q(-0.01805432735),
    DSP32_Q(-0.01909830056),
    DSP32_Q(-0.01067285908),
    DSP32_Q(0.00672896857),
    DSP32_Q(0.03170418002),
    DSP32_Q(0.06207594915),
    DSP32_Q(0.09510565163),
    DSP32_Q(0.12776001555),
    DSP32_Q(0.15700725030),
    DSP32_Q(0.18011316957),
    DSP32_Q(0.19490832447),
    DSP32_Q(0.20000000000),
    DSP32_Q(0.19490832447),
    DSP32_Q(0.18011316957),
    DSP32_Q(0.15700725030),
    DSP32_Q(0.12776001555),
    DSP32_Q(0.09510565163),
    DSP32_Q(0.06207594915),
    DSP32_Q(0.03170418002),
    DSP32_Q(0.00672896857),
    DSP32_Q(-0.01067285908),
    DSP32_Q(-0.01909830056),
    DSP32_Q(-0.01805432735),
    DSP32_Q(-0.00800483670),
    DSP32_Q(0.00967618536),
    DSP32_Q(0.03284069954),
    DSP32_Q(0.05877852523),
    DSP32_Q(0.08448437894),
    DSP32_Q(0.10695389264),
    DSP32_Q(0.12347962859),
    DSP32_Q(0.13191810690),
    DSP32_Q(0.13090169944),
    DSP32_Q(0.11997464035),
    DSP32_Q(0.09963983090),
    DSP32_Q(0.07131184859),
    DSP32_Q(0.03718075139),
    DSP32_Q(0.00000000000),
    DSP32_Q(-0.03718075139),
    DSP32_Q(-0.07131184859),
    DSP32_Q(-0.09963983090),
    DSP32_Q(-0.11997464035),
    DSP32_Q(-0.13090169944),
    DSP32_Q(-0.13191810690),
    DSP32_Q(-0.12347962859),
    DSP32_Q(-0.10695389264),
    DSP32_Q(-0.08448437894),
    DSP32_Q(-0.05877852523),
    DSP32_Q(-0.03284069954),
    DSP32_Q(-0.00967618536),
    DSP32_Q(0.00800483670)
};
//! \brief Buffer containing filtered input data
A_ALIGNED dsp32_t input_d[SIZE] = {
    DSP32_Q(0.0000),
    DSP32_Q(0.0005),
    DSP32_Q(0.0017),
    DSP32_Q(0.0045),
    DSP32_Q(0.0100),
    DSP32_Q(0.0191),
    DSP32_Q(0.0320),
    DSP32_Q(0.0480),
    DSP32_Q(0.0655),
    DSP32_Q(0.0823),
    DSP32_Q(0.0960),
    DSP32_Q(0.1043),
    DSP32_Q(0.1061),
    DSP32_Q(0.1014),
    DSP32_Q(0.0907),
    DSP32_Q(0.0756),
    DSP32_Q(0.0581),
    DSP32_Q(0.0403),
    DSP32_Q(0.0245),
    DSP32_Q(0.0127),
    DSP32_Q(0.0064),
    DSP32_Q(0.0066),
    DSP32_Q(0.0137),
    DSP32_Q(0.0272),
    DSP32_Q(0.0463),
    DSP32_Q(0.0692),
    DSP32_Q(0.0940),
    DSP32_Q(0.1183),
    DSP32_Q(0.1401),
    DSP32_Q(0.1573),
    DSP32_Q(0.1683),
    DSP32_Q(0.1721),
    DSP32_Q(0.1683),
    DSP32_Q(0.1573),
    DSP32_Q(0.1401),
    DSP32_Q(0.1183),
    DSP32_Q(0.0940),
    DSP32_Q(0.0692),
    DSP32_Q(0.0463),
    DSP32_Q(0.0272),
    DSP32_Q(0.0137),
    DSP32_Q(0.0066),
    DSP32_Q(0.0064),
    DSP32_Q(0.0127),
    DSP32_Q(0.0245),
    DSP32_Q(0.0403),
    DSP32_Q(0.0581),
    DSP32_Q(0.0756),
    DSP32_Q(0.0907),
    DSP32_Q(0.1014),
    DSP32_Q(0.1061),
    DSP32_Q(0.1039),
    DSP32_Q(0.0943),
    DSP32_Q(0.0778),
    DSP32_Q(0.0555),
    DSP32_Q(0.0289),
    DSP32_Q(0.0000),
    DSP32_Q(-0.0289),
    DSP32_Q(-0.0555),
    DSP32_Q(-0.0778),
    DSP32_Q(-0.0943),
    DSP32_Q(-0.1039),
    DSP32_Q(-0.1061),
    DSP32_Q(-0.1014)

};
#define FIR_COEF_SIZE   12
A_ALIGNED dsp32_t unknown_coef_filter[FIR_COEF_SIZE] = {
    DSP32_Q(0.0132),
    DSP32_Q(0.0254),
    DSP32_Q(0.0579),
    DSP32_Q(0.1006),
    DSP32_Q(0.1398),
    DSP32_Q(0.1632),
    DSP32_Q(0.1632),
    DSP32_Q(0.1398),
    DSP32_Q(0.1006),
    DSP32_Q(0.0579),
    DSP32_Q(0.0254),
    DSP32_Q(0.0132)
};
A_ALIGNED dsp32_t x[FIR_COEF_SIZE];

//! \brief coefficients of the filter
A_ALIGNED dsp32_t w[FIR_COEF_SIZE];


//! The main function
int main(int argc, char *argv[])
{
  dsp32_t sample_x,sample_d;
  dsp32_t y, e;
  int i;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);


  // Initialization of the buffers
  for(i=0; i<FIR_COEF_SIZE; i++)
  {
    w[i] = 0;
    x[i] = 0;
  }

  for(i=0; i<SIZE; i++)
  {
    // Compute a new sample
    sample_x = input_x[i];
    sample_d = input_d[i];
    // Compute the LMS filter
    dsp32_filt_lms(x, w, FIR_COEF_SIZE, sample_x, sample_d, &y, &e);

  }

  // Print the  output signal
  dsp32_debug_print_vect(&w[0], FIR_COEF_SIZE);

  while(1);
}


