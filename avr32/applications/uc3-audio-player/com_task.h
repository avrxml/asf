/**************************************************************************
 *
 * \file
 *
 * \brief uShell command line interpreter.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _COM_TASK_H_
#define _COM_TASK_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_audio_player.h"
#include "audio_interface.h"

#define MAX_BUFFER_FILE   10
#define MAX_DISPLAY_FILE  10
#define STR_MAX_LENGTH    24

enum file_type
{
  FILE_TYPE_DIRECTORY,
  FILE_TYPE_FILE,
  FILE_TYPE_PLAYLIST
};

struct file_elt
{
  // File name.
  uint8_t file_name[STR_MAX_LENGTH];
  // Defines if the type of the file selected.
  enum file_type type;
  // This flag tells the GUI if this element has been updated.
  bool updated;
};

struct file_list
{
  // File list.
  struct file_elt list[MAX_BUFFER_FILE];
  // File position of the 1rst element of the list.
  uint16_t file_pos;
  // The umber of valid entries on this list.
  uint16_t nb_valid_entries;
  // Number of files in the directory. This number is not always right and
  // it is calculated when the list has been fully explored.
  uint16_t nb_files;
};

struct display_file_list
{
  struct file_elt *list;
  union
  {
    uint16_t nb_elts;
    // Cursor selection of the mode configuration.
    enum
    {
      MODE_POS_REPEAT,
      MODE_POS_SHUFFLE
    } mode_pos;
  };
};

struct file_info
{
  uint32_t total_time;
  uint32_t elapsed_time;
  uint16_t file_pos;
  uint8_t volume;
  uint8_t name[STR_MAX_LENGTH];
  uint8_t artist[STR_MAX_LENGTH];
  uint8_t title[STR_MAX_LENGTH];
  ai_image_size_t size;
  void *image_data;
};

struct player_info
{
  Ai_player_flag_t flags;
  Ai_repeat_mode repeat;
  Ai_shuffle_mode shuffle;
};

void com_task_init(void);
void com_task(void);

#endif  // _COM_TASK_H_
