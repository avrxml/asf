/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented windowing functions
 *
 * This file show a list of the windowing functions currently implemented in the DSP library.
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

/*! \defgroup group_windowing DSPLIB - Windowing
 *  \ingroup group_functions
 *  All the windowing functions currently supported by the DSP library.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
 * These functions generate a Hann window that fits in the output buffer.\n
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

/*! \brief 16-bit fixed point version of the Hann windowing function.
 * \ingroup group_windowing
 * \param vect1 A pointer on a 16-bit real vector that will contain the window
 * multiplied by \b vect2.
 * \param vect2 A pointer on a 16-bit real vector that contains the signal on
 * which the window should apply.
 * \param size The size of the output buffer.
 */
void dsp16_win_hann(dsp16_t *vect1, dsp16_t *vect2, int size);
/*! \brief 32-bit fixed point version of the Hann windowing function.
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
