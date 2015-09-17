/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit IIR filter function optimized for the AVR32 UC3.
 *
 * This file contains the code of the IIR filter.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "dsp.h"
#include "preprocessor.h"

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_FILT16_IIRPART) && \
    defined(TARGET_SPECIFIC_FILT16_IIRPART)

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
\
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "ld.sh  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r11:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum2)", r4:b, r11:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum3)", r4:b, r10:t\n\t" \
        "machh.w     "ASTRINGZ(r_sum4)", r4:b, r10:b\n\t"


#define DSP16_COMPUT_TAP_ENDING_2(r_vect1, r_h, r_i, r_sum1, r_sum2, r_sum3, r_sum4) \
\
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -4\n\t" \
\
        "ld.uh  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
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
        "ld.d  r10, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
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
        "machh.w  "ASTRINGZ(r_sum4)", r5:t, r6:b\n\t"

/*********************************************************************************************/





#if __GNUC__
#  define DSP16_IIRPART_KERNEL_NUM_X_FCT(x_num, data)  __attribute__((__naked__)) DSP16_IIRPART_KERNEL_NUM_X_FCT__(x_num, data)
#  define DSP16_IIRPART_KERNEL_DEN_X_FCT(x_num, data)  __attribute__((__naked__)) DSP16_IIRPART_KERNEL_DEN_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP16_IIRPART_KERNEL_NUM_X_FCT(x_num, data)  DSP16_IIRPART_KERNEL_NUM_X_FCT__(x_num, data)
#  define DSP16_IIRPART_KERNEL_DEN_X_FCT(x_num, data)  DSP16_IIRPART_KERNEL_DEN_X_FCT__(x_num, data)
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
 *        vect1[n++] -= sum1 >> (DSP16_QB - prediv);
 *        vect1[n++] -= sum2 >> (DSP16_QB - prediv);
 *        vect1[n++] -= sum3 >> (DSP16_QB - prediv);
 *        vect1[n++] -= sum4 >> (DSP16_QB - prediv);
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
#define DSP16_IIRPART_KERNEL_NUM_X_FCT__(x_num, data)    \
static void TPASTE2(dsp16_filt_iirpart_kernel_num_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect1_size, dsp16_t *vect3, int vect3_size, int prediv) \
{ \
  __asm__ __volatile__ ( \
        "pushm  r0-r7, lr\n\t" \
        "sub  sp, 12\n\t" \
\
        "lddsp  r4, sp[48]\n\t" \
        "rsub   r4, r4, "ASTRINGZ(DSP16_QB)" \n\t" \
        "stdsp  sp[48], r4\n\t" \
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
    "__dsp16_iirpart_num_loop_main"ASTRINGZ(x_num)":\n\t" \
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
\
    "__dsp16_iirpart_num_loop_tap"ASTRINGZ(x_num)":\n\t" \
        "cp.h  lr, r8\n\t" \
      ASM_INSTRUCT_COMPACKED("brge __dsp16_iirpart_num_endloop_tap"ASTRINGZ(x_num))"\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_4x4_, x_num)(r12, r9, lr, r3, r2, r1, r0) \
\
        "bral __dsp16_iirpart_num_loop_tap"ASTRINGZ(x_num)"\n" \
\
    "__dsp16_iirpart_num_endloop_tap"ASTRINGZ(x_num)":\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_ENDING_, x_num)(r12, r9, lr, r3, r2, r1, r0) \
\
        "lddsp  r4, sp[48]\n\t" \
\
        "asr    r3, r3, r4\n\t" \
        "asr    r2, r2, r4\n\t" \
        "asr    r1, r1, r4\n\t" \
        "asr    r0, r0, r4\n\t" \
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
        "brgt  __dsp16_iirpart_num_loop_main"ASTRINGZ(x_num)"\n\t" \
\
        "sub  sp, -12\n\t" \
        "popm  r0-r7, pc\n\t" \
    ); \
}
/*********************************************************************************************/














/*********************************************************************************************
 * Description:
 * This routine follows this algorithm:
 *   sum1 += h(i)*vect1(i) + h(i+1)*vect1(i+1) + h(i+2)*vect1(i+2) +  h(i+3)*vect1(i+3)
 *                + h(i+4)*vect1(i+4) + h(i+5)*vect1(i+5) + h(i+6)*vect1(i+6) +  h(i+7)*vect1(i+7)
 *   i += 8
 * Used registers:
 *   r4, r5, r6, r7, r2
 *
 *********************************************************************************************/

/*
 * here r_h is 32-bits aligned
 */
#define DSP16_COMPUT_TAP_DEN_8_EVEN(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 16\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.d  r6, r2[8]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -8\n\t"


