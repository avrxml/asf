/*****************************************************************************
 *
 * \file
 *
 * \brief IMA/DVI ADPCM functions for AVR32 UC3.
 *
 * This file defines a useful set of functions for IMA/DVI ADPCM compression/decompression
 * on AVR32 devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "dsp.h"

/*
 * needs the declaration of:
 * int diff, step;
 */
#define ADPCM_IMA_DECODE_NIBBLE(v_nibble, v_step_index, v_predicted_value) \
  step = adpcm_ima_step_table[v_step_index]; \
\
  v_step_index += adpcm_ima_index_table[v_nibble]; \
\
  if (v_step_index < 0) \
    v_step_index = 0; \
  else if (v_step_index > 88) \
    v_step_index = 88; \
\
  diff = step >> 3; \
\
  if (nibble & 4) \
    diff += step; \
  if (nibble & 2) \
    diff += step >> 1; \
  if (nibble & 1) \
    diff += step >> 2; \
  if (nibble & 8) \
    v_predicted_value -= diff; \
  else \
    v_predicted_value += diff; \
\
  if ((v_predicted_value) < -0x8000) \
    (v_predicted_value) = -0x8000; \
  else if ((v_predicted_value) > 0x7fff) \
    (v_predicted_value) = 0x7fff;


/*
 * needs the declaration of:
 * int diff, step, sign, vpdiff;
 */
#define ADPCM_IMA_ENCODE_NIBBLE(v_out, v_nibble, v_step_index, v_predicted_value) \
  step = adpcm_ima_step_table[v_step_index]; \
\
  diff = v_nibble - v_predicted_value; \
  sign = (diff < 0)?8:0; \
  if (sign) \
    diff = -diff; \
\
  v_out = 0; \
  vpdiff = (step >> 3); \
\
  if (diff >= step) \
  { \
    v_out = 4; \
    diff -= step; \
    vpdiff += step; \
  } \
  step >>= 1; \
  if (diff >= step) \
  { \
    v_out |= 2; \
    diff -= step; \
    vpdiff += step; \
  } \
  step >>= 1; \
  if ( diff >= step ) \
  { \
    v_out |= 1; \
    vpdiff += step; \
  } \
\
  if (sign) \
    v_predicted_value -= vpdiff; \
  else \
    v_predicted_value += vpdiff; \
\
  if ((v_predicted_value) < -0x8000) \
    (v_predicted_value) = -0x8000; \
  else if ((v_predicted_value) > 0x7fff) \
    (v_predicted_value) = 0x7fff; \
\
  v_step_index += adpcm_ima_index_table[v_out]; \
\
  v_out |= sign; \
\
  if (v_step_index < 0) \
    v_step_index = 0; \
  else if (v_step_index > 88) \
    v_step_index = 88;



