/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit kaiser windowing functions for the AVR32 UC3
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "dsp.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_WIN32_KAISER) || \
    !defined(TARGET_SPECIFIC_WIN32_KAISER)

/* Zeroth order modified Bessel function  */
static dsp32_t dsp32_op_kaiser_i0(S64 ax, int *power)
{
  S64 ans, y, k, num_temp;

  const S64 A0 = (S64) (1.0 * (1 << 28));                                                               // 1.0 << 12
  const S64 A1 = (S64) ((3.5156229 * (1 << 28)) / (3.75*3.75));                                         // 14.0625    0.2499999 << 28
  const S64 A2 = (S64) ((3.0899424 * (1 << 28)) / (3.75*3.75*3.75*3.75));                               // 197.75391  0.0156252 << 28
  const S64 A3 = (S64) ((1.2067492 * (1 << 28)) / (3.75*3.75*3.75*3.75*3.75*3.75));                     // 2780.9143  0.0004339 << 12
  const S64 A4 = (S64) ((0.2659732 * (1 << 28)) / (3.75*3.75*3.75*3.75*3.75*3.75*3.75*3.75));           // 39106.607  0.0000068 << 12
  const S64 A5 = (S64) ((0.0360768 * (1 << 28)) / (3.75*3.75*3.75*3.75*3.75*3.75*3.75*3.75*3.75*3.75)); // 549936.67  6.560D-08 << 12

  const S64 B0 = DSP32_Q(0.39894228);
  const S64 B1 = DSP32_Q(0.01328592);
  const S64 B2 = DSP32_Q(0.00225319);
  const S64 B3 = DSP32_Q(-0.00157565);
  const S64 B4 = DSP32_Q(0.00916281);
  const S64 B5 = DSP32_Q(-0.02057706);

  const S64 cst_inv_ln_2 = DSP32_Q((1./0.69314718055994530941723212145818)/(1.*(1 << (DSP32_QB >> 1))));
  const S64 cst_ln_2 = DSP32_Q(0.69314718055994530941723212145818);

  if (ax < (S64) (0xFLL << (DSP32_QB - 2)))
  {
    // calculate the exponent*
    *power = 3;

    // Format Q4.12 to contain 3.75^2
    // Change format
#if (DSP32_QB > 28)
    y = ax >> (DSP32_QB - 28);
#else
    y = ax << (28 - DSP32_QB);
#endif
    y = (y*y) >> 28;

    ans = A5;
    ans = (((ans*y) >> 28) + A4);
    ans = (((ans*y) >> 28) + A3);
    ans = (((ans*y) >> 28) + A2);
    ans = (((ans*y) >> 28) + A1);
    ans = (((ans*y) >> 28) + A0);

#if (DSP32_QB > 31)
    ans = ans << (DSP32_QB - 31);
#else
    ans = ans >> (31 - DSP32_QB);
#endif
  }
  else
  {
    // ans is in the range [1; 0]
    ans = ((S64) (0xFLL << (DSP32_QB - 2))) / ax;

    y = B5;
    y = (((y*ans) >> DSP32_QB) + B4);
    y = (((y*ans) >> DSP32_QB) + B3);
    y = (((y*ans) >> DSP32_QB) + B2);
    y = (((y*ans) >> DSP32_QB) + B1);
    y = (((y*ans) >> DSP32_QB) + B0);

/***** Computation of exp *****/
#if (DSP32_QB & 1)
    ans = (ax >> ((DSP32_QB >> 1) + 1));
#else
    ans = (ax >> (DSP32_QB >> 1));
#endif

    k = ((cst_inv_ln_2*ans) >> DSP32_QB) + 1;
    num_temp = ax - k*cst_ln_2;
    if (num_temp > 0)
      num_temp = ax - (++k)*cst_ln_2;

    ans = dsp32_op_exp(num_temp);

    *power = k;
/******************************/

/***** Computation of sqrt *****/
    // get ~ int(log4(num))
    k = 0;
    num_temp = ax >> DSP32_QB;
    while(num_temp)
    {
      num_temp >>= 2;
      k++;
    }
    // subtract k to the power because of the division
    *power -= k;

    // ax = ax / 4^k
    num_temp = ax >> (k << 1);

    // now ax is in the range [1; 0]
    num_temp = dsp32_op_sqrt(num_temp);
/******************************/

    ans = (y*ans) / num_temp;
  }

  return ans;
}

// Kaiser
// w(n) = I0(PI*alpha*sqrt(1-(2*n/(N-1)-1)^2))/I0(PI*alpha)
// avec
//   N = signal width
//   alpha = a coefficient
//   I0 = The zeroth order modified Bessel function of the first kind
void dsp32_win_kaiser(dsp32_t *vect1, dsp32_t *vect2, int size, int alpha)
{
  int n, power_num, power_den;
  S64 pi_alpha, pi_alpha_div;
  S64 temp64;
  dsp32_t s, t, temp32, num, den;

  // 2/(N-1)
  s = (DSP32_Q(1.) / (size - 1)) << 1;
  // PI*alpha
  pi_alpha = (S64) (CST_PI*(1LL << DSP32_QB))*alpha;
  // PI*alpha >> floor(DSP32_QB/2)
  pi_alpha_div = pi_alpha >> (DSP32_QB >> 1);
  // I0(PI*alpha);
  den = dsp32_op_kaiser_i0(pi_alpha, &power_den);

  t = 0;
  // Take advantage of the symmetry
  for(n=0; n<(size >> 1); n++)
  {
    // 2*n/(N-1)-1
    temp32 = t - DSP32_Q(1);
    // (2*n/(N-1)-1)^2
    temp32 = (((S64) temp32) * ((S64) temp32)) >> DSP32_QB;
    // sqrt(1-(2*n/(N-1)-1)^2)
    temp32 = dsp32_op_sqrt(DSP32_Q(1.) - temp32);

    // PI*alpha*sqrt(1-(2*n/(N-1)-1)^2)
#if (DSP32_QB & 1)
    temp64 = (pi_alpha_div * ((S64) temp32)) >> ((DSP32_QB >> 1) + 1);
#else
    temp64 = (pi_alpha_div * ((S64) temp32)) >> (DSP32_QB >> 1);
#endif
    // I0(temp);
    num = dsp32_op_kaiser_i0(temp64, &power_num);

    // Perform the division
    power_num = power_den - power_num;

    temp64 = ((S64) num) << (DSP32_QB);
    temp64 = temp64 / (S64) den;
    temp64 >>= power_num;

    vect1[n] = dsp32_op_mul(temp64, vect2[n]);
    vect1[size-n-1] = dsp32_op_mul(temp64, vect2[size-n-1]);

    // t = 2/(N-1)*n
    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif
