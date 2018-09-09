/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit rectangular signal generator functions for the AVR32 UC3
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
    defined(FORCE_GENERIC_GEN16_RECT) || \
    !defined(TARGET_SPECIFIC_GEN16_RECT)

void dsp16_gen_rect(dsp16_t *vect1, int size, int f, int fs, dsp16_t duty, dsp16_t delay)
{
  int i = 0;
  int t_low_inc, t_high_inc, cp;
  int t_low, t_high, t_delay;

  // Number of sample per period
  t_low_inc = fs / f;
  // Number of sample per high signal
  t_high_inc = (((S32) duty) * ((S32) t_low_inc)) >> DSP16_QB;
  // Number of sample for the delay
  t_delay = (((S32) delay) * ((S32) t_low_inc)) >> DSP16_QB;
  // Number of sample per low signal
  t_low_inc -= t_high_inc;

  i = 0;

  // For the delay
  t_high = MIN(t_high_inc, t_delay - t_low_inc);
  t_low = MIN(t_low_inc, t_delay);
  if (t_high > 0)
    t_low += t_high;

  // Main loop
  while(i < size)
  {
    // High part
    cp = MIN(t_high, size);
    for(; i<cp; i++)
      vect1[i] = DSP16_Q(1.);

    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP16_Q(-1.);

    t_high = t_high_inc + i;
    t_low = t_high + t_low_inc;
  }

}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_GEN32_RECT) || \
    !defined(TARGET_SPECIFIC_GEN32_RECT)

void dsp32_gen_rect(dsp32_t *vect1, int size, int f, int fs, dsp32_t duty, dsp32_t delay)
{
  int i = 0;
  int t_low_inc, t_high_inc, cp;
  int t_low, t_high, t_delay;

  // Number of sample per period
  t_low_inc = fs / f;
  // Number of sample per high signal
  t_high_inc = (((S64) duty) * ((S64) t_low_inc)) >> DSP32_QB;
  // Number of sample for the delay
  t_delay = (((S64) delay) * ((S64) t_low_inc)) >> DSP32_QB;
  // Number of sample per low signal
  t_low_inc -= t_high_inc;

  i = 0;

  // For the delay
  t_high = MIN(t_high_inc, t_delay - t_low_inc);
  t_low = MIN(t_low_inc, t_delay);
  if (t_high > 0)
    t_low += t_high;

  // Main loop
  while(i < size)
  {
    // High part
    cp = MIN(t_high, size);
    for(; i<cp; i++)
      vect1[i] = DSP32_Q(1.);

    // Low part
    cp = MIN(t_low, size);
    for(; i<cp; i++)
      vect1[i] = DSP32_Q(-1.);

    t_high = t_high_inc + i;
    t_low = t_high + t_low_inc;
  }

}

#endif
