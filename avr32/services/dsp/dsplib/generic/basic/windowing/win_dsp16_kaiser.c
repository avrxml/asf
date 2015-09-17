/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit kaiser windowing functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_WIN16_KAISER) || \
    !defined(TARGET_SPECIFIC_WIN16_KAISER)

/* Zeroth order modified Bessel function  */
static dsp16_t dsp16_op_kaiser_i0(S32 ax, int *power)
{
  S32 ans, y, k, num_temp;

  const S32 A0 = (S32) (1.0 * (1 << 12));                                           // 1.0 << 12
  const S32 A1 = (S32) ((3.5156229 * (1 << 12)) / (3.75*3.75));                     // 14.0625    0.2499999 << 12
  const S32 A2 = (S32) ((3.0899424 * (1 << 12)) / (3.75*3.75*3.75*3.75));           // 197.75391  0.0156252 << 12
  const S32 A3 = (S32) ((1.2067492 * (1 << 12)) / (3.75*3.75*3.75*3.75*3.75*3.75)); // 2780.9143  0.0004339 << 12

  const S32 B0 = DSP16_Q(0.39894228);
  const S32 B1 = DSP16_Q(0.01328592);
  const S32 B2 = DSP16_Q(0.00225319);

  const S32 cst_inv_ln_2 = ((S32) DSP_Q(32-DSP16_QB, DSP16_QB, 1./0.69314718055994530941723212145818)) >> (DSP16_QB >> 1);
  const S32 cst_ln_2 = DSP16_Q(0.69314718055994530941723212145818);


  if (ax < (S32) DSP_Q(32-DSP16_QB, DSP16_QB, 3.75))
  {
    // calculate the exponent*
    *power = 3;

    // Format Q4.12 to contain 3.75^2
    // Change format
#if (DSP16_QB > 12)
    y = ax >> (DSP16_QB - 12);
#else
    y = ax << (12 - DSP16_QB);
#endif
    y = (y*y) >> 12;

    ans = A3;
    ans = (((ans*y) >> 12) + A2);
    ans = (((ans*y) >> 12) + A1);
    ans = (((ans*y) >> 12) + A0);

#if (DSP16_QB > 15)
    ans = ans << (DSP16_QB - 15);
#else
    ans = ans >> (15 - DSP16_QB);
#endif
  }
  else
  {
    // ans is in the range [1; 0]
    ans = ((S32) DSP_Q(32-DSP16_QB, DSP16_QB, 3.75)) / ax;

    y = B2;
    y = (((y*ans) >> DSP16_QB) + B1);
    y = (((y*ans) >> DSP16_QB) + B0);

/***** Computation of exp *****/
#if (DSP16_QB & 1)
    ans = (ax >> ((DSP16_QB >> 1) + 1));
#else
    ans = (ax >> (DSP16_QB >> 1));
#endif

    k = ((cst_inv_ln_2*ans) >> DSP16_QB) + 1;
    num_temp = ax - k*cst_ln_2;
    if (num_temp > 0)
      num_temp = ax - (++k)*cst_ln_2;

    ans = dsp16_op_exp(num_temp);

    *power = k;
/******************************/

/***** Computation of sqrt *****/
    // get ~ int(log4(num))
    k = 0;
    num_temp = ax >> DSP16_QB;
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
    num_temp = dsp16_op_sqrt(num_temp);
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
void dsp16_win_kaiser(dsp16_t *vect1, dsp16_t *vect2, int size, int alpha)
{
  int n, power_num, power_den;
  S32 pi_alpha, pi_alpha_div;
  S32 temp32;
  dsp16_t s, t, temp16, num, den;

  // 2/(N-1)
  s = (DSP16_Q(1.) / (size - 1)) << 1;
  // PI*alpha
  pi_alpha = ((S32) DSP_Q(32-DSP16_QB, DSP16_QB, CST_PI))*alpha;
  // PI*alpha >> floor(DSP16_QB/2)
  pi_alpha_div = pi_alpha >> (DSP16_QB >> 1);
  // I0(PI*alpha);
  den = dsp16_op_kaiser_i0(pi_alpha, &power_den);

  t = 0;
  // Take advantage of the symmetry
  for(n=0; n<(size >> 1); n++)
  {
    // 2*n/(N-1)-1
    temp16 = t - DSP16_Q(1);
    // (2*n/(N-1)-1)^2
    temp16 = (((S32) temp16) * ((S32) temp16)) >> DSP16_QB;
    // sqrt(1-(2*n/(N-1)-1)^2)
    // Uses 32-bit sqrt because of precision needed
    temp16 = dsp32_op_sqrt((DSP16_Q(1.) - temp16) << (DSP32_QB - DSP16_QB)) >> (DSP32_QB - DSP16_QB);

    // PI*alpha*sqrt(1-(2*n/(N-1)-1)^2)
#if (DSP16_QB & 1)
    temp32 = (pi_alpha_div * ((S32) temp16)) >> ((DSP16_QB >> 1) + 1);
#else
    temp32 = (pi_alpha_div * ((S32) temp16)) >> (DSP16_QB >> 1);
#endif
    // I0(temp);
    num = dsp16_op_kaiser_i0(temp32, &power_num);

    // Perform the division
    power_num = power_den - power_num;

    temp32 = ((S32) num) << (DSP16_QB);
    temp32 = temp32 / (S32) den;
    temp32 >>= power_num;

    vect1[n] = dsp16_op_mul(temp32, vect2[n]);
    vect1[size-n-1] = dsp16_op_mul(temp32, vect2[size-n-1]);

    // t = 2/(N-1)*n
    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif
