/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit NLMS filter function.
 *
 * This file contains the code of the NLMS filter.
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

