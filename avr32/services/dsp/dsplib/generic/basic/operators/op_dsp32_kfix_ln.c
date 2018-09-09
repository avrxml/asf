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
#include "preprocessor.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_LN) || \
    !defined(TARGET_SPECIFIC_OP32_LN)

S64 dsp32_op_ln_raw(dsp32_t num)
{
  dsp32_t num_temp;
  S64 a, a_sqr, res;
  int under_bit_val;

  // Limit
  if (num <= DSP32_Q(0.))
    return DSP_Q_MIN(DSP32_QA, DSP32_QB);

// Remove warnings
#if DSP32_QA > 1
  // get ~ int(log2(num))
  // num is in the range [1; 0.5]
  if (num > DSP32_Q(1.))
  {
    under_bit_val = 0;
    num_temp = num;
    while (num_temp > DSP32_Q(1.))
    {
      num_temp >>= 1;
      under_bit_val++;
    }
    num_temp = (num >> under_bit_val);
  }
  else
#endif
  {
    under_bit_val = 0;
    num_temp = num;
    while (num_temp < DSP32_Q(0.5))
    {
      num_temp <<= 1;
      under_bit_val++;
    }
    num_temp = (num << under_bit_val);
  }

  // << (DSP32_QB - 1) to avoid overflows
  a = (((S64) num_temp) - DSP32_Q(1.)) << DSP32_QB;
  a = a/(((S64) num_temp) + DSP32_Q(1.));
  // calculate a^2
  a_sqr = (a*a) >> DSP32_QB;

  res = (a_sqr*DSP32_Q(1./13.)) >> DSP32_QB;
  res = (a_sqr*(res + DSP32_Q(1./11.))) >> DSP32_QB;
  res = (a_sqr*(res + DSP32_Q(1./9.))) >> DSP32_QB;
  res = (a_sqr*(res + DSP32_Q(1./7.))) >> DSP32_QB;
  res = (a_sqr*(res + DSP32_Q(1./5.))) >> DSP32_QB;
  res = (a_sqr*(res + DSP32_Q(1./3.))) >> DSP32_QB;
  res = (a*(res + DSP32_Q(1.))) >> (DSP32_QB - 1);

// Remove warnings
#if DSP32_QA > 1
  if (num > DSP32_Q(1.))
    res += ((S64) under_bit_val)*((S64) DSP32_Q(CST_LN_2));
  else
#endif
    res -= ((S64) under_bit_val)*((S64) DSP32_Q(CST_LN_2));

  return res;
}

#endif
