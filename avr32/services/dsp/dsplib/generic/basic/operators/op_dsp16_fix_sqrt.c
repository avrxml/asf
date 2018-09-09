/*****************************************************************************
 *
 * \file
 *
 * \brief Square root operator for the AVR32 UC3.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include "dsp.h"
#include "preprocessor.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP16_SQRT) || \
    !defined(TARGET_SPECIFIC_OP16_SQRT)

#define DSP16_SQRT_ONE_POINT_FIVE   ((S32) (3 << (DSP16_QB-1)))

#define DSP16_SQRT_NEWTON_ITERATION(x_num, data) \
  a = ((S32) x)*((S32) x); \
  a = (((S32) num)*a) >> (DSP16_QB+1); \
  x = (((S32) x)*(DSP16_SQRT_ONE_POINT_FIVE - a)) >> (DSP16_QB);

// Square root using Reciproot Iteration
// Adapted for fixed point numbers
dsp16_t dsp16_op_sqrt(dsp16_t num)
{
  int under_bit_val;
  dsp16_t num_temp, x;
  S32 a;

  // Limit
  if (num < 0)
    return 0;

  // Find an approximation of 1/sqrt(x);
  under_bit_val = 0;
  num_temp = num;
  while(num_temp)
  {
    num_temp >>= 1;
    under_bit_val++;
  }
  under_bit_val >>= 1;

  // x ~ 1/sqrt(num)
  x = 1 << (DSP16_QB - under_bit_val);

  // Perform a Newton Iteration
  MREPEAT(3, DSP16_SQRT_NEWTON_ITERATION, "");

#if (DSP16_QB%2 == 1)
  // To support Q1.(2N+1) fixed point numbers
  num = (num*((S32) DSP16_Q(CST_INV_SQUARE_ROOT_2))) >> (DSP16_QB);
#endif
  // Get sqrt(x) from 1/sqrt(x)
  a = (((S32) x)*((S32) num));
  // Adjust the result for fixed point format
  a >>= (DSP16_QB >> 1);

  return (dsp16_t) a;
}

#endif
