/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit partial convolution function optimized for the AVR32 UC3
 *
 * This file contains the code of the partial convolution.
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
    !defined(FORCE_GENERIC_VECT16_CONVPART) && \
    defined(TARGET_SPECIFIC_VECT16_CONVPART)

/*********************************************************************************************
 * Description:
 * This routine follows this algorithm:
 *   sum1 += h(i)*vect1(i) + h(i+1)*vect1(i+1) + h(i+2)*vect1(i+2) +  h(i+3)*vect1(i+3)
 *   sum2 += h(i)*vect1(i+1) + h(i+1)*vect1(i+2) + h(i+2)*vect1(i+3) +  h(i+3)*vect1(i+4)
 *   sum3 += h(i)*vect1(i+2) + h(i+1)*vect1(i+3) + h(i+2)*vect1(i+4) +  h(i+3)*vect1(i+5)
 *   sum4 += h(i)*vect1(i+3) + h(i+1)*vect1(i+4) + h(i+2)*vect1(i+5) +  h(i+3)*vect1(i+6)
 *   i += 4
 * Used registers:
 *   r4, r5, r6, r7, r10, r11
 *
 *********************************************************************************************/
#define DSP16_COMPUT_TAP_4x4_0(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
\
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:b, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:t, r11:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:t, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:t, r11:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum4)", r4:b, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:t, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:b, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:t, r10:t\n\t"


#define DSP16_COMPUT_TAP_4x4_1(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ldins.h  r5:t, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
\
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum2)", r5:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:t, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:b, r11:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum3)", r5:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:t, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:b, r11:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum4)", r5:t, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:t, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:b, r10:t\n\t"


#define DSP16_COMPUT_TAP_4x4_2(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
\
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:b, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:t, r11:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:t, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:t, r11:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum4)", r4:b, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:t, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:b, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:t, r10:t\n\t"


#define DSP16_COMPUT_TAP_4x4_3(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ldins.h  r5:t, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
\
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum2)", r5:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:t, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:b, r11:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum3)", r5:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:t, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:b, r11:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum4)", r5:t, r6:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:t, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:b, r10:t\n\t"

/*********************************************************************************************/

/*********************************************************************************************
 * Macro name: DSP16_COMPUT_TAP_ENDING_X
 * Used registers:
 *   r4, r6, r10, r11, r_i
 * Pre:
 *  r11:r10 : vect(i) .. vect(i+3)
 * Description:
 *   sum1 += h(i)*vect1(i) + h(i+1)*vect1(i+1) + ...
 *   sum2 += h(i)*vect1(i+1) + h(i+1)*vect1(i+2) + ...
 *   sum3 += h(i)*vect1(i+2) + h(i+1)*vect1(i+3) + ...
 *   sum4 += h(i)*vect1(i+3) + h(i+1)*vect1(i+4) + ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP16_COMPUT_TAP_ENDING_0(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4)

#define DSP16_COMPUT_TAP_ENDING_1(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
        "ld.sh  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r10:t\n\t" \
        "machh.w     "ASTRINGZ(r_sum4)", r4:b, r10:b\n\t"


#define DSP16_COMPUT_TAP_ENDING_2(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
\
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
\
        "ld.sh  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r11:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:t, r10:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r10:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:t, r10:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum4)", r4:b, r10:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r4:t, r6:b\n\t"


#define DSP16_COMPUT_TAP_ENDING_3(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
\
        "sub  "ASTRINGZ(r_h)", 4\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
        "ld.w  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r10:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum2)", r4:t, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:b, r10:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r5:t, r10:b\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum3)", r4:t, r10:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:b, r10:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r5:t, r6:t\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum4)", r4:t, r10:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum4)", r5:t, r6:b\n\t" \
/*********************************************************************************************/

#if __GNUC__
#  define DSP16_CONVPART_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP16_CONVPART_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP16_CONVPART_KERNEL_X_FCT(x_num, data)  DSP16_CONVPART_KERNEL_X_FCT__(x_num, data)
#endif

#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str) str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str) str":E"
#endif


