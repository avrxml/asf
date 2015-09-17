/*****************************************************************************
 *
 * \file
 *
 * \brief Square root operator for the AVR32 UC3.
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
    !defined(FORCE_GENERIC_OP16_LN) && \
    defined(TARGET_SPECIFIC_OP16_LN)

#if (defined __GNUC__)
#  define ASM_INSTRUCT_COMPACKED(str) str
#  define ASM_INSTRUCT_EXTENDED(str) str
#elif __ICCAVR32__
#  define ASM_INSTRUCT_COMPACKED(str) str":C"
#  define ASM_INSTRUCT_EXTENDED(str) str":E"
#endif

// Constants
#if (defined __GNUC__)
# define CST_MIN              "%[CST_MIN__]"
# define CST_ONE              "%[CST_ONE__]"
# define CST_LESS_ONE         "%[CST_LESS_ONE__]"
# define CST_INV_NINE         "%[CST_INV_NINE__]"
# define CST_LESS_INV_SEVEN   "%[CST_LESS_INV_SEVEN__]"
# define CST_LESS_INV_FIVE    "%[CST_LESS_INV_FIVE__]"
# define CST_LESS_INV_THREE   "%[CST_LESS_INV_THREE__]"
# define CST_LN_TWO           "%[CST_LN_TWO__]"
#elif __ICCAVR32__
# define CST_MIN              ASTRINGZ(-1 << (DSP16_QA+DSP16_QB-1))
# if DSP16_QB >= 15
#   define CST_ONE              ASTRINGZ((1 << 15) - 1)
# else
#   define CST_ONE              ASTRINGZ(1 << DSP16_QB)
# endif
# define CST_LESS_ONE         ASTRINGZ(-1 << DSP16_QB)
# define CST_INV_NINE         ASTRINGZ(0x00000E38 >> (DSP16_QA - 1))
# define CST_LESS_INV_SEVEN   ASTRINGZ(0xFFFFEDB7 >> (DSP16_QA - 1))
# define CST_LESS_INV_FIVE    ASTRINGZ(0xFFFFE667 >> (DSP16_QA - 1))
# define CST_LESS_INV_THREE   ASTRINGZ(0xFFFFD556 >> (DSP16_QA - 1))
# define CST_LN_TWO           ASTRINGZ(0x000058B9 >> (DSP16_QA - 1))
#endif

// Natural logarithm
// for fixed point numbers
//! avr32-uc3 16 bit version
#if (defined __GNUC__)
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
S32 dsp16_op_ln_raw(dsp16_t num)
{
  __asm__ __volatile__ (
    "pushm  r0-r3, lr\n\t"

    // Limit
    "cp.w   r12, 0\n\t"
    ASM_INSTRUCT_COMPACKED("brlt   __dsp16_sqrt_end_err")"\n\t"

    // get ~ int(log2(num))
    "clz    r0, r12\n\t"
    "sub    r0, r0, "ASTRINGZ(32 - DSP16_QB)"\n\t"

    // now r12 is in the range [1; 0.5]
    "lsl    r12, r12, r0\n\t"

    // << (DSP16_QB - 1) to avoid overflows
    "sub    r1, r12, "CST_ONE"\n\t"
    "lsl    r1, r1, "ASTRINGZ(DSP16_QB)"\n\t"
    "sub    r2, r12, "CST_LESS_ONE"\n\t"
    // r2 = a = (1-num)/(1+num)
    "divs   r2, r1, r2\n\t"

    // calculate a^2
    "mul    r1, r2, r2\n\t"
    // r1 = a_sqr = a*a
    "asr    r1, r1, "ASTRINGZ(DSP16_QB)"\n\t"

    // r3 = (a_sqr*DSP16_Q(1./9.)) >> DSP16_QB;
    "mov    r3, "CST_INV_NINE"\n\t"
    "mul    r3, r1, r3\n\t"
    "asr    r3, r3, "ASTRINGZ(DSP16_QB)"\n\t"
    // r3 = (a_sqr*(r3 + DSP16_Q(1./7.))) >> DSP16_QB;
    "sub    r3, r3, "CST_LESS_INV_SEVEN"\n\t"
    "mul    r3, r1, r3\n\t"
    "asr    r3, r3, "ASTRINGZ(DSP16_QB)"\n\t"
    // r3 = (a_sqr*(r3 + DSP16_Q(1./5.))) >> DSP16_QB;
    "sub    r3, r3, "CST_LESS_INV_FIVE"\n\t"
    "mul    r3, r1, r3\n\t"
    "asr    r3, r3, "ASTRINGZ(DSP16_QB)"\n\t"
    // r3 = (a_sqr*(r3 + DSP16_Q(1./3.))) >> DSP16_QB;
    "sub    r3, r3, "CST_LESS_INV_THREE"\n\t"
    "mul    r3, r1, r3\n\t"
    "asr    r3, r3, "ASTRINGZ(DSP16_QB)"\n\t"
    // r3 = (a*(r3 + DSP16_Q(1.))) >> (DSP16_QB - 1);
    "sub    r3, r3, "CST_LESS_ONE"\n\t"
    "mul    r3, r2, r3\n\t"
    "asr    r3, r3, "ASTRINGZ(DSP16_QB - 1)"\n\t"

    // r3 -= ((S32) r0)*((S32) DSP16_Q(LN_2));
    "mov    r1, "CST_LN_TWO"\n\t"
    "mul    r0, r0, r1\n\t"
    "sub    r12, r3, r0\n\t"

    "__dsp16_sqrt_end_fct:\n\t"
    "popm   r0-r3, pc\n"

    // Return -MIN if num < 0
    "__dsp16_sqrt_end_err:\n\t"
    "mov    r12, "CST_MIN"\n\t"
    "popm   r0-r3, pc\n\t"
#if (defined __GNUC__)
    :
    :
    [CST_MIN__] "i" (DSP_Q_MIN(DSP16_QA, DSP16_QB)),
    [CST_ONE__] "i" (DSP16_Q(1.)),
    [CST_LESS_ONE__] "i" (DSP16_Q(-1.)),
    [CST_INV_NINE__] "i" (DSP16_Q(1./9.)),
    [CST_LESS_INV_SEVEN__] "i" (DSP16_Q(-1./7.)),
    [CST_LESS_INV_FIVE__] "i" (DSP16_Q(-1./5.)),
    [CST_LESS_INV_THREE__] "i" (DSP16_Q(-1./3.)),
    [CST_LN_TWO__] "i" (DSP16_Q(CST_LN_2))
#endif
  );

  return 0;
}

#endif
