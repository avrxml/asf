/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit NLMS filter function.
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

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_FILT16_NLMS) || \
    !defined(TARGET_SPECIFIC_FILT16_NLMS)

#include "filt_nlms.h"

void dsp16_filt_nlms(dsp16_t *x, dsp16_t *w, int size, dsp16_t new_x, dsp16_t d, dsp16_t *y, dsp16_t *e)
{
  int i;
  static S32 normalization = 0;
  S32 temp;

  x[0] = new_x;

  temp = 0;
  // Performed a FIR
  for(i=0; i<size; i+=4)
  {
    temp += (((S32) w[i])*((S32) x[i]));
    temp += (((S32) w[i+1])*((S32) x[i+1]));
    temp += (((S32) w[i+2])*((S32) x[i+2]));
    temp += (((S32) w[i+3])*((S32) x[i+3]));
  }
  *y = temp >> (DSP16_QB);

  // Error calculation
  *e = d - *y;

  // Update the normalization coefficient
  normalization += ((((S32) new_x)*((S32) new_x))) >> (DSP16_QB);


  if (!normalization)
    normalization = 1;

#if !(DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
  temp = ((S32) *e << DSP16_QB)/normalization;
  if (temp > 0x7FFF)
    temp = 0x7FFF;
  else if (temp < -0x8000)
    temp = -0x8000;
#endif

  // Refresh the w coefficients
  for(i=0; i<size; i+=4)
  {
#if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
    w[i] += ((((((S32) *e)*((S32) x[i])))/normalization) >> (DSP_NLMS_MU - 1));
    w[i+1] += ((((((S32) *e)*((S32) x[i+1])))/normalization) >> (DSP_NLMS_MU - 1));
    w[i+2] += ((((((S32) *e)*((S32) x[i+2])))/normalization) >> (DSP_NLMS_MU - 1));
    w[i+3] += ((((((S32) *e)*((S32) x[i+3])))/normalization) >> (DSP_NLMS_MU - 1));
#else
    w[i] += (temp*((S32) x[i])) >> (DSP_NLMS_MU - 1 + DSP16_QB);
    w[i+1] += (temp*((S32) x[i+1])) >> (DSP_NLMS_MU - 1 + DSP16_QB);
    w[i+2] += (temp*((S32) x[i+2])) >> (DSP_NLMS_MU - 1 + DSP16_QB);
    w[i+3] += (temp*((S32) x[i+3])) >> (DSP_NLMS_MU - 1 + DSP16_QB);
#endif
  }

  // Update the normalization coefficient
  normalization -= ((((S32) x[size-1])*((S32) x[size-1]))) >> (DSP16_QB);

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

