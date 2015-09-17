/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented transform functions
 *
 * This file show a list of the transform functions currently implemented in the DSP library.
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

/*! \defgroup group_transforms DSPLIB - Transforms
 *  \ingroup group_functions
 *  All the transforms functions currently supported by the DSP library.
 */
/* - \ref Complex_FFT
 * - \ref Complex_iFFT
 * - \ref Real_Complex_FFT
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
