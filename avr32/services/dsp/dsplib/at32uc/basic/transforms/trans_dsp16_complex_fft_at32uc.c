/*****************************************************************************
 *
 * \file
 *
 * \brief 16 bits AVR32 UC3 optimized version of the complex FFT algorithm.
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

/* Compute a Butterfly-4-entries
 * USED REGISTERS
 *    r3, r5, r6, r7, r8, r9, r10, r11, r12, lr
 * REQUIREMENT
 * Input:
 *    r0:t = e1_real
 *    r0:b = e1_imag
 *    r4 = vect1 + j + r
 *    r1 = m
 *    r2 = 3*m
 * Algorithm:
 *    a0_real = a0_real / 4 = r5:t
 *    a0_imag = a0_imag / 4 = r5:b
 *    a1_real = (a1_real*e_real - a1_imag*e_imag) / 4 = r8:t
 *    a1_imag = (a1_real*e_imag + a1_imag*e_real) / 4 = r9:t
 *    a2_real = (a2_real*e2_real - a2_imag*e2_imag) / 4 = r10:t
 *    a2_imag = (a2_real*e2_imag + a2_imag*e2_real) / 4 = r11:t
 *    a3_real = (a3_real*e3_real - a3_imag*e3_imag) / 4 = r12:t
 *    a3_imag = (a3_real*e3_imag + a3_imag*e3_real) / 4 = lr:t
 *
 *    vect1[i0].real = (a0_real + a2_real + (a1_real + a3_real))
 *    vect1[i2].imag = (a0_imag + a2_imag - (a1_imag + a3_imag))
 *    vect1[i1].real = (a0_real - a2_real + (a1_imag - a3_imag))
 *    vect1[i1].imag = (a0_imag - a2_imag - (a1_real - a3_real))
 *    vect1[i2].real = (a0_real + a2_real - (a1_real + a3_real))
 *    vect1[i2].imag = (a0_imag + a2_imag - (a1_imag + a3_imag))
 *    vect1[i3].real = (a0_real - a2_real - (a1_imag - a3_imag));
 *    vect1[i3].imag = (a0_imag - a2_imag + (a1_real - a3_real))
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "dsp.h"

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_TRANS16_COMPLEXFFT) && \
    defined(TARGET_SPECIFIC_TRANS16_COMPLEXFFT)

#include "trans_dsp16_twiddle_factors.h"

#if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)

#define M_BUTTERFLY4_COMPUT() \
        "ld.w  r5, r4[0]\n\t" \
        "mov  lr, -65536\n\t" \
        "ld.w  r6, r4[r1 << 1]\n\t" \
        "mulnhh.w  r12, r6:b, r0:b\n\t" \
        "machh.w  r12, r6:t, r0:t\n\t" \
        "and  r12, lr, r12 << "ASTRINGZ(16-DSP16_QB)"\n\t" \
        "mulhh.w  r9, r6:t, r0:b\n\t" \
        "machh.w  r9, r6:b, r0:t\n\t" \
        "or    r6, r12, r9 >> "ASTRINGZ(DSP16_QB)"\n\t" \
        "ld.d  r10, sp[0x1C]\n\t" \
        "ld.w  r8, r4[r2]\n\t" \
        "mulnhh.w  r12, r8:b, r10:b\n\t" \
        "machh.w  r12, r8:t, r10:t\n\t" \
        "and  r12, lr, r12 << "ASTRINGZ(16-DSP16_QB)"\n\t" \
        "mulhh.w  r9, r8:t, r10:b\n\t" \
        "machh.w  r9, r8:b, r10:t\n\t" \
        "or    r8, r12, r9 >> "ASTRINGZ(DSP16_QB)"\n\t" \
        "ld.w  r7, r4[r1]\n\t" \
        "mulnhh.w  r12, r7:b, r11:b\n\t" \
        "machh.w  r12, r7:t, r11:t\n\t" \
        "and  r12, lr, r12 << "ASTRINGZ(16-DSP16_QB)"\n\t" \
        "mulhh.w  r9, r7:t, r11:b\n\t" \
        "machh.w  r9, r7:b, r11:t\n\t" \
        "or    r7, r12, r9 >> "ASTRINGZ(DSP16_QB)"\n\t" \
        \
        "addhh.w  r9, r5:t, r7:t\n\t" \
        "addhh.w  r10, r6:t, r8:t\n\t" \
        "addhh.w  r11, r5:b, r7:b\n\t" \
        "addhh.w  r12, r6:b, r8:b\n\t" \
        \
        "add  lr, r9, r10\n\t" \
        "add  r3, r11, r12\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[0], lr:b, r3:b\n\t" \
        \
        "sub  lr, r9, r10\n\t" \
        "sub  r3, r11, r12\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[r1 << 1], lr:b, r3:b\n\t" \
        \
        "subhh.w  r9, r5:t, r7:t\n\t" \
        "subhh.w  r10, r6:t, r8:t\n\t" \
        "subhh.w  r11, r5:b, r7:b\n\t" \
        "subhh.w  r12, r6:b, r8:b\n\t" \
        \
        "add  lr, r9, r12\n\t" \
        "sub  r3, r11, r10\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[r1], lr:b, r3:b\n\t" \
        \
        "sub  lr, r9, r12\n\t" \
        "add  r3, r11, r10\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[r2], lr:b, r3:b\n\t"

#else

#define M_BUTTERFLY4_COMPUT() \
        "ld.w  r7, r4[r1 << 1]\n\t" \
        "mulnhh.w  r8, r7:b, r0:b\n\t" \
        "machh.w  r8, r7:t, r0:t\n\t" \
        "mulhh.w  r9, r7:t, r0:b\n\t" \
        "machh.w  r9, r7:b, r0:t\n\t" \
        \
        "ld.d  r6, sp[0x1C]\n\t" \
        "ld.w  r3, r4[r2]\n\t" \
        "mulnhh.w  r12, r3:b, r6:b\n\t" \
        "machh.w  r12, r3:t, r6:t\n\t" \
        "mulhh.w  lr, r3:t, r6:b\n\t" \
        "machh.w  lr, r3:b, r6:t\n\t" \
        \
        "ld.w  r3, r4[r1]\n\t" \
        "mulnhh.w  r10, r3:b, r7:b\n\t" \
        "machh.w  r10, r3:t, r7:t\n\t" \
        "mulhh.w  r11, r3:t, r7:b\n\t" \
        "machh.w  r11, r3:b, r7:t\n\t" \
        \
        "ld.w  r5, r4[0]\n\t" \
        "asr  r5, 2\n\t" \
        "bfexts  r3, r5, 0, 14\n\t" \
        "bfins  r5, r3, 0, 16\n\t" \
        \
        "add  r3, r8, r12\n\t" \
        "sub  r12, r8, r12\n\t" \
        "add  r6, r9, lr\n\t" \
        "sub  lr, r9, lr\n\t" \
        \
        "addhh.w  r8, r5:t, r10:t\n\t" \
        "subhh.w  r10, r5:t, r10:t\n\t" \
        "addhh.w  r9, r5:b, r11:t\n\t" \
        "subhh.w  r11, r5:b, r11:t\n\t" \
        \
        "addhh.w  r7, r8:b, r3:t\n\t" \
        "addhh.w  r5, r9:b, r6:t\n\t" \
        "sthh.w  r4[0], r7:b, r5:b\n\t" \
        \
        "subhh.w  r7, r8:b, r3:t\n\t" \
        "subhh.w  r5, r9:b, r6:t\n\t" \
        "sthh.w  r4[r1 << 1], r7:b, r5:b\n\t" \
        \
        "addhh.w  r7, r10:b, lr:t\n\t" \
        "subhh.w  r5, r11:b, r12:t\n\t" \
        "sthh.w  r4[r1], r7:b, r5:b\n\t" \
        \
        "subhh.w  r7, r10:b, lr:t\n\t" \
        "addhh.w  r5, r11:b, r12:t\n\t" \
        "sthh.w  r4[r2], r7:b, r5:b\n\t"

#endif
/* Compute a Butterfly-4-entries with no twiddle factors
 * USED REGISTERS
 *    r3, r5, r6, r7, r8, r9, r10, r11, r12, lr
 * REQUIREMENT
 * Input:
 *    r4 = vect1 + j + r
 *    r1 = m
 *    r2 = 3*m
 * Algorithm:
 *    vect1[i0].real = (a0_real + a2_real + (a1_real + a3_real)) / 4
 *    vect1[i2].imag = (a0_imag + a2_imag - (a1_imag + a3_imag)) / 4
 *    vect1[i1].real = (a0_real - a2_real + (a1_imag - a3_imag)) / 4
 *    vect1[i1].imag = (a0_imag - a2_imag - (a1_real - a3_real)) / 4
 *    vect1[i2].real = (a0_real + a2_real - (a1_real + a3_real)) / 4
 *    vect1[i2].imag = (a0_imag + a2_imag - (a1_imag + a3_imag)) / 4
 *    vect1[i3].real = (a0_real - a2_real - (a1_imag - a3_imag)) / 4
 *    vect1[i3].imag = (a0_imag - a2_imag + (a1_real - a3_real)) / 4
 */