/*
 * here r_h is 16-bits aligned
 */
#define DSP16_COMPUT_TAP_DEN_8_ODD(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 18\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
\
        "ld.uh  r4, "ASTRINGZ(r_h)"[16]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "ld.d  r6, r2[8]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r7:t\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -8\n\t" \
        "sub  "ASTRINGZ(r_h)", -2\n\t" \


#define DSP16_COMPUT_TAP_DEN_8_0  DSP16_COMPUT_TAP_DEN_8_EVEN
#define DSP16_COMPUT_TAP_DEN_8_1  DSP16_COMPUT_TAP_DEN_8_ODD
#define DSP16_COMPUT_TAP_DEN_8_2  DSP16_COMPUT_TAP_DEN_8_EVEN
#define DSP16_COMPUT_TAP_DEN_8_3  DSP16_COMPUT_TAP_DEN_8_ODD
#define DSP16_COMPUT_TAP_DEN_8_4  DSP16_COMPUT_TAP_DEN_8_EVEN
#define DSP16_COMPUT_TAP_DEN_8_5  DSP16_COMPUT_TAP_DEN_8_ODD
#define DSP16_COMPUT_TAP_DEN_8_6  DSP16_COMPUT_TAP_DEN_8_EVEN
#define DSP16_COMPUT_TAP_DEN_8_7  DSP16_COMPUT_TAP_DEN_8_ODD

/********************************************************************************************
 * Idem, except that the starting values we need for r_vect1 are NOT 32-bits aligned
 * but 16-bits aligned
 ********************************************************************************************/
/*
 * here r_h is 32-bits aligned
 */
#define DSP16_COMPUT_TAP_BIS_DEN_8_EVEN(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 16\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
        "ld.d  r6, r2[8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r7:t\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
        "ld.uh  r6, r2[16]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -8\n\t"


/*
 * here r_h is 16-bits aligned
 */
#define DSP16_COMPUT_TAP_BIS_DEN_8_ODD(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 18\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
\
        "ld.uh  r4, "ASTRINGZ(r_h)"[16]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:b\n\t" \
        "ld.d  r6, r2[8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r7:t\n\t" \
\
\
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r7:b\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:b\n\t" \
        "ld.uh  r6, r2[16]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -8\n\t" \
        "sub  "ASTRINGZ(r_h)", -2\n\t"


#define DSP16_COMPUT_TAP_BIS_DEN_8_0  DSP16_COMPUT_TAP_BIS_DEN_8_EVEN
#define DSP16_COMPUT_TAP_BIS_DEN_8_1  DSP16_COMPUT_TAP_BIS_DEN_8_ODD
#define DSP16_COMPUT_TAP_BIS_DEN_8_2  DSP16_COMPUT_TAP_BIS_DEN_8_EVEN
#define DSP16_COMPUT_TAP_BIS_DEN_8_3  DSP16_COMPUT_TAP_BIS_DEN_8_ODD
#define DSP16_COMPUT_TAP_BIS_DEN_8_4  DSP16_COMPUT_TAP_BIS_DEN_8_EVEN
#define DSP16_COMPUT_TAP_BIS_DEN_8_5  DSP16_COMPUT_TAP_BIS_DEN_8_ODD
#define DSP16_COMPUT_TAP_BIS_DEN_8_6  DSP16_COMPUT_TAP_BIS_DEN_8_EVEN
#define DSP16_COMPUT_TAP_BIS_DEN_8_7  DSP16_COMPUT_TAP_BIS_DEN_8_ODD

/********************************************************************************************/


/*********************************************************************************************
 * Macro name: DSP32_COMPUT_TAP_DEN_ENDING_X
 * Used registers:
 *   r4, r5, r6, r7, r2
 * Description:
 *   sum1 += h(i)*vect1(i) + h(i+1)*vect1(i+1) + ...
 *   where X is the number of iterations
 * Note:
 *   r_vect1 is aligned on 32 bits
 *********************************************************************************************/
