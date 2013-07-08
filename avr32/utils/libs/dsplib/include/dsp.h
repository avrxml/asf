// This header file is a compilation of multiple dsp header files

/*****************************************************************************
 *
 * \file
 *
 * \brief Doxygen documentation
 *
 * This file add some doxygen documentation to the project
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
 ******************************************************************************/

/************* Group definition for doxygen documentation ******************/

/*! \defgroup group_functions Basic
 *  This group regroups all the basic functions you can use in your project.
 */
/*! \defgroup group_filtering Filtering
 *  \ingroup group_functions
 *  All the filtering functions currently supported by the DSP library.
 */
/*! \defgroup group_operators Operators
 *  \ingroup group_functions
 *  All the fixed point operators functions currently implemented in the DSP library.
 */
/*! \defgroup group_signal_generation Signal generation
 *  \ingroup group_functions
 *  All the signal generation functions currently supported by the DSP library.
 */
/*! \defgroup group_transforms Transforms
 *  \ingroup group_functions
 *  All the transforms functions currently supported by the DSP library.
 */
/*! \defgroup group_vectors Vectors
 *  \ingroup group_functions
 *  All the vector management functions currently supported by the DSP library.
 */
/*! \defgroup group_windowing Windowing
 *  \ingroup group_functions
 *  All the windowing functions currently supported by the DSP library.
 */

/*! \defgroup group_config Configuration
 *  This group regroups all the defines you can change to optimize the DSP Library for your needs.
 */
/*! \defgroup group_cstdata Constant tables
 *  \ingroup group_config
 * These defines are used to configure data table storage methods
 */
/*! \defgroup group_optimization Optimization options
 *  \ingroup group_config
 * Those parameters are used for optimizing the algorithms according to your needs.\n
 * By default algorithms are defined to run the fastest as they can.
 */
/*! \defgroup group_qformat Q Format
 * \ingroup group_config
 * This module regroups the options used to configure the signed fixed point Q format.\n
 */
/*! \defgroup group_constants Constants
 *  This group regroups all the constants you can use in your project.
 */
/*! \defgroup group_macros Macros
 *  This group regroups all the defined macros you can use in your project.
 */
/*! \defgroup group_types Types
 *  This group regroups all the types you can use in your project.
 */

/***********************************************************************/
/*****************************************************************************
 *
 * \file
 *
 * \brief Fixed Point DSP library data type definition
 *
 * This file defines the types used in the Fixed Point DSP library
 *
 ******************************************************************************/

/**
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
 */
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
#define dsp16_t__  int16_t
/*! \ingroup group_types
 * \brief 16-bit signed fixed point type optimized for 32-bit or lower microcontrollers
 */
typedef dsp16_t__ dsp16_t;

/*! \ingroup group_types
 * \brief 32-bit signed fixed point type
 */
#define dsp32_t__  int32_t
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
#define DSP_Q_MAX(a, b)        ((int32_t) (((uint32_t) -1) >> (32 - ((a)+(b)-1))))
/*! \ingroup group_macros
 * \brief Return the minimal value of a fixed-point Qa.b typed number.
 */
#define DSP_Q_MIN(a, b)        ((int32_t) ((-1) << ((a)+(b)-1)))
//! \}

/*! \name Resolution in floating-point format of a signed fixed-point number typed Qa.b
 */
//! \{
/*! \ingroup group_macros
 * \brief Return the resolution of a fixed-point Qa.b typed number.
 */
#define DSP_FP_RES(a, b)        (1./((unsigned) (1 << (b))))
//! \}

/*! \name Conversion macro from a float to a Q Formated value
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





#ifndef false
/*! \ingroup group_constants
 * \brief Boolean false constant
 */
# define false                   0
#endif
#ifndef true
/*! \ingroup group_constants
 * \brief Boolean true constant
 */
# define true                    1
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
/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented vector management functions
 *
 * This file show a list of the vector management functions currently implemented in the DSP library.
 *
 ******************************************************************************/

/**
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
 */
/* - Real
 *  - \ref Addition
 *  - \ref Addition_with_saturation
 *  - \ref Addition_with_a_real
 *  - \ref Subtraction
 *  - \ref Subtraction_with_a_real
 *  - \ref Multiplication_with_a_real
 *  - \ref Division_with_a_real
 *  - \ref Dot_multiplication
 *  - \ref Dot_division
 *  - \ref Multiplication_with_an_integer
 *  - \ref Division_with_an_integer
 *  - \ref Power
 *  - \ref Minimum
 *  - \ref Maximum
 *  - \ref Negate
 *  - \ref Zero_padding
 *  - \ref Copy
 *  - \ref Partial_Convolution
 *  - \ref Convolution
 * - Complex
 *  - \ref Complex_addition
 *  - \ref Complex_subtraction
 *  - \ref Complex_absolute
 *  - \ref Complex_conjugate
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

#ifndef __DSP_VECTORS_H__
#define __DSP_VECTORS_H__

#ifdef __AVR32_ABI_COMPILER__

#include "string.h"


/**************************** REAL FUNCTIONS *************************/

/*! \addtogroup group_vectors
 * \section Addition Addition
 * This function adds two vectors and store the result into another one.
 *
 * Relative functions:
 * - \ref dsp16_vect_add
 * - \ref dsp32_vect_add
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector addition function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the addition.
 * \param vect2 A pointer on the 16-bit real vector that will be added with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit real vector that will be added with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_add(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size);
/*! \brief 32-bit fixed point version of the vector addition function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the addition.
 * \param vect2 A pointer on the 32-bit real vector that will be added with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit real vector that will be added with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_add(dsp32_t *vect1, dsp32_t *vect2, dsp32_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Addition_with_saturation Addition with saturation
 * This function adds two vectors and store the result into another one.
 * It takes care of the saturation.
 *
 * Relative functions:
 * - \ref dsp16_vect_add_and_sat
 * - \ref dsp32_vect_add_and_sat
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector addition function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the addition.
 * \param vect2 A pointer on the 16-bit real vector that will be added with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit real vector that will be added with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_add_and_sat(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size);
/*! \brief 32-bit fixed point version of the vector addition function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the addition.
 * \param vect2 A pointer on the 32-bit real vector that will be added with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit real vector that will be added with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_add_and_sat(dsp32_t *vect1, dsp32_t *vect2, dsp32_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Addition_with_a_real Addition with a real
 * This function adds each items of a vector with a real number and store the result into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_realadd
 * - \ref dsp32_vect_realadd
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector addition with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be added with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be added with the vector (vect2).
 */
void dsp16_vect_realadd(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real);
/*! \brief 32-bit fixed point version of the vector addition with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be added with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be added with the vector (vect2).
 */
void dsp32_vect_realadd(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t real);

/*! \addtogroup group_vectors
 * \section Subtraction Subtraction
 * This function subtracts two vectors and store the result into another one.
 *
 * Relative functions:
 * - \ref dsp16_vect_sub
 * - \ref dsp32_vect_sub
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector subtraction function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the subtraction.
 * \param vect2 A pointer on the 16-bit real vector that will be subtracted with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit real vector that will be subtracted with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_sub(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size);
/*! \brief 32-bit fixed point version of the vector subtraction function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the subtraction.
 * \param vect2 A pointer on the 32-bit real vector that will be subtracted with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit real vector that will be subtracted with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_sub(dsp32_t *vect1, dsp32_t *vect2, dsp32_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Subtraction_with_a_real Subtraction with a real
 * This function subtracts each items of a vector with a real number and store the result into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_realsub
 * - \ref dsp32_vect_realsub
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector subtraction with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be subtracted with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be subtracted with the vector (vect2).
 */
void dsp16_vect_realsub(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real);
/*! \brief 32-bit fixed point version of the vector subtraction with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be subtracted with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be subtracted with the vector (vect2).
 */
void dsp32_vect_realsub(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t real);

/*! \addtogroup group_vectors
 * \section Multiplication_with_a_real Multiplication with a real
 * This function multiplies one vector with a real number and store the result into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_realmul
 * - \ref dsp32_vect_realmul
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector multiplication with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be multiplied with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be multiplied with the vector (vect2).
 */
void dsp16_vect_realmul(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real);
/*! \brief 32-bit fixed point version of the vector multiplication with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be multiplied with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be multiplied with the vector (vect2).
 */
void dsp32_vect_realmul(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t real);

/*! \addtogroup group_vectors
 * \section Division_with_a_real Division with a real
 * This function divides one vector with a real number and store the result into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_realdiv
 * - \ref dsp32_vect_realdiv
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector division with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be divided with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be divided with the vector (vect2).
 */
void dsp16_vect_realdiv(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real);
/*! \brief 32-bit fixed point version of the vector division with a real number.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be divided with the real number.
 * \param size The size of the buffers.
 * \param real The real number to be divided with the vector (vect2).
 */
void dsp32_vect_realdiv(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t real);

/*! \addtogroup group_vectors
 * \section Multiplication_with_an_integer Multiplication with an integer
 * This function multiplies one vector with an integer and store the result into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_intmul
 * - \ref dsp32_vect_intmul
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector multiplication with an integer.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be multiplied with the integer.
 * \param size The size of the buffers.
 * \param integer The integer to be multiplied with the vector (vect2).
 */
void dsp16_vect_intmul(dsp16_t *vect1, dsp16_t *vect2, int size, int integer);
/*! \brief 32-bit fixed point version of the vector multiplication with an integer.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be multiplied with the integer.
 * \param size The size of the buffers.
 * \param integer The integer to be multiplied with the vector (vect2).
 */
void dsp32_vect_intmul(dsp32_t *vect1, dsp32_t *vect2, int size, int integer);

/*! \addtogroup group_vectors
 * \section Division_with_an_integer Division with an integer
 * This function divides one vector with an integer and store the result into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_intdiv
 * - \ref dsp32_vect_intdiv
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector division with an integer.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be divided with the integer.
 * \param size The size of the buffers.
 * \param integer The integer to be divided with the vector (vect2).
 */
void dsp16_vect_intdiv(dsp16_t *vect1, dsp16_t *vect2, int size, int integer);
/*! \brief 32-bit fixed point version of the vector division with an integer.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be divided with the integer.
 * \param size The size of the buffers.
 * \param integer The integer to be divided with the vector (vect2).
 */
void dsp32_vect_intdiv(dsp32_t *vect1, dsp32_t *vect2, int size, int integer);

