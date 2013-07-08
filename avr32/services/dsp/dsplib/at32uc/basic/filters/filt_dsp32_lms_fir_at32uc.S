/*****************************************************************************
 *
 * \file
 *
 * \brief 32-bit LMS filter function optimized for the AVR32 UC3.
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
    !defined(FORCE_GENERIC_FILT32_LMS) && \
    defined(TARGET_SPECIFIC_FILT32_LMS)

#include "filt_lms.h"

//! @{
//! \verbatim



  .section  .text, "ax"

  .balign 2

  // Reset vector: This must be linked @ 0x80000000.
  .global dsp32_filt_lms_fir
  .type dsp32_filt_lms_fir, @function

dsp32_filt_lms_fir:
    pushm    r0-r7, lr
    mov      r6, r8

    ld.w     lr, r9[0x0]
    mov      r6, 0
    mov      r4, r6
    mov      r5, r6
    cp.w     r10, 0
    brle     _dsp32_filt_lms_fir_end

    _dsp32_filt_lms_fir_loop:

    ld.d     r0, r11[r6<<0x2]
    ld.d     r2, r12[r6<<0x2]
    macs.d   r4, r0, r2
    macs.d   r4, r1, r3
    sub      r6, -2
    ld.d     r0, r11[r6<<0x2]
    ld.d     r2, r12[r6<<0x2]
    macs.d   r4, r0, r2
    macs.d   r4, r1, r3
    sub      r6, -2
    cp.w     r10, r6
    brgt     _dsp32_filt_lms_fir_loop

    _dsp32_filt_lms_fir_end:
    lsr      lr, r4, DSP32_QB
    or       lr, lr, r5<<DSP32_QA
    st.w     r9[0x0], lr

    ldm      sp++, r0-r7, pc

//! \endverbatim
//! @}

#endif