/*********************************************************************************************
 * This function is the light main function that permits to compute the numerators coefficients.
 * Algorithm:
 *   for(n=0; n<vect1_size; n+=4)
 *   {
 *        sum1 = 0;
 *        sum2 = 0;
 *        sum3 = 0;
 *        sum4 = 0;
 *        for(m=0; m<vect3_size; m+=4)
 *        {
 *              sum1 += vect3[m]*vect2[n - m] + vect3[m + 1]*vect2[n - m - 1] + vect3[m + 2]*vect2[n - m - 2] + vect3[m + 3]*vect2[n - m - 3];
 *              sum2 += vect3[m]*vect2[n - m + 1] + vect3[m + 1]*vect2[n - m] + vect3[m + 2]*vect2[n - m - 1] + vect3[m + 3]*vect2[n - m - 2];
 *              sum3 += vect3[m]*vect2[n - m + 2] + vect3[m + 1]*vect2[n - m + 1] + vect3[m + 2]*vect2[n - m] + vect3[m + 3]*vect2[n - m - 1];
 *              sum4 += vect3[m]*vect2[n - m + 3] + vect3[m + 1]*vect2[n - m + 2] + vect3[m + 2]*vect2[n - m + 1] + vect3[m + 3]*vect2[n - m];
 *        }
 *        vect1[n++] -= sum1 >> DSP16_QB;
 *        vect1[n++] -= sum2 >> DSP16_QB;
 *        vect1[n++] -= sum3 >> DSP16_QB;
 *        vect1[n++] -= sum4 >> DSP16_QB;
 *   }
 * Used registers: all
 * Register before the main loop:
 *   r12 = vect2
 *   r11 = vect1 = sp[0x04]
 *   r10 = vect1_size = sp[0x00]
 *   r9 = vect3
 *   r8 = vect3_size
 * Registers inside the loops:
 *   r12 = vect2
 *   r9 = vect3
 *   r8 = vect3_size
 *   r3 = sum1
 *   r2 = sum2
 *   r1 = sum3
 *   r0 = sum4
 *   lr = i
 * Condition: vect3, vect1 and vect2 must be word-align
 *********************************************************************************************/
#define DSP16_CONVPART_KERNEL_X_FCT__(x_num, data)    \
static void TPASTE2(dsp16_vect_convpart_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect1_size, dsp16_t *vect3, int vect3_size) \
{ \
  __asm__ __volatile__ ( \
        "pushm  r0-r7, lr\n\t" \
        "sub  sp, 12\n\t" \
\
        "mov  r1,  r11\n\t" \
        "mov  r11, r12\n\t" \
        "mov  r12, r1\n\t" \
\
        "add  r9,  r9, r8 << 1\n\t" \
        "mov  lr, 0x3\n\t" \
        "andn  r9, lr\n\t" \
        "stdsp  sp[0x08], r9\n\t" \
\
        "sub  r8, 3\n" \
\
    "__dsp16_convpart_loop_main"ASTRINGZ(x_num)":\n\t" \
        "stdsp  sp[0x04], r11\n\t" \
        "stdsp  sp[0x00], r10\n\t" \
        "lddsp  r9, sp[0x08]\n\t" \
\
        "mov  r0, 0\n\t" \
        "mov  r1, r0\n\t" \
        "mov  r2, r0\n\t" \
        "mov  r3, r0\n\t" \
        "mov  lr, r0\n" \
\
    "__dsp16_convpart_loop_tap"ASTRINGZ(x_num)":\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_4x4_, x_num)(r12, r9, lr, r3, r2, r1, r0) \
\
        "cp.h  lr, r8\n\t" \
      ASM_INSTRUCT_COMPACKED("brlt __dsp16_convpart_loop_tap"ASTRINGZ(x_num))"\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_ENDING_, x_num)(r12, r9, lr, r3, r2, r1, r0) \
\
        "asr  r3, "ASTRINGZ(DSP16_QB)"\n\t" \
        "asr  r2, "ASTRINGZ(DSP16_QB)"\n\t" \
        "asr  r1, "ASTRINGZ(DSP16_QB)"\n\t" \
        "asr  r0, "ASTRINGZ(DSP16_QB)"\n\t" \
\
        "lddsp  r11, sp[0x04]\n\t" \
        "sthh.w  r11[0x0], r3:b, r2:b\n\t" \
        "sthh.w  r11[0x4], r1:b, r0:b\n\t" \
\
        "sub  r11, -8\n\t" \
        "sub  r12, -8\n\t" \
\
        "lddsp  r10, sp[0x00]\n\t" \
        "sub  r10, 4\n\t" \
        "brgt  __dsp16_convpart_loop_main"ASTRINGZ(x_num)"\n\t" \
\
        "sub  sp, -12\n\t" \
        "popm  r0-r7, pc\n\t" \
    ); \
}
/*********************************************************************************************/

DSP16_CONVPART_KERNEL_X_FCT(0, "")
DSP16_CONVPART_KERNEL_X_FCT(1, "")
DSP16_CONVPART_KERNEL_X_FCT(2, "")
DSP16_CONVPART_KERNEL_X_FCT(3, "")

void dsp16_vect_convpart(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *vect3, int vect3_size)
{
  typedef void (*convpart_kernel_opti_t)(dsp16_t *, dsp16_t *, int, dsp16_t *, int);
  static const convpart_kernel_opti_t convpart_kernel_opti[4] = {
      dsp16_vect_convpart_kernel_x0,
      dsp16_vect_convpart_kernel_x1,
      dsp16_vect_convpart_kernel_x2,
      dsp16_vect_convpart_kernel_x3
    };

  // Jump on different functions depending on the length of the partial convolution to compute
  convpart_kernel_opti[vect3_size&0x3](vect1, vect2, vect2_size - vect3_size + 1, vect3, vect3_size);
}

#endif
