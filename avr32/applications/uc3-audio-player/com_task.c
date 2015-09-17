/**************************************************************************
 *
 * \file
 *
 * \brief Joystick communication task
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/


/* This file simulate an audio player interface */
/* Its main goal is to give an example of using */
/* the audio interface.                         */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "intc.h"
#include "pm.h"
#include "cycle_counter.h"

#include "conf_audio_player.h"
#include "audio_interface.h"
#include "audio_mixer.h"

#include "com_task.h"
#include "controller.h"
#include "gui.h"
#include "com_task_struct.h"

#define ELAPSED_TIME_TIMER_VALUE_MS    1000
#define SLEEP_MODE_MS                  5000

static t_cpu_time sleep_timer;

extern device_state_t usb_device_get_state(void);

/*! \brief convert a unicode string to an ascii string.
 *
 * The conversion is done by copying the lower byte of the unicode character
 * to the destination ascii string.
 *
 * \param srcstr Pointer to source unicode string.
 * \param deststr Pointer to destination ascii string location.
 * \param srcstr_len Length of the source unicode string in characters.
 */
static void unicode2ascii(char* deststr, const char* srcstr, size_t srcstr_len)
{
  uint16_t i=0, j=0;

  if(srcstr_len == 0)
  {
    *deststr = '\0';
  }

  while(i<srcstr_len)
  {
    *(deststr+j) = *(srcstr+i+1);
    j++;
    i+=2;
  }
}

/*! \brief Initializes the hardware/software resources required for the task.
 */
void com_task_init(void)
{
  cpu_stop_timeout(&sleep_timer);
  gui_init(FCPU_HZ, FHSB_HZ, FPBB_HZ, FPBA_HZ);
}

device_state_t usb_device_get_state(void)
{
  switch (ai_get_device_status())
  {
  case AI_DEVICE_STATUS_NOT_PRESENT:
    return DEVICE_STATE_NOT_CONNECTED;
  case AI_DEVICE_STATUS_PRESENT:
    return DEVICE_STATE_PRESENT;
  case AI_DEVICE_STATUS_NOT_SUPPORTED:
    return DEVICE_STATE_UNSUPPORTED;
  case AI_DEVICE_STATUS_ERROR:
    return DEVICE_STATE_ERROR;
  case AI_DEVICE_STATUS_CONNECTED:
    return DEVICE_STATE_READY;
  }
  return DEVICE_STATE_ERROR;
}

static void idle_task(struct state_machine_context *state_m)
{
  uint16_t nb_drives;
  uint16_t i_drive;
  static device_state_t last_state;

  state_m->view = GUI_UPDATE_VIEW_IDLE;
  state_m->async_cmd = true;

  switch (state_m->state)
  {
  // This state id the entry point of the idle view.
  // It must be call on every access to this view.
  case STATE_IDLE_ENTRY_POINT:
    state_m->state = STATE_IDLE_WAIT_FOR_EVENT;
    state_m->async_cmd = false;
    state_m->view_elt = GUI_UPDATE_ELT_NONE;
    state_m->cmd_status = true;
    break;
  // This state is the "idle" state of this view.
  case STATE_IDLE_WAIT_FOR_EVENT:
    state_m->async_cmd = false;
    state_m->cmd_status = true;
    switch (usb_device_get_state())
    {
    case DEVICE_STATE_NOT_CONNECTED:
      state_m->cmd_status = true;
      if (last_state != DEVICE_STATE_NOT_CONNECTED)
        state_m->view_elt |= GUI_UPDATE_ELT_IDLE_NO_DEVICE_PRESENT;
      last_state = DEVICE_STATE_NOT_CONNECTED;
      break;
    case DEVICE_STATE_PRESENT:
      if (last_state != DEVICE_STATE_PRESENT)
        state_m->view_elt |= GUI_UPDATE_ELT_IDLE_DEVICE_PRESENT;
      last_state = DEVICE_STATE_PRESENT;
      state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
      break;
    case DEVICE_STATE_READY:
      state_m->state = STATE_IDLE_DRIVE_LOAD;
      last_state = DEVICE_STATE_READY;
      state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
      break;
    case DEVICE_STATE_UNSUPPORTED:
    case DEVICE_STATE_ERROR:
      if (last_state != DEVICE_STATE_UNSUPPORTED)
        state_m->view_elt |= GUI_UPDATE_ELT_IDLE_NOT_SUPPORTED;
      last_state = DEVICE_STATE_UNSUPPORTED;
      break;
    }
    break;
  // Set and mount a drive.
  case STATE_IDLE_DRIVE_LOAD:
    nb_drives = ai_nav_drive_nb();
    for (i_drive=0; i_drive<nb_drives; i_drive++)
    {
      ai_nav_drive_set(i_drive);
      if (ai_nav_drive_mount())
        break;
    }
    if (i_drive == nb_drives)
      state_m->cmd_status = false;
    state_m->async_cmd = false;
    state_m->state = STATE_DEVICE_CONNECTED;
    state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
    break;
  default:
    return;
  }

  // Error management.
  if (state_m->cmd_status == false)
  {
    state_m->view_elt |= GUI_UPDATE_ELT_IDLE_NOT_SUPPORTED;
    last_state = DEVICE_STATE_UNSUPPORTED;
  }
}

