/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit NLMS filter function optimized for the AVR32 UC3.
 *
 * This file contains the code of the LMS filter.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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

#include "dsp.h"

#if !defined(FORCE_ALL_GENERICS) && \
    !defined(FORCE_GENERIC_FILT16_NLMS) && \
    defined(TARGET_SPECIFIC_FILT16_NLMS)

#include "filt_nlms.h"

//! @{
//! \verbatim

  .section  .bss, "aw"
dsp16_nlms_normalization:
  .word 0,0

  .section  .text, "ax"

  .balign 2

  // Reset vector: This must be linked @ 0x80000000.
  .global dsp16_filt_nlms
  .type dsp16_filt_nlms, @function

dsp16_filt_nlms:
    pushm    r0-r7, lr

    sub      lr, sp, -36
    bfexts   r5, r9, 0x0, 0x10
    mov      r6, r10
  // int i;
  // S32 temp;

  // x[0] = new_x;

  // temp = 0;
  // Performed a FIR
  // for(i=0; i<size; i+=4)
    mov      r9, 0
    ld.w     r4, lr[0x4]
    mov      r7, r11
    st.h     r12[0x0], r5
    casts.h  r8
    ld.w     lr, lr[0x0]
    mov      r10, r9
    cp.w     r6, 0
    brle     _dsp16_nlms_fir_end
  // {
  _dsp16_nlms_fir_loop:

    ld.d     r0, r7[r10<<0x1]
    ld.d     r2, r12[r10<<0x1]
    machh.w  r11, r0:t, r2:t
    machh.w  r11, r0:b, r2:b
    machh.w  r11, r1:t, r3:t
    machh.w  r11, r1:b, r3:b
    sub      r10, -4

    cp.w     r6, r10
    brgt     _dsp16_nlms_fir_loop

    bfexts   r10, r11, DSP16_QB, 0x10
    bfextu   r9, r10, 0x0, 0x10

  // *y = temp >> (DSP16_QB);

  // Error calculation
  // *e = d - *y;
  _dsp16_nlms_fir_end:

    sub      r9, r8, r9

  // Update the normalization coefficient
  // normalization += ((((S32) new_x)*((S32) new_x))) >> (DSP16_QB);
    mul      r8, r5, r5
    mov      r11, lo(dsp16_nlms_normalization)
    orh      r11, hi(dsp16_nlms_normalization)
    st.h     lr[0x0], r10
    casts.h  r9
    st.h     r4[0x0], r9
    ld.w     lr, r11[0x0]
    asr      r8, DSP16_QB
    add      lr, r8, lr
    st.w     r11[0x0], lr

  // if (!normalization)
    brne _dsp16_nlms_if_normalization
  //   normalization = 1;
    mov      lr, 1
    st.w     r11[0x0], lr

  _dsp16_nlms_if_normalization:
#if !(DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)
  // temp = ((S32) *e << DSP16_QB)/normalization;
    lsl      r9, DSP16_QB
    divs     r8, r9, lr
    mov      r10, -32768
    mov      r11, 32767
    max      r10, r10, r8
    min      r10, r10, r11
  // if (temp > 0x7FFF)
  //   temp = 0x7FFF;
  // else if (temp < -0x8000)
  //   temp = -0x8000;
#endif

  // Refresh the w coefficients
  // for(i=0; i<size; i+=2)
    cp.w     r6, 0
    brle     _dsp16_nlms_update_end
    mov      r8, 0
  // {
  _dsp16_nlms_update_loop:

#if (DSP_OPTIMIZATION & DSP_OPTI_ACCURACY)

  //  w[i] += (((((S32) *e)*((S32) x[i])))/normalization) >> (DSP_NLMS_MU - 1);
  //  w[i+1] += (((((S32) *e)*((S32) x[i+1])))/normalization) >> (DSP_NLMS_MU - 1);
  //  w[i+2] += (((((S32) *e)*((S32) x[i+2])))/normalization) >> (DSP_NLMS_MU - 1);
  //  w[i+3] += (((((S32) *e)*((S32) x[i+3])))/normalization) >> (DSP_NLMS_MU - 1);

    ld.d     r0, r7[r8<<0x1]
    ld.d     r2, r12[r8<<0x1]

    mulhh.w  r4, r9:b, r3:b
    divs     r10, r4, lr
    asr      r10, DSP_NLMS_MU - 1
    addhh.w  r4, r10:b, r1:b

    mulhh.w  r5, r9:b, r3:t
    divs     r10, r5, lr
    asr      r10, DSP_NLMS_MU - 1
    addhh.w  r5, r10:b, r1:t

    sthh.w   r7[r8<<0x1], r5:b, r4:b

    mulhh.w  r4, r9:b, r2:b
    divs     r10, r4, lr
    asr      r10, DSP_NLMS_MU - 1
    addhh.w  r4, r10:b, r0:b

    mulhh.w  r5, r9:b, r2:t
    divs     r10, r5, lr
    asr      r10, DSP_NLMS_MU - 1
    addhh.w  r5, r10:b, r0:t

    sub      r8, -2
    sthh.w   r7[r8<<0x1], r5:b, r4:b

    sub      r8, -2

#else

    ld.d     r0, r7[r8<<0x1]
    ld.d     r2, r12[r8<<0x1]

    mulhh.w  r4, r10:b, r3:b
    asr      r4, DSP_NLMS_MU - 1 + DSP16_QB
    addhh.w  r4, r4:b, r1:b

    mulhh.w  r5, r10:b, r3:t
    asr      r5, DSP_NLMS_MU - 1 + DSP16_QB
    addhh.w  r5, r5:b, r1:t

    sthh.w   r7[r8<<0x1], r5:b, r4:b

    mulhh.w  r4, r10:b, r2:b
    asr      r4, DSP_NLMS_MU - 1 + DSP16_QB
    addhh.w  r4, r4:b, r0:b

    mulhh.w  r5, r10:b, r2:t
    asr      r5, DSP_NLMS_MU - 1 + DSP16_QB
    addhh.w  r5, r5:b, r0:t

    sub      r8, -2
    sthh.w   r7[r8<<0x1], r5:b, r4:b

    sub      r8, -2

#endif

    cp.w     r6, r8
    brgt     _dsp16_nlms_update_loop

  // }

  // Update the normalization coefficient
  // normalization -= ((((S32) x[size-1])*((S32) x[size-1]))) >> (DSP16_QB);
  _dsp16_nlms_update_end:
    add      r9, r12, r6 << 0x1
    ld.sh    r8, r9[-2]
    mov      r9, lo(dsp16_nlms_normalization)
    orh      r9, hi(dsp16_nlms_normalization)

  // Shift the circular buffer
  // for(i=size-1; i>0;)
    sub      r10, r6, 1
    mul      r8, r8
    asr      r8, DSP16_QB
    sub      r11, lr, r8
    st.w     r9[0x0], r11
    cp.w     r10, 0
    brle     _dsp16_nlms_shift_end
  // {
  _dsp16_nlms_shift_loop:
    sub      lr, r10, 3
    ld.d     r0, r12[lr<<0x1]
    sub      r10, lr, 1
    ld.sh    r2, r12[r10<<0x1]
    sthh.w   r12[lr<<0x1], r2:b, r1:t
    sub      lr, -2
    sthh.w   r12[lr<<0x1], r1:b, r0:t
    cp.w     r10, 0
    brgt     _dsp16_nlms_shift_loop
  // }
  _dsp16_nlms_shift_end:

    popm     r0-r7, pc

//! \endverbatim
//! @}

#endif
