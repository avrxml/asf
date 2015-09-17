/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit dirac comb signal generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_DCOMB) || \
    !defined(TARGET_SPECIFIC_GEN16_DCOMB)

void dsp16_gen_dcomb(dsp16_t *vect1, int size, int f, int fs, dsp16_t delay)
{
  int i;
  int t_inc, t_low, cp;
  int t_delay;

  // Number of sample per period
  t_inc = fs / f;
  // Number of sample for the delay
  t_delay = (((S32) delay) * ((S32) t_inc)) >> DSP16_QB;

  // Number of sample per low signal
  t_inc--;

  i = 0;
  t_low = t_delay;

  // Main loop
  while(i < size)
  {
    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP16_Q(0.);
    // High part
    if (i < size)
      vect1[i++] = DSP16_Q(1.);

    t_low = t_inc + i;
  }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_DCOMB) || \
    !defined(TARGET_SPECIFIC_GEN32_DCOMB)

void dsp32_gen_dcomb(dsp32_t *vect1, int size, int f, int fs, dsp32_t delay)
{
  int i;
  int t_inc, t_low, cp;
  int t_delay;

  // Number of sample per period
  t_inc = fs / f;
  // Number of sample for the delay
  t_delay = (((S64) delay) * ((S64) t_inc)) >> DSP32_QB;

  // Number of sample per low signal
  t_inc--;

  i = 0;
  t_low = t_delay;

  // Main loop
  while(i < size)
  {
    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP32_Q(0.);
    // High part
    if (i < size)
      vect1[i++] = DSP32_Q(1.);

    t_low = t_inc + i;
  }
}

#endif
