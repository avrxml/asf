/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit vector division with real function optimized for the avr32uc3
 *
 * This file contains the code of the vector division with real.
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
    !defined(FORCE_GENERIC_VECT16_REALDIV) && \
    defined(TARGET_SPECIFIC_VECT16_REALDIV)

#if __GNUC__
#  define DSP16_REALDIV_END_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP16_REALDIV_END_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP16_REALDIV_END_KERNEL_X_FCT(x_num, data)  DSP16_REALDIV_END_KERNEL_X_FCT__(x_num, data)
#endif


#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str)  str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str)  str":E"
#endif

/*********************************************************************************************
 * Macro name: DSP16_REALDIVISION_X
 * Used registers:
 *   r0, r1, r2, r3
 * Pre:
 * Description:
 *   vect1(0) = (vect2(0) * real) >> DSP16_QB
 *   vect1(1) = (vect2(1) * real) >> DSP16_QB
 *   ... X times ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP16_REALDIVISION_0(r_vect1, r_vect2, real)

#define DSP16_REALDIVISION_1(r_vect1, r_vect2, real) \
        "ld.sh  r2, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "lsl    r2, "ASTRINGZ(DSP16_QB)"\n\t" \
\
        "divs   r0, r2, "ASTRINGZ(real)"\n\t" \
        "st.h   "ASTRINGZ(r_vect1)"[0x0], r0\n\t"

#define DSP16_REALDIVISION_2(r_vect1, r_vect2, real) \
        "ld.sh  r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
        "divs   r2, r0, "ASTRINGZ(real)"\n\t" \
\
        "ld.sh  r0, "ASTRINGZ(r_vect2)"[0x2]\n\t" \
        "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
        "divs   r4, r0, "ASTRINGZ(real)"\n\t" \
\
        "sthh.w  r12[0x0], r2:b, r4:b\n\t"

#define DSP16_REALDIVISION_3(r_vect1, r_vect2, real) \
        "ld.sh  r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
        "divs   r2, r0, "ASTRINGZ(real)"\n\t" \
\
        "ld.sh  r0, "ASTRINGZ(r_vect2)"[0x2]\n\t" \
        "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
        "divs   r4, r0, "ASTRINGZ(real)"\n\t" \
\
        "sthh.w  r12[0x0], r2:b, r4:b\n\t" \
\
        "ld.sh  r2, "ASTRINGZ(r_vect2)"[0x4]\n\t" \
        "lsl    r2, "ASTRINGZ(DSP16_QB)"\n\t" \
\
        "divs   r0, r2, "ASTRINGZ(real)"\n\t" \
        "st.h   "ASTRINGZ(r_vect1)"[0x4], r0\n\t"
/*********************************************************************************************/


/*********************************************************************************************
 * This function is the ending function of the real division. It is used to divide the last items of a vector.
 *********************************************************************************************/
#define DSP16_REALDIV_END_KERNEL_X_FCT__(x_num, data) \
static void TPASTE2(dsp16_vect_realdiv_end_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, dsp16_t real) \
{ \
  __asm__ __volatile__ ( \
    "pushm  r0-r3, lr\n\t" \
    TPASTE2(DSP16_REALDIVISION_, x_num)(r12, r11, r10) \
    "popm  r0-r3, pc\n\t" \
  ); \
}
/*********************************************************************************************/



/*********************************************************************************************
 * Algorithm:
 * for(n=0; n<size-4; n+=4)
 * {
 *   vect1[n] = (vect2[n] * real) >> DSP16_QB;
 *   vect1[n+1] = (vect2[n+1] * real) >> DSP16_QB;
 *   vect1[n+2] = (vect2[n+2] * real) >> DSP16_QB;
 *   vect1[n+3] = (vect2[n+3] * real) >> DSP16_QB;
 * }
 *********************************************************************************************/
//! avr32-uc3 16 bit version
#if __GNUC__
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
static int dsp16_vect_realdiv_kernel_ext(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real)
{
  __asm__ __volatile__ ( \
      "pushm    r0-r7, lr\n\t" \
\
      "mov      lr, 0\n\t" \
      "sub      r10, 3\n\t" \
\
      "cp.h     lr, r10\n\t" \
      ASM_INSTRUCT_COMPACKED("brge __dsp16_realdiv_ext_end_loop")"\n" \
\
    "__dsp16_realdiv_ext_loop:\n\t" \
\
      "ld.sh  r0, r11[0x0]\n\t" \
      "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
      "divs   r2, r0, r9\n\t" \
\
      "ld.sh  r0, r11[0x2]\n\t" \
      "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
      "divs   r4, r0, r9\n\t" \
\
      "sthh.w  r12[lr << 1], r2:b, r4:b\n\t" \
      "sub    lr, -2\n\t" \
\
      "ld.sh  r0, r11[0x4]\n\t" \
      "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
      "divs   r2, r0, r9\n\t" \
\
      "ld.sh  r0, r11[0x6]\n\t" \
      "lsl    r0, "ASTRINGZ(DSP16_QB)"\n\t" \
      "divs   r4, r0, r9\n\t" \
\
      "sub    r11, -8\n\t" \
\
      "sthh.w  r12[lr << 1], r2:b, r4:b\n\t" \
      "sub    lr, -2\n\t" \
\
      "cp.h     lr, r10\n\t" \
      ASM_INSTRUCT_COMPACKED("brlt __dsp16_realdiv_ext_loop")"\n" \
\
    "__dsp16_realdiv_ext_end_loop:\n\t" \
\
      "mov      r12, lr\n\t" \
      "popm     r0-r7, pc\n\t" \
    ); \

  return 0;
}
/*********************************************************************************************/

DSP16_REALDIV_END_KERNEL_X_FCT(0, "")
DSP16_REALDIV_END_KERNEL_X_FCT(1, "")
DSP16_REALDIV_END_KERNEL_X_FCT(2, "")
DSP16_REALDIV_END_KERNEL_X_FCT(3, "")

void dsp16_vect_realdiv(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real)
{
  typedef void (*realdiv_end_kernel_opti_t)(dsp16_t *, dsp16_t *, dsp16_t);
  static const realdiv_end_kernel_opti_t realdiv_end_kernel_opti[4] = {
    dsp16_vect_realdiv_end_kernel_x0,
    dsp16_vect_realdiv_end_kernel_x1,
    dsp16_vect_realdiv_end_kernel_x2,
    dsp16_vect_realdiv_end_kernel_x3
  };
  int n;

  n = dsp16_vect_realdiv_kernel_ext(vect1, vect2, size, real);

  // Jump on different functions depending on the length of the vectors to compute
  realdiv_end_kernel_opti[size&0x3](&vect1[n], &vect2[n], real);
}

#endif
