/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit fixed-point arc sine functions or the AVR32 UC3
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "dsp.h"
                                                 // CST = %pi/2;
#define A   0.63661977236758134307553505349036   // 1/CST
#define B   0.10610329539459689051258917558125   // (1/2)/3/CST
#define C   0.047746482927568600730665129011754  // (1*3)/(2*4)/5/CST
#define D   0.028420525552124167101586386315975  // (1*3*5)/(2*4*6)/7/CST
#define E   0.019341746556306724833024068465127  // (1*3*5*7)/(2*4*6*8)/9/CST
#define F   0.01424255882782586101340863223362   // (1*3*5*7*9)/(2*4*6*8*10)/11/CST

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP16_ASIN) || \
    !defined(TARGET_SPECIFIC_OP16_ASIN)

/*
 *  Output range = [-1; 1] corresponding to [-pi; pi]
 */
dsp16_t dsp16_op_asin(dsp16_t num)
{
  S32 num_sqr, res;
  dsp16_t num_abs;

  num_abs = dsp16_op_abs(num);

#if DSP16_QB < 15
  // Limits
  if (((S32) num) >= ((S32) DSP16_Q(1.)))
    return DSP16_Q(0.5);
  if (((S32) num) <= ((S32) DSP16_Q(-1.)))
    return DSP16_Q(-0.5);
#endif

  // If |num| > 0.5
  if (num_abs > DSP16_Q(0.5))
  {
    num_abs = dsp16_op_sqrt((DSP16_Q(1.)-num_abs) >> 1);
    num_abs = DSP16_Q(0.5) - (dsp16_op_asin(num_abs) << 1);
    return (num < 0)?-num_abs:num_abs;
  }
  else
  {
    num_sqr = (((S32) num)*((S32) num)) >> DSP16_QB;
    res = (num_sqr*DSP16_Q(D)) >> DSP16_QB;
    res = (num_sqr*(res + DSP16_Q(C))) >> DSP16_QB;
    res = (num_sqr*(res + DSP16_Q(B))) >> DSP16_QB;
    num = (((S32) num)*(res + DSP16_Q(A))) >> (DSP16_QB + 1);

    return num;
  }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_ASIN) || \
    !defined(TARGET_SPECIFIC_OP32_ASIN)

/*
 *  Output range = [-1; 1] corresponding to [-pi; pi]
 */
dsp32_t dsp32_op_asin(dsp32_t num)
{
  S64 num_sqr, res;
  dsp32_t num_abs;

  num_abs = dsp32_op_abs(num);

#if DSP32_QB < 31
  // Limits
  if (((S64) num) >= ((S64) DSP32_Q(1.)))
    return DSP32_Q(0.5);
  if (((S64) num) <= ((S64) DSP32_Q(-1.)))
    return DSP32_Q(-0.5);
#endif

  // If |num| > 0.5
  if (num_abs > DSP32_Q(0.5))
  {
    num_abs = dsp32_op_sqrt((DSP32_Q(1.)-num_abs) >> 1);
    num_abs = DSP32_Q(0.5) - (dsp32_op_asin(num_abs) << 1);
    return (num < 0)?-num_abs:num_abs;
  }
  else
  {
    num_sqr = (((S64) num)*((S64) num)) >> DSP32_QB;
    res = (num_sqr*((S64) DSP32_Q(F))) >> DSP32_QB;
    res = (num_sqr*(res + ((S64) DSP32_Q(E)))) >> DSP32_QB;
    res = (num_sqr*(res + ((S64) DSP32_Q(D)))) >> DSP32_QB;
    res = (num_sqr*(res + ((S64) DSP32_Q(C)))) >> DSP32_QB;
    res = (num_sqr*(res + ((S64) DSP32_Q(B)))) >> DSP32_QB;
    num = (((S64) num)*(res + ((S64) DSP32_Q(A)))) >> (DSP32_QB + 1);

    return num;
  }
}

#endif
