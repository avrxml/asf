/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit vector minimum function optimized for the avr32uc3
 *
 * This file contains the code of the vector minimum.
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
