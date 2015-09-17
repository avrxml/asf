/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit LMS filter function optimized for the AVR32 UC3.
 *
 * This file contains the code of the LMS filter.
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
    !defined(FORCE_GENERIC_FILT32_LMS) && \
    defined(TARGET_SPECIFIC_FILT32_LMS)

#include "filt_lms.h"

void dsp32_filt_lms(dsp32_t *x, dsp32_t *w, int size, dsp32_t new_x, dsp32_t d, dsp32_t *y, dsp32_t *e)
{
  int i = 0;

  x[0] = new_x;

/*  // Performed a FIR
  __asm__ __volatile__ (
    "mov      %3, 0\n\t"
    "mov      r4, %3\n\t"
    "mov      r5, %3\n\t"
    "cp.w     %4, 0\n\t"
    "brle     _filt_nlms_end_fir\n"

    "_filt_nlms_loop_fir:\n\t"

    "ld.d     r0, %1[%3 << 2]\n\t"
    "ld.d     r2, %2[%3 << 2]\n\t"
    "macs.d   r4, r0, r2\n\t"
    "macs.d   r4, r1, r3\n\t"

    "sub      %3, -2\n\t"

    "ld.d     r0, %1[%3 << 2]\n\t"
    "ld.d     r2, %2[%3 << 2]\n\t"
    "macs.d   r4, r0, r2\n\t"
    "macs.d   r4, r1, r3\n\t"

    "sub      %3, -2\n\t"
    "cp.w     %4, %3\n\t"
    "brgt     _filt_nlms_loop_fir\n"

    "_filt_nlms_end_fir:\n\t"

    "lsr      %0, r4, "ASTRINGZ(DSP32_QB)"\n\t"
    "or       %0, %0, r5 << "ASTRINGZ(DSP32_QA)"\n\t"

    : "=r" (*y)
    : "r" (w), "r" (x), "r" (i), "r" (size), "0" (*y)
    : "r0", "r1", "r2", "r3", "r4", "r5", "r8"
  );*/

  dsp32_filt_lms_fir(x, w, size, y, i);

  // Error calculation
  *e = d - *y;

  // Refresh the w coefficients
  for(i=0; i<size; i+=4)
  {
    w[i] += ((((S64) *e)*((S64) x[i]))) >> (DSP_LMS_MU - 1 + DSP32_QB);
    w[i+1] += ((((S64) *e)*((S64) x[i+1]))) >> (DSP_LMS_MU - 1 + DSP32_QB);
    w[i+2] += ((((S64) *e)*((S64) x[i+2]))) >> (DSP_LMS_MU - 1 + DSP32_QB);
    w[i+3] += ((((S64) *e)*((S64) x[i+3]))) >> (DSP_LMS_MU - 1 + DSP32_QB);
  }

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
