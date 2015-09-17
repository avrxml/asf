/**************************************************************************
 *
 * \file
 *
 * \brief Joystick communication header file, containing the structure
 *        definition.
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

typedef enum
{
  DEVICE_STATE_NOT_CONNECTED,
  DEVICE_STATE_PRESENT,
  DEVICE_STATE_READY,
  DEVICE_STATE_UNSUPPORTED,
  DEVICE_STATE_ERROR
} device_state_t;

enum com_state
{
  // General state
  STATE_INITIALIZATION,
  STATE_DEVICE_CONNECTED,
  STATE_DEVICE_DISCONNECTED,
  // Idle view states
  STATE_IDLE_ENTRY_POINT,
  STATE_IDLE_WAIT_FOR_EVENT,
  STATE_IDLE_DRIVE_LOAD,
  // Navigation view states
  STATE_NAVIGATION_ENTRY_POINT,
  STATE_NAVIGATION_UPDATE_LIST,
  STATE_NAVIGATION_UPDATE_LIST_GET_NAME,
  STATE_NAVIGATION_UPDATE_LIST_STORE_NAME,
  STATE_NAVIGATION_UPDATE_ISDIR,
  STATE_NAVIGATION_WAIT_FOR_EVENT,
  STATE_NAVIGATION_UPDATE_STATUS,
  STATE_NAVIGATION_CD,
  STATE_NAVIGATION_GOTOPARENT,
  STATE_NAVIGATION_GOTOPARENT_ERROR_HANDLING,
  STATE_NAVIGATION_PLAY_SELECTED_FILE,
  STATE_NAVIGATION_WAIT_FOR_SELECTION,
  STATE_NAVIGATION_UPDATE_METADATA_AND_PLAY,
  // Playback view states
  STATE_PLAYBACK_ENTRY_POINT,
  STATE_PLAYBACK_WAIT_FOR_EVENT,
  STATE_PLAYBACK_HANDLE_FAST_MODES,
  STATE_PLAYBACK_UPDATE_TIME,
  STATE_PLAYBACK_UPDATE_STATUS,
  // Configuration view states
  STATE_CONFIG_ENTRY_POINT,
  STATE_CONFIG_WAIT_FOR_EVENT,
  STATE_CONFIG_UPDATE_STATES,
  STATE_CONFIG_READ_REPEAT_STATE,
  STATE_CONFIG_READ_SHUFFLE_STATE,
  // Check device
  STATE_CHECK_DEVICE_ENTRY_POINT,
  STATE_CHECK_DEVICE_UPDATE_STATUS,
  // Track changed
  STATE_TRACK_CHANGED_ENTRY_POINT,
  STATE_TRACK_CHANGED_TOTAL_TIME,
  STATE_TRACK_CHANGED_FILE_NAME,
  STATE_TRACK_CHANGED_ARTIST,
  STATE_TRACK_CHANGED_TITLE,
  STATE_TRACK_CHANGED_IMAGE,
  STATE_TRACK_CHECK_RESUME,
  STATE_TRACK_CHANGED_RESUME,
  // Commands
  STATE_COMMAND_PLAY_ANY_SONG,
};

struct state_machine_context
{
  enum com_state state;
  enum com_state recorded_state;
  bool async_cmd;
  bool cmd_status;
  enum gui_update_view view;
  enum gui_update_elt view_elt;
  struct file_list list;
  struct display_file_list display_list;
  struct file_info info;
  struct player_info player_status;
  uint16_t cursor_pointer;
  t_cpu_time elapsed_time_timer;
  t_cpu_time in_progress_timer;
};

