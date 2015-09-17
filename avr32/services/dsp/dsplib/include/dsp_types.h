/*****************************************************************************
 *
 * \file
 *
 * \brief Fixed Point DSP library data type definition
 *
 * This file defines the types used in the Fixed Point DSP library
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

/*! \addtogroup group_qformat
 * This module regroups the options used to configure the signed fixed point Q format.\n
 * Here is a description of the Q-Format:\n
 * Q-format: Qa.b\n
 * - a is the number of bits used to defined the integer value.
 * - b is to define the number of bits used after the radix point (the fractional value).
 *
 * Here is the formula linking a Q-format number (x) to a decimal number (d):\n
 * \image html qformat_main_formula.gif
 * - Xn: The value of the bit number n of the Q-formatted number x.
 * - N: The number of bits of the Q-formatted number x.
 *
 * The resolution of a Q-formatted number is calculated as follow:
 * \image html qformat_resolution.gif
 * The range of a Q-formatted number x defines the maximum and minimum numbers which frame x.
 * Here is a formula to calculate those values:
 * \image html qformat_range.gif
 * - N: The number of bits of the Q-formatted number x.
 *
 * Example:\n
 * <em>
 * For the Q1.15 type:\n
 * - Resolution:	0.00003
 * - Range:          -1. <= range <= 0.99997
 *
 * For the Q1.31 type:\n
 * - Resolution:  	0.00000000047
 * - Range:               -1. <= range <= 0.99999999953
 *
 *</em>
 * Here is a link where you can find more information <a target="_parent" href="http://en.wikipedia.org/wiki/Fixed-point_arithmetic">
 * http://en.wikipedia.org/wiki/Fixed-point_arithmetic</a> on fixed-point formats.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef __DSP_TYPES_H__
#define __DSP_TYPES_H__


#if (defined __ICCAVR32__)
#  include <intrinsics.h>
#endif
#include "compiler.h"
#include "preprocessor.h"


#ifdef __AVR32_ABI_COMPILER__

#if __ICCAVR32__

/*! \name Compiler Keywords
 *
 * Port of some keywords from GNU GCC for AVR32 to IAR Embedded Workbench for Atmel AVR32.
 */
//! @{
#ifndef __asm__
#  define __asm__             asm
#endif
#ifndef __inline__
#  define __inline__          inline
#endif
#ifndef __volatile__
#  define __volatile__
#endif
//! @}

#endif

#if (defined __GNUC__)
//! \brief Alignment attribute on 4 bytes for 32-bit or lower microcontroller optimization for GCC compiler
# define A_ALIGNED   __attribute__ ((aligned(4)))
//! \brief Packed data attribute for GCC compiler
# define A_PACKED    __attribute__ ((__packed__))
#elif __ICCAVR32__
//! \brief Alignment attribute on 4 bytes for 32-bit or lower microcontroller optimization for GCC compiler
# define A_ALIGNED   _Pragma("data_alignment=4")
//! \brief Packed data attribute
# define A_PACKED
#else
# error Unknown compiler
#endif

#endif // __AVR32_ABI_COMPILER__

/*! \name Q Format definition for the 16-bit type
 * \note The 16-bit functions are optimized to work with a Q1.15 format:\n
 * Resolution: 0.00003\n
 * Range: [-1.; 0.99997]
 */
//!  @{
#ifndef DSP16_FORMAT
/*! \brief number of integer bits
 * \ingroup group_qformat
 */
# define DSP16_QA  1
/*! \brief number of fractional bits
 * \ingroup group_qformat
 */
# define DSP16_QB  15
#else
/*! \brief number of integer bits
 * \ingroup group_qformat
 */
#  define DSP16_QA  (16 - DSP16_FORMAT)
/*! \brief number of fractional bits
 * \ingroup group_qformat
 */
#  define DSP16_QB  DSP16_FORMAT
#endif
//!  @}
#if (DSP16_QA + DSP16_QB) != 16
# error in the definition of the 16-bit type. The sum of the "Q format" coefficients should be 16.
#endif

/*! \name Q Format definition for the 32-bit type
 * \note The 32-bit functions are optimized to work with a Q1.31 format:\n
 * Resolution: 0.00000000047\n
 * Range: [-1.; 0.99999999953]
 */
//! @{
#ifndef DSP32_FORMAT
/*! \brief number of integer bits
 * \ingroup group_qformat
 */
# define DSP32_QA  1
/*! \brief number of fractional bits
 * \ingroup group_qformat
 */
# define DSP32_QB  31
#else
/*! \brief number of integer bits
 * \ingroup group_qformat
 */
# define DSP32_QA  (32 - DSP32_FORMAT)
/*! \brief number of fractional bits
 * \ingroup group_qformat
 */
# define DSP32_QB  DSP32_FORMAT
#endif
//! @}
#if (DSP32_QA + DSP32_QB) != 32
# error in the definition of the 32-bit type. The sum of the "Q format" coefficients should be 32.
#endif

#ifdef __AVR32_ABI_COMPILER__

/*! \ingroup group_types
 * \brief 16-bit signed fixed point type
 */
#define dsp16_t__  S16
/*! \ingroup group_types
 * \brief 16-bit signed fixed point type optimized for 32-bit or lower microcontrollers
 */
typedef dsp16_t__ dsp16_t;

