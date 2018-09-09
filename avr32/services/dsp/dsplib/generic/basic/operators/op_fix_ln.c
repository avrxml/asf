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

dsp16_t dsp16_op_ln(dsp16_t number)
{
  S32 res;
  extern S32 dsp16_op_ln_raw(dsp16_t num);

  res = dsp16_op_ln_raw(number);

  if (res < ((S32) DSP_Q_MIN(DSP16_QA, DSP16_QB)))
    return DSP_Q_MIN(DSP16_QA, DSP16_QB);

  return (dsp16_t) res;
}

dsp32_t dsp32_op_ln(dsp32_t number)
{
  S64 res;
  extern S64 dsp32_op_ln_raw(dsp32_t num);

  res = dsp32_op_ln_raw(number);

  if (res < ((S64) DSP_Q_MIN(DSP32_QA, DSP32_QB)))
    return DSP_Q_MIN(DSP32_QA, DSP32_QB);

  return (dsp32_t) res;
}
