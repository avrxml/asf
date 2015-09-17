/*****************************************************************************
 *
 * \file
 *
 * \brief This module provides a useful set of signal re-sampling functions.
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

/*! \defgroup group_resampling DSPLIB - Signal re-sampling
 *  \ingroup group_advanced
 *  All the signal re-sampling functions implemented in the DSP advanced library.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
/*! \brief Function used to free the previously allocated structure issued by the
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
