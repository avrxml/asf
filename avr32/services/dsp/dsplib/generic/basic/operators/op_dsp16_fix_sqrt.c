/*****************************************************************************
 *
 * \file
 *
 * \brief Square root operator for the AVR32 UC3.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
