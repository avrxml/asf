/**
 * \file
 *
 * \brief Audio interface USB Mass Storage Services.
 *
 * This function should be connected to the audio interface to give access to the mass storage audio player
 * If you use this routines then you don't must use the other FileSystem routine provide in nav_auto.h, navigation.h, file.h, playlist.h
 *
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_audio_player.h"
#include "conf_explorer.h"
#include LIB_CTRLACCESS
#include LIB_MEM
#include "file.h"
#include "navigation.h"
#include "play_list.h"
#ifdef NAVIGATION_MODE_FLAT
#include "nav_filterflat.h"
#else
#include "nav_filterlist.h"
#endif
#include "nav_automatic.h"
#include "ai_usb_ms.h"

#if (FS_NB_NAVIGATOR < 3)
#error To use the audio interface, the define FS_NB_NAVIGATOR in conf_explorer.h must be > 2
#endif
#if (FS_NAV_ID_PLAYLIST != 2)
#error To use the audio interface, the define FS_NAV_ID_PLAYLIST in conf_explorer.h must be 2
#endif

#define EX_RETRY_COUNT    1  // 0 means infinite. 1 to n: process by iteration of EX_RETRY_COUNT

#if !defined(AI_ASYNC_STACK_NB_ELEMENTS) || AI_ASYNC_STACK_NB_ELEMENTS < 1
#error This version of the ai_usb_ms.c file needs a context that can support \
at least 1 element. Therefore, AI_ASYNC_STACK_NB_ELEMENTS must be defined with \
a value greater or equals to 1.
#endif

#if defined(SUPPORT_PLAYLISTS) && SUPPORT_PLAYLISTS == false
  #define PLAYLIST_FILE_EXT   ""
#else
  #define PLAYLIST_FILE_EXT   PL_FILE_EXT
#endif

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

const static struct
{
  const char *file_ext;
  void (*init)(void);
  void (*set_play_file_notification)(void);
  void (*clear_play_file_notification)(void);
  bool (*is_file_playing)(void);
  bool (*is_audio_file)(void);
  bool (*is_metadata_compatible_file)(void);
  St_ai_info_version (*read_metadata_version)(void);
  bool (*read_metadata_title)(FS_STRING, uint8_t);
  bool (*read_metadata_artist)(FS_STRING, uint8_t);
  bool (*read_metadata_album)(FS_STRING, uint8_t);
  uint16_t (*read_metadata_year)(void);
  uint32_t (*read_metadata_track)(void);
  bool (*read_metadata_genre)(FS_STRING, uint8_t);
  uint32_t (*read_metadata_duration)(void);
  void *(*read_metadata_image)(ai_image_size_t *);
} file_format_supported[] = SUPPORTED_FILE_FORMATS;

static uint8_t    g_Cmd_state;
static bool  ai_usb_ms_audio_nav_mov( bool b_direction, ai_async_status_t *cmd_ai_status  );
bool  ai_usb_ms_read_string( bool (*fcnt_read_string)(FS_STRING, uint8_t), char** string, uint16_t* size_string );
uint16_t   ai_usb_ms_song_read_info( uint16_t (*fcnt_song_read_info)(FS_STRING*, ai_async_status_t *), char** string, ai_async_status_t * cmd_ai_status );

char *ai_usb_ms_filter_file = NULL;
char *ai_usb_ms_filter_songfile = NULL;
char *ai_usb_ms_filter_playlist = NULL;

bool g_player_flag_error;
uint32_t  g_player_duration;
uint32_t  g_player_currentTime_ms;
uint32_t  g_player_target_file_pos;
uint8_t   g_smooth_com_cnt;
uint16_t  g_u16_count;

Ai_player_flag_t g_player_flag;

Ai_repeat_mode  g_ai_usb_ms_repeat_mode   = AI_DEFAULT_REPEAT_MODE;    //!< repeat mode used
Ai_shuffle_mode g_ai_usb_ms_shuffle_mode  = AI_DEFAULT_SHUFFLE_MODE;   //!< shuffle mode used
bool            g_ai_usb_ms_b_audio_file = false;                //!< true if a file is selected in audio navigator
bool            g_b_stop_player   = false;

static bool b_previous_direction = FS_FIND_NEXT;

//_____ D E C L A R A T I O N S ____________________________________________

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true
void cache_flush(void);
#endif

#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true
static void *alloc_context(size_t len);
static void free_context(void);
static void *g_context = NULL;

static void free_context(void)
{
  if (g_context)
  {
    free(g_context);
    g_context = NULL;
  }
}

static void *alloc_context(size_t len)
{
  if (g_context)
    free(g_context);
  g_context = malloc(len);
  return g_context;
}
#endif

static bool ai_usb_ms_specific_async_cmd_task(cmd_ai_id_t cmd_id, uint32_t u32_in, uint16_t *u16_arg_size, char **u8_arg_in, uint32_t *u32_out, uint64_t *u64_out, ai_async_status_t *cmd_ai_status);

const ai_fct_set ai_usb_ms_fct_set =
{
  ai_usb_ms_init,
  ai_usb_ms_get_device_status,
  ai_usb_ms_get_product_id,
  ai_usb_ms_get_vendor_id,
  ai_usb_ms_get_serial_number,
  ai_usb_ms_nav_drive_nb,
  ai_usb_ms_nav_drive_set,
  ai_usb_ms_nav_drive_get,
  ai_usb_ms_nav_drive_mount,
  ai_usb_ms_nav_drive_total_space,
  ai_usb_ms_nav_drive_free_space,
  ai_usb_ms_nav_dir_root,
  ai_usb_ms_nav_dir_cd,
  ai_usb_ms_nav_dir_gotoparent,
  ai_usb_ms_nav_file_isdir,
  ai_usb_ms_nav_file_next,
  ai_usb_ms_nav_file_previous,
  ai_usb_ms_nav_file_goto,
  ai_usb_ms_nav_file_pos,
  ai_usb_ms_nav_file_nb,
  ai_usb_ms_nav_dir_nb,
  ai_usb_ms_nav_playlist_nb,
  ai_usb_ms_nav_dir_name,
  ai_usb_ms_nav_file_name,
  ai_usb_ms_nav_file_info_type,
  ai_usb_ms_nav_file_info_version,
  ai_usb_ms_nav_file_info_title,
  ai_usb_ms_nav_file_info_artist,
  ai_usb_ms_nav_file_info_album,
  ai_usb_ms_nav_file_info_year,
  ai_usb_ms_nav_file_info_track,
  ai_usb_ms_nav_file_info_genre,
  ai_usb_ms_nav_file_info_duration,
  ai_usb_ms_nav_file_info_image,
  ai_usb_ms_nav_getplayfile,
  ai_usb_ms_audio_nav_playfile,
  ai_usb_ms_audio_context_save,
  ai_usb_ms_audio_context_restore,
  ai_usb_ms_audio_nav_next,
  ai_usb_ms_audio_nav_previous,
  ai_usb_ms_audio_nav_eof_occur,
  ai_usb_ms_audio_nav_nb,
  ai_usb_ms_audio_nav_getpos,
  ai_usb_ms_audio_nav_setpos,
  ai_usb_ms_audio_nav_repeat_get,
  ai_usb_ms_audio_nav_repeat_set,
  ai_usb_ms_audio_nav_expmode_get,
  ai_usb_ms_audio_nav_expmode_set,
  ai_usb_ms_audio_nav_shuffle_get,
  ai_usb_ms_audio_nav_shuffle_set,
  ai_usb_ms_audio_nav_getname,
  ai_usb_ms_audio_nav_file_info_type,
  ai_usb_ms_audio_nav_file_info_version,
  ai_usb_ms_audio_nav_file_info_title,
  ai_usb_ms_audio_nav_file_info_artist,
  ai_usb_ms_audio_nav_file_info_album,
  ai_usb_ms_audio_nav_file_info_year,
  ai_usb_ms_audio_nav_file_info_track,
  ai_usb_ms_audio_nav_file_info_genre,
  ai_usb_ms_audio_nav_file_info_duration,
  ai_usb_ms_audio_nav_file_info_image,
  ai_usb_ms_audio_ctrl_stop,
  ai_usb_ms_audio_ctrl_resume,
  ai_usb_ms_audio_ctrl_pause,
  ai_usb_ms_audio_ctrl_time,
  ai_usb_ms_audio_ctrl_status,
  ai_usb_ms_audio_ctrl_ffw,
  ai_usb_ms_audio_ctrl_frw,
  ai_usb_ms_audio_ctrl_start_ffw,
  ai_usb_ms_audio_ctrl_start_frw,
  ai_usb_ms_audio_ctrl_stop_ffw_frw,
  ai_usb_ms_specific_async_cmd_task
};

void  ai_usb_ms_init( void )
{
  int i;
  size_t song_file_list_size;
  size_t playlist_file_list_size;
  size_t nb_file_format_supported = sizeof(file_format_supported) / sizeof(file_format_supported[0]);

  // Do it only once - useful if another module uses this function (like SD/MMC card support for example)
  if (ai_usb_ms_filter_file && ai_usb_ms_filter_playlist)
    return;

  // Free allocated buffers if any.
  if (ai_usb_ms_filter_songfile)
    free(ai_usb_ms_filter_songfile);
  if (ai_usb_ms_filter_file)
    free(ai_usb_ms_filter_file);
  if (ai_usb_ms_filter_playlist)
    free(ai_usb_ms_filter_playlist);
  ai_usb_ms_filter_songfile = NULL;
  ai_usb_ms_filter_playlist = NULL;
  ai_usb_ms_filter_file = NULL;

  // Song files filter
  song_file_list_size = 0;
  for (i = 0; i < nb_file_format_supported; i++)
    song_file_list_size += strlen(file_format_supported[i].file_ext);
  if (!(ai_usb_ms_filter_songfile = (char *) malloc(song_file_list_size + nb_file_format_supported + sizeof((char)'\0'))))
    return;
  ai_usb_ms_filter_songfile[0] = '\0';
  for (i = 0; i < nb_file_format_supported; i++)
  {
    if (i)
      strcat(ai_usb_ms_filter_songfile, ",");
    strcat(ai_usb_ms_filter_songfile, file_format_supported[i].file_ext);
  }
  song_file_list_size = song_file_list_size + (i - 1) * sizeof((char)',');

  // Playlist files filter
  playlist_file_list_size = strlen(PLAYLIST_FILE_EXT);
  if (!(ai_usb_ms_filter_playlist = (char *) malloc(playlist_file_list_size + sizeof((char)'\0'))))
    return;
  strcpy(ai_usb_ms_filter_playlist, PLAYLIST_FILE_EXT);

  // All files filter
  if (playlist_file_list_size)
    song_file_list_size += playlist_file_list_size + sizeof(',');
  if (!(ai_usb_ms_filter_file = (char *) malloc(song_file_list_size + sizeof((char)'\0'))))
    return;
  strcpy(ai_usb_ms_filter_file, ai_usb_ms_filter_songfile);
  if (playlist_file_list_size)
  {
    strcat(ai_usb_ms_filter_file, ",");
    strcat(ai_usb_ms_filter_file, ai_usb_ms_filter_playlist);
  }
}

void  ai_usb_ms_new_connection( void )
{
  int i;
  memset(&g_player_flag, 0, sizeof(g_player_flag));
  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
    file_format_supported[i].init();
  g_player_flag_error = false;
  g_ai_usb_ms_b_audio_file = false;
  g_Cmd_state = 0;
  g_player_currentTime_ms = 0;
#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true
  g_context = NULL;
#endif
  // Set repeat mode
  g_ai_usb_ms_repeat_mode = AI_DEFAULT_REPEAT_MODE;
  // Set shuffle mode
  g_ai_usb_ms_shuffle_mode = AI_DEFAULT_SHUFFLE_MODE;
  if (g_ai_usb_ms_shuffle_mode == AUDIO_SHUFFLE_OFF)
    navauto_setrand(NAVAUTO_RAND_OFF);
  else
    navauto_setrand(NAVAUTO_RAND_ON);

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true
  cache_flush();
#endif
}

ai_device_status_t ai_usb_ms_get_device_status( ai_async_status_t *cmd_ai_status )
{
  extern volatile bool ms_connected;
  *cmd_ai_status = CMD_DONE;

#if defined(USB_MS_INCLUDED) && USB_MS_INCLUDED == true
  // Not relevant if the USB macro is not in host state
  if (g_usb_mode != USB_MODE_HOST)
    return AI_DEVICE_STATUS_NOT_PRESENT;

  if (ms_connected)
    return AI_DEVICE_STATUS_CONNECTED;
  else if (Is_host_detect_unsupported_dev())
    return AI_DEVICE_STATUS_NOT_SUPPORTED;
  else if (!Is_host_powered())
    return AI_DEVICE_STATUS_NOT_PRESENT;
  else if (Is_host_error())
    return AI_DEVICE_STATUS_ERROR;
  else if (Is_host_attached())
    return AI_DEVICE_STATUS_PRESENT;
  else
#endif
    return AI_DEVICE_STATUS_NOT_SUPPORTED;
}

uint16_t   ai_usb_ms_get_product_id( ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
#if defined(USB_MS_INCLUDED) && USB_MS_INCLUDED == true
  return Get_PID();
#else
  return 0;
#endif
}


uint16_t   ai_usb_ms_get_vendor_id( ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
#if defined(USB_MS_INCLUDED) && USB_MS_INCLUDED == true
  return Get_VID();
#else
  return 0;
#endif
}


uint16_t   ai_usb_ms_get_serial_number( char** sz_sn, ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
  return 0;
}


uint8_t    ai_usb_ms_nav_drive_nb( ai_async_status_t *cmd_ai_status )
{
  uint8_t u8_res;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  u8_res = nav_drive_nb();
#if MEM_USB == ENABLE
  u8_res -= MAX_LUN;
#endif
  return u8_res;
}

/*! Set the file extension filters to the drive
 */
