/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 32-bit partial convolution function for the AVR32 UC3
 *
 * This file contains the code of the partial convolution.
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
#include "preprocessor.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_VECT32_CONVPART) || \
    !defined(TARGET_SPECIFIC_VECT32_CONVPART)

#define DSP32_SUM_ITEM(x, line)       sum += ((long long) pvect2[x])*((long long) pvect3[8 - x - 1]);

#define DSP32_SUM_ITEM_INIT(x, line)  sum += ((long long) *pvect2++)*((long long) *--pvect3);

#define DSP32_CONVPART_KERNEL_X_FCT(x_num, data)    \
static void TPASTE2(dsp32_vect_convpart_kernel_x, x_num)(dsp32_t *vect1, dsp32_t *vect2, int vect1_size, dsp32_t *vect3, int vect3_size) \
{ \
  int i, j; \
  long long sum; \
  dsp32_t *pvect3, *pvect2; \
\
  for(j=0; j<vect1_size; j++) \
    { \
      sum = 0; \
      pvect3 = &vect3[vect3_size]; \
      pvect2 = &vect2[j]; \
\
      MREPEAT(x_num, DSP32_SUM_ITEM_INIT, ) \
\
      for(i=x_num; i<vect3_size; i += 8) \
        { \
          pvect3 -= 8; \
          MREPEAT8(DSP32_SUM_ITEM, ) \
          pvect2 += 8; \
        } \
\
      vect1[j] = sum >> DSP32_QB; \
    } \
}

DSP32_CONVPART_KERNEL_X_FCT(0, )
DSP32_CONVPART_KERNEL_X_FCT(1, )
DSP32_CONVPART_KERNEL_X_FCT(2, )
DSP32_CONVPART_KERNEL_X_FCT(3, )
DSP32_CONVPART_KERNEL_X_FCT(4, )
DSP32_CONVPART_KERNEL_X_FCT(5, )
DSP32_CONVPART_KERNEL_X_FCT(6, )
DSP32_CONVPART_KERNEL_X_FCT(7, )

void dsp32_vect_convpart(dsp32_t *vect1, dsp32_t *vect2, int vect2_size, dsp32_t *vect3, int vect3_size)
{
  typedef void (*convpart_kernel_opti_t)(dsp32_t *, dsp32_t *, int, dsp32_t *, int);
  static const convpart_kernel_opti_t convpart_kernel_opti[8] = {
      dsp32_vect_convpart_kernel_x0,
      dsp32_vect_convpart_kernel_x1,
      dsp32_vect_convpart_kernel_x2,
      dsp32_vect_convpart_kernel_x3,
      dsp32_vect_convpart_kernel_x4,
      dsp32_vect_convpart_kernel_x5,
      dsp32_vect_convpart_kernel_x6,
      dsp32_vect_convpart_kernel_x7
    };

  // Jump on different functions depending on the length of the partial convolution to compute
  convpart_kernel_opti[vect3_size&0x7](vect1, vect2, vect2_size - vect3_size + 1, vect3, vect3_size);
}

#endif

