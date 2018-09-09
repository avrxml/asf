/*****************************************************************************
 *
 * \file
 *
 * \brief Natural logarithm operator for the AVR32 UC3.
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
