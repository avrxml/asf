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
