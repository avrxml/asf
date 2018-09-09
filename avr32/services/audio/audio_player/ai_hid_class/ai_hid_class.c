/*****************************************************************************
 *
 * \file
 *
 * \brief Audio Interface HID Class Services.
 *
 * This function should be connected to the audio interface to give access to the Audio class audio player
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
