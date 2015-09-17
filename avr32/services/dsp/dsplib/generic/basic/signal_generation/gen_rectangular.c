/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit rectangular signal generator functions for the AVR32 UC3
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

#ifndef MIN
# define MIN(x, y)  (((x) < (y))?(x):(y))
#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN16_RECT) || \
    !defined(TARGET_SPECIFIC_GEN16_RECT)

void dsp16_gen_rect(dsp16_t *vect1, int size, int f, int fs, dsp16_t duty, dsp16_t delay)
{
  int i = 0;
  int t_low_inc, t_high_inc, cp;
  int t_low, t_high, t_delay;

  // Number of sample per period
  t_low_inc = fs / f;
  // Number of sample per high signal
  t_high_inc = (((S32) duty) * ((S32) t_low_inc)) >> DSP16_QB;
  // Number of sample for the delay
  t_delay = (((S32) delay) * ((S32) t_low_inc)) >> DSP16_QB;
  // Number of sample per low signal
  t_low_inc -= t_high_inc;

  i = 0;

  // For the delay
  t_high = MIN(t_high_inc, t_delay - t_low_inc);
  t_low = MIN(t_low_inc, t_delay);
  if (t_high > 0)
    t_low += t_high;

  // Main loop
  while(i < size)
  {
    // High part
    cp = MIN(t_high, size);
    for(; i<cp; i++)
      vect1[i] = DSP16_Q(1.);

    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP16_Q(-1.);

    t_high = t_high_inc + i;
    t_low = t_high + t_low_inc;
  }

}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_RECT) || \
    !defined(TARGET_SPECIFIC_GEN32_RECT)

void dsp32_gen_rect(dsp32_t *vect1, int size, int f, int fs, dsp32_t duty, dsp32_t delay)
{
  int i = 0;
  int t_low_inc, t_high_inc, cp;
  int t_low, t_high, t_delay;

  // Number of sample per period
  t_low_inc = fs / f;
  // Number of sample per high signal
  t_high_inc = (((S64) duty) * ((S64) t_low_inc)) >> DSP32_QB;
  // Number of sample for the delay
  t_delay = (((S64) delay) * ((S64) t_low_inc)) >> DSP32_QB;
  // Number of sample per low signal
  t_low_inc -= t_high_inc;

  i = 0;

  // For the delay
  t_high = MIN(t_high_inc, t_delay - t_low_inc);
  t_low = MIN(t_low_inc, t_delay);
  if (t_high > 0)
    t_low += t_high;

  // Main loop
  while(i < size)
  {
    // High part
    cp = MIN(t_high, size);
    for(; i<cp; i++)
      vect1[i] = DSP32_Q(1.);

    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP32_Q(-1.);

    t_high = t_high_inc + i;
    t_low = t_high + t_low_inc;
  }

}

#endif
