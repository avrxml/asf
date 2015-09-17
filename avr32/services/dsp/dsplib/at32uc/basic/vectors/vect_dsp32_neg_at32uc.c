/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit addition function optimized for the avr32uc3
 *
 * This file contains the code of the vector addition.
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
    !defined(FORCE_GENERIC_VECT32_NEG) && \
    defined(TARGET_SPECIFIC_VECT32_NEG)

#if __GNUC__
#  define DSP32_NEG_END_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP32_NEG_END_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP32_NEG_END_KERNEL_X_FCT(x_num, data)  DSP32_NEG_END_KERNEL_X_FCT__(x_num, data)
#endif


#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str)  str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str)  str":E"
#endif


/*********************************************************************************************
 * Macro name: DSP32_NEGATE_X
 * Used registers:
 *   r0, r1, r2, r3, r4, r5
 * Pre:
 * Description:
 *   vect1(0) = -vect2(0)
 *   vect1(1) = -vect2(1)
 *   ... X times ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP32_NEGATE_0(r_vect1, r_vect2)

#define DSP32_NEGATE_1(r_vect1, r_vect2) \
        "ld.w  r2, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
\
        "neg    r2\n\t" \
        "st.w   "ASTRINGZ(r_vect1)"[0x0], r2\n\t"

#define DSP32_NEGATE_2(r_vect1, r_vect2) \
        "ld.d   r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
\
        "neg    r0\n\t" \
        "neg    r1\n\t" \
        "st.d  "ASTRINGZ(r_vect1)"[0x0], r0\n\t"

#define DSP32_NEGATE_3(r_vect1, r_vect2) \
        "ld.d   r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
\
        "neg    r0\n\t" \
        "neg    r1\n\t" \
        "st.d  "ASTRINGZ(r_vect1)"[0x0], r0\n\t" \
\
        "ld.w  r0, "ASTRINGZ(r_vect2)"[0x8]\n\t" \
\
        "neg    r0\n\t" \
        "st.w   "ASTRINGZ(r_vect1)"[0x8], r0\n\t"
/*********************************************************************************************/


/*********************************************************************************************
 * This function is the ending function of the negate. It is used to negate the last items of a vector.
 *********************************************************************************************/
#define DSP32_NEG_END_KERNEL_X_FCT__(x_num, data) \
static void TPASTE2(dsp32_vect_neg_end_kernel_x, x_num)(dsp32_t *vect1, dsp32_t *vect2) \
{ \
  __asm__ __volatile__ ( \
    "pushm  r0-r7, lr\n\t" \
    TPASTE2(DSP32_NEGATE_, x_num)(r12, r11) \
    "popm  r0-r7, pc\n\t" \
  ); \
}
/*********************************************************************************************/



/*********************************************************************************************
 * Algorithm:
 * for(n=0; n<size-4; n+=4)
 * {
 *   vect1[n] = -vect2[n];
 *   vect1[n+1] = -vect2[n+1];
 *   vect1[n+2] = -vect2[n+2];
 *   vect1[n+3] = -vect2[n+3];
 * }
 *********************************************************************************************/
//! avr32-uc3 32-bit version
#if __GNUC__
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
static int dsp32_vect_neg_kernel_ext(dsp32_t *vect1, dsp32_t *vect2, int size)
{
  __asm__ __volatile__ (
      "pushm  r0-r7, lr\n\t"

      "mov    lr, 0\n\t"
      "sub    r10, 3\n\t"

      "cp.h   lr, r10\n\t"
      ASM_INSTRUCT_COMPACKED("brge __dsp32_neg_ext_end_loop")"\n"

    "__dsp32_neg_ext_loop:\n\t"

      "ld.d   r0, r11[lr << 2]\n\t"

      "neg    r0\n\t"
      "neg    r1\n\t"
      "st.d   r12[lr << 2], r0\n\t"

      "sub    lr, -2\n\t"

      "ld.d   r0, r11[lr << 2]\n\t"

      "neg    r0\n\t"
      "neg    r1\n\t"
      "st.d   r12[lr << 2], r0\n\t"

      "sub    lr, -2\n\t"

      "cp.h   lr, r10\n\t"
      ASM_INSTRUCT_COMPACKED("brlt __dsp32_neg_ext_loop")"\n"

    "__dsp32_neg_ext_end_loop:\n\t"

      "mov    r12, lr\n\t"
      "popm   r0-r7, pc\n\t"
    );

  return 0;
}
/*********************************************************************************************/

DSP32_NEG_END_KERNEL_X_FCT(0, "")
DSP32_NEG_END_KERNEL_X_FCT(1, "")
DSP32_NEG_END_KERNEL_X_FCT(2, "")
DSP32_NEG_END_KERNEL_X_FCT(3, "")

void dsp32_vect_neg(dsp32_t *vect1, dsp32_t *vect2, int size)
{
  typedef void (*neg_end_kernel_opti_t)(dsp32_t *, dsp32_t *);
  static const neg_end_kernel_opti_t neg_end_kernel_opti[4] = {
    dsp32_vect_neg_end_kernel_x0,
    dsp32_vect_neg_end_kernel_x1,
    dsp32_vect_neg_end_kernel_x2,
    dsp32_vect_neg_end_kernel_x3
  };
  int n;

  n = dsp32_vect_neg_kernel_ext(vect1, vect2, size);

  // Jump on different functions depending on the length of the vectors to compute
  neg_end_kernel_opti[size&0x3](&vect1[n], &vect2[n]);
}

#endif
