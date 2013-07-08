/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit LMS filter function optimized for the AVR32 UC3.
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
    !defined(FORCE_GENERIC_FILT16_LMS) && \
    defined(TARGET_SPECIFIC_FILT16_LMS)

#include "filt_lms.h"

//! @{
//! \verbatim



  .section  .text, "ax"

  .balign 2

  // Reset vector: This must be linked @ 0x80000000.
  .global dsp16_filt_lms
  .type dsp16_filt_lms, @function

dsp16_filt_lms:
    pushm    r0-r7, lr
    sub      lr, sp, -36

//  int i;
//  S32 temp;

//  x[0] = new_x;
    st.h     r12[0x0], r9
    bfexts   r9, r8, 0x0, 0x10
    mov      r8, 0

//  temp = 0;
//  Performed a FIR
//  for(i=0; i<size; i+=4)
//  {
    cp.w     r10, 0
    brle     _dsp16_lms_fir_end
    mov      r5, 0

    _dsp16_lms_fir_loop:
    ld.d     r0, r11[r5<<0x1]
    ld.d     r2, r12[r5<<0x1]
    machh.w  r8, r0:t, r2:t
    machh.w  r8, r0:b, r2:b
    machh.w  r8, r1:t, r3:t
    machh.w  r8, r1:b, r3:b
    sub      r5, -4
    cp.w     r10, r5
    brgt     _dsp16_lms_fir_loop
    _dsp16_lms_fir_end:

//  }
//  *y = temp >> (DSP16_QB);
    bfexts   r8, r8, DSP16_QB, 0x10

//  Error calculation
//  *e = d - *y;
    sub      r9, r8
    st.h     lr[0x4], r8
    casts.h  r9
    st.h     lr[0x0], r9
    mov      r8, 0

//  Refresh the w coefficients
//  for(i=0; i<size; i+=4)
//  {
    _dsp16_lms_w_loop:

    ld.d     r0, r11[r8<<0x1]
    ld.d     r2, r12[r8<<0x1]
    mulhh.w  r4, r9:b, r3:b
    asr      r4, DSP_LMS_MU - 1 + DSP16_QB
    addhh.w  r4, r4:b, r1:b
    mulhh.w  r5, r9:b, r3:t
    asr      r5, DSP_LMS_MU - 1 + DSP16_QB
    addhh.w  r5, r5:b, r1:t
    sthh.w   r11[r8<<0x1], r5:b, r4:b
    mulhh.w  r4, r9:b, r2:b
    asr      r4, DSP_LMS_MU - 1 + DSP16_QB
    addhh.w  r4, r4:b, r0:b
    mulhh.w  r5, r9:b, r2:t
    asr      r5, DSP_LMS_MU - 1 + DSP16_QB
    addhh.w  r5, r5:b, r0:t
    sub      r8, -2
    sthh.w   r11[r8<<0x1], r5:b, r4:b
    sub      r8, -2
    cp.w     r10, r8
    brgt     _dsp16_lms_w_loop
//  }


//  Shift the circular buffer
//  for(i=size-1; i>0;)
    sub      r10, 1
    cp.w     r10, 1
    brle     _dsp16_lms_shift_end
//  {
    _dsp16_lms_shift_loop:

    sub      lr, r10, 3
    ld.d     r0, r12[lr<<0x1]
    sub      r10, lr, 1
    ld.sh    r2, r12[r10<<0x1]
    sthh.w   r12[lr<<0x1], r2:b, r1:t
    sub      lr, -2
    sthh.w   r12[lr<<0x1], r1:b, r0:t
    cp.w     r10, 1
    brgt     _dsp16_lms_shift_loop
//  }
    _dsp16_lms_shift_end:
    popm     r0-r7, pc

//! \endverbatim
//! @}

#endif
