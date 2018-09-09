/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit generic version of the real to complex FFT algorithm.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include "dsp.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_TRANS16_REALCOMPLEXFFT) || \
    !defined(TARGET_SPECIFIC_TRANS16_REALCOMPLEXFFT)

#include "trans_dsp16_twiddle_factors.h"

//! Generic 16-bit version
void dsp16_trans_realcomplexfft(dsp16_complex_t *vect1, dsp16_t *vect2, int nlog)
{
  int size;
  int m;
  int stage, j, r, indice, inc_indice, r_brev, r_temp;
  dsp16_t e_real, e_imag, e2_real, e2_imag, e3_real, e3_imag;
  dsp16_t temp;
  int i0, i1, i2, i3;
  dsp32_t a0_real, a0_imag, a1_real, a1_imag, a2_real, a2_imag, a3_real, a3_imag;

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
  inc_indice = (DSP16_N_TWIDDLE_FACTORS/8);
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
      e_real = dsp16_twiddle_factors[indice];
      e_imag = dsp16_twiddle_factors[indice + 1];

#if DSP_OPTIMIZATION & DSP_OPTI_SIZE
      //  e2 = exp(2.0*j*-2*PI*%i/m);
      if (indice >= DSP16_N_TWIDDLE_FACTORS/4)
      {
        r = DSP16_N_TWIDDLE_FACTORS - (indice << 1);
        e2_real = -dsp16_twiddle_factors[r];
        e2_imag = dsp16_twiddle_factors[r + 1];

        if (r < DSP16_N_TWIDDLE_FACTORS/3)
        {
          r = indice - r;
          e3_real = -dsp16_twiddle_factors[r];
          e3_imag = -dsp16_twiddle_factors[r + 1];
        }
        else
        {
          r -= indice;
          e3_real = -dsp16_twiddle_factors[r];
          e3_imag = dsp16_twiddle_factors[r + 1];
        }
      }
      else
      {
        r = indice << 1;
        e2_real = dsp16_twiddle_factors[r];
        e2_imag = dsp16_twiddle_factors[r + 1];

        if (r >= DSP16_N_TWIDDLE_FACTORS/3)
        {
          r = DSP16_N_TWIDDLE_FACTORS - (indice + r);
          e3_real = -dsp16_twiddle_factors[r];
          e3_imag = dsp16_twiddle_factors[r + 1];
        }
        else
        {
          r += indice;
          e3_real = dsp16_twiddle_factors[r];
          e3_imag = dsp16_twiddle_factors[r + 1];
        }
      }

#else
      //  e2 = exp(2.0*j*-2*PI*%i/m);
      e2_real = dsp16_twiddle_factors2[indice*2];
      e2_imag = dsp16_twiddle_factors2[indice*2 + 1];

      //  e3 = exp(3.0*j*-2*PI*%i/m);
      e3_real = dsp16_twiddle_factors2[indice*2 + 2];
      e3_imag = dsp16_twiddle_factors2[indice*2 + 3];
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

        temp = (a1_real*e_real - a1_imag*e_imag) >> (DSP16_QB);
        a1_imag = (a1_real*e_imag + a1_imag*e_real) >> (DSP16_QB);
        a1_real = temp;
        temp = (a2_real*e2_real - a2_imag*e2_imag) >> (DSP16_QB);
        a2_imag = (a2_real*e2_imag + a2_imag*e2_real) >> (DSP16_QB);
        a2_real = temp;
        temp = (a3_real*e3_real - a3_imag*e3_imag) >> (DSP16_QB);
        a3_imag = (a3_real*e3_imag + a3_imag*e3_real) >> (DSP16_QB);
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
        temp = (a1_real*e_real - a1_imag*e_imag) >> (DSP16_QB + 1);
        a1_imag = (a1_real*e_imag + a1_imag*e_real) >> (DSP16_QB + 1);
        a1_real = temp;
        temp = (a2_real*e2_real - a2_imag*e2_imag) >> (DSP16_QB + 1);
        a2_imag = (a2_real*e2_imag + a2_imag*e2_real) >> (DSP16_QB + 1);
        a2_real = temp;
        temp = (a3_real*e3_real - a3_imag*e3_imag) >> (DSP16_QB + 1);
        a3_imag = (a3_real*e3_imag + a3_imag*e3_real) >> (DSP16_QB + 1);
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
