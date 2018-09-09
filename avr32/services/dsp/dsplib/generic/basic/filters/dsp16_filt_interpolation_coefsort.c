/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit coefficients sorting function for the interpolation
 * filter.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
