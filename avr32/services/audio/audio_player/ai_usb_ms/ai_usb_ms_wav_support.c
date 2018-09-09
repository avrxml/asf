/**
 * \file
 *
 * \brief Wav file support.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
