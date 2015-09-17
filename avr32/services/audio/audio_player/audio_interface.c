/*****************************************************************************
 *
 * \file
 *
 * \brief Audio Interface Services.
 *
 * The Audio Interface (AI) is aimed at managing audio decoding, and file system
 * navigation.
 * When using the Audio Interface, the user does not have to directly interface with
 * File System API (provided in nav_auto.h, navigation.h, file.h, playlist.h, fat.h, ...).
 * This greatly simplifies software architecture.
 *
 * The Audio Interface can be used in 2 different ways:
 *
 * - Using "asynchronous" functions, i.e. which result/effect may
 *   not be produced immediately. Using these functions usually leads to the use of
 *   state-machines in the user firmware (since we must wait for the completion of
 *   a command before launching a new one), but has the advantage of reducing the
 *   risks of audio "blips".
 *
 * - Using "synchronous" functions, i.e. which are executed immediately. This drastically
 *   simplifies the user firmware architecture (we do not have to use state-machines since
 *   the synchronous AI functions are immediately executed) but *may* produce audio
 *   "blips" since the execution time of these functions can lead to audio underrun.
 *
 * Each function of the Audio Interface has a "synchronous" and "asynchronous"
 * interface. For example, the command which returns the number of drives is:
 * ai_async_nav_drive_nb() in the "asynchronous" interface.
 * ai_nav_drive_nb()       in the "synchronous" interface.
 *
 * Using "asynchronous" functions shall be the preferred solution in order to avoid
 * as much as possible audio "blips".
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


//_____  I N C L U D E S ___________________________________________________

#include <string.h>
#include "compiler.h"
#include "conf_audio_interface.h"

#include "audio_interface.h"

//_____ M A C R O S ________________________________________________________

#define AI_NONE         0
#define AI_USB_MS       1
#define AI_HID_CLASS    2
#define AI_SD_MMC       3

#if (defined USB_MS_INCLUDED) && (USB_MS_INCLUDED == true)
  #define AI_CALL_AI_USB_MS(macro) macro(AI_USB_MS)
#else
  #define AI_CALL_AI_USB_MS(macro)
#endif
#if (defined HID_CLASS_INCLUDED) && (HID_CLASS_INCLUDED == true)
  #define AI_CALL_AI_HID_CLASS(macro) macro(AI_HID_CLASS)
#else
  #define AI_CALL_AI_HID_CLASS(macro)
#endif
#if (defined SD_MMC_INCLUDED) && (SD_MMC_INCLUDED == true)
  #define AI_CALL_AI_SD_MMC(macro) macro(AI_SD_MMC)
#else
  #define AI_CALL_AI_SD_MMC(macro)
#endif

#define AI_CALL(fct_set, macro) AI_CALL_##fct_set(macro);

// These macros define the priority order of the AI modules
#define AI_PRIORITY0(macro)
#define AI_PRIORITY1(macro) AI_PRIORITY0(macro) AI_CALL(AI_USB_MS, macro)
#define AI_PRIORITY2(macro) AI_PRIORITY1(macro) AI_CALL(AI_HID_CLASS, macro)
#define AI_PRIORITY3(macro) AI_PRIORITY2(macro) AI_CALL(AI_SD_MMC, macro)

#define AI_PRIORITY_CALL(macro) AI_PRIORITY3(macro)

//_____ D E F I N I T I O N S ______________________________________________

static   cmd_ai_id_t g_ai_cmdid;
static   uint32_t   g_ai_u32_in;
static   uint8_t    g_ai_status;
static   uint32_t   g_ai_u32_out;
static   uint64_t   g_ai_u64_out;
static   uint16_t   g_ai_u16_arg_size;
static   char* g_ai_char_arg;

#ifdef AI_ASYNC_STACK_NB_ELEMENTS
static ai_async_context_t ai_context_bp[AI_ASYNC_STACK_NB_ELEMENTS];
static ai_async_context_t *ai_context_sp;
#endif

uint8_t       g_retry_cnt;

volatile bool  g_b_ai_cmd_received;

#define AI_CMD_RETRY_RELOAD        3

#if (!defined(AI_SYNC_TASK_CALL_BACK))
#  define AI_SYNC_TASK_CALL_BACK()
#endif

ai_async_status_t g_ai_cmd_execution_status;

//_____ D E C L A R A T I O N S ____________________________________________


bool  ai_read_string             ( bool (*fcnt_read_string)(char*, uint8_t), char** string, uint16_t *size_string );
uint16_t   ai_song_read_info          ( uint16_t (*fcnt_song_read_info)(char**), char** string );

// Handle specific commands
static bool ai_specific_async_cmd_task(cmd_ai_id_t cmd_id, uint32_t u32_in, uint16_t *u16_arg_size, char **u8_arg_in, uint32_t *u32_out, uint64_t *u64_out);

#if (!defined USB_MS_INCLUDED) && (!defined HID_CLASS_INCLUDED) && (!defined SD_MMC_INCLUDED)
#error These should be defined to something!
#endif
#if (defined USB_MS_INCLUDED) && (USB_MS_INCLUDED == false) \
  && (defined HID_CLASS_INCLUDED) && (HID_CLASS_INCLUDED == false) \
  && (defined SD_MMC_INCLUDED) && (SD_MMC_INCLUDED == false)
#error Please define at least one of (USB_MS_INCLUDED, HID_CLASS_INCLUDED, SD_MMC_INCLUDED)
#endif

const ai_fct_set *ai_fct_set_tab[4];

uint8_t current_ai_fct_set = AI_NONE;

// Default entry
const ai_fct_set ai_default_fct_set =
{
  ai_init_null,
  ai_get_device_status_null,
  ai_get_product_id_null,
  ai_get_vendor_id_null,
  ai_get_serial_number_null,
  ai_nav_drive_nb_null,
  ai_nav_drive_set_null,
  ai_nav_drive_get_null,
  ai_nav_drive_mount_null,
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
  ai_audio_nav_next_null,
  ai_audio_nav_previous_null,
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
  ai_audio_ctrl_resume_null,
  ai_audio_ctrl_pause_null,
  ai_audio_ctrl_time_null,
  ai_audio_ctrl_status_null,
  ai_audio_ctrl_ffw_null,
  ai_audio_ctrl_frw_null,
  ai_audio_ctrl_start_ffw_null,
  ai_audio_ctrl_start_frw_null,
  ai_audio_ctrl_stop_ffw_frw_null,
  ai_specific_async_cmd_task_null
};

//! Initialize the audio interface
//!
void  ai_init( void )
{
  // Default entry
  ai_fct_set_tab[AI_NONE] = &ai_default_fct_set;

#if (defined USB_MS_INCLUDED) && (USB_MS_INCLUDED == true)
  extern const ai_fct_set ai_usb_ms_fct_set;
  ai_fct_set_tab[AI_USB_MS] = &ai_usb_ms_fct_set;
  ai_fct_set_tab[AI_USB_MS]->init();
#endif
#if (defined HID_CLASS_INCLUDED) && (HID_CLASS_INCLUDED == true)
  extern const ai_fct_set ai_hid_class_fct_set;
  ai_fct_set_tab[AI_HID_CLASS] = &ai_hid_class_fct_set;
  ai_fct_set_tab[AI_HID_CLASS]->init();
#endif
#if (defined SD_MMC_INCLUDED) && (SD_MMC_INCLUDED == true)
  extern const ai_fct_set ai_sd_mmc_fct_set;
  ai_fct_set_tab[AI_SD_MMC] = &ai_sd_mmc_fct_set;
  ai_fct_set_tab[AI_SD_MMC]->init();
#endif

  g_b_ai_cmd_received = false;
  g_ai_status         = true;        // By default command OK

#ifdef AI_ASYNC_STACK_NB_ELEMENTS
  // Set the context stack pointer at the bottom of the stack
  ai_context_sp = ai_context_bp + AI_ASYNC_STACK_NB_ELEMENTS;
#endif
}

#define AI_TASK_SELECT_FCT_SET(cur_fct_set) \
  if (ai_fct_set_tab[cur_fct_set]->get_device_status(&status) == AI_DEVICE_STATUS_CONNECTED) \
  { \
    current_ai_fct_set = cur_fct_set; \
    return; \
  }

void  ai_task(void)
{
  ai_async_status_t status;

  // By default, no connection
  current_ai_fct_set = AI_NONE;

  AI_PRIORITY_CALL(AI_TASK_SELECT_FCT_SET);
}

bool  ai_is_none(void)
{
  return ( current_ai_fct_set==AI_NONE ? true : false );
}

bool  ai_is_usb_ms(void)
{
  return ( current_ai_fct_set==AI_USB_MS ? true : false );
}

bool  ai_is_hid_class(void)
{
  return ( current_ai_fct_set==AI_HID_CLASS ? true : false );
}

bool  ai_is_sd_mmc(void)
{
  return ( current_ai_fct_set==AI_SD_MMC ? true : false );
}

#define AI_GET_DEVICE_STATUS(fct_set) { \
  ai_device_status_t device_status; \
  ai_async_status_t async_status; \
  device_status = ai_fct_set_tab[fct_set]->get_device_status(&async_status); \
  if (device_status != AI_DEVICE_STATUS_NOT_PRESENT) \
    return device_status; \
}

ai_device_status_t ai_get_device_status(void)
{
  AI_PRIORITY_CALL(AI_GET_DEVICE_STATUS);
  return AI_DEVICE_STATUS_NOT_PRESENT;
}

//!
//!-------- ASYNCHRONOUS routines --------
//! note: to use these routines, you must call specific routine to know the end of execution and take the result
//!

void ai_async_cmd_task( void )
{
  St_ai_info_version ver;

  if ( !g_b_ai_cmd_received )
    return;

  g_ai_u32_out = 0;           // By default result uint32_t is NULL
  g_ai_u64_out = 0;           // By default result uint64_t is NULL

  if( ai_fct_set_tab[current_ai_fct_set]==NULL )
  {
    g_ai_cmd_execution_status = CMD_ERROR;
    g_ai_status               = false;
    g_b_ai_cmd_received       = false;
    return;
  }
  else
  {
    g_ai_cmd_execution_status = CMD_EXECUTING;
    g_ai_status               = true;        // By default command OK
  }

  switch ( g_ai_cmdid )
  {
  case CMD_AI_GET_DEVICE_STATUS    :
    g_ai_u32_out = (uint32_t) ai_fct_set_tab[current_ai_fct_set]->get_device_status(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_GET_PRODUCT_ID         :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->get_product_id(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_GET_VENDOR_ID          :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->get_vendor_id(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_GET_SERIAL_NUMBER      :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->get_serial_number(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DRIVE_NB           :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_drive_nb(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DRIVE_SET          :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_drive_set(g_ai_u32_in, &g_ai_cmd_execution_status );
    break;
  case CMD_AI_NAV_DRIVE_GET          :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_drive_get(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DRIVE_MOUNT        :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_drive_mount(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DRIVE_TOTAL_SPACE  :
    g_ai_u64_out = ai_fct_set_tab[current_ai_fct_set]->nav_drive_total_space(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DRIVE_FREE_SPACE   :
    g_ai_u64_out = ai_fct_set_tab[current_ai_fct_set]->nav_drive_free_space(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DIR_ROOT           :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_dir_root(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DIR_CD             :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_dir_cd(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DIR_GOTOPARENT     :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_dir_gotoparent(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DIR_NAME           :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->nav_dir_name( &g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_FILE_NAME          :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->nav_file_name( &g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_FILE_ISDIR         :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_isdir(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_FILE_NEXT          :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_file_next(&g_ai_cmd_execution_status );
    break;
  case CMD_AI_NAV_FILE_PREVIOUS      :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_file_previous(&g_ai_cmd_execution_status );
    break;
  case CMD_AI_NAV_FILE_GOTO          :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_file_goto(g_ai_u32_in, &g_ai_cmd_execution_status );
    break;
  case CMD_AI_NAV_FILE_POS           :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_pos(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_FILE_NB            :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_nb(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_DIR_NB             :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_dir_nb(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_PLAYLIST_NB        :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_playlist_nb(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_GETPLAYFILE        :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->nav_getplayfile(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_TYPE          :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_type(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_VERSION       :
    ver = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_version(&g_ai_cmd_execution_status);
    g_ai_u32_out = ver.digit.major << 16 |
                   ver.digit.minor << 8 |
                   ver.digit.revision;
    break;
  case CMD_AI_NAV_INFO_TITLE         :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_title(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_ARTIST        :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_artist(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_ALBUM         :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_album(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_YEAR          :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_year(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_TRACK         :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_track(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_GENRE         :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_genre(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_DURATION      :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->nav_file_info_duration(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_NAV_INFO_IMAGE         :
    g_ai_u32_out = (uint32_t) ai_fct_set_tab[current_ai_fct_set]->nav_file_info_image((ai_image_size_t *) g_ai_u32_in, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CONTEXT_SAVE     :
    g_ai_char_arg = (char*) ai_fct_set_tab[current_ai_fct_set]->audio_context_save(&g_ai_status, &g_ai_u16_arg_size, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CONTEXT_RESTORE  :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_context_restore((void*) g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_PLAYFILE     :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_nav_playfile(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_NEXT         :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_nav_next(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_PREV         :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_nav_previous(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_EOF_OCCUR    :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_nav_eof_occur(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_NB           :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_nb(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_GETPOS       :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_getpos(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_SETPOS       :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_setpos(g_ai_u32_in, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_GETNAME      :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->audio_nav_getname( &g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_TYPE    :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_type(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_VERSION :
    ver = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_version(&g_ai_cmd_execution_status);
    g_ai_u32_out = ver.digit.major << 16 |
                   ver.digit.minor << 8 |
                   ver.digit.revision;
    break;
  case CMD_AI_AUDIO_NAV_INFO_TITLE   :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_title(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_ARTIST  :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_artist(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_ALBUM   :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_album(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_YEAR    :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_year(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_TRACK   :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_track(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_GENRE   :
    g_ai_u16_arg_size = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_genre(&g_ai_char_arg, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_DURATION:
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_duration(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_INFO_IMAGE   :
    g_ai_u32_out = (uint32_t) ai_fct_set_tab[current_ai_fct_set]->audio_nav_file_info_image((ai_image_size_t *) g_ai_u32_in, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_REPEAT_GET   :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_repeat_get(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_REPEAT_SET   :
    ai_fct_set_tab[current_ai_fct_set]->audio_nav_repeat_set(g_ai_u32_in, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_EXPMODE_GET  :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_expmode_get(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_EXPMODE_SET  :
    ai_fct_set_tab[current_ai_fct_set]->audio_nav_expmode_set( g_ai_u32_in, &g_ai_cmd_execution_status );
    break;
  case CMD_AI_AUDIO_NAV_SHUFFLE_GET  :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_nav_shuffle_get(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_NAV_SHUFFLE_SET  :
    ai_fct_set_tab[current_ai_fct_set]->audio_nav_shuffle_set( g_ai_u32_in, &g_ai_cmd_execution_status );
    break;
  case CMD_AI_AUDIO_CTRL_STOP        :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_stop(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_RESUME      :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_resume(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_PAUSE       :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_pause(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_TIME        :
    g_ai_u32_out = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_time(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_STATUS      :
    g_ai_u32_out = (uint32_t)(ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_status(&g_ai_cmd_execution_status)).all;
    break;
  case CMD_AI_AUDIO_CTRL_FFW         :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_ffw(g_ai_u32_in, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_FRW         :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_frw(g_ai_u32_in, &g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_START_FFW   :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_start_ffw(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_START_FRW   :
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_start_frw(&g_ai_cmd_execution_status);
    break;
  case CMD_AI_AUDIO_CTRL_STOP_FFW_FRW:
    g_ai_status = ai_fct_set_tab[current_ai_fct_set]->audio_ctrl_stop_ffw_frw(&g_ai_cmd_execution_status);
    break;
  default:
    // Specific commands
    g_ai_status = ai_specific_async_cmd_task(g_ai_cmdid,
                                             g_ai_u32_in,
                                             &g_ai_u16_arg_size,
                                             &g_ai_char_arg,
                                             &g_ai_u32_out,
                                             &g_ai_u64_out);
  }

  if (g_ai_cmd_execution_status == CMD_EXECUTING) return;

#ifdef AI_ASYNC_STACK_NB_ELEMENTS
  // Set the context stack pointer at the bottom of the stack
  ai_context_sp = ai_context_bp + AI_ASYNC_STACK_NB_ELEMENTS;
#endif

  // Make response to send
  g_b_ai_cmd_received = false;
}

bool ai_async_context_push(ai_async_context_t *ctx)
{
#ifdef AI_ASYNC_STACK_NB_ELEMENTS
  if (ai_context_sp == ai_context_bp)
    return false;

  ai_context_sp--;
  memcpy(ai_context_sp, ctx, sizeof(ai_async_context_t));
  return true;
#else
  return false;
#endif
}

bool ai_async_context_pop(ai_async_context_t *ctx)
{
#ifdef AI_ASYNC_STACK_NB_ELEMENTS
  if (ai_context_sp == ai_context_bp + AI_ASYNC_STACK_NB_ELEMENTS)
    return false;

  memcpy(ctx, ai_context_sp, sizeof(ai_async_context_t));
  ai_context_sp++;
  return true;
#else
  return false;
#endif
}

void ai_async_update_cmd_status(ai_async_status_t *cmd_ai_status)
{
#ifdef AI_ASYNC_STACK_NB_ELEMENTS
  ai_async_context_t ctx;

  // If no elements on the stack, then don't do anything
  if (ai_context_sp == ai_context_bp + AI_ASYNC_STACK_NB_ELEMENTS)
    return;
  // If an error has been previously raised, then keep it
  if (*cmd_ai_status == CMD_ERROR)
    return;

  // Else read the current context
  memcpy(&ctx, ai_context_sp, sizeof(ai_async_context_t));

  // Update the status
  if (ctx.status == CMD_ERROR)
    *cmd_ai_status = CMD_ERROR;
  else if (*cmd_ai_status == CMD_DONE)
    *cmd_ai_status = ctx.status;
#endif
}

// Abort the current command
void ai_command_abort(void)
{
  if( g_b_ai_cmd_received ) {
      g_ai_cmd_execution_status = CMD_ERROR;
      g_ai_status               = false;
      g_b_ai_cmd_received       = false;
  }
}

bool  is_ai_async_cmd_finished( void )
{
  return (!g_b_ai_cmd_received);
}

uint8_t    ai_async_cmd_out_status( void )
{
  if (g_ai_cmd_execution_status == CMD_ERROR)
    return (uint8_t) false;
  return g_ai_status;
}

uint32_t   ai_async_cmd_out_u32( void )
{
  return g_ai_u32_out;
}

uint64_t   ai_async_cmd_out_u64( void )
{
  return g_ai_u64_out;
}

void  ai_async_cmd_out_free_ArrayU8( void )
{
  if (g_ai_u16_arg_size != 0)
  {
    free( g_ai_char_arg );
    g_ai_char_arg = NULL;
    g_ai_u16_arg_size = 0;
  }
}

uint16_t   ai_async_cmd_out_SizeArrayU8( void )
{
  return g_ai_u16_arg_size;
}

char*   ai_async_cmd_out_PtrArrayU8( void )
{
  return g_ai_char_arg;
}

//! This routine may be used to send directly a command values from a communication port to audio interface
//! @note you can use the ai_async_cmd_put() routine or ai_async_X() routines to send an asynchronous command
bool ai_async_cmd_put(cmd_ai_id_t cmd_id, uint32_t u32_in, uint16_t u16_arg_size, char* u8_arg_in, bool need_extra_buffer)
{
  if ( g_b_ai_cmd_received )
    return false;
  g_ai_cmdid         = cmd_id;
  g_ai_u32_in        = u32_in;
  if (need_extra_buffer)
  {
    ai_async_cmd_out_free_ArrayU8();
    g_ai_u16_arg_size  = u16_arg_size;
    g_ai_char_arg      = u8_arg_in;
  }
  g_b_ai_cmd_received = true;
  g_retry_cnt         = AI_CMD_RETRY_RELOAD;
  return true;
}

static bool ai_specific_async_cmd_task(cmd_ai_id_t cmd_id, uint32_t u32_in, uint16_t *u16_arg_size, char **u8_arg_in, uint32_t *u32_out, uint64_t *u64_out)
{
  return ai_fct_set_tab[current_ai_fct_set]->specific_async_cmd_task(cmd_id, u32_in, u16_arg_size, u8_arg_in, u32_out, u64_out, &g_ai_cmd_execution_status);
}



//!
//!-------- SYNCHRONOUS routines --------
//!

#define AI_SYNC_FUNC_RET_STRUCT( ret_type, func_name, arg_type, arg_val, err_ret )   \
ret_type ai_##func_name ( arg_type arg_val )   \
{                                              \
  ret_type var;                                \
  var.all=(err_ret);                           \
  if( !ai_async_##func_name(arg_val) )         \
    return var;                                \
                                               \
  do                                           \
  {                                            \
    AI_SYNC_TASK_CALL_BACK();                  \
    ai_async_cmd_task();                       \
  }while( !is_ai_async_cmd_finished() );       \
                                               \
  var.all=ai_async_cmd_out_u32();              \
  return var;                                  \
}

#define AI_SYNC_FUNC_RET_VALUE( ret_type, func_name, arg_type, arg_val, err_ret )   \
ret_type ai_##func_name ( arg_type arg_val )   \
{                                              \
  if( !ai_async_##func_name(arg_val) )         \
    return (err_ret);                          \
                                               \
  do                                           \
  {                                            \
    AI_SYNC_TASK_CALL_BACK();                  \
    ai_async_cmd_task();                       \
  }while( !is_ai_async_cmd_finished() );       \
                                               \
  return (ret_type)ai_async_cmd_out_u32();     \
}

#define AI_SYNC_FUNC_RET_VOID( func_name, arg_type, arg_val  ) \
void ai_##func_name ( arg_type arg_val )       \
{                                              \
  if( !ai_async_##func_name(arg_val) )         \
    return ;                                   \
                                               \
  do                                           \
  {                                            \
    AI_SYNC_TASK_CALL_BACK();                  \
    ai_async_cmd_task();                       \
  }while( !is_ai_async_cmd_finished() );       \
}

#define AI_SYNC_FUNC_MALLOC( ret_type, func_name, pp_arg, err_ret )   \
ret_type ai_##func_name ( char** pp_arg )          \
{                                                  \
  if( !ai_async_##func_name() )                    \
    return (err_ret);                              \
                                                   \
  do                                               \
  {                                                \
    AI_SYNC_TASK_CALL_BACK();                      \
    ai_async_cmd_task();                           \
  }while( !is_ai_async_cmd_finished() );           \
                                                   \
  *pp_arg= ai_async_cmd_out_PtrArrayU8();          \
  return (ret_type)ai_async_cmd_out_SizeArrayU8(); \
}

//                      Return type            Func name              arg type  arg     error value
AI_SYNC_FUNC_RET_VALUE( uint16_t,              get_product_id,        void,             , 0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              get_vendor_id,         void,, 0);

//                      Return type            Func name              string name       error value
AI_SYNC_FUNC_MALLOC(    uint16_t,              get_serial_number,     pp_sz_sn, 0 );


// File navigation
// ===============
//                      Return type            Func name              arg type  arg       error value
AI_SYNC_FUNC_RET_VALUE( uint8_t,               nav_drive_nb,          void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_drive_set,         uint8_t,  u8_number , 0);
AI_SYNC_FUNC_RET_VALUE( uint8_t,               nav_drive_get,         void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_drive_mount,       void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint64_t,              nav_drive_total_space, void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint64_t,              nav_drive_free_space,  void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_dir_root,          void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_dir_cd,            void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_dir_gotoparent,    void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_file_isdir,        void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_file_next,         void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_file_previous,     void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_file_goto,         uint32_t, u32_newpos, 0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              nav_file_pos,          void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              nav_file_nb,           void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              nav_dir_nb,            void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              nav_playlist_nb,       void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint32_t,              nav_file_info_type,    void,     ,           0);
AI_SYNC_FUNC_RET_STRUCT(St_ai_info_version,    nav_file_info_version, void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              nav_file_info_year,    void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint32_t,              nav_file_info_track,   void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( uint32_t,              nav_file_info_duration,void,     ,           0);
AI_SYNC_FUNC_RET_VALUE( bool,                  nav_getplayfile,       void,     ,           0);

//                      Return type            Func name              string name       error value
AI_SYNC_FUNC_MALLOC(    uint16_t,              nav_dir_name,          ppstring,         0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              nav_file_name,         ppstring,         0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              nav_file_info_title,   ppsz_title,       0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              nav_file_info_artist,  ppsz_artist,      0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              nav_file_info_album,   ppsz_album,       0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              nav_file_info_genre,   ppsz_genre,       0);



// Audio navigation
// ================
//                      Return type            Func name              arg type  arg       error value
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_nav_playfile,    void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_context_restore, void*,    ctx,      0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_nav_next,        void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_nav_previous,    void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_nav_eof_occur,   void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              audio_nav_nb,          void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              audio_nav_getpos,      void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_nav_setpos,      uint16_t, u16_pos,  0);
AI_SYNC_FUNC_RET_VALUE( Ai_repeat_mode,        audio_nav_repeat_get,  void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( Ai_shuffle_mode,       audio_nav_shuffle_get, void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( Ai_explorer_mode,      audio_nav_expmode_get, void,     ,         0);

//                      Func name              arg type  arg       error value
AI_SYNC_FUNC_RET_VOID(  audio_nav_repeat_set,  Ai_repeat_mode,     repeat_mode);
AI_SYNC_FUNC_RET_VOID(  audio_nav_shuffle_set, Ai_shuffle_mode,    shuffle );
AI_SYNC_FUNC_RET_VOID(  audio_nav_expmode_set, Ai_explorer_mode,   exp_mode );



//                      Return type            Func name                     arg type  arg       error value
AI_SYNC_FUNC_RET_VALUE( uint32_t,              audio_nav_file_info_type,     void,     ,         0);
AI_SYNC_FUNC_RET_STRUCT(St_ai_info_version,    audio_nav_file_info_version,  void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( uint16_t,              audio_nav_file_info_year,     void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( uint32_t,              audio_nav_file_info_track,    void,     ,         0);
AI_SYNC_FUNC_RET_VALUE( uint32_t,              audio_nav_file_info_duration, void,     ,         0);

//                      Return type            Func name                    string name       error value
AI_SYNC_FUNC_MALLOC(    uint16_t,              audio_nav_getname,           ppstring,         0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              audio_nav_file_info_title,   ppsz_title,       0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              audio_nav_file_info_artist,  ppsz_artist,      0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              audio_nav_file_info_album,   ppsz_album,       0);
AI_SYNC_FUNC_MALLOC(    uint16_t,              audio_nav_file_info_genre,   ppsz_genre,       0);



// Audio control
// =============
//                      Return type            Func name                arg type  arg             error value
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_stop,         void,     ,               0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_resume,       void,     ,               0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_pause,        void,     ,               0);
AI_SYNC_FUNC_RET_VALUE( uint32_t,              audio_ctrl_time,         void,     ,               0);
AI_SYNC_FUNC_RET_STRUCT(Ai_player_flag_t,      audio_ctrl_status,       void,     ,               0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_ffw,          uint32_t, u32_duration,   0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_frw,          uint32_t, u32_duration,   0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_start_ffw,    void,     ,               0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_start_frw,    void,     ,               0);
AI_SYNC_FUNC_RET_VALUE( bool,                  audio_ctrl_stop_ffw_frw, void,     ,               0);

void *ai_audio_context_save( uint8_t *status, uint16_t *size )
{
  if( !ai_async_audio_context_save() )
  {
    *status = false;
    return NULL;
  }

  do
  {
    AI_SYNC_TASK_CALL_BACK();
    ai_async_cmd_task();
  }while( !is_ai_async_cmd_finished() );

  *status = ai_async_cmd_out_status();
  *size   = ai_async_cmd_out_SizeArrayU8();
  return (void *)ai_async_cmd_out_PtrArrayU8();
}

void *ai_nav_file_info_image(ai_image_size_t *size)
{
  if (!ai_async_nav_file_info_image(size))
    return NULL;

  do
  {
    AI_SYNC_TASK_CALL_BACK();
    ai_async_cmd_task();
  } while( !is_ai_async_cmd_finished() );

  return (void *) ai_async_cmd_out_u32();
}

void *ai_audio_nav_file_info_image(ai_image_size_t *size)
{
  if (!ai_async_audio_nav_file_info_image(size))
    return NULL;

  do
  {
    AI_SYNC_TASK_CALL_BACK();
    ai_async_cmd_task();
  } while( !is_ai_async_cmd_finished() );

  return (void *) ai_async_cmd_out_u32();
}

