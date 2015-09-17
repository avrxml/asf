/*****************************************************************************
 *
 * \file
 *
 * \brief Audio Interface HID Class Services.
 *
 * This function should be connected to the audio interface to give access to the Audio class audio player
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
#include "ai_hid_class.h"
#include "audio_interface.h"
#include "device_hid_task.h"

static Ai_player_flag_t player_flags;

static bool ai_hid_class_nav_drive_mount(ai_async_status_t *status)
{
  player_flags.all = 0;
  player_flags.status = PLAYER_FLAG_PLAY;

  *status = CMD_DONE;
  return true;
}

static ai_device_status_t ai_hid_class_get_device_status(ai_async_status_t *cmd_ai_status)
{
  extern volatile bool usb_connected;
  *cmd_ai_status = CMD_DONE;

  if (g_usb_mode == USB_MODE_DEVICE && usb_connected)
    return AI_DEVICE_STATUS_CONNECTED;

  return AI_DEVICE_STATUS_NOT_PRESENT;
}

static bool ai_hid_class_audio_nav_next(ai_async_status_t *status)
{
  // Wait until the command as been taken into account
  if (!device_hid_send(HID_NEXT))
  {
    *status = CMD_EXECUTING;
    return false;
  }

  player_flags.status = PLAYER_FLAG_PLAY;
  *status = CMD_DONE;
  return true;
}

static bool ai_hid_class_audio_nav_previous(ai_async_status_t *status)
{
  // Wait until the command as been taken into account
  if (!device_hid_send(HID_PREV))
  {
    *status = CMD_EXECUTING;
    return false;
  }

  player_flags.status = PLAYER_FLAG_PLAY;
  *status = CMD_DONE;
  return true;
}

static bool ai_hid_class_audio_ctrl_resume(ai_async_status_t *status)
{
  // Wait until the command as been taken into account
  if (!device_hid_send(HID_PLAY_PAUSE))
  {
    *status = CMD_EXECUTING;
    return false;
  }

  player_flags.status = PLAYER_FLAG_PLAY;
  *status = CMD_DONE;
  return true;
}

static bool ai_hid_class_audio_ctrl_pause(ai_async_status_t *status)
{
  // Wait until the command as been taken into account
  if (!device_hid_send(HID_PLAY_PAUSE))
  {
    *status = CMD_EXECUTING;
    return false;
  }

  player_flags.status = PLAYER_FLAG_PAUSE;
  *status = CMD_DONE;
  return true;
}

static Ai_player_flag_t ai_hid_class_audio_ctrl_status(ai_async_status_t *status)
{
  *status = CMD_DONE;
  return player_flags;
}

static bool ai_hid_class_audio_ctrl_ffw(uint32_t n, ai_async_status_t *status)
{
  // Wait until the command as been taken into account
  if (!device_hid_send(HID_FAST_FWD))
  {
    *status = CMD_EXECUTING;
    return false;
  }

  player_flags.status = PLAYER_FLAG_PLAY;
  *status = CMD_DONE;
  return true;
}

static bool ai_hid_class_audio_ctrl_frw(uint32_t n, ai_async_status_t *status)
{
  // Wait until the command as been taken into account
  if (!device_hid_send(HID_FAST_RWD))
  {
    *status = CMD_EXECUTING;
    return false;
  }

  player_flags.status = PLAYER_FLAG_PLAY;
  *status = CMD_DONE;
  return true;
}


const ai_fct_set ai_hid_class_fct_set =
{
  ai_init_null,
  ai_hid_class_get_device_status,
  ai_get_product_id_null,
  ai_get_vendor_id_null,
  ai_get_serial_number_null,
  ai_nav_drive_nb_null,
  ai_nav_drive_set_null,
  ai_nav_drive_get_null,
  ai_hid_class_nav_drive_mount,
  ai_nav_drive_total_space_null,
  ai_nav_drive_free_space_null,
  ai_nav_dir_root_null,
  ai_nav_dir_cd_null,
  ai_nav_dir_gotoparent_null,
  ai_nav_file_isdir_null,
  ai_nav_file_next_null,
  ai_nav_file_previous_null,
  ai_nav_file_goto_null,
  ai_nav_file_pos_null,
  ai_nav_file_nb_null,
  ai_nav_dir_nb_null,
  ai_nav_playlist_nb_null,
  ai_nav_dir_name_null,
  ai_nav_file_name_null,
  ai_nav_file_info_type_null,
  ai_nav_file_info_version_null,
  ai_nav_file_info_title_null,
  ai_nav_file_info_artist_null,
  ai_nav_file_info_album_null,
  ai_nav_file_info_year_null,
  ai_nav_file_info_track_null,
  ai_nav_file_info_genre_null,
  ai_nav_file_info_duration_null,
  ai_nav_file_info_image_null,
  ai_nav_getplayfile_null,
  ai_audio_nav_playfile_null,
  ai_audio_context_save_null,
  ai_audio_context_restore_null,
  ai_hid_class_audio_nav_next,
  ai_hid_class_audio_nav_previous,
  ai_audio_nav_eof_occur_null,
  ai_audio_nav_nb_null,
  ai_audio_nav_getpos_null,
  ai_audio_nav_setpos_null,
  ai_audio_nav_repeat_get_null,
  ai_audio_nav_repeat_set_null,
  ai_audio_nav_expmode_get_null,
  ai_audio_nav_expmode_set_null,
  ai_audio_nav_shuffle_get_null,
  ai_audio_nav_shuffle_set_null,
  ai_audio_nav_getname_null,
  ai_audio_nav_file_info_type_null,
  ai_audio_nav_file_info_version_null,
  ai_audio_nav_file_info_title_null,
  ai_audio_nav_file_info_artist_null,
  ai_audio_nav_file_info_album_null,
  ai_audio_nav_file_info_year_null,
  ai_audio_nav_file_info_track_null,
  ai_audio_nav_file_info_genre_null,
  ai_audio_nav_file_info_duration_null,
  ai_audio_nav_file_info_image_null,
  ai_audio_ctrl_stop_null,
  ai_hid_class_audio_ctrl_resume,
  ai_hid_class_audio_ctrl_pause,
  ai_audio_ctrl_time_null,
  ai_hid_class_audio_ctrl_status,
  ai_hid_class_audio_ctrl_ffw,
  ai_hid_class_audio_ctrl_frw,
  ai_audio_ctrl_start_ffw_null,
  ai_audio_ctrl_start_frw_null,
  ai_audio_ctrl_stop_ffw_frw_null,
  ai_specific_async_cmd_task_null
};
