/*****************************************************************************
 *
 * \file
 *
 * \brief generic 16-bit AEC
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "dsp.h"

// Must be a multiple of 2 to ensure compiler optimizations
#define LOOP_UNROLL             6
#define LOOP_UNROLL_PLUS_ONE    7

// After calling the function "dsp16_resampling_link", the value defines the number
// of points to be interpolated. This ensure a clean link with the output buffers.
#define NB_PTS_TO_INTERPOLATE  3

// Default values for the filter
#define DEFAULT_FILTER_FUNCTION dsp16_filt_lpfirdesign_windowed_sinc
#define DEFAULT_WINDOW_FUNCTION dsp16_win_hann

#define DSP16_RESAMPLING_FUNCTION_NAME(x_num, data) \
  TPASTE2(dsp16_resampling_kernel_x, x_num),

#define DSP16_RESAMPLING_NO_LOOP_FUNCTION_NAME(x_num, data) \
  TPASTE2(dsp16_resampling_no_loop_kernel_x, x_num),

#define DSP16_RESAMPLING_FILTER(x_num, data) \
  sum += ph[(x_num+data)] * pvect2[-(x_num+data)];

static void dsp16_resampling_polynomial_interpolation(dsp16_t y[4], dsp16_t output_y[NB_PTS_TO_INTERPOLATE]);

static int dsp_op_gcd(int m, int n)
{
  int r;
  if (m < n)
  {
    int temp;
    temp = m;
    m = n;
    n = temp;
  }
  r = m - (m / n) * n;
  while(r != 0)
  {
     m = n;
     n = r;
     r = m - (m / n) * n;
  }
  return n;
}

dsp_resampling_t *dsp16_resampling_setup(int input_sample_rate, int output_sample_rate, int buffer_size, int filter_order, int nb_channels, malloc_fct_t malloc_fct, dsp16_resampling_options_t *options)
{
  dsp_resampling_t *dsp_resampling;
  int interpolation_factor, divisor, i;
  size_t memory_to_allocate_for_last_samples, memory_to_allocate_for_fir_coefs, memory_to_allocate_for_context;
  void (*dsp16_window)(dsp16_t *window, dsp16_t *input, int size);
  void (*dsp16_filter)(dsp16_t *coefficients, int fc_hz, int fs_hz, int order);

  // Makes sure the right options are selected
  if (options && options->coefficients_generation != DSP16_RESAMPLING_OPTIONS_USE_DYNAMIC &&
      options->coefficients_generation != DSP16_RESAMPLING_OPTIONS_USE_FIXED)
    return NULL;

  // Get the greater common divisor from the 2 frequencies to find a common denominator
  divisor = dsp_op_gcd(input_sample_rate, output_sample_rate);
  interpolation_factor = output_sample_rate / divisor;

  // Include few more samples to the input buffer to ensure good link with two consecutive buffers
  buffer_size += filter_order;

  // Check conditions
  // filter_order must be lower than buffer_size
  if (filter_order >= buffer_size && buffer_size > Max(2, NB_PTS_TO_INTERPOLATE))
    return NULL;
  if ((input_sample_rate * interpolation_factor) / output_sample_rate < 2 && input_sample_rate != output_sample_rate)
    return NULL;

  // Allocate enough memory
  memory_to_allocate_for_last_samples = sizeof(dsp16_t) * buffer_size * nb_channels + 4;          // Buffer to store previous samples
  if (options && options->coefficients_generation == DSP16_RESAMPLING_OPTIONS_USE_FIXED)
    memory_to_allocate_for_fir_coefs = 0;
  else
    memory_to_allocate_for_fir_coefs = sizeof(dsp16_t) * filter_order * interpolation_factor + 4; // FIR filter coefficients
  memory_to_allocate_for_context = sizeof(dsp_resampling_context_t) * nb_channels + 4;            // Resampling streaming information
  if (!(dsp_resampling = (dsp_resampling_t *) malloc_fct(memory_to_allocate_for_last_samples
                                                       + memory_to_allocate_for_fir_coefs
                                                       + memory_to_allocate_for_context
                                                       + sizeof(dsp_resampling_t))))
    return NULL;

  // Link memory sections
  // Link last samples buffer and align on 32 bits
  dsp_resampling->last_samples = (void *) &dsp_resampling[1];
  dsp_resampling->last_samples = (void *) Align_up((int) dsp_resampling->last_samples, 4);
  // Link fir coefficients buffer and align on 32 bits
  dsp_resampling->fir_coefs = (void *) ((int) dsp_resampling->last_samples + memory_to_allocate_for_last_samples);
  dsp_resampling->fir_coefs = (void *)  Align_up((int) dsp_resampling->fir_coefs, 4);
  // Link context and align on 32 bits
  dsp_resampling->context = (dsp_resampling_context_t *) ((int) dsp_resampling->fir_coefs + memory_to_allocate_for_fir_coefs);
  dsp_resampling->context = (dsp_resampling_context_t *) Align_up((int) dsp_resampling->context, 4);

  // Fill the structure
  dsp_resampling->fs = input_sample_rate;
  dsp_resampling->interpolation_factor = interpolation_factor;
  dsp_resampling->decimation_factor = input_sample_rate / divisor;
  dsp_resampling->buffer_size = buffer_size;
  dsp_resampling->nb_channels = nb_channels;
  dsp_resampling->filter_order = filter_order;
  dsp_resampling->link_required = false;
  for(i=0; i<nb_channels; i++)
  {
    dsp_resampling->context[i].input_buffer_pos = 0;
    dsp_resampling->context[i].index = 0;
  }

  // If we use a static table for the coefficients
  if (options && options->coefficients_generation == DSP16_RESAMPLING_OPTIONS_USE_FIXED)
  {
    dsp_resampling->fir_coefs = options->fixed.dsp16_coefficients;
  }
  // else we choose to use the dynamic coefficient generation method...
  else
  {
    int fc_hz;

    // Assign filter function to be used
    if (options && options->dynamic.dsp16_custom_filter)
      dsp16_filter = options->dynamic.dsp16_custom_filter;
    else
      dsp16_filter = DEFAULT_FILTER_FUNCTION;

    // Assign window function to be used
    if (options && options->dynamic.dsp16_custom_window)
      dsp16_window = options->dynamic.dsp16_custom_window;
    else
      dsp16_window = DEFAULT_WINDOW_FUNCTION;

    // Choose the cut-off frequency of the filter
    if (options && options->dynamic.custom_fc_hz)
      fc_hz = options->dynamic.custom_fc_hz;
    else
      fc_hz = Min(input_sample_rate, output_sample_rate) / 2;

    // Generate the filter
    dsp16_filter(dsp_resampling->fir_coefs,
      fc_hz,
      input_sample_rate * dsp_resampling->interpolation_factor,
      dsp_resampling->filter_order * dsp_resampling->interpolation_factor);

    // Add a window to the filter
    dsp16_window(dsp_resampling->fir_coefs,
      dsp_resampling->fir_coefs,
      dsp_resampling->filter_order * dsp_resampling->interpolation_factor);

    // Re-order the coefficient positions for enhancement (poly-phase filtering)
    dsp16_filt_interpolation_coefsort(
      dsp_resampling->fir_coefs,
      dsp_resampling->filter_order,
      dsp_resampling->interpolation_factor);

    // Filter normalization
    // It avoid some saturation problems
    if (!options || options->dynamic.coefficients_normalization)
    {
      S32 sum_max = 0;
      dsp16_t *p;
      p = (dsp16_t *) dsp_resampling->fir_coefs;
      for(i=0; i<dsp_resampling->filter_order * dsp_resampling->interpolation_factor; i += dsp_resampling->filter_order)
      {
        S32 sum = 0;
        int j;
        for(j=0; j<dsp_resampling->filter_order; j++)
          sum += (S32) dsp16_op_abs(*p++);
        if (sum > sum_max)
          sum_max = sum + 1; // +1 to avoid some rounding issues.
      }
      if (sum_max < (1 << DSP16_QB))
        dsp16_vect_realdiv(dsp_resampling->fir_coefs, dsp_resampling->fir_coefs, dsp_resampling->filter_order * dsp_resampling->interpolation_factor, sum_max);
      else
      {
        sum_max = ((1 << DSP16_QB) << DSP16_QB) / sum_max;
        dsp16_vect_realmul(dsp_resampling->fir_coefs, dsp_resampling->fir_coefs, dsp_resampling->filter_order * dsp_resampling->interpolation_factor, sum_max);
      }
    }
  }


  // Reset buffer
  dsp16_vect_zeropad(dsp_resampling->last_samples, buffer_size * nb_channels, buffer_size * nb_channels);

  return dsp_resampling;
}

#if LOOP_UNROLL > 4

#define DSP16_RESAMPLING_KERNEL_X_FCT(x_num, data)                              \
static int TPASTE2(dsp16_resampling_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int n_tap, int interpolation_ratio, int decimation_ratio, int *pcounter, int *pn) \
{                                                                               \
  S32 sum = 0;                                                                  \
  int i, k, n;                                                                  \
  dsp16_t *ph;                                                                  \
  dsp16_t *pvect1;                                                              \
  dsp16_t *pvect2;                                                              \
  int size = 0;                                                                 \
  int counter;                                                                  \
                                                                                \
  pvect1 = vect1;                                                               \
                                                                                \
  n = *pn;                                                                      \
  counter = *pcounter;                                                          \
  vect2_size -= n_tap;                                                          \
  while(n < vect2_size)                                                         \
  {                                                                             \
    for(k=counter; k<interpolation_ratio; k += decimation_ratio)                \
    {                                                                           \
      sum = 0;                                                                  \
      ph = &h[k*n_tap];                                                         \
      pvect2 = &vect2[n + n_tap];                                               \
      for(i=0; i<n_tap - LOOP_UNROLL + 1; i += LOOP_UNROLL)                     \
      {                                                                         \
        MREPEAT(LOOP_UNROLL, DSP16_RESAMPLING_FILTER, 0)                        \
        ph += LOOP_UNROLL;                                                      \
        pvect2 -= LOOP_UNROLL;                                                  \
      }                                                                         \
      MREPEAT(x_num, DSP16_RESAMPLING_FILTER, 0);                               \
      *pvect1++ = sum >> DSP16_QB;                                              \
      size++;                                                                   \
    }                                                                           \
    counter = k / interpolation_ratio;                                          \
    n += counter;                                                               \
    counter = k - counter * interpolation_ratio;                                \
  }                                                                             \
  *pcounter = counter;                                                          \
  *pn = n - vect2_size;                                                         \
                                                                                \
  return size;                                                                  \
}

#else // LOOP_UNROLL <= 4

#define DSP16_RESAMPLING_KERNEL_X_FCT(x_num, data)                              \
static int TPASTE2(dsp16_resampling_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int n_tap, int interpolation_ratio, int decimation_ratio, int *pcounter, int *pn) \
{                                                                               \
  S32 sum = 0;                                                                  \
  int i, k, n;                                                                  \
  dsp16_t *ph;                                                                  \
  dsp16_t *pvect1;                                                              \
  dsp16_t *pvect2;                                                              \
  int size = 0;                                                                 \
  int counter;                                                                  \
                                                                                \
  pvect1 = vect1;                                                               \
                                                                                \
  n = *pn;                                                                      \
  counter = *pcounter;                                                          \
  vect2_size -= n_tap;                                                          \
  while(n < vect2_size)                                                         \
  {                                                                             \
    pvect2 = &vect2[n + n_tap];                                                 \
    for(k=counter; k<interpolation_ratio; k += decimation_ratio)                \
    {                                                                           \
      sum = 0;                                                                  \
      ph = &h[k*n_tap];                                                         \
      for(i=0; i<n_tap - LOOP_UNROLL + 1; i += LOOP_UNROLL)                     \
      {                                                                         \
        MREPEAT(LOOP_UNROLL, DSP16_RESAMPLING_FILTER, i)                        \
      }                                                                         \
      MREPEAT(x_num, DSP16_RESAMPLING_FILTER, i);                               \
      *pvect1++ = sum >> DSP16_QB;                                              \
      size++;                                                                   \
    }                                                                           \
    counter = k / interpolation_ratio;                                          \
    n += counter;                                                               \
    counter = k - counter * interpolation_ratio;                                \
  }                                                                             \
  *pcounter = counter;                                                          \
  *pn = n - vect2_size;                                                         \
                                                                                \
  return size;                                                                  \
}

#endif // LOOP_UNROLL > 4

#define DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(x_num, data)                      \
static int TPASTE2(dsp16_resampling_no_loop_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int n_tap, int interpolation_ratio, int decimation_ratio, int *pcounter, int *pn) \
{                                                                               \
  S32 sum = 0;                                                                  \
  int k, n;                                                                     \
  dsp16_t *ph;                                                                  \
  dsp16_t *pvect1;                                                              \
  dsp16_t *pvect2;                                                              \
  int size = 0;                                                                 \
  int counter;                                                                  \
                                                                                \
  pvect1 = vect1;                                                               \
                                                                                \
  n = *pn;                                                                      \
  counter = *pcounter;                                                          \
  vect2_size -= n_tap;                                                          \
  while(n < vect2_size)                                                         \
  {                                                                             \
    pvect2 = &vect2[n + n_tap];                                                 \
    for(k=counter; k<interpolation_ratio; k += decimation_ratio)                \
    {                                                                           \
      sum = 0;                                                                  \
      ph = &h[k*n_tap];                                                         \
      MREPEAT(x_num, DSP16_RESAMPLING_FILTER, 0)                                \
      *pvect1++ = sum >> DSP16_QB;                                              \
      size++;                                                                   \
    }                                                                           \
    counter = k / interpolation_ratio;                                          \
    n += counter;                                                               \
    counter = k - counter * interpolation_ratio;                                \
  }                                                                             \
  *pcounter = counter;                                                          \
  *pn = n - vect2_size;                                                         \
                                                                                \
  return size;                                                                  \
}

static int dsp16_resampling_no_loop_kernel_x0(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int n_tap, int interpolation_ratio, int decimation_ratio, int *pcounter, int *pn)
{
  int k, n;
  dsp16_t *pvect1;
  dsp16_t *pvect2;
  int counter;
  int size = 0;

  pvect1 = vect1;

  n = *pn;
  counter = *pcounter;
  while(n < vect2_size)
  {
    pvect2 = &vect2[n];
    for(k=counter; k<interpolation_ratio; k += decimation_ratio)
    {
      *pvect1++ = *pvect2;
      size++;
    }
    counter = k / interpolation_ratio;
    n += counter;
    counter = k - counter * interpolation_ratio;
  }
  *pcounter = counter;
  *pn = n - vect2_size;

  return size;
}
DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(1, )
DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(2, )
DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(3, )
DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(4, )
DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(5, )
DSP16_RESAMPLING_NO_LOOP_KERNEL_X_FCT(6, )

DSP16_RESAMPLING_KERNEL_X_FCT(0, )
DSP16_RESAMPLING_KERNEL_X_FCT(1, )
DSP16_RESAMPLING_KERNEL_X_FCT(2, )
DSP16_RESAMPLING_KERNEL_X_FCT(3, )
DSP16_RESAMPLING_KERNEL_X_FCT(4, )
DSP16_RESAMPLING_KERNEL_X_FCT(5, )

static void dsp16_resampling_polynomial_interpolation(dsp16_t y[4], dsp16_t output_y[NB_PTS_TO_INTERPOLATE])
{
  dsp32_t f_x0_x1, f_x1_x2, f_x2_x3;
  dsp32_t f_x0_x1_x2, f_x1_x2_x3;
  dsp32_t f_x0_x1_x2_x3;
  int i;
  const int x0 = 0;
  const int x1 = 1;
  const int x2 = NB_PTS_TO_INTERPOLATE + 1;
  const int x3 = NB_PTS_TO_INTERPOLATE + 2;

  // Compute the interpolation coefficients using the Newton method
  f_x0_x1 = ((dsp32_t) y[1] - y[0]) / (x1 - x0);
  f_x1_x2 = ((dsp32_t) y[2] - y[1]) / (x2 - x1);
  f_x2_x3 = ((dsp32_t) y[3] - y[2]) / (x3 - x2);

  f_x0_x1_x2 = (f_x1_x2 - f_x0_x1) / (x2 - x0);
  f_x1_x2_x3 = (f_x2_x3 - f_x1_x2) / (x3 - x1);

  f_x0_x1_x2_x3 = (f_x1_x2_x3 - f_x0_x1_x2) / (x3 - x0);

  // The polynomialial equals:
  // P = x0 + f_x0_x1 * e1(x) + f_x0_x1_x2 * e2(x) + f_x0_x1_x2_x3 * e3(x)
  // Where:
  // e1(x) = (x - x0)
  // e2(x) = (x - x0) * (x - x1)
  // e3(x) = (x - x0) * (x - x1) * (x - x2)

  for (i=0; i<NB_PTS_TO_INTERPOLATE; i++)
  {
    int e1, e2, e3;
    int x = i + 2;
    dsp32_t temp_y;

    e1 = (x - x0);
    e2 = e1 * (x - x1);
    e3 = e2 * (x - x2);

    temp_y = y[0] + f_x0_x1 * e1 + f_x0_x1_x2 * e2 + f_x0_x1_x2_x3 * e3;
    output_y[i] = (dsp16_t) temp_y;
  }
}

void dsp16_resampling_compute(dsp_resampling_t *dsp_resampling, dsp16_t *output, dsp16_t *input, int channel)
{
  int n_tap;
  dsp16_t *input_extended;
  dsp16_t link_y[4];
  typedef int (*resampling_kernel_opti_t)(dsp16_t *, dsp16_t *, int, dsp16_t *, int, int, int, int *, int *);
  static const resampling_kernel_opti_t resampling_end_kernel_opti[] = {
    MREPEAT(LOOP_UNROLL, DSP16_RESAMPLING_FUNCTION_NAME, )
  };
  static const resampling_kernel_opti_t resampling_no_loop_end_kernel_opti[] = {
    MREPEAT(LOOP_UNROLL_PLUS_ONE, DSP16_RESAMPLING_NO_LOOP_FUNCTION_NAME, )
  };

  n_tap = dsp_resampling->filter_order;
  input_extended = ((dsp16_t *) dsp_resampling->last_samples) + dsp_resampling->buffer_size * channel;

  // If a clean link is required between this buffer and the previous one, then save the previous output samples.
  if (dsp_resampling->link_required)
    dsp16_vect_copy(link_y, &input_extended[n_tap], 2);

  // Add to the input buffer previous samples
  dsp16_vect_copy(&input_extended[n_tap], input, dsp_resampling->buffer_size - n_tap);

  // Special case, the input frequency is equal to the output frequency
  if (dsp_resampling->interpolation_factor == dsp_resampling->decimation_factor)
  {
    int i;
    dsp16_t *data;
    dsp16_t weight_x1, weight_x2;

    data = &input_extended[(n_tap + 1)/2];
    weight_x1 = dsp_resampling->interpolation_factor - dsp_resampling->context[channel].input_buffer_pos;
    weight_x2 = dsp_resampling->context[channel].input_buffer_pos;

    dsp_resampling->current_buffer_size = dsp_resampling->buffer_size - n_tap;

    for (i=0; i<dsp_resampling->current_buffer_size; i++)
      output[i] = ((dsp32_t) (data[i]*weight_x1) + (data[i+1]*weight_x2)) / dsp_resampling->interpolation_factor;
  }
  else if (n_tap <= LOOP_UNROLL)
  {
    dsp_resampling->current_buffer_size = resampling_no_loop_end_kernel_opti[n_tap](
      output,
      input_extended,
      dsp_resampling->buffer_size,
      (dsp16_t *) dsp_resampling->fir_coefs,
      dsp_resampling->filter_order,
      dsp_resampling->interpolation_factor,
      dsp_resampling->decimation_factor,
      &dsp_resampling->context[channel].input_buffer_pos,
      &dsp_resampling->context[channel].index);
  }
  else
  {
    dsp_resampling->current_buffer_size = resampling_end_kernel_opti[n_tap%LOOP_UNROLL](
      output,
      input_extended,
      dsp_resampling->buffer_size,
      (dsp16_t *) dsp_resampling->fir_coefs,
      dsp_resampling->filter_order,
      dsp_resampling->interpolation_factor,
      dsp_resampling->decimation_factor,
      &dsp_resampling->context[channel].input_buffer_pos,
      &dsp_resampling->context[channel].index);
  }

  // Compute a polynomial interpolation to ensure a clean link between the 2 buffers
  if (dsp_resampling->link_required)
  {
    link_y[2] = output[NB_PTS_TO_INTERPOLATE + 0];
    link_y[3] = output[NB_PTS_TO_INTERPOLATE + 1];
    dsp16_resampling_polynomial_interpolation(link_y, output);
    // Remove the flag
    dsp_resampling->link_required = false;
  }

  // Store a few of last samples
  dsp16_vect_copy(input_extended, &input_extended[dsp_resampling->buffer_size - n_tap], n_tap);
  // Copy the last 2 output samples to ensure a clean link between 2 buffers after using the dsp16_resampling_link function
  dsp16_vect_copy(&input_extended[n_tap], &output[dsp_resampling->current_buffer_size - 2], 2);
}

bool dsp16_resampling_link(dsp_resampling_t *dsp_resampling_output, dsp_resampling_t *dsp_resampling_input)
{
  int channel;
  int n_tap_input, n_tap_output;

  // Calculate the number of taps of each structure to make sure they are matching
  n_tap_input = dsp_resampling_input->filter_order;
  n_tap_output = dsp_resampling_output->filter_order;

  // Makes sure the two structure are compatible
  if (n_tap_input != n_tap_output
   || dsp_resampling_output->buffer_size != dsp_resampling_input->buffer_size
   || dsp_resampling_output->nb_channels != dsp_resampling_input->nb_channels)
    return false;

  // Handle special case - the input frequency equals the output frequency
  if (dsp_resampling_output->interpolation_factor == dsp_resampling_output->decimation_factor)
  {
    dsp_resampling_output->interpolation_factor = dsp_resampling_input->interpolation_factor;
    dsp_resampling_output->decimation_factor = dsp_resampling_input->interpolation_factor;
  }

  // Copy the last samples from a structure to the other
  for (channel = 0; channel < dsp_resampling_output->nb_channels; channel++)
  {
    dsp16_t *input_extended_input = ((dsp16_t *) dsp_resampling_input->last_samples) + dsp_resampling_input->buffer_size * channel;
    dsp16_t *input_extended_output = ((dsp16_t *) dsp_resampling_output->last_samples) + dsp_resampling_output->buffer_size * channel;
    // "+ 2" to copy also the last previous samples of the resampling output
    dsp16_vect_copy(input_extended_output, input_extended_input, n_tap_input + 2);

    // Convert the context from a structure to the other
    dsp_resampling_output->context[channel].index = dsp_resampling_input->context[channel].index;
    dsp_resampling_output->context[channel].input_buffer_pos = (dsp_resampling_input->context[channel].input_buffer_pos
                                                              * dsp_resampling_output->interpolation_factor)
                                                              / dsp_resampling_input->interpolation_factor;
  }

  // This is to make sure the link between the two buffers is clean.
  dsp_resampling_output->link_required = true;

  return true;
}

int dsp16_resampling_get_output_current_buffer_size(dsp_resampling_t *dsp_resampling)
{
  return dsp_resampling->current_buffer_size;
}

int dsp16_resampling_get_output_max_buffer_size(dsp_resampling_t *dsp_resampling)
{
  return (dsp_resampling->buffer_size * dsp_resampling->interpolation_factor - dsp_resampling->filter_order * dsp_resampling->interpolation_factor) / dsp_resampling->decimation_factor + 1;
}

void dsp16_resampling_free(dsp_resampling_t *dsp_resampling, free_fct_t free_fct)
{
  free_fct(dsp_resampling);
}