static void navigation_update_view(struct state_machine_context *state_m)
{
  // Set the display view
  state_m->display_list.list = &state_m->list.list[0];
  state_m->display_list.nb_elts = Min(state_m->list.nb_valid_entries, MAX_DISPLAY_FILE);
  state_m->view_elt |= GUI_UPDATE_ELT_NAVIGATION_FILE_LIST;
}

static enum file_type navigation_get_current_file_type(struct state_machine_context *state_m)
{
  return state_m->list.list[0].type;
}

static void update_player_status(struct state_machine_context *state_m, Ai_player_flag_t *new_player_status)
{
  state_m->player_status.flags.status = new_player_status->status;
  state_m->player_status.flags.status_fast = new_player_status->status_fast;
  if (new_player_status->new_directory)
    state_m->player_status.flags.new_directory = 1;
  if (new_player_status->new_file_played)
    state_m->player_status.flags.new_file_played = 1;
}

static void navigation_task(struct state_machine_context *state_m)
{
  static size_t item_updated;
  static uint16_t temp_cursor_pos;
  size_t i;

  // By default, the command executed is asynchronous.
  state_m->async_cmd = true;
  state_m->view = GUI_UPDATE_VIEW_NAVIGATION;

  switch (state_m->state)
  {
  // This state id the entry point of the navigation view.
  // It must be call on every access to this view.
  case STATE_NAVIGATION_ENTRY_POINT:
    state_m->view_elt = GUI_UPDATE_ELT_NAVIGATION_CURSOR;
    state_m->async_cmd = false;
    state_m->cmd_status = true;
    // Update file_list relatives info.
    state_m->list.file_pos = 0;
    state_m->list.nb_files = 0xFFFF;
    state_m->list.nb_valid_entries = 0;
    temp_cursor_pos = 0;
    state_m->cursor_pointer = 0;
    // Invalidate all the items on the list
    for (i=0; i<MAX_BUFFER_FILE; i++)
      state_m->list.list[i].updated = false;
    state_m->state = STATE_NAVIGATION_UPDATE_LIST;
    break;
  // Update the file list from the current directory.
  case STATE_NAVIGATION_UPDATE_LIST:
    // Re-center the file list.
    if (temp_cursor_pos < state_m->list.file_pos &&
        state_m->list.nb_valid_entries)
    {
      // Scroll-down the list
      memmove((void *) &state_m->list.list[1],
              (const void *) &state_m->list.list[0],
              sizeof(state_m->list.list[0])*(MAX_BUFFER_FILE - 1));
      // Every element of the list have to be updated
      for (i=1; i<MAX_BUFFER_FILE; i++)
        state_m->list.list[i].updated = true;
      // Get information of the file.
      item_updated = 0;
      state_m->list.file_pos--;
      state_m->list.nb_valid_entries--;
      ai_async_nav_file_goto(state_m->list.file_pos + item_updated);
      state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
      state_m->state = STATE_NAVIGATION_UPDATE_LIST_GET_NAME;
      break;
    }
    if (temp_cursor_pos > state_m->list.file_pos &&
        state_m->list.nb_valid_entries)
    {
      // Scroll-up the list
      memmove((void *) &state_m->list.list[0],
              (const void *) &state_m->list.list[1],
              sizeof(state_m->list.list[0])*(MAX_BUFFER_FILE - 1));
      // Every element of the list have to be updated
      for (i=0; i<MAX_BUFFER_FILE-1; i++)
        state_m->list.list[i].updated = true;
      // Get information of the file.
      item_updated = MAX_BUFFER_FILE - 1;
      state_m->list.file_pos++;
      state_m->list.nb_valid_entries--;
      ai_async_nav_file_goto(state_m->list.file_pos + item_updated);
      state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
      state_m->state = STATE_NAVIGATION_UPDATE_LIST_GET_NAME;
      break;
    }
    // There is nothing to update
    state_m->async_cmd = false;
    state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
    state_m->state = STATE_NAVIGATION_WAIT_FOR_EVENT;
    break;
  // Read the file name of the file selected.
  case STATE_NAVIGATION_UPDATE_LIST_GET_NAME:
    // The case when there is no more files...
    if (state_m->cmd_status == false)
    {
      state_m->list.nb_files = state_m->list.file_pos + item_updated;
      // If no files/folders are found in the current directory...
      if (!state_m->list.nb_files)
        state_m->view_elt |= GUI_UPDATE_ELT_NAVIGATION_NO_FILES;
      state_m->cmd_status = true;
      state_m->async_cmd = false;
      navigation_update_view(state_m);
      state_m->state = STATE_NAVIGATION_WAIT_FOR_EVENT;
      break;
    }
    ai_async_nav_file_name();
    state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
    state_m->state = STATE_NAVIGATION_UPDATE_LIST_STORE_NAME;
    break;
  // Store the file name of the selected file.
  case STATE_NAVIGATION_UPDATE_LIST_STORE_NAME:
    unicode2ascii((char *) state_m->list.list[item_updated].file_name,
                  (const char *) ai_async_cmd_out_PtrArrayU8(),
                  Min(ai_async_cmd_out_SizeArrayU8(), STR_MAX_LENGTH*2));
    ai_async_nav_file_isdir();
    state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
    state_m->state = STATE_NAVIGATION_UPDATE_ISDIR;
    break;
  // Check if the selected file is a directory or not.
  case STATE_NAVIGATION_UPDATE_ISDIR:
    state_m->list.list[item_updated].type = (ai_async_cmd_out_u32())?FILE_TYPE_DIRECTORY:FILE_TYPE_FILE;
    state_m->list.list[item_updated].updated = true;
    state_m->list.nb_valid_entries++;
    navigation_update_view(state_m);
    state_m->async_cmd = false;
    state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
    state_m->state = STATE_NAVIGATION_UPDATE_LIST;
    break;
  // This state is the "idle" state of this view.
  case STATE_NAVIGATION_WAIT_FOR_EVENT:
    // Catch new track event
    state_m->cmd_status = true;
    if (state_m->player_status.flags.new_file_played)
    {
      state_m->async_cmd = false;
      state_m->player_status.flags.new_file_played = 0;
      state_m->recorded_state = STATE_NAVIGATION_WAIT_FOR_EVENT;
      state_m->state = STATE_TRACK_CHANGED_ENTRY_POINT;
      break;
    }
    // Switch to playback view
    else if (controller_switch_to_playback_view(GUI_UPDATE_VIEW_NAVIGATION))
    {
      controller_clear();
      state_m->async_cmd = false;
      state_m->state = STATE_PLAYBACK_ENTRY_POINT;
      break;
    }
    // Switch to configuration view
    else if (controller_switch_to_config_view(GUI_UPDATE_VIEW_NAVIGATION))
    {
      controller_clear();
      state_m->async_cmd = false;
      state_m->state = STATE_CONFIG_ENTRY_POINT;
      break;
    }
    // Go up in the file list.
    else if (controller_navigation_cursor_previous() &&
             temp_cursor_pos)
    {
      state_m->async_cmd = false;
      temp_cursor_pos--;
      state_m->state = STATE_NAVIGATION_UPDATE_LIST;
      break;
    }
    // Go down in the file list.
    else if (controller_navigation_cursor_next() &&
             (temp_cursor_pos + 1 < state_m->list.file_pos + state_m->list.nb_valid_entries))
    {
      state_m->async_cmd = false;
      temp_cursor_pos++;
      state_m->state = STATE_NAVIGATION_UPDATE_LIST;
      break;
    }
    // Enter directory of play the file.
    else if (controller_navigation_change_directory())
    {
      if (navigation_get_current_file_type(state_m) == FILE_TYPE_FILE)
      {
        ai_async_nav_file_goto(temp_cursor_pos);
        state_m->state = STATE_NAVIGATION_PLAY_SELECTED_FILE;
        break;
      }
      else
      {
        ai_async_nav_file_goto(temp_cursor_pos);
        state_m->state = STATE_NAVIGATION_CD;
        break;
      }
    }
    // Go to parent directory.
    else if (controller_navigation_go_to_parent_directory())
    {
      ai_async_nav_file_goto(temp_cursor_pos);
      state_m->state = STATE_NAVIGATION_GOTOPARENT;
      break;
    }
    // Play the file or the directory.
    else if (controller_navigation_play())
    {
      ai_async_nav_file_goto(temp_cursor_pos);
      state_m->state = STATE_NAVIGATION_PLAY_SELECTED_FILE;
      break;
    }
    // Fill the file list with valid data.
    else if (state_m->list.nb_valid_entries < MAX_BUFFER_FILE &&
             state_m->list.file_pos + state_m->list.nb_valid_entries < state_m->list.nb_files)
    {
      item_updated = state_m->list.nb_valid_entries;
      ai_async_nav_file_goto(state_m->list.file_pos + state_m->list.nb_valid_entries);
      state_m->view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
      state_m->state = STATE_NAVIGATION_UPDATE_LIST_GET_NAME;
      break;
    }
    state_m->async_cmd = false;
    state_m->state = STATE_CHECK_DEVICE_ENTRY_POINT;
    state_m->recorded_state = STATE_NAVIGATION_WAIT_FOR_EVENT;
    break;
  // Enter directory.
  case STATE_NAVIGATION_CD:
    ai_async_nav_dir_cd();
    state_m->state = STATE_NAVIGATION_ENTRY_POINT;
    break;
  // Go to parent directory.
  case STATE_NAVIGATION_GOTOPARENT:
    state_m->cmd_status = true;
    ai_async_nav_dir_gotoparent();
    state_m->state = STATE_NAVIGATION_GOTOPARENT_ERROR_HANDLING;
    break;
  // This state handle a gotoparent error.
  case STATE_NAVIGATION_GOTOPARENT_ERROR_HANDLING:
    state_m->async_cmd = false;
    if (state_m->cmd_status == false)
      state_m->state = STATE_NAVIGATION_WAIT_FOR_EVENT;
    else
      state_m->state = STATE_NAVIGATION_ENTRY_POINT;
    state_m->cmd_status = true;
    break;
  // This states play the file selected by the cursor.
  case STATE_NAVIGATION_PLAY_SELECTED_FILE:
    if (state_m->cmd_status)
      ai_async_audio_nav_playfile();
    else
    {
      state_m->cmd_status = true;
      state_m->async_cmd = false;
      state_m->state = STATE_NAVIGATION_ENTRY_POINT;
      break;
    }
    state_m->state = STATE_NAVIGATION_WAIT_FOR_SELECTION;
    break;
  // This states makes sure the file has been selected
  case STATE_NAVIGATION_WAIT_FOR_SELECTION:
    // If we were not able to play the selected song, then play any song
    if (!state_m->cmd_status)
    {
      state_m->cmd_status = true;
      state_m->state = STATE_COMMAND_PLAY_ANY_SONG;
    }
    else
    state_m->state = STATE_CHECK_DEVICE_ENTRY_POINT;
    state_m->recorded_state = STATE_NAVIGATION_UPDATE_METADATA_AND_PLAY;
    state_m->async_cmd = false;
    break;
  // This state update the status of the audio player.
  case STATE_NAVIGATION_UPDATE_METADATA_AND_PLAY:
    state_m->async_cmd = false;
    state_m->player_status.flags.new_file_played = 0;
    state_m->recorded_state = STATE_PLAYBACK_ENTRY_POINT;
    state_m->state = STATE_TRACK_CHANGED_ENTRY_POINT;
    break;

  default:
    return;
  }

  // Error management.
  if (state_m->cmd_status == false)
    state_m->state = STATE_NAVIGATION_WAIT_FOR_EVENT;
}

