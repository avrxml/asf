/**************************************************************************
 *
 * \file
 *
 * \brief uShell command line interpreter.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