// Static table used in the IMA/DVI ADPCM algorithm
static int adpcm_ima_step_table[89] = {
  7, 8, 9, 10, 11, 12, 13, 14,
  16, 17, 19, 21, 23, 25, 28, 31,
  34, 37, 41, 45, 50, 55, 60, 66,
  73, 80, 88, 97, 107, 118, 130, 143,
  157, 173, 190, 209, 230, 253, 279, 307,
  337, 371, 408, 449, 494, 544, 598, 658,
  724, 796, 876, 963, 1060, 1166, 1282, 1411,
  1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
  3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
  7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
  15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

// Static table used in the IMA/DVI ADPCM algorithm
static int adpcm_ima_index_table[16] = {
  -1, -1, -1, -1, 2, 4, 6, 8,
  -1, -1, -1, -1, 2, 4, 6, 8
};

// Decode a 4-bit nibble previously encoded with the IMA/DVI ADPCM algorithm
S16 dsp_adpcm_ima_decode_nibble(S8 nibble, S16 *step_index, S16 *predicted_value)
{
  int diff, step;
  int int_predicted_value, int_step_index;

  // Cast the predicted value and the step index into 32-bit values
  int_predicted_value = (int) *predicted_value;
  int_step_index = (int) *step_index;

  // Macro to decode a 4-bit sample with the IMA/DVI ADPCM algorithm
  ADPCM_IMA_DECODE_NIBBLE(((int) nibble), int_step_index, int_predicted_value);

  // Restore the predicted value and step index
  *predicted_value = (S16) int_predicted_value;
  *step_index = (S16) int_step_index;

  // Return the result stored in int_predicted_value
  return int_predicted_value;
}

// Decode a buffer previously encoded with the IMA/DVI ADPCM algorithm
void dsp_adpcm_ima_decode(S16 *out, void *in, int size, S16 *step_index, S16 *predicted_value)
{
  U8 *c_in = in;
  int i, nibble, step, diff;
  int int_predicted_value, int_step_index;

  // Cast the predicted value and the step index into 32-bit values
  int_predicted_value = *predicted_value;
  int_step_index = *step_index;

  // Main loop
  for(i=0; i<size; i+=2)
  {
    // Decode the low-nibble (4-bit) of the byte
    nibble = ((int) *c_in)&0xF;
    // Macro to decode a 4-bit sample with the IMA/DVI ADPCM algorithm
    ADPCM_IMA_DECODE_NIBBLE(nibble, int_step_index, int_predicted_value);
    // Store the result
    out[i] = (S16) int_predicted_value;

    // Decode the high-nibble (4-bit) of the byte
    nibble = (((int) *c_in) >> 4)&0xF;
    // Macro to decode a 4-bit sample with the IMA/DVI ADPCM algorithm
    ADPCM_IMA_DECODE_NIBBLE(nibble, int_step_index, int_predicted_value);
    // Store the result
    out[i + 1] = (S16) int_predicted_value;

    c_in++;
  }

  // Restore the predicted value and step index
  *predicted_value = int_predicted_value;
  *step_index = int_step_index;
}

// Encode a 16-bit sample with the IMA/DVI ADPCM algorithm
S8 dsp_adpcm_ima_encode_nibble(S16 nibble, S16 *step_index, S16 *predicted_value)
{
  int int_predicted_value, int_step_index;
  int diff, step, delta, sign, vpdiff;

  // Cast the predicted value and the step index into 32-bit values
  int_predicted_value = (int) *predicted_value;
  int_step_index = (int) *step_index;

  // Macro to encode a sample with the IMA/DVI ADPCM algorithm
  ADPCM_IMA_ENCODE_NIBBLE(delta, ((int) nibble), int_step_index, int_predicted_value);

  // Restore the predicted value and step index
  *predicted_value = (S16) int_predicted_value;
  *step_index = (S16) int_step_index;

  // Return the result stored in delta
  return (S8) delta;
}

// Encode a buffer with the IMA/DVI ADPCM algorithm
void dsp_adpcm_ima_encode(void *out, S16 *in, int size, S16 *step_index, S16 *predicted_value)
{
  U8 *c_out = out;
  int i;
  int int_predicted_value, int_step_index;
  int nibble, diff, step, delta, sign, vpdiff;

  // Cast the predicted value and the step index into 32-bit values
  int_predicted_value = *predicted_value;
  int_step_index = *step_index;

  // Main loop
  for(i=0; i<size; i+=2)
  {
    // Get the first sample to encode
    nibble = in[i];
    // Macro to encode a sample with the IMA/DVI ADPCM algorithm
    ADPCM_IMA_ENCODE_NIBBLE(delta, nibble, int_step_index, int_predicted_value);
    // Store the result in the low-nibble (4-bit) of the byte
    *c_out = delta;

    // Get the second sample to encode
    nibble = in[i + 1];
    // Macro to encode a sample with the IMA/DVI ADPCM algorithm
    ADPCM_IMA_ENCODE_NIBBLE(delta, nibble, int_step_index, int_predicted_value);
    // Store the result in the high-nibble (4-bit) of the byte
    *c_out |= delta << 4;

    c_out++;
  }

  // Restore the predicted value and step index
  *predicted_value = int_predicted_value;
  *step_index = int_step_index;
}
