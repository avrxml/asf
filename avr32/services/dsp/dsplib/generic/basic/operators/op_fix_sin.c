/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit fixed-point sine functions or the AVR32 UC3
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

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP16_SIN) || \
    !defined(TARGET_SPECIFIC_OP16_SIN)

#define DSP16_MODULO_1_MASK  (((U16) -1) >> (16 - (DSP16_QB+1)))

extern dsp16_t dsp16_op_kernel_cosfix(dsp16_t angle);
extern dsp16_t dsp16_op_kernel_sinfix(dsp16_t angle);

/*
 *  Input range = [-1; 1] corresponding to [-pi; pi]
 */
dsp16_t dsp16_op_sin(dsp16_t angle)
{
#if DSP16_QA > 1
  U16 n = ((angle + DSP16_Q(0.25)) & DSP16_MODULO_1_MASK) >> (DSP16_QB-1);
#else
  U16 n = ((U16) (angle + DSP16_Q(0.25))) >> (DSP16_QB-1);
#endif

    // Translate input down to +/- pi/4
  angle -= n*DSP16_Q(0.5);

#if DSP16_QA > 1
    // angle modulo 1 (signed values)
  angle <<= (16 - (DSP16_QB+1));
  angle >>= (16 - (DSP16_QB+1));
#endif

  switch(n)
    {
  case 0:
      return dsp16_op_kernel_sinfix(angle);
  case 1:
      return dsp16_op_kernel_cosfix(angle);
  case 2:
      return -dsp16_op_kernel_sinfix(angle);
  default:
      return -dsp16_op_kernel_cosfix(angle);
    }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_SIN) || \
    !defined(TARGET_SPECIFIC_OP32_SIN)

#define DSP32_MODULO_1_MASK  (((U32) -1) >> (32 - (DSP32_QB+1)))

extern dsp32_t dsp32_op_kernel_cosfix(dsp32_t angle);
extern dsp32_t dsp32_op_kernel_sinfix(dsp32_t angle);

/*
 *  Input range = [-1; 1] corresponding to [-pi; pi]
 */
dsp32_t dsp32_op_sin(dsp32_t angle)
{
#if DSP32_QA > 1
  U32 n = ((angle + DSP32_Q(0.25)) & DSP32_MODULO_1_MASK) >> (DSP32_QB-1);
#else
  U32 n = DSP32_Q(0.25);
  n += angle;
  n = n >> (DSP32_QB-1);
#endif

  // Translate input down to +/- pi/4
  angle -= n*DSP32_Q(0.5);

#if DSP32_QA > 1
  // angle modulo 1 (signed values)
  angle <<= (32 - (DSP32_QB + 1));
  angle >>= (32 - (DSP32_QB + 1));
#endif

  switch(n)
    {
  case 0:
      return dsp32_op_kernel_sinfix(angle);
  case 1:
      return dsp32_op_kernel_cosfix(angle);
  case 2:
      return -dsp32_op_kernel_sinfix(angle);
  default:
      return -dsp32_op_kernel_cosfix(angle);
    }
}

#endif
