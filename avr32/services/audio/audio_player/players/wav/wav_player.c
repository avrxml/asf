/**
 * \file
 *
 * \brief WAV player
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "compiler.h"
#include "com_task.h"
#include "audio_interface.h"
#include "file.h"

#include "audio_mixer.h"
#include "conf_audio_player.h"
#include "wavdecode.h"

/*! Size of the input buffer in bytes.
 * This buffer is used to read the data from the file itself and will be passed directly
 * to the WAV decoding function.
 */
#define IN_BUFFER_SIZE	512

bool play_wav(const char *filename, bool verbose);
extern Ai_player_flag_t g_player_flag;
extern uint32_t g_player_duration;
extern uint32_t g_player_currentTime_ms;
static uint32_t wav_file_duration_s = 0;

/*! \brief Callback from WAV decoder
 */
bool wav_file_get_data(struct wav_info_t *wav_info)
{
	// How many bytes actually got.
	uint32_t num_bytes_got = 0;

	nav_select(FS_NAV_ID_AUDIO);
	if (wav_info->in_buffer_left) {
		file_seek(wav_info->in_buffer_left, FS_SEEK_CUR_RE);
	}

	// Copy the requested data from the WAV file to the frame buffer.
	nav_checkdisk_disable();
	if (((num_bytes_got = file_read_buf(wav_info->buffer, IN_BUFFER_SIZE))
			< IN_BUFFER_SIZE && !file_eof())) {
		// fprintf(stderr, "** Error while reading the WAV file at byte offset %llu.\r\n", offset);
		return false;
	}

	nav_checkdisk_enable();

	wav_info->buffer_current_pos = wav_info->buffer;
	wav_info->in_buffer_left = num_bytes_got;
	return true;
}

uint32_t wav_get_file_duration(void)
{
	return wav_file_duration_s * 1000;
}

bool play_wav(const char *filename, bool verbose)
{
	struct wav_info_t wav_info;
	bool retval;
	int i;

	// Open the WAV file.
	if (!file_open(FOPEN_MODE_R)) {
		// fprintf(stderr, "** Cannot open the WAV file.\r\n");
		goto lerror;
	}

	// Allocate input buffer
	if ((wav_info.buffer = malloc(IN_BUFFER_SIZE)) == NULL) {
		// fprintf(stderr, "** Not enough memory.\r\n");
		goto lerror;
	}
	wav_info.in_buffer_left = 0;

	// Allocate the PCM buffers (output buffer) (* 2 to handle mono to stereo translation)
	if (!audio_mixer_create_buffers(2, OUTPUT_BUFFER_SIZE * 2)) {
		// fprintf(stderr, "** Cannot create buffers.\r\n");
		goto lerror;
	}

	// Decode WAV file Header
	if (!wav_header_decode(&wav_info)) {
		// fprintf(stderr, "** Error while decoding WAV header.\r\n");
		return false;
	}

	// Read the file duration
	wav_file_duration_s = wav_get_duration_s(&wav_info);

	audio_mixer_dacs_setup(wav_info.sampling_rate,
			2, // wav_info.nChannels,
			16,
			false);
	g_player_currentTime_ms = 0;

	do {
		// Handle pause
		do {
			com_task();
			nav_select(FS_NAV_ID_AUDIO);

			// Mute the DAC(s) if the player is in pause state
			if (g_player_flag.status == PLAYER_FLAG_PAUSE
					&& !audio_mixer_dacs_is_volume_muted()) {
				audio_mixer_mute_audio(true);
			}
		} while (g_player_flag.status == PLAYER_FLAG_PAUSE
				&& g_player_flag.status_fast == PLAYER_FLAG_NORMAL
				&& fat_check_mount_select_open());

		// Unmute the audio if needed
		if (g_player_flag.status == PLAYER_FLAG_PLAY) {
			audio_mixer_mute_audio(false);
		}

		// Wait until a output buffer is available
		if (!(wav_info.pcm_samples = audio_mixer_get_buffer(OUTPUT_BUFFER_SIZE))) {
			continue;
		}

		// FFW / FRW
		if (g_player_flag.status_fast == PLAYER_FLAG_FFW) {
			uint32_t pos;
			audio_mixer_dacs_flush(true);
			nav_select(FS_NAV_ID_AUDIO);
			pos = wav_info.sampling_rate * wav_info.nChannels *
					wav_info.bits_per_sample / 8 * g_player_duration;
			file_seek(pos, FS_SEEK_CUR_FW);
			g_player_currentTime_ms += g_player_duration * 1000;
			g_player_flag.status_fast = PLAYER_FLAG_NORMAL;
		}
		else if (g_player_flag.status_fast == PLAYER_FLAG_FRW) {
			uint32_t pos;
			audio_mixer_dacs_flush(true);
			nav_select(FS_NAV_ID_AUDIO);
			pos = wav_info.sampling_rate * wav_info.nChannels *
					wav_info.bits_per_sample / 8 * g_player_duration;
			file_seek(pos, FS_SEEK_CUR_RE);
			g_player_currentTime_ms -= g_player_duration * 1000;
			g_player_flag.status_fast = PLAYER_FLAG_NORMAL;
		}

		// Decode the Wav file
		if (!wav_decode(&wav_info)) {
			goto lerror;
		}

		// If one channel, duplicate the samples
		if (wav_info.nChannels == 1) {
			for (i = wav_info.pcm_samples_length - 1; i >= 0; i--) {
				wav_info.pcm_samples[2 * i + 1] =
						wav_info.pcm_samples[2 * i] =
						wav_info.pcm_samples[i];
			}
		}

		// Render reconstructed data.
		audio_mixer_dacs_output(wav_info.pcm_samples,
				wav_info.pcm_samples_length / wav_info.nChannels);

		// Update current time
		g_player_currentTime_ms += (wav_info.pcm_samples_length / wav_info.nChannels * 1000)
				/ wav_info.sampling_rate;
		g_player_currentTime_ms = Min(wav_file_duration_s * 1000,
				g_player_currentTime_ms);

	} while (g_player_flag.status != PLAYER_FLAG_STOP);

	retval = true;
	goto lexit;

lerror:
	retval = false;

lexit:
	// Wait for end of rendering.
	audio_mixer_dacs_flush(false);
	audio_mixer_close();

	wav_decode_end(&wav_info);

	free(wav_info.buffer);
	file_close();

	return retval;
}