static void playback_task(struct state_machine_context *state_m)
{
  static enum
  {
    PLAYER_STATUS_NOT_DEFINED,
    PLAYER_STATUS_PLAY,
    PLAYER_STATUS_PAUSE,
    PLAYER_STATUS_STOP,
    PLAYER_STATUS_FFW,
    PLAYER_STATUS_FRW
  } current_view_player_status;
  static bool fast_mode = false;

  // By default, the command executed is asynchronous.
  state_m->async_cmd = true;
  state_m->view = GUI_UPDATE_VIEW_PLAYBACK;

  switch (state_m->state)
  {
  // This state id the entry point of the navigation view.
  // It must be call on every access to this view.
  case STATE_PLAYBACK_ENTRY_POINT:
    state_m->view_elt = GUI_UPDATE_ELT_NONE;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_ELAPSED_TIME;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_VOLUME;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_ARTIST;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_TITLE;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_FILE_NAME;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_TOTAL_TIME;
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_COVER_ART;
    state_m->cmd_status = true;
    state_m->async_cmd = false;
    state_m->info.volume = audio_mixer_dacs_get_volume();
    current_view_player_status = PLAYER_STATUS_NOT_DEFINED;
    cpu_set_timeout(cpu_ms_2_cy(ELAPSED_TIME_TIMER_VALUE_MS, FCPU_HZ), &state_m->elapsed_time_timer);
    state_m->state = STATE_PLAYBACK_WAIT_FOR_EVENT;
    break;
  // This state is the "idle" state of this view.
  case STATE_PLAYBACK_WAIT_FOR_EVENT:
    // Catch new track event
    if (state_m->player_status.flags.new_file_played)
    {
      // Notify the controller a new track is being played
      controller_playback_ffw(true);
      controller_playback_frw(true);
      state_m->async_cmd = false;
      state_m->player_status.flags.new_file_played = 0;
      state_m->recorded_state = STATE_PLAYBACK_ENTRY_POINT;
      state_m->state = STATE_TRACK_CHANGED_ENTRY_POINT;
      break;
    }
    // Switch to navigation view
    else if (controller_switch_to_navigation_view(GUI_UPDATE_VIEW_PLAYBACK))
    {
      controller_clear();
      state_m->async_cmd = false;
      state_m->state = STATE_NAVIGATION_ENTRY_POINT;
      break;
    }
    // Switch to configuration view
    else if (controller_switch_to_config_view(GUI_UPDATE_VIEW_PLAYBACK))
    {
      controller_clear();
      state_m->async_cmd = false;
      state_m->state = STATE_CONFIG_ENTRY_POINT;
      break;
    }
    // Increase volume
    else if (controller_playback_increase_volume())
    {
      state_m->async_cmd = false;
      audio_mixer_dacs_increase_volume();
      audio_mixer_dacs_increase_volume();
      state_m->info.volume = audio_mixer_dacs_get_volume();
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_VOLUME;
      break;
    }
    // Decrease volume
    else if (controller_playback_decrease_volume())
    {
      state_m->async_cmd = false;
      audio_mixer_dacs_decrease_volume();
      audio_mixer_dacs_decrease_volume();
      state_m->info.volume = audio_mixer_dacs_get_volume();
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_VOLUME;
      break;
    }
    else if (controller_playback_ffw(false))
    {
      ai_async_audio_ctrl_start_ffw();
      state_m->state = STATE_CHECK_DEVICE_ENTRY_POINT;
      state_m->recorded_state = STATE_PLAYBACK_HANDLE_FAST_MODES;
      fast_mode = true;
      break;
    }
    else if (controller_playback_frw(false))
    {
      ai_async_audio_ctrl_start_frw();
      state_m->state = STATE_CHECK_DEVICE_ENTRY_POINT;
      state_m->recorded_state = STATE_PLAYBACK_HANDLE_FAST_MODES;
      fast_mode = true;
      break;
    }
    else if (fast_mode)
    {
      ai_async_audio_ctrl_stop_ffw_frw();
      state_m->state = STATE_CHECK_DEVICE_ENTRY_POINT;
      state_m->recorded_state = STATE_PLAYBACK_HANDLE_FAST_MODES;
      fast_mode = false;
      break;
    }
    // Previous track
    else if (controller_playback_previous_track())
    {
      ai_async_audio_nav_previous();
      break;
    }
    // Next track
    else if (controller_playback_next_track())
    {
      ai_async_audio_nav_next();
      break;
    }
    // Toggle play/pause
    else if (controller_playback_toggle_play_pause())
    {
      switch (state_m->player_status.flags.status)
      {
      case PLAYER_FLAG_PLAY:
        ai_async_audio_ctrl_pause();
        state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_PAUSE;
        break;
      case PLAYER_FLAG_PAUSE:
        ai_async_audio_ctrl_resume();
        state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_PLAY;
        break;
      case PLAYER_FLAG_STOP:
        ai_async_audio_nav_playfile();
        state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_PLAY;
        break;
      }
      break;
    }
    else if (cpu_is_timeout(&state_m->elapsed_time_timer))
    {
      cpu_set_timeout(cpu_ms_2_cy(ELAPSED_TIME_TIMER_VALUE_MS, FCPU_HZ), &state_m->elapsed_time_timer);
      ai_async_audio_ctrl_time();
      state_m->state = STATE_PLAYBACK_UPDATE_TIME;
      break;
    }
    state_m->async_cmd = false;
    state_m->state = STATE_CHECK_DEVICE_ENTRY_POINT;
    state_m->recorded_state = STATE_PLAYBACK_UPDATE_STATUS;
    break;
  // This state is called after fats forward or fast rewind commands to handle return states.
  case STATE_PLAYBACK_HANDLE_FAST_MODES:
    cpu_set_timeout(cpu_ms_2_cy(ELAPSED_TIME_TIMER_VALUE_MS, FCPU_HZ), &state_m->elapsed_time_timer);
    ai_async_audio_ctrl_time();
    state_m->state = STATE_PLAYBACK_UPDATE_TIME;
    break;
  // This state update the elapsed time of the current track being played.
  case STATE_PLAYBACK_UPDATE_TIME:
    state_m->async_cmd = false;
    state_m->info.elapsed_time = ai_async_cmd_out_u32();
    state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_ELAPSED_TIME;
    state_m->state = STATE_PLAYBACK_WAIT_FOR_EVENT;
    break;
  // This state update the elapsed time of the current track being played.
  case STATE_PLAYBACK_UPDATE_STATUS:
    state_m->async_cmd = false;
    // Update control GUI
    if (state_m->player_status.flags.status_fast == PLAYER_FLAG_FFW &&
        current_view_player_status != PLAYER_STATUS_FFW)
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_FFW;
    else if (state_m->player_status.flags.status_fast == PLAYER_FLAG_FRW &&
             current_view_player_status != PLAYER_STATUS_FRW)
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_FRW;
    else if (state_m->player_status.flags.status == PLAYER_FLAG_PLAY &&
             current_view_player_status != PLAYER_STATUS_PLAY)
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_PLAY;
    else if (state_m->player_status.flags.status == PLAYER_FLAG_PAUSE &&
             current_view_player_status != PLAYER_STATUS_PAUSE)
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_PAUSE;
    else if (state_m->player_status.flags.status == PLAYER_FLAG_STOP &&
             current_view_player_status != PLAYER_STATUS_STOP)
      state_m->view_elt |= GUI_UPDATE_ELT_PLAYBACK_STOP;
    state_m->state = STATE_PLAYBACK_WAIT_FOR_EVENT;
    break;
  default:
    return;
  }

  // Error management
  if (state_m->cmd_status == false)
    state_m->state = STATE_PLAYBACK_WAIT_FOR_EVENT;
}


