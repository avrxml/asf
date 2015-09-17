/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit generic version of the real to complex FFT algorithm.
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
    defined(FORCE_GENERIC_TRANS32_REALCOMPLEXFFT) || \
    !defined(TARGET_SPECIFIC_TRANS32_REALCOMPLEXFFT)

#include "trans_dsp32_twiddle_factors.h"

void dsp32_trans_realcomplexfft(dsp32_complex_t *vect1, dsp32_t *vect2, int nlog)
{
  int size;
  int m;
  int stage, j, r, indice, inc_indice, r_brev, r_temp;
  dsp32_t e_real, e_imag, e2_real, e2_imag, e3_real, e3_imag;
  dsp32_t temp;
  int i0, i1, i2, i3;
    // 64-bit type
  long long a0_real, a0_imag, a1_real, a1_imag, a2_real, a2_imag, a3_real, a3_imag;

  size = 1 << nlog;

  for(r=0; r<size; r += 4)
  {
    r_brev = 0;
    r_temp = r;
    for(j=0; j<nlog; j++)
    {
      r_brev <<= 1;
      if (r_temp & 1)
        r_brev |= 1;
      r_temp >>= 1;
    }

    a0_real = vect2[r_brev];
    r_brev += size >> 2;
    a1_real = vect2[r_brev];
    r_brev += size >> 2;
    a2_real = vect2[r_brev];
    r_brev += size >> 2;
    a3_real = vect2[r_brev];

    vect1[r].real = (a0_real + a2_real + a1_real + a3_real) >> 2;
    vect1[r].imag = 0;
    vect1[r + 2].real = (a0_real + a2_real - a1_real - a3_real) >> 2;
    vect1[r + 2].imag = 0;

    vect1[r + 1].real = (a0_real - a2_real) >> 2;
    vect1[r + 1].imag = (-a1_real + a3_real) >> 2;
    vect1[r + 3].real = (a0_real - a2_real) >> 2;
    vect1[r + 3].imag = (a1_real - a3_real) >> 2;
  }

  m = 4;
  inc_indice = (DSP32_N_TWIDDLE_FACTORS/8);
  for(stage=4; stage <= nlog; stage+=2)
  {
    m <<= 2;

    indice = 0;

    for(r=0; r<size; r += m)
    {
      i0 = r;
      i1 = i0 + (m >> 2);
      i2 = i1 + (m >> 2);
      i3 = i2 + (m >> 2);

      a0_real = vect1[i0].real;
      a0_imag = vect1[i0].imag;
      a1_real = vect1[i2].real;
      a1_imag = vect1[i2].imag;
      a2_real = vect1[i1].real;
      a2_imag = vect1[i1].imag;
      a3_real = vect1[i3].real;
      a3_imag = vect1[i3].imag;

      vect1[i0].real = (a0_real + a2_real + a1_real + a3_real) >> 2;
      vect1[i0].imag = (a0_imag + a2_imag + a1_imag + a3_imag) >> 2;
      vect1[i2].real = (a0_real + a2_real - a1_real - a3_real) >> 2;
      vect1[i2].imag = (a0_imag + a2_imag - a1_imag - a3_imag) >> 2;

      vect1[i1].real = (a0_real - a2_real + a1_imag - a3_imag) >> 2;
      vect1[i1].imag = (a0_imag - a2_imag - a1_real + a3_real) >> 2;
      vect1[i3].real = (a0_real - a2_real - a1_imag + a3_imag) >> 2;
      vect1[i3].imag = (a0_imag - a2_imag + a1_real - a3_real) >> 2;
    }

    for(j=1; j<(m >> 2); j++)
    {
      indice += inc_indice;

      //  e = exp(1.0*j*-2*PI*%i/m);
      e_real = dsp32_twiddle_factors[indice];
      e_imag = dsp32_twiddle_factors[indice + 1];

#if DSP_OPTIMIZATION & DSP_OPTI_SIZE
      //  e2 = exp(2.0*j*-2*PI*%i/m);
      if (indice >= DSP32_N_TWIDDLE_FACTORS/4)
      {
        r = DSP32_N_TWIDDLE_FACTORS - (indice << 1);
        e2_real = -dsp32_twiddle_factors[r];
        e2_imag = dsp32_twiddle_factors[r + 1];

        if (r < DSP32_N_TWIDDLE_FACTORS/3)
        {
          r = indice - r;
          e3_real = -dsp32_twiddle_factors[r];
          e3_imag = -dsp32_twiddle_factors[r + 1];
        }
        else
        {
          r -= indice;
          e3_real = -dsp32_twiddle_factors[r];
          e3_imag = dsp32_twiddle_factors[r + 1];
        }
      }
      else
      {
        r = indice << 1;
        e2_real = dsp32_twiddle_factors[r];
        e2_imag = dsp32_twiddle_factors[r + 1];

        if (r >= DSP32_N_TWIDDLE_FACTORS/3)
        {
          r = DSP32_N_TWIDDLE_FACTORS - (indice + r);
          e3_real = -dsp32_twiddle_factors[r];
          e3_imag = dsp32_twiddle_factors[r + 1];
        }
        else
        {
          r += indice;
          e3_real = dsp32_twiddle_factors[r];
          e3_imag = dsp32_twiddle_factors[r + 1];
        }
      }

#else
      //  e2 = exp(2.0*j*-2*PI*%i/m);
      e2_real = dsp32_twiddle_factors2[indice*2];
      e2_imag = dsp32_twiddle_factors2[indice*2 + 1];

      //  e3 = exp(3.0*j*-2*PI*%i/m);
      e3_real = dsp32_twiddle_factors2[indice*2 + 2];
      e3_imag = dsp32_twiddle_factors2[indice*2 + 3];
#endif

      for(r=0; r<size; r += m)
      {
        i0 = j + r;
        i1 = i0 + (m >> 2);
        i2 = i1 + (m >> 2);
        i3 = i2 + (m >> 2);

#if DSP_OPTIMIZATION & DSP_OPTI_ACCURACY
        a0_real = vect1[i0].real;
        a0_imag = vect1[i0].imag;
#else
        a0_real = vect1[i0].real >> 2;
        a0_imag = vect1[i0].imag >> 2;
#endif

        a1_real = vect1[i2].real;
        a1_imag = vect1[i2].imag;
        a2_real = vect1[i1].real;
        a2_imag = vect1[i1].imag;
        a3_real = vect1[i3].real;
        a3_imag = vect1[i3].imag;

#if DSP_OPTIMIZATION & DSP_OPTI_ACCURACY

        temp = (a1_real*e_real - a1_imag*e_imag) >> (DSP32_QB);
        a1_imag = (a1_real*e_imag + a1_imag*e_real) >> (DSP32_QB);
        a1_real = temp;
        temp = (a2_real*e2_real - a2_imag*e2_imag) >> (DSP32_QB);
        a2_imag = (a2_real*e2_imag + a2_imag*e2_real) >> (DSP32_QB);
        a2_real = temp;
        temp = (a3_real*e3_real - a3_imag*e3_imag) >> (DSP32_QB);
        a3_imag = (a3_real*e3_imag + a3_imag*e3_real) >> (DSP32_QB);
        a3_real = temp;

        vect1[i0].real = (a0_real + a2_real + (a1_real + a3_real)) >> 2;
        vect1[i0].imag = (a0_imag + a2_imag + (a1_imag + a3_imag)) >> 2;
        vect1[i2].real = (a0_real + a2_real - (a1_real + a3_real)) >> 2;
        vect1[i2].imag = (a0_imag + a2_imag - (a1_imag + a3_imag)) >> 2;

        vect1[i1].real = (a0_real - a2_real + (a1_imag - a3_imag)) >> 2;
        vect1[i1].imag = (a0_imag - a2_imag - (a1_real - a3_real)) >> 2;
        vect1[i3].real = (a0_real - a2_real - (a1_imag - a3_imag)) >> 2;
        vect1[i3].imag = (a0_imag - a2_imag + (a1_real - a3_real)) >> 2;
#else
        temp = (a1_real*e_real - a1_imag*e_imag) >> (DSP32_QB + 1);
        a1_imag = (a1_real*e_imag + a1_imag*e_real) >> (DSP32_QB + 1);
        a1_real = temp;
        temp = (a2_real*e2_real - a2_imag*e2_imag) >> (DSP32_QB + 1);
        a2_imag = (a2_real*e2_imag + a2_imag*e2_real) >> (DSP32_QB + 1);
        a2_real = temp;
        temp = (a3_real*e3_real - a3_imag*e3_imag) >> (DSP32_QB + 1);
        a3_imag = (a3_real*e3_imag + a3_imag*e3_real) >> (DSP32_QB + 1);
        a3_real = temp;

        vect1[i0].real = (a0_real + a2_real + (a1_real + a3_real));
        vect1[i0].imag = (a0_imag + a2_imag + (a1_imag + a3_imag));
        vect1[i2].real = (a0_real + a2_real - (a1_real + a3_real));
        vect1[i2].imag = (a0_imag + a2_imag - (a1_imag + a3_imag));

        vect1[i1].real = (a0_real - a2_real + (a1_imag - a3_imag));
        vect1[i1].imag = (a0_imag - a2_imag - (a1_real - a3_real));
        vect1[i3].real = (a0_real - a2_real - (a1_imag - a3_imag));
        vect1[i3].imag = (a0_imag - a2_imag + (a1_real - a3_real));
#endif
      }
    }
    inc_indice >>= 2;
  }
}

#endif