void ai_usb_ms_init_drive(void)
{
  nav_reset();
  nav_select(FS_NAV_ID_FILESYSTEM);
  nav_filterlist_setfilter( (FS_STRING) ai_usb_ms_filter_file );  // Extension managed in navigation filtered

  // PS: This next routine ignore ID navigator selected
  if (ai_usb_ms_filter_songfile)
    navauto_init( (FS_STRING) ai_usb_ms_filter_songfile );            // Extension managed in audio navigation
  else
    navauto_init( (FS_STRING) "" );
}

bool  ai_usb_ms_nav_drive_set( uint8_t u8_number, ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;

  ai_usb_ms_init_drive();

#if MEM_USB == ENABLE
  u8_number += MAX_LUN;
#endif
  status = nav_drive_set( u8_number );
  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


uint8_t    ai_usb_ms_nav_drive_get( ai_async_status_t *cmd_ai_status )
{
  uint8_t u8_res;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  u8_res = nav_drive_get();
#if MEM_USB == ENABLE
  u8_res -= MAX_LUN;
#endif
  return u8_res;
}


bool  ai_usb_ms_nav_drive_mount( ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  g_ai_usb_ms_b_audio_file = false;
  status = nav_partition_mount();

  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


uint64_t   ai_usb_ms_nav_drive_total_space( ai_async_status_t *cmd_ai_status )
{
  uint32_t u32;

  nav_select(FS_NAV_ID_FILESYSTEM);
  u32 = nav_partition_space();
  *cmd_ai_status = CMD_DONE;

  return ((uint64_t) u32) * FS_SIZE_OF_SECTOR;
}


uint64_t   ai_usb_ms_nav_drive_free_space ( ai_async_status_t *cmd_ai_status )
{
  uint32_t u32;

  nav_select(FS_NAV_ID_FILESYSTEM);
  u32 = nav_partition_freespace();
  *cmd_ai_status = CMD_DONE;

  return ((uint64_t) u32) * FS_SIZE_OF_SECTOR;
}


bool  ai_usb_ms_nav_dir_root( ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  g_smooth_com_cnt = 10;
  status = nav_dir_root();
  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}

bool  ai_usb_ms_nav_dir_cd( ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  g_smooth_com_cnt = 10;
#ifdef NAVIGATION_MODE_FLAT
  status = nav_filterflat_cd();
#else
  status = nav_dir_cd();
  nav_filterlist_reset();
#endif
  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


bool  ai_usb_ms_nav_dir_gotoparent( ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;
  g_smooth_com_cnt = 10;
  nav_select(FS_NAV_ID_FILESYSTEM);
#ifdef NAVIGATION_MODE_FLAT
  status = nav_filterflat_gotoparent();
#else
  status = nav_filterlist_gotoparent();
#endif
  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


bool  ai_usb_ms_nav_file_isdir( ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  status = nav_file_isdir();
  return status;
}

bool  ai_usb_ms_nav_file_next( ai_async_status_t *cmd_ai_status )
{
  bool status;

  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  status = nav_filterlist_next();
  return status;
}

bool  ai_usb_ms_nav_file_previous( ai_async_status_t *cmd_ai_status )
{
  bool status;

  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  status = nav_filterlist_previous();
  return status;
}

bool  ai_usb_ms_nav_file_goto( uint32_t u32_newpos, ai_async_status_t *cmd_ai_status )
{
  bool status;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  g_smooth_com_cnt = 10;
#ifdef NAVIGATION_MODE_FLAT
  status = nav_filterflat_goto( u32_newpos );
#else
  status = nav_filterlist_goto( u32_newpos );
#endif
  if (!status && *cmd_ai_status != CMD_EXECUTING)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


uint32_t   ai_usb_ms_nav_file_pos( ai_async_status_t *cmd_ai_status )
{
  uint32_t u32_res;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
#ifdef NAVIGATION_MODE_FLAT
  u32_res = nav_filterflat_get();
#else
  u32_res = nav_filterlist_get();
#endif
  return u32_res;
}


uint16_t   ai_usb_ms_nav_file_nb( ai_async_status_t *cmd_ai_status )
{
  char *filter_songfile = "";

  nav_select(FS_NAV_ID_FILESYSTEM);
  g_smooth_com_cnt = 10;
  if (g_Cmd_state == 0)
  {
    *cmd_ai_status = CMD_EXECUTING;
    g_u16_count = (uint16_t) -1;
    g_Cmd_state++;
    // no return here
  }

  if (ai_usb_ms_filter_songfile)
    filter_songfile = ai_usb_ms_filter_songfile;

#ifdef NAVIGATION_MODE_FLAT
  g_u16_count = nav_filterflat_nb();
#else
  if ( !nav_filterlist_nb_ex(FL_FILE, filter_songfile, &g_u16_count, EX_RETRY_COUNT) )
    return 0; // Count takes to much time, leave here and come back later.
#endif

  *cmd_ai_status = CMD_DONE;
  g_Cmd_state = 0;
  return g_u16_count;
}


uint16_t   ai_usb_ms_nav_dir_nb( ai_async_status_t *cmd_ai_status )
{
  nav_select(FS_NAV_ID_FILESYSTEM);
  g_smooth_com_cnt = 10;
  if (g_Cmd_state == 0)
  {
    *cmd_ai_status = CMD_EXECUTING;
    g_u16_count = (uint16_t) -1;
    g_Cmd_state++;
    // no return here
  }

#ifdef NAVIGATION_MODE_FLAT
  g_u16_count = 0;
#else
  if ( !nav_filterlist_nb_ex(FL_DIR, NULL, &g_u16_count, EX_RETRY_COUNT) )
    return 0; // Count takes to much time, leave here and come back later.
#endif

  *cmd_ai_status = CMD_DONE;
  g_Cmd_state = 0;
  return g_u16_count;
}


uint16_t   ai_usb_ms_nav_playlist_nb( ai_async_status_t *cmd_ai_status )
{
  nav_select(FS_NAV_ID_FILESYSTEM);
  g_smooth_com_cnt = 10;
  if (g_Cmd_state == 0)
  {
    *cmd_ai_status = CMD_EXECUTING;
    g_u16_count = (uint16_t) -1;
    g_Cmd_state++;
    // no return here
  }

#ifdef NAVIGATION_MODE_FLAT
  g_u16_count = 0;
#else
  if ( !nav_filterlist_nb_ex(FL_FILE, ai_usb_ms_filter_playlist, &g_u16_count, EX_RETRY_COUNT) )
    return 0; // Count takes to much time, leave here and come back later.
#endif

  *cmd_ai_status = CMD_DONE;
  g_Cmd_state = 0;
  return g_u16_count;
}


bool  ai_usb_ms_read_string( bool (*fcnt_read_string)(FS_STRING, uint8_t), char** string, uint16_t* size_string )
{
  bool status;

#ifndef STRING_MAX_LENGTH
  // Compute size name
  nav_string_length_enable();
  status = fcnt_read_string( (FS_STRING)size_string , 1 );
  nav_string_length_disable();
  if ( !status )
  {
    *size_string = 0;
    return false;
  }
#else
  char* pstring;
  uint16_t real_size_string;

  *size_string = STRING_MAX_LENGTH;
#endif

#if (FS_UNICODE == true)
  while (*size_string && !(*string = malloc( (*size_string) * sizeof(uint16_t) )))
    * size_string /= 2;
#else
  while (*size_string && !(*string = malloc( (*size_string) * sizeof(uint8_t) )))
    * size_string /= 2;
#endif

  // Alloc & Get name
  if ( NULL == *string )
  {
    *size_string = 0;
    return false;
  }

  status = fcnt_read_string( *string , (*size_string) );

#if defined(STRING_MAX_LENGTH) && STRING_MAX_LENGTH > 0
  // Update string size
  pstring = *string;
  real_size_string = 1; // Take into account NULL character
#if (FS_UNICODE == true)
  while ((pstring[0] || pstring[1]) && real_size_string < *size_string)
  {
    real_size_string++;
    pstring += 2;
  }
#else
  while (*pstring++ && real_size_string < *size_string)
    real_size_string++;
#endif
  *size_string = real_size_string;
#endif

#if (FS_UNICODE == true)
  *size_string *= sizeof(uint16_t);
#endif

  if ( !status )
  {
    free( *string );
    *size_string = 0;
    return false;
  }
  return true;
}


uint16_t   ai_usb_ms_nav_dir_name( char** string, ai_async_status_t *cmd_ai_status )
{
  uint16_t u16_size_string;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  ai_usb_ms_read_string( &nav_dir_name, string , &u16_size_string );
  return u16_size_string;
}

uint16_t   ai_usb_ms_nav_file_name( char** string, ai_async_status_t *cmd_ai_status )
{
  uint16_t u16_size_string;
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  ai_usb_ms_read_string( &nav_file_getname, string , &u16_size_string );
  return u16_size_string;
}

uint32_t  ai_usb_ms_nav_file_info_type(ai_async_status_t *cmd_ai_status)
{
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_FILESYSTEM);
  if (      nav_file_checkext( "mp3" ) )  return AUDIO_TYPE_MP3; // Most popular types are tested first...
  else if ( nav_file_checkext( "mp1" ) )  return AUDIO_TYPE_MP1;
  else if ( nav_file_checkext( "mp2" ) )  return AUDIO_TYPE_MP2;
  else if ( nav_file_checkext( "wav" ) )  return AUDIO_TYPE_WAV;
  else                                    return AUDIO_TYPE_UNKNOWN;
}


St_ai_info_version   ai_usb_ms_nav_file_info_version   ( ai_async_status_t *cmd_ai_status )
{
  int i;
  St_ai_info_version ver = {.digit.major=0, .digit.minor=0, .digit.revision=0};
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_version)
      return file_format_supported[i].read_metadata_version();
  }

  return ver;
}


uint16_t   ai_usb_ms_nav_file_info_title   ( char** sz_title, ai_async_status_t *cmd_ai_status )
{
  int i;
  uint16_t u16_size_string = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_title)
    {
      ai_usb_ms_read_string(file_format_supported[i].read_metadata_title, sz_title, &u16_size_string);
      return u16_size_string;
    }
  }

  return u16_size_string;
}


