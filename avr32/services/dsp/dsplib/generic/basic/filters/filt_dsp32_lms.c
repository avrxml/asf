/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 32-bit LMS filter function.
 *
 * This file contains the code of the LMS filter.
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
    defined(FORCE_GENERIC_FILT32_LMS) || \
    !defined(TARGET_SPECIFIC_FILT32_LMS)

#include "filt_lms.h"

void dsp32_filt_lms(dsp32_t *x, dsp32_t *w, int size, dsp32_t new_x, dsp32_t d, dsp32_t *y, dsp32_t *e)
{
  int i;
  S64 temp;

  x[0] = new_x;

  temp = 0;
  // Performed a FIR
  for(i=0; i<size; i+=4)
  {
    temp += (((S64) w[i])*((S64) x[i]));
    temp += (((S64) w[i+1])*((S64) x[i+1]));
    temp += (((S64) w[i+2])*((S64) x[i+2]));
    temp += (((S64) w[i+3])*((S64) x[i+4]));
  }
  *y = temp >> (DSP32_QB);

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
