/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented filters
 *
 * This file show a list of the filtering functions currently implemented in the DSP library.
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

/*! \defgroup group_filters DSPLIB - Filters
 *  \ingroup group_functions
 *  All the filters functions currently supported by the DSP library.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef __DSP_FILTERS_H__
#define __DSP_FILTERS_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \addtogroup group_filters
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

/*! \addtogroup group_filters
 * \section FIR Finite Impulse Response Filter
 * This function computes a real FIR filter using the impulse response of the desire
 * filter onto a fixed-length signal. It returns a signal of a length equals to (size - h_size + 1) elements.\n
 * Here is the formula of the FIR filter:
 * \image html filters_fir_equation.gif
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
 * \ingroup group_filters
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
 * \ingroup group_filters
 * \param vect1 A pointer on a 32-bit fixed-point vector of (size - h_size + 1) elements corresponding to the output buffer.
 * \param vect2 A pointer on a 32-bit fixed-point vector of size elements corresponding to the input buffer.
 * \param size The length of the input buffer (must be greater or equals to 4).
 * \param h A pointer on a 32-bit fixed-point vector of h_size elements corresponding to the buffer containing the impulse response coefficients.
 * \param h_size The length of the impulse response of the filter (must be greater than 7)
 * \note You need the "Partial Convolution" module
 */
inline static void dsp32_filt_fir(dsp32_t *vect1, dsp32_t *vect2, int size, dsp32_t *h, int h_size) { dsp32_vect_convpart(vect1, vect2, size, h, h_size); };


/*! \addtogroup group_filters
 * \section IIR Infinite Impulse Response Filter
 * This function computes a real IIR filter using the coefficients of the desire
 * filter onto a fixed-length signal. It returns a signal of a length equals to size elements.\n
 * Here is the formula of the IIR filter:
 * \image html filters_iir_equation.gif
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
 * \ingroup group_filters
 * \param y A pointer on a 16-bit fixed-point vector of size elements corresponding to the output buffer.
 *          The den_size previous elements correspond to values of old output samples and have to be set to 0 at the initialization (y-1, y-2, ...).
 * \param x A pointer on a 16-bit fixed-point vector of size elements corresponding to the input buffer.
 *          The num_size previous elements correspond to old samples input samples have to be set to 0 at the initialization (x-1, x-2, ...).
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
 * \ingroup group_filters
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


/*! \addtogroup group_filters
 * \section Partial_IIR Partial Infinite Impulse Response Filter
 * This function computes a real IIR filter using the coefficients of the desire
 * filter onto a fixed-length signal. It returns a signal of a length equals to (size - num_size + 1) elements.\n
 * Here is the formula of the IIR filter:
 * \image html filters_iir_equation.gif
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
 * \ingroup group_filters
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
 * \ingroup group_filters
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



/*! \addtogroup group_filters
 * \section LMS Least Mean Square Filter
 * This function computes an adaptive LMS filter. It returns a (size)-length signal.\n
 * Here is the formula of the LMS filter:
 * \image html filters_lms_equations.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.\n
 *       You can change the value of the mu coefficient of the LMS filter by defying DSP_LMS_MU,
 *       at the compilation, with the desired value. This value defines mu as follow:
 * \image html filters_lms_coefficient.gif
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
 * \ingroup group_filters
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
 * \ingroup group_filters
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

/*! \addtogroup group_filters
 * \section NLMS Normalized Least Mean Square Filter
 * This function computes an adaptive NLMS filter. It returns a (size)-length signal.\n
 * Here is the formula of the NLMS filter:
 * \image html filters_nlms_equations.gif
 * \note The data have to be scaled to avoid overflowing values.\n
 *       All the vectors have to be 32-bit aligned.
 *       You can change the value of the mu coefficient of the NLMS filter by defying DSP_NLMS_MU,
 *       at the compilation, with the desired value. This value defines mu as follow:
 * \image html filters_nlms_coefficient.gif
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
 * \ingroup group_filters
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
 * \ingroup group_filters
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

/*! \addtogroup group_filters
 * \section Interpolation Interpolation filter
 * This function performs an interpolation over the input signal. It returns a (\b vect2_size * \b interpolation_ratio)-length signal.\n
 * Here is the principle of the interpolation:
 * \image html filters_interpolation.gif
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
 * \ingroup group_filters
 * \param fir_coefs A pointer on a 16-bit fixed-point vector where the coefficients
 *                  are stored.
 * \param n_tap The number of tap of this filter.
 * \param interpolation_ratio The interpolation factor of the interpolation to
 *                            perform with this filter.
 */
void dsp16_filt_interpolation_coefsort(dsp16_t *fir_coefs, int n_tap, int interpolation_ratio);
/*! \brief This function interpolates a vector.
 * \ingroup group_filters
 * \param vect1 A pointer on a 16-bit fixed-point vector where to store the result.
 *              It must be of a size (in sample) equals to the size of the input
 *              buffer multiplied by the interpolation factor.
 * \param vect2 A pointer on a 16-bit fixed-point vector containig the input samples.
 * \param vect2_size The size of the input buffer.
 * \param h A pointer on a 16-bit fixed-point vector which contains the coefficients
 *          of the filter. These coefficients must be reorder with the function
 *          \ref dsp16_filt_interpolation_coefsort before being used.
 * \param h_size The size of this buffer.
 * \param interpolation_ratio The interpolation factor desired for this interpolation.
 */
void dsp16_filt_interpolation(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio);


/*! \addtogroup group_filters
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
 * \ingroup group_filters
 * \param c A pointer on a 16-bit fixed-point vector of "order" size, used to store the coefficients of the filter designed.
 * \param fc Cutoff frequency of the low-pass filter.
 * \param fs Sample rate of the signal to filter.
 * \param order Order of the filter to design.
 */
void dsp16_filt_lpfirdesign(dsp16_t *c, int fc, int fs, int order);

/*! \brief 16-bit fixed point version of the windowed sinc low-pass FIR filter design.
 * \ingroup group_filters
 * \param c A pointer on a 16-bit fixed-point vector of "order" size, used to store the coefficients of the filter designed.
 * \param fc Cutoff frequency of the low-pass filter.
 * \param fs Sample rate of the signal to filter.
 * \param order Order of the filter to design.
 */
void dsp16_filt_lpfirdesign_windowed_sinc(dsp16_t *c, int fc_hz, int fs_hz, int order);

#endif // __AVR32_ABI_COMPILER__

#endif //__DSP_FILTERS_H__

