/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit vector minimum function optimized for the avr32uc3
 *
 * This file contains the code of the vector minimum.
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

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_VECT32_MIN) && \
    defined(TARGET_SPECIFIC_VECT32_MIN)

#if __GNUC__
#  define DSP32_MIN_END_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP32_MIN_END_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP32_MIN_END_KERNEL_X_FCT(x_num, data)  DSP32_MIN_END_KERNEL_X_FCT__(x_num, data)
#endif


#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str)  str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str)  str":E"
#endif



/*********************************************************************************************
 * Macro name: DSP32_MINIMUM_X
 * Used registers:
 *   r0, r1, r2, r3
 * Pre:
 * Description:
 *   vect1(0) = vect2(0) + vect3(0)
 *   vect1(1) = vect2(1) + vect3(1)
 *   ... X times ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP32_MINIMUM_0(r_vect1, min)

#define DSP32_MINIMUM_1(r_vect1, min) \
        "ld.w   r0, "ASTRINGZ(r_vect1)"[0x0]\n\t" \
        "min    "ASTRINGZ(min)", "ASTRINGZ(min)", r0\n\t"

#define DSP32_MINIMUM_2(r_vect1, min) \
        "ld.d   r0, "ASTRINGZ(r_vect1)"[0x0]\n\t" \
        "min    "ASTRINGZ(min)", "ASTRINGZ(min)", r0\n\t" \
        "min    "ASTRINGZ(min)", "ASTRINGZ(min)", r1\n\t"

#define DSP32_MINIMUM_3(r_vect1, min) \
        "ld.d   r0, "ASTRINGZ(r_vect1)"[0x0]\n\t" \
        "min    "ASTRINGZ(min)", "ASTRINGZ(min)", r0\n\t" \
        "min    "ASTRINGZ(min)", "ASTRINGZ(min)", r1\n\t" \
        "ld.w   r0, "ASTRINGZ(r_vect1)"[0x8]\n\t" \
        "min    "ASTRINGZ(min)", "ASTRINGZ(min)", r0\n\t"

/*********************************************************************************************/


/*********************************************************************************************
 * This function is the ending function of the minimum. It is used to compute the last items of a vector.
 *********************************************************************************************/
#define DSP32_MIN_END_KERNEL_X_FCT__(x_num, data) \
static dsp32_t TPASTE2(dsp32_vect_min_end_kernel_x, x_num)(dsp32_t *vect1, dsp32_t min) \
{ \
  __asm__ __volatile__ ( \
    "pushm  r0-r3, lr\n\t" \
    TPASTE2(DSP32_MINIMUM_, x_num)(r12, r11) \
    "mov    r12, r11\n\t" \
    "popm   r0-r3, pc\n\t" \
  ); \
\
  return 0; \
}
/*********************************************************************************************/



/*********************************************************************************************
 * Algorithm:
 * for(n=0; n<size-4; n+=4)
 * {
 *   vect1[n] = vect2[n] + vect3[n];
 *   vect1[n+1] = vect2[n+1] + vect3[n+1];
 *   vect1[n+2] = vect2[n+2] + vect3[n+2];
 *   vect1[n+3] = vect2[n+3] + vect3[n+3];
 * }
 *********************************************************************************************/
//! avr32-uc3 32 bit version
#if __GNUC__
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
static int dsp32_vect_min_kernel_ext(dsp32_t *vect1, int size, dsp32_t *min)
{
  __asm__ __volatile__ (
      "pushm  r0-r7, lr\n\t"

      "ld.w   r2, r10[0x00]\n\t"

      "sub    lr, r11, 4\n\t"
      ASM_INSTRUCT_COMPACKED("brlt __dsp32_min_ext_end_loop")"\n"

    "__dsp32_min_ext_loop:\n\t"

      "ld.d   r0, r12[0x00]\n\t"
      "min    r2, r2, r0\n\t"
      "min    r2, r2, r1\n\t"

      "ld.d   r0, r12[0x08]\n\t"
      "min    r2, r2, r0\n\t"
      "min    r2, r2, r1\n\t"

      "sub    r12, -16\n\t"

      "sub    lr, 4\n\t"
      ASM_INSTRUCT_COMPACKED("brge __dsp32_min_ext_loop")"\n"

    "__dsp32_min_ext_end_loop:\n\t"

      "st.w   r10[0x00], r2\n\t"

      "sub    lr, -4\n\t"
      "sub    r12, r11, lr\n\t"
      "popm   r0-r7, pc\n\t"
    );

  return 0;
}
/*********************************************************************************************/

DSP32_MIN_END_KERNEL_X_FCT(0, "")
DSP32_MIN_END_KERNEL_X_FCT(1, "")
DSP32_MIN_END_KERNEL_X_FCT(2, "")
DSP32_MIN_END_KERNEL_X_FCT(3, "")

dsp32_t dsp32_vect_min(dsp32_t *vect1, int size)
{
  typedef dsp32_t (*min_end_kernel_opti_t)(dsp32_t *, dsp32_t);
  static const min_end_kernel_opti_t min_end_kernel_opti[4] = {
    dsp32_vect_min_end_kernel_x0,
    dsp32_vect_min_end_kernel_x1,
    dsp32_vect_min_end_kernel_x2,
    dsp32_vect_min_end_kernel_x3
  };
  int n;
  dsp32_t min = (dsp32_t) DSP_Q_MAX(DSP32_QA, DSP32_QB);

  n = dsp32_vect_min_kernel_ext(vect1, size, &min);

  // Jump on different functions depending on the length of the vectors to compute
  return min_end_kernel_opti[size&0x3](&vect1[n], min);
}

#endif