/*
 * 0 TAP
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 0
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_0(r_vect1, r_h, r_i, r_sum1)

/*
 * 1 TAP
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 2
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_1(r_vect1, r_h, r_i, r_sum1) \
        "ld.uh  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.uh  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:b\n\t" \

/*
 * 2 TAPs
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 4
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_2(r_vect1, r_h, r_i, r_sum1) \
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.w  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:b\n\t"

/*
 * 3 TAPs
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 6
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_3(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 6\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:t\n\t"

/*
 * 4 TAPs
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 0
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_4(r_vect1, r_h, r_i, r_sum1) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t"


/*
 * 5 TAPs
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 2
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_5(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 10\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x4]\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:t\n\t" \
        "ld.w  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:b\n\t" \
\
        "ld.uh  r6, r2[0x8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:b\n\t"


/*
 * 6 TAPs
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 4
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_6(r_vect1, r_h, r_i, r_sum1) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.w  r6, r2[0x8]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:b\n\t"

/*
 * 7 TAPs
 * r_vect1 is aligned on 32 bits
 * r_h is at a address%8 == 6
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_DEN_ENDING_7(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 14\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x8]\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:t\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:b\n\t" \
\
        "ld.d  r6, r2[0x8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:t\n\t"




/*********************************************************************************************/



/*********************************************************************************************
 * Macro name: DSP32_COMPUT_TAP_DEN_ENDING_X
 * Used registers:
 *   r4, r5, r6, r7, r2
 * Description:
 *   sum1 += h(i)*vect1(i) + h(i+1)*vect1(i+1) + ...
 *   where X is the number of iterations
 * Note:
 *   r_vect1 is aligned on 32 bits but the data we need are aligned on 16-bits
 *********************************************************************************************/
/*
 * 0 TAP
 * r_h is at a address%8 == 0
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_0(r_vect1, r_h, r_i, r_sum1)

/*
 * 1 TAP
 * r_h is at a address%8 == 2
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_1(r_vect1, r_h, r_i, r_sum1) \
        "ld.uh  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.w  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:b\n\t" \

/*
 * 2 TAPs
 * r_h is at a address%8 == 4
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_2(r_vect1, r_h, r_i, r_sum1) \
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t"

/*
 * 3 TAPs
 * r_h is at a address%8 == 6
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_3(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 6\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 1]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t"


/*
 * 4 TAPs
 * r_h is at a address%8 == 0
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_4(r_vect1, r_h, r_i, r_sum1) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "ld.uh  r6, r2[8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t"


/*
 * 5 TAPs
 * r_h is at a address%8 == 2
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_5(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 10\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x4]\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "ld.w  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
        "ld.w  r6, r2[0x8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:b\n\t"


/*
 * 6 TAPs
 * r_h is at a address%8 == 4
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_6(r_vect1, r_h, r_i, r_sum1) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:b\n\t" \
\
        "ld.d  r6, r2[8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r7:t\n\t" \
\
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r6:t\n\t"

/*
 * 7 TAPs
 * r_h is at a address%8 == 6
 * r_i = X*8
 */
#define DSP16_COMPUT_TAP_BIS_DEN_ENDING_7(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 14\n\t" \
        "ld.d  r4, "ASTRINGZ(r_h)"[0x8]\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 1\n\t" \
        "ld.d  r6, r2[0x0]\n\t" \
\
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[0x0]\n\t" \
        "ld.d  r6, r2[0x8]\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:b, r7:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r4:t, r7:b\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:b, r6:t\n\t" \
        "machh.w  "ASTRINGZ(r_sum1)", r5:t, r6:b\n\t"



/*********************************************************************************************/


/*********************************************************************************************
 * This function is the main function that permits to compute the denominator coefficients.
 * Algorithm:
 *   for(n=0; n<vect1_size; n++)
 *   {
 *        sum = 0;
 *        for(m=0; m<vect3_size; m+=8)
 *              REPx8(sum += vect3[m]*vect2[n-m];)
 *        REPx(x_num, sum += vect3[m]*vect2[n-m];)
 *        vect1[n++] -= sum >> (DSP16_QB - prediv);
  *        sum = 0;
 *        for(m=0; m<vect3_size; m+=8)
 *               REPx8(sum += vect3[m]*vect2[n-m];)
 *        REPx(x_num, sum += vect3[m]*vect2[n-m];)
 *        vect1[n] -= sum >> (DSP16_QB - prediv);
 *   }
 *  Note:
 *   The two symmetrical routines inside the main loop are here to make sure vect2 is always aligned on 32 bits
 *   at the top of the loop.
 *********************************************************************************************/
