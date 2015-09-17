/*****************************************************************************
 *
 * \file
 *
 * \brief Natural logarithm operator for the AVR32 UC3.
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
    defined(FORCE_GENERIC_OP16_LOG2) || \
    !defined(TARGET_SPECIFIC_OP16_LOG2)

dsp16_t dsp16_op_log2(dsp16_t number)
{
  S32 res;
  const S32 cst_ln2 = (S32) DSP_Q(32-DSP16_QB, DSP16_QB, 1./CST_LN_2);
  extern S32 dsp16_op_ln_raw(dsp16_t num);

  res = dsp16_op_ln_raw(number);

  if (res < ((S32) 0x80000000)/cst_ln2)
    return DSP_Q_MIN(DSP16_QA, DSP16_QB);

  res = (res*cst_ln2) >> DSP16_QB;

  if (res < ((S32) DSP_Q_MIN(DSP16_QA, DSP16_QB)))
    return (dsp16_t) DSP_Q_MIN(DSP16_QA, DSP16_QB);

  return (dsp16_t) res;
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_LOG2) || \
    !defined(TARGET_SPECIFIC_OP32_LOG2)

dsp32_t dsp32_op_log2(dsp32_t number)
{
  S64 res;
  const S64 cst_ln2 = (S64) ((1./CST_LN_2)*(1LL << DSP32_QB));
  extern S64 dsp32_op_ln_raw(dsp32_t num);

  res = dsp32_op_ln_raw(number);

  if (res < ((S64) -0x8000000000000000LL)/cst_ln2)
    return DSP_Q_MIN(DSP32_QA, DSP32_QB);

  res = (res*cst_ln2) >> DSP32_QB;

  if (res < ((S64) DSP_Q_MIN(DSP32_QA, DSP32_QB)))
    return (dsp32_t) DSP_Q_MIN(DSP32_QA, DSP32_QB);

  return (dsp32_t) res;
}

#endif
