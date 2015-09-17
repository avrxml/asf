/**
 * \file
 *
 * \mainpage
 * \section title MFCC Sphinx-4 implementation.
 *
 * \section file File(s)
 * - \ref mfcc_example.c
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "cycle_counter.h"
#include "power_clocks_lib.h"

#include "dsp.h"
#include "conf_mfcc.h"

//! Input frame size in number of samples
#define INPUT_FRAME_NB_SAMPLES ((MFCC_INPUT_FS * MFCC_FRAME_SIZE_MS) / 1000)
//! frame shift in number of samples
#define INPUT_FRAME_SHIFT_NB_SAMPLES ((MFCC_INPUT_FS * MFCC_FRAME_SHIFT_MS) / 1000)

// To store the input signal
A_ALIGNED dsp16_t input_frame[INPUT_FRAME_NB_SAMPLES];
// For the Hamming window
A_ALIGNED dsp16_t hamming_window[INPUT_FRAME_NB_SAMPLES];
// To store mel-scale coefficients
int pt_mel[MEL_FILTER_BANK_NB_CHANNELS + 2];
// For temporary data
A_ALIGNED dsp16_t temp_buffer[1024];

// ---------------- Input signal + Frame blocking ------------------------------

dsp16_t *generate_input_signal()
{
  static dsp16_t phases[GEN_INPUT_NB_SINES] = { DSP16_Q(0.1548) };
  A_ALIGNED dsp16_t temp_sine[INPUT_FRAME_SHIFT_NB_SAMPLES];
  int i;
  dsp16_t max;

  // Shift the signal by MFCC_FRAME_SHIFT_MS ms
  for (i=0; i<(INPUT_FRAME_NB_SAMPLES - INPUT_FRAME_SHIFT_NB_SAMPLES); i++)
    input_frame[i] = input_frame[i + INPUT_FRAME_SHIFT_NB_SAMPLES];

  // Fill the new signal slot with zeros
  for (; i<INPUT_FRAME_NB_SAMPLES; i++)
    input_frame[i] = DSP16_Q(0.);

  // The input signal is mix of different sine waves
  for (i=0; i<GEN_INPUT_NB_SINES; i++)
  {
    // Generate the sine wave
    phases[i] = dsp16_gen_sin(temp_sine,
      INPUT_FRAME_SHIFT_NB_SAMPLES,
      (i + 1) * (MFCC_INPUT_FS / 2) / GEN_INPUT_NB_SINES, //!< To generate frequency in the pass band.
      MFCC_INPUT_FS,
      phases[i]
    );
    // Scale the signal to prevent overflows
    dsp16_vect_realmul(temp_sine, temp_sine, INPUT_FRAME_SHIFT_NB_SAMPLES, DSP16_Q(1.) / GEN_INPUT_NB_SINES);
    // Add this signal to the main stream
    dsp16_vect_add(&input_frame[INPUT_FRAME_NB_SAMPLES - INPUT_FRAME_SHIFT_NB_SAMPLES],
      &input_frame[INPUT_FRAME_NB_SAMPLES - INPUT_FRAME_SHIFT_NB_SAMPLES],
      temp_sine,
      INPUT_FRAME_SHIFT_NB_SAMPLES);
  }

  // Normalize the signal to have the maximum excursion
  max = dsp16_vect_max(&input_frame[INPUT_FRAME_NB_SAMPLES - INPUT_FRAME_SHIFT_NB_SAMPLES],
    INPUT_FRAME_SHIFT_NB_SAMPLES);
  dsp16_vect_realdiv(&input_frame[INPUT_FRAME_NB_SAMPLES - INPUT_FRAME_SHIFT_NB_SAMPLES],
    &input_frame[INPUT_FRAME_NB_SAMPLES - INPUT_FRAME_SHIFT_NB_SAMPLES],
    INPUT_FRAME_SHIFT_NB_SAMPLES,
    max);

  // Output buffer
  return input_frame;
}

// ---------------- Pre-emphasis -----------------------------------------------

dsp16_t *mfcc_pre_emphasis(dsp16_t *buffer, int size)
{
  int i;
  static dsp16_t last_sample = DSP16_Q(0.);
  // Apply a high-pass filter to the signal
  //   s2(n) = s(n) - a*s(n-1)
  // Unfortunately the FIR filter form the DSPLib cannot be used in this example
  // because it needs at least a 8-tap filter coefficients.
  // That's not much a problem since the compiler will optimize quite well the
  // for loop if we use only 1-tap FIR filter anyway.
  // Note: the output is divided by 2 to avoid overflows
  buffer[0] = (((S32) buffer[0] - dsp16_op_mul(DSP16_Q(PRE_EMPHASIS_ALPHA), last_sample)) >> 1);
  for (i=1; i<size; i++)
    buffer[i] = (((S32) buffer[i] - dsp16_op_mul(DSP16_Q(PRE_EMPHASIS_ALPHA), buffer[i - 1])) >> 1);
  last_sample = buffer[i];

  // Output buffer
  return buffer;
}

// ---------------- Hamming window ---------------------------------------------

dsp16_t *mfcc_hamming_window(dsp16_t *buffer, int size)
{
  dsp16_vect_dotmul(buffer, buffer, hamming_window, size);
  // Output buffer
  return buffer;
}

// ---------------- FFT --------------------------------------------------------

dsp16_t *mfcc_fft_512pts(dsp16_t *buffer, int size)
{
  int i, mul_norm;
#if defined(FFT_NORMALIZE) && FFT_NORMALIZE == true
  dsp16_t max;
#endif
  // Use static to not overload the stack
  A_ALIGNED static dsp16_complex_t cplx_fft_buffer[1024];

  // The FFT 4-radix is implemented in the DSPLib which gives good speed results.
  // The drawback is that only 256, 1024, 4096... samples can be computed a time.
  // A workaround is then to zero-pad the input signal with zeros.
  dsp16_vect_zeropad(buffer, 1024, (1024 - size));
  // 1024-point FFT
  dsp16_trans_realcomplexfft(cplx_fft_buffer, buffer, 10);
  // Zero-padding in the time domain corresponds to interpolation in the frequency
  // domain. It does not add any new information to the signal.
  // The result is even more accurate than doing a 512 point FFT.
  // Now we need to get the absolute value of the signal to convert the complex input
  // into a real output.
  // Get the square of the magnitude and scale the output to 512 points
  // We will also normalize the buffer to avoid sample loss

#if defined(FFT_NORMALIZE) && FFT_NORMALIZE == true
  // For a normalized value
  max = dsp16_vect_max((dsp16_t *) cplx_fft_buffer, 512 * 2);
  mul_norm = (DSP16_Q(1.) / max) / 4;
#else
  // For a fixed gain
  mul_norm = FFT_INTEGER_GAIN;
#endif
  // Compute only the first half of the samples, the other half is not needed for
  // the rest of the algorithm
  for (i=0; i<512; i+=2)
  {
    S32 real, imag;
    real = ((S32) cplx_fft_buffer[i].real + cplx_fft_buffer[i + 1].real) * mul_norm;
    imag = ((S32) cplx_fft_buffer[i].imag + cplx_fft_buffer[i + 1].imag) * mul_norm;
    buffer[i >> 1] = dsp16_op_mul(real, real) + dsp16_op_mul(imag, imag);
  }

  // Output buffer
  return buffer;
}

// ---------------- Mel-Scale Filter Bank --------------------------------------

// This function converts a linear frequency to a mel frequency
// m = 2595 * log10(f / 700 + 1)
S32 mfcc_lin_to_mel(S32 f_hz)
{
  // Use this undocumented function from the DSPLib to ease and speed up the process
  extern S32 dsp16_op_ln_raw(dsp16_t num);
  const S32 cst_ln2 = (S32) DSP_Q(32 - DSP16_QB, DSP16_QB, 1. / CST_LN_10);
  S32 f_mel;

  // Calculate the inside of the log10 - this number is always greater than 1.
  f_mel = DSP16_Q(1.) + (f_hz << DSP16_QB) / 700;
  // Use the relation: log10(a / b) = log10(a) - log10(b) to calculate the log10, with a = 1.
  f_mel = dsp16_op_ln_raw((DSP16_Q(1.) << DSP16_QB) / f_mel);
  f_mel = (f_mel * cst_ln2) >> DSP16_QB;
  f_mel = -f_mel;
  // Return the mel frequency
  return (S32) ((2595 * f_mel) >> DSP16_QB);
}

// This function converts a mel frequency to a linear frequency
// f = 700 * (10 ^ (m / 2595) - 1)
S32 mfcc_mel_to_lin(S32 f_mel_hz)
{
  const int dsp32_qb = 18;
  // This constant equals to 10 ^ (1 / 2595) - 1
  S32 cst_mel = (S32) DSP_Q(32 - dsp32_qb, dsp32_qb, 0.0008877);
  S32 f_hz;

  f_hz = (S32) DSP_Q(32 - dsp32_qb, dsp32_qb, 1.);
  while (f_mel_hz--)
    f_hz = ((f_hz * cst_mel) >> dsp32_qb) + f_hz;
  f_hz -= DSP_Q(32 - dsp32_qb, dsp32_qb, 1.);
  // Convert this number to a dsp16_t type.
  f_hz >>= (dsp32_qb - DSP16_QB);

  return ((700 * f_hz) >> DSP16_QB);
}

void mfcc_mel_scale_filter_bank_init()
{
  int i;
  S32 f_high_mel_hz;

  // Compute the high frequency needed and convert it to mel frequency.
  f_high_mel_hz = mfcc_lin_to_mel(MFCC_INPUT_FS / 2);

  // Calculate the mel frequencies
  // From 0 -> FS / 2
  // Set the boundaries
  pt_mel[0] = 0;
  pt_mel[MEL_FILTER_BANK_NB_CHANNELS + 1] = 512 / 2;
  for (i=1; i<(MEL_FILTER_BANK_NB_CHANNELS + 1); i++)
  {
    S32 f_mel_hz;
    // The relation is (taken from matlab code):
    // f(i) = conv_mel_to_lin(i * conv_lin_to_mel(fs / 2) / (filter_number + 1));
    f_mel_hz = i * f_high_mel_hz / (MEL_FILTER_BANK_NB_CHANNELS + 1);
    // Calculate the mel frequency
    f_mel_hz = mfcc_mel_to_lin(f_mel_hz);
    // We now need to convert these frequencies into points
    // MFCC_INPUT_FS / 2 == 512 / 2
    pt_mel[i] = f_mel_hz * (512 / 2) / (MFCC_INPUT_FS / 2);
  }
}
// The result of this function is a dsp16_t type coded on 32-bits.
// The MSBs are defining the integer part of the number.
S32 *mfcc_mel_scale_filter_bank(dsp16_t *buffer, int size)
{
  int i;
  S32 *mel_power;

  // Set a pointer where to store the power of the signal
  mel_power = (S32 *) &buffer[256];

  // Process the filters
  for (i=1; i<(MEL_FILTER_BANK_NB_CHANNELS + 1); i++)
  {
    int j;
    int pt_start = pt_mel[i - 1];
    int length = pt_mel[i] - pt_mel[i - 1];
    dsp16_t *buffer_start;
    dsp16_t inc;
    S32 power = 0;

    buffer_start = &buffer[pt_start];

    // Rising edge of the triangle
    inc = DSP16_Q(1.) / (pt_mel[i] - pt_mel[i - 1]);
    for (j=1; j<=length; j++)
      power += dsp16_op_mul(j * inc, *buffer_start++);

    // Falling edge of the triangle
    inc = DSP16_Q(1.) / (pt_mel[i + 1] - pt_mel[i]);
    for (j=length; j>0; j--)
      power += dsp16_op_mul(j * inc, *buffer_start++);

    mel_power[i - 1] = power;
  }

  return mel_power;
}

// ---------------- Log --------------------------------------------------------

S32 *mfcc_log(S32 *buffer, int size)
{
  // Use this undocumented function from the DSPLib to ease and speed up the process
  extern S32 dsp16_op_ln_raw(dsp16_t num);
  int i;

  for (i=0; i<size; i++)
    buffer[i] = dsp16_op_ln_raw((dsp16_t) buffer[i]);

  return buffer;
}

// ---------------- DCT 13 points ----------------------------------------------

S32 *mfcc_dct_13_pts(S32 *buffer, int size)
{
  S32 *dct_result;
  int k, n;
  dsp16_t cst;

  dct_result = &buffer[size];

  // cst = PI / size - because cos(PI) = dsp16_op_cos(1.)
  cst = (dsp16_t) (DSP16_Q(1.) / size);

  // Use the DCT-II formula:
  // DCT(k) = SUM(n=0, N-1, buffer[n] * cos(pi / N * (n + 1 / 2) * k))
  for (k=0; k<13; k++)
  {
    S32 sum = 0;
    for (n=0; n<size; n++)
    {
      dsp16_t temp;
      // ok because the result is modulo 1. -> that's how the cosinus function works.
      temp = (dsp16_t) ((S32) cst * ((n + 1 / 2) * k));
      // buffer[n] is never lower than -16 because log(1/2^15) = -10.xxx
      // So it is ok to shift buffer[n] by 4 to avoid overflows when we do the
      // multiplication
      sum += (((buffer[n] >> 4) * dsp16_op_cos(temp)) >> (DSP16_QB - 4));
    }
    dct_result[k] = sum;
  }

  return dct_result;
}

// ---------------- Numerical Differentiation ----------------------------------

S32 *mfcc_numerical_diff_13(S32 *cepstrum)
{
  int i;
  S32 *first_derivative;
  S32 *second_derivative;
  // cepstrum = current + 3
  // current_cepstrum[0] = current + 2
  // current_cepstrum[1] = current + 1
  // current_cepstrum[2] = current
  // current_cepstrum[3] = current - 1
  // current_cepstrum[4] = current - 2
  // current_cepstrum[5] = current - 3
  static S32 current_cepstrum[6][13] = { { 0 } };

  // Map the pointers
  first_derivative = &cepstrum[13];
  second_derivative = &first_derivative[13];

  // first_derivative = current_cepstrum[0] - current_cepstrum[4]
  for (i=0; i<13; i++)
    first_derivative[i] = current_cepstrum[0][i] - current_cepstrum[4][i];

  // second_derivative = (cepstrum - current_cepstrum[3])
  //   - (current_cepstrum[1] - current_cepstrum[5])
  for (i=0; i<13; i++)
    second_derivative[i] = (cepstrum[i] - current_cepstrum[3][i])
      - (current_cepstrum[1][i] - current_cepstrum[5][i]);

  // Shift the current_cepstrum array
  for (i=5; i>0; i--)
    memcpy(current_cepstrum[i], current_cepstrum[i - 1], 13 * sizeof(S32));
  // Copy the new cepstrum to become "current + 2"
  memcpy(current_cepstrum[i], cepstrum, 13 * sizeof(S32));

  return cepstrum;
}

// ---------------- Main process -----------------------------------------------

//! This function should be called every MFCC_FRAME_SHIFT_MS ms
void mfcc_process()
{
  void *working_buffer;
  static volatile U32 cycle_count, cc_pre_emphasis, cc_hamming, cc_fft, cc_mel, cc_log, cc_dct, cc_diff;

  // Hack to generate a virtual input signal
  // It should be mapped on the ADC or another external input source
  generate_input_signal();

  // To measure the actual cycle count of each iteration
  cycle_count = Get_sys_count();

  // Copy this signal to a working buffer
  memcpy(temp_buffer, input_frame, INPUT_FRAME_NB_SAMPLES * sizeof(dsp16_t));
  // Pre-emphasis
  cc_pre_emphasis = Get_sys_count();
  working_buffer = mfcc_pre_emphasis(temp_buffer, INPUT_FRAME_NB_SAMPLES);
  cc_pre_emphasis = Get_sys_count() - cc_pre_emphasis;
  // Hamming window
  cc_hamming = Get_sys_count();
  working_buffer = mfcc_hamming_window(working_buffer, INPUT_FRAME_NB_SAMPLES);
  cc_hamming = Get_sys_count() - cc_hamming;
  // Fast Fourier Transform -> 512 points output
  cc_fft = Get_sys_count();
  working_buffer = mfcc_fft_512pts(working_buffer, INPUT_FRAME_NB_SAMPLES);
  cc_fft = Get_sys_count() - cc_fft;
  // Mel-Scale filter bank
  cc_mel = Get_sys_count();
  working_buffer = mfcc_mel_scale_filter_bank(working_buffer, 512);
  cc_mel = Get_sys_count() - cc_mel;
  // Log compression
  cc_log = Get_sys_count();
  working_buffer = mfcc_log(working_buffer, MEL_FILTER_BANK_NB_CHANNELS);
  cc_log = Get_sys_count() - cc_log;
  // DCT of the 13 first points
  cc_dct = Get_sys_count();
  working_buffer = mfcc_dct_13_pts(working_buffer, MEL_FILTER_BANK_NB_CHANNELS);
  cc_dct = Get_sys_count() - cc_dct;
  // Numerical Differentiation
  cc_diff = Get_sys_count();
  working_buffer = mfcc_numerical_diff_13(working_buffer);
  cc_diff = Get_sys_count() - cc_diff;

  // Total cycle count of the operation
  cycle_count = Get_sys_count() - cycle_count;
  while(1);
}

// ---------------- Initialization ---------------------------------------------

// This function initializes all data related to the MFCC process
void mfcc_init()
{
  int i;

  // Fill the working buffer with zeros
  dsp16_vect_zeropad(temp_buffer,
    sizeof(temp_buffer)/sizeof(temp_buffer[0]),
    sizeof(temp_buffer)/sizeof(temp_buffer[0]));
  // Fill the input buffer with zeros
  dsp16_vect_zeropad(input_frame,
    sizeof(input_frame)/sizeof(input_frame[0]),
    sizeof(input_frame)/sizeof(input_frame[0]));
  // Generate the hamming window
  for (i=0; i<INPUT_FRAME_NB_SAMPLES; i++)
    hamming_window[i] = DSP16_Q(1.); //< Fills the input buffer with ones in order to get this signal multiplied by the hamming window
  dsp16_win_hamm(hamming_window, hamming_window, INPUT_FRAME_NB_SAMPLES);
  // Generate the mel-scale filter bank coefficients
  mfcc_mel_scale_filter_bank_init();
  // Run several times the generate_input_signal function in order to fill the input buffer
  generate_input_signal();
  generate_input_signal();
  generate_input_signal();
}

int main(void)
{
  int i;
  /* avoid Cppcheck Warning */
  UNUSED(i);
  scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
  pcl_freq_param_t pcl_dfll_freq_param = {
    .main_clk_src = PCL_MC_DFLL0,
    .cpu_f = 48000000,
    .pba_f = 48000000,
    .pbb_f = 48000000,
    .dfll_f = 96000000,
    .pextra_params = &gc_dfllif_ref_opt
  };

  // Set the CPU to 48 MHz
  pcl_configure_clocks(&pcl_dfll_freq_param);
  // Configure the Hmatrix
#if (defined AVR32_HMATRIX)
  for (i=0; i<AVR32_HMATRIX_SLAVE_NUM; i++)
    AVR32_HMATRIX.SCFG[i].defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
#endif
#if (defined AVR32_HMATRIXB)
  for (i=0; i<AVR32_HMATRIXB_SLAVE_NUM; i++)
    AVR32_HMATRIXB.SCFG[i].defmstr_type = AVR32_HMATRIXB_DEFMSTR_TYPE_LAST_DEFAULT;
#endif

  mfcc_init();
  mfcc_process();
  while(1);
}