uint16_t   ai_usb_ms_nav_file_info_artist ( char** sz_artist, ai_async_status_t *cmd_ai_status )
{
  int i;
  uint16_t u16_size_string = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_artist)
    {
      ai_usb_ms_read_string(file_format_supported[i].read_metadata_artist, sz_artist, &u16_size_string);
      return u16_size_string;
    }
  }

  return u16_size_string;
}

uint16_t   ai_usb_ms_nav_file_info_album   ( char** sz_album, ai_async_status_t *cmd_ai_status )
{
  int i;
  uint16_t u16_size_string = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_album)
    {
      ai_usb_ms_read_string(file_format_supported[i].read_metadata_album, sz_album, &u16_size_string);
      return u16_size_string;
    }
  }

  return u16_size_string;
}


uint16_t   ai_usb_ms_nav_file_info_year    ( ai_async_status_t *cmd_ai_status )
{
  int i;
  uint16_t u16_year = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_year)
      return file_format_supported[i].read_metadata_year();
  }

  return u16_year;
}


uint32_t   ai_usb_ms_nav_file_info_track   ( ai_async_status_t *cmd_ai_status )
{
  int i;
  uint32_t u32_track = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_track)
      return file_format_supported[i].read_metadata_track();
  }

  return u32_track;
}


uint16_t   ai_usb_ms_nav_file_info_genre   ( char** sz_genre, ai_async_status_t *cmd_ai_status )
{
  int i;
  uint16_t u16_size_string = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_genre)
    {
      ai_usb_ms_read_string(file_format_supported[i].read_metadata_genre, sz_genre, &u16_size_string);
      return u16_size_string;
    }
  }

  return u16_size_string;
}


uint32_t   ai_usb_ms_nav_file_info_duration   ( ai_async_status_t *cmd_ai_status )
{
  int i;
  uint32_t u32_duration = 0;
  *cmd_ai_status = CMD_DONE;

  nav_select(FS_NAV_ID_FILESYSTEM);

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file()
      && file_format_supported[i].read_metadata_duration)
      return file_format_supported[i].read_metadata_duration();
  }

  return u32_duration;
}

void *ai_usb_ms_nav_file_info_image(ai_image_size_t *size, ai_async_status_t *cmd_ai_status)
{
  int i;
  nav_select(FS_NAV_ID_FILESYSTEM);
  *cmd_ai_status = CMD_DONE;

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file() &&
        file_format_supported[i].read_metadata_image)
      return file_format_supported[i].read_metadata_image(size);
  }

  return NULL;
}

bool  ai_usb_ms_nav_getplayfile( ai_async_status_t *cmd_ai_status )
{
  bool status;
  Fs_index index;

  *cmd_ai_status = CMD_DONE;
  // Means no file selected
  if (!g_ai_usb_ms_b_audio_file)
  {
    nav_select(FS_NAV_ID_FILESYSTEM);
    nav_dir_root();
    return true;
  }

  // Get the position selected in File System navigation
  nav_select(FS_NAV_ID_AUDIO);
  index = nav_getindex();
  nav_select(FS_NAV_ID_FILESYSTEM);
  status = nav_filterlist_gotoindex( &index );
  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


bool  ai_usb_ms_audio_nav_playfile( ai_async_status_t *cmd_ai_status )
{
  Fs_index index;
  Fs_index index2;

  *cmd_ai_status = CMD_DONE;

  // Wait Stop audio to execute next
  if ( !ai_usb_ms_audio_ctrl_stop(cmd_ai_status))
  {
    *cmd_ai_status = CMD_EXECUTING;
    return false;
  }

  // Get the position selected in File System navigation
  nav_select(FS_NAV_ID_FILESYSTEM);
  index = nav_getindex();
  nav_select(FS_NAV_ID_AUDIO);
  index2 = nav_getindex();

  // exit navigator automatic (in case of)
  navauto_close();
  g_ai_usb_ms_b_audio_file = false;

  // Init audio navigator with file selected in Filesystem Navigator
  if ( !nav_gotoindex( &index ))
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }

  if ( nav_file_checkext( (FS_STRING) ai_usb_ms_filter_playlist ) )
  {
    // Start play list
    if ( !navauto_open( true , (uint16_t) -1 ))
    {
      *cmd_ai_status = CMD_ERROR;
      return false;
    }
  }
  else
  {
    //** Start play
    // If the file selected have a correct extension then play file
    // If no file selected then play the first file or a random file
    if ( !navauto_open( false , 0 ))
    {
      *cmd_ai_status = CMD_ERROR;
      return false;
    }
  }
  g_ai_usb_ms_b_audio_file = true;
  g_player_flag.new_file_played = 1;

  if (index.u32_cluster_sel_dir != index2.u32_cluster_sel_dir)
    g_player_flag.new_directory = 1;

#if defined(PAUSE_ON_TRACK_CHANGED) && PAUSE_ON_TRACK_CHANGED == true
  ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
  // Automatically pause new track
  return ai_usb_ms_audio_ctrl_pause(cmd_ai_status);
#else
  return ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
#endif
}


void *ai_usb_ms_audio_context_save( uint8_t *status, uint16_t *size, ai_async_status_t *cmd_ai_status )
{
  St_audio_ctx* ctx = malloc( sizeof(St_audio_ctx) );
  Ai_player_flag_t player_flag;

  // Wait Stop audio to execute next
  *cmd_ai_status = CMD_DONE;

  if ( ctx == NULL )
  {
    *status = false;
    *size = 0;
    return NULL;
  }

  if (!g_ai_usb_ms_b_audio_file)
  {
    free(ctx);
    *status = false;
    *size = 0;
    return NULL;
  }

  ctx->b_valid               = g_ai_usb_ms_b_audio_file;

  player_flag = ai_usb_ms_audio_ctrl_status(cmd_ai_status);
  memcpy(&ctx->u8_audio_ctrl_status, &player_flag, sizeof(ctx->u8_audio_ctrl_status));
  // Restore previous status (before calling ai_usb_ms_audio_ctrl_status)
  if (ctx->u8_audio_ctrl_status.new_file_played)
  {
    g_player_flag.new_file_played = 1;
  }
  if (ctx->u8_audio_ctrl_status.new_directory)
  {
    g_player_flag.new_directory = 1;
  }

  nav_select(FS_NAV_ID_AUDIO);

  // Save all modes
  ctx->repeat                = g_ai_usb_ms_repeat_mode;
  ctx->exp_mode              = navauto_getmode();
  ctx->shuffle               = g_ai_usb_ms_shuffle_mode; //navauto_getrand();

  if ( g_ai_usb_ms_b_audio_file )
  {
    extern bool g_navauto_b_playlist;
    extern uint16_t g_navauto_u16_pos, g_navauto_u16_dir_level;

    // Get current time play
    ctx->u32_time = ai_usb_ms_audio_ctrl_time(cmd_ai_status);

    // Get current file position
    ctx->u32_file_pos = file_getpos();

    //* Save Filesystem position
    // Directory level OR position in the playlist, of last file

    if ( g_navauto_b_playlist )
      ctx->u16_pos_level = g_navauto_u16_pos;
    else
      ctx->u16_pos_level = g_navauto_u16_dir_level;

    // FileSystem pointer on the last file play or last playlist
    ctx->index           = nav_getindex();
  }

  if (!fat_check_device())
  {
    free(ctx);
    *status = false;
    *size = 0;
    return NULL;
  }

  *status = true;
  *size = sizeof(St_audio_ctx);

  return ctx;
}


bool  ai_usb_ms_audio_context_restore( void *p, ai_async_status_t *cmd_ai_status )
{
  St_audio_ctx* ctx= (St_audio_ctx*)p;

  // Wait Stop audio to execute next
  if ( !ai_usb_ms_audio_ctrl_stop(cmd_ai_status) )
  {
    *cmd_ai_status = CMD_EXECUTING;
    return false;  // Restart command
  }
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_AUDIO);
  // Restore all modes
  // g_ai_usb_ms_repeat_mode = ctx->repeat;
  ai_usb_ms_audio_nav_repeat_set(ctx->repeat, cmd_ai_status);
  navauto_setmode( ctx->exp_mode );
  ai_usb_ms_audio_nav_shuffle_set(ctx->shuffle, cmd_ai_status);
  // navauto_setrand( ctx->shuffle );

  if ( ctx->b_valid )
  {
    navauto_close();
    g_ai_usb_ms_b_audio_file = false;

    // FileSystem pointer on the last file play or last playlist
    if ( !nav_gotoindex(&(ctx->index)) )
    {
      *cmd_ai_status = CMD_ERROR;
      return false;
    }
    if ( nav_file_checkext( (FS_STRING) ai_usb_ms_filter_playlist ) )
    {
      // Start play list
      if ( !navauto_open( true , ctx->u16_pos_level ))
      {
        *cmd_ai_status = CMD_ERROR;
        return false;
      }
    }
    else
    {
      //** Start play file
      if ( !navauto_open( false , ctx->u16_pos_level ))
      {
        *cmd_ai_status = CMD_ERROR;
        return false;
      }
    }
    g_ai_usb_ms_b_audio_file = true;

    // Set the track changed bit
    g_player_flag.new_file_played = 1;

    // restore the Audio controller status
    if ( ctx->u8_audio_ctrl_status.status == PLAYER_FLAG_PLAY
         ||  ctx->u8_audio_ctrl_status.status_fast == PLAYER_FLAG_FFW
         ||  ctx->u8_audio_ctrl_status.status_fast == PLAYER_FLAG_FRW)
    {
      if ( !ai_usb_ms_audio_ctrl_resume( cmd_ai_status ))
        return false;
      if ( !ai_usb_ms_audio_ctrl_ffw(ctx->u32_time / 1000, cmd_ai_status))
        return false;
      g_player_target_file_pos = ctx->u32_file_pos;
    }
    else if (ctx->u8_audio_ctrl_status.status == PLAYER_FLAG_PAUSE)
    {
      if ( !ai_usb_ms_audio_ctrl_resume( cmd_ai_status ))
        return false;
      if ( !ai_usb_ms_audio_ctrl_ffw(ctx->u32_time / 1000, cmd_ai_status))
        return false;
      g_player_target_file_pos = ctx->u32_file_pos;
      if (!ai_usb_ms_audio_ctrl_pause( cmd_ai_status ))
        return false;
    }
  }
  return true;
}


bool  ai_usb_ms_audio_nav_next(  ai_async_status_t *cmd_ai_status   )
{
  return ai_usb_ms_audio_nav_mov( FS_FIND_NEXT, cmd_ai_status );
}

bool  ai_usb_ms_audio_nav_previous(  ai_async_status_t *cmd_ai_status   )
{
  return ai_usb_ms_audio_nav_mov( FS_FIND_PREV, cmd_ai_status );
}

//! Go to previous or next file to play
//!
//! @return    false in case of error
//!
static bool  ai_usb_ms_audio_nav_mov( bool b_direction, ai_async_status_t *cmd_ai_status  )
{
  Navauto_mov_state navauto_state;
  typedef enum
  {  STATE_INIT
     ,  STATE_PLAY
     ,  STATE_STOP
     ,  STATE_PAUSE
  } t_player_state;
  static t_player_state state;
  ai_async_context_t ctx;
  int i;
  bool is_file_playing;

  // If false, means it is the very first call
  if (!ai_async_context_pop(&ctx))
    ctx.state = 0;
  // By default set the status to DONE
  ctx.status = CMD_DONE;
  *cmd_ai_status = CMD_DONE;

  switch(ctx.state)
  {
  // Initialization
  case 0:
    if (!g_ai_usb_ms_b_audio_file)
    {
      *cmd_ai_status = CMD_ERROR;
      return false;
    }

    is_file_playing = false;
    for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
    {
      if (file_format_supported[i].is_file_playing())
      {
        is_file_playing = true;
        break;
      }
    }
    state = (is_file_playing) ? ( (g_player_flag.status == PLAYER_FLAG_PAUSE) ? STATE_PAUSE : STATE_PLAY ) : STATE_STOP;
    ctx.state = 1;
    ctx.status = CMD_EXECUTING;
    break;

  case 1:
    if ( !ai_usb_ms_audio_ctrl_stop(cmd_ai_status) )
    {
      ctx.status = CMD_EXECUTING;
      break;  // Restart command
    }

    nav_select(FS_NAV_ID_AUDIO);

    if ( g_ai_usb_ms_repeat_mode != AUDIO_REPEAT_TRACK )
    {
      navauto_state = navauto_mov( b_direction,  (   g_ai_usb_ms_repeat_mode   == AUDIO_REPEAT_FOLDER
                                                 ||  g_ai_usb_ms_shuffle_mode  == AUDIO_SHUFFLE_FOLDER
                                                 ) ? NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR : NAVAUTO_MOV_OPTS_NONE );
      if (navauto_state.bitfield.folder_change)
        g_player_flag.new_directory = 1;

      // Stop audio and exit navigator automatic (in case of)
      switch ( navauto_state.status )
      {
      case NAVAUTO_MOV_OK:
      case NAVAUTO_MOV_OK_LOOP:
        break;

      default:
        // Error occur then close audio
        navauto_close();
        g_ai_usb_ms_b_audio_file = false;
        state = STATE_INIT;
        *cmd_ai_status = CMD_ERROR;
        return false;
      }
    }
    // Save the direction for future use.
    b_previous_direction = b_direction;

    g_player_flag.new_file_played = 1;

    if ( state == STATE_PLAY )
    {   // Restart play automatic
      state = STATE_INIT;
#if defined(PAUSE_ON_TRACK_CHANGED) && PAUSE_ON_TRACK_CHANGED == true
      ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
      // Automatically pause new track
      return ai_usb_ms_audio_ctrl_pause(cmd_ai_status);
#else
      return ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
#endif
    }
    if ( state == STATE_PAUSE )
    {
      // Restart play automatic
      state = STATE_INIT;
      ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
      return ai_usb_ms_audio_ctrl_pause(cmd_ai_status);
    }
    return true;
  }

  if (!ai_async_context_push(&ctx))
    ctx.status = CMD_ERROR;

  ai_async_update_cmd_status(cmd_ai_status);

  return false;
}


bool  ai_usb_ms_audio_nav_eof_occur( ai_async_status_t *cmd_ai_status )
{
  Navauto_mov_state navauto_state;
  bool b_direction = FS_FIND_NEXT;

  nav_select(FS_NAV_ID_AUDIO);
  *cmd_ai_status = CMD_DONE;

  if ( g_ai_usb_ms_repeat_mode == AUDIO_REPEAT_TRACK )
  {
    g_player_flag.new_file_played = 1;
#if defined(PAUSE_ON_TRACK_CHANGED) && PAUSE_ON_TRACK_CHANGED == true
    ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
    // Automatically pause new track
    return ai_usb_ms_audio_ctrl_pause(cmd_ai_status);
#else
    return ai_usb_ms_audio_ctrl_resume(cmd_ai_status);   // start play of same file
#endif
  }

  if (g_player_flag_error)
  {
    // Clear the error
    g_player_flag_error = false;
    // Keep the same direction as it was before.
    b_direction = b_previous_direction;
  }
  else
  {
    // By default skip NEXT file
    b_previous_direction = FS_FIND_NEXT;
  }

  navauto_state = navauto_mov( b_direction, (   g_ai_usb_ms_repeat_mode   == AUDIO_REPEAT_FOLDER
                                                ||  g_ai_usb_ms_shuffle_mode  == AUDIO_SHUFFLE_FOLDER
                                            ) ? NAVAUTO_MOV_OPTS_STAY_CURRENT_DIR : NAVAUTO_MOV_OPTS_NONE );

  if (navauto_state.bitfield.folder_change)
    g_player_flag.new_directory = 1;

  // Stop audio and exit navigator automatic (in case of)
  switch ( navauto_state.status )
  {
  case NAVAUTO_MOV_OK:
    break;

  case NAVAUTO_MOV_OK_LOOP:
    if (g_ai_usb_ms_repeat_mode != AUDIO_REPEAT_ALL &&
        g_ai_usb_ms_repeat_mode != AUDIO_REPEAT_FOLDER)
      return false;  // End of loop then stop but new file selected
    break;

  default:
    // Error occur then close audio
    navauto_close();
    g_ai_usb_ms_b_audio_file = false;
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  g_player_flag.new_file_played = 1;

#if defined(PAUSE_ON_TRACK_CHANGED) && PAUSE_ON_TRACK_CHANGED == true
  ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
  // Automatically pause new track
  return ai_usb_ms_audio_ctrl_pause(cmd_ai_status);
#else
  return ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
#endif
}


uint16_t   ai_usb_ms_audio_nav_nb( ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
  if ( !g_ai_usb_ms_b_audio_file )
    return 0;
#if defined(FS_NAV_AUTOMATIC_NBFILE) && FS_NAV_AUTOMATIC_NBFILE == ENABLE
  nav_select(FS_NAV_ID_AUDIO);
  return navauto_getnb();
#else
  return 0;
#endif
}


uint16_t   ai_usb_ms_audio_nav_getpos( ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
  if ( !g_ai_usb_ms_b_audio_file )
    return 0;
  nav_select(FS_NAV_ID_AUDIO);
  return navauto_getpos();
}


bool  ai_usb_ms_audio_nav_setpos( uint16_t u16_pos, ai_async_status_t *cmd_ai_status )
{
  bool status;

  *cmd_ai_status = CMD_DONE;

  if ( !g_ai_usb_ms_b_audio_file )
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }

  nav_select(FS_NAV_ID_AUDIO);

  g_player_flag.status = PLAYER_FLAG_PAUSE;
  if ( !ai_usb_ms_audio_ctrl_stop(cmd_ai_status) )
  {
    *cmd_ai_status = CMD_EXECUTING;
    return false;  // Restart command
  }

  status = navauto_setpos(u16_pos);

  if (g_player_flag.status == PLAYER_FLAG_STOP)
  {
    // Restart play automatic
    return ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
  }

  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}


Ai_repeat_mode ai_usb_ms_audio_nav_repeat_get( ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
  return g_ai_usb_ms_repeat_mode;
}


void  ai_usb_ms_audio_nav_repeat_set( Ai_repeat_mode repeat_mode, ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
  g_ai_usb_ms_repeat_mode = repeat_mode;
}


Ai_explorer_mode ai_usb_ms_audio_nav_expmode_get( ai_async_status_t *cmd_ai_status )
{
  nav_select(FS_NAV_ID_AUDIO);
  *cmd_ai_status = CMD_DONE;

  // Convert Navauto_mode to Ai_explorer_mode
  switch (navauto_getmode())
  {
  case NAVAUTO_MODE_DISKS:
    return AUDIO_EXPLORER_MODE_DISKS;
  case NAVAUTO_MODE_DISK:
    return AUDIO_EXPLORER_MODE_DISK;
  case NAVAUTO_MODE_DIRONLY:
    return AUDIO_EXPLORER_MODE_DIRONLY;
  case NAVAUTO_MODE_DIRSUB:
    return AUDIO_EXPLORER_MODE_DIRSUB;
  default:
    return 0;
  }
}


void  ai_usb_ms_audio_nav_expmode_set( Ai_explorer_mode exp_mode, ai_async_status_t *cmd_ai_status )
{
  Navauto_mode navauto_exp_mode;
  static bool b_is_player_stopped = true;

  nav_select(FS_NAV_ID_AUDIO);
  *cmd_ai_status = CMD_DONE;

  // Convert Ai_explorer_mode to Navauto_mode
  switch (exp_mode)
  {
  case AUDIO_EXPLORER_MODE_DISKS:
    navauto_exp_mode = NAVAUTO_MODE_DISKS;
    break;
  case AUDIO_EXPLORER_MODE_DISK:
    navauto_exp_mode = NAVAUTO_MODE_DISK;
    break;
  case AUDIO_EXPLORER_MODE_DIRONLY:
    navauto_exp_mode = NAVAUTO_MODE_DIRONLY;
    break;
  case AUDIO_EXPLORER_MODE_DIRSUB:
    navauto_exp_mode = NAVAUTO_MODE_DIRSUB;
    break;
  default:
    return;
  }

  if (g_player_flag.status != PLAYER_FLAG_STOP)
    b_is_player_stopped = false;

  g_player_flag.status = PLAYER_FLAG_PAUSE;
  if ( !ai_usb_ms_audio_ctrl_stop(cmd_ai_status) )
  {
    *cmd_ai_status = CMD_EXECUTING;
    return;  // Restart command
  }

  navauto_setmode( navauto_exp_mode );

  if (g_player_flag.status == PLAYER_FLAG_STOP && !b_is_player_stopped)
  {
    // Restart play automatic
    ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
  }

  b_is_player_stopped = true;
}


Ai_shuffle_mode ai_usb_ms_audio_nav_shuffle_get( ai_async_status_t *cmd_ai_status )
{
  nav_select(FS_NAV_ID_AUDIO);
  *cmd_ai_status = CMD_DONE;
  return g_ai_usb_ms_shuffle_mode;
}


void  ai_usb_ms_audio_nav_shuffle_set( Ai_shuffle_mode shuffle, ai_async_status_t *cmd_ai_status )
{
  nav_select(FS_NAV_ID_AUDIO);
  *cmd_ai_status = CMD_DONE;
  g_ai_usb_ms_shuffle_mode = shuffle;
  switch (shuffle)
  {
  case AUDIO_SHUFFLE_OFF:
    navauto_setrand(NAVAUTO_RAND_OFF);
    break;
  case AUDIO_SHUFFLE_FOLDER:
  case AUDIO_SHUFFLE_ALL:
    navauto_setrand(NAVAUTO_RAND_ON);
#if defined(CHANGE_TRACK_ON_SHUFFLE) && CHANGE_TRACK_ON_SHUFFLE == ENABLE
    ai_usb_ms_audio_nav_next(cmd_ai_status);
#endif
    break;
  }
}


uint16_t   ai_usb_ms_audio_nav_getname( char** string, ai_async_status_t *cmd_ai_status )
{
  uint16_t u16_size_string;
  *cmd_ai_status = CMD_DONE;
  if ( !g_ai_usb_ms_b_audio_file )
    return 0;
  nav_select(FS_NAV_ID_AUDIO);
  ai_usb_ms_read_string( &nav_file_getname, string , &u16_size_string );
  return u16_size_string;
}


uint16_t   ai_usb_ms_song_read_info( uint16_t (*fcnt_song_read_info)(FS_STRING*, ai_async_status_t *), char** string, ai_async_status_t * cmd_ai_status )
{
  Fs_index idx_save;
  Fs_index idx_audio;
  uint16_t u16_lgt = 0;

  if ( !g_ai_usb_ms_b_audio_file )
  {
    *cmd_ai_status = CMD_DONE;
    return u16_lgt;
  }

  // Read Audio file information via FileSystem Navigator in case of file is played (already open)
  nav_select(FS_NAV_ID_AUDIO);
  idx_audio = nav_getindex();         // Get position of Audio navigator
  nav_select(FS_NAV_ID_FILESYSTEM);
  idx_save = nav_getindex();          // Save position of FileSystem navigator
  if ( nav_gotoindex( &idx_audio ) )  // Set Audio position in FileSystem navigator
    u16_lgt = fcnt_song_read_info( string, cmd_ai_status );
  else
    *cmd_ai_status = CMD_ERROR;
  nav_gotoindex( &idx_save );         // Restore position of FileSystem navigator
  return u16_lgt;
}


uint32_t  ai_usb_ms_audio_nav_file_info_type(ai_async_status_t *cmd_ai_status)
{
  *cmd_ai_status = CMD_DONE;
  nav_select(FS_NAV_ID_AUDIO);
  if (      nav_file_checkext( "mp3" ) )  return AUDIO_TYPE_MP3; // Most popular types are tested first...
  else if ( nav_file_checkext( "mp1" ) )  return AUDIO_TYPE_MP1;
  else if ( nav_file_checkext( "mp2" ) )  return AUDIO_TYPE_MP2;
  else if ( nav_file_checkext( "wav" ) )  return AUDIO_TYPE_WAV;
  else                                    return AUDIO_TYPE_UNKNOWN;
}


St_ai_info_version   ai_usb_ms_audio_nav_file_info_version   ( ai_async_status_t *cmd_ai_status )
{
  Fs_index idx_save;
  Fs_index idx_audio;
  St_ai_info_version ver = {.digit.major=0, .digit.minor=0, .digit.revision=0};

  if ( !g_ai_usb_ms_b_audio_file )
  {
    *cmd_ai_status = CMD_DONE;
    return ver;
  }

  // Read Audio file information via FileSystem Navigator in case of file is played (already open)
  nav_select(FS_NAV_ID_AUDIO);
  idx_audio = nav_getindex();         // Get position of Audio navigator
  nav_select(FS_NAV_ID_FILESYSTEM);
  idx_save = nav_getindex();          // Save position of FileSystem navigator
  if ( nav_gotoindex( &idx_audio ) )  // Set Audio position in FileSystem navigator
    ver = ai_usb_ms_nav_file_info_version( cmd_ai_status );
  else
    *cmd_ai_status = CMD_ERROR;
  nav_gotoindex( &idx_save );         // Restore position of FileSystem navigator
  return ver;
}


uint16_t   ai_usb_ms_audio_nav_file_info_title   ( char** sz_title, ai_async_status_t *cmd_ai_status )
{
  return ai_usb_ms_song_read_info( &ai_usb_ms_nav_file_info_title, sz_title, cmd_ai_status );
}


uint16_t   ai_usb_ms_audio_nav_file_info_artist ( char** sz_artist, ai_async_status_t *cmd_ai_status )
{
  return ai_usb_ms_song_read_info( &ai_usb_ms_nav_file_info_artist, sz_artist, cmd_ai_status );
}


uint16_t   ai_usb_ms_audio_nav_file_info_album   ( char** sz_album, ai_async_status_t *cmd_ai_status )
{
  return ai_usb_ms_song_read_info( &ai_usb_ms_nav_file_info_album, sz_album, cmd_ai_status );
}


uint16_t   ai_usb_ms_audio_nav_file_info_year    ( ai_async_status_t *cmd_ai_status )
{
  Fs_index idx_save;
  Fs_index idx_audio;
  uint16_t u16_year = 0;

  if ( !g_ai_usb_ms_b_audio_file )
  {
    *cmd_ai_status = CMD_DONE;
    return u16_year;
  }

  // Read Audio file information via FileSystem Navigator in case of file is played (already open)
  nav_select(FS_NAV_ID_AUDIO);
  idx_audio = nav_getindex();         // Get position of Audio navigator
  nav_select(FS_NAV_ID_FILESYSTEM);
  idx_save = nav_getindex();          // Save position of FileSystem navigator
  if ( nav_gotoindex( &idx_audio ) )  // Set Audio position in FileSystem navigator
    u16_year = ai_usb_ms_nav_file_info_year( cmd_ai_status );
  else
    *cmd_ai_status = CMD_ERROR;
  nav_gotoindex( &idx_save );         // Restore position of FileSystem navigator
  return u16_year;
}

uint32_t   ai_usb_ms_audio_nav_file_info_track   ( ai_async_status_t *cmd_ai_status )
{
  Fs_index idx_save;
  Fs_index idx_audio;
  uint32_t u32_track = 0;

  if ( !g_ai_usb_ms_b_audio_file )
  {
    *cmd_ai_status = CMD_DONE;
    return u32_track;
  }

  // Read Audio file information via FileSystem Navigator in case of file is played (already open)
  nav_select(FS_NAV_ID_AUDIO);
  idx_audio = nav_getindex();         // Get position of Audio navigator
  nav_select(FS_NAV_ID_FILESYSTEM);
  idx_save = nav_getindex();          // Save position of FileSystem navigator
  if ( nav_gotoindex( &idx_audio ) )  // Set Audio position in FileSystem navigator
    u32_track = ai_usb_ms_nav_file_info_track( cmd_ai_status );
  else
    *cmd_ai_status = CMD_ERROR;
  nav_gotoindex( &idx_save );         // Restore position of FileSystem navigator
  return u32_track;
}


uint16_t   ai_usb_ms_audio_nav_file_info_genre   ( char** sz_genre, ai_async_status_t *cmd_ai_status )
{
  return ai_usb_ms_song_read_info( &ai_usb_ms_nav_file_info_genre, sz_genre, cmd_ai_status );
}


uint32_t   ai_usb_ms_audio_nav_file_info_duration   ( ai_async_status_t *cmd_ai_status )
{
  Fs_index idx_save;
  Fs_index idx_audio;
  uint32_t u32_duration = 0;

  if ( !g_ai_usb_ms_b_audio_file )
  {
    *cmd_ai_status = CMD_DONE;
    return u32_duration;
  }

  // Read Audio file information via FileSystem Navigator in case of file is played (already open)
  nav_select(FS_NAV_ID_AUDIO);
  idx_audio = nav_getindex();         // Get position of Audio navigator
  nav_select(FS_NAV_ID_FILESYSTEM);
  idx_save = nav_getindex();          // Save position of FileSystem navigator
  if ( nav_gotoindex( &idx_audio ) )  // Set Audio position in FileSystem navigator
    u32_duration = ai_usb_ms_nav_file_info_duration( cmd_ai_status );
  else
    *cmd_ai_status = CMD_ERROR;
  nav_gotoindex( &idx_save );         // Restore position of FileSystem navigator
  return u32_duration;
}

void *ai_usb_ms_audio_nav_file_info_image(ai_image_size_t *size, ai_async_status_t *cmd_ai_status)
{
  int i;
  nav_select(FS_NAV_ID_AUDIO);
  *cmd_ai_status = CMD_DONE;

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_metadata_compatible_file() &&
        file_format_supported[i].read_metadata_image)
      return file_format_supported[i].read_metadata_image(size);
  }

  return NULL;
}

bool  ai_usb_ms_audio_ctrl_stop      ( ai_async_status_t *cmd_ai_status )
{
  int i;

  g_player_flag.status = PLAYER_FLAG_STOP;
  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_file_playing())
    {
      *cmd_ai_status = CMD_EXECUTING;
      return false;  // Stop no finish
    }
  }
  // Reset current time to 0
  g_player_currentTime_ms = 0;
  g_ai_usb_ms_b_audio_file = false;
  *cmd_ai_status = CMD_DONE;
  return true;   // Player stopped
}


bool  ai_usb_ms_audio_ctrl_resume    ( ai_async_status_t *cmd_ai_status )
{
  int i;
  char *filter_songfile = "";

  *cmd_ai_status = CMD_DONE;

  if (ai_usb_ms_filter_songfile)
    filter_songfile = ai_usb_ms_filter_songfile;

  // If there is no files selected in the audio navigator...
  nav_select(FS_NAV_ID_AUDIO);
  if ((nav_filelist_get() == FS_NO_SEL)
   || !(nav_file_checkext((FS_STRING) ai_usb_ms_filter_playlist)
   || nav_file_checkext((FS_STRING) filter_songfile)))
  {
    Fs_index index;
    bool is_file_selected;

    // Get the position selected in File System navigation
    nav_select(FS_NAV_ID_FILESYSTEM);
    index = nav_getindex();
    is_file_selected = (fs_g_nav.u16_pos_filterlist != FS_NO_SEL);
    nav_select(FS_NAV_ID_AUDIO);

    // exit navigator automatic (in case of)
    navauto_close();
    g_ai_usb_ms_b_audio_file = false;

    // Init audio navigator with file selected in Filesystem Navigator
    if ( !nav_gotoindex( &index ))
    {
      *cmd_ai_status = CMD_ERROR;
      return false;
    }

    if (nav_file_checkext( (FS_STRING) ai_usb_ms_filter_playlist ) && is_file_selected)
    {
      // Start play list
      if ( !navauto_open( true , (uint16_t) -1 ))
      {
        *cmd_ai_status = CMD_ERROR;
        return false;
      }
    }
    else
    {
      //** Start play
      // If the file selected have a correct extension then play file
      // If no file selected then play the first file or a random file
      if ( !navauto_open( false , 0 ))
      {
        *cmd_ai_status = CMD_ERROR;
        return false;
      }
    }
    g_player_flag.new_file_played = 1;
  }

  // Here, it means the pointer points on a valid file
  g_ai_usb_ms_b_audio_file = true;

  g_player_flag.status_fast = PLAYER_FLAG_NORMAL;
  g_player_duration = 0;
  g_player_target_file_pos = 0;
  g_player_flag.status = PLAYER_FLAG_PLAY;

  // If a file is already being played.
  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_file_playing())
      return true;
  }
  // Check the selected file format and plays it if it can be played.
  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_audio_file())
    {
      file_format_supported[i].set_play_file_notification();
      break;
    }
  }
  // Else returns error.
  if (i == sizeof(file_format_supported) / sizeof(file_format_supported[0]))
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }

  return true;
}


bool   ai_usb_ms_audio_ctrl_pause      ( ai_async_status_t *cmd_ai_status )
{
  int i;

  *cmd_ai_status = CMD_DONE;
  g_player_flag.status = PLAYER_FLAG_PAUSE;

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_file_playing())
      return true;
  }

  // No file being played
  *cmd_ai_status = CMD_ERROR;
  return false;
}


uint32_t   ai_usb_ms_audio_ctrl_time      ( ai_async_status_t *cmd_ai_status )
{
  *cmd_ai_status = CMD_DONE;
  return (g_player_currentTime_ms);
}

Ai_player_flag_t ai_usb_ms_audio_ctrl_status    ( ai_async_status_t *cmd_ai_status )
{
  Ai_player_flag_t player_flag;

  nav_select(FS_NAV_ID_FILESYSTEM);
  if (!fat_check_device())
  {
    *cmd_ai_status = CMD_ERROR;
    memset(&player_flag, 0, sizeof(player_flag));
    return player_flag;
  }

  memcpy(&player_flag, &g_player_flag, sizeof(player_flag));

  if (g_player_flag.new_file_played)
    g_player_flag.new_file_played = 0;
  if (g_player_flag.new_directory)
    g_player_flag.new_directory = 0;

  *cmd_ai_status = CMD_DONE;
  return player_flag;
}

bool  ai_usb_ms_audio_ctrl_ffw    ( uint32_t u32_duration, ai_async_status_t *cmd_ai_status)
{
  int i;

  *cmd_ai_status = CMD_DONE;
  g_player_target_file_pos = 0;

  for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
  {
    if (file_format_supported[i].is_file_playing())
    {
      g_player_duration = u32_duration;
      g_player_flag.status_fast = PLAYER_FLAG_FFW;
      return true;
    }
  }

  *cmd_ai_status = CMD_ERROR;
  return false;
}


bool  ai_usb_ms_audio_ctrl_frw    ( uint32_t u32_duration, ai_async_status_t *cmd_ai_status)
{
  int i;

  *cmd_ai_status = CMD_DONE;
  g_player_target_file_pos = 0;

  // The track position is the beginning of the file
  if (ai_usb_ms_audio_ctrl_time(cmd_ai_status) <= u32_duration*1000 ||
      g_player_flag.status == PLAYER_FLAG_STOP)
  {
    // Stop the file make sure we are at the beginning
    if ( !ai_usb_ms_audio_ctrl_stop(cmd_ai_status) )
    {
      *cmd_ai_status = CMD_EXECUTING;
      return false;  // Restart command
    }
    // Restart the current file
    ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
  }
  else
  {
    for (i = 0; i < sizeof(file_format_supported) / sizeof(file_format_supported[0]); i++)
    {
      if (file_format_supported[i].is_file_playing())
      {
        g_player_duration = u32_duration;
        g_player_flag.status_fast = PLAYER_FLAG_FRW;
        return true;
      }
    }
  }
  *cmd_ai_status = CMD_ERROR;
  return false;
}


bool ai_usb_ms_audio_ctrl_start_ffw(ai_async_status_t *cmd_ai_status)
{
  return ai_usb_ms_audio_ctrl_ffw(3, cmd_ai_status);
}


bool ai_usb_ms_audio_ctrl_start_frw(ai_async_status_t *cmd_ai_status)
{
  return ai_usb_ms_audio_ctrl_frw(3, cmd_ai_status);
}


bool ai_usb_ms_audio_ctrl_stop_ffw_frw(ai_async_status_t *cmd_ai_status)
{
  *cmd_ai_status = CMD_ERROR;
  return false;
}


/****************************** USB Mass Storage Specific Functions *******************************/
static uint16_t ai_usb_ms_specific_nav_get_next_playlist_pos(uint16_t u16_start_file_pos, ai_async_status_t *cmd_ai_status);
#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true
static uint16_t ai_usb_ms_specific_nav_playlist_link_nb(ai_async_status_t *cmd_ai_status);
static uint16_t ai_usb_ms_specific_nav_playlist_link_name(uint16_t u16_link_pos, char **string, ai_async_status_t *cmd_ai_status);
#endif
static bool ai_usb_ms_specific_audio_nav_playlist_play_link(uint16_t u16_link_pos, ai_async_status_t *cmd_ai_status);
static bool ai_usb_ms_specific_audio_nav_get_folder_name(char **string, uint8_t u8_size_max, ai_async_status_t *cmd_ai_status);
static bool ai_usb_ms_specific_nav_getplaylistfile(ai_async_status_t *cmd_ai_status);

#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true
// Hack to use the same navigator (PLAYLIST_NAV) for both the disk navigation and the audio navigation
static void ai_usb_ms_specific_restore_playlist_navigator(void *ctx_save);
static void *ai_usb_ms_specific_save_playlist_navigator(void);
#endif

typedef struct
{
  uint8_t navigator_id;
  Fs_management fs_g_nav;
  Fs_management_entry fs_g_nav_entry;
  Fs_management_fast fs_g_nav_fast;
  bool pl_g_list_is_open;
  uint16_t pl_g_u16_list_size;
  uint16_t pl_g_u16_list_sel;
} navigator_context_t;

static void ai_usb_ms_specific_save_navigator(navigator_context_t *nav_ctx);
static bool ai_usb_ms_specific_save_navigator_ex(uint8_t navigator_id, navigator_context_t **nav_ctx, ai_async_status_t *cmd_ai_status);
static void ai_usb_ms_specific_restore_navigator(navigator_context_t *nav_ctx);
static bool ai_usb_ms_specific_restore_navigator_ex(uint8_t navigator_id, navigator_context_t **nav_ctx, ai_async_status_t *cmd_ai_status);

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true

typedef uint32_t cache_id_t;
typedef struct
{
  cache_id_t id;
  uint8_t data[];
} cache_t;

static cache_t *cache = NULL;

//! \brief This function creates an ID for the process.
//! It will be compared with the one stored on the cache to check its validity.
//! \param id_string A string to generate this ID.
//! \param id The ID. It will be filled with zeros if the function failed.
//! \return The status of the generation.
bool cache_get_id(uint8_t *id_string, cache_id_t *id)
{
  cache_id_t hash;

  // Generates a simple hash using the JS hash algorithm.
  hash = 1315423911;
  while (*id_string)
    hash ^= ((hash << 5) + *id_string++ + (hash >> 2));
  memcpy(id, &hash, sizeof(id));

  return true;
}

//! \brief This function updates an ID for the current process.
//! \param u32 Data for the update.
//! \param id The ID. It will be filled with zeros if the function failed.
//! \return The status of the generation.
bool cache_update_id(uint32_t u32, cache_id_t *id)
{
  cache_id_t hash = *id;

  // Generates a simple hash using the JS hash algorithm.
  hash ^= ((hash << 5) + LSB0(u32) + (hash >> 2));
  hash ^= ((hash << 5) + LSB1(u32) + (hash >> 2));
  hash ^= ((hash << 5) + LSB2(u32) + (hash >> 2));
  hash ^= ((hash << 5) + LSB3(u32) + (hash >> 2));
  memcpy(id, &hash, sizeof(id));

  return true;
}

//! \brief This function checks if the cache is valid for this specific ID.
//! \param id The ID to check.
//! \return true if the cache is valid with this ID, false otherwise.
bool chache_test_id(cache_id_t *id)
{
  size_t i;
  uint8_t *p_id;
  bool valid;

  // If no cache...
  if (!cache)
    return false;

  // By default it is not valid
  valid = false;
  // Check if the ID is itself valid
  p_id = (uint8_t *) id;
  for (i=0; i<sizeof(cache_id_t); i++)
  {
    if (p_id[i])
    {
      valid = true;
      break;
    }
  }
  if (!valid)
    return false;
  // Check with the current cache
  if (memcmp((char const *) &(cache->id), (char const *) id, sizeof(cache_id_t)))
    return false;
  return true;
}

//! \brief Flush the cache
void cache_flush(void)
{
  if (!cache)
    return;
  free(cache);
  cache = NULL;
}

//! \brief Returns the cache associated with this ID, if any. Or creates a new
//! one.
//! \param id The ID associated with the cache requested.
//! \param data_size The data size to be stored.
//! \return A pointer on the data stored in the cache.
void *cache_get(cache_id_t *id, size_t data_size)
{
  if (chache_test_id(id))
    return &(cache->data);
  cache_flush();
  if (!(cache = (cache_t *) calloc(sizeof(cache_t) + data_size, 1)))
    return NULL;
  memcpy((char *) &(cache->id), (char const *) id, sizeof(cache_id_t));
  return &(cache->data);
}

#endif

