/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 32-bit IIR filter function.
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
    defined(FORCE_GENERIC_FILT32_IIR) || \
    !defined(TARGET_SPECIFIC_FILT32_IIR)

// &x[-num_size+1] size: size + num_size - 1
// &y[-den_size+1] size: size + den_size - 1
void dsp32_filt_iir(dsp32_t *y, dsp32_t *x, int size, dsp32_t *num, int num_size, dsp32_t *den, int den_size, int num_prediv, int den_prediv)
{
  int n, m;
  S64 sum1, sum2;
  dsp32_t *px, *py;

  num_prediv = DSP32_QB - num_prediv;
  den_prediv = DSP32_QB - den_prediv;

  for(n=0; n<size; n++)
  {
    sum1 = 0;
    sum2 = 0;
    px = &x[n];
    for(m=0; m<num_size; m++)
      sum1 += ((S64) num[m] * (S64) px[-m]);
    py = &y[n-1];
    for(m=0; m<den_size; m++)
      sum2 += ((S64) den[m] * (S64) py[-m]);

    sum1 >>= num_prediv;
    sum2 >>= den_prediv;

    y[n] = sum1 - sum2;
  }
}

#endif