static void config_task(struct state_machine_context *state_m)
{
  // By default, the command executed is asynchronous.
  state_m->async_cmd = true;
  state_m->view = GUI_UPDATE_VIEW_CONFIG;

  switch (state_m->state)
  {
  // This state id the entry point of the configuration view.
  // It must be call on every access to this view.
  case STATE_CONFIG_ENTRY_POINT:
    state_m->view_elt = GUI_UPDATE_ELT_NONE;
    state_m->view_elt |= GUI_UPDATE_ELT_CONFIG_CURSOR;
    state_m->cmd_status = true;
    state_m->async_cmd = false;
    state_m->display_list.mode_pos = MODE_POS_REPEAT;
    state_m->state = STATE_CONFIG_UPDATE_STATES;
    break;
  // This state is the "idle" state of this view.
  case STATE_CONFIG_WAIT_FOR_EVENT:
    // Switch to navigation view
    if (controller_switch_to_navigation_view(GUI_UPDATE_VIEW_CONFIG))
    {
      controller_clear();
      state_m->async_cmd = false;
      state_m->state = STATE_NAVIGATION_ENTRY_POINT;
      break;
    }
    else if (controller_switch_to_playback_view(GUI_UPDATE_VIEW_CONFIG))
    {
      controller_clear();
      state_m->async_cmd = false;
      state_m->state = STATE_PLAYBACK_ENTRY_POINT;
      break;
    }
    else if (controller_config_change_mode())
    {
      switch (state_m->display_list.mode_pos)
      {
      case MODE_POS_SHUFFLE:
        switch (state_m->player_status.shuffle)
        {
        case AUDIO_SHUFFLE_FOLDER:
          ai_async_audio_nav_shuffle_set(AUDIO_SHUFFLE_ALL);
          break;
        case AUDIO_SHUFFLE_ALL:
          ai_async_audio_nav_shuffle_set(AUDIO_SHUFFLE_OFF);
          break;
        case AUDIO_SHUFFLE_OFF:
        default:
          ai_async_audio_nav_shuffle_set(AUDIO_SHUFFLE_FOLDER);
        }
        break;
      case MODE_POS_REPEAT:
        switch (state_m->player_status.repeat)
        {
        case AUDIO_REPEAT_TRACK:
          ai_async_audio_nav_repeat_set(AUDIO_REPEAT_FOLDER);
          break;
        case AUDIO_REPEAT_FOLDER:
          ai_async_audio_nav_repeat_set(AUDIO_REPEAT_ALL);
          break;
        case AUDIO_REPEAT_ALL:
          ai_async_audio_nav_repeat_set(AUDIO_REPEAT_OFF);
          break;
        case AUDIO_REPEAT_OFF:
        default:
          ai_async_audio_nav_repeat_set(AUDIO_REPEAT_TRACK);
        }
        break;
      }
      state_m->state = STATE_CONFIG_UPDATE_STATES;
      break;
    }
    else if (controller_config_next_option())
    {
      state_m->async_cmd = false;
      switch (state_m->display_list.mode_pos)
      {
      case MODE_POS_REPEAT:
        state_m->display_list.mode_pos = MODE_POS_SHUFFLE;
        break;
      case MODE_POS_SHUFFLE:
      default:
        state_m->display_list.mode_pos = MODE_POS_REPEAT;
      }
      state_m->view_elt |= GUI_UPDATE_ELT_CONFIG_CURSOR;
      break;
    }
    else if (controller_config_previous_option())
    {
      state_m->async_cmd = false;
      switch (state_m->display_list.mode_pos)
      {
      case MODE_POS_REPEAT:
        state_m->display_list.mode_pos = MODE_POS_SHUFFLE;
        break;
      case MODE_POS_SHUFFLE:
      default:
        state_m->display_list.mode_pos = MODE_POS_REPEAT;
      }
      state_m->view_elt |= GUI_UPDATE_ELT_CONFIG_CURSOR;
      break;
    }
    state_m->async_cmd = false;
    break;
  // Get repeat states.
  case STATE_CONFIG_UPDATE_STATES:
    ai_async_audio_nav_repeat_get();
    state_m->state = STATE_CONFIG_READ_REPEAT_STATE;
    break;
  // Get shuffle states.
  case STATE_CONFIG_READ_REPEAT_STATE:
    state_m->player_status.repeat = ai_async_cmd_out_u32();
    state_m->view_elt |= GUI_UPDATE_ELT_CONFIG_REPEAT;
    ai_async_audio_nav_shuffle_get();
    state_m->state = STATE_CONFIG_READ_SHUFFLE_STATE;
    break;
  // Read shuffle states.
  case STATE_CONFIG_READ_SHUFFLE_STATE:
    state_m->player_status.shuffle = ai_async_cmd_out_u32();
    state_m->view_elt |= GUI_UPDATE_ELT_CONFIG_SHUFFLE;
    state_m->state = STATE_CONFIG_WAIT_FOR_EVENT;
    break;
  default:
    return;
  }

  // Error management
  if (state_m->cmd_status == false)
    state_m->state = STATE_CONFIG_ENTRY_POINT;
}