/*! \ingroup group_types
 * \brief 32-bit signed fixed point type
 */
#define dsp32_t__  S32
/*! \ingroup group_types
 * \brief 32-bit signed fixed point type optimized for 32-bit or lower microcontrollers
 */
typedef dsp32_t__ dsp32_t;

/*! \ingroup group_types
 * \brief 16-bit complex signed fixed point type
 */
#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED dsp16_complex_t
{
    //! real part
  dsp16_t__ real;
    //! imaginary part
  dsp16_t__ imag;
}dsp16_complex_t;
#if __ICCAVR32__
# pragma pack()
#endif

/*! \ingroup group_types
 * \brief 32-bit complex signed fixed point type
 */
#if __ICCAVR32__
# pragma pack(1)
#endif
typedef struct A_PACKED dsp32_complex_t
{
    //! real part
  dsp32_t__ real;
    //! imaginary part
  dsp32_t__ imag;
}dsp32_complex_t;
#if __ICCAVR32__
# pragma pack()
#endif

#endif // __AVR32_ABI_COMPILER__

/*! \name Limit values in floating-point format of a signed fixed-point number typed Qa.b
 */
//! \{
/*! \ingroup group_macros
 * \brief Return the maximal value of a fixed-point Qa.b typed number. The result is returned as a floating point number.
 */
#define DSP_FP_MAX(a, b)    (((float) (1 << ((a)-1))) - DSP_FP_RES(a, b))
/*! \ingroup group_macros
 * \brief Return the minimal value of a fixed-point Qa.b typed number. The result is returned as a floating point number.
 */
#define DSP_FP_MIN(a, b)    (-((float) (1 << ((a)-1))))
//! \}

/*! \name Limit values in fixed-point format of a signed fixed-point number typed Qa.b
 */
//! \{
/*! \ingroup group_macros
 * \brief Return the maximal value of a fixed-point Qa.b typed number.
 */
#define DSP_Q_MAX(a, b)        ((S32) (((U32) -1) >> (32 - ((a)+(b)-1))))
/*! \ingroup group_macros
 * \brief Return the minimal value of a fixed-point Qa.b typed number.
 */
#define DSP_Q_MIN(a, b)        ((S32) ((-1) << ((a)+(b)-1)))
//! \}

/*! \name Resolution in floating-point format of a signed fixed-point number typed Qa.b
 */
//! \{
/*! \ingroup group_macros
 * \brief Return the resolution of a fixed-point Qa.b typed number.
 */
#define DSP_FP_RES(a, b)        (1./((unsigned) (1 << (b))))
//! \}

/*! \name Convertion macro from a float to a Q Formated value
 * \note These macros limit the number in its range
 */
//! \{
/*! \ingroup group_macros
 * \brief Convert a float into a signed fixed-point typed Qa.b
 */
#define DSP_Q(a, b, fnum)         (((fnum) >= DSP_FP_MAX(a, b) - DSP_FP_RES(a, b))?\
                                  DSP_Q_MAX(a, b):\
                                  (((fnum) <= DSP_FP_MIN(a, b) + DSP_FP_RES(a, b))?\
                                  DSP_Q_MIN(a, b):\
                                  (((fnum)*(((unsigned) (1 << (b))))))))

/*! \ingroup group_macros
 * \brief Convert a float into a 16-bit signed fixed-point type
 */
#define DSP16_Q(fnum)             ((dsp16_t__) DSP_Q(DSP16_QA, DSP16_QB, fnum))
/*! \ingroup group_macros
 * \brief Convert a float into a 32-bit signed fixed-point type
 */
#define DSP32_Q(fnum)             ((dsp32_t__) DSP_Q(DSP32_QA, DSP32_QB, fnum))
//! \}





#ifndef FALSE
/*! \ingroup group_constants
 * \brief Boolean FALSE constant
 */
# define FALSE                   0
#endif
#ifndef TRUE
/*! \ingroup group_constants
 * \brief Boolean TRUE constant
 */
# define TRUE                    1
#endif
/*! \ingroup group_constants
 * \brief PI definition also known as the Archimedes' constant
 */
#define CST_PI                  3.141592653589793238462643383279502884197
/*! \ingroup group_constants
 * \brief E definition also known as the natural logarithmic base
 */
#define CST_E                   2.718281828459045235360287471352662497757

/*! \ingroup group_constants
 * \brief sqrt(2) definition
 */
#define CST_SQUARE_ROOT_2       1.4142135623730950488016887242097
/*! \ingroup group_constants
 * \brief 1/sqrt(2) definition
 */
#define CST_INV_SQUARE_ROOT_2   0.70710678118654752440084436210485
/*! \ingroup group_constants
 * \brief ln(2) definition
 */
#define CST_LN_2                0.69314718055994530941723212145818
/*! \ingroup group_constants
 * \brief ln(10) definition
 */
#define CST_LN_10               2.3025850929940456840179914546844

/*! \ingroup group_constants
 * \brief PI/4 definition for the 16-bit fixed point type
 */
#define DSP16_PI_DIV_4          DSP16_Q(CST_PI/4.)
/*! \ingroup group_constants
 * \brief PI/4 definition for the 32-bit fixed point type
 */
#define DSP32_PI_DIV_4          DSP32_Q(CST_PI/4.)

#endif //__DSP_TYPES_H__
