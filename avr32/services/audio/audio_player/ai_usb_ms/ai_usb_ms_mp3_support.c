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

#include "conf_audio_player.h"
#include "ai_usb_ms_mp3_support.h"

#if defined(SUPPORT_EMBEDDED_COVER_ARTS) && SUPPORT_EMBEDDED_COVER_ARTS == true
  #include "jpeg_decoder.h"
#endif

bool g_player_flag_run_mp3;

void ai_usb_ms_mp3_support_init(void)
{
  ai_usb_ms_mp3_support_clear_play_file_notification();
#if defined(SUPPORT_EMBEDDED_COVER_ARTS) && SUPPORT_EMBEDDED_COVER_ARTS == true
  jpeg_lib_init();
#endif
}

bool ai_usb_ms_mp3_support_is_audio_file(void)
{
  if (nav_file_checkext("mp3"))
    return true;
  return false;
}

bool ai_usb_ms_mp3_support_is_metadata_compatible_file(void)
{
  if (nav_file_checkext("mp1") || nav_file_checkext("mp2") || nav_file_checkext("mp3"))
    return true;
  return false;
}

St_ai_info_version ai_usb_ms_mp3_support_get_metadata_version(void)
{
  St_ai_info_version ver;
  St_id3ver id3_ver = reader_id3_version();

  ver.digit.major = id3_ver.ver1;
  ver.digit.minor = id3_ver.ver10;
  ver.digit.revision = id3_ver.ver100;

  return ver;
}

void ai_usb_ms_mp3_support_set_play_file_notification(void)
{
  g_player_flag_run_mp3 = true;
}

void ai_usb_ms_mp3_support_clear_play_file_notification(void)
{
  g_player_flag_run_mp3 = false;
}

bool ai_usb_ms_mp3_support_is_file_playing(void)
{
  return g_player_flag_run_mp3;
}

void *ai_usb_ms_support_info_image(ai_image_size_t *size)
{
#if defined(SUPPORT_EMBEDDED_COVER_ARTS) && SUPPORT_EMBEDDED_COVER_ARTS == true

  char mime_type[24];   // "image/jpeg"
  uint8_t picture_type;
  uint32_t picture_offset;
  uint8_t navigator;
  void *image_data;

  navigator = nav_get();
  nav_copy(FS_NAV_ID_COVER);
  nav_select(FS_NAV_ID_COVER);

  // Make sure the selected file is a supported file format
  if (!ai_usb_ms_mp3_support_is_metadata_compatible_file())
  {
    nav_select(navigator);
    return NULL;
  }

  // Read ID3 info about the image
  if( reader_id3_picture ( mime_type, (sizeof(mime_type) - 2) / 2, &picture_type, &picture_offset))
    image_data = (void *) jpeg_lib_decode_ex(picture_offset, (uint16_t *) &(size->width), (uint16_t *) &(size->height));
  else
    image_data = NULL;

  nav_select(navigator);

  return image_data;

#else

  return NULL;

#endif
}
