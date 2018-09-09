/*****************************************************************************
 *
 * \file
 *H
 * \brief 16-bit complex conjugate function optimized for the avr32uc3
 *
 * This file contains the code of the vector addition.
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
    !defined(FORCE_GENERIC_VECT16_COMPLEXCONJ) && \
    defined(TARGET_SPECIFIC_VECT16_COMPLEXCONJ)

#if __GNUC__
#  define DSP16_CONJ_END_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP16_CONJ_END_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP16_CONJ_END_KERNEL_X_FCT(x_num, data)  DSP16_CONJ_END_KERNEL_X_FCT__(x_num, data)
#endif


#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str)  str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str)  str":E"
#endif



/*********************************************************************************************
 * Macro name: DSP16_CONJUGATE_X
 * Used registers:
 *   r0, r1, r2, r3
 * Pre:
 * Description:
 *   vect1(0) = -vect2(0)
 *   vect1(1) = -vect2(1)
 *   ... X times ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP16_CONJUGATE_0(r_vect1, r_vect2)

#define DSP16_CONJUGATE_1(r_vect1, r_vect2) \
        "ld.w   r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "sthh.w  "ASTRINGZ(r_vect1)"[0x0], r0:b, r0:t\n\t"

#define DSP16_CONJUGATE_2(r_vect1, r_vect2) \
        "ld.d   r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "swap.h  r0\n\t" \
        "swap.h  r1\n\t" \
        "st.d  "ASTRINGZ(r_vect1)"[0x0], r0\n\t"

#define DSP16_CONJUGATE_3(r_vect1, r_vect2) \
        "ld.d   r0, "ASTRINGZ(r_vect2)"[0x0]\n\t" \
        "swap.h  r0\n\t" \
        "swap.h  r1\n\t" \
        "st.d  "ASTRINGZ(r_vect1)"[0x0], r0\n\t" \
\
        "ld.w   r0, "ASTRINGZ(r_vect2)"[0x8]\n\t" \
        "sthh.w  "ASTRINGZ(r_vect1)"[0x8], r0:b, r0:t\n\t"

/*********************************************************************************************/


/*********************************************************************************************
 * This function is the ending function of the CONJUGATE. It is used to CONJUGATE the last items of a vector.
 *********************************************************************************************/
#define DSP16_CONJ_END_KERNEL_X_FCT__(x_num, data) \
static void TPASTE2(dsp16_vect_conj_end_kernel_x, x_num)(dsp16_complex_t *vect1, dsp16_complex_t *vect2) \
{ \
  __asm__ __volatile__ ( \
    "pushm  r0-r3, lr\n\t" \
    TPASTE2(DSP16_CONJUGATE_, x_num)(r12, r11) \
    "popm  r0-r3, pc\n\t" \
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
//! avr32-uc3 16 bit version
#if __GNUC__
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
static int dsp16_vect_conj_kernel_ext(dsp16_complex_t *vect1, dsp16_complex_t *vect2, int size)
{
  __asm__ __volatile__ (
      "pushm  r0-r7, lr\n\t"

      "mov    lr, 0\n\t"
      "sub    r10, 3\n\t"
      "mov    r9, 0\n\t"

      "cp.h   lr, r10\n\t"
      ASM_INSTRUCT_COMPACKED("brge __dsp16_conj_ext_end_loop")"\n"

    "__dsp16_conj_ext_loop:\n\t"

      "ld.d   r0, r11[lr << 2]\n\t"
      "swap.h  r0\n\t"
      "swap.h  r1\n\t"
      "st.d  r12[lr << 2], r0\n\t"

      "sub    lr, -2\n\t"

      "ld.d   r0, r11[lr << 2]\n\t"
      "swap.h  r0\n\t"
      "swap.h  r1\n\t"
      "st.d  r12[lr << 2], r0\n\t"

      "sub    lr, -2\n\t"

      "cp.h   lr, r10\n\t"
      ASM_INSTRUCT_COMPACKED("brlt __dsp16_conj_ext_loop")"\n"

    "__dsp16_conj_ext_end_loop:\n\t"

      "mov    r12, lr\n\t"
      "popm   r0-r7, pc\n\t"
    );

  return 0;
}
/*********************************************************************************************/

DSP16_CONJ_END_KERNEL_X_FCT(0, "")
DSP16_CONJ_END_KERNEL_X_FCT(1, "")
DSP16_CONJ_END_KERNEL_X_FCT(2, "")
DSP16_CONJ_END_KERNEL_X_FCT(3, "")

void dsp16_vect_complex_conj(dsp16_complex_t *vect1, dsp16_complex_t *vect2, int size)
{
  typedef void (*conj_end_kernel_opti_t)(dsp16_complex_t *, dsp16_complex_t *);
  static const conj_end_kernel_opti_t conj_end_kernel_opti[4] = {
    dsp16_vect_conj_end_kernel_x0,
    dsp16_vect_conj_end_kernel_x1,
    dsp16_vect_conj_end_kernel_x2,
    dsp16_vect_conj_end_kernel_x3
  };
  int n;

  n = dsp16_vect_conj_kernel_ext(vect1, vect2, size);

  // Jump on different functions depending on the length of the vectors to compute
  conj_end_kernel_opti[size&0x3](&vect1[n], &vect2[n]);
}

#endif