static void check_device_task(struct state_machine_context *state_m)
{
  Ai_player_flag_t player_flag_temp;
  static uint32_t old_status = 0;
  static t_cpu_time get_play_time_timer = {
      .timer_state    = CPU_TIMER_STATE_STOPPED
  };

  // By default, the command executed is asynchronous.
  state_m->async_cmd = true;

  switch (state_m->state)
  {
  // This state id the entry point of the check device function.
  case STATE_CHECK_DEVICE_ENTRY_POINT:
    state_m->cmd_status = true;
    if (usb_device_get_state() != DEVICE_STATE_READY)
    {
      state_m->async_cmd = false;
      state_m->state = STATE_DEVICE_DISCONNECTED;
      break;
    }
    if( cpu_is_timer_stopped(&get_play_time_timer) ) {
      cpu_set_timeout(cpu_ms_2_cy(ELAPSED_TIME_TIMER_VALUE_MS, FCPU_HZ), &get_play_time_timer);
      ai_async_audio_ctrl_status();
      state_m->state = STATE_CHECK_DEVICE_UPDATE_STATUS;
    } else {
      if( cpu_is_timeout(&get_play_time_timer) ) {
        cpu_stop_timeout(&get_play_time_timer);
      }
      state_m->async_cmd = false;
      player_flag_temp.all = old_status;
      update_player_status(state_m, (Ai_player_flag_t *) &player_flag_temp);
      state_m->state = state_m->recorded_state;
    }
    break;
  // This state update the elapsed time of the current track being played.
  case STATE_CHECK_DEVICE_UPDATE_STATUS:
    state_m->async_cmd = false;
    player_flag_temp.all = ai_async_cmd_out_u32();
    update_player_status(state_m, (Ai_player_flag_t *) &player_flag_temp);

    // The transitional states such as "new file played" can not be kept in the saved status.
    // Otherwise, we will send during 'ELAPSED_TIME_TIMER_VALUE_MS' second(s) a saved status with
    // the "new_file_played" event, leading do a full redisplay of the metadata information and audio
    // cracks. In other words, the saved status needs to keep the states (play/pause/...), not the events.
    player_flag_temp.new_file_played = 0;
    player_flag_temp.new_directory = 0;
    old_status = player_flag_temp.all;

    state_m->state = state_m->recorded_state;
    break;
  default:
    return;
  }

  // Error management
  if (state_m->cmd_status == false)
    state_m->state = STATE_DEVICE_DISCONNECTED;
}

