/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit IIR filter function.
 *
 * This file contains the code of the IIR filter.
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
