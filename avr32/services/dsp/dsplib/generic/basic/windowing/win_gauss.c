/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit gauss windowing functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_WIN16_GAUSS) || \
    !defined(TARGET_SPECIFIC_WIN16_GAUSS)

#include "win_gauss.h"

// Gaussian
// w(n) = exp(-1/2*((n-(N-1)/2)/(teta*(N-1)/2))^2)
// avec
//   N = signal width
//   teta = coefficient < 0.5
void dsp16_win_gauss(dsp16_t *vect1, dsp16_t *vect2, int size)
{
  int k, i;
  S32 x, res;
  dsp16_t s, t;
  // CST = 1/(2*ln(2))
  const S32 cst_a = (S32) DSP_Q(32-DSP16_QB, DSP16_QB, 1./CST_LN_2) >> 1;
  // CST = ln(2)
  const S32 cst_b = (S32) DSP16_Q(CST_LN_2);
  // CST = 1./DSP_GAUSS_TETA*2^-floor(DSP16_QB/2)
  const S32 cst_c = (S32) DSP_Q(32-DSP16_QB, DSP16_QB, 1./DSP_GAUSS_TETA) >> (DSP16_QB >> 1);

  // Initialization
  t = DSP16_Q(0.);
  // Increment 1/((N-1)/2)
  s = (DSP16_Q(1.)/(size-1)) << 1;

  // Take advantage of the symmetry
  for(i=0; i<(size >> 1); i++)
  {
    // res = (n-(size-1)/2)/((size-1)/2*TETA)/2
    res = (((S32) (t - DSP16_Q(1.)))) >> ((DSP16_QB >> 1) + 1);
#if (DSP16_QB & 1)
    res >>= 1;
#endif
    res *= cst_c;

    // res = -res^2
    res = res >> (DSP16_QB >> 1);
#if (DSP16_QB & 1)
    res *= -(res >> 1);
#else
    res *= -res;
#endif

    // Calculation of exp(res), where x <= 0
    // i.e. x = -25
    // k = floor(res/((log(2))));
    x = res >> (DSP16_QB - 2);
    k = (x*cst_a) >> (DSP16_QB + 1);

    if (k < -31)
      x = 0;
    else
    {
      // x = (k*log(2)) - res;
      x = ((S32) res) - cst_b*((S32) ++k);
      // Adjust
      if (x > 0)
        x = ((S32) res) - cst_b*((S32) ++k);
      // x = exp(x)*2^k
      x = dsp16_op_exp(x) >> dsp16_op_abs(k);
    }

    // Compute the first half of the window
    vect1[i] = dsp16_op_mul(x, vect2[i]);
    // Compute the rest of the window
    vect1[size-i-1] = dsp16_op_mul(x, vect2[size-i-1]);

    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_WIN32_GAUSS) || \
    !defined(TARGET_SPECIFIC_WIN32_GAUSS)

// Gaussian
// w(n) = exp(-1/2*((n-(N-1)/2)/(teta*(N-1)/2))^2)
// avec
//   N = signal width
//   teta = coefficient < 0.5
void dsp32_win_gauss(dsp32_t *vect1, dsp32_t *vect2, int size)
{
  int k, i;
  dsp32_t s, t;
  S64 x, res;
  // CST = 1/(2*ln(2))
  const S64 cst_a = (S64) ((1LL/CST_LN_2)*(1LL << (DSP32_QB-1)));
  // CST = ln(2)
  const S64 cst_b = (S64) DSP32_Q(CST_LN_2);
  // CST = 1./DSP_GAUSS_TETA*2^-floor(DSP16_QB/2)
  const S64 cst_c = (S64) ((1LL/DSP_GAUSS_TETA)*(1LL << (DSP32_QB-(DSP32_QB >> 1))));

  // Initialization
  t = DSP32_Q(0.);
  // Increment 1/((N-1)/2)
  s = (DSP32_Q(1.)/(size-1)) << 1;

  // Take advantage of the symmetry
  for(i=0; i<(size >> 1); i++)
  {
    // res = (n-(size-1)/2)/((size-1)/2*TETA)/2
    res = (((S64) (t - DSP32_Q(1.)))) >> ((DSP32_QB >> 1) + 1);
#if (DSP32_QB & 1)
    res >>= 1;
#endif
    res *= cst_c;

    // res = -res^2
    res = res >> (DSP32_QB >> 1);
#if (DSP32_QB & 1)
    res *= -(res >> 1);
#else
    res *= -res;
#endif

    // Calculation of exp(res), where x <= 0
    // i.e. x = -25
    // k = floor(res/((log(2))));
    x = res >> (DSP32_QB - 2);
    k = (x*cst_a) >> (DSP32_QB + 1);

    if (k < -31)
      x = 0;
    else
    {
      // x = (k*log(2)) - res;
      x = ((S64) res) - cst_b*((S64) ++k);
      // Adjust
      if (x > 0)
        x = ((S64) res) - cst_b*((S64) ++k);
      // x = exp(x)*2^k
      x = dsp32_op_exp(x) >> dsp32_op_abs(k);
    }

    // Compute the first half of the window
    vect1[i] = dsp32_op_mul(x, vect2[i]);
    // Compute the rest of the window
    vect1[size-i-1] = dsp32_op_mul(x, vect2[size-i-1]);

    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif
