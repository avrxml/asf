/*****************************************************************************
 *
 * \file
 *
 * \brief FIR coefficients for the dsp16_resampling function from the DSPLib.
 *        These coefficients have the following characterizations:
 *          - Input sampling frequency: 32000 Hz
 *          - Output sampling frequency: 48000 Hz
 *          - Polyphase FIR filter order: 60
 *          - Coefficients are normalized
 *          - Memory foot print: 360 bytes
 *         It matches the following parameters:
 *         (dsp16_resampling_options_t::dsp16_custom_filter)
 *          - Cut-off frequency (fc_hz): 16000
 *          - Sampling frequency (fs_hz): 96000
 *          - Actual FIR filter order (order): 180
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

A_ALIGNED static const dsp16_t dsp16_resampling_fircoef_ord60_32000_48000_norm[] = {
  DSP16_Q(-0.00000000),
  DSP16_Q(0.00000530),
  DSP16_Q(-0.00002188),
  DSP16_Q(0.00005084),
  DSP16_Q(-0.00009327),
  DSP16_Q(0.00015033),
  DSP16_Q(-0.00022327),
  DSP16_Q(0.00031339),
  DSP16_Q(-0.00042212),
  DSP16_Q(0.00055104),
  DSP16_Q(-0.00070195),
  DSP16_Q(0.00087691),
  DSP16_Q(-0.00107835),
  DSP16_Q(0.00130921),
  DSP16_Q(-0.00157311),
  DSP16_Q(0.00187456),
  DSP16_Q(-0.00221941),
  DSP16_Q(0.00261529),
  DSP16_Q(-0.00307246),
  DSP16_Q(0.00360505),
  DSP16_Q(-0.00423300),
  DSP16_Q(0.00498551),
  DSP16_Q(-0.00590684),
  DSP16_Q(0.00706750),
  DSP16_Q(-0.00858673),
  DSP16_Q(0.01068297),
  DSP16_Q(-0.01380334),
  DSP16_Q(0.01902721),
  DSP16_Q(-0.02979063),
  DSP16_Q(0.06602661),
  DSP16_Q(0.33074397),
  DSP16_Q(-0.04707470),
  DSP16_Q(0.02511410),
  DSP16_Q(-0.01692942),
  DSP16_Q(0.01260413),
  DSP16_Q(-0.00989964),
  DSP16_Q(0.00802992),
  DSP16_Q(-0.00664803),
  DSP16_Q(0.00557733),
  DSP16_Q(-0.00471850),
  DSP16_Q(0.00401153),
  DSP16_Q(-0.00341806),
  DSP16_Q(0.00291249),
  DSP16_Q(-0.00247709),
  DSP16_Q(0.00209921),
  DSP16_Q(-0.00176957),
  DSP16_Q(0.00148121),
  DSP16_Q(-0.00122878),
  DSP16_Q(0.00100809),
  DSP16_Q(-0.00081577),
  DSP16_Q(0.00064908),
  DSP16_Q(-0.00050571),
  DSP16_Q(0.00038371),
  DSP16_Q(-0.00028135),
  DSP16_Q(0.00019712),
  DSP16_Q(-0.00012961),
  DSP16_Q(0.00007755),
  DSP16_Q(-0.00003974),
  DSP16_Q(0.00001503),
  DSP16_Q(-0.00000233),
  DSP16_Q(-0.00000115),
  DSP16_Q(0.00001904),
  DSP16_Q(-0.00006021),
  DSP16_Q(0.00012685),
  DSP16_Q(-0.00022121),
  DSP16_Q(0.00034564),
  DSP16_Q(-0.00050267),
  DSP16_Q(0.00069498),
  DSP16_Q(-0.00092553),
  DSP16_Q(0.00119761),
  DSP16_Q(-0.00151497),
  DSP16_Q(0.00188196),
  DSP16_Q(-0.00230375),
  DSP16_Q(0.00278661),
  DSP16_Q(-0.00333832),
  DSP16_Q(0.00396871),
  DSP16_Q(-0.00469051),
  DSP16_Q(0.00552054),
  DSP16_Q(-0.00648154),
  DSP16_Q(0.00760512),
  DSP16_Q(-0.00893640),
  DSP16_Q(0.01054213),
  DSP16_Q(-0.01252512),
  DSP16_Q(0.01505211),
  DSP16_Q(-0.01841197),
  DSP16_Q(0.02315151),
  DSP16_Q(-0.03044409),
  DSP16_Q(0.04334284),
  DSP16_Q(-0.07304678),
  DSP16_Q(0.22036016),
  DSP16_Q(0.22036016),
  DSP16_Q(-0.07304678),
  DSP16_Q(0.04334284),
  DSP16_Q(-0.03044409),
  DSP16_Q(0.02315151),
  DSP16_Q(-0.01841197),
  DSP16_Q(0.01505211),
  DSP16_Q(-0.01252512),
  DSP16_Q(0.01054213),
  DSP16_Q(-0.00893640),
  DSP16_Q(0.00760512),
  DSP16_Q(-0.00648154),
  DSP16_Q(0.00552054),
  DSP16_Q(-0.00469051),
  DSP16_Q(0.00396871),
  DSP16_Q(-0.00333832),
  DSP16_Q(0.00278661),
  DSP16_Q(-0.00230375),
  DSP16_Q(0.00188196),
  DSP16_Q(-0.00151497),
  DSP16_Q(0.00119761),
  DSP16_Q(-0.00092553),
  DSP16_Q(0.00069498),
  DSP16_Q(-0.00050267),
  DSP16_Q(0.00034564),
  DSP16_Q(-0.00022121),
  DSP16_Q(0.00012685),
  DSP16_Q(-0.00006021),
  DSP16_Q(0.00001904),
  DSP16_Q(-0.00000115),
  DSP16_Q(-0.00000233),
  DSP16_Q(0.00001503),
  DSP16_Q(-0.00003974),
  DSP16_Q(0.00007755),
  DSP16_Q(-0.00012961),
  DSP16_Q(0.00019712),
  DSP16_Q(-0.00028135),
  DSP16_Q(0.00038371),
  DSP16_Q(-0.00050571),
  DSP16_Q(0.00064908),
  DSP16_Q(-0.00081577),
  DSP16_Q(0.00100809),
  DSP16_Q(-0.00122878),
  DSP16_Q(0.00148121),
  DSP16_Q(-0.00176957),
  DSP16_Q(0.00209921),
  DSP16_Q(-0.00247709),
  DSP16_Q(0.00291249),
  DSP16_Q(-0.00341806),
  DSP16_Q(0.00401153),
  DSP16_Q(-0.00471850),
  DSP16_Q(0.00557733),
  DSP16_Q(-0.00664803),
  DSP16_Q(0.00802992),
  DSP16_Q(-0.00989964),
  DSP16_Q(0.01260413),
  DSP16_Q(-0.01692942),
  DSP16_Q(0.02511410),
  DSP16_Q(-0.04707470),
  DSP16_Q(0.33074397),
  DSP16_Q(0.06602661),
  DSP16_Q(-0.02979063),
  DSP16_Q(0.01902721),
  DSP16_Q(-0.01380334),
  DSP16_Q(0.01068297),
  DSP16_Q(-0.00858673),
  DSP16_Q(0.00706750),
  DSP16_Q(-0.00590684),
  DSP16_Q(0.00498551),
  DSP16_Q(-0.00423300),
  DSP16_Q(0.00360505),
  DSP16_Q(-0.00307246),
  DSP16_Q(0.00261529),
  DSP16_Q(-0.00221941),
  DSP16_Q(0.00187456),
  DSP16_Q(-0.00157311),
  DSP16_Q(0.00130921),
  DSP16_Q(-0.00107835),
  DSP16_Q(0.00087691),
  DSP16_Q(-0.00070195),
  DSP16_Q(0.00055104),
  DSP16_Q(-0.00042212),
  DSP16_Q(0.00031339),
  DSP16_Q(-0.00022327),
  DSP16_Q(0.00015033),
  DSP16_Q(-0.00009327),
  DSP16_Q(0.00005084),
  DSP16_Q(-0.00002188),
  DSP16_Q(0.00000530),
  DSP16_Q(-0.00000000)
};
