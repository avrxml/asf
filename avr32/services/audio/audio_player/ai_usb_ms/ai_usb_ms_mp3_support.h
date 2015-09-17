/*****************************************************************************
 *
 * \file
 *
 * \brief MP3 file support.
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

#ifndef _AI_USB_MS_MP3_SUPPORT_H_
#define _AI_USB_MS_MP3_SUPPORT_H_

#include "navigation.h"
#include "reader_id3.h"
#include "audio_interface.h"

#define MP3_FILE_FORMAT                                   \
{                                                         \
  "mp3",                                                  \
  ai_usb_ms_mp3_support_init,                             \
  ai_usb_ms_mp3_support_set_play_file_notification,       \
  ai_usb_ms_mp3_support_clear_play_file_notification,     \
  ai_usb_ms_mp3_support_is_file_playing,                  \
  ai_usb_ms_mp3_support_is_audio_file,                    \
  ai_usb_ms_mp3_support_is_metadata_compatible_file,      \
  ai_usb_ms_mp3_support_get_metadata_version,             \
  reader_id3_title,                                       \
  reader_id3_artist,                                      \
  reader_id3_album,                                       \
  reader_id3_year,                                        \
  reader_id3_track,                                       \
  reader_id3_genre,                                       \
  reader_id3_duration,                                    \
  ai_usb_ms_support_info_image                            \
}
void ai_usb_ms_mp3_support_init(void);
bool ai_usb_ms_mp3_support_is_audio_file(void);
bool ai_usb_ms_mp3_support_is_metadata_compatible_file(void);
St_ai_info_version ai_usb_ms_mp3_support_get_metadata_version(void);
void ai_usb_ms_mp3_support_set_play_file_notification(void);
void ai_usb_ms_mp3_support_clear_play_file_notification(void);
bool ai_usb_ms_mp3_support_is_file_playing(void);
void *ai_usb_ms_support_info_image(ai_image_size_t *size);

#endif // _AI_USB_MS_MP3_SUPPORT_H_
