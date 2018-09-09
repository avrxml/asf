/**
 * \file
 *
 * \brief WAV PCM decoder.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
