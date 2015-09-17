/*****************************************************************************
 *
 * \file
 *
 * \brief List of currently implemented signal generation functions
 *
 * This file show a list of the signal generation functions currently implemented in the DSP library.
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

/*! \defgroup group_signal_generation DSPLIB - Signal generation
 *  \ingroup group_functions
 *  All the signal generation functions currently supported by the DSP library.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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

