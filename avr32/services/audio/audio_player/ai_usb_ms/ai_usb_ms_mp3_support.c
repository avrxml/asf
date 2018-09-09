/*****************************************************************************
 *
 * \file
 *
 * \brief MP3 file support.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
