/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit vectors program test
 *
 * This file show a way to use the 32-bit fixed point operators functions and print the output
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
 * This example demonstrates how to use the 32-bit fixed point vectors functions.\n
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

// Length of the output signal
#define VECT1_SIZE  256
// Length of the first input signal
#define VECT2_SIZE  256
// Length of the second input signal
#define VECT3_SIZE  256

// The output signal
A_ALIGNED dsp32_t vect1[VECT1_SIZE];
// The first input signal
A_ALIGNED dsp32_t vect2[VECT2_SIZE] = {
  DSP32_Q(0.012010),
  DSP32_Q(0.059717),
  DSP32_Q(0.101397),
  DSP32_Q(0.0583150),
  DSP32_Q(0.0000000),
  DSP32_Q(0.0921177),
  DSP32_Q(0.0951057),
  DSP32_Q(0.0884270),
  DSP32_Q(0.0732020),
  DSP32_Q(0.515080),
  DSP32_Q(0.0583150)
};
// The second input signal
A_ALIGNED dsp32_t vect3[VECT3_SIZE] = {
  DSP32_Q(0.5),
  DSP32_Q(0.101397),
  DSP32_Q(0.110140),
  DSP32_Q(0.076849),
  DSP32_Q(-0.794687),
  DSP32_Q(-0.113757),
  DSP32_Q(-0.250267),
  DSP32_Q(-0.389747),
  DSP32_Q(-0.509602),
  DSP32_Q(-0.587785),
  DSP32_Q(-0.113757)
};

