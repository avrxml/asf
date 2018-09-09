/**************************************************************************
 *
 * \file
 *
 * \brief Joystick communication header file, containing the structure
 *        definition.
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
 ***************************************************************************/


/* This file simulate an audio player interface */
/* Its main goal is to give an example of using */
/* the audio interface.                         */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

