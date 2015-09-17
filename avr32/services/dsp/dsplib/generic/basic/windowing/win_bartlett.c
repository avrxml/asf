/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit Bartlett windowing functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_WIN16_BART) || \
    !defined(TARGET_SPECIFIC_WIN16_BART)

void dsp16_win_bart(dsp16_t *vect1, dsp16_t *vect2, int size)
{
  dsp16_t s, t;
  int i;

  // Initialization
  t = DSP16_Q(0.);
  // Increment
  s = (DSP16_Q(1.) / size) << 1;
  // Compute the 1st half
  for(i=0; i<(size >> 1); i++)
  {
    vect1[i] = dsp16_op_mul(t, vect2[i]);
    vect1[size-i-1] = dsp16_op_mul(t, vect2[size-i-1]);
    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_WIN32_BART) || \
    !defined(TARGET_SPECIFIC_WIN32_BART)

void dsp32_win_bart(dsp32_t *vect1, dsp32_t *vect2, int size)
{
  dsp32_t s, t;
  int i;

  // Initialization
  t = DSP32_Q(0.);
  // Increment
  s = (DSP32_Q(1.) / size) << 1;
  // Compute the 1st half
  for(i=0; i<(size >> 1); i++)
  {
    vect1[i] = dsp32_op_mul(t, vect2[i]);
    vect1[size-i-1] = dsp32_op_mul(t, vect2[size-i-1]);
    t += s;
  }

  // If the size is odd
  if (size & 1)
    vect1[size >> 1] = vect2[size >> 1];

}

#endif
