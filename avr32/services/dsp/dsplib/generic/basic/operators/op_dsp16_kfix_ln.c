/*****************************************************************************
 *
 * \file
 *
 * \brief Natural logarithm operator for the AVR32 UC3.
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
    defined(FORCE_GENERIC_OP16_LN) || \
    !defined(TARGET_SPECIFIC_OP16_LN)

S32 dsp16_op_ln_raw(dsp16_t num)
{
  dsp16_t num_temp;
  S32 a, a_sqr, res;
  int under_bit_val;

  // Limit
  if (num <= DSP16_Q(0.))
    return DSP_Q_MIN(DSP16_QA, DSP16_QB);

// Remove warnings
#if DSP16_QA > 1
  // get ~ int(log2(num))
  // num is in the range [1; 0.5]
  if (num > DSP16_Q(1.))
  {
    under_bit_val = 0;
    num_temp = num;
    while (num_temp > DSP16_Q(1.))
    {
      num_temp >>= 1;
      under_bit_val++;
    }
    num_temp = (num >> under_bit_val);
  }
  else
#endif
  {
    under_bit_val = 0;
    num_temp = num;
    while (num_temp < DSP16_Q(0.5))
    {
      num_temp <<= 1;
      under_bit_val++;
    }
    num_temp = (num << under_bit_val);
  }

  // << (DSP16_QB - 1) to avoid overflows
  a = (((S32) num_temp) - DSP16_Q(1.)) << DSP16_QB;
  a = a/(((S32) num_temp) + DSP16_Q(1.));
  // calculate a^2
  a_sqr = (a*a) >> DSP16_QB;

  res = (a_sqr*DSP16_Q(1./9.)) >> DSP16_QB;
  res = (a_sqr*(res + DSP16_Q(1./7.))) >> DSP16_QB;
  res = (a_sqr*(res + DSP16_Q(1./5.))) >> DSP16_QB;
  res = (a_sqr*(res + DSP16_Q(1./3.))) >> DSP16_QB;
  res = (a*(res + DSP16_Q(1.))) >> (DSP16_QB - 1);

// Remove warnings
#if DSP16_QA > 1
  if (num > DSP16_Q(1.))
    res += ((S32) under_bit_val)*((S32) DSP16_Q(CST_LN_2));
  else
#endif
    res -= ((S32) under_bit_val)*((S32) DSP16_Q(CST_LN_2));

  return res;
}

#endif