#define CHECK_CONDITIONS_ALL(x_cond) \
  if (!(x_cond)) \
  { \
    dsp32_debug_printf("The condition '" #x_cond "' is not verified. (line %i)\n", __LINE__); \
    return -1; \
  }

#if __GNUC__
#define CHECK_CONDITIONS(x_cond) \
{ \
  CHECK_CONDITIONS_ALL(x_cond) \
}
#elif __ICCAVR32__
#define CHECK_CONDITIONS(x_cond) \
{ \
  _Pragma("diag_suppress=Pe111") \
  CHECK_CONDITIONS_ALL(x_cond) \
}
#endif

// Type to point on the function for the menu
typedef int (*fct_item_menu)(int *);

// Type to define the items of the menu
typedef struct
{
  char *title;
  fct_item_menu action;
}s_item_menu;

// Functions implemented in this example
int addition(int *size);
int subtraction(int *size);
int real_multiplication(int *size);
int real_division(int *size);
int dot_multiplication(int *size);
int dot_division(int *size);
int minimum(int *size);
int maximum(int *size);
int negate(int *size);
int convolution(int *size);
int partial_convolution(int *size);
int copy(int *size);
int zero_padding(int *size);
int power(int *size);

// To define the menu
s_item_menu item_menu[] = {
  { "Addition", addition },
  { "Subtraction", subtraction },
  { "Multiplication with real", real_multiplication },
  { "Division with real", real_division },
  { "Dot multiplication", dot_multiplication },
  { "Dot division", dot_division },
  { "Power", power },
  { "Minimum", minimum },
  { "Maximum", maximum },
  { "Negate", negate },
  { "Copy", copy },
  { "Zero padding", zero_padding },
  { "Convolution", convolution },
  { "Partial convolution", partial_convolution }
};

/****************************************************/

int addition(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

  // Action
  dsp32_debug_printf("vect1 = vect2 + vect3\n");

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_add(vect1, vect2, vect3, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int subtraction(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

  // Action
  dsp32_debug_printf("vect1 = vect2 - vect3\n");

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_sub(vect1, vect2, vect3, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int real_multiplication(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT3_SIZE > 0)

  // Action
  dsp32_debug_printf("vect1 = vect2 * %f\n", vect3[0]);

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_realmul(vect1, vect2, VECT2_SIZE, vect3[0]);

  return Get_sys_count() - cycle_count;
}

int real_division(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT3_SIZE > 0)

  // Action
  dsp32_debug_printf("vect1 = vect2 / %f\n", vect3[0]);

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_realdiv(vect1, vect2, VECT2_SIZE, vect3[0]);

  return Get_sys_count() - cycle_count;
}

int dot_multiplication(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

  // Action
  dsp32_debug_printf("vect1 = vect2 .* vect3\n");

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_dotmul(vect1, vect2, vect3, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int dot_division(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

  // Action
  dsp32_debug_printf("vect1 = vect2 ./ vect3\n");

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_dotdiv(vect1, vect2, vect3, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int power(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
  CHECK_CONDITIONS(VECT3_SIZE > 0)

  // Action
  dsp32_debug_printf("vect1 = vect2 ^ %f\n", vect3[0]);

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_pow(vect1, vect2, VECT2_SIZE, vect3[0]);

  return Get_sys_count() - cycle_count;
}

int minimum(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE > 0)
  CHECK_CONDITIONS(VECT2_SIZE > 0)

  // Action
  dsp32_debug_printf("min(vect2)\n");

  *size = 1;

  cycle_count = Get_sys_count();
  vect1[0] = dsp32_vect_min(vect2, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int maximum(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE > 0)
  CHECK_CONDITIONS(VECT2_SIZE > 0)

  // Action
  dsp32_debug_printf("max(vect2)\n");

  *size = 1;

  cycle_count = Get_sys_count();
  vect1[0] = dsp32_vect_max(vect2, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int negate(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)

  // Action
  dsp32_debug_printf("vect1 = neg(vect2)\n");

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_neg(vect1, vect2, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int copy(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)

  // Action
  dsp32_debug_printf("vect1 = vect2\n");

  *size = VECT2_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_copy(vect1, vect2, VECT2_SIZE);

  return Get_sys_count() - cycle_count;
}

int zero_padding(int *size)
{
  int cycle_count;

  // Action
  dsp32_debug_printf("vect1 = zeros(vect1)\n");

  *size = VECT1_SIZE;

  cycle_count = Get_sys_count();
  dsp32_vect_zeropad(vect1, VECT1_SIZE, VECT1_SIZE);

  return Get_sys_count() - cycle_count;
}

int convolution(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= (VECT2_SIZE + VECT3_SIZE - 1))
  CHECK_CONDITIONS(VECT2_SIZE >= 8)
  CHECK_CONDITIONS(VECT3_SIZE >= 8)
  if (VECT2_SIZE > VECT3_SIZE)
    CHECK_CONDITIONS(VECT1_SIZE >= (VECT2_SIZE + 2*VECT3_SIZE - 2))
  else
    CHECK_CONDITIONS(VECT1_SIZE >= (VECT3_SIZE + 2*VECT2_SIZE - 2))

  // Action
  dsp32_debug_printf("vect1 = conv(vect2, vect3)\n");

  *size = VECT2_SIZE + VECT3_SIZE - 1;

  cycle_count = Get_sys_count();
  // Perform a convolution
  dsp32_vect_conv(vect1, vect2, VECT2_SIZE, vect3, VECT3_SIZE);
  return Get_sys_count() - cycle_count;
}

int partial_convolution(int *size)
{
  int cycle_count;

  // Conditions
  CHECK_CONDITIONS(VECT1_SIZE >= (VECT2_SIZE - VECT3_SIZE + 4))
  CHECK_CONDITIONS(!(VECT2_SIZE&3) && VECT2_SIZE > 0)
  CHECK_CONDITIONS(VECT3_SIZE >= 8)

  // Action
  dsp32_debug_printf("vect1 = partial_conv(vect2, vect3)\n");

  *size = VECT2_SIZE - VECT3_SIZE + 1;

  cycle_count = Get_sys_count();
  // Perform a partial convolution
  dsp32_vect_convpart(vect1, vect2, VECT2_SIZE, vect3, VECT3_SIZE);
  return Get_sys_count() - cycle_count;
}


/****************************************************/




//! The main function
int main(int argc, char *argv[])
{
  int cycle_count, size;
  int i;

  // Switch to external Oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the DSP debug module
  dsp_debug_initialization(FOSC0);

  dsp32_debug_printf("32-bit fixed point vectors program test\n");

  dsp32_debug_printf("Output vector 1 (size %i)\n", VECT1_SIZE);
  dsp32_debug_printf("Input vector 2 (size %i)\n", VECT2_SIZE);
  dsp32_debug_printf("Input vector 3 (size %i)\n", VECT3_SIZE);

  while(1)
  {
    // Print the menu
    dsp32_debug_printf("***** Menu *****\n");
    for(i=0; i<sizeof(item_menu)/sizeof(s_item_menu); i++)
      dsp32_debug_printf("%i:\t%s\n", i, item_menu[i].title);

    // Prompt
    dsp32_debug_printf("> ");
    i = dsp_debug_read_ui();

    if (i >= 0 && i < sizeof(item_menu)/sizeof(s_item_menu))
    {
      // Print the title
      dsp32_debug_printf("%s\n", item_menu[i].title);

      // Call the function to execute
      cycle_count = item_menu[i].action(&size);

      if (cycle_count != -1)
      {
        // Print the number of cycles
        dsp32_debug_printf("Cycle count: %d\n", cycle_count);
        // Print the result
        dsp32_debug_print_vect(vect1, size);
      }
    }
    else
      dsp32_debug_printf("!Invalid item!\n");

    dsp32_debug_printf("<Press any key to continue>\n");
    dsp_debug_read_fct();
  }
}

