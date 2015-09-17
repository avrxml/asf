/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit noise generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_NOISE) || \
    !defined(TARGET_SPECIFIC_GEN16_NOISE)

// rand()*amp
void dsp16_gen_noise(dsp16_t *vect1, int size, dsp16_t amp)
{
  int t;

  // If amp == 1. then the signal is much faster to compute
  if (amp == DSP16_Q(1.))
  {
    // Main loop
    for(t=0; t<size; t++)
      vect1[t] = dsp16_op_rand();
  }
  else
  {
    // Main loop
    for(t=0; t<size; t++)
      // Get a pseudo-random sample and scale it with the desired amplitude
      vect1[t] = ((S32) dsp16_op_rand())*((S32) amp) >> DSP16_QB;
  }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_NOISE) || \
    !defined(TARGET_SPECIFIC_GEN32_NOISE)

// rand()*amp
void dsp32_gen_noise(dsp32_t *vect1, int size, dsp32_t amp)
{
  int t;

  // If amp == 1. then the signal is much faster to compute
  if (amp == DSP32_Q(1.))
  {
    // Main loop
    for(t=0; t<size; t++)
      vect1[t] = dsp32_op_rand();
  }
  else
  {
    // Main loop
    for(t=0; t<size; t++)
      // Get a pseudo-random sample and scale it with the desired amplitude
      vect1[t] = ((S64) dsp32_op_rand())*((S64) amp) >> DSP32_QB;
  }
}

#endif
