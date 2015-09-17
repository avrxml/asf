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

#include "wavdecode.h"
#include <string.h>

/*! Tags to delimit sections in the WAV file format.
 * \{
 */
#define TAG_RIFF 0x52494646
#define TAG_WAVE 0x57415645
#define TAG_FMT  0x666D7420
#define TAG_DATA 0x64617461
/*!
 * \}
 */

//! Tag for PCM format
#define PCM_COMP_CODE   0x1

bool (*p_decode_fct) (struct wav_info_t *);

/*! Helper functions
 * \{
 */
static bool get_wav_data(struct wav_info_t *wav_info, uint8_t *data, uint8_t nbbyte);
static bool get_wav_byte(struct wav_info_t *wav_info, uint8_t *data8);
static uint16_t conv16little2big(uint16_t i);
static uint32_t conv32little2big(uint32_t i);
/*!
 * \}
 */

/*!
 * \brief Decode a buffer of \ref OUTPUT_BUFFER_SIZE bytes from the WAV PCM file.
 * \param wav_info the \ref wav_info_t structure describing the file of interest.
 * \return true on success, false otherwise.
 */
static bool pcm_decode(struct wav_info_t *wav_info);

static bool get_wav_data(struct wav_info_t *wav_info, uint8_t *data, uint8_t nbbyte)
{
	if (wav_info->in_buffer_left < nbbyte) {
		if (!wav_file_get_data(wav_info)) {
			return false;
		}

		if (!wav_info->in_buffer_left) {
			return false;
		}
	}

	memcpy(data, wav_info->buffer_current_pos, nbbyte);
	wav_info->buffer_current_pos += nbbyte;
	wav_info->in_buffer_left -= nbbyte;
	return true;
}

static bool get_wav_byte(struct wav_info_t *wav_info, uint8_t *data8)
{
	if (!wav_info->in_buffer_left) {
		if (!wav_file_get_data(wav_info)) {
			return false;
		}
		if (!wav_info->in_buffer_left) {
			return false;
		}
	}

	*data8 = *wav_info->buffer_current_pos;
	wav_info->buffer_current_pos ++;
	wav_info->in_buffer_left -= 1;
	return true;
}

// Little-Big endian conversion functions
static uint16_t conv16little2big(uint16_t i)
{
	return ((i>>8)&0xff)+((i << 8)&0xff00);
}

static uint32_t conv32little2big(uint32_t i)
{
	return ((i&0xff)<<24)+((i&0xff00)<<8)+((i&0xff0000)>>8)+((i>>24)&0xff);
}

//**************************************************
static bool pcm_decode(struct wav_info_t *wav_info)
{
	uint16_t *data16;
	uint8_t data8;

	data16 =  wav_info->pcm_samples;
	wav_info->pcm_samples_length = 0;

	while (wav_info->pcm_samples_length < OUTPUT_BUFFER_SIZE / sizeof(uint16_t)) {
		if (!get_wav_byte(wav_info, &data8)) {
			return false;
		}
		if (wav_info->bits_per_sample == 8) {
			*(data16++)= (data8 + 0x80) << 8;
		}

		if (wav_info->bits_per_sample == 16) {
			*(data16) = data8;
			if (!get_wav_byte(wav_info, &data8)) {
				return false;
			}
			*(data16++) += data8 << 8;
		}
		wav_info->pcm_samples_length++;
	}
	return true;
}

//**********************************************************
// Wav Header Decode Function

bool wav_header_decode(struct wav_info_t *wav_info)
{
	uint32_t tag = 0;
	uint16_t comp_code;


	// Get TAG_RIFF
	get_wav_data(wav_info,(uint8_t*) &tag,4);

	if (tag != TAG_RIFF) {
		return false;
	}

	// Get File Size
	get_wav_data(wav_info,(uint8_t*) &tag,4);

	// Get TAG_WAVE
	get_wav_data(wav_info,(uint8_t*) &tag,4);

	if (tag != TAG_WAVE) {
		return false;
	}

	// Search for TAG_FMT
	while (tag != TAG_FMT) {
		uint8_t newbyte;

		if (!get_wav_data(wav_info,&newbyte,1)) {
			return false; // No more byte available
		}

		tag = (tag << 8) + newbyte;
	}

	// Get Fmt Size
	get_wav_data(wav_info,(uint8_t*) &tag,4);

	// Get Compression Code
	get_wav_data(wav_info,(uint8_t*) &comp_code,2);
	comp_code = conv16little2big(comp_code);

	// Test Compression Code to affect decoder
	if(comp_code == PCM_COMP_CODE) {
		p_decode_fct = pcm_decode;
	}
	else {
		return false;
	}

	// Get Number of Channel
	get_wav_data(wav_info,(uint8_t*) &(wav_info->nChannels),2);
	wav_info->nChannels = conv16little2big(wav_info->nChannels);

	// Get Sample Rate
	get_wav_data(wav_info,(uint8_t*) &(wav_info->sampling_rate),4);
	wav_info->sampling_rate = conv32little2big(wav_info->sampling_rate);

	// Get Average Byte per Seconds
	get_wav_data(wav_info,(uint8_t*) &tag,4);

	// Get Block Align
	get_wav_data(wav_info,(uint8_t*) &(wav_info->block_align),2);
	wav_info->block_align = conv16little2big(wav_info->block_align);

	// Get Bits per Sample
	get_wav_data(wav_info,(uint8_t*) &(wav_info->bits_per_sample),2);
	wav_info->bits_per_sample = conv16little2big(wav_info->bits_per_sample);

	// Search for TAG_DATA
	while(tag != TAG_DATA) {
		uint8_t newbyte;

		if (!get_wav_data(wav_info,&newbyte,1)) {
			return false; // No more byte available
		}

		tag = (tag << 8) + newbyte;
	}

	// Get Data Chunk Size
	get_wav_data(wav_info,(uint8_t*) &(wav_info->dataleft),4);
	wav_info->dataleft = conv32little2big(wav_info->dataleft);

	return true;
}

uint32_t wav_get_duration_s(struct wav_info_t *wav_info)
{
	return (wav_info->dataleft + wav_info->sampling_rate / 2)
			/ (wav_info->bits_per_sample / 8)
			/ wav_info->sampling_rate / wav_info->nChannels;
}

bool wav_decode(struct wav_info_t *wav_info)
{
	return p_decode_fct(wav_info);
}

bool wav_decode_end(struct wav_info_t *wav_info)
{
	return true;
}
