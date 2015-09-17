/*****************************************************************************
 *
 * \file
 *
 * \brief Power operator for the AVR32 UC3.
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
    defined(FORCE_GENERIC_OP16_POW) || \
    !defined(TARGET_SPECIFIC_OP16_POW)

dsp16_t dsp16_op_pow(dsp16_t x, dsp16_t y)
{
  S32 res, temp;
  int k;
  extern S32 dsp16_op_ln_raw(dsp16_t num);

  if (x < DSP16_Q(0.))
    return (dsp16_t) DSP_Q_MIN(DSP16_QA, DSP16_QB);

  res = dsp16_op_ln_raw(x) >> (DSP16_QB >> 1);

  res = (res*((S32) y)) >> ((DSP16_QB&1)?(DSP16_QB >> 1)+1:(DSP16_QB >> 1));

  // In order to prevent overflows ...
  if (res < ((S32) DSP_Q_MIN(DSP16_QA, DSP16_QB)))
  {
    // k = floor(res/log(2));
    temp = res >> (DSP16_QB - 2);
    k = (S32) DSP_Q(32-DSP16_QB, DSP16_QB, 1./CST_LN_2) >> 2;
    k = ((temp*k) >> (DSP16_QB)) + 1;

    // temp = res - (k*log(2));
    temp = DSP16_Q(CST_LN_2);
    // temp is in the range ]-ln(2), 0]
    temp = ((S32) res) - temp*((S32) k);

    res = dsp16_op_exp((dsp16_t) temp);

    res >>= dsp16_op_abs(k);

    return (dsp16_t) res;
  }

  if (res > ((S32) DSP_Q_MAX(DSP16_QA, DSP16_QB)))
    res = DSP_Q_MAX(DSP16_QA, DSP16_QB);

  return (dsp16_t) dsp16_op_exp(res);
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_POW) || \
    !defined(TARGET_SPECIFIC_OP32_POW)

dsp32_t dsp32_op_pow(dsp32_t x, dsp32_t y)
{
  S64 res, temp;
  int k;
  extern S64 dsp32_op_ln_raw(dsp32_t num);

  if (x < DSP32_Q(0.))
    return (dsp32_t) DSP_Q_MIN(DSP32_QA, DSP32_QB);

  res = dsp32_op_ln_raw(x) >> (DSP32_QB >> 1);

  res = (res*((S64) y)) >> ((DSP32_QB&1)?(DSP32_QB >> 1)+1:(DSP32_QB >> 1));

  // In order to prevent overflows ...
  if (res < ((S64) DSP_Q_MIN(DSP32_QA, DSP32_QB)))
  {
    // k = floor(res/log(2));
    temp = res >> (DSP32_QB - 2);
    k = (S64) ((1LL/CST_LN_2)*(1LL << (DSP32_QB - 2 )));
    k = ((temp*k) >> (DSP32_QB)) + 1;

    // temp = res - (k*log(2));
    temp = DSP32_Q(CST_LN_2);
    // temp is in the range ]-ln(2), 0]
    temp = ((S64) res) - temp*((S64) k);

    res = dsp32_op_exp((dsp32_t) temp);

    res >>= dsp32_op_abs(k);

    return (dsp32_t) res;
  }

  if (res > ((S64) DSP_Q_MAX(DSP32_QA, DSP32_QB)))
    res = DSP_Q_MAX(DSP32_QA, DSP32_QB);

  return (dsp32_t) dsp32_op_exp(res);
}

#endif
