/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit saw tooth signal generator functions for the AVR32 UC3
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