static void track_changed_task(struct state_machine_context *state_m)
{
  // By default, the command executed is asynchronous.
  state_m->async_cmd = true;
  Ai_player_flag_t player_flag_temp;

  switch (state_m->state)
  {
  case STATE_TRACK_CHANGED_ENTRY_POINT:
    memset(&state_m->info, 0, sizeof(struct file_info));
    ai_async_audio_nav_file_info_duration();
    state_m->state = STATE_TRACK_CHANGED_TOTAL_TIME;
    break;
  // Record total time info and call file name.
  case STATE_TRACK_CHANGED_TOTAL_TIME:
    state_m->info.total_time = ai_async_cmd_out_u32();
    ai_async_audio_nav_getname();
    state_m->state = STATE_TRACK_CHANGED_FILE_NAME;
    break;
  // Record file name information and call for artist metadata.
  case STATE_TRACK_CHANGED_FILE_NAME:
    unicode2ascii((char *) state_m->info.name,
                  (const char *) ai_async_cmd_out_PtrArrayU8(),
                  Min(ai_async_cmd_out_SizeArrayU8(), STR_MAX_LENGTH*2));
    ai_async_audio_nav_file_info_artist();
    state_m->state = STATE_TRACK_CHANGED_ARTIST;
    break;
  // Record artist metadata and call title metadata.
  case STATE_TRACK_CHANGED_ARTIST:
    unicode2ascii((char *) state_m->info.artist,
                  (const char *) ai_async_cmd_out_PtrArrayU8(),
                  Min(ai_async_cmd_out_SizeArrayU8(), STR_MAX_LENGTH*2));
    ai_async_audio_nav_file_info_title();
    state_m->state = STATE_TRACK_CHANGED_TITLE;
    break;
  // Record title metadata.
  case STATE_TRACK_CHANGED_TITLE:
    unicode2ascii((char *) state_m->info.title,
                  (const char *) ai_async_cmd_out_PtrArrayU8(),
                  Min(ai_async_cmd_out_SizeArrayU8(), STR_MAX_LENGTH*2));
#if defined(SUPPORT_EMBEDDED_COVER_ARTS) && SUPPORT_EMBEDDED_COVER_ARTS == true
    state_m->info.size.width = 100;
    state_m->info.size.height = 100;
    ai_async_audio_nav_file_info_image(&state_m->info.size);
    state_m->state = STATE_TRACK_CHANGED_IMAGE;
#else
    ai_async_audio_ctrl_status();
    state_m->state = STATE_TRACK_CHECK_RESUME;
#endif
    break;
  // Record image data.
  case STATE_TRACK_CHANGED_IMAGE:
    state_m->info.image_data = (void *) ai_async_cmd_out_u32();
    ai_async_audio_ctrl_status();
    state_m->state = STATE_TRACK_CHECK_RESUME;
    break;
  // Resume the track
  case STATE_TRACK_CHANGED_RESUME:
    state_m->async_cmd = false;
    state_m->state = state_m->recorded_state;
    break;
    // Check if the device is in pause before sending a ai_async_audio_ctrl_resume()
    // command.
  case STATE_TRACK_CHECK_RESUME:
    player_flag_temp.all = ai_async_cmd_out_u32();
    if (player_flag_temp.status == PLAYER_FLAG_PAUSE)
      ai_async_audio_ctrl_resume();
    state_m->state = STATE_TRACK_CHANGED_RESUME;
    break;
  default:
    return;
  }

  // Error management
  if (state_m->cmd_status == false)
    state_m->state = STATE_DEVICE_DISCONNECTED;
}