/*! \addtogroup group_vectors
 * \section Dot_multiplication Dot multiplication
 * This function multiplies two vectors point per point and store the result into another one.
 *
 * Relative functions:
 * - \ref dsp16_vect_dotmul
 * - \ref dsp32_vect_dotmul
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector dot multiplication function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the dot multiplication.
 * \param vect2 A pointer on the 16-bit real vector that will be multiplied with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit real vector that will be multiplied with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_dotmul(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size);
/*! \brief 32-bit fixed point version of the vector dot multiplication function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the dot multiplication.
 * \param vect2 A pointer on the 32-bit real vector that will be multiplied with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit real vector that will be multiplied with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_dotmul(dsp32_t *vect1, dsp32_t *vect2, dsp32_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Dot_division Dot division
 * This function divides two vectors point per point and store the result into another one.
 *
 * Relative functions:
 * - \ref dsp16_vect_dotdiv
 * - \ref dsp32_vect_dotdiv
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector dot division function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the dot division.
 * \param vect2 A pointer on the 16-bit real vector that will be divided with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit real vector that will be divided with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_dotdiv(dsp16_t *vect1, dsp16_t *vect2, dsp16_t *vect3, int size);
/*! \brief 32-bit fixed point version of the vector dot division function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the dot division.
 * \param vect2 A pointer on the 32-bit real vector that will be divided with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit real vector that will be divided with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_dotdiv(dsp32_t *vect1, dsp32_t *vect2, dsp32_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Power Power
 * These functions compute vect2^real.
 * \note The output will be limit in the range of the fixed point format used.
 *
 * Relative functions:
 * - \ref dsp16_vect_pow
 * - \ref dsp32_vect_pow
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the power function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 16-bit real vector that will be raised to the power 'real'.
 * \param size The size of the buffers.
 * \param real The real number used to raised to the power 'vect2'.
 * \note If the vector contains negative numbers, the result for these items will be DSP_Q_MIN.
 */
void dsp16_vect_pow(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t real);
/*! \brief 32-bit fixed point version of the power function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will received the result of the operation.
 * \param vect2 A pointer on the 32-bit real vector that will be raised to the power 'real'.
 * \param size The size of the buffers.
 * \param real The real number used to raised to the power 'vect2'.
 * \note If the vector contains negative numbers, the result for these items will be DSP_Q_MIN.
 */
void dsp32_vect_pow(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t real);

/*! \addtogroup group_vectors
 * \section Minimum Minimum
 * This function retrieves the minimum of a vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_min
 * - \ref dsp32_vect_min
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector minimum function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that is used to find its minimum.
 * \param size The size of the input buffer.
 * \return The minimum of the vector (vect1).
 */
dsp16_t dsp16_vect_min(dsp16_t *vect1, int size);
/*! \brief 32-bit fixed point version of the vector minimum function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that is used to find its minimum.
 * \param size The size of the input buffer.
 * \return The minimum of the vector (vect1).
 */
dsp32_t dsp32_vect_min(dsp32_t *vect1, int size);

/*! \addtogroup group_vectors
 * \section Maximum Maximum
 * This function retrieves the maximum of a vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_max
 * - \ref dsp32_vect_max
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector maximum function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that is used to find its maximum.
 * \param size The size of the input buffer.
 * \return The maximum of the vector (vect1).
 */
dsp16_t dsp16_vect_max(dsp16_t *vect1, int size);
/*! \brief 32-bit fixed point version of the vector maximum function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that is used to find its maximum.
 * \param size The size of the input buffer.
 * \return The maximum of the vector (vect1).
 */
dsp32_t dsp32_vect_max(dsp32_t *vect1, int size);

/*! \addtogroup group_vectors
 * \section Negate Negate
 * This function negate a vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_neg
 * - \ref dsp32_vect_neg
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the vector negate function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will receive the result.
 * \param vect2 A pointer on the 16-bit real vector that will be negate.
 * \param size The size of the input buffer.
 */
