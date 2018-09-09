/*****************************************************************************
 *
 * \file
 *
 * \brief Square root operator for the AVR32 UC3.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

