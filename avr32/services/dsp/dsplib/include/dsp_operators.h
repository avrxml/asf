/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented operators
 *
 * This file show a list of the operators currently implemented in the DSP library.
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
 ******************************************************************************/

/*! \defgroup group_operators DSPLIB - Operators
 *  \ingroup group_functions
 *  All the fixed point operators functions currently implemented in the DSP library.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
inline static dsp16_t dsp16_op_mul(dsp16_t num1, dsp16_t num2) { return (dsp16_t) ((((S32) num1)*((S32) num2)) >> DSP16_QB); }
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
inline static dsp16_t dsp16_op_div(dsp16_t num, dsp16_t den) { return (dsp16_t) ((((S32) num) << DSP16_QB)/((S32) den)); }
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