void dsp16_vect_neg(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the vector negate function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will receive the result.
 * \param vect2 A pointer on the 32-bit real vector that will be negate.
 * \param size The size of the input buffer.
 */
void dsp32_vect_neg(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_vectors
 * \section Zero_padding Zero padding
 * This function zero pads the tail of the vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_zeropad
 * - \ref dsp32_vect_zeropad
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the zero padding function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16 bits real vector that has to be padded with zeros.
 * \param size The size of this vector.
 * \param num_zero The number of zeros to pad at the end of the vector.
 */
inline static void dsp16_vect_zeropad(dsp16_t *vect1, int size, int num_zero) { memset((uint8_t *) vect1 + (size - num_zero)*sizeof(dsp16_t), 0, num_zero*sizeof(dsp16_t)); };
/*! \brief 32-bit fixed point version of the zero padding function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32 bits real vector that has to be padded with zeros.
 * \param size The size of this vector.
 * \param num_zero The number of zeros to pad at the end of the vector.
 */
inline static void dsp32_vect_zeropad(dsp32_t *vect1, int size, int num_zero) { memset((uint8_t *) vect1 + (size - num_zero)*sizeof(dsp32_t), 0, num_zero*sizeof(dsp32_t)); };

/*! \addtogroup group_vectors
 * \section Copy Copy
 * This function copy a vector into another vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_copy
 * - \ref dsp32_vect_copy
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the copy function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16 bits real vector that will receive data.
 * \param vect2 A pointer on the 16 bits real vector to be copied.
 * \param size The size of those vectors.
 */
inline static void dsp16_vect_copy(dsp16_t *vect1, dsp16_t *vect2, int size) { memcpy(vect1, vect2, size*sizeof(dsp16_t)); };
/*! \brief 32-bit fixed point version of the copy function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32 bits real vector that will receive data.
 * \param vect2 A pointer on the 32 bits real vector to be copied.
 * \param size The size of those vectors.
 */
inline static void dsp32_vect_copy(dsp32_t *vect1, dsp32_t *vect2, int size) { memcpy(vect1, vect2, size*sizeof(dsp32_t)); };

/*! \addtogroup group_vectors
 * \section Partial_Convolution Partial Convolution
 * This function performs a linear partial convolution between two discrete sequences.
 * It returns a signal of a length equals to (vect2_size - vect3_size + 1) elements.
 * \note The two discrete sequences have to be scaled to avoid overflowing values.\n
 * All the vectors have to be 32-bits aligned.
 *
 * Relative functions:
 * - \ref dsp16_vect_convpart
 * - \ref dsp32_vect_convpart
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the Partial Convolution.
 * \ingroup group_vectors
 * \param vect1 A pointer on a 16-bits fixed-point vector corresponding to the output buffer.
 * \param vect2 A pointer on a 16-bits fixed-point vector of vect2_size elements corresponding to the first input buffer.
 * \param vect2_size The length of the first input buffer (must be greater or equals to 4).
 * \param vect3 A pointer on a 16-bits fixed-point vector of vect3_size elements corresponding to the second input buffer.
 * \param vect3_size The length of the second input buffer (must be greater or equals to 8)
 * \warning Due to its implementation, for the avr32-uc3 optimized version of the FIR,
 * the output buffer (vect1) have to have a length of 4*n elements to avoid overflows.
 */
void dsp16_vect_convpart(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *vect3, int vect3_size);
/*! \brief 32-bit fixed point version of the Partial Convolution.
 * \ingroup group_vectors
 * \param vect1 A pointer on a 32-bits fixed-point vector corresponding to the output buffer.
 * \param vect2 A pointer on a 32-bits fixed-point vector of vect2_size elements corresponding to the first input buffer.
 * \param vect2_size The length of the first input buffer (must be greater or equals to 4).
 * \param vect3 A pointer on a 32-bits fixed-point vector of vect3_size elements corresponding to the second input buffer.
 * \param vect3_size The length of the second input buffer (must be greater or equals to 8)
 */
void dsp32_vect_convpart(dsp32_t *vect1, dsp32_t *vect2, int vect2_size, dsp32_t *vect3, int vect3_size);

/*! \addtogroup group_vectors
 * \section Convolution Convolution
 * This function performs a linear convolution between two discrete sequences.
 * It returns a signal of a length equals to (vect2_size + vect3_size - 1) elements.
 * \note The two discrete sequences have to be scaled to avoid overflowing values.\n
 * All the vectors have to be 32-bits aligned.
 * \warning The output buffer of the function has to have at least a length of N + 2*M -
 * 2 elements because of internal computations, where N is the length of the
 * largest input buffer and M, the length of the smallest input buffer.
 *
 * Relative functions:
 * - \ref dsp16_vect_conv
 * - \ref dsp32_vect_conv
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the Convolution.
 * \ingroup group_vectors
 * \param vect1 A pointer on a 16-bits fixed-point vector corresponding to the output buffer.
 * \param vect2 A pointer on a 16-bits fixed-point vector of vect2_size elements corresponding to the first input buffer.
 * \param vect2_size The length of the first input buffer (must be greater or equals to 8).
 * \param vect3 A pointer on a 16-bits fixed-point vector of vect3_size elements corresponding to the second input buffer.
 * \param vect3_size The length of the second input buffer (must be greater or equals to 8)
 * \warning Due to its implementation, for the avr32-uc3 optimized version of the FIR,
 * the output buffer (vect1) have to have a length of 4*n elements to avoid overflows.
 * \note You need the "Partial Convolution" module\n
 * the "Copy" module\n
 * the "Zero Padding" module
 */
void dsp16_vect_conv(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *vect3, int vect3_size);
/*! \brief 32-bit fixed point version of the Convolution.
 * \ingroup group_vectors
 * \param vect1 A pointer on a 32-bits fixed-point vector corresponding to the output buffer.
 * \param vect2 A pointer on a 32-bits fixed-point vector of vect2_size elements corresponding to the first input buffer.
 * \param vect2_size The length of the first input buffer (must be greater or equals to 8).
 * \param vect3 A pointer on a 32-bits fixed-point vector of vect3_size elements corresponding to the second input buffer.
 * \param vect3_size The length of the second input buffer (must be greater or equals to 8)
 * \note You need the "Partial Convolution" module\n
 * the "Copy" module\n
 * the "Zero Padding" module
 */
void dsp32_vect_conv(dsp32_t *vect1, dsp32_t *vect2, int vect2_size, dsp32_t *vect3, int vect3_size);




/**************************** COMPLEX FUNCTIONS *************************/







/*! \addtogroup group_vectors
 * \section Complex_addition Complex addition
 * This function add two complex vectors and store the result into another one.
 *
 * Relative functions:
 * - \ref dsp16_vect_complex_add
 * - \ref dsp32_vect_complex_add
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the complex vector addition function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit complex vector that will received the result of the addition.
 * \param vect2 A pointer on the 16-bit complex vector that will be added with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit complex vector that will be added with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_complex_add(dsp16_complex_t *vect1, dsp16_complex_t *vect2, dsp16_complex_t *vect3, int size);
/*! \brief 32-bit fixed point version of the complex vector addition function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit complex vector that will received the result of the addition.
 * \param vect2 A pointer on the 32-bit complex vector that will be added with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit complex vector that will be added with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_complex_add(dsp32_complex_t *vect1, dsp32_complex_t *vect2, dsp32_complex_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Complex_subtraction Complex subtraction
 * This function sub two complex vectors and store the result into another one.
 *
 * Relative functions:
 * - \ref dsp16_vect_complex_sub
 * - \ref dsp32_vect_complex_sub
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the complex vector subtraction function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit complex vector that will received the result of the subtraction.
 * \param vect2 A pointer on the 16-bit complex vector that will be subtracted with the other input vector (vect3).
 * \param vect3 A pointer on the 16-bit complex vector that will be subtracted with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp16_vect_complex_sub(dsp16_complex_t *vect1, dsp16_complex_t *vect2, dsp16_complex_t *vect3, int size);
/*! \brief 32-bit fixed point version of the complex vector subtraction function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit complex vector that will received the result of the subtraction.
 * \param vect2 A pointer on the 32-bit complex vector that will be subtracted with the other input vector (vect3).
 * \param vect3 A pointer on the 32-bit complex vector that will be subtracted with the other input vector (vect2).
 * \param size The size of the input buffers.
 */
void dsp32_vect_complex_sub(dsp32_complex_t *vect1, dsp32_complex_t *vect2, dsp32_complex_t *vect3, int size);

/*! \addtogroup group_vectors
 * \section Complex_absolute Complex absolute
 * This function returns the absolute value of a complex vector.
 *
 * Relative functions:
 * - \ref dsp16_vect_complex_abs
 * - \ref dsp32_vect_complex_abs
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the complex vector absolute function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit real vector that will receive the result.
 * \param vect2 A pointer on the 16-bit complex vector that will be computed.
 * \param size The size of the input buffer.
 */
void dsp16_vect_complex_abs(dsp16_t *vect1, dsp16_complex_t *vect2, int size);
/*! \brief 32-bit fixed point version of the complex vector absolute function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32-bit real vector that will receive the result.
 * \param vect2 A pointer on the 32-bit complex vector that will be computed.
 * \param size The size of the input buffer.
 */
void dsp32_vect_complex_abs(dsp32_t *vect1, dsp32_complex_t *vect2, int size);

/*! \addtogroup group_vectors
 * \section Complex_conjugate Complex conjugate
 * This function returns the conjugate complex vector of the input.
 *
 * Relative functions:
 * - \ref dsp16_vect_complex_conj
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the complex vector conjugate function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 16-bit complex vector that will receive the result.
 * \param vect2 A pointer on the 16-bit complex vector that will be computed.
 * \param size The size of the input buffer.
 */
void dsp16_vect_complex_conj(dsp16_complex_t *vect1, dsp16_complex_t *vect2, int size);

#endif //__AVR32_ABI_COMPILER__
#endif //__DSP_VECTORS_H__

/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented filters
 *
 * This file show a list of the filtering functions currently implemented in the DSP library.
 *
 ******************************************************************************/

/**
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
 */
/* - \ref FIR
 * - \ref IIR
 * - \ref Partial_IIR
 * - \ref LMS
 * - \ref NLMS
 * - \ref Interpolation
 * - \ref lp_FIR_design
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

#ifndef __DSP_FILTERING_H__
#define __DSP_FILTERING_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \addtogroup group_filtering
 * \section filt_coef_gen Coefficient Generation
 * The following functions are using coefficients to operate. These coefficients
 * define the properties of the filter used and must be generated first.\n
 * There are different ways to do it:
 * - By using a dedicated software, many of them can be easily found on the web.
 * - By using a digital signal software which provides the capability to do it
 * such as Matlab (http://www.mathworks.com/) or Scilab
 * (http://www.scilab.org/) (Scilab is a free tool similar to Matlab).
 * - By using a dedicated DSP Library function such as \ref lp_FIR_design.
 *
 * The following shows how to generate 4th order low-pass filter coefficients
 * using Scilab.
 * \code wfir('lp', 4, [fc/fs, 0], 're', [0 0]) \endcode
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \addtogroup group_filtering
 * \section FIR Finite Impulse Response Filter
 * This function computes a real FIR filter using the impulse response of the desire
 * filter onto a fixed-length signal. It returns a signal of a length equals to (size - h_size + 1) elements.\n
 * Here is the formula of the FIR filter:
 * \image html filtering_fir_equation.gif
 * \note The impulse response of the filter has to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.
 *
 * Relative functions:
 * - \ref dsp16_filt_fir
 * - \ref dsp32_filt_fir
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16 bits fixed point version of the FIR .
 * \ingroup group_filtering
 * \param vect1 A pointer on a 16-bit fixed-point vector of (size - h_size + 1) elements corresponding to the output buffer.
 * \param vect2 A pointer on a 16-bit fixed-point vector of size elements corresponding to the input buffer.
 * \param size The length of the input buffer (must be greater or equals to 4).
 * \param h A pointer on a 16-bit fixed-point vector of h_size elements corresponding to the buffer containing the impulse response coefficients.
 * \param h_size The length of the impulse response of the filter (must be greater than 7)
 * \warning Due to its implementation, for the avr32-uc3 optimized version of the FIR,
 *          the output buffer (vect1) have to have a length of 4*n elements to avoid overflows.
 * \note You need the "Partial Convolution" module
 */
inline static void dsp16_filt_fir(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t *h, int h_size) { dsp16_vect_convpart(vect1, vect2, size, h, h_size); };
/*! \brief 32 bits fixed point version of the FIR .
 * \ingroup group_filtering
 * \param vect1 A pointer on a 32-bit fixed-point vector of (size - h_size + 1) elements corresponding to the output buffer.
 * \param vect2 A pointer on a 32-bit fixed-point vector of size elements corresponding to the input buffer.
 * \param size The length of the input buffer (must be greater or equals to 4).
 * \param h A pointer on a 32-bit fixed-point vector of h_size elements corresponding to the buffer containing the impulse response coefficients.
 * \param h_size The length of the impulse response of the filter (must be greater than 7)
 * \note You need the "Partial Convolution" module
 */
inline static void dsp32_filt_fir(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t *h, int h_size) { dsp32_vect_convpart(vect1, vect2, size, h, h_size); };


/*! \addtogroup group_filtering
 * \section IIR Infinite Impulse Response Filter
 * This function computes a real IIR filter using the coefficients of the desire
 * filter onto a fixed-length signal. It returns a signal of a length equals to size elements.\n
 * Here is the formula of the IIR filter:
 * \image html filtering_iir_equation.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.
 *
 * Relative functions:
 * - \ref dsp16_filt_iir
 * - \ref dsp32_filt_iir
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the IIR .
 * \ingroup group_filtering
 * \param y A pointer on a 16-bit fixed-point vector of size elements corresponding to the output buffer.
 *          The den_size previous elements correspond to values of old output samples and have to be set to 0 at the initialization (y-1, y-2, ...).
 * \param x A pointer on a 16-bit fixed-point vector of size elements corresponding to the input buffer.
 *          The num_size previous elements correspond to old samples intput samples have to be set to 0 at the initialization (x-1, x-2, ...).
 * \param size The length of the input buffer.
 * \param num A pointer on a 16-bit fixed-point vector of num_size elements corresponding to the buffer containing the numerator's coefficients of the filter.
 * \param num_size The length of the numerator's coefficients of the filter (must be a multiple of 2).
 * \param den A pointer on a 16-bit fixed-point vector of den_size elements corresponding to the buffer containing the denominator's coefficients of the filter.
 * \param den_size The length of the denominator's coefficients of the filter (must be a multiple of 2).
 * \param num_prediv The predivisor used to scale down the numerator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the numerator's coefficients by 2^num_prediv else leave this field to 0.
 * \param den_prediv The predivisor used to scale down the denominator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the denominator's coefficients by 2^den_prediv else leave this field to 0.
 * \warning Due to its implementation, for the avr32-uc3 optimized version of the IIR,
 *          the length of the output buffer (vect1) have to be multiple of 6 in order to avoid overflows.
 */
void dsp16_filt_iir(dsp16_t *y, dsp16_t *x, int size, dsp16_t *num, int num_size, dsp16_t *den, int den_size, int num_prediv, int den_prediv);
/*! \brief 32-bit fixed point version of the IIR .
 * \ingroup group_filtering
 * \param y A pointer on a 32-bit fixed-point vector of size elements corresponding to the output buffer.
 *          The den_size previous elements correspond to values of old output samples and have to be set to 0 at the initialization (y-1, y-2, ...).
 * \param x A pointer on a 32-bit fixed-point vector of size elements corresponding to the input buffer.
 *          The num_size previous elements correspond to old samples input samples have to be set to 0 at the initialization (x-1, x-2, ...).
 * \param size The length of the input buffer.
 * \param num A pointer on a 32-bit fixed-point vector of num_size elements corresponding to the buffer containing the numerator's coefficients of the filter.
 * \param num_size The length of the numerator's coefficients of the filter (must be a multiple of 2).
 * \param den A pointer on a 32-bit fixed-point vector of den_size elements corresponding to the buffer containing the denominator's coefficients of the filter.
 * \param den_size The length of the denominator's coefficients of the filter (must be a multiple of 2).
 * \param num_prediv The predivisor used to scale down the numerator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the numerator's coefficients by 2^num_prediv else leave this field to 0.
 * \param den_prediv The predivisor used to scale down the denominator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the denominator's coefficients by 2^den_prediv else leave this field to 0.
 * \warning Due to its implementation, for the avr32-uc3 optimized version of the IIR,
 *          the length of the output buffer (vect1) have to be multiple of 6 in order to avoid overflows.
 */
void dsp32_filt_iir(dsp32_t *y, dsp32_t *x, int size, dsp32_t *num, int num_size, dsp32_t *den, int den_size, int num_prediv, int den_prediv);


/*! \addtogroup group_filtering
 * \section Partial_IIR Partial Infinite Impulse Response Filter
 * This function computes a real IIR filter using the coefficients of the desire
 * filter onto a fixed-length signal. It returns a signal of a length equals to (size - num_size + 1) elements.\n
 * Here is the formula of the IIR filter:
 * \image html filtering_iir_equation.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.
 *
 * Relative functions:
 * - \ref dsp16_filt_iirpart
 * - \ref dsp32_filt_iirpart
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16 bits fixed point version of the IIR .
 * \ingroup group_filtering
 * \param vect1 A pointer on a 16-bit fixed-point vector of (size - num_size + 1) elements corresponding to the output buffer.
 * \param vect2 A pointer on a 16-bit fixed-point vector of size elements corresponding to the input buffer.
 * \param size The length of the input buffer.
 * \param num A pointer on a 16-bit fixed-point vector of num_size elements corresponding to the buffer containing the numerator's coefficients of the filter.
 * \param num_size The length of the numerator's coefficients of the filter.
 * \param den A pointer on a 16-bit fixed-point vector of den_size elements corresponding to the buffer containing the denominator's coefficients of the filter.
 * \param den_size The length of the denominator's coefficients of the filter.
 * \param num_prediv The predivisor used to scale down the numerator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the numerator's coefficients by 2^num_prediv else leave this field to 0.
 * \param den_prediv The predivisor used to scale down the denominator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the denominator's coefficients by 2^den_prediv else leave this field to 0.
 * \warning Due to its implementation, for the avr32-uc3 optimized version of the IIR,
 *          the length of the output buffer (vect1) have to be multiple of 6 in order to avoid overflows.
 */
void dsp16_filt_iirpart(dsp16_t *vect1, dsp16_t *vect2, int size, dsp16_t *num, int num_size, dsp16_t *den, int den_size, int num_prediv, int den_prediv);
/*! \brief 32 bits fixed point version of the IIR .
 * \ingroup group_filtering
 * \param vect1 A pointer on a 32-bit fixed-point vector of (size - num_size + 1) elements corresponding to the output buffer.
 * \param vect2 A pointer on a 32-bit fixed-point vector of size elements corresponding to the input buffer.
 * \param size The length of the input buffer.
 * \param num A pointer on a 32-bit fixed-point vector of num_size elements corresponding to the buffer containing the numerator's coefficients of the filter.
 * \param num_size The length of the numerator's coefficients of the filter.
 * \param den A pointer on a 32-bit fixed-point vector of den_size elements corresponding to the buffer containing the denominator's coefficients of the filter.
 * \param den_size The length of the denominator's coefficients of the filter.
 * \param num_prediv The predivisor used to scale down the numerator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the numerator's coefficients by 2^num_prediv else leave this field to 0.
 * \param den_prediv The predivisor used to scale down the denominator's coefficients of the filter in order to avoid overflow values.
 *                   So when you use this feature, you have to prescale manually the denominator's coefficients by 2^den_prediv else leave this field to 0.
 */
void dsp32_filt_iirpart(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t *num, int num_size, dsp32_t *den, int den_size, int num_prediv, int den_prediv);



/*! \addtogroup group_filtering
 * \section LMS Least Mean Square Filter
 * This function computes an adaptive LMS filter. It returns a (size)-length signal.\n
 * Here is the formula of the LMS filter:
 * \image html filtering_lms_equations.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.\n
 *       You can change the value of the mu coefficient of the LMS filter by defying DSP_LMS_MU,
 *       at the compilation, with the desired value. This value defines mu as follow:
 * \image html filtering_lms_coefficient.gif
 *
 * Relative functions:
 * - \ref dsp16_filt_lms
 * - \ref dsp32_filt_lms
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the LMS filter.
 * \ingroup group_filtering
 * \param x A pointer on a 16-bit fixed-point vector of (size) elements that acts as a circular buffer, filled with the input samples. Its elements
 *          have to be initialized to zero and then you just need to reinject this vector each time you call this functions without filling any of its values.
 * \param w A pointer on a 16-bit fixed-point vector of size elements corresponding to the coefficients of the filter. Just initialize its elements
 *          to zero and after several  iterations, this buffer will be filled with the actual coefficients of the filter.
 * \param size The length of the circular buffer (x) and of the coefficient's buffer (w). It must be a multiple of 4.
 * \param new_x A 16-bit fixed-point value which contains a new input sample signal.
 * \param d A 16-bit fixed-point value which contains the current sample of the reference's signal.
 * \param y A pointer on a 16-bit fixed-point value corresponding to the current sample of the output signal.
 * \param e A pointer on a 16-bit fixed-point value corresponding to the current sample of the output error signal.
 */
void dsp16_filt_lms(dsp16_t *x, dsp16_t *w, int size, dsp16_t new_x, dsp16_t d, dsp16_t *y, dsp16_t *e);

/*! \brief 32-bit fixed point version of the LMS filter.
 * \ingroup group_filtering
 * \param x A pointer on a 32-bit fixed-point vector of (size) elements that acts as a circular buffer, filled with the input samples. Its elements
 *          have to be initialized to zero and then you just need to reinject this vector each time you call this functions without filling any of its values.
 * \param w A pointer on a 32-bit fixed-point vector of size elements corresponding to the coefficients of the filter. Just initialize its elements
 *          to zero and after several  iterations, this buffer will be filled with the actual coefficients of the filter.
 * \param size The length of the circular buffer (x) and of the coefficient's buffer (w).
 * \param new_x A 32-bit fixed-point value which contains a new input sample signal.
 * \param d A 32-bit fixed-point value which contains the current sample of the reference's signal.
 * \param y A pointer on a 32-bit fixed-point value corresponding to the current sample of the output signal.
 * \param e A pointer on a 32-bit fixed-point value corresponding to the current sample of the output error signal.
 */
void dsp32_filt_lms(dsp32_t *x, dsp32_t *w, int size, dsp32_t new_x, dsp32_t d, dsp32_t *y, dsp32_t *e);
void dsp32_filt_lms_fir(dsp32_t *x, dsp32_t *w, int size, dsp32_t *y, int i);

/*! \addtogroup group_filtering
 * \section NLMS Normalized Least Mean Square Filter
 * This function computes an adaptive NLMS filter. It returns a (size)-length signal.\n
 * Here is the formula of the NLMS filter:
 * \image html filtering_nlms_equations.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.
 *       You can change the value of the mu coefficient of the NLMS filter by defying DSP_NLMS_MU,
 *       at the compilation, with the desired value. This value defines mu as follow:
 * \image html filtering_nlms_coefficient.gif
 *
 * Relative functions:
 * - \ref dsp16_filt_nlms
 * - \ref dsp32_filt_nlms
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the NLMS filter.
 * \ingroup group_filtering
 * \param x A pointer on a 16-bit fixed-point vector of (size) elements that acts as a circular buffer, filled with the input samples. Its elements
 *          have to be initialized to zero and then you just need to reinject this vector each time you call this functions without filling any of its values.
 * \param w A pointer on a 16-bit fixed-point vector of size elements corresponding to the coefficients of the filter. Just initialize its elements
 *          to zero and after several  iterations, this buffer will be filled with the actual coefficients of the filter.
 * \param size The length of the circular buffer (x) and of the coefficient's buffer (w). It must be a multiple of 4.
 * \param new_x A 16-bit fixed-point value which contains a new input sample signal.
 * \param d A 16-bit fixed-point value which contains the current sample of the reference's signal.
 * \param y A pointer on a 16-bit fixed-point value corresponding to the current sample of the output signal.
 * \param e A pointer on a 16-bit fixed-point value corresponding to the current sample of the output error signal.
 */
void dsp16_filt_nlms(dsp16_t *x, dsp16_t *w, int size, dsp16_t new_x, dsp16_t d, dsp16_t *y, dsp16_t *e);

/*! \brief 32-bit fixed point version of the NLMS filter.
 * \ingroup group_filtering
 * \param x A pointer on a 32-bit fixed-point vector of (size) elements that acts as a circular buffer, filled with the input samples. Its elements
 *          have to be initialized to zero and then you just need to reinject this vector each time you call this functions without filling any of its values.
 * \param w A pointer on a 32-bit fixed-point vector of size elements corresponding to the coefficients of the filter. Just initialize its elements
 *          to zero and after several  iterations, this buffer will be filled with the actual coefficients of the filter.
 * \param size The length of the circular buffer (x) and of the coefficient's buffer (w).
 * \param new_x A 32-bit fixed-point value which contains a new input sample signal.
 * \param d A 32-bit fixed-point value which contains the current sample of the reference's signal.
 * \param y A pointer on a 32-bit fixed-point value corresponding to the current sample of the output signal.
 * \param e A pointer on a 32-bit fixed-point value corresponding to the current sample of the output error signal.
 */
void dsp32_filt_nlms(dsp32_t *x, dsp32_t *w, int size, dsp32_t new_x, dsp32_t d, dsp32_t *y, dsp32_t *e);
void dsp32_filt_nlms_fir(dsp32_t *x, dsp32_t *w, int size, dsp32_t *y, int i);

/*! \addtogroup group_filtering
 * \section Interpolation Interpolation filter
 * This function performs an interpolation over the input signal. It returns a (\b vect2_size * \b interpolation_ratio)-length signal.\n
 * Here is the principle of the interpolation:
 * \image html filtering_interpolation.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.
 *
 * Relative functions:
 * - \ref dsp16_filt_interpolation_coefsort
 * - \ref dsp16_filt_interpolation
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief This function resort the coefficients of a FIR filter to be used with
 *         the function \ref dsp16_filt_interpolation.
 * \ingroup group_filtering
 * \param fir_coefs A pointer on a 16-bit fixed-point vector where the coefficients
 *                  are stored.
 * \param n_tap The number of tap of this filter.
 * \param interpolation_ratio The interpolation factor of the interpolation to
 *                            perform with this filter.
 */
void dsp16_filt_interpolation_coefsort(dsp16_t *fir_coefs, int n_tap, int interpolation_ratio);
/*! \brief This function interpolates a vector.
 * \ingroup group_filtering
 * \param vect1 A pointer on a 16-bit fixed-point vector where to store the result.
 *              It must be of a size (in sample) equals to the size of the input
 *              buffer multiplied by the interpolation factor.
 * \param vect2 A pointer on a 16-bit fixed-point vector containing the input samples.
 * \param vect2_size The size of the input buffer.
 * \param h A pointer on a 16-bit fixed-point vector which contains the coefficients
 *          of the filter. These coefficients must be reorder with the function
 *          \ref dsp16_filt_interpolation_coefsort before being used.
 * \param h_size The size of this buffer.
 * \param interpolation_ratio The interpolation factor desired for this interpolation.
 */
void dsp16_filt_interpolation(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio);


/*! \addtogroup group_filtering
 * \section lp_FIR_design Lowpass FIR design
 * These functions calculate lowpass FIR filter's coefficients.
 * \note It does not take care of overflowing values.
 *
 * Relative functions:
 * - \ref dsp16_filt_lpfirdesign
 * - \ref dsp16_filt_lpfirdesign_windowed_sinc
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the low-pass FIR filter design.
 * \ingroup group_filtering
 * \param c A pointer on a 16-bit fixed-point vector of "order" size, used to store the coefficients of the filter designed.
 * \param fc Cutoff frequency of the low-pass filter.
 * \param fs Sample rate of the signal to filter.
 * \param order Order of the filter to design.
 */
void dsp16_filt_lpfirdesign(dsp16_t *c, int fc, int fs, int order);

/*! \brief 16-bit fixed point version of the windowed sinc low-pass FIR filter design.
 * \ingroup group_filtering
 * \param c A pointer on a 16-bit fixed-point vector of "order" size, used to store the coefficients of the filter designed.
 * \param fc Cutoff frequency of the low-pass filter.
 * \param fs Sample rate of the signal to filter.
 * \param order Order of the filter to design.
 */
void dsp16_filt_lpfirdesign_windowed_sinc(dsp16_t *c, int fc_hz, int fs_hz, int order);

#endif // __AVR32_ABI_COMPILER__

#endif //__DSP_FILTERING_H__

/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented operators
 *
 * This file show a list of the operators currently implemented in the DSP library.
 *
 ******************************************************************************/

/**
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
 */
/* - \ref Multiplication
 * - \ref Division
 * - \ref Sine
 * - \ref Cosine
 * - \ref Arc_sine
 * - \ref Arc_cosine
 * - \ref Absolute
 * - \ref Square_root
 * - \ref Natural_logarithm
 * - \ref Binary_logarithm
 * - \ref Common_logarithm
 * - \ref Exponential
 * - \ref Power
 * - \ref Random
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

#ifndef __DSP_OPERATORS_H__
#define __DSP_OPERATORS_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \addtogroup group_operators
 * \section Multiplication Multiplication
 * These functions multiply two fixed-point numbers.
 *
 * Relative functions:
 * - \ref dsp16_op_mul
 * - \ref dsp32_op_mul
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the multiplication function.
 * \ingroup group_operators
 * \param num1 The first member of the multiplication.
 * \param num2 The second member of the multiplication.
 * \return The result of the multiplication.
 */
inline static dsp16_t dsp16_op_mul(dsp16_t num1, dsp16_t num2) { return (dsp16_t) ((((int32_t) num1)*((int32_t) num2)) >> DSP16_QB); }
/*! \brief 32-bit fixed point version of the multiplication function.
 * \ingroup group_operators
 * \param num1 The first member of the multiplication.
 * \param num2 The second member of the multiplication.
 * \return The result of the multiplication.
 */
inline static dsp32_t dsp32_op_mul(dsp32_t num1, dsp32_t num2) { return (dsp32_t) ((((S64) num1)*((S64) num2)) >> DSP32_QB); }

/*! \addtogroup group_operators
 * \section Division Division
 * These functions divide two fixed-point numbers.
 * Relative functions:
 * - \ref dsp16_op_div
 * - \ref dsp32_op_div
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the division function.
 * \ingroup group_operators
 * \param num The numerator of the division.
 * \param den The denominator of the division.
 * \return The result of the division.
 */
inline static dsp16_t dsp16_op_div(dsp16_t num, dsp16_t den) { return (dsp16_t) ((((int32_t) num) << DSP16_QB)/((int32_t) den)); }
/*! \brief 32-bit fixed point version of the division function.
 * \ingroup group_operators
 * \param num The numerator of the division.
 * \param den The denominator of the division.
 * \return The result of the division.
 */
inline static dsp32_t dsp32_op_div(dsp32_t num, dsp32_t den) { return (dsp32_t) ((((S64) num) << DSP32_QB)/((S64) den)); }

/*! \addtogroup group_operators
 * \section Sine Sine
 * These functions compute the sine of the argument 'angle'.
 * \note Angle is scaled to fit in the range [-1; 1], where -1 corresponds to -pi and 1 to pi.
 *
 * Relative functions:
 * - \ref dsp16_op_sin
 * - \ref dsp32_op_sin
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the sine function.
 * \ingroup group_operators
 * \param angle The angle to compute
 * \return The sine of 'angle' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the angle,
 * the function will take the modulo 1 of this angle.
 */
dsp16_t dsp16_op_sin(dsp16_t angle);
/*! \brief 32-bit fixed point version of the sine function.
 * \ingroup group_operators
 * \param angle The angle to compute
 * \return The sine of 'angle' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the angle,
 * the function will take the modulo 1 of this angle.
 */
dsp32_t dsp32_op_sin(dsp32_t angle);

/*! \addtogroup group_operators
 * \section Cosine Cosine
 * These functions compute the cosine of the argument 'angle'.
 * Angles are scale to fit in the range [-1; 1], where -1 corresponds to -pi and 1 to pi.
 * Relative functions:
 * - \ref dsp16_op_cos
 * - \ref dsp32_op_cos
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the cosine function.
 * \ingroup group_operators
 * \param angle The angle to compute
 * \return The cosine of 'angle' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the angle,
 * the function will take the modulo 1 of this angle.
 */
inline static dsp16_t dsp16_op_cos(dsp16_t angle) { return dsp16_op_sin(angle + DSP16_Q(0.5)); }
/*! \brief 32-bit fixed point version of the cosine function.
 * \ingroup group_operators
 * \param angle The angle to compute
 * \return The cosine of 'angle' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the angle,
 * the function will take the modulo 1 of this angle.
 */
inline static dsp32_t dsp32_op_cos(dsp32_t angle) { return dsp32_op_sin(angle + DSP32_Q(0.5)); }

/*! \addtogroup group_operators
 * \section Arc_sine Arc sine
 * These functions compute the arc sine of the argument.
 * \note The output is scaled to fit in the range [-0.5; 0.5], where -0.5 corresponds to -pi/2 and 0.5 to pi/2.
 *
 * Relative functions:
 * - \ref dsp16_op_asin
 * - \ref dsp32_op_asin
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the arc sine function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The arc sine of 'number' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the 'number',
 * the function will limit the output to the range [-0.5; 0.5].
 */
dsp16_t dsp16_op_asin(dsp16_t number);
/*! \brief 32-bit fixed point version of the arc sine function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The arc sine of 'number' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the 'number',
 * the function will limit the output to the range [-0.5; 0.5].
 */
dsp32_t dsp32_op_asin(dsp32_t number);

/*! \addtogroup group_operators
 * \section Arc_cosine Arc cosine
 * These functions compute the arc cosine of the argument.
 * \note The output is scaled to fit in the range [0.; 1.], where 0. corresponds to 0. and 1. to pi.
 *
 * Relative functions:
 * - \ref dsp16_op_acos
 * - \ref dsp32_op_acos
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the arc cosine function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The arc cosine of 'number' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the 'number',
 * the function will limit the output to the range [0.; 1.].
 */
inline static dsp16_t dsp16_op_acos(dsp16_t number) { return (DSP16_Q(0.5)-1) - dsp16_op_asin(number); }
/*! \brief 32-bit fixed point version of the arc cosine function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The arc cosine of 'number' is returned.
 * \note If you are using a number greater than 1 (or lower than -1) for the 'number',
 * the function will limit the output to the range [0.; 1.].
 */
inline static dsp32_t dsp32_op_acos(dsp32_t number) { return (DSP32_Q(0.5)-1) - dsp32_op_asin(number); }

/*! \addtogroup group_operators
 * \section Absolute Absolute
 * These functions compute the absolute value of the argument.
 * Relative functions:
 * - \ref dsp16_op_abs
 * - \ref dsp32_op_abs
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the absolute function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The absolute value of the number.
 */
inline static dsp16_t dsp16_op_abs(dsp16_t number) { return (number < 0)?-number:number; }
/*! \brief 32-bit fixed point version of the absolute function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The absolute value of the number.
 */
inline static dsp32_t dsp32_op_abs(dsp32_t number) { return (number < 0)?-number:number; }

/*! \addtogroup group_operators
 * \section Square_root Square root
 * These functions compute the square root of the argument.
 * Relative functions:
 * - \ref dsp16_op_sqrt
 * - \ref dsp32_op_sqrt
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the square root function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The square root of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return 0.
 */
dsp16_t dsp16_op_sqrt(dsp16_t number);
/*! \brief 32-bit fixed point version of the square root function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The square root of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return 0.
 */
dsp32_t dsp32_op_sqrt(dsp32_t number);

/*! \addtogroup group_operators
 * \section Natural_logarithm Natural logarithm
 * These functions compute the natural logarithm of the argument.
 * Relative functions:
 * - \ref dsp16_op_ln
 * - \ref dsp32_op_ln
 *
 * \note The output will be limit in the range of the fixed point format used.
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the natural logarithm function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The natural logarithm of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return DSP_Q_MIN(DSP16_QA, DSP16_QB).
 */
dsp16_t dsp16_op_ln(dsp16_t number);
/*! \brief 32-bit fixed point version of the natural logarithm function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The natural logarithm of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return DSP_Q_MIN(DSP32_QA, DSP32_QB).
 */
dsp32_t dsp32_op_ln(dsp32_t number);

/*! \addtogroup group_operators
 * \section Binary_logarithm Binary logarithm or base 2 logarithm
 * These functions compute the binary logarithm of the argument.
 * \note The output will be limit in the range of the fixed point format used.
 *
 * Relative functions:
 * - \ref dsp16_op_log2
 * - \ref dsp32_op_log2
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the binary logarithm function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The binary logarithm of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return DSP_Q_MIN(DSP16_QA, DSP16_QB).
 */
dsp16_t dsp16_op_log2(dsp16_t number);
/*! \brief 32-bit fixed point version of the binary logarithm function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The binary logarithm of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return DSP_Q_MIN(DSP32_QA, DSP32_QB).
 */
dsp32_t dsp32_op_log2(dsp32_t number);

/*! \addtogroup group_operators
 * \section Common_logarithm Common logarithm or base 10 logarithm
 * These functions compute the common logarithm of the argument.
 * \note The output will be limit in the range of the fixed point format used.
 *
 * Relative functions:
 * - \ref dsp16_op_log10
 * - \ref dsp32_op_log10
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the common logarithm function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The common logarithm of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return DSP_Q_MIN(DSP16_QA, DSP16_QB).
 */
dsp16_t dsp16_op_log10(dsp16_t number);
/*! \brief 32-bit fixed point version of the common logarithm function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The common logarithm of 'number' is returned.
 * \note If you are using a number lower than 0 for the number,
 * the function will return DSP_Q_MIN(DSP32_QA, DSP32_QB).
 */
dsp32_t dsp32_op_log10(dsp32_t number);

/*! \addtogroup group_operators
 * \section Exponential Exponential
 * These functions compute the exponential of the argument.
 * \note The output will be limit in the range of the fixed point format used.
 *
 * Relative functions:
 * - \ref dsp16_op_exp
 * - \ref dsp32_op_exp
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the exponential function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The exponential of 'number' is returned.
 */
dsp16_t dsp16_op_exp(dsp16_t number);
/*! \brief 32-bit fixed point version of the exponential function.
 * \ingroup group_operators
 * \param number The number to compute
 * \return The exponential of 'number' is returned.
 */
dsp32_t dsp32_op_exp(dsp32_t number);


/*! \addtogroup group_operators
 * \section Power Power
 * These functions compute x^y.
 * \note The output will be limit in the range of the fixed point format used.
 *
 * Relative functions:
 * - \ref dsp16_op_pow
 * - \ref dsp32_op_pow
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the power function.
 * \ingroup group_operators
 * \param x The number from which the power has to be applied.
 * \param y The power.
 * \return The x power y is returned.
 * \return \ref DSP_Q_MIN if the argument x is negative: this would result in a complex number.
 */
dsp16_t dsp16_op_pow(dsp16_t x, dsp16_t y);
/*! \brief 32-bit fixed point version of the power function.
 * \ingroup group_operators
 * \param x The number from which the power has to be applied.
 * \param y The power.
 * \return The x power y is returned.
 * \return \ref DSP_Q_MIN if the argument x is negative: this would result in a complex number.
 */
dsp32_t dsp32_op_pow(dsp32_t x, dsp32_t y);

/*! \addtogroup group_operators
 * \section Random Random
 * These functions generate a pseudo-radomized number
 * Relative functions:
 * - \ref dsp_op_srand
 * - \ref dsp16_op_rand
 * - \ref dsp32_op_rand
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief Initialize the pseudo-random number generator.
 * \ingroup group_operators
 * \param new_seed An integer value to be used as seed by the pseudo-random number generator algorithm.
 */
void dsp_op_srand(int new_seed);
/*! \brief 16-bit fixed point version of the random operator.
 * \ingroup group_operators
 * \return The cosine of 'angle' is returned.
 * \note The number generated can be any 16-bit value in the range [-1; 1[.
 */
dsp16_t dsp16_op_rand(void);
/*! \brief 32-bit fixed point version of the random operator.
 * \ingroup group_operators
 * \return The cosine of 'angle' is returned.
 * \note The number generated can be any 32-bit value in the range [-1; 1[.
 */
dsp32_t dsp32_op_rand(void);

#endif // __AVR32_ABI_COMPILER__

#endif //__DSP_OPERATORS_H__

/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented signal generation functions
 *
 * This file show a list of the signal generation functions currently implemented in the DSP library.
 *
 ******************************************************************************/

/**
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
 */

/*! \addtogroup group_signal_generation
 * - Periodic
 *  - \ref Cosinusoidal
 *  - \ref Sinusoidal
 *  - \ref Square
 *  - \ref Rectangular
 *  - \ref Saw_tooth
 *  - \ref Dirac_comb
 * - Non-periodic
 *  - \ref Noise
 *  - \ref Ramp
 *  - \ref Step
 *  - \ref Dirac
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

#ifndef __DSP_SIGNAL_GENERATION_H__
#define __DSP_SIGNAL_GENERATION_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \addtogroup group_signal_generation
 * \section Sinusoidal Sinusoidal
 * \image html signal_generation_sinusoidal.gif
 * This function generates a sinusoidal signal with a specified frequency,
 * sampling frequency and phase.
 * \note The amplitude of the signal fits in the range [-1; 1].
 * \note The phase is scaled to fit in the range [-1; 1], where -1 corresponds to -pi and 1 to pi.
 *
 * Relative functions:
 * - \ref dsp16_gen_sin
 * - \ref dsp32_gen_sin
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the sinusoidal signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param phase The phase of the output signal.
 * \return The phase of the signal generated.
 */
dsp16_t dsp16_gen_sin(dsp16_t *vect1, int size, int f, int fs, dsp16_t phase);
/*! \brief 32-bit fixed point version of the sinusoidal signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param phase The phase of the output signal.
 * \return The phase of the signal generated.
 */
dsp32_t dsp32_gen_sin(dsp32_t *vect1, int size, int f, int fs, dsp32_t phase);


/*! \addtogroup group_signal_generation
 * \section Cosinusoidal Cosinusoidal
 * \image html signal_generation_cosinusoidal.gif
 * This function generates a cosinusoidal signal with a specified frequency,
 * sampling frequency and phase.
 * \note The amplitude of the signal fits in the range [-1; 1].
 * \note The phase is scaled to fit in the range [-1; 1], where -1 corresponds to -pi and 1 to pi.
 *
 * Relative functions:
 * - \ref dsp16_gen_cos
 * - \ref dsp32_gen_cos
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the cosinusoidal signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param phase The phase of the output signal.
 * \return The phase of the signal generated.
 */
dsp16_t dsp16_gen_cos(dsp16_t *vect1, int size, int f, int fs, dsp16_t phase);
/*! \brief 32-bit fixed point version of the cosinusoidal signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param phase The phase of the output signal.
 * \return The phase of the signal generated.
 */
dsp32_t dsp32_gen_cos(dsp32_t *vect1, int size, int f, int fs, dsp32_t phase);


/*! \addtogroup group_signal_generation
 * \section Noise Noise
 * \image html signal_generation_noise.gif
 * This function generates a noise with a specified amplitude.
 * \note The amplitude of the signal fits in the range [-1; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_noise
 * - \ref dsp32_gen_noise
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the noise generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param amp The amplitude of the signal.
 */
void dsp16_gen_noise(dsp16_t *vect1, int size, dsp16_t amp);
/*! \brief 32-bit fixed point version of the noise generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param amp The amplitude of the signal.
 */
void dsp32_gen_noise(dsp32_t *vect1, int size, dsp32_t amp);

/*! \addtogroup group_signal_generation
 * \section Rectangular Rectangular
 * \image html signal_generation_rectangular.gif
 * This function generates a rectangular signal.
 * \note The amplitude of the signal fits in the range [-1; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_rect
 * - \ref dsp32_gen_rect
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the rectangular signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param duty The duty cycle of the output signal. The duty cycle is a number in the range ]0; 1]
 * which is the ratio between the pulse duration and the period of the waveform.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 */
void dsp16_gen_rect(dsp16_t *vect1, int size, int f, int fs, dsp16_t duty, dsp16_t delay);
/*! \brief 32-bit fixed point version of the rectangular signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param duty The duty cycle of the output signal. The duty cycle is a number in the range ]0; 1]
 * which is the ratio between the pulse duration and the period of the waveform.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 */
void dsp32_gen_rect(dsp32_t *vect1, int size, int f, int fs, dsp32_t duty, dsp32_t delay);

/*! \addtogroup group_signal_generation
 * \section Square Square
 * \image html signal_generation_square.gif
 * This function generates a square signal.
 * \note The amplitude of the signal fits in the range [-1; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_sqr
 * - \ref dsp32_gen_sqr
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the square signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 */
inline static void dsp16_gen_sqr(dsp16_t *vect1, int size, int f, int fs, dsp16_t delay) { dsp16_gen_rect(vect1, size, f, fs, DSP16_Q(0.5), delay); }
/*! \brief 32-bit fixed point version of the square signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 */
inline static void dsp32_gen_sqr(dsp32_t *vect1, int size, int f, int fs, dsp32_t delay) { dsp32_gen_rect(vect1, size, f, fs, DSP32_Q(0.5), delay); }

/*! \addtogroup group_signal_generation
 * \section Saw_tooth Saw tooth
 * \image html signal_generation_saw_tooth.gif
 * This function generates a saw tooth signal.
 * \note The amplitude of the signal fits in the range [-1; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_saw
 * - \ref dsp32_gen_saw
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the saw tooth signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param duty The duty cycle of the output signal. The duty cycle is a number in the range ]0; 1]
 * which is the ratio between the pulse duration and the period of the waveform.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 * \return The phase of the signal generated.
 */
dsp16_t dsp16_gen_saw(dsp16_t *vect1, int size, int f, int fs, dsp16_t duty, dsp16_t delay);
/*! \brief 32-bit fixed point version of the saw tooth signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param duty The duty cycle of the output signal. The duty cycle is a number in the range ]0; 1]
 * which is the ratio between the pulse duration and the period of the waveform.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 * \return The phase of the signal generated.
 */
dsp32_t dsp32_gen_saw(dsp32_t *vect1, int size, int f, int fs, dsp32_t duty, dsp32_t delay);

/*! \addtogroup group_signal_generation
 * \section Dirac_comb Dirac comb
 * \image html signal_generation_dirac_comb.gif
 * This function generates a dirac comb signal.
 * \note The amplitude of the signal fits in the range [0; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_dcomb
 * - \ref dsp32_gen_dcomb
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the dirac comb signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 */
void dsp16_gen_dcomb(dsp16_t *vect1, int size, int f, int fs, dsp16_t delay);
/*! \brief 32-bit fixed point version of the dirac comb signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param f The frequency of the output signal.
 * \param fs The sampling frequency of the output signal.
 * \param delay The delay of the periodic waveform. The delay must feet in the range [0; 1]
 * where 1 to T.
 */
void dsp32_gen_dcomb(dsp32_t *vect1, int size, int f, int fs, dsp32_t delay);

/*! \addtogroup group_signal_generation
 * \section Ramp Ramp
 * \image html signal_generation_ramp.gif
 * This function generates a ramp.
 * \note The amplitude of the signal fits in the range [0; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_ramp
 * - \ref dsp32_gen_ramp
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the ramp signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param increment The incrementation of the signal per vector's element.
 */
void dsp16_gen_ramp(dsp16_t *vect1, int size, dsp16_t increment);
/*! \brief 32-bit fixed point version of the ramp signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param increment The incrementation of the signal per vector's element.
 */
void dsp32_gen_ramp(dsp32_t *vect1, int size, dsp32_t increment);

/*! \addtogroup group_signal_generation
 * \section Step Step
 * \image html signal_generation_step.gif
 * This function generates a step.
 *
 * Relative functions:
 * - \ref dsp16_gen_step
 * - \ref dsp32_gen_step
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the dirac comb signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param intial_value The initial value of the signal.
 * \param final_value The final value of the signal.
 * \param step_index The index in the buffer, where the step appears.
 */
void dsp16_gen_step(dsp16_t *vect1, int size, dsp16_t intial_value, dsp16_t final_value, int step_index);
/*! \brief 32-bit fixed point version of the dirac comb signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param intial_value The initial value of the signal.
 * \param final_value The final value of the signal.
 * \param step_index The index in the buffer, where the step appears.
 */
void dsp32_gen_step(dsp32_t *vect1, int size, dsp32_t intial_value, dsp32_t final_value, int step_index);

/*! \addtogroup group_signal_generation
 * \section Dirac Dirac
 * \image html signal_generation_dirac.gif
 * This function generates a dirac.
 * \note The amplitude of the signal fits in the range [0; 1].
 *
 * Relative functions:
 * - \ref dsp16_gen_dirac
 * - \ref dsp32_gen_dirac
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the dirac comb signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 16-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param dirac_index The index in the buffer, where the dirac appears.
 */
void dsp16_gen_dirac(dsp16_t *vect1, int size, int dirac_index);
/*! \brief 32-bit fixed point version of the dirac comb signal generation.
 * \ingroup group_signal_generation
 * \param vect1 A pointer on a 32-bit vector which is the output buffer of this function.
 * \param size The number of elements of the output vector.
 * \param dirac_index The index in the buffer, where the dirac appears.
 */
void dsp32_gen_dirac(dsp32_t *vect1, int size, int dirac_index);

#endif // __AVR32_ABI_COMPILER__
#endif //__DSP_SIGNAL_GENERATION_H__

/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented transform functions
 *
 * This file show a list of the transform functions currently implemented in the DSP library.
 *
 ******************************************************************************/

/**
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
 */
/* - \ref Complex_FFT
 * - \ref Complex_iFFT
 * - \ref Real_Complex_FFT
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

#ifndef __DSP_TRANSFORMS_H__
#define __DSP_TRANSFORMS_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \addtogroup group_transforms
 * \section Complex_FFT Complex Fast Fourier Transform
 * This function computes a complex FFT from an input signal.
 * It uses the Radix-4 "Decimate In Time" algorithm and does not perform a calculation "in place"
 * which means that the input buffer has to be different from the output buffer.\n
 * Here is the formula of the FFT:
 * \image html transforms_fft_equation.gif
 * Relative functions:
 * - \ref dsp16_trans_complexifft
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the complex iFFT algorithm.
 * \ingroup group_transforms
 * \param vect1 A pointer on a 16-bit complex vector which is the output buffer of this function.
 * \param vect2 A pointer on a 16-bit complex vector which is the input buffer of this function.
 * \param nlog It is the base-2-logarithm of the size of the input/output vector.\n
 * Due to its implementation, this function computes only 4^n-point complex iFFT. Therefore, the \b nlog argument has to be \b even.
 * \note The following requirements have to be matched:
 * - The size of the output buffer has to be the same as the size of the input buffer.
 * - This function uses a static twiddle factors table which determines the maximal FFT size
 * which can be performed. By default it is 1024. Please check the value of \ref DSP16_N_TWIDDLE_FACTORS.
 * - \a vect1 and \a vect2 MUST be 4-byte aligned. Please use \ref A_ALIGNED to do so.
 * - To avoid overflowing values, the resulting vector amplitude is scaled by 2^\b nlog.
 */
void dsp16_trans_complexifft(dsp16_complex_t *vect1, dsp16_complex_t *vect2, int nlog);

/*! \addtogroup group_transforms
 * \section Complex_iFFT Complex inverse Fast Fourier Transform
 * This function computes a complex inverse FFT from an input signal.
 * It uses the Radix-4 "Decimate In Time" algorithm and does not perform a calculation "in place"
 * which means that the input buffer has to be different from the output buffer.\n
 * Here is the formula of the iFFT:
 * \image html transforms_ifft_equation.gif
 * Relative functions:
 * - \ref dsp16_trans_complexfft
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the complex FFT algorithm.
 * \ingroup group_transforms
 * \param vect1 A pointer on a 16-bit complex vector which is the output buffer of this function.
 * \param vect2 A pointer on a 16-bit complex vector which is the input buffer of this function.
 * \param nlog It is the base-2-logarithm of the size of the input/output vector.\n
 * Due to its implementation, this function computes only 4^n-point complex FFT. Therefore, the \b nlog argument has to be \b even.
 * \note The following requirements have to be matched:
 * - The size of the output buffer has to be the same as the size of the input buffer.
 * - This function uses a static twiddle factors table which determines the maximal FFT size
 * which can be performed. By default it is 1024. Please check the value of \ref DSP16_N_TWIDDLE_FACTORS.
 * - \a vect1 and \a vect2 MUST be 4-byte aligned. Please use \ref A_ALIGNED to do so.
 * - To avoid overflowing values, the resulting vector amplitude is scaled by 2^\b nlog.
 */
void dsp16_trans_complexfft(dsp16_complex_t *vect1, dsp16_complex_t *vect2, int nlog);

/*! \addtogroup group_transforms
 * \section Real_Complex_FFT Real to complex Fast Fourier Transform
 * This function computes a complex FFT from a real input signal.
 * It uses the Radix-4 "Decimate In Time" algorithm and does not perform a calculation "in place"
 * which means that the input buffer has to be different from the output buffer.\n
 * Here is the formula of the FFT:
 * \image html transforms_fft_equation.gif
 * Relative functions:
 * - \ref dsp16_trans_realcomplexfft
 * - \ref dsp32_trans_realcomplexfft
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */
/*! \brief 16-bit fixed point version of the real to complex FFT algorithm.
 * \ingroup group_transforms
 * \param vect1 A pointer on a 16-bit complex vector which is the output buffer of this function.
 * \param vect2 A pointer on a 16-bit real vector which is the input buffer of this function.
 * \param nlog It is the base-2-logarithm of the size of the input/output vector.\n
 * Due to its implementation, this function computes only 4^n-point complex FFT. Therefore, the \b nlog argument has to be \b even.
 * \note The following requirements have to be matched:
 * - The size of the output buffer has to be the same as the size of the input buffer.
 * - This function uses a static twiddle factors table which determines the maximal FFT size
 * which can be performed. By default it is 1024. Please check the value of \ref DSP16_N_TWIDDLE_FACTORS.
 * - \a vect1 and \a vect2 MUST be 4-byte aligned. Please use \ref A_ALIGNED to do so.
 * - To avoid overflowing values, the resulting vector amplitude is scaled by 2^\b nlog.
 */
void dsp16_trans_realcomplexfft(dsp16_complex_t *vect1, dsp16_t *vect2, int nlog);
/*! \brief 32-bit fixed point version of the real to complex FFT algorithm.
 * \ingroup group_transforms
 * \param vect1 A pointer on a 32-bit complex vector which is the output buffer of this function.
 * \param vect2 A pointer on a 32-bit real vector which is the input buffer of this function.
 * \param nlog It is the base-2-logarithm of the size of the input/output vector.\n
 * Due to its implementation, this function computes only 4^n-point complex FFT. Therefore, the \b nlog argument has to be \b even.
 * \note The following requirements have to be matched:
 * - The size of the output buffer has to be the same as the size of the input buffer.
 * - This function uses a static twiddle factors table which determines the maximal FFT size
 * which can be performed. By default it is 1024. Please check the value of \ref DSP32_N_TWIDDLE_FACTORS.
 * - \a vect1 and \a vect2 MUST be 4-byte aligned. Please use \ref A_ALIGNED to do so.
 * - To avoid overflowing values, the resulting vector amplitude is scaled by 2^\b nlog.
 */
void dsp32_trans_realcomplexfft(dsp32_complex_t *vect1, dsp32_t *vect2, int nlog);

#endif // __AVR32_ABI_COMPILER__
#endif //__DSP_TRANSFORMS_H__
/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented windowing functions
 *
 * This file show a list of the windowing functions currently implemented in the DSP library.
 *
 ******************************************************************************/

/**
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
 */
/* - \ref Rectangular
 * - \ref Bartlett
 * - \ref Blackman
 * - \ref Hamming
 * - \ref Gauss
 * - \ref Hann
 * - \ref Kaiser
 * - \ref Welch
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */


#ifndef __DSP_WINDOWING_H__
#define __DSP_WINDOWING_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \addtogroup group_windowing
 * \section Rectangular Rectangular
 * \image html windowing_rectangular.gif
 * These functions generate a rectangular window that fits in the output buffer.\n
 *  The rectangular window filled the output buffer with 1.
 *
 * Relative functions:
 * - \ref dsp16_win_rect
 * - \ref dsp32_win_rect
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the rectangular windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_rect(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the rectangular windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_rect(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_windowing
 * \section Bartlett Bartlett
 * \image html windowing_bartlett.gif
 * Also known simply as the triangular windows, these functions generate
 * a Bartlett window that fits in the output buffer.\n
 * The amplitude of the signal is in the range [0; 1]
 *
 * Relative functions:
 * - \ref dsp16_win_bart
 * - \ref dsp32_win_bart
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the Bartlett windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_bart(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the Bartlett windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_bart(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_windowing
 * \section Blackman Blackman
 * \image html windowing_blackman.gif
 * These functions generate a Blackman window that fits in the output buffer.\n
 * The amplitude of the signal is in the range [0; 1]
 *
 * Relative functions:
 * - \ref dsp16_win_black
 * - \ref dsp32_win_black
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the Blackman windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_black(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the Blackman windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_black(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_windowing
 * \section Hamming Hamming
 * \image html windowing_hamming.gif
 * These functions generate a hamming window that fits in the output buffer.\n
 * The amplitude of the signal is in the range [0; 1]
 *
 * Relative functions:
 * - \ref dsp16_win_hamm
 * - \ref dsp32_win_hamm
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the hamming windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_hamm(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the hamming windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_hamm(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_windowing
 * \section Gauss Gauss
 * \image html windowing_gauss.gif
 * These functions generate a Gaussian window that fits in the output buffer.\n
 * The amplitude of the signal is in the range [0; 1]
 * \note You can change the value of the teta coefficient by defining DSP_GAUSS_TETA
 * at the compilation with a value that fits in the range ]0; 0.5]
 *
 * Relative functions:
 * - \ref dsp16_win_gauss
 * - \ref dsp32_win_gauss
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the Gaussian windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_gauss(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the Gaussian windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_gauss(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_windowing
 * \section Hann Hann
 * \image html windowing_hann.gif
 * These functions generate a hann window that fits in the output buffer.\n
 * The amplitude of the signal is in the range [0; 1]
 *
 * Relative functions:
 * - \ref dsp16_win_hann
 * - \ref dsp32_win_hann
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the hann windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_hann(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the hann windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_hann(dsp32_t *vect1, dsp32_t *vect2, int size);

/*! \addtogroup group_windowing
 * \section Kaiser Kaiser
 * \image html windowing_kaiser.gif
 * These functions generate a kaiser window that fits in the output buffer.\n
 * The amplitude of the signal is in the range [0; 1]
 *
 * Relative functions:
 * - \ref dsp16_win_kaiser
 * - \ref dsp32_win_kaiser
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the kaiser windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 * \param alpha The alpha coefficient which must be greater than 0.
 */
void dsp16_win_kaiser(dsp16_t *vect1, dsp16_t *vect2, int size, int alpha);
/*! \brief 32-bit fixed point version of the kaiser windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 * \param alpha The alpha coefficient which must be greater than 0.
 */
void dsp32_win_kaiser(dsp32_t *vect1, dsp32_t *vect2, int size, int alpha);

/*! \addtogroup group_windowing
 * \section Welch Welch
 * \image html windowing_welch.gif
 * These functions generate a Welch window that fits in the output buffer.\n
 * The Welch window is commonly used as a window for power spectral estimation.\n
 * The amplitude of the signal is in the range [0; 1]
 *
 * Relative functions:
 * - \ref dsp16_win_welch
 * - \ref dsp32_win_welch
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

/*! \brief 16-bit fixed point version of the Welch windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_welch(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the Welch windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 32-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 32-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp32_win_welch(dsp32_t *vect1, dsp32_t *vect2, int size);

#endif // __AVR32_ABI_COMPILER__
#endif // __DSP_WINDOWING_H__
/*****************************************************************************
 *
 * \file
 *
 * \brief This module provides a useful set of IMA/DVI ADPCM coding and decoding functions.
 *
 ******************************************************************************/

/**
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
 */
/*! \addtogroup group_adpcm
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */

#ifndef __ADPCM_H__
#define __ADPCM_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \brief IMA/DVI ADPCM sample encoder
 * \ingroup group_adpcm
 * \param nibble The sample to encode.
 * \param step_index A pointer on a 16 bits data which contain the current
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the current
 * predicted value of the ADPCM algorithm.
 * \return A 4-bit data that corresponds to the sample encoded.
 */
int8_t dsp_adpcm_ima_encode_nibble(int16_t nibble, int16_t *step_index, int16_t *predicted_value);

/*! \brief IMA/DVI ADPCM sample decoder
 * \ingroup group_adpcm
 * \param nibble The sample to decode. It must be a 4-bit data.
 * \param step_index A pointer on a 16 bits data which contain the previous
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the previous
 * predicted value of the ADPCM algorithm.
 * \return A 16-bit data that corresponds to the sample decoded.
 */
int16_t dsp_adpcm_ima_decode_nibble(int8_t nibble, int16_t *step_index, int16_t *predicted_value);

/*! \brief IMA/DVI ADPCM encoder
 * \ingroup group_adpcm
 * \param out A 4-bit data vector that will contain the encoded data.
 * \param in A 16-bit data vector that contains the data to encode.
 * \param size The number of data to encode.
 * \param step_index A pointer on a 16 bits data which contain the current
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the current
 * predicted value of the ADPCM algorithm.
 * \note Can be performed "in-place".
 */
void dsp_adpcm_ima_encode(void *out, int16_t *in, int size, int16_t *step_index, int16_t *predicted_value);

/*! \brief IMA/DVI ADPCM decoder
 * \ingroup group_adpcm
 * \param out A 16-bit data vector that will contain the decoded data.
 * \param in A 4-bit data vector that contains the IMA/DVI ADPCM encoded data to decode.
 * \param size The number of data to decode.
 * \param step_index A pointer on a 16 bits data which contain the current
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the current
 * predicted value of the ADPCM algorithm.
 */
void dsp_adpcm_ima_decode(int16_t *out, void *in, int size, int16_t *step_index, int16_t *predicted_value);

#endif // __AVR32_ABI_COMPILER__
#endif //__ADPCM_H__
/*****************************************************************************
 *
 * \file
 *
 * \brief This module provides a useful set of signal re-sampling functions.
 *
 ******************************************************************************/

/**
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
 */
#ifndef __DSP_RESAMPLING_H__
#define __DSP_RESAMPLING_H__

#ifdef __AVR32_ABI_COMPILER__

#include "dsp.h"

/*! \addtogroup group_resampling
 *
 * Following is a brief description of the frequency re-sampling algorithm used in
 * this module. It is aimed for anybody so no specifics digital signal processing
 * knowledges are required to understand this presentation.
 *
 * \section summary Summary
 * The principle is simple, it consists in 2 main stages, up-sampling the signal
 * frequency by an integer value (\b L), this action is also called \b interpolation,
 * and then down-sampling it by another integer value (\b M), also known as \b decimation.
 *
 * \section l_and_m_calculation L and M calculation
 * L and M are 2 integers that are calculated by getting the \b GCD
 * (Greatest Common Divisor) of the input (\b Fsin) and the output (\b Fsout)
 * frequencies.\n
 * The number resulting will divide Fsin and Fsout to respectively
 * give M and L.
 * \image html resampling_l_and_m.gif
 *
 * \section interpolation Interpolation (frequency up-sampling)
 * This process up samples the frequency of the input signal by an integer factor.
 * The factor used at this stage of the process by the re-sampling algorithm
 * will be the pre-calculated "interpolation factor" \b L. Which means, if we
 * consider this process as a black box with 1 input (\b u) and 1 output (\b v),
 * the output signal sampling frequency (Fs(v)) will be equals to the input
 * signal sampling frequency (Fs(u)) multiplied by L.
 * \image html resampling_interpolation.gif
 * The following describes the algorithm used to implement the interpolation.\n
 * The method consists in \b extending the signal by filling "blank spaces" with
 * \b zeros, in order to obtain a signal with the desired sampling rate.
 * \image html resampling_interpolation2.gif
 * Then this signal goes through a filter in order to remove the highest frequencies,
 * to give it back its original shape. The cut off frequency is calculated according
 * to the input frequency of the signal.\n
 * The filter used in this algorithm, is most likely a \b lowpass \b FIR filter, which is used
 * as a \b poly-phase filter. This optimizes greatly the performances of this process
 * because poly-phase filters are simply, classical filters cut into pieces.
 * And in this case, the aim is to have one piece with the original samples and
 * the other with the zeros used to up sample the signal.\n
 * Then, by \b re-ordering the \b coefficients in a certain way, this process is equivalent
 * to apply a filter only on the original sample parts since the result of
 * filtering a null signal is a null signal.
 * \image html resampling_interpolation3.gif
 * Now, the signal is interpolated, it needs to be down sampled.
 *
 * \section decimation Decimation (frequency down-sampling)
 * This process is much simpler than the interpolation.\n
 * It just consists in \b removing \b samples in order to keep the same signal wave
 * form but with a lower sampling rate.\n
 * Therefore, to obtain the desired output sampling frequency, the signal has to
 * be down sampled by \b M (decimation factor).
 * \image html resampling_decimation.gif
 * Every M samples are kept from the input signal and all the others are removed.
 * \image html resampling_decimation2.gif
 *
 * \section conclusion Conclusion
 * By processing these 2 main stages, the signal is re-sampled by a factor equals
 * to \b L/M. Therefore, the smaller the 2 frequencies have their GCD
 * (Greatest Common Divisor), the more memory it will need (to store the FIR
 * filter coefficients).\n
 * This method is one of the most used in digital signal processing systems.
 * It will generate a clean signal and evaluate at best the waveform of the
 * output signal.
 *
 * \section freq_response Frequency response
 * The signal is attenuated on high frequencies. Following are traces showing the
 * frequency response of the re-sampling algorithm over different sampling rate conversions.
 * \image html freq_resp_from_32kHz_to_44.1KHz.jpg "Frequency response from 32KHz to 44.1KHz"
 * \image html freq_resp_from_48kHz_to_48.51KHz.jpg "Frequency response from 48KHz to 48.51KHz"
 */

/*! \brief This structure is used to store the context of streaming information
 *         during resampling process.
 * \ingroup group_resampling
 */
typedef struct
{
  //! Input buffer position for next computation.
  int input_buffer_pos;
  //! Internal index
  int index;
} dsp_resampling_context_t;
/*! \brief This structure is used to store the context of a resampling process.
 * \ingroup group_resampling
 */
typedef struct
{
  //! The sample rate of the input buffer.
  int fs;
  //! The interpolation factor used for the resampling process.
  int interpolation_factor;
  //! The decimation factor used for the resampling process.
  int decimation_factor;
  //! A buffer to store the current buffer + some previous samples.
  void *last_samples;
  //! The size in sample of this buffer.
  int buffer_size;
  //! An array to store the FIR coefficients of the filter.
  void *fir_coefs;
  //! The actual order of the filter. A polyphase filter is used, therefore,
  //! this order is equal to desired the order of the filter * the interpolation
  //! factor.
  int filter_order;
  //! The output buffer size.
  int current_buffer_size;
  //! The number of channels.
  int nb_channels;
  //! If a clean link between to output buffer is required.
  bool link_required;
  //! Context of the process
  dsp_resampling_context_t *context;
} dsp_resampling_t;

/*! \name Resampling setup function
 */
//!  @{
/*! \brief Options to attribute to the resampling algorithm
 * \note The structure must be initialized to 0 before using it. This will ensure
 * all the default options will be chosen.
 * \ingroup group_resampling
 */
typedef struct
{
  //! Method used to generate the filter coefficients.
  enum
  {
    //! Dynamic is the default method.
    DSP16_RESAMPLING_OPTIONS_USE_DYNAMIC = 0,
    //! Use coefficients from a static table.
    DSP16_RESAMPLING_OPTIONS_USE_FIXED
  } coefficients_generation;

  union
  {
    //! If using the dynamic method to generate the filter coefficients
    struct
    {
      //! If set to true, normalize the filter coefficients to ensure the output
      //! won't be too reduced.
      bool coefficients_normalization;
      //! Use a custom cut-off frequency for this filter. By default it will choose min(fsin, fsmax) / 2.
      int custom_fc_hz;
      //! Custom filter function. If set to NULL, the algorithm will use the dsp16_filt_lpfirdesign function.
      void (*dsp16_custom_filter)(dsp16_t *coefficients, int fc_hz, int fs_hz, int order);
      //! Custom window function. If set to NULL, the algorithm will use a rectangular window.
      void (*dsp16_custom_window)(dsp16_t *window, dsp16_t *input, int size);
    } dynamic;
    //! If using the static method to generate the filter coefficients
    struct
    {
      //! Pointer on the fixed coefficient table.
      dsp16_t *dsp16_coefficients;
    } fixed;
  };

} dsp16_resampling_options_t;

/*! \brief A pointer on a memory allocation function
 * \ingroup group_resampling
 */
typedef void *(*malloc_fct_t)(int);
/*! \brief This function is the 16-bit signal resampling setup function.
 *         It has to be called only once at the initialization of the resampling process.
 * \ingroup group_resampling
 * \param input_sample_rate The sample rate of the input signal.
 * \param output_sample_rate The sample rate of the output signal.
 * \param buffer_size The size of the input buffers.
 * \param order The order of the re-sampling module. This defines indirectly the order of the
 *        filter to be used. Its order will be <order> * <interpolation factor>.
 * \param malloc_fct A pointer on a memory allocation function.
 * \param nb_channels The number of channels to compute.
 * \param options Add specific options to the algorithm.
 * \return A pointer on a structure containing the context that will be used during
 *         the re-sampling process.
 * \note The output must be freed with the \ref dsp16_resampling_free function once the
 *       re-sampling process is completed.
 */
dsp_resampling_t *dsp16_resampling_setup(int input_sample_rate, int output_sample_rate, int buffer_size, int order, int nb_channels, malloc_fct_t malloc_fct, dsp16_resampling_options_t *options);
//!  @}

/*! \name Resampling setup function
 */
//!  @{
/*! \brief A pointer on a memory free function
 * \ingroup group_resampling
 */
typedef void (*free_fct_t)(void *);
/*! \brief Function used to free the previously allocted structure issued by the
 *         \ref dsp16_resampling_setup function.
 * \ingroup group_resampling
 * \param dsp_resampling The re-sampling context structure to be freed.
 * \param free_fct A pointer on the free function to be used.
 */
void dsp16_resampling_free(dsp_resampling_t *dsp_resampling, free_fct_t free_fct);
//!  @}

/*! \brief Returns the maximal length in sample of the output signal.
 * \ingroup group_resampling
 * \param dsp_resampling The re-sampling context structure associated.
 */
int dsp16_resampling_get_output_max_buffer_size(dsp_resampling_t *dsp_resampling);

/*! \brief Returns the current length in sample of the output signal.
 * \ingroup group_resampling
 * \param dsp_resampling The re-sampling context structure associated.
 */
int dsp16_resampling_get_output_current_buffer_size(dsp_resampling_t *dsp_resampling);

/*! \brief The re-sampling computation function.
 * \ingroup group_resampling
 * \param dsp_resampling The re-sampling context structure associated.
 * \param output A pointer on a 16-bit vector used to store output data. The length
 *               of this vector is defined by the output of the
 *               \ref dsp16_resampling_get_output_current_buffer_size function.
 * \param input A pointer on a 16-bit vector used as an input to the re-sampling
 *              process. It has to be of a length defined to the
 *              \ref dsp16_resampling_setup function as for its sampling rate.
 * \param channel The channel number to compute (starting from 0 to nb_channels - 1
 *                referred in \ref dsp16_resampling_setup).
 */
void dsp16_resampling_compute(dsp_resampling_t *dsp_resampling, dsp16_t *output, dsp16_t *input, int channel);

/*! \brief Link a stream previously re-sampled using the \a dsp_resampling_input
 *         resampling structure with the new re-sampling structure
 *         \a dsp_resampling_output.
 *         This is used to keep the continuity with two pieces of a stream
 *         re-sampled using two different re-sampling parameters.
 * \ingroup group_resampling
 * \pre Some considerations have to be taken care of before using this function.
 *      The two structure MUST have in common: the number of channels, the filter
 *      order and the input buffer size.
 * \param dsp_resampling_output The re-sampling context which will be updated
 *                              according to the \a dsp_resampling_input context.
 * \param dsp_resampling_input The input re-sampling context.
 *
 * \return \b true if the process succeed, \b false otherwise. A process can fail
 *         only if the preliminary conditions are not respected.
 */
bool dsp16_resampling_link(dsp_resampling_t *dsp_resampling_output, dsp_resampling_t *dsp_resampling_input);

#endif // __AVR32_ABI_COMPILER__
#endif //__DSP_RESAMPLING_H__