#define M_BUTTERFLY4_ZERO_COMPUT() \
        "ld.w  r5, r4[0]\n\t" \
        "ld.w  r6, r4[r1 << 1]\n\t" \
        "ld.w  r8, r4[r2]\n\t" \
        "ld.w  r7, r4[r1]\n\t" \
        \
        "addhh.w  r9, r5:t, r7:t\n\t" \
        "addhh.w  r10, r6:t, r8:t\n\t" \
        "addhh.w  r11, r5:b, r7:b\n\t" \
        "addhh.w  r12, r6:b, r8:b\n\t" \
        \
        "add  lr, r9, r10\n\t" \
        "add  r3, r11, r12\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[0], lr:b, r3:b\n\t" \
        \
        "sub  lr, r9, r10\n\t" \
        "sub  r0, r11, r12\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r0, 2\n\t" \
        "sthh.w  r4[r1 << 1], lr:b, r0:b\n\t" \
        \
        "subhh.w  r9, r5:t, r7:t\n\t" \
        "subhh.w  r10, r6:t, r8:t\n\t" \
        "subhh.w  r11, r5:b, r7:b\n\t" \
        "subhh.w  r12, r6:b, r8:b\n\t" \
        \
        "add  lr, r9, r12\n\t" \
        "sub  r0, r11, r10\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r0, 2\n\t" \
        "sthh.w  r4[r1], lr:b, r0:b\n\t" \
        \
        "sub  lr, r9, r12\n\t" \
        "add  r0, r11, r10\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r0, 2\n\t" \
        "sthh.w  r4[r2], lr:b, r0:b\n\t"

