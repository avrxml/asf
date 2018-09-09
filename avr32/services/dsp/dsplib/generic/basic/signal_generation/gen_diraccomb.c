/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit dirac comb signal generator functions for the AVR32 UC3
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

#ifndef MIN
# define MIN(x, y)  (((x) < (y))?(x):(y))
#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN16_DCOMB) || \
    !defined(TARGET_SPECIFIC_GEN16_DCOMB)

void dsp16_gen_dcomb(dsp16_t *vect1, int size, int f, int fs, dsp16_t delay)
{
  int i;
  int t_inc, t_low, cp;
  int t_delay;

  // Number of sample per period
  t_inc = fs / f;
  // Number of sample for the delay
  t_delay = (((S32) delay) * ((S32) t_inc)) >> DSP16_QB;

  // Number of sample per low signal
  t_inc--;

  i = 0;
  t_low = t_delay;

  // Main loop
  while(i < size)
  {
    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP16_Q(0.);
    // High part
    if (i < size)
      vect1[i++] = DSP16_Q(1.);

    t_low = t_inc + i;
  }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_DCOMB) || \
    !defined(TARGET_SPECIFIC_GEN32_DCOMB)

void dsp32_gen_dcomb(dsp32_t *vect1, int size, int f, int fs, dsp32_t delay)
{
  int i;
  int t_inc, t_low, cp;
  int t_delay;

  // Number of sample per period
  t_inc = fs / f;
  // Number of sample for the delay
  t_delay = (((S64) delay) * ((S64) t_inc)) >> DSP32_QB;

  // Number of sample per low signal
  t_inc--;

  i = 0;
  t_low = t_delay;

  // Main loop
  while(i < size)
  {
    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP32_Q(0.);
    // High part
    if (i < size)
      vect1[i++] = DSP32_Q(1.);

    t_low = t_inc + i;
  }
}

#endif
