/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit coefficients sorting function for the interpolation
 * filter.
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
    defined(FORCE_GENERIC_FILT16_INTERPOLATION_COEFSORT) || \
    !defined(TARGET_SPECIFIC_FILT16_INTERPOLATION_COEFSORT)

inline static int get_new_index(int cur_index, int n_tap, int interpolation_ratio)
{
  return n_tap * cur_index + (1 - n_tap * interpolation_ratio) * (cur_index / interpolation_ratio);
}

void dsp16_filt_interpolation_coefsort(dsp16_t *fir_coefs, int n_tap, int interpolation_ratio)
{
  int start_index, new_index, cur_index;
  int temp1, temp2;
  int size;

  size = n_tap * interpolation_ratio;

  // Uses the LSB of each data as a marker.
  // This will avoid a lot of computation time
  // or memory requirement.
  for (temp1=1; temp1<size-1; temp1++)
    fir_coefs[temp1] &= 0xFFFE;

  for (start_index = 1; start_index < size - 1; start_index++)
  {
    // If marker set, means item already proceeded.
    if (fir_coefs[start_index] & 0x0001)
      continue;

    new_index = start_index + 1;
    cur_index = start_index;
    temp1 = fir_coefs[cur_index];

    while (start_index != new_index)
    {
      new_index = get_new_index(cur_index, n_tap, interpolation_ratio);
      temp2 = fir_coefs[new_index];
      fir_coefs[new_index] = temp1 | 0x0001;
      temp1 = temp2;
      cur_index = new_index;
    }
  }
}

#endif
