/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit ramp signal generator functions for the AVR32 UC3
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include "dsp.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN16_RAMP) || \
    !defined(TARGET_SPECIFIC_GEN16_RAMP)

// y = (0:L-1)*m + b
void dsp16_gen_ramp(dsp16_t *vect1, int size, dsp16_t increment)
{
  int i = 0;
  dsp16_t t, limit_t;

  // Limit of the signal
  limit_t = DSP16_Q(1.) - increment;
  if ((((S32) increment) * ((S32) size)) < ((S32) limit_t))
    limit_t = increment * size;

  // Ramp loop
  for(t = DSP16_Q(0.); t < limit_t; t += increment)
    vect1[i++] = t;
  // Filling loop
  for(; i<size; i++)
    vect1[i] = DSP16_Q(1.);
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_RAMP) || \
    !defined(TARGET_SPECIFIC_GEN32_RAMP)

void dsp32_gen_ramp(dsp32_t *vect1, int size, dsp32_t increment)
{
  int i = 0;
  dsp32_t t, limit_t;

  // Limit of the signal
  limit_t = DSP32_Q(1.) - increment;
  if ((((S64) increment) * ((S64) size)) < ((S64) limit_t))
    limit_t = increment * size;

  // Ramp loop
  for(t = DSP32_Q(0.); t < limit_t; t += increment)
    vect1[i++] = t;
  // Filling loop
  for(; i<size; i++)
    vect1[i] = DSP32_Q(1.);
}

#endif
