/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit hamming windowing functions for the AVR32 UC3
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

#define WIN_HAMM_A0  0.53836
#define WIN_HAMM_A1  0.46164

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_WIN16_HAMM) || \
    !defined(TARGET_SPECIFIC_WIN16_HAMM)

// Hamming
// w(n) = 0.53836 - 0.46164*cos(2*PI*n/(N-1))
// avec
// N = signal width
// Constraints: size > 1
void dsp16_win_hamm(dsp16_t *vect1, dsp16_t *vect2, int size)
{
  dsp16_t s, t, w;
  int i;

  // Initialization
  t = DSP16_Q(0.);
  // Calculation of 2/(N-1) <- because a cos is in the range [-1; 1],
  // therefore -1 equals -PI and 1, PI.
  s = (DSP16_Q(1.)/(size - 1)) << 1;

  // Compute the window
  for(i=0; i<(size >> 1); i++)
  {
    w = DSP16_Q(WIN_HAMM_A0);
    w -= (((S32) DSP16_Q(WIN_HAMM_A1))*((S32) dsp16_op_cos(t))) >> DSP16_QB;
    vect1[i] = dsp16_op_mul(w, vect2[i]);
    vect1[size-i-1] = dsp16_op_mul(w, vect2[size-i-1]);
    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_WIN32_HAMM) || \
    !defined(TARGET_SPECIFIC_WIN32_HAMM)

void dsp32_win_hamm(dsp32_t *vect1, dsp32_t *vect2, int size)
{
  dsp32_t s, t, w;
  int i;

  // Initialization
  t = DSP32_Q(0.);
  // Calculation of 2/(N-1) <- because a cos is in the range [-1; 1],
  // therefore -1 equals -PI and 1, PI.
  s = (DSP32_Q(1.)/(size - 1)) << 1;

  // Compute the window
  for(i=0; i<(size >> 1); i++)
  {
    w = DSP32_Q(WIN_HAMM_A0);
    w -= (((S64) DSP32_Q(WIN_HAMM_A1))*((S64) dsp32_op_cos(t))) >> DSP32_QB;
    vect1[i] = dsp32_op_mul(w, vect2[i]);
    vect1[size-i-1] = dsp32_op_mul(w, vect2[size-i-1]);
    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif
