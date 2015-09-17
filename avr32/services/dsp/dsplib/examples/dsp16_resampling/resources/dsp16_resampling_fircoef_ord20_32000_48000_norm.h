/*****************************************************************************
 *
 * \file
 *
 * \brief FIR coefficients for the dsp16_resampling function from the DSPLib.
 *        These coefficients have the following characterizations:
 *          - Input sampling frequency: 32000 Hz
 *          - Output sampling frequency: 48000 Hz
 *          - Polyphase FIR filter order: 20
 *          - Coefficients are normalized
 *          - Memory foot print: 120 bytes
 *         It matches the following parameters:
 *         (dsp16_resampling_options_t::dsp16_custom_filter)
 *          - Cut-off frequency (fc_hz): 16000
 *          - Sampling frequency (fs_hz): 96000
 *          - Actual FIR filter order (order): 60
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

A_ALIGNED static const dsp16_t dsp16_resampling_fircoef_ord20_32000_48000_norm[] = {
  DSP16_Q(-0.00000000),
  DSP16_Q(0.00020913),
  DSP16_Q(-0.00091944),
  DSP16_Q(0.00227166),
  DSP16_Q(-0.00445154),
  DSP16_Q(0.00775413),
  DSP16_Q(-0.01275414),
  DSP16_Q(0.02084121),
  DSP16_Q(-0.03650608),
  DSP16_Q(0.08607207),
  DSP16_Q(0.43776711),
  DSP16_Q(-0.06043393),
  DSP16_Q(0.02982017),
  DSP16_Q(-0.01764324),
  DSP16_Q(0.01084090),
  DSP16_Q(-0.00650202),
  DSP16_Q(0.00361814),
  DSP16_Q(-0.00173952),
  DSP16_Q(0.00061888),
  DSP16_Q(-0.00008999),
  DSP16_Q(-0.00004354),
  DSP16_Q(0.00076763),
  DSP16_Q(-0.00258199),
  DSP16_Q(0.00578992),
  DSP16_Q(-0.01081536),
  DSP16_Q(0.01837858),
  DSP16_Q(-0.02998343),
  DSP16_Q(0.04957962),
  DSP16_Q(-0.09186740),
  DSP16_Q(0.29019255),
  DSP16_Q(0.29019255),
  DSP16_Q(-0.09186740),
  DSP16_Q(0.04957962),
  DSP16_Q(-0.02998343),
  DSP16_Q(0.01837858),
  DSP16_Q(-0.01081536),
  DSP16_Q(0.00578992),
  DSP16_Q(-0.00258199),
  DSP16_Q(0.00076763),
  DSP16_Q(-0.00004354),
  DSP16_Q(-0.00008999),
  DSP16_Q(0.00061888),
  DSP16_Q(-0.00173952),
  DSP16_Q(0.00361814),
  DSP16_Q(-0.00650202),
  DSP16_Q(0.01084090),
  DSP16_Q(-0.01764324),
  DSP16_Q(0.02982017),
  DSP16_Q(-0.06043393),
  DSP16_Q(0.43776711),
  DSP16_Q(0.08607207),
  DSP16_Q(-0.03650608),
  DSP16_Q(0.02084121),
  DSP16_Q(-0.01275414),
  DSP16_Q(0.00775413),
  DSP16_Q(-0.00445154),
  DSP16_Q(0.00227166),
  DSP16_Q(-0.00091944),
  DSP16_Q(0.00020913),
  DSP16_Q(-0.00000000)
};
