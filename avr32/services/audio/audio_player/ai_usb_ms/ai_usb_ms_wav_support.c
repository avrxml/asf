/**
 * \file
 *
 * \brief Wav file support.
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

#include "conf_audio_player.h"
#include "ai_usb_ms_wav_support.h"

bool g_player_flag_run_wav;

void ai_usb_ms_wav_support_init(void)
{
  ai_usb_ms_wav_support_clear_play_file_notification();
}

bool ai_usb_ms_wav_support_is_audio_file(void)
{
  if (nav_file_checkext("wav"))
    return true;
  return false;
}

bool ai_usb_ms_wav_support_is_metadata_compatible_file(void)
{
  return ai_usb_ms_wav_support_is_audio_file();
}

St_ai_info_version ai_usb_ms_wav_support_get_metadata_version(void)
{
  St_ai_info_version ver = { .all = 0 };
  return ver;
}

void ai_usb_ms_wav_support_set_play_file_notification(void)
{
  g_player_flag_run_wav = true;
}

void ai_usb_ms_wav_support_clear_play_file_notification(void)
{
  g_player_flag_run_wav = false;
}

bool ai_usb_ms_wav_support_is_file_playing(void)
{
  return g_player_flag_run_wav;
}
