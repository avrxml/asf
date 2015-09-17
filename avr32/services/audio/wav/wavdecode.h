/**
 * \file
 *
 * \brief WAV PCM decoder.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _WAVDECODE_H_
#define _WAVDECODE_H_

#include "compiler.h"

/*! \defgroup group_avr32_services_audio_wav WAV Decoder
 *
 * This is the public API for the WAV decoder.
 * This decoder only supports WAV PCM format.
 *
 * \{
 */

/*! Size in byte of the output buffer size
 */
#define OUTPUT_BUFFER_SIZE 0x400

/*!
 * \brief This structure contains information about the WAV file format.
 */
struct wav_info_t {
	uint16_t compression_code;
	uint16_t nChannels;
	uint32_t sampling_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint32_t dataleft;

	uint8_t *buffer;
	uint8_t *buffer_current_pos;
	uint32_t in_buffer_left;

	uint16_t *pcm_samples;
	uint32_t pcm_samples_length;
};

/*!
 * \brief Decode the header of a WAV file.\n
 * This function will fill the \ref wav_info_t structure passed in argument
 * to this function.
 * \param wav_info WAV format information. This structure will be updated with
 * the call of this function.
 * \return true on success, false otherwise.
 */
bool wav_header_decode(struct wav_info_t *wav_info);

/*!
 * \brief Get the duration of a WAV file in seconds.
 * \param wav_info the \ref wav_info_t structure describing the file of interest.
 * \return the total duration of the WAV file in seconds.
 */
uint32_t wav_get_duration_s(struct wav_info_t *wav_info);

/*!
 * \brief Decode a buffer of \ref OUTPUT_BUFFER_SIZE bytes from the WAV file.
 * \param wav_info the \ref wav_info_t structure describing the file of interest.
 * \return true on success, false otherwise.
 */
bool wav_decode(struct wav_info_t *wav_info);

/*!
 * \brief End the decoding, clean remaining buffers.
 * \param wav_info the \ref wav_info_t structure describing the file of interest.
 * \return true on success, false otherwise.
 */
bool wav_decode_end(struct wav_info_t *wav_info);

/*!
 * \brief Callback which needs to be implement on the user side to acquire
 * the WAV file data.
 * \param wav_info the \ref wav_info_t structure describing the file of interest.
 * \return true on success, false otherwise.
 */
extern bool wav_file_get_data(struct wav_info_t *wav_info);

/*! \}
 */

#endif  // _WAVDECODE_H_
