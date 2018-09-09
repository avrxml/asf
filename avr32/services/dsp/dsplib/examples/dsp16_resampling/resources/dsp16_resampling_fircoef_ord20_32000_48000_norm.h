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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
