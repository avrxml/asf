/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit saw tooth signal generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_SAW) || \
    !defined(TARGET_SPECIFIC_GEN16_SAW)

dsp16_t dsp16_gen_saw(dsp16_t *vect1, int size, int f, int fs, dsp16_t duty, dsp16_t delay)
{
  int i = 0;
  S32 t_low_inc, t_high_inc, t;
  dsp16_t delta_rise, delta_decrease, delta;

  // Number of sample per period
  t_low_inc = fs / f;
  // Number of sample per high signal
  t_high_inc = (((S32) duty) * ((S32) t_low_inc)) >> DSP16_QB;
  // Number of sample for the delay
  t = (((S32) delay) * ((S32) t_low_inc)) >> DSP16_QB;
  // Number of sample per low signal
  t_low_inc -= t_high_inc;

  // Calculate the delta coefficient of the rising edge of the saw tooth
  delta_rise = (DSP16_Q(1.) / t_high_inc) * 2;
  // Calculate the delta coefficient of the decreasing edge of the saw tooth
  delta_decrease = (DSP16_Q(1.) / t_low_inc) * 2;

  // Compute the initial value
  if (t < t_high_inc)
    delta = DSP16_Q(-1.) + t * delta_rise;
  else
    delta = DSP16_Q(1.) - (t - t_high_inc) * delta_decrease;

  while(i < size)
  {
    int lim, j;

    if (i)
      t = 0;

    lim = Max(0, Min(t_high_inc - t, size - i));
    for(j=0; j<lim; j++)
      vect1[i++] = (delta += delta_rise);
    t += lim;

    if (j)
      delta = DSP16_Q(1.);

    lim = Min(t_high_inc + t_low_inc - t, size - i);
    for(j=0; j<lim; j++)
      vect1[i++] = (delta -= delta_decrease);
    t += lim;

    delta = DSP16_Q(-1.);
  }

  return (t << DSP16_QB) / (t_high_inc + t_low_inc);
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_SAW) || \
    !defined(TARGET_SPECIFIC_GEN32_SAW)

dsp32_t dsp32_gen_saw(dsp32_t *vect1, int size, int f, int fs, dsp32_t duty, dsp32_t delay)
{
  int i = 0;
  S32 t_low_inc, t_high_inc, t;
  dsp32_t delta_rise, delta_decrease, delta;

  // Number of sample per period
  t_low_inc = fs / f;
  // Number of sample per high signal
  t_high_inc = (((S64) duty) * ((S64) t_low_inc)) >> DSP32_QB;
  // Number of sample for the delay
  t = (((S64) delay) * ((S64) t_low_inc)) >> DSP32_QB;
  // Number of sample per low signal
  t_low_inc -= t_high_inc;

  // Calculate the delta coefficient of the rising edge of the saw tooth
  delta_rise = (DSP32_Q(1.) / t_high_inc) * 2;
  // Calculate the delta coefficient of the decreasing edge of the saw tooth
  delta_decrease = (DSP32_Q(1.) / t_low_inc) * 2;

  // Compute the initial value
  if (t < t_high_inc)
    delta = DSP32_Q(-1.) + t * delta_rise;
  else
    delta = DSP32_Q(1.) - (t - t_high_inc) * delta_decrease;

  while(i < size)
  {
    int lim, j;

    if (i)
      t = 0;

    lim = Max(0, Min(t_high_inc - t, size - i));
    for(j=0; j<lim; j++)
      vect1[i++] = (delta += delta_rise);
    t += lim;

    if (j)
      delta = DSP32_Q(1.);

    lim = Min(t_high_inc + t_low_inc - t, size - i);
    for(j=0; j<lim; j++)
      vect1[i++] = (delta -= delta_decrease);
    t += lim;

    delta = DSP32_Q(-1.);
  }

  return (t << DSP32_QB) / (t_high_inc + t_low_inc);
}


#endif
