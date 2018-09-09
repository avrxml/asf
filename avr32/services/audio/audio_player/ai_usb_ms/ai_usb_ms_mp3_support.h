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
