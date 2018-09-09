/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit noise generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_NOISE) || \
    !defined(TARGET_SPECIFIC_GEN16_NOISE)

// rand()*amp
void dsp16_gen_noise(dsp16_t *vect1, int size, dsp16_t amp)
{
  int t;

  // If amp == 1. then the signal is much faster to compute
  if (amp == DSP16_Q(1.))
  {
    // Main loop
    for(t=0; t<size; t++)
      vect1[t] = dsp16_op_rand();
  }
  else
  {
    // Main loop
    for(t=0; t<size; t++)
      // Get a pseudo-random sample and scale it with the desired amplitude
      vect1[t] = ((S32) dsp16_op_rand())*((S32) amp) >> DSP16_QB;
  }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_NOISE) || \
    !defined(TARGET_SPECIFIC_GEN32_NOISE)

// rand()*amp
void dsp32_gen_noise(dsp32_t *vect1, int size, dsp32_t amp)
{
  int t;

  // If amp == 1. then the signal is much faster to compute
  if (amp == DSP32_Q(1.))
  {
    // Main loop
    for(t=0; t<size; t++)
      vect1[t] = dsp32_op_rand();
  }
  else
  {
    // Main loop
    for(t=0; t<size; t++)
      // Get a pseudo-random sample and scale it with the desired amplitude
      vect1[t] = ((S64) dsp32_op_rand())*((S64) amp) >> DSP32_QB;
  }
}

#endif
