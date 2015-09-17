/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit fixed-point exponential functions or the AVR32 UC3
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "dsp.h"
#include "preprocessor.h"

#define A 1.
#define B 0.5
#define C 0.16666666666666666666666666666667
#define D 0.041666666666666666666666666666667
#define E 0.0083333333333333333333333333333333

/*
 * These defines correspond to the limit values
 * before which the result saturate.
 * It has been calculate by the following formula:
 * log(2^(QA-1))
 * Where QA is the
 */
#define DSP_EXP_0_LIMIT_VAL   -0.69314718056
#define DSP_EXP_1_LIMIT_VAL   0.
#define DSP_EXP_2_LIMIT_VAL   0.69314718056
#define DSP_EXP_3_LIMIT_VAL   1.38629436112
#define DSP_EXP_4_LIMIT_VAL   2.07944154168
#define DSP_EXP_5_LIMIT_VAL   2.77258872224
#define DSP_EXP_6_LIMIT_VAL   3.46573590280
#define DSP_EXP_7_LIMIT_VAL   4.15888308336
#define DSP_EXP_8_LIMIT_VAL   4.85203026392
#define DSP_EXP_9_LIMIT_VAL   5.54517744448
#define DSP_EXP_10_LIMIT_VAL  6.23832462504
#define DSP_EXP_11_LIMIT_VAL  6.93147180560
#define DSP_EXP_12_LIMIT_VAL  7.62461898616
#define DSP_EXP_13_LIMIT_VAL  8.31776616672
#define DSP_EXP_14_LIMIT_VAL  9.01091334728
#define DSP_EXP_15_LIMIT_VAL  9.70406052784
#define DSP_EXP_16_LIMIT_VAL  10.39720770840
#define DSP_EXP_17_LIMIT_VAL  11.09035488896
#define DSP_EXP_18_LIMIT_VAL  11.78350206952
#define DSP_EXP_19_LIMIT_VAL  12.47664925008
#define DSP_EXP_20_LIMIT_VAL  13.16979643064
#define DSP_EXP_21_LIMIT_VAL  13.86294361120
#define DSP_EXP_22_LIMIT_VAL  14.55609079176
#define DSP_EXP_23_LIMIT_VAL  15.24923797232
#define DSP_EXP_24_LIMIT_VAL  15.94238515288
#define DSP_EXP_25_LIMIT_VAL  16.63553233344
#define DSP_EXP_26_LIMIT_VAL  17.32867951400
#define DSP_EXP_27_LIMIT_VAL  18.02182669456
#define DSP_EXP_28_LIMIT_VAL  18.71497387512
#define DSP_EXP_29_LIMIT_VAL  19.40812105568
#define DSP_EXP_30_LIMIT_VAL  20.10126823624
#define DSP_EXP_31_LIMIT_VAL  20.79441541680
#define DSP_EXP_32_LIMIT_VAL  21.48756259736

// Limit value before saturation for 16-bit fixed-point type
#define DSP16_EXP_LIMIT_VAL   DSP16_Q(ATPASTE3(DSP_EXP_, DSP16_QA, _LIMIT_VAL) - DSP_FP_RES(DSP16_QA, DSP16_QB))
// Limit value before saturation for 32-bit fixed-point type
#define DSP32_EXP_LIMIT_VAL   DSP32_Q(ATPASTE3(DSP_EXP_, DSP32_QA, _LIMIT_VAL) - DSP_FP_RES(DSP32_QA, DSP32_QB))

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP16_EXP) || \
    !defined(TARGET_SPECIFIC_OP16_EXP)

// exp(num) = exp(a + b*log(2)/2) = exp(a)*2^b/2
dsp16_t dsp16_op_exp(dsp16_t num)
{
  int k;
  S32 x, res;

  if (num > DSP16_EXP_LIMIT_VAL)
    return DSP_Q_MAX(DSP16_QA, DSP16_QB);

  // k = floor(res/((log(2)*0.5)));
  x = (S32) DSP_Q(32-DSP16_QB, DSP16_QB, 2./CST_LN_2) >> 1;
  k = ((((S32) num)*x)) >> (2*DSP16_QB - 1);

  // x = res - (k*log(2)*0.5);
  x = DSP16_Q(CST_LN_2*0.5);
  x = ((S32) num) - x*((S32) k);

  // res = 1 + x + x*x/2 + x*x*x/6 + x*x*x*x/24;
  res = (x*((S32) DSP16_Q(C))) >> DSP16_QB;
  res = (x*(res + ((S32) DSP16_Q(B)))) >> DSP16_QB;
  res = (x*(res + ((S32) DSP16_Q(A)))) >> DSP16_QB;
  res += ((S32) DSP16_Q(1.));

  if (k > 0)
  {
    if (k&1)
      res = ((res << (k >> 1))*DSP16_Q(CST_SQUARE_ROOT_2)) >> DSP16_QB;
    else
      res <<= (k >> 1);
  }
  else
  {
    if (k&1)
      res = ((res >> (dsp16_op_abs(k) >> 1))*DSP16_Q(CST_INV_SQUARE_ROOT_2)) >> DSP16_QB;
    else
      res >>= (dsp16_op_abs(k) >> 1);
  }

  return res;
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_EXP) || \
    !defined(TARGET_SPECIFIC_OP32_EXP)

dsp32_t dsp32_op_exp(dsp32_t num)
{
  int k;
  S64 x, res;

  if (num > DSP32_EXP_LIMIT_VAL)
    return DSP_Q_MAX(DSP32_QA, DSP32_QB);

  // k = floor(res/((log(2)*0.5)));
  x = (S64) ((2LL/CST_LN_2)*(1LL << (DSP32_QB -1 )));
  k = ((((S64) num)*x)) >> (2*DSP32_QB - 1);

  // x = res - (k*log(2)*0.5);
  x = DSP32_Q(CST_LN_2*0.5);
  x = ((S64) num) - x*((S64) k);

  // res = 1 + x + x*x/2 + x*x*x/6 + x*x*x*x/24 + x*x*x*x*x/120;
  res = (x*((S32) DSP32_Q(E))) >> DSP32_QB;
  res = (x*(res + ((S64) DSP32_Q(D)))) >> DSP32_QB;
  res = (x*(res + ((S64) DSP32_Q(C)))) >> DSP32_QB;
  res = (x*(res + ((S64) DSP32_Q(B)))) >> DSP32_QB;
  res = (x*(res + ((S64) DSP32_Q(A)))) >> DSP32_QB;
  res += ((S64) DSP32_Q(1.));

  if (k > 0)
  {
    if (k&1)
      res = ((res << (k >> 1))*DSP32_Q(CST_SQUARE_ROOT_2)) >> DSP32_QB;
    else
      res <<= (k >> 1);
  }
  else
  {
    if (k&1)
      res = ((res >> (dsp32_op_abs(k) >> 1))*DSP32_Q(CST_INV_SQUARE_ROOT_2)) >> DSP32_QB;
    else
      res >>= (dsp32_op_abs(k) >> 1);
  }

  return res;
}

#endif
