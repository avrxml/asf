/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit NLMS filter function optimized for the AVR32 UC3.
 *
 * This file contains the code of the NLMS filter.
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

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_FILT32_NLMS) && \
    defined(TARGET_SPECIFIC_FILT32_NLMS)

#include "filt_nlms.h"

void dsp32_filt_nlms(dsp32_t *x, dsp32_t *w, int size, dsp32_t new_x, dsp32_t d, dsp32_t *y, dsp32_t *e)
{
  int i = 0;
  static S64 normalization = 0;
#if !(DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
  S64 temp = 0;
#endif

  x[0] = new_x;

  dsp32_filt_nlms_fir(x, w, size, y, i);

  // Error calculation
  *e = d - *y;

  // Update the normalization coefficient
  normalization += ((((S64) new_x)*((S64) new_x))) >> (DSP32_QB);

#if !(DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
  temp = (((S64) *e) << DSP32_QB)/normalization;

  if (temp > ((S64) 0x7FFFFFFF))
    temp = (S64) 0x7FFFFFFF;
  else if (temp < -((S64) 0x80000000))
    temp = -((S64) 0x80000000);
#endif

  if (!normalization)
    normalization = 1;

  // Refresh the w coefficients
  for(i=1; i<size-1; i+=4)
  {
#if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
    w[i] += (((((S64) *e)*((S64) x[i])))/normalization) >> (DSP_NLMS_MU - 1);
    w[i+1] += (((((S64) *e)*((S64) x[i+1])))/normalization) >> (DSP_NLMS_MU - 1);
    w[i+2] += (((((S64) *e)*((S64) x[i+2])))/normalization) >> (DSP_NLMS_MU - 1);
    w[i+3] += (((((S64) *e)*((S64) x[i+3])))/normalization) >> (DSP_NLMS_MU - 1);
#else
    w[i] += ((temp*((S64) x[i])) >> (DSP_NLMS_MU - 1 + DSP32_QB));
    w[i+1] += ((temp*((S64) x[i+1])) >> (DSP_NLMS_MU - 1 + DSP32_QB));
    w[i+2] += ((temp*((S64) x[i+2])) >> (DSP_NLMS_MU - 1 + DSP32_QB));
    w[i+3] += ((temp*((S64) x[i+3])) >> (DSP_NLMS_MU - 1 + DSP32_QB));
#endif
  }

  // Update the normalization coefficient
  normalization -= ((((S64) x[size-1])*((S64) x[size-1]))) >> (DSP32_QB);

  // Shift the circular buffer
  for(i=size-1; i>0; i-=4)
  {
    x[i] = x[i-1];
    x[i-1] = x[i-2];
    x[i-2] = x[i-3];
    x[i-3] = x[i-4];
  }
}

#endif