/* Compute a Butterfly-4-entries with no twiddle factors and store data in bit reverse
 * USED REGISTERS
 *    r3, r5, r6, r7, r8, r9, r10, r11, r12, lr
 * REQUIREMENT
 * Input:
 *    r0 = vect2
 *    r1 = cpt
 *    r2 = 32 - nlog
 *    r12 = size/4
 *    r3 = 0
 *    r4 = vect1 + j + r
 * Algorithm:
 *    vect1[i0].real = (a0_real + a2_real + (a1_real + a3_real)) / 4
 *    vect1[i0].imag = 0
 *    vect1[i1].real = (a0_real - a2_real) / 4
*     vect1[i1].imag = - (a1_real - a3_real) / 4
*     vect1[i2].real = (a0_real + a2_real - (a1_real + a3_real))  / 4
*     vect1[i2].imag = 0
*     vect1[i3].real = (a0_real - a2_real) / 4
*     vect1[i3].imag = (a1_real - a3_real) / 4
 */

#define M_BITREV_BUTTERFLY4_ZERO_COMPUT() \
        "lsl  r9, r1, r2\n\t" \
        "brev  r9\n\t" \
        \
        "ld.w  r5, r11[r9 << 2]\n\t" \
        "add  r9, r12\n\t" \
        "ld.w  r6, r11[r9 << 2]\n\t" \
        "add  r9, r12\n\t" \
        "ld.w  r7, r11[r9 << 2]\n\t" \
        "add  r9, r12\n\t" \
        "ld.w  r8, r11[r9 << 2]\n\t" \
        \
        "addhh.w  r11, r5:t, r7:t\n\t" \
        "addhh.w  r12, r6:t, r8:t\n\t" \
        "addhh.w  r9, r5:b, r7:b\n\t" \
        "addhh.w  r10, r6:b, r8:b\n\t" \
        \
        "add  lr, r9, r10\n\t" \
        "add  r3, r11, r12\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[0], r3:b, lr:b\n\t" \
        \
        "sub  lr, r9, r10\n\t" \
        "sub  r3, r11, r12\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[8], r3:b, lr:b\n\t" \
        \
        "subhh.w  r11, r5:t, r7:t\n\t" \
        "subhh.w  r12, r6:t, r8:t\n\t" \
        "subhh.w  r9, r5:b, r7:b\n\t" \
        "subhh.w  r10, r6:b, r8:b\n\t" \
        \
        "sub  lr, r9, r12\n\t" \
        "add  r3, r11, r10\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[4], r3:b, lr:b\n\t" \
        \
        "add  lr, r9, r12\n\t" \
        "sub  r3, r11, r10\n\t" \
        "asr  lr, 2\n\t" \
        "asr  r3, 2\n\t" \
        "sthh.w  r4[12], r3:b, lr:b\n\t"