static void command_task(struct state_machine_context *state_m)
{
  // By default, the command executed is asynchronous.
  state_m->async_cmd = true;

  switch (state_m->state)
  {
  // This state plays any song on the device.
  // It can be used at startup if the user wants to play a track.
  case STATE_COMMAND_PLAY_ANY_SONG:
    ai_async_audio_ctrl_resume();
    state_m->state = state_m->recorded_state;
    break;
  default:
    return;
  }

  // Error management
  if (state_m->cmd_status == false)
    state_m->state = state_m->recorded_state;
}

/*! \brief Entry point of the audio management interface.
 *
 */
void com_task(void)
{
  static struct state_machine_context state_m = {
    .state                = STATE_INITIALIZATION,
    .async_cmd            = false,
    .view                 = GUI_UPDATE_VIEW_NONE,
    .view_elt             = GUI_UPDATE_ELT_NONE,
    .elapsed_time_timer.timer_state   = CPU_TIMER_STATE_STOPPED
  };

  // Update the GUI
  gui_update(state_m.view, &state_m.view_elt, &state_m.display_list, &state_m.info, &state_m.player_status);

  // Ask the audio interface to execute pending tasks
  ai_async_cmd_task();

  if (state_m.async_cmd)
  {
    // If current command is not done
    if (!is_ai_async_cmd_finished())
    {
      // If it is a new command that is being proceed
      if (state_m.in_progress_timer.timer_state == CPU_TIMER_STATE_STOPPED)
        cpu_set_timeout(cpu_ms_2_cy(500, FCPU_HZ), &state_m.in_progress_timer);
      // If current command is not done and it is taking a long
      else if (cpu_is_timeout(&state_m.in_progress_timer))
        state_m.view_elt |= GUI_UPDATE_ELT_IN_PROGRESS;
      return;
    }
    else
    {
      state_m.cmd_status = ai_async_cmd_out_status();
      cpu_stop_timeout(&state_m.in_progress_timer);
    }
  }

  // If a device is connected
  if (state_m.state != STATE_INITIALIZATION &&
      state_m.state != STATE_IDLE_ENTRY_POINT &&
      state_m.state != STATE_IDLE_WAIT_FOR_EVENT)
  {
    // If no device is connected, then jump to the disconnection state
    if (ai_is_none())
    {
      ai_command_abort();
      state_m.state = STATE_DEVICE_DISCONNECTED;
    }
  }

  switch (state_m.state)
  {
  case STATE_INITIALIZATION:
    state_m.state = STATE_IDLE_ENTRY_POINT;
    cpu_stop_timeout(&state_m.in_progress_timer);
    // Set default volume if specified
#if defined(DEFAULT_VOLUME)
    audio_mixer_dacs_set_volume(DEFAULT_VOLUME);
#endif
    break;
  case STATE_DEVICE_CONNECTED:
    controller_init(FCPU_HZ, FHSB_HZ, FPBB_HZ, FPBA_HZ);
    state_m.state = STATE_NAVIGATION_ENTRY_POINT;
    state_m.view_elt |= GUI_UPDATE_ELT_CONNECTED;
    break;
  case STATE_DEVICE_DISCONNECTED:
    controller_shutdown();
    state_m.state = STATE_IDLE_ENTRY_POINT;
    state_m.view_elt |= GUI_UPDATE_ELT_DISCONNECTED;
    break;
  case STATE_IDLE_ENTRY_POINT:
  case STATE_IDLE_WAIT_FOR_EVENT:
  case STATE_IDLE_DRIVE_LOAD:
    idle_task(&state_m);
    break;
  case STATE_NAVIGATION_ENTRY_POINT:
  case STATE_NAVIGATION_UPDATE_LIST:
  case STATE_NAVIGATION_UPDATE_LIST_GET_NAME:
  case STATE_NAVIGATION_UPDATE_LIST_STORE_NAME:
  case STATE_NAVIGATION_UPDATE_ISDIR:
  case STATE_NAVIGATION_WAIT_FOR_EVENT:
  case STATE_NAVIGATION_UPDATE_STATUS:
  case STATE_NAVIGATION_CD:
  case STATE_NAVIGATION_GOTOPARENT:
  case STATE_NAVIGATION_GOTOPARENT_ERROR_HANDLING:
  case STATE_NAVIGATION_PLAY_SELECTED_FILE:
  case STATE_NAVIGATION_WAIT_FOR_SELECTION:
  case STATE_NAVIGATION_UPDATE_METADATA_AND_PLAY:
    navigation_task(&state_m);
    break;
  case STATE_PLAYBACK_ENTRY_POINT:
  case STATE_PLAYBACK_WAIT_FOR_EVENT:
  case STATE_PLAYBACK_HANDLE_FAST_MODES:
  case STATE_PLAYBACK_UPDATE_TIME:
  case STATE_PLAYBACK_UPDATE_STATUS:
    playback_task(&state_m);
    break;
  case STATE_CONFIG_ENTRY_POINT:
  case STATE_CONFIG_WAIT_FOR_EVENT:
  case STATE_CONFIG_UPDATE_STATES:
  case STATE_CONFIG_READ_REPEAT_STATE:
  case STATE_CONFIG_READ_SHUFFLE_STATE:
    config_task(&state_m);
    break;
  case STATE_CHECK_DEVICE_ENTRY_POINT:
  case STATE_CHECK_DEVICE_UPDATE_STATUS:
    check_device_task(&state_m);
    break;
  case STATE_TRACK_CHANGED_ENTRY_POINT:
  case STATE_TRACK_CHANGED_TOTAL_TIME:
  case STATE_TRACK_CHANGED_FILE_NAME:
  case STATE_TRACK_CHANGED_ARTIST:
  case STATE_TRACK_CHANGED_TITLE:
  case STATE_TRACK_CHANGED_IMAGE:
  case STATE_TRACK_CHANGED_RESUME:
  case STATE_TRACK_CHECK_RESUME:
    track_changed_task(&state_m);
    break;
  case STATE_COMMAND_PLAY_ANY_SONG:
    command_task(&state_m);
    break;
  default:
    break;
  }
/*
  // Power sleep mode is managed here
  if( usb_device_get_state()==DEVICE_STATE_NOT_CONNECTED ) {
     if( cpu_is_timer_stopped(&sleep_timer) ) {
        cpu_set_timeout(cpu_ms_2_cy(SLEEP_MODE_MS, FCPU_HZ), &sleep_timer);
     }
     else if( cpu_is_timeout(&sleep_timer) ) {
        gui_enter_idle();
        SLEEP(AVR32_PM_SMODE_IDLE);
        gui_leave_idle();
     }
  } else {
     cpu_stop_timeout(&sleep_timer);
  }
*/
}

