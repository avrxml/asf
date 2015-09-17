/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit IIR filter function optimized for the AVR32 UC3.
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

#include "preprocessor.h"

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_FILT32_IIR) && \
    defined(TARGET_SPECIFIC_FILT32_IIR)

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
#define DSP32_COMPUT_TAP_8(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 32\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 2\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[24]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[16]\n\t" \
        "ld.d  r6, r2[8]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, r2[16]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.d  r6, r2[24]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "sub  "ASTRINGZ(r_i)", -8\n\t"
/*********************************************************************************************/

/*********************************************************************************************
 * Macro name: DSP32_COMPUT_TAP_ENDING_X
 * Used registers:
 *   r4, r5, r6, r7, r2
 * Description:
 *   sum1 += h(i)*vect1(i) + h(i+1)*vect1(i+1) + ...
 *   where X is the number of iteration
 *********************************************************************************************/
#define DSP32_COMPUT_TAP_ENDING_0(r_vect1, r_h, r_i, r_sum1)

#define DSP32_COMPUT_TAP_ENDING_1(r_vect1, r_h, r_i, r_sum1) \
        "ld.w  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.w  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 2]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r4, r6\n\t"

#define DSP32_COMPUT_TAP_ENDING_2(r_vect1, r_h, r_i, r_sum1) \
        "ld.d  r4, --"ASTRINGZ(r_h)"\n\t" \
        "ld.d  r6, "ASTRINGZ(r_vect1)"["ASTRINGZ(r_i)" << 2]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t"


#define DSP32_COMPUT_TAP_ENDING_3(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 12\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 2\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[4]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.w  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.w  r6, r2[8]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r4, r6\n\t"


#define DSP32_COMPUT_TAP_ENDING_4(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 16\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 2\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.d  r6, r2[8]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t"


#define DSP32_COMPUT_TAP_ENDING_5(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 20\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 2\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[12]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[4]\n\t" \
        "ld.d  r6, r2[8]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.w  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.w  r6, r2[16]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r4, r6\n\t"

#define DSP32_COMPUT_TAP_ENDING_6(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 24\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 2\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[16]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[8]\n\t" \
        "ld.d  r6, r2[8]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.d  r6, r2[16]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t"

#define DSP32_COMPUT_TAP_ENDING_7(r_vect1, r_h, r_i, r_sum1) \
        "sub  "ASTRINGZ(r_h)", 28\n\t" \
        "add  r2, "ASTRINGZ(r_vect1)", "ASTRINGZ(r_i)" << 2\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[20]\n\t" \
        "ld.d  r6, r2[0]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[12]\n\t" \
        "ld.d  r6, r2[8]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.d  r4, "ASTRINGZ(r_h)"[4]\n\t" \
        "ld.d  r6, r2[16]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r5, r6\n\t" \
        "macs.d  "ASTRINGZ(r_sum1)", r4, r7\n\t" \
\
        "ld.w  r4, "ASTRINGZ(r_h)"[0]\n\t" \
        "ld.w  r6, r2[24]\n\t" \
\
        "macs.d  "ASTRINGZ(r_sum1)", r4, r6\n\t"
/*********************************************************************************************/

#if __GNUC__
#  define DSP32_IIR_NUM_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP32_IIR_NUM_KERNEL_X_FCT__(x_num, data)
#  define DSP32_IIR_DEN_KERNEL_X_FCT(x_num, data)  __attribute__((__naked__)) DSP32_IIR_DEN_KERNEL_X_FCT__(x_num, data)
#elif __ICCAVR32__
#  define DSP32_IIR_NUM_KERNEL_X_FCT(x_num, data)  DSP32_IIR_NUM_KERNEL_X_FCT__(x_num, data)
#  define DSP32_IIR_DEN_KERNEL_X_FCT(x_num, data)  DSP32_IIR_DEN_KERNEL_X_FCT__(x_num, data)
#endif


/*********************************************************************************************
 * Algorithm:
 *   for(n=0; n<vect1_size; n++)
 *   {
 *        sum = 0;
 *        for(m=0; m<vect3_size; m+=8)
 *              REPx8(sum += vect3[m]*vect2[n-m];)
 *        REPx(x_num, sum += vect3[m]*vect2[n-m];)
 *        vect1[n++] = sum >> (DSP16_QB - prediv);
 *   }
 * Used registers:
 *   all
 * REGISTERS:
 * before the main loop:
 *   r12 = vect2
 *   r11 = vect1 = sp[0x04]
 *   r10 = vect1_size = sp[0x00]
 *   r9 = vect3
 *   r8 = vect3_size
 * inside the loops:
 *   r12 = vect2
 *   r9 = vect3
 *   r8 = vect3_size
 *   r1 = sum:t
 *   r0 = sum:b
 *   lr = i
 * Condition: h, vect1 and vect2 must be word-align
 *********************************************************************************************/

#if __GNUC__
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str) str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str) str":E"
#endif