#define DSP16_IIRPART_KERNEL_DEN_X_FCT__(x_num, data)    \
static void TPASTE2(dsp16_filt_iirpart_kernel_den_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect1_size, dsp16_t *vect3, int vect3_size, int prediv) \
{ \
  __asm__ __volatile__ ( \
        "pushm  r0-r7, lr\n\t" \
        "sub  sp, 12\n\t" \
\
\
        "mov  r1,  r11\n\t" \
        "mov  r11, r12\n\t" \
        "mov  r12, r1\n\t" \
\
        "lddsp  r1, sp[48]\n\t" \
        "rsub  r1, r1, "ASTRINGZ(DSP16_QB)"\n\t" \
\
        "add  r9,  r9, r8 << 1\n\t" \
        "stdsp  sp[0x08], r9\n\t" \
\
        "sub  r8, 7\n" \
\
    "__dsp16_iirpart_den_loop_main"ASTRINGZ(x_num)":\n\t" \
        "stdsp  sp[0x04], r11\n\t" \
        "stdsp  sp[0x00], r10\n\t" \
        "lddsp  r9, sp[0x08]\n\t" \
\
        "mov  r0, 0\n\t" \
        "mov  lr, r0\n"  \
\
\
    "__dsp16_iirpart_den_loop_tap"ASTRINGZ(x_num)":\n\t" \
        "cp.h  lr, r8\n\t" \
      ASM_INSTRUCT_COMPACKED("brge  __dsp16_iirpart_den_endloop_tap"ASTRINGZ(x_num))"\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_DEN_8_, x_num)(r12, r9, lr, r0)  \
\
        "bral  __dsp16_iirpart_den_loop_tap"ASTRINGZ(x_num)"\n" \
    "__dsp16_iirpart_den_endloop_tap"ASTRINGZ(x_num)":\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_DEN_ENDING_, x_num)(r12, r9, lr, r0) \
\
        "asr  r0, r0, r1\n\t" \
\
        "lddsp  r11, sp[0x04]\n\t" \
        "ld.sh  r2, r11[0x0]\n\t" \
        "sub  r2, r0\n\t" \
        "st.h  r11++, r2\n\t" \
\
\
\
\
\
\
        "stdsp  sp[0x04], r11\n\t" \
        "lddsp  r9, sp[0x08]\n\t" \
\
        "mov  r0, 0\n\t" \
        "mov  lr, r0\n"  \
\
\
    "__dsp16_iirpart_den_loop_tap_bis"ASTRINGZ(x_num)":\n\t" \
        "cp.h  lr, r8\n\t" \
      ASM_INSTRUCT_COMPACKED("brge  __dsp16_iirpart_den_endloop_tap_bis"ASTRINGZ(x_num))"\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_BIS_DEN_8_, x_num)(r12, r9, lr, r0) \
\
        "bral  __dsp16_iirpart_den_loop_tap_bis"ASTRINGZ(x_num)"\n" \
    "__dsp16_iirpart_den_endloop_tap_bis"ASTRINGZ(x_num)":\n\t" \
\
      TPASTE2(DSP16_COMPUT_TAP_BIS_DEN_ENDING_, x_num)(r12, r9, lr, r0) \
\
        "asr  r0, r0, r1\n\t" \
\
        "lddsp  r11, sp[0x04]\n\t" \
        "ld.sh  r2, r11[0x0]\n\t" \
        "sub  r2, r0\n\t" \
        "st.h  r11++, r2\n\t" \
\
\
        "sub  r12, -4\n\t" \
\
\
\
\
        "lddsp  r10, sp[0x00]\n\t" \
        "sub  r10, 2\n\t" \
        "brgt  __dsp16_iirpart_den_loop_main"ASTRINGZ(x_num)"\n\t" \
\
        "sub  sp, -12\n\t" \
        "popm  r0-r7, pc\n\t" \
    ); \
}

/*********************************************************************************************/


DSP16_IIRPART_KERNEL_NUM_X_FCT(0, "")
DSP16_IIRPART_KERNEL_NUM_X_FCT(1, "")
DSP16_IIRPART_KERNEL_NUM_X_FCT(2, "")
DSP16_IIRPART_KERNEL_NUM_X_FCT(3, "")

#if !(DSP_OPTIMIZATION & DSP_OPTI_SIZE)

DSP16_IIRPART_KERNEL_DEN_X_FCT(0, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(1, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(2, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(3, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(4, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(5, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(6, "")
DSP16_IIRPART_KERNEL_DEN_X_FCT(7, "")

#else


/*********************************************************************************************
 * This function is the light main function that permits to compute the denominator coefficients.
 * Algorithm:
 *   for(n=0; n<vect1_size; n++)
 *   {
 *        sum = 0;
 *        for(m=0; m<vect3_size; m+=8)
 *              sum += vect3[m]*vect2[n-m];
 *        vect1[n++] -= sum >> (DSP16_QB - prediv);
 *   }
 *********************************************************************************************/
#if __GNUC__
__attribute__((__naked__))
#endif
void iirpart_kernel_opti_den_small(dsp16_t *vect1, dsp16_t *vect2, int vect1_size, dsp16_t *vect3, int vect3_size, int prediv)
{
  __asm__ __volatile__ (
  "pushm  r0-r7, lr\n\t"
  "sub    sp, 12\n\t"

  "mov    r1,  r11\n\t"
  "mov    r11, r12\n\t"
  "mov    r12, r1\n\t"

  "lddsp  r1, sp[48]\n\t"
  "rsub   r1, r1, "ASTRINGZ(DSP16_QB)"\n\t"

  "add    r9,  r9, r8 << 1\n\t"
  "stdsp  sp[0x08], r9\n"

"__dsp16_iirpart_loop_main:\n\t"
  "stdsp  sp[0x04], r11\n\t"
  "stdsp  sp[0x00], r10\n\t"
  "lddsp  r9, sp[0x08]\n\t"

  "mov    r0, 0\n\t"
  "mov    lr, r0\n"


"__dsp16_iirpart_loop_tap:\n\t"

  "ld.uh  r4, --r9\n\t"
  "ld.uh  r6, r12[lr << 1]\n\t"

  "machh.w r0, r4:b, r6:b\n\t"

  "sub    lr, -1\n\t"

  "cp.h   lr, r8\n\t"
  ASM_INSTRUCT_COMPACKED("brlt  __dsp16_iirpart_loop_tap")"\n\t"

  "asr    r0, r0, r1\n\t"

  "lddsp  r11, sp[0x04]\n\t"
  "ld.sh  r2, r11[0x0]\n\t"
  "sub    r2, r0\n\t"
  "st.h   r11++, r2\n\t"

  "sub    r12, -2\n\t"

  "lddsp  r10, sp[0x00]\n\t"
  "sub    r10, 1\n\t"
  "brgt   __dsp16_iirpart_loop_main\n\t"

  "sub    sp, -12\n\t"
  "popm   r0-r7, pc\n\t"
    );
}
/*********************************************************************************************/

#endif

void dsp16_filt_iirpart(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t *num, int num_size, dsp16_t *den, int den_size, int num_prediv, int den_prediv)
{
  typedef void (*iirpart_kernel_opti_t)(dsp16_t *, dsp16_t *, int, dsp16_t *, int, int);
  static const iirpart_kernel_opti_t iirpart_kernel_opti_num[4] = {
      dsp16_filt_iirpart_kernel_num_x0,
      dsp16_filt_iirpart_kernel_num_x1,
      dsp16_filt_iirpart_kernel_num_x2,
      dsp16_filt_iirpart_kernel_num_x3
    };
#if !(DSP_OPTIMIZATION & DSP_OPTI_SIZE)
  typedef void (*iirpart_kernel_opti_den_t)(dsp16_t *, dsp16_t *, int, dsp16_t *, int, int);
  static const iirpart_kernel_opti_den_t iirpart_kernel_opti_den[8] = {
      dsp16_filt_iirpart_kernel_den_x0,
      dsp16_filt_iirpart_kernel_den_x1,
      dsp16_filt_iirpart_kernel_den_x2,
      dsp16_filt_iirpart_kernel_den_x3,
      dsp16_filt_iirpart_kernel_den_x4,
      dsp16_filt_iirpart_kernel_den_x5,
      dsp16_filt_iirpart_kernel_den_x6,
      dsp16_filt_iirpart_kernel_den_x7
    };
#endif
  int n, m;
  dsp32_t sum;

  // Compute the numerator (it is a partial convolution)
  iirpart_kernel_opti_num[num_size&0x3](vect1, vect2, size - num_size + 1, num, num_size, num_prediv);

  // Compute the first den_size elements
  // to initialize the vect1
  for(n=1; n<den_size; n++)
  {
    sum = 0;
    for(m=0; m<n; m++)
      sum += den[m]*vect1[n-m-1];
    vect1[n] -= (sum >> (DSP16_QB - den_prediv));
  }

#if (DSP_OPTIMIZATION & DSP_OPTI_SIZE)
    // Change the size of the denominator to avoid the first coefficient
  --den_size;

    // Compute the denominator
  iirpart_kernel_opti_den_small(&vect1[n], vect1 + 1, size - num_size - den_size, &den[1], den_size, den_prediv);
#else
    // If den_size is odd
    // This is to make sure we have vect2 aligned on 32-bits
  sum = 0;
  for(m=0; m<den_size; m++)
      sum += den[m]*vect1[n-m-1];
  vect1[n] -= (sum >> (DSP16_QB - den_prediv));
  n++;

    // Compute the denominator
  iirpart_kernel_opti_den[den_size&0x7](&vect1[n], vect1 + 2, size - num_size - den_size, den, den_size, den_prediv);
#endif
}

#endif
