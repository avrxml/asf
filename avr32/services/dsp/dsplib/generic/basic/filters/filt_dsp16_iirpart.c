/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit IIR filter function.
 *
 * This file contains the code of the IIR filter.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "dsp.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_FILT16_IIRPART) || \
    !defined(TARGET_SPECIFIC_FILT16_IIRPART)

void dsp16_filt_iirpart(dsp16_t *y, dsp16_t *x, int size, dsp16_t *num, int num_size, dsp16_t *den, int den_size, int num_prediv, int den_prediv)
{
  int n, m;
  S32 sum1, sum2;
  dsp16_t *px, *py;

  num_prediv = DSP16_QB - num_prediv;
  den_prediv = DSP16_QB - den_prediv;

  // Compute the first den_size elements
  // to initialize the vect1
  for(n=0; n<den_size; n++)
  {
    sum1 = 0;
    sum2 = 0;
    px = &x[n+num_size-1];
    for(m=0; m<num_size; m++)
      sum1 += (S32) num[m] * (S32) px[-m];
    py = &y[n-1];
    for(m=0; m<n; m++)
      sum2 += (S32) den[m] * (S32) py[-m];

    sum1 >>= num_prediv;
    sum2 >>= den_prediv;

    y[n] = sum1 - sum2;
  }

  for(; n<=(size-num_size); n++)
  {
    sum1 = 0;
    sum2 = 0;
    px = &x[n+num_size-1];
    for(m=0; m<num_size; m++)
      sum1 += (S32) num[m] * (S32) x[-m];
    py = &y[n-1];
    for(m=0; m<den_size; m++)
      sum2 += (S32) den[m] * (S32) y[-m];

    sum1 >>= num_prediv;
    sum2 >>= den_prediv;

    y[n] = sum1 - sum2;
  }
}

#endif