#define DSP32_IIR_NUM_KERNEL_X_FCT__(x_num, data)    \
static void TPASTE2(dsp32_filt_iir_num_kernel_x, x_num)(dsp32_t *vect1, dsp32_t *vect2, int vect1_size, dsp32_t *vect3, int vect3_size) \
{ \
  __asm__ __volatile__ ( \
        "pushm  r0-r7, lr\n\t" \
        "sub  sp, 12\n\t" \
\
        "mov  r1,  r11\n\t" \
        "mov  r11, r12\n\t" \
        "mov  r12, r1\n\t" \
\
        "add  r9,  r9, r8 << 2\n\t" \
        "stdsp  sp[0x08], r9\n\t" \
\
        "sub  r8, 7\n" \
\
    "__dsp32_iir_num_loop_main"ASTRINGZ(x_num)":\n\t" \
        "stdsp  sp[0x04], r11\n\t" \
        "stdsp  sp[0x00], r10\n\t" \
        "lddsp  r9, sp[0x08]\n\t" \
\
        "mov  r0, 0\n\t" \
        "mov  r1, 0\n\t" \
        "mov  lr, r0\n"  \
\
     "__dsp32_iir_num_loop_tap"ASTRINGZ(x_num)":\n\t" \
\
        "cp.h  lr, r8\n\t" \
      ASM_INSTRUCT_COMPACKED("brge  __dsp32_iir_num_endloop_tap"ASTRINGZ(x_num))"\n\t" \
\
      DSP32_COMPUT_TAP_8(r12, r9, lr, r0) \
\
        "bral  __dsp32_iir_num_loop_tap"ASTRINGZ(x_num)"\n" \
    "__dsp32_iir_num_endloop_tap"ASTRINGZ(x_num)":\n\t" \
\
      TPASTE2(DSP32_COMPUT_TAP_ENDING_, x_num)(r12, r9, lr, r0) \
\
        "lsl  r0, "ASTRINGZ(DSP32_QB)"\n\t" \
        "bfins  r0, r1, "ASTRINGZ(32-DSP32_QB)", "ASTRINGZ(DSP32_QB)"\n\t" \
\
        "lddsp  r11, sp[0x04]\n\t" \
        "st.w  r11++, r0\n\t" \
\
        "sub  r12, -4\n\t" \
\
        "lddsp  r10, sp[0x00]\n\t" \
        "sub  r10, 1\n\t" \
        "brgt  __dsp32_iir_num_loop_main"ASTRINGZ(x_num)"\n\t" \
\
        "sub  sp, -12\n\t" \
        "popm  r0-r7, pc\n\t" \
    ); \
}
/*********************************************************************************************/

/*********************************************************************************************
 * Algorithm:
 *   for(n=0; n<vect1_size; n++)
 *   {
 *        sum = 0;
 *        for(m=0; m<vect3_size; m+=8)
 *              REPx8(sum += vect3[m]*vect2[n-m];)
 *        REPx(x_num, sum += vect3[m]*vect2[n-m];)
 *        vect1[n++] -= sum >> (DSP16_QB - prediv);
 *   }
 * Used registers:
 *   all
 * REGISTERS:
 * before the main loop:
 *   r12 = vect2
 *   r11 = vect1 = sp[0x04]
 *   r10 = vect1_size = sp[0x00]
 *   r9 = vect3
 *   r8 = vect3_size
 * inside the loops:
 *   r12 = vect2
 *   r9 = vect3
 *   r8 = vect3_size
 *   r1 = sum:t
 *   r0 = sum:b
 *   lr = i
 * Condition: h, vect1 and vect2 must be word-align
 *********************************************************************************************/