static bool ai_usb_ms_specific_async_cmd_task(cmd_ai_id_t cmd_id, uint32_t u32_in, uint16_t *u16_arg_size, char **u8_arg_in, uint32_t *u32_out, uint64_t *u64_out, ai_async_status_t *cmd_ai_status)
{
  bool ai_status = true;

  switch(cmd_id)
  {
  case CMD_AI_USB_MS_NAV_GET_NEXT_PLAYLIST_POS:
    *u32_out = ai_usb_ms_specific_nav_get_next_playlist_pos(u32_in, cmd_ai_status);
    break;
#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true
  case CMD_AI_USB_MS_NAV_PLAYLIST_LINK_NB:
    *u32_out = ai_usb_ms_specific_nav_playlist_link_nb(cmd_ai_status);
    break;
  case CMD_AI_USB_MS_NAV_PLAYLIST_LINK_NAME:
    *u16_arg_size = ai_usb_ms_specific_nav_playlist_link_name(u32_in, u8_arg_in, cmd_ai_status);
    break;
#endif
  case CMD_AI_USB_MS_AUDIO_NAV_PLAYLIST_PLAY_LINK:
    ai_status = ai_usb_ms_specific_audio_nav_playlist_play_link(u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_AUDIO_NAV_GET_FOLDER_NAME:
    *u16_arg_size = ai_usb_ms_specific_audio_nav_get_folder_name(u8_arg_in, u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_NAV_GET_PLAYLIST_FILE:
    ai_status = ai_usb_ms_specific_nav_getplaylistfile(cmd_ai_status);
    break;
  case CMD_AI_USB_MS_NAV_SAVE:
    ai_status = ai_usb_ms_specific_save_navigator_ex(FS_NAV_ID_FILESYSTEM, (navigator_context_t **) u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_NAV_RESTORE:
    ai_status = ai_usb_ms_specific_restore_navigator_ex(FS_NAV_ID_FILESYSTEM, (navigator_context_t **) u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_AUDIO_NAV_SAVE:
    ai_status = ai_usb_ms_specific_save_navigator_ex(FS_NAV_ID_AUDIO, (navigator_context_t **) u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_AUDIO_NAV_RESTORE:
    ai_status = ai_usb_ms_specific_restore_navigator_ex(FS_NAV_ID_AUDIO, (navigator_context_t **) u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_PLAYLIST_NAV_SAVE:
    ai_status = ai_usb_ms_specific_save_navigator_ex(FS_NAV_ID_PLAYLIST, (navigator_context_t **) u32_in, cmd_ai_status);
    break;
  case CMD_AI_USB_MS_PLAYLIST_NAV_RESTORE:
    ai_status = ai_usb_ms_specific_restore_navigator_ex(FS_NAV_ID_PLAYLIST, (navigator_context_t **) u32_in, cmd_ai_status);
    break;
  default:
    ai_status = false;
  }

  return ai_status;
}

static void ai_usb_ms_specific_save_navigator(navigator_context_t *nav_ctx)
{
  memcpy(&nav_ctx->fs_g_nav, &fs_g_nav, sizeof(Fs_management));
  memcpy(&nav_ctx->fs_g_nav_fast, &fs_g_nav_fast, sizeof(Fs_management_fast));
  memcpy(&nav_ctx->fs_g_nav_entry, &fs_g_nav_entry, sizeof(Fs_management_entry));
  if (nav_get() == FS_NAV_ID_PLAYLIST)
  {
    extern bool pl_g_list_is_open;
    extern uint16_t pl_g_u16_list_size;
    extern uint16_t pl_g_u16_list_sel;
    nav_ctx->pl_g_list_is_open = pl_g_list_is_open;
    nav_ctx->pl_g_u16_list_size = pl_g_u16_list_size;
    nav_ctx->pl_g_u16_list_sel = pl_g_u16_list_sel;
  }
}

static bool ai_usb_ms_specific_save_navigator_ex(uint8_t navigator_id, navigator_context_t **nav_ctx, ai_async_status_t *cmd_ai_status)
{
  uint8_t current_navigator_id;

  *cmd_ai_status = CMD_DONE;

  // Alloc memory for the context
  if (!(*nav_ctx = (navigator_context_t *) malloc(sizeof(navigator_context_t))))
    return false;

  // Save current context
  current_navigator_id = nav_get();
  nav_select(navigator_id);
  (*nav_ctx)->navigator_id = navigator_id;
  // Save the navigator
  ai_usb_ms_specific_save_navigator(*nav_ctx);
  // Restore context
  nav_select(current_navigator_id);

  return true;
}

static void ai_usb_ms_specific_restore_navigator(navigator_context_t *nav_ctx)
{
  memcpy(&fs_g_nav, &nav_ctx->fs_g_nav, sizeof(Fs_management));
  memcpy(&fs_g_nav_fast, &nav_ctx->fs_g_nav_fast, sizeof(Fs_management_fast));
  memcpy(&fs_g_nav_entry, &nav_ctx->fs_g_nav_entry, sizeof(Fs_management_entry));
  if (nav_ctx->navigator_id == FS_NAV_ID_PLAYLIST)
  {
    extern bool pl_g_list_is_open;
    extern uint16_t pl_g_u16_list_size;
    extern uint16_t pl_g_u16_list_sel;
    pl_g_list_is_open = nav_ctx->pl_g_list_is_open;
    pl_g_u16_list_size = nav_ctx->pl_g_u16_list_size;
    pl_g_u16_list_sel = nav_ctx->pl_g_u16_list_sel;
  }
}

static bool ai_usb_ms_specific_restore_navigator_ex(uint8_t navigator_id, navigator_context_t **nav_ctx, ai_async_status_t *cmd_ai_status)
{
  uint8_t current_navigator_id;

  *cmd_ai_status = CMD_DONE;

  if (!(*nav_ctx))
    return false;

  // Check if it is the same navigator
  if ((*nav_ctx)->navigator_id != navigator_id)
    return false;

  current_navigator_id = nav_get();
  nav_select((*nav_ctx)->navigator_id);

  ai_usb_ms_specific_restore_navigator(*nav_ctx);

  // Free the allocated memory
  free(*nav_ctx);
  *nav_ctx = NULL;

  nav_select(current_navigator_id);

  return true;
}

static bool ai_usb_ms_specific_audio_nav_get_folder_name(char **string, uint8_t u8_size_max, ai_async_status_t *cmd_ai_status)
{
  bool status;
  navigator_context_t nav_ctx;
  uint32_t string_length;

  *cmd_ai_status = CMD_DONE;

#if (FS_UNICODE == true)
  // Make sure the u8_size_max is a multiple of 2
  u8_size_max = u8_size_max & ~1;
#endif

  if (!(*string = (char *) malloc(u8_size_max)))
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }

  nav_select(FS_NAV_ID_AUDIO);
  ai_usb_ms_specific_save_navigator(&nav_ctx);

  file_close();

#if (FS_UNICODE == true)
  status = nav_dir_name(*string, u8_size_max / sizeof(uint16_t));
#else
  status = nav_dir_name(*string, u8_size_max);
#endif

  nav_select(FS_NAV_ID_AUDIO);
  ai_usb_ms_specific_restore_navigator(&nav_ctx);

  if (!status)
    return 0;

  // Compute the real length of the string
#if (FS_UNICODE == true)
  string_length = 0;
  while((string_length + 1 < u8_size_max) && ((*string)[string_length] || (*string)[string_length + 1]))
    string_length += 2;
#else
  string_length = 0;
  while((string_length < u8_size_max) && (*string)[string_length])
    string_length++;
#endif

  return string_length;
}

//! Return the next playlist on the current directory
//!
//! @param  u16_start_file_pos  The starting file position where to start searching for the next playlist.
//!                             This value has to be set to ((uint16_t) -1) at the beginning of the list.
//!
//! @return The next playlist position found.
//!         If no playlist are found, returns ((uint16_t) -1).
//!
//! @note This function does not restore the position of the FILESYSTEM navigator.\n
//!       This has been done to avoid audio blips.
//!
static uint16_t ai_usb_ms_specific_nav_get_next_playlist_pos(uint16_t u16_start_file_pos, ai_async_status_t *cmd_ai_status)
{
  nav_select(FS_NAV_ID_FILESYSTEM);

  // Set the cursor to the starting file
  if (!nav_filterlist_goto(u16_start_file_pos + 1))
  {
    *cmd_ai_status = CMD_ERROR;
    return ((uint16_t) -1);
  }

  // Look for the next playlist
  do
  {
    if (!nav_file_isdir() && nav_file_checkext(ai_usb_ms_filter_playlist))
    {
      uint16_t u16;
      // Read the file position
      u16 = nav_filterlist_get();
      // Restore the index
      *cmd_ai_status = CMD_DONE;
      return u16;
    }
  } while(nav_filterlist_next());

  // Here means no playlist has been found
  *cmd_ai_status = CMD_ERROR;
  return ((uint16_t) -1);
}

#if defined(PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET) && \
    PLAYLIST_USE_NON_BLOCKING_FUNCTION_SET == true

static void *ai_usb_ms_specific_save_playlist_navigator(void)
{
  navigator_context_t *ctx;
  ai_async_status_t cmd_ai_status;

  if (!ai_usb_ms_specific_save_navigator_ex(FS_NAV_ID_PLAYLIST, &ctx, &cmd_ai_status))
    return NULL;

  // Make sure the playlist is closed
  if (pl_main_isopen())
  {
    Fs_index index;
    // Save the index because pl_main_close changes it
    index = nav_getindex();
    pl_main_close();
    nav_gotoindex(&index);
  }

  return (void *) ctx;
}


static void ai_usb_ms_specific_restore_playlist_navigator(void *ctx_save)
{
  navigator_context_t *ctx = (navigator_context_t *) ctx_save;
  ai_async_status_t cmd_ai_status;

  ai_usb_ms_specific_restore_navigator_ex(FS_NAV_ID_PLAYLIST, &ctx, &cmd_ai_status);
}


//! This function closes the play list
//!
//! @retval    false in case of error, see global value "fs_g_status" for more detail
//! @retval    true otherwise
//!
static bool pl_main_specific_close(void)
{
  uint8_t nav_id_save;
  extern bool pl_g_list_is_open;

  if (!pl_main_isopen())
    return false;

  // Close play list
  nav_id_save = nav_get();
  nav_select(FS_NAV_ID_PLAYLIST);
  file_close();

  // Select the play list file in current navigator
  nav_select(nav_id_save);

  pl_g_list_is_open = false;
  return true;
}

//! Returns the number of links inside the currently selected playlist
//!
//! @return The number of links inside the currently selected playlist
//!
static uint16_t ai_usb_ms_specific_nav_playlist_link_nb(ai_async_status_t *cmd_ai_status)
{
  typedef struct
  {
    void *ctx_save;
    uint16_t u16_nb_links;
    Fs_index index;
  } s_context;
  s_context *p_c = (s_context *)g_context;
  uint16_t u16_temp;
  bool b_status;
  g_smooth_com_cnt = 20;

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true && \
    defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NB)
  uint16_t *p_u16_temp;
  cache_id_t id;
#endif

  *cmd_ai_status = CMD_DONE;
  switch (g_Cmd_state)
  {
  case 0:
    nav_select(FS_NAV_ID_FILESYSTEM);

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true && \
    defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NB)
    // Check in the cache if this entry is already present.
    // If not creates this entry in the cache.
    cache_get_id("ai_usb_ms_specific_nav_playlist_link_nb", &id);
    cache_update_id(fs_g_nav_entry.u32_cluster, &id);
    b_status = chache_test_id(&id);
    if (!(p_u16_temp = (uint16_t *) cache_get(&id, sizeof(uint16_t))))
    {
      *cmd_ai_status = CMD_ERROR;
      return 0;
    }
    if (b_status)
      return *p_u16_temp;
#endif

    // Malloc memory for local variables
    if (!(p_c = (s_context *)alloc_context(sizeof(s_context))))
    {
      *cmd_ai_status = CMD_ERROR;
      return 0;
    }
    // Save current index
    p_c->index = nav_getindex();

    // Save the current context of the playlist navigator to be able to navigate in the playlist
    // while another one is being played.
    p_c->ctx_save = ai_usb_ms_specific_save_playlist_navigator();
    if (!p_c->ctx_save)
    {
      *cmd_ai_status = CMD_ERROR;
      free_context();
      return 0;
    }
    g_Cmd_state++;
    *cmd_ai_status = CMD_EXECUTING;

    return 0;

  case 1:
    nav_select(FS_NAV_ID_FILESYSTEM);

    b_status = pl_main_open_non_blocking(1);
    ai_async_update_cmd_status(cmd_ai_status);
    if (*cmd_ai_status == CMD_EXECUTING)
      return 0;

    if (!b_status)
    {
      // Restore navigator's index
      nav_gotoindex(&p_c->index);
      ai_usb_ms_specific_restore_playlist_navigator(p_c->ctx_save);
      *cmd_ai_status = CMD_ERROR;
      g_Cmd_state = 0;
      free_context();
      return 0;
    }

    p_c->u16_nb_links = pl_nav_getnbfile();

    pl_main_specific_close();
    g_Cmd_state++;
    *cmd_ai_status = CMD_EXECUTING;
    return 0;

  case 2:
    nav_select(FS_NAV_ID_FILESYSTEM);

    ai_usb_ms_specific_restore_playlist_navigator(p_c->ctx_save);

    // Restore navigator's index
    nav_gotoindex(&p_c->index);

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true && \
    defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NB)
    nav_select(FS_NAV_ID_FILESYSTEM);
    cache_get_id("ai_usb_ms_specific_nav_playlist_link_nb", &id);
    cache_update_id(fs_g_nav_entry.u32_cluster, &id);
    if (!(p_u16_temp = (uint16_t *) cache_get(&id, sizeof(uint16_t))))
    {
      *cmd_ai_status = CMD_ERROR;
      return 0;
    }
    *p_u16_temp = p_c->u16_nb_links;
#endif

  default:
    g_Cmd_state = 0;
  }

  u16_temp = p_c->u16_nb_links;
  free_context();
  *cmd_ai_status = CMD_DONE;

  return u16_temp;
}


//! Returns the name of the link inside the currently selected playlist
//!
//! @param  u16_link_pos  The position in the playlist of the link to select
//! @param  string        The location where has to be stored the address of the output buffer.
//!
//! @return cmd_out_status - false in case of error, true is success
//!
static uint16_t ai_usb_ms_specific_nav_playlist_link_name(uint16_t u16_link_pos, char **string, ai_async_status_t *cmd_ai_status)
{
  typedef struct
  {
    void *ctx_save;
    uint16_t string_length;
    Fs_index index;
    pl_nav_readentry_context_t *readentry_data_ctx;
  } s_context;
  s_context *p_c = (s_context *)g_context;
  bool b_status;
  int i;
  g_smooth_com_cnt = 20;

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true && \
    defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NAME)
  cache_id_t id;
#endif

  *cmd_ai_status = CMD_DONE;
  switch (g_Cmd_state)
  {
  case 0:
    // Malloc memory for local variables
    if (!(p_c = (s_context *)alloc_context(sizeof(s_context))))
    {
      *cmd_ai_status = CMD_ERROR;
      return 0;
    }

    // Select the disk navigator
    nav_select(FS_NAV_ID_FILESYSTEM);

#if defined(AI_USB_MS_USE_CACHE) && AI_USB_MS_USE_CACHE == true && \
    defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NAME)
    cache_get_id("ai_usb_ms_specific_nav_playlist_link_name", &id);
    cache_update_id(fs_g_nav_entry.u32_cluster, &id);
    if (!(p_c->readentry_data_ctx = (pl_nav_readentry_context_t *) cache_get(&id, sizeof(pl_nav_readentry_context_t))))
    {
      ai_usb_ms_specific_restore_playlist_navigator(p_c->ctx_save);
      // Restore navigator's index
      nav_gotoindex(&p_c->index);
      fs_g_nav.b_mode_nav_single = false;
      pl_main_close();
      *cmd_ai_status = CMD_ERROR;
      g_Cmd_state = 0;
      free_context();
      return 0;
    }
#endif

    // Ugly Hack for specific file system management
    fs_g_nav.b_mode_nav_single = false;
    fs_g_nav.b_mode_nav = FS_DIR;
    // Hack

    // Save current index
    p_c->index = nav_getindex();

    // Save the current context of the playlist navigator to be able to navigate in the playlist
    // while another one is being played.
    p_c->ctx_save = ai_usb_ms_specific_save_playlist_navigator();
    if (!p_c->ctx_save)
    {
      *cmd_ai_status = CMD_ERROR;
      fs_g_nav.b_mode_nav_single = false;
      free_context();
      return 0;
    }
    g_Cmd_state++;
    *cmd_ai_status = CMD_EXECUTING;
    return 0;

  case 1:
    nav_select(FS_NAV_ID_FILESYSTEM);

    b_status = pl_main_open_non_blocking(0);
    ai_async_update_cmd_status(cmd_ai_status);
    if (*cmd_ai_status == CMD_EXECUTING)
      return 0;

    if (!b_status)
    {
      ai_usb_ms_specific_restore_playlist_navigator(p_c->ctx_save);
      // Restore navigator's index
      nav_gotoindex(&p_c->index);
      fs_g_nav.b_mode_nav_single = false;
      *cmd_ai_status = CMD_ERROR;
      g_Cmd_state = 0;
      free_context();
      return 0;
    }

    g_Cmd_state++;
    *cmd_ai_status = CMD_EXECUTING;

#if !defined(AI_USB_MS_USE_CACHE) || AI_USB_MS_USE_CACHE != true || \
    !defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NAME)
    p_c->readentry_data_ctx = (pl_nav_readentry_context_t *) calloc(sizeof(pl_nav_readentry_context_t), 1);
#endif

    return 0;

  case 2:
    nav_select(FS_NAV_ID_FILESYSTEM);
    *string = NULL;

    b_status = pl_nav_readentry_non_blocking(p_c->readentry_data_ctx, u16_link_pos + 1, string, &p_c->string_length, 1);

    ai_async_update_cmd_status(cmd_ai_status);
    if (*cmd_ai_status == CMD_EXECUTING)
      return 0;

#if !defined(AI_USB_MS_USE_CACHE) || AI_USB_MS_USE_CACHE != true || \
    !defined(AI_USB_MS_CACHE_SPECIFIC_NAV_PLAYLIST_LINK_NAME)
    free(p_c->readentry_data_ctx);
#endif

    if (!b_status)
    {
      ai_usb_ms_specific_restore_playlist_navigator(p_c->ctx_save);
      // Restore navigator's index
      nav_gotoindex(&p_c->index);
      fs_g_nav.b_mode_nav_single = false;
      pl_main_close();
      *cmd_ai_status = CMD_ERROR;
      g_Cmd_state = 0;
      free_context();
      return 0;
    }

#if (FS_UNICODE == true)
    p_c->string_length *= sizeof(uint16_t);
#endif

    // Resize the string to keep only the file name.
    for(i=p_c->string_length-2; i>=0; i-=2)
    {
      // Look for the first '/' or '\' character
      if ((*string)[i] == 0 && ((*string)[i + 1] == '/' || (*string)[i + 1] == '\\'))
        break;
    }
    if (i >= 0)
    {
      p_c->string_length = p_c->string_length-i-2;
      memmove(*string, &(*string)[i+2], p_c->string_length);
    }

    pl_main_close();
    g_Cmd_state++;
    *cmd_ai_status = CMD_EXECUTING;
    return 0;

  case 3:
    nav_select(FS_NAV_ID_FILESYSTEM);
    // Restore previous playlist context
    ai_usb_ms_specific_restore_playlist_navigator(p_c->ctx_save);

    // Restore navigator's index
    nav_gotoindex(&p_c->index);

  default:
    g_Cmd_state = 0;
  }

  fs_g_nav.b_mode_nav_single = false;
  i = p_c->string_length;
  free_context();

  *cmd_ai_status = CMD_DONE;
  return (uint16_t)i;
}

