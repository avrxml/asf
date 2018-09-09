/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit gauss windowing functions for the AVR32 UC3
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