#if (defined __GNUC__)
#  define LO  "lo"
#  define HI  "hi"
#elif (defined __ICCAVR32__)
#  define LO  "LWRD"
#  define HI  "HWRD"
#endif

#if (defined __GNUC__)
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str) str
#elif (defined __ICCAVR32__)
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str) str":E"
#endif

//! avr32-uc3 16 bit version
#if (defined __GNUC__)
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
void dsp16_trans_complexfft(dsp16_complex_t *vect1, dsp16_complex_t *vect2, int nlog)
{

#if __ICCAVR32__
       // Force import of global symbols for inline assembly
       // Suppress warnings
# pragma diag_suppress=Pe174
       dsp16_twiddle_factors;
#if !(DSP_OPTIMIZATION & DSP_OPTI_SIZE)
       dsp16_twiddle_factors2;
#endif
       // Restore warnings
# pragma diag_default=Pe174
#endif

    // sp[0x1C]:8 = e_real | e_imag | e3_real | e3_imag
    // sp[0x18]:4 = m
    // sp[0x14]:4 = inc_indice
    // sp[0x10]:4 = indice
    // sp[0x0C]:4 = j
    // sp[0x08]:4 = vect1 + size
    // sp[0x04]:4 = vect1_saved
    // sp[0x00]:4 = nlog


  __asm__ __volatile__ (
        "pushm  r0-r7, lr\n\t"
        "sub  sp, 36\n\t"

        // sp[0x00] = nlog-2
        "sub  r0, r10, 2\n\t"
        "stdsp  sp[0x00], r0\n\t"

        // size = 1 << nlog;
        "mov  r1, 1\n\t"
        "lsl  r2, r1, r10\n\t"

        // r4 = vect1
        "mov  r4, r12\n\t"
        // save r4
        "stdsp  sp[0x04], r4\n\t"

        // vect1 + size*4
        "add  r0, r4, r2 << 2\n\t"
        "stdsp  sp[0x08], r0\n\t"

        // inc_indice = DSP16_N_TWIDDLE_FACTORS/4;
        "mov  r1, "ASTRINGZ(DSP16_N_TWIDDLE_FACTORS/4)"\n\t"
        "stdsp  sp[0x14], r1\n\t"

        // r12 = size/4
        "asr  r12, r2, 2\n\t"
        // r1 = cpt = 0
        "mov  r1, 0\n\t"
        // r3 = 0
        "mov  r3, 0\n\t"
        // r2 = 32 - nlog
        "rsub  r2, r10, 32\n\t"
        // r0 = vect1 + size*4
        // r11 = vect1
        "stdsp  sp[0x10], r12\n\t"
        "stdsp  sp[0x0C], r11\n"
    // for(r=0; r<size; r+=m)
    "__loop_init:\n\t"

        "lddsp  r12, sp[0x10]\n\t"
        "lddsp  r11, sp[0x0C]\n\t"

        // ****************************************

      M_BITREV_BUTTERFLY4_ZERO_COMPUT()

        // ****************************************

        // r1 += 4
        "sub  r1, r1, -4\n\t"
        // for(r=0; r<size; r+=16)
        // vect1 += m*4
        "sub  r4, r4, -16\n\t"
        "cp.w  r0, r4\n\t"
        // if (size > r + m) -> branch
        "brgt  __loop_init\n\t"


        // r4 = vect1
        "lddsp  r4, sp[0x04]\n\t"

        "mov  r1, 4\n\t"
        "stdsp  sp[0x18], r1\n"

    // for(stage=2; stage <= nlog; stage+=2)
    "__loop_stage:\n\t"

        "lddsp  r1, sp[0x18]\n\t"
        // m <<= 2;
        "lsl  r1, r1, 2\n\t"
        "stdsp  sp[0x18], r1\n\t"

        // r9 = indice = 0;
        "mov  r0, 0\n\t"
        "stdsp  sp[0x10], r0\n\t"

/****************************************************************/

        // push r4
        "lddsp  r4, sp[0x04]\n\t"

        // r2 = 3*m
        "mul  r2, r1, 3\n"

    // for(r=0; r<size; r+=m)
    "__loop_r1:\n\t"

        // ****************************************

      M_BUTTERFLY4_ZERO_COMPUT()

        // ****************************************

        // for(r=0; r<size; r+=m)
        // r1 = m

        // vect1 += m*4
        "add  r4, r4, r1 << 2\n\t"

        "lddsp  r8, sp[0x08]\n\t"
        "cp.w  r8, r4\n\t"

        // if (size > r + m) -> branch
        "brgt  __loop_r1\n\t"


        // r1 = m
        // if (m <= 4) -> branch
        "cp.w  r1, 4\n\t"
        "brls  __loop_stage_end\n\t"


        // j = 4
        "mov  r9, 4\n\t"

        "stdsp  sp[0x0C], r9\n"


/***************************************************************/

    // for(j=0; j<m; j += 4)
    "__loop_j:\n\t"

        // r9 = j
        // r4 = vect1
        "lddsp  r4, sp[0x04]\n\t"

        // r4 = vect1 + j
        "add  r4, r9\n\t"


        // indice += inc_indice;
        "lddsp  r1, sp[0x14]\n\t"
        // r9 = indice
        "lddsp  r9, sp[0x10]\n\t"
        "add  r9, r1\n\t"
        "stdsp  sp[0x10], r9\n\t"

        // r0 = *(twiddle_factors + r9)
        "mov  r1, "LO"("ASTRINGZ(dsp16_twiddle_factors)")\n\t"
        "orh  r1, "HI"("ASTRINGZ(dsp16_twiddle_factors)")\n\t"
        "ld.w  r0, r1[r9]\n\t"

#if (DSP_OPTIMIZATION & DSP_OPTI_SIZE)

        //if (indice < (DSP16_N_TWIDDLE_FACTORS)/2) -> branch
        "cp.w  r9, "ASTRINGZ(DSP16_N_TWIDDLE_FACTORS/2)"\n\t"
        ASM_INSTRUCT_COMPACKED("brlt  __address_indice_not_overflow")"\n\t"

        // r = DSP16_N_TWIDDLE_FACTORS*2 - (indice << 1);
        "mov  r8, "ASTRINGZ(DSP16_N_TWIDDLE_FACTORS*2)"\n\t"
        "sub  r10, r8, r9 << 1\n\t"

        // r7:t = -e2_real & r7:b = e2_imag
        "ld.w  r8, r1[r10]\n\t"

        // r7 = (-r8:t << 16) + r8:b
#  if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
        "lsr  r7, r8, 16\n\t"
        "neg  r7\n\t"
        "bfins  r7, r8, 16, 16\n\t"
        "swap.h  r7\n\t"
#  else
        "mov  r7, r8\n\t"
        "neg  r7\n\t"
        "bfins  r7, r8, 0, 16\n\t"
#  endif  // DSP_OPTIMIZATION & DSP_OPTI_ACCURACY

        // if (r >= DSP16_N_TWIDDLE_FACTORS*2/3) -> branch

        "cp.w  r10, "ASTRINGZ((DSP16_N_TWIDDLE_FACTORS*2)/3)"\n\t"
        ASM_INSTRUCT_COMPACKED("brge  __address_r_not_overflow1")"\n\t"

        //r = indice - r;
        "sub  r10, r9, r10\n\t"
        // r8:t = -e3_real & r8:b = -e3_imag
        "ld.w  r6, r1[r10]\n\t"

        // r6 = (-r8:t << 16) + -r8:b
#  if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
        "lsr  r8, r6, 16\n\t"
        "neg  r6\n\t"
        "neg  r8\n\t"
        "bfins  r6, r8, 16, 16\n\t"
#  else
        "neg  r6\n\t"
#  endif  // DSP_OPTIMIZATION & DSP_OPTI_ACCURACY

        "bral  __address_end_twiddle_factors\n"
    "__address_r_not_overflow1:\n\t"

        //r -= indice;
        "sub  r10, r10, r9\n\t"

        // r8:t = -e3_real & r8:b = e3_imag
        "ld.w  r8, r1[r10]\n\t"

        // r6 = (-r8:t << 16) + r8:b
#  if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
        "lsr  r6, r8, 16\n\t"
        "neg  r6\n\t"
        "bfins  r6, r8, 16, 16\n\t"
        "swap.h  r6\n\t"
#  else
        "mov  r6, r8\n\t"
        "neg  r6\n\t"
        "bfins  r6, r8, 0, 16\n\t"
#  endif  // DSP_OPTIMIZATION & DSP_OPTI_ACCURACY
        "bral  __address_end_twiddle_factors\n"
    "__address_indice_not_overflow:\n\t"

        // e2 = twiddle_factors[indice << 1];
        "ld.w  r7, r1[r9 << 1]\n\t"

        // if (r < DSP16_N_TWIDDLE_FACTORS/3) -> branch
        "cp.w  r9, "ASTRINGZ(DSP16_N_TWIDDLE_FACTORS/3)"\n\t"

        ASM_INSTRUCT_COMPACKED("brlt  __address_r_not_overflow2")"\n\t"

        // r = DSP16_N_TWIDDLE_FACTORS*2 - (indice + (r << 1));
        "mov  r8, "ASTRINGZ(DSP16_N_TWIDDLE_FACTORS*2)"\n\t"
        "sub  r8, r8, r9\n\t"
        "sub  r10, r8, r9 << 1\n\t"

        // r8:t = -e3_real & r8:b = e3_imag
        "ld.w  r8, r1[r10]\n\t"

#  if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
        "lsr  r6, r8, 16\n\t"
        "neg  r6\n\t"
        "bfins  r6, r8, 16, 16\n\t"
        "swap.h  r6\n\t"
#  else
        "mov  r6, r8\n\t"
        "neg  r6\n\t"
        "bfins  r6, r8, 0, 16\n\t"
#  endif  // DSP_OPTIMIZATION & DSP_OPTI_ACCURACY

        "bral  __address_end_twiddle_factors\n"
    "__address_r_not_overflow2:\n\t"

        "add  r10, r9, r9 << 1\n\t"
        "ld.w  r6, r1[r10]\n"

    "__address_end_twiddle_factors:\n\t"

        "st.d  sp[0x1C], r6\n\t"
#else

        // r1 = twiddle_factors
        "mov  r1, "LO"("ASTRINGZ(dsp16_twiddle_factors2)")\n\t"
        "orh  r1, "HI"("ASTRINGZ(dsp16_twiddle_factors2)")\n\t"
        // r7:t = twiddle_factors2[indice]
        // r7:b = twiddle_factors2[indice+1]
        // r6:t = twiddle_factors2[indice+2]
        // r6:b = twiddle_factors2[indice+3]
        "ld.d  r6, r1[r9 << 1]\n\t"
        "st.d  sp[0x1C], r6\n\t"

#endif  // DSP_OPTIMIZATION & DSP_OPTI_SIZE

        // r5 = m
        "lddsp  r1, sp[0x18]\n"

    // for(r=0; r<size; r+=m)
    "__loop_r2:\n\t"

        // ****************************************

      M_BUTTERFLY4_COMPUT()

        // ****************************************

        // r1 = m
        // r4 += m*4
        "add  r4, r4, r1 << 2\n\t"

        // r8 = size + vect1
        "lddsp  r8, sp[0x08]\n\t"
        "cp.w  r8, r4\n\t"
        // if (size + vect1 > r4) -> branch
        "brgt  __loop_r2\n\t"


        // for(j=0; j<m4; j++)
        // r1 = j += 4
        "lddsp  r9, sp[0x0C]\n\t"
        "sub  r9, -4\n\t"
        "stdsp  sp[0x0C], r9\n\t"
        // r5 = m
        "lddsp  r5, sp[0x18]\n\t"

        // if (m > j+4) -> branch
        "cp.w  r5, r9\n\t"
        "brgt  __loop_j\n"

    "__loop_stage_end:\n\t"

        // inc_indice >>= 2;
        "lddsp  r1, sp[0x14]\n\t"
        "lsr  r1, r1, 2\n\t"
        "stdsp  sp[0x14], r1\n\t"

        // for(stage=2; stage <= nlog; stage+=2)
        "lddsp  r1, sp[0x00]\n\t"
        "sub  r1, 2\n\t"
        "stdsp  sp[0x00], r1\n\t"
        "brgt  __loop_stage\n"

    "__address_end:\n\t"
        "sub  sp, -36\n\t"
        "popm  r0-r7, pc\n\t"
#if (defined __GNUC__)
        :
        :
#  if !(DSP_OPTIMIZATION & DSP_OPTI_SIZE)
        [VAR_TWIDDLE_FACTORS] "i" (dsp16_twiddle_factors),
        [VAR_TWIDDLE_FACTORS2] "i" (dsp16_twiddle_factors2)
#  else
        [VAR_TWIDDLE_FACTORS] "i" (dsp16_twiddle_factors)
#  endif
#endif
    );
}

#endif