#define DSP32_IIR_DEN_KERNEL_X_FCT__(x_num, data)    \
static void TPASTE2(dsp32_filt_iir_den_kernel_x, x_num)(dsp32_t *vect1, dsp32_t *vect2, int vect1_size, dsp32_t *vect3, int vect3_size, int prediv) \
{ \
  __asm__ __volatile__ ( \
        "pushm  r0-r7, lr\n\t" \
        "sub  sp, 12\n\t" \
\
        "mov  r1,  r11\n\t" \
        "mov  r11, r12\n\t" \
        "mov  r12, r1\n\t" \
\
        "lddsp  r3, sp[48]\n\t" \
        "rsub  r3, r3, "ASTRINGZ(DSP32_QB)"\n\t" \
\
        "add  r9,  r9, r8 << 2\n\t" \
        "stdsp  sp[0x08], r9\n\t" \
\
        "sub  r8, 7\n" \
\
    "__dsp32_iir_den_loop_main"ASTRINGZ(x_num)":\n\t" \
        "stdsp  sp[0x04], r11\n\t" \
        "stdsp  sp[0x00], r10\n\t" \
        "lddsp  r9, sp[0x08]\n\t" \
\
        "mov  r0, 0\n\t" \
        "mov  r1, 0\n\t" \
        "mov  lr, r0\n"  \
\
    "__dsp32_iir_den_loop_tap"ASTRINGZ(x_num)":\n\t" \
\
        "cp.h  lr, r8\n\t" \
      ASM_INSTRUCT_COMPACKED("brge  __dsp32_iir_den_endloop_tap"ASTRINGZ(x_num))"\n\t" \
\
      DSP32_COMPUT_TAP_8(r12, r9, lr, r0) \
\
        "bral  __dsp32_iir_den_loop_tap"ASTRINGZ(x_num)"\n" \
    "__dsp32_iir_den_endloop_tap"ASTRINGZ(x_num)":\n\t" \
\
      TPASTE2(DSP32_COMPUT_TAP_ENDING_, x_num)(r12, r9, lr, r0) \
\
        "lsr  r0, r0, r3\n\t" \
        "rsub  r2, r3, 32\n\t" \
        "lsl  r1, r1, r2\n\t" \
        "or    r0, r1\n\t" \
\
        "lddsp  r11, sp[0x04]\n\t" \
        "ld.w  r2, r11[0x0]\n\t" \
        "sub  r2, r0\n\t" \
        "st.w  r11++, r2\n\t" \
\
        "sub  r12, -4\n\t" \
\
        "lddsp  r10, sp[0x00]\n\t" \
        "sub  r10, 1\n\t" \
        "brgt  __dsp32_iir_den_loop_main"ASTRINGZ(x_num)"\n\t" \
\
        "sub  sp, -12\n\t" \
        "popm  r0-r7, pc\n\t" \
    ); \
}
/*********************************************************************************************/


DSP32_IIR_NUM_KERNEL_X_FCT(0, )
DSP32_IIR_NUM_KERNEL_X_FCT(1, )
DSP32_IIR_NUM_KERNEL_X_FCT(2, )
DSP32_IIR_NUM_KERNEL_X_FCT(3, )
DSP32_IIR_NUM_KERNEL_X_FCT(4, )
DSP32_IIR_NUM_KERNEL_X_FCT(5, )
DSP32_IIR_NUM_KERNEL_X_FCT(6, )
DSP32_IIR_NUM_KERNEL_X_FCT(7, )


DSP32_IIR_DEN_KERNEL_X_FCT(0, )
DSP32_IIR_DEN_KERNEL_X_FCT(1, )
DSP32_IIR_DEN_KERNEL_X_FCT(2, )
DSP32_IIR_DEN_KERNEL_X_FCT(3, )
DSP32_IIR_DEN_KERNEL_X_FCT(4, )
DSP32_IIR_DEN_KERNEL_X_FCT(5, )
DSP32_IIR_DEN_KERNEL_X_FCT(6, )
DSP32_IIR_DEN_KERNEL_X_FCT(7, )

void dsp32_filt_iir(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t *num, int num_size, dsp32_t *den, int den_size, int prediv)
{
  typedef void (*iir_kernel_opti_num_t)(dsp32_t *, dsp32_t *, int, dsp32_t *, int);
  static const iir_kernel_opti_num_t iir_kernel_opti_num[8] = {
      dsp32_filt_iir_num_kernel_x0,
      dsp32_filt_iir_num_kernel_x1,
      dsp32_filt_iir_num_kernel_x2,
      dsp32_filt_iir_num_kernel_x3,
      dsp32_filt_iir_num_kernel_x4,
      dsp32_filt_iir_num_kernel_x5,
      dsp32_filt_iir_num_kernel_x6,
      dsp32_filt_iir_num_kernel_x7
    };
  typedef void (*iir_kernel_opti_den_t)(dsp32_t *, dsp32_t *, int, dsp32_t *, int, int);
  static const iir_kernel_opti_den_t iir_kernel_opti_den[8] = {
      dsp32_filt_iir_den_kernel_x0,
      dsp32_filt_iir_den_kernel_x1,
      dsp32_filt_iir_den_kernel_x2,
      dsp32_filt_iir_den_kernel_x3,
      dsp32_filt_iir_den_kernel_x4,
      dsp32_filt_iir_den_kernel_x5,
      dsp32_filt_iir_den_kernel_x6,
      dsp32_filt_iir_den_kernel_x7
    };
  int n, m;
  S64 sum;

  // Compute the numerator (it is a partial convolution)
  iir_kernel_opti_num[num_size&0x7](vect1, vect2, size - num_size + 1, num, num_size);

  // Compute the first den_size elements
  // to initialize the vect1
  for(n=0; n<den_size; n++)
  {
    sum = 0;
    for(m=1; m<=n; m++)
      sum += ((S64) den[m])*((S64) vect1[n-m]);
    vect1[n] -= sum >> (DSP32_QB - prediv);
  }

  den_size--;

  // Compute the denominator
  iir_kernel_opti_den[den_size&0x7](&vect1[n], vect1 + 1, size - num_size - den_size, &den[1], den_size, prediv);
}

#endif
