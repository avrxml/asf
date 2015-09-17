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
    !defined(FORCE_GENERIC_OP16_SQRT) && \
    defined(TARGET_SPECIFIC_OP16_SQRT)

#define DSP16_SQRT_ONE_POINT_FIVE   (3 << (DSP16_QB-1))

#define DSP16_SQRT_NEWTON_ITERATION(x_num, data) \
  "mul    r1, r0, r0\n\t" \
  "mul    r1, r12, r1\n\t" \
  "lsr    r1, r1, "ASTRINGZ(DSP16_QB+1)"\n\t" \
  "sub    r1, r2, r1\n\t" \
  "mul    r0, r0, r1\n\t" \
  "lsr    r0, r0, "ASTRINGZ(DSP16_QB)"\n\t"

#if (defined __GNUC__)
# define ASM_INSTRUCT_COMPACKED(str) str
# define ASM_INSTRUCT_EXTENDED(str) str
#elif __ICCAVR32__
# define ASM_INSTRUCT_COMPACKED(str) str":C"
# define ASM_INSTRUCT_EXTENDED(str) str":E"
#endif

// Constants
#if (defined __GNUC__)
# define CST_ZERO  "%[CST_ZERO__]"
#elif __ICCAVR32__
# define CST_ZERO  ASTRINGZ(0)
#endif

// Square root using Reciproot Iteration
// Adapted for fixed point numbers
//! avr32-uc3 16 bit version
#if (defined __GNUC__)
__attribute__((__naked__))
__attribute__((__noinline__))
#elif __ICCAVR32__
# pragma shadow_registers=full
# pragma optimize=none no_inline
#endif
dsp16_t dsp16_op_sqrt(dsp16_t num)
{
  __asm__ __volatile__ (
    "pushm  r0-r3, lr\n\t"

    // Limit
    "cp.w   r12, 0\n\t"
    ASM_INSTRUCT_COMPACKED("brlt   __dsp16_sqrt_end_err")"\n\t"

    // Find an approximation of 1/sqrt(x);
    "clz    r0, r12\n\t"
    "lsr    r0, r0, 1\n\t"
    "sub    r0, r0, "ASTRINGZ(-(DSP16_QB-16))"\n\t"
    "mov    r1, 1\n\t"

    // r0 ~ 1/sqrt(num)
    "lsl    r0, r1, r0\n\t"

    // r2 = 1.5
    "mov    r2, "ASTRINGZ(DSP16_SQRT_ONE_POINT_FIVE)"\n\t"

    // Perform a Newton Iteration
    MREPEAT(4, DSP16_SQRT_NEWTON_ITERATION, "")

#if (DSP16_QB%2 == 1)
    // To support Q1.(2N+1) fixed point numbers
    "mov    r2, "ASTRINGZ(46340 >> DSP16_QA)"\n\t"
    "mul    r12, r12, r2\n\t"
    "lsr    r12, r12, "ASTRINGZ(DSP16_QB)"\n\t"
#endif

    // Get sqrt(x) from 1/sqrt(x)
    "mul    r12, r0, r12\n\t"
    // Adjust the result for fixed point format
    "lsr    r12, r12, "ASTRINGZ(DSP16_QB >> 1)"\n"

    "__dsp16_sqrt_end_fct:\n\t"
    "popm   r0-r3, pc\n"

    // Return -1 if num < 0
    "__dsp16_sqrt_end_err:\n\t"
    "mov    r12, "CST_ZERO"\n\t"
    "popm   r0-r3, pc\n\t"
#if (defined __GNUC__)
    :
    :
    [CST_ZERO__] "i" (DSP16_Q(0.))
#endif
  );

  return 0;
}

#endif

