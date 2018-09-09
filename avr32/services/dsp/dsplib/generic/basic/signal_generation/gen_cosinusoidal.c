/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit cosinusoidal signal generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_COS) || \
    !defined(TARGET_SPECIFIC_GEN16_COS)

// cos(2*pi*f/fs*t + phase)
dsp16_t dsp16_gen_cos(dsp16_t *vect1, int size, int f, int fs, dsp16_t phase)
{
  int t;
  dsp16_t s, angle;

  angle = phase;
  // Calculate the angle "s" which is used to increment the argument of the cosine each transition in the loop
  s = (((S32) f) << (DSP16_QB + 1))/fs;

  // Main loop
  for(t=0; t<size; t++)
  {
    vect1[t] = dsp16_op_cos(angle);
    // Increment the argument of the cosine
    angle += s;
  }

  // Return the current phase of the generated signal
  return angle;
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_COS) || \
    !defined(TARGET_SPECIFIC_GEN32_COS)

// cos(2*pi*f/fs*t + phase)
dsp32_t dsp32_gen_cos(dsp32_t *vect1, int size, int f, int fs, dsp32_t phase)
{
  int t;
  dsp32_t s, angle;

  angle = phase;
  // Calculate the angle "s" which is used to increment the argument of the cosine each transition in the loop
  s = (((long long) f) << (DSP32_QB + 1))/fs;

  // Main loop
  for(t=0; t<size; t++)
  {
    vect1[t] = dsp32_op_cos(angle);
    // Increment the argument of the cosine
    angle += s;
  }

  // Return the current phase of the generated signal
  return angle;
}

#endif

