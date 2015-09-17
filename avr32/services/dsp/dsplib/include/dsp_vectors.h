/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented vector management functions
 *
 * This file show a list of the vector management functions currently implemented in the DSP library.
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

/*! \defgroup group_vectors DSPLIB - Vectors
 *  \ingroup group_functions
 *  All the vector management functions currently supported by the DSP library.
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
 *  - \ref Power_
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
 * \section Power_ Power
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
inline static void dsp16_vect_zeropad(dsp16_t *vect1, int size, int num_zero) { memset((U8 *) vect1 + (size - num_zero)*sizeof(dsp16_t), 0, num_zero*sizeof(dsp16_t)); };
/*! \brief 32-bit fixed point version of the zero padding function.
 * \ingroup group_vectors
 * \param vect1 A pointer on the 32 bits real vector that has to be padded with zeros.
 * \param size The size of this vector.
 * \param num_zero The number of zeros to pad at the end of the vector.
 */
inline static void dsp32_vect_zeropad(dsp32_t *vect1, int size, int num_zero) { memset((U8 *) vect1 + (size - num_zero)*sizeof(dsp32_t), 0, num_zero*sizeof(dsp32_t)); };

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

