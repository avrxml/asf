/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit sinusoidal signal generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_SIN) || \
    !defined(TARGET_SPECIFIC_GEN16_SIN)

// sin(2*pi*f/fs*t + phase)
dsp16_t dsp16_gen_sin(dsp16_t *vect1, int size, int f, int fs, dsp16_t phase)
{
  int t;
  dsp16_t s, angle;

  angle = phase;
  // Calculate the angle "s" which is used to increment the argument of the sine each transition in the loop
  s = (((S32) f) << (DSP16_QB + 1))/fs;

  // Main loop
  for(t=0; t<size; t++)
  {
    vect1[t] = dsp16_op_sin(angle);
    // Increment the argument of the sine
    angle += s;
  }

  // Return the current phase of the generated signal
  return angle;
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_SIN) || \
    !defined(TARGET_SPECIFIC_GEN32_SIN)

// sin(2*pi*f/fs*t + phase)
dsp32_t dsp32_gen_sin(dsp32_t *vect1, int size, int f, int fs, dsp32_t phase)
{
  int t;
  dsp32_t s, angle;

  angle = phase;
  // Calculate the angle "s" which is used to increment the argument of the sine each transition in the loop
  s = (((long long) f) << (DSP32_QB + 1))/fs;

  // Main loop
  for(t=0; t<size; t++)
  {
    vect1[t] = dsp32_op_sin(angle);
    // Increment the argument of the sine
    angle += s;
  }

  // Return the current phase of the generated signal
  return angle;
}

#endif
