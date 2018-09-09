/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit lowpass FIR filter design function.
 *
 * This file contains the code of the lowpass FIR filter design.
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
    defined(FORCE_GENERIC_FILT16_LPFIRDESIGN) || \
    !defined(TARGET_SPECIFIC_FILT16_LPFIRDESIGN)

#ifndef Max
# define Max(a, b)           (((a) > (b)) ?  (a) : (b))
#endif

#define DSP16_MUL_TO_DSP32(num1, num2) \
  ((DSP32_QB > DSP16_QB*2)? \
  ((((S32) (num1)) * ((S32) (num2))) << (DSP32_QB - DSP16_QB*2)) \
  :((((S32) (num1)) * ((S32) (num2))) >> (DSP16_QB*2 - DSP32_QB)))

#define DSP32_DIV_TO_DSP16(num1, num2) \
  ((DSP32_QB > DSP16_QB*2)? \
  (((num1) >> (DSP32_QB - DSP16_QB*2)) / ((S32) (num2))) \
  :(((num1) << (DSP16_QB*2 - DSP32_QB)) / ((S32) (num2))))

void dsp16_filt_lpfirdesign(dsp16_t *c, int fc, int fs, int order)
{
  S32 n_fc;
  S32 s32_temp1;
  S32 s32_temp2;
  dsp16_t dsp16_temp;
  int num, i;
  dsp32_t dsp32_temp;

  if (!order)
    return;

  // Calculate normalized cutoff frequency multiplied by 2
  n_fc = (((S32) fc) << (DSP16_QB + 1)) / fs;
  num = order / 2 + 1;

  // Fills the buffer with ones
  for(i=0; i<order; i++)
    c[i] = DSP16_Q(1.);

  c[0] = dsp16_op_mul(c[0], n_fc);
  for(i=1; i<order; i++)
  {
    s32_temp1 = (S32) DSP_Q(32 - DSP16_QB, DSP16_QB, (double) i + 1.) / num;
    s32_temp2 = (S32) DSP_Q(32 - DSP16_QB, DSP16_QB, CST_PI * ((double) i + 1.)) / num;
    dsp32_temp = dsp32_op_sin(DSP16_MUL_TO_DSP32(s32_temp1, n_fc));
    dsp16_temp = DSP32_DIV_TO_DSP16(dsp32_temp, s32_temp2);
    c[i] = dsp16_op_mul(c[i], dsp16_temp);
  }
}

/** Scilab algorithm:
 *
 * k is chosen so that coef(order/2+1) equals 1.
 *
 * fc = f_hz / fs_hz;
 * for i=0:(order-1),
 *   if (i - order/2) <> 0,
 *     cst = sin(2 * %pi * fc * (i - order/2)) / (i - order/2);
 *     coef(i+1) = k * cst * (0.42 - 0.5 * cos(2 * %pi * i / order) + 0.08 * cos(4 * %pi * i / order));
 *   end
 * end
 * coef(order/2+1) = k * 2 * %pi * fc;
 */
void dsp16_filt_lpfirdesign_windowed_sinc(dsp16_t *c, int fc_hz, int fs_hz, int order)
{
  dsp16_t fc;
  int i;
  dsp16_t cst1, cst2;
  S32 k;

  // Cut-off frequency calculation
  fc = (((S32) fc_hz) << DSP16_QB) / fs_hz;
  // Constant k
  k = (DSP16_Q(1.) / ((2 * fc * (S32) DSP_Q(32 - DSP16_QB, DSP16_QB, CST_PI)) >> DSP16_QB));

  for (i=0; i < order; i++)
  {
    if (i == order / 2)
      continue;
    cst1 = dsp16_op_sin(2 * fc * (i - order / 2)) / (i - order / 2);
    cst2 = 2 * (((S32) i) << DSP16_QB) / order;
    cst2 = DSP16_Q(0.42) - dsp16_op_mul(DSP16_Q(0.5), dsp16_op_cos(cst2)) + dsp16_op_mul(DSP16_Q(0.08), dsp16_op_cos(2 * cst2));
    c[i] = dsp16_op_mul(cst1, cst2);
    c[i] = k * c[i];
  }
  c[order / 2] = k * ((2 * fc * (S32) DSP_Q(32 - DSP16_QB, DSP16_QB, CST_PI)) >> DSP16_QB);
}

#endif
