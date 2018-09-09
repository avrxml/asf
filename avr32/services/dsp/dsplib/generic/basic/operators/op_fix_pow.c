/*****************************************************************************
 *
 * \file
 *
 * \brief Power operator for the AVR32 UC3.
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
