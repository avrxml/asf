/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit operators program test
 *
 * This file show a way to use the 16-bit fixed point operators functions and print the output
 * results through the USART.
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
 * This example demonstrates how to use the 16-bit fixed point operators functions.
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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"
#include "dsp_debug.h"

#include "pm.h"
#include "cycle_counter.h"

//! The main function
int main(int argc, char *argv[])
{
  dsp16_t number, result;
  int cycle_count;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  dsp16_debug_printf("16-bit fixed point operators program test\n");

  dsp16_debug_printf("Type a number: ");
  number = dsp_debug_read_q(DSP16_QA, DSP16_QB);

  dsp16_debug_printf("Number to compute: %f\n", number);

  // 16-bit fixed point cosine
  cycle_count = Get_sys_count();
  result = dsp16_op_cos(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("cos(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point sine
  cycle_count = Get_sys_count();
  result = dsp16_op_sin(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("sin(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point arc cosine
  cycle_count = Get_sys_count();
  result = dsp16_op_acos(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("acos(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point arc sine
  cycle_count = Get_sys_count();
  result = dsp16_op_asin(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("asin(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point random
  dsp_op_srand(number);
  cycle_count = Get_sys_count();
  result = dsp16_op_rand();
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("rand()\t\t\t%f\t(%i cycles)\n", result, cycle_count);

  // 16-bit fixed point square root
  cycle_count = Get_sys_count();
  result = dsp16_op_sqrt(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("sqrt(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point absolute
  cycle_count = Get_sys_count();
  result = dsp16_op_abs(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("abs(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point natural logarithm
  cycle_count = Get_sys_count();
  result = dsp16_op_ln(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("ln(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point binary logarithm
  cycle_count = Get_sys_count();
  result = dsp16_op_log2(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("log2(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point common logarithm
  cycle_count = Get_sys_count();
  result = dsp16_op_log10(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("log10(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point exponential
  cycle_count = Get_sys_count();
  result = dsp16_op_exp(number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("exp(%f)\t\t%f\t(%i cycles)\n", number, result, cycle_count);

  // 16-bit fixed point power
  cycle_count = Get_sys_count();
  result = dsp16_op_pow(DSP16_Q(0.5), number);
  cycle_count = Get_sys_count() - cycle_count;
  dsp16_debug_printf("pow(0.5, %f)\t%f\t(%i cycles)\n", number, result, cycle_count);

  while(1);
}