#endif

//! Play the link inside the currently selected playlist
//!
//! @param  u16_link_pos  The position in the playlist of the link to select
//!
//! @return cmd_out_status - false in case of error, true is success
//!
static bool ai_usb_ms_specific_audio_nav_playlist_play_link(uint16_t u16_link_pos, ai_async_status_t *cmd_ai_status)
{
  Fs_index index;
  Fs_index index2;
  Navauto_rand rand_mode_save;

  *cmd_ai_status = CMD_DONE;

  // Wait Stop audio to execute next
  if (!ai_usb_ms_audio_ctrl_stop(cmd_ai_status))
  {
    *cmd_ai_status = CMD_EXECUTING;
    return false;
  }

  // Get the position selected in File System navigation
  nav_select(FS_NAV_ID_AUDIO);
  index2 = nav_getindex();

  // exit navigator automatic (in case of)
  navauto_close();
  g_ai_usb_ms_b_audio_file = false;

  nav_select(FS_NAV_ID_FILESYSTEM);
  nav_copy( FS_NAV_ID_AUDIO );
  nav_select( FS_NAV_ID_AUDIO );
  index = nav_getindex();

  // Disable the random mode if any
  rand_mode_save = navauto_getrand();
  navauto_setrand(NAVAUTO_RAND_OFF);
  //** Start play
  if (!navauto_open(true , u16_link_pos))
  {
    // restore the random mode
    navauto_setrand(rand_mode_save);
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  // restore the random mode
  navauto_setrand(rand_mode_save);

  g_ai_usb_ms_b_audio_file = true;
  g_player_flag.new_file_played = 1;

  if (index.u32_cluster_sel_dir != index2.u32_cluster_sel_dir)
    g_player_flag.new_directory = 1;

  // The link is invalid and it plays another file
  if (u16_link_pos != navauto_getpos())
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
#if defined(PAUSE_ON_TRACK_CHANGED) && PAUSE_ON_TRACK_CHANGED == true
  ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
  // Automatically pause new track
  return ai_usb_ms_audio_ctrl_pause(cmd_ai_status);
#else
  return ai_usb_ms_audio_ctrl_resume(cmd_ai_status);
#endif
}

static bool ai_usb_ms_specific_nav_getplaylistfile(ai_async_status_t *cmd_ai_status)
{
  bool status;
  Fs_index index;

  *cmd_ai_status = CMD_DONE;
  // Makes sure the playlist is still open
  if (!pl_main_isopen())
    return false;

  // Get the position selected in File System navigation
  nav_select(FS_NAV_ID_PLAYLIST);
  index = nav_getindex();
  nav_select(FS_NAV_ID_FILESYSTEM);
  status = nav_filterlist_gotoindex( &index );
  if (!status)
  {
    *cmd_ai_status = CMD_ERROR;
    return false;
  }
  return true;
}
