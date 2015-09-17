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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

//! Number of retries before aborting the command
extern   uint8_t    g_retry_cnt;

#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
typedef struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  union
  {
    uint8_t all;
    struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
    {
#define PLAYER_FLAG_STOP   0
#define PLAYER_FLAG_PLAY   1
#define PLAYER_FLAG_PAUSE  2
      uint8_t status             : 2;

#define PLAYER_FLAG_NORMAL  0
#define PLAYER_FLAG_FFW     1
#define PLAYER_FLAG_FRW     2
      uint8_t status_fast        : 2;

      uint8_t new_directory      : 1;
      uint8_t new_file_played    : 1;
    };
  };
} Ai_player_flag_t;
#if (defined __ICCAVR32__)
#pragma pack()
#endif

//! Structure to store the repeat option
typedef  uint8_t    Ai_repeat_mode;
//! \name Definition of the repeat modes
//! @{
#define  AUDIO_REPEAT_OFF            0
#define  AUDIO_REPEAT_TRACK          1
#define  AUDIO_REPEAT_FOLDER         2
#define  AUDIO_REPEAT_ALL            3
//! @}

//! Structure to store the shuffle option
typedef  uint8_t    Ai_shuffle_mode;
//! \name Definition of the shuffle modes
//! @{
#define  AUDIO_SHUFFLE_OFF           0
#define  AUDIO_SHUFFLE_FOLDER        1
#define  AUDIO_SHUFFLE_ALL           2
//! @}

//! Structure to store the explorer mode option
typedef  uint8_t    Ai_explorer_mode;
//! \name Definition of the explorer modes
//! @{
#define  AUDIO_EXPLORER_MODE_DISKS          0
#define  AUDIO_EXPLORER_MODE_DISK           1
#define  AUDIO_EXPLORER_MODE_DIRONLY        2
#define  AUDIO_EXPLORER_MODE_DIRSUB         3
//! @}

//! \name Definition of the song type
//! @{
#define  AUDIO_TYPE_UNKNOWN                 0
#define  AUDIO_TYPE_MP1                     1
#define  AUDIO_TYPE_MP2                     2
#define  AUDIO_TYPE_MP3                     3
#define  AUDIO_TYPE_WAV                     5
//! @}

typedef struct
{
  uint16_t width;
  uint16_t height;
} ai_image_size_t;

//! \name Internal ID commands used by ai_async_cmd_put()
//! @{
typedef enum
{
  CMD_AI_GET_DEVICE_STATUS,

  CMD_AI_GET_PRODUCT_ID,
  CMD_AI_GET_VENDOR_ID,
  CMD_AI_GET_SERIAL_NUMBER,

  CMD_AI_NAV_DRIVE_NB,
  CMD_AI_NAV_DRIVE_SET,
  CMD_AI_NAV_DRIVE_GET,
  CMD_AI_NAV_DRIVE_MOUNT,
  CMD_AI_NAV_DRIVE_TOTAL_SPACE,
  CMD_AI_NAV_DRIVE_FREE_SPACE,
  CMD_AI_NAV_DIR_ROOT,
  CMD_AI_NAV_DIR_CD,
  CMD_AI_NAV_DIR_GOTOPARENT,
  CMD_AI_NAV_DIR_NAME,
  CMD_AI_NAV_FILE_NAME,
  CMD_AI_NAV_FILE_ISDIR,
  CMD_AI_NAV_FILE_NEXT,
  CMD_AI_NAV_FILE_PREVIOUS,
  CMD_AI_NAV_FILE_GOTO,
  CMD_AI_NAV_FILE_POS,
  CMD_AI_NAV_FILE_NB,
  CMD_AI_NAV_DIR_NB,
  CMD_AI_NAV_PLAYLIST_NB,
  CMD_AI_NAV_GETPLAYFILE,

  CMD_AI_NAV_INFO_TYPE,
  CMD_AI_NAV_INFO_VERSION,
  CMD_AI_NAV_INFO_TITLE,
  CMD_AI_NAV_INFO_ARTIST,
  CMD_AI_NAV_INFO_ALBUM,
  CMD_AI_NAV_INFO_YEAR,
  CMD_AI_NAV_INFO_TRACK,
  CMD_AI_NAV_INFO_GENRE,
  CMD_AI_NAV_INFO_DURATION,
  CMD_AI_NAV_INFO_IMAGE,

  CMD_AI_AUDIO_CONTEXT_SAVE,
  CMD_AI_AUDIO_CONTEXT_RESTORE,

  CMD_AI_AUDIO_NAV_PLAYFILE,
  CMD_AI_AUDIO_NAV_NEXT,
  CMD_AI_AUDIO_NAV_PREV,
  CMD_AI_AUDIO_NAV_EOF_OCCUR,
  CMD_AI_AUDIO_NAV_NB,
  CMD_AI_AUDIO_NAV_GETPOS,
  CMD_AI_AUDIO_NAV_SETPOS,
  CMD_AI_AUDIO_NAV_GETNAME,
  CMD_AI_AUDIO_NAV_REPEAT_GET,
  CMD_AI_AUDIO_NAV_REPEAT_SET,
  CMD_AI_AUDIO_NAV_EXPMODE_GET,
  CMD_AI_AUDIO_NAV_EXPMODE_SET,
  CMD_AI_AUDIO_NAV_SHUFFLE_GET,
  CMD_AI_AUDIO_NAV_SHUFFLE_SET,

  CMD_AI_AUDIO_NAV_INFO_TYPE,
  CMD_AI_AUDIO_NAV_INFO_VERSION,
  CMD_AI_AUDIO_NAV_INFO_TITLE,
  CMD_AI_AUDIO_NAV_INFO_ARTIST,
  CMD_AI_AUDIO_NAV_INFO_ALBUM,
  CMD_AI_AUDIO_NAV_INFO_YEAR,
  CMD_AI_AUDIO_NAV_INFO_TRACK,
  CMD_AI_AUDIO_NAV_INFO_GENRE,
  CMD_AI_AUDIO_NAV_INFO_DURATION,
  CMD_AI_AUDIO_NAV_INFO_IMAGE,

  CMD_AI_AUDIO_CTRL_STOP,
  CMD_AI_AUDIO_CTRL_RESUME,
  CMD_AI_AUDIO_CTRL_TIME,
  CMD_AI_AUDIO_CTRL_STATUS,
  CMD_AI_AUDIO_CTRL_FFW,
  CMD_AI_AUDIO_CTRL_FRW,
  CMD_AI_AUDIO_CTRL_PAUSE,
  CMD_AI_AUDIO_CTRL_START_FFW,
  CMD_AI_AUDIO_CTRL_START_FRW,
  CMD_AI_AUDIO_CTRL_STOP_FFW_FRW,

  CMD_AI_SPECIFIC_ID_START
} cmd_ai_id_t;
//! @}

//! Structure Context which includes all information about audio
typedef enum
{
  CMD_DONE      = 0,
  CMD_EXECUTING = 1,
  CMD_ERROR     = 2
} ai_async_status_t;

typedef struct
{
  ai_async_status_t status;
  uint32_t state;
} ai_async_context_t;

typedef union
{
  uint32_t all;
  struct
  {
    uint8_t major;
    uint8_t minor;
    uint8_t revision;
  } digit;
} St_ai_info_version;

typedef enum
{
  AI_DEVICE_STATUS_NOT_PRESENT = 0,
  AI_DEVICE_STATUS_PRESENT,
  AI_DEVICE_STATUS_NOT_SUPPORTED,
  AI_DEVICE_STATUS_ERROR,
  AI_DEVICE_STATUS_CONNECTED
} ai_device_status_t;

typedef struct
{
                  void (*init)(void);
    ai_device_status_t (*get_device_status)     (                  ai_async_status_t * );
              uint16_t (*get_product_id)        (                  ai_async_status_t * );
              uint16_t (*get_vendor_id)         (                  ai_async_status_t * );
              uint16_t (*get_serial_number)     ( char**,          ai_async_status_t * );
               uint8_t (*nav_drive_nb)          (                  ai_async_status_t * );
                  bool (*nav_drive_set)         ( uint8_t,              ai_async_status_t * );
               uint8_t (*nav_drive_get)         (                  ai_async_status_t * );
                  bool (*nav_drive_mount)       (                  ai_async_status_t * );
              uint64_t (*nav_drive_total_space) (                  ai_async_status_t * );
              uint64_t (*nav_drive_free_space)  (                  ai_async_status_t * );
                  bool (*nav_dir_root)          (                  ai_async_status_t * );
                  bool (*nav_dir_cd)            (                  ai_async_status_t * );
                  bool (*nav_dir_gotoparent)    (                  ai_async_status_t * );
                  bool (*nav_file_isdir)        (                  ai_async_status_t * );
                  bool (*nav_file_next)         (                  ai_async_status_t * );
                  bool (*nav_file_previous)     (                  ai_async_status_t * );
                  bool (*nav_file_goto)         ( uint32_t,        ai_async_status_t * );
              uint32_t (*nav_file_pos)          (                  ai_async_status_t * );
              uint16_t (*nav_file_nb)           (                  ai_async_status_t * );
              uint16_t (*nav_dir_nb)            (                  ai_async_status_t * );
              uint16_t (*nav_playlist_nb)       (                  ai_async_status_t * );
              uint16_t (*nav_dir_name)          ( char**,          ai_async_status_t * );
              uint16_t (*nav_file_name)         ( char**,          ai_async_status_t * );
              uint32_t (*nav_file_info_type)    (                  ai_async_status_t * );
    St_ai_info_version (*nav_file_info_version) (                  ai_async_status_t * );
              uint16_t (*nav_file_info_title)   ( char**,          ai_async_status_t * );
              uint16_t (*nav_file_info_artist)  ( char**,          ai_async_status_t * );
              uint16_t (*nav_file_info_album)   ( char**,          ai_async_status_t * );
              uint16_t (*nav_file_info_year)    (                  ai_async_status_t * );
              uint32_t (*nav_file_info_track)   (                  ai_async_status_t * );
              uint16_t (*nav_file_info_genre)   ( char**,          ai_async_status_t * );
              uint32_t (*nav_file_info_duration)(                  ai_async_status_t * );
                 void* (*nav_file_info_image)   ( ai_image_size_t*,ai_async_status_t * );
                  bool (*nav_getplayfile)       (                  ai_async_status_t * );

                  bool (*audio_nav_playfile)          (                   ai_async_status_t * );
                 void* (*audio_context_save)     ( uint8_t *, uint16_t *, ai_async_status_t * );
                  bool (*audio_context_restore)       ( void*,            ai_async_status_t * );
                  bool (*audio_nav_next)              (                   ai_async_status_t * );
                  bool (*audio_nav_previous)          (                   ai_async_status_t * );
                  bool (*audio_nav_eof_occur)         (                   ai_async_status_t * );
              uint16_t (*audio_nav_nb)                (                   ai_async_status_t * );
              uint16_t (*audio_nav_getpos)            (                   ai_async_status_t * );
                  bool (*audio_nav_setpos)            ( uint16_t,         ai_async_status_t * );
        Ai_repeat_mode (*audio_nav_repeat_get)        (                   ai_async_status_t * );
                  void (*audio_nav_repeat_set)        ( Ai_repeat_mode,   ai_async_status_t * );
      Ai_explorer_mode (*audio_nav_expmode_get)       (                   ai_async_status_t * );
                  void (*audio_nav_expmode_set)       ( Ai_explorer_mode, ai_async_status_t * );
       Ai_shuffle_mode (*audio_nav_shuffle_get)       (                   ai_async_status_t * );
                  void (*audio_nav_shuffle_set)       ( Ai_shuffle_mode,  ai_async_status_t * );
              uint16_t (*audio_nav_getname)           ( char**,           ai_async_status_t * );
              uint32_t (*audio_nav_file_info_type)    (                   ai_async_status_t * );
    St_ai_info_version (*audio_nav_file_info_version) (                   ai_async_status_t * );
              uint16_t (*audio_nav_file_info_title)   ( char**,           ai_async_status_t * );
              uint16_t (*audio_nav_file_info_artist)  ( char**,           ai_async_status_t * );
              uint16_t (*audio_nav_file_info_album)   ( char**,           ai_async_status_t * );
              uint16_t (*audio_nav_file_info_year)    (                   ai_async_status_t * );
              uint32_t (*audio_nav_file_info_track)   (                   ai_async_status_t * );
              uint16_t (*audio_nav_file_info_genre)   ( char**,           ai_async_status_t * );
              uint32_t (*audio_nav_file_info_duration)(                   ai_async_status_t * );
                 void* (*audio_nav_file_info_image)   ( ai_image_size_t*, ai_async_status_t * );

                  bool (*audio_ctrl_stop)        (           ai_async_status_t * );
                  bool (*audio_ctrl_resume)      (           ai_async_status_t * );
                  bool (*audio_ctrl_pause)       (           ai_async_status_t * );
              uint32_t (*audio_ctrl_time)        (           ai_async_status_t * );
      Ai_player_flag_t (*audio_ctrl_status)      (           ai_async_status_t * );
                  bool (*audio_ctrl_ffw)         ( uint32_t, ai_async_status_t * );
                  bool (*audio_ctrl_frw)         ( uint32_t, ai_async_status_t * );
                  bool (*audio_ctrl_start_ffw)   (           ai_async_status_t * );
                  bool (*audio_ctrl_start_frw)   (           ai_async_status_t * );
                  bool (*audio_ctrl_stop_ffw_frw)(           ai_async_status_t * );

                  bool (*specific_async_cmd_task)(cmd_ai_id_t, uint32_t, uint16_t *, char **, uint32_t *, uint64_t *, ai_async_status_t *);

}ai_fct_set;

//! \name Empty functions
//! The following describes empty entries which can be used if a feature is not available on a specific interface
//! @{
static inline void ai_init_null(void) {};
static inline ai_device_status_t ai_get_device_status_null(ai_async_status_t *status) { *status = CMD_DONE; return AI_DEVICE_STATUS_NOT_PRESENT; };
static inline uint16_t ai_get_product_id_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_get_vendor_id_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_get_serial_number_null(char **str, ai_async_status_t *status) { *str = ""; *status = CMD_DONE; return 0; };
static inline uint8_t ai_nav_drive_nb_null(ai_async_status_t *status) { *status = CMD_DONE; return 1; };
static inline bool ai_nav_drive_set_null(uint8_t n, ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline uint8_t ai_nav_drive_get_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline bool ai_nav_drive_mount_null(ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline uint64_t ai_nav_drive_total_space_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint64_t ai_nav_drive_free_space_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline bool ai_nav_dir_root_null(ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline bool ai_nav_dir_cd_null(ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline bool ai_nav_dir_gotoparent_null(ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline bool ai_nav_file_isdir_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_nav_file_next_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_nav_file_previous_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_nav_file_goto_null(uint32_t n, ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline uint32_t ai_nav_file_pos_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_file_nb_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_dir_nb_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_playlist_nb_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_dir_name_null(char **str, ai_async_status_t *status) { *str = ""; *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_file_name_null(char **str, ai_async_status_t *status) { *str = ""; *status = CMD_DONE; return 0; };
static inline uint32_t ai_nav_file_info_type_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline St_ai_info_version ai_nav_file_info_version_null(ai_async_status_t *status) { St_ai_info_version v = { .all = 0 }; *status = CMD_DONE; return v; };
static inline uint16_t ai_nav_file_info_title_null(char **str, ai_async_status_t *status) { *str = ""; *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_file_info_artist_null(char **str, ai_async_status_t *status) { *str = ""; *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_file_info_album_null(char **str, ai_async_status_t *status) { *str = ""; *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_file_info_year_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint32_t ai_nav_file_info_track_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_nav_file_info_genre_null(char **str, ai_async_status_t *status) { *str = "", *status = CMD_DONE; return 0; };
static inline uint32_t ai_nav_file_info_duration_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline void *ai_nav_file_info_image_null(ai_image_size_t *img, ai_async_status_t *status) { *status = CMD_DONE; return NULL; };
static inline bool ai_nav_getplayfile_null(ai_async_status_t *status) { *status = CMD_DONE; return true; };

static inline bool ai_audio_nav_playfile_null(ai_async_status_t *status) { *status = CMD_DONE; return true; };
static inline void *ai_audio_context_save_null(uint8_t *a, uint16_t *b, ai_async_status_t *status) { *status = CMD_DONE; return NULL; };
static inline bool ai_audio_context_restore_null(void *c, ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_nav_next_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_nav_previous_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_nav_eof_occur_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline uint16_t ai_audio_nav_nb_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_audio_nav_getpos_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline bool ai_audio_nav_setpos_null(uint16_t n, ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline Ai_repeat_mode ai_audio_nav_repeat_get_null(ai_async_status_t *status) { *status = CMD_DONE; return AUDIO_REPEAT_OFF; };
static inline void ai_audio_nav_repeat_set_null(Ai_repeat_mode m, ai_async_status_t *status) { *status = CMD_DONE; };
static inline Ai_explorer_mode ai_audio_nav_expmode_get_null(ai_async_status_t *status) { *status = CMD_DONE; return AUDIO_EXPLORER_MODE_DISK; };
static inline void ai_audio_nav_expmode_set_null(Ai_explorer_mode m, ai_async_status_t *status) { *status = CMD_DONE; };
static inline Ai_shuffle_mode ai_audio_nav_shuffle_get_null(ai_async_status_t *status) { *status = CMD_DONE; return AUDIO_SHUFFLE_OFF; };
static inline void ai_audio_nav_shuffle_set_null(Ai_shuffle_mode m, ai_async_status_t *status) { *status = CMD_DONE; };
static inline uint16_t ai_audio_nav_getname_null(char **str, ai_async_status_t *status) { *str = "", *status = CMD_DONE; return 0; };
static inline uint32_t ai_audio_nav_file_info_type_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline St_ai_info_version ai_audio_nav_file_info_version_null(ai_async_status_t *status) { St_ai_info_version v = { .all = 0 }; *status = CMD_DONE; return v; };
static inline uint16_t ai_audio_nav_file_info_title_null(char **str, ai_async_status_t *status) { *str = "", *status = CMD_DONE; return 0; };
static inline uint16_t ai_audio_nav_file_info_artist_null(char **str, ai_async_status_t *status) { *str = "", *status = CMD_DONE; return 0; };
static inline uint16_t ai_audio_nav_file_info_album_null(char **str, ai_async_status_t *status) { *str = "", *status = CMD_DONE; return 0; };
static inline uint16_t ai_audio_nav_file_info_year_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint32_t ai_audio_nav_file_info_track_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline uint16_t ai_audio_nav_file_info_genre_null(char **str, ai_async_status_t *status) { *str = "", *status = CMD_DONE; return 0; };
static inline uint32_t ai_audio_nav_file_info_duration_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline void *ai_audio_nav_file_info_image_null(ai_image_size_t *img, ai_async_status_t *status) { *status = CMD_DONE; return NULL; };

static inline bool ai_audio_ctrl_stop_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_ctrl_resume_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_ctrl_pause_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline uint32_t ai_audio_ctrl_time_null(ai_async_status_t *status) { *status = CMD_DONE; return 0; };
static inline Ai_player_flag_t ai_audio_ctrl_status_null(ai_async_status_t *status) { Ai_player_flag_t flags; flags.all = 0; *status = CMD_DONE; return flags; };
static inline bool ai_audio_ctrl_ffw_null(uint32_t n, ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_ctrl_frw_null(uint32_t n, ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_ctrl_start_ffw_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_ctrl_start_frw_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };
static inline bool ai_audio_ctrl_stop_ffw_frw_null(ai_async_status_t *status) { *status = CMD_DONE; return false; };

static inline bool ai_specific_async_cmd_task_null(cmd_ai_id_t a, uint32_t b, uint16_t *c, char **d, uint32_t *e, uint64_t *f, ai_async_status_t *status) { *status = CMD_DONE; return false; };
//! @}

//! Initialize the Audio Interface.
//!
//! @param     filter_file     list of the files that will be filtered in the disk navigator.
//! @param     filter_songfile list of the audio files that will be filtered in the audio navigator.
//! @param     filter_file     list of the playlist files that will be filtered in the audio navigator.
//!
void  ai_init( void );

//! Task for the internal behaviors of the Audio Interface.
void  ai_task(void);

//! Returns true if no devices are connected
bool  ai_is_none(void);

//! Returns true if the current device is a USB mass storage device; false otherwise
bool  ai_is_usb_ms(void);

//! Returns true if the application is using hid class; false otherwise
bool ai_is_hid_class(void);

//! Returns true if the application is using sd/mmc card support; false otherwise
bool  ai_is_sd_mmc(void);

//! Aborts the current command
void ai_command_abort(void);

//! Tells the status of the device connected
ai_device_status_t ai_get_device_status(void);

//  -----------------------------------------------------------------------------------------------
//   OO                                    OO
//     O                                     O
//     O                                     O
//    O O    OOOOO  OOO OOO OO OO    OOOOO   O OO   OOO OO   OOOOO  OO OO    OOOOO  OO  OO   OOOOO
//    O O   O     O  O   O   OO  O  O     O  OO  O    OO  O O     O  OO  O  O     O  O   O  O     O
//   O   O   OOO     O   O   O   O  O        O   O    O     O     O  O   O  O     O  O   O   OOO
//   OOOOO      OO    O O    O   O  O        O   O    O     O     O  O   O  O     O  O   O      OO
//   O   O  O     O   O O    O   O  O     O  O   O    O     O     O  O   O  O     O  O  OO  O     O
//  OOO OOO  OOOOO     O    OOO OOO  OOOOO  OOO OOO OOOOO    OOOOO  OOO OOO  OOOOO    OO OO  OOOOO
//                     O
//                   OO
//
//  -----------------------------------------------------------------------------------------------


//! \name The following routines manage the asynchronous AI functions.
//! @{

//! Launch the execution (it can be a part of the job or its full execution)
//! of the current asynchronous command.
//!
//! @note Call this one when the application have a free time
//!
void  ai_async_cmd_task             ( void );

//! Tests if the current asynchronous routine is finished.
//!
//! @return true when the current asynchronous command is finished,
//!         false when the asynchronous command is still running.
//!
bool  is_ai_async_cmd_finished      ( void );

//! Returns the status of the last asynchronous command executed.
//!
//! @note this value is valid only if is_ai_async_cmd_finished() returns true
//!
//! @return status of command (which value depends on command)
//!
uint8_t    ai_async_cmd_out_status       ( void );

//! Return the result (max 32bits) of the last asynchronous command executed.
//!
//! @note this value is valid only if is_ai_async_cmd_finished() returns true.
//!
//! @return result of command (which value depends on command)
//!
uint32_t   ai_async_cmd_out_u32          ( void );

//! Return the result (max 64 bits) of the last asynchronous command executed.
//!
//! @note this value is valid only if is_ai_async_cmd_finished() returns true.
//!
//! @return result of command (which value depends on command)
//!
uint64_t   ai_async_cmd_out_u64          ( void );

//! Returns the size of the extra result (no size limit) of the last asynchronous executed command.
//! @note this value is valid only if is_ai_async_cmd_finished() return true
//!
//! @return 0 = no extra value, else get pointer on extra result with ai_async_cmd_out_PtrArrayU8() routine
//!
uint16_t   ai_async_cmd_out_SizeArrayU8  ( void );

//! Returns the pointer on extra result of the last asynchronous command executed.
//! @note this value is valid only if is_ai_async_cmd_finished() return true, and ai_async_cmd_out_SizeArrayU8() return != 0
//!
//! @return pointer on extra result
//!
char*   ai_async_cmd_out_PtrArrayU8   ( void );

//! Free the allocated buffer which holds the extra-result. This shall be done when the
//! "extra results" are no more needed by the application.
//!
void  ai_async_cmd_out_free_ArrayU8( void );

//! This routine may be use to send directly a command values from a communication port to audio interface
//! @note you can use the ai_async_cmd_put() routine or ai_async_X() MACRO to send a asynchronous command
//!
bool ai_async_cmd_put(cmd_ai_id_t cmd_id, uint32_t u32_in, uint16_t u16_arg_size, char* u8_arg_in, bool need_extra_buffer);
//! @}


//! \name Context specific commands
//! @{

//! \brief Push a new context into the stack.
//! \param ctx The new context to push.
//! \return true if the command succeed,\n
//!         false otherwise.
bool ai_async_context_push(ai_async_context_t *ctx);

//! \brief Pop a context from the stack.
//! \param ctx A pointer on an empty context structure to store the new context.
//! \return true if the command succeed,\n
//!         false otherwise.
bool ai_async_context_pop(ai_async_context_t *ctx);

//! \brief Update the current command status
//! \param cmd_ai_status The current command status to update
void ai_async_update_cmd_status(ai_async_status_t *cmd_ai_status);

//! @}

//! Here is the list of the ASYNCHRONOUS functions.
//!

//! Tells if a device is connected or not
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_is_device_connected( void )
{
  return ai_async_cmd_put( CMD_AI_GET_DEVICE_STATUS         , 0, 0, 0, false);
}

//! Return the device identifier
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_get_product_id( void )
{
  return ai_async_cmd_put( CMD_AI_GET_PRODUCT_ID              , 0, 0, 0, false);
}

//! Return the vendor identifier
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_get_vendor_id( void )
{
  return ai_async_cmd_put( CMD_AI_GET_VENDOR_ID              , 0, 0, 0, false);
}

//! Return the serial number of the connected device
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_get_serial_number( void )
{
  return ai_async_cmd_put( CMD_AI_GET_SERIAL_NUMBER          , 0, 0, 0, true);
}

//! Return the number of drives.
//!
//! @return    cmd_out_u32    - number of drives, 0 = NO DEVICE AVAILABLE
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_drive_nb( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DRIVE_NB           , 0, 0, 0, false);
}

//! Selects the drive in current navigator but do not mount it.
//!
//! @param     u8_number    device number (0 to nav_drive_nb()-1 )
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_drive_set( uint8_t u8_number )
{
  return ai_async_cmd_put( CMD_AI_NAV_DRIVE_SET          , (uint32_t)u8_number, 0, 0, false);
}

//! Returns the number of the selected drive.
//!
//! @return    cmd_out_u32    - 0 to nav_drive_nb()-1
//! @return    cmd_out_u32    - 0xFF in case of no drive selected
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_drive_get        ( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DRIVE_GET          , 0, 0, 0, false);
}

//! Mounts the current drive selected.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_drive_mount      ( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DRIVE_MOUNT        , 0, 0, 0, false);
}

//! Get the total space in bytes available on the device
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_drive_total_space( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DRIVE_TOTAL_SPACE  , 0, 0, 0, false);
}

//! Get the free space in bytes available on the device
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_drive_free_space ( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DRIVE_FREE_SPACE   , 0, 0, 0, false);
}

//! Initializes the file list filtered on the root directory.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_dir_root( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DIR_ROOT           , 0, 0, 0, false);
}

//! Enters in the current directory selected in file list filtered.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_dir_cd( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DIR_CD             , 0, 0, 0, false);
}

//! Goes to the parent directory of current directory corresponding at file list filtered.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_dir_gotoparent( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DIR_GOTOPARENT     , 0, 0, 0, false);
}

//! Returns the type of the selected file.
//!
//! @return    cmd_out_u32    - true, it is a directory
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_isdir( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_ISDIR         , 0, 0, 0, false);
}

//! Goes to the next file of the filtered list.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_next( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_NEXT          , 0, 0, 0, false);
}

//! Goes to the previous file of the filtered list.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_previous( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_PREVIOUS      , 0, 0, 0, false);
}

//! Goes to a position in the filtered list.
//!
//! @param     u16_newpos     new position to select in file list, 0 is the first position
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_goto( uint32_t u32_newpos )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_GOTO          , (uint32_t)u32_newpos, 0, 0, false);
}

//! Returns the position of selected file in list filtered.
//!
//! @return    cmd_out_u32    - position of selected file in current directory, 0 is the first position
//! @return    cmd_out_u32    - FS_NO_SEL, in case of no file selected
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_pos( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_POS           , 0, 0, 0, false);
}

//! Computes the number of files in the filtered list.
//!
//! @return    cmd_out_u32    - number of file and directory present in list filtered
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_nb( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_NB            , 0, 0, 0, false);
}

//! Computes the number of directories in the filtered list.
//!
//! @return    cmd_out_u32    - number of file and directory present in list filtered
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_dir_nb( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DIR_NB             , 0, 0, 0, false);
}

//! Computes the number of playlist in the filtered list.
//!
//! @return    cmd_out_u32    - number of file and directory present in list filtered
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_playlist_nb( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_PLAYLIST_NB        , 0, 0, 0, false);
}

//! Returns the file name of current parent directory
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_dir_name( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_DIR_NAME           , 0, 0, 0, true);
}

//! Returns the file name of the current file selected.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_name        ( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_FILE_NAME          , 0, 0, 0, true);
}

//! Return the file extension of the current file selected.
//!
//! @return    cmd_out_u32    - type of the current file selected
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_type( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_TYPE , 0, 0, 0, false);
}

//! Return the version of the metadata storage method used for the selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_version( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_VERSION , 0, 0, 0, false);
}

//! Returns the title of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_title( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_TITLE   , 0, 0, 0, true);
}

//! Returns the artist of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_artist( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_ARTIST , 0, 0, 0, true);
}

//! Returns the album of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_album( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_ALBUM   , 0, 0, 0, true);
}

//! Returns the year of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_year( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_YEAR    , 0, 0, 0, false);
}

//! Returns the track name of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_track( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_TRACK   , 0, 0, 0, false);
}

//! Return the genre of the current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_genre( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_GENRE  , 0, 0, 0, true);
}

//! Returns the track duration of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_duration( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_DURATION   , 0, 0, 0, false);
}

//! Returns a pointer and image information of the embedded image of the selected track..
//!
//! @return    size - The real dimensions of the image.
//! @return    ai_async_cmd_out_u32 - pointer on the image data.
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_file_info_image( ai_image_size_t *size )
{
  return ai_async_cmd_put( CMD_AI_NAV_INFO_IMAGE, (uint32_t) size, 0, 0, false);
}

//! Position the File system navigation on the file play.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_nav_getplayfile( void )
{
  return ai_async_cmd_put( CMD_AI_NAV_GETPLAYFILE        , 0, 0, 0, false);
}

//! Starts the play of current file selected in File System navigation.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_playfile( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_PLAYFILE     , 0, 0, 0, false);
}

//! Gives the full audio context (player state, play time, repeat, shuffle, file played, explorer mode )
//! @note May be interesting before a power OFF.
//!
//! @return    cmd_out_SizeArrayU8 - size of St_audio_ctx structure
//! @return    cmd_out_PtrArrayU8  - pointer on St_audio_ctx structure
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_audio_context_save( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CONTEXT_SAVE     , 0, 0, 0, true);
}

//! Restores an audio context (possibly restart play).
//! @note May be interesting after a power ON.
//!
//! @param     ctx   structure with all information about context to restore
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_context_restore( void *ctx )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CONTEXT_RESTORE  , 0, 0, (char*)ctx, true);
}

//! Goes to the next file to play.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_next( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_NEXT         , 0, 0, 0, false);
}

//! Goes to the previous file to play.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_previous( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_PREV         , 0, 0, 0, false);
}

//! Start next file
//! @note Call this routine after the end of song
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
//! TODO: TBD
__always_inline static bool  ai_async_audio_nav_eof_occur( void )
{
   return ai_async_cmd_put( CMD_AI_AUDIO_NAV_EOF_OCCUR    ,0,0,0,false);
}

//! Number of song file present in current played directory
//! or the number of file present in played playlist.
//!
//! @return cmd_out_u32    - nb file in directory or play list
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_nb( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_NB           , 0, 0, 0, false);
}

//! Gives the position of play file
//!
//! @return cmd_out_u32    - 1 to nb file in directory or play list
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_getpos( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_GETPOS       , 0, 0, 0, false);
}


//! Change the position of play file
//!
//! @param u16_pos  position to set
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_setpos( uint16_t u16_pos )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_SETPOS       , (uint32_t)u16_pos, 0, 0, false);
}


//! Gets the repeat mode.
//!
//! @return cmd_out_u32    - repeat mode used (AUDIO_REPEAT_OFF, AUDIO_REPEAT_TRACK, AUDIO_REPEAT_FOLDER, AUDIO_REPEAT_ALL)
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_repeat_get( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_REPEAT_GET   , 0, 0, 0, false);
}

//! Sets the repeat mode.
//!
//! @param repeat_mode  repeat mode to select (AUDIO_REPEAT_OFF, AUDIO_REPEAT_TRACK, AUDIO_REPEAT_FOLDER, AUDIO_REPEAT_ALL)
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_repeat_set( Ai_repeat_mode repeat_mode  )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_REPEAT_SET   , (uint32_t)repeat_mode, 0, 0, false);
}

//! Gets the shuffle mode.
//!
//! @return    cmd_out_u32    - shuffle mode used (AUDIO_SHUFFLE_OFF, AUDIO_SHUFFLE_FOLDER, AUDIO_SHUFFLE_ALL)
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_shuffle_get( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_SHUFFLE_GET     , 0, 0, 0, false);
}

//! Sets the shuffle mode.
//!
//! @param shuffle   shuffle mode to select (AUDIO_SHUFFLE_OFF, AUDIO_SHUFFLE_FOLDER, AUDIO_SHUFFLE_ALL)
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_shuffle_set( Ai_shuffle_mode shuffle )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_SHUFFLE_SET     , (uint32_t)shuffle, 0, 0, false);
}

//! Returns the used mode.
//!
//! @return cmd_out_u32    - mode used (AUDIO_EXPLORER_MODE_DISKS, AUDIO_EXPLORER_MODE_DISK, AUDIO_EXPLORER_MODE_DIRONLY, AUDIO_EXPLORER_MODE_DIRSUB)
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_expmode_get( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_EXPMODE_GET  , 0, 0, 0, false);
}

//! Selects the loop mode.
//! @note It reinit the folder level at the current position
//!
//! @param exp_mode    mode to select (AUDIO_EXPLORER_MODE_DISKS, AUDIO_EXPLORER_MODE_DISK, AUDIO_EXPLORER_MODE_DIRONLY, AUDIO_EXPLORER_MODE_DIRSUB)
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_expmode_set( Ai_explorer_mode exp_mode )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_EXPMODE_SET  , (uint32_t)exp_mode, 0, 0, false);
}

//! Gets the name of current played file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_getname    ( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_GETNAME      , 0, 0, 0, true);
}

//! Return the file extension of the current file selected.
//!
//! @return    cmd_out_u32    - type of the current file selected
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_type( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_TYPE , 0, 0, 0, false);
}

//! Return the version of the metadata storage method used for the selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_version( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_VERSION , 0, 0, 0, false);
}

//! Returns title of current song file selected
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_title ( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_TITLE   , 0, 0, 0, true);
}

//! Returns the artist's name of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_artist( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_ARTIST , 0, 0, 0, true);
}

//! Returns the album name of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_audio_nav_file_info_album( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_ALBUM   , 0, 0, 0, true);
}

//! Returns the year of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_year( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_YEAR    , 0, 0, 0, false);
}

//! Returns the track info of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_track( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_TRACK   , 0, 0, 0, false);
}

//! Returns the genre of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_genre( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_GENRE   , 0, 0, 0, true);
}

//! Returns the track duration of current selected file.
//!
//! @return    cmd_out_SizeArrayU8 - number of character (unit 2B for UNICODE) allocated
//! @return    cmd_out_PtrArrayU8  - pointer on string allocated by the routine
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_duration( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_DURATION   , 0, 0, 0, false);
}

//! Returns a pointer and image information of the embedded image of the selected track..
//!
//! @return    size - The real dimensions of the image.
//! @return    ai_async_cmd_out_u32 - pointer on the image data.
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_nav_file_info_image( ai_image_size_t *size )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_NAV_INFO_IMAGE, (uint32_t) size, 0, 0, false);
}

//! Stops the player.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_ctrl_stop( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_STOP        , 0, 0, 0, false);
}

//! Plays the current audio file.
//! @note use it after a stop or a pause.
//!
//! @param     u32_time_start  time at start the play
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_ctrl_resume( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_RESUME      , 0, 0, 0, false);
}

//! Pause the current audio file.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_ctrl_pause( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_PAUSE        , 0, 0, 0, false);
}

//! Returns the current played time (ms).
//!
//! @return    cmd_out_u32    - current time
//! @return    cmd_out_status - false in case of error, true is success
//!
__always_inline static bool  ai_async_audio_ctrl_time( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_TIME        , 0, 0, 0, false);
}

//! Returns the current status of audio player.
//!
//! @return    cmd_out_u32    - field of bits (PLAYER_STATUS_PLAY)
//! @return    cmd_out_status - false in case of error, true is success
//!
//! TODO: Add more status flags (new file played, file stopped, TBD)...
//!
__always_inline static bool  ai_async_audio_ctrl_status( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_STATUS      , 0, 0, 0, false);
}

//! Fast Forward
//!
//! @param     u32_duration  duration of FFW in seconds
//!
//! @return    false in case of error
//!
__always_inline static bool  ai_async_audio_ctrl_ffw( uint32_t u32_duration )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_FFW      , u32_duration, 0, 0, false);
}

//! Fast Rewind
//!
//! @param     u32_duration  duration of FFW in seconds
//!
//! @return    false in case of error
//!
__always_inline static bool  ai_async_audio_ctrl_frw( uint32_t u32_duration )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_FRW      , u32_duration, 0, 0, false);
}

//! Start Fast Forward
//!
//! @return    false in case of error
//!
__always_inline static bool  ai_async_audio_ctrl_start_ffw( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_START_FFW      , 0, 0, 0, false);
}

//! Start Fast Rewind
//!
//! @return    false in case of error
//!
__always_inline static bool  ai_async_audio_ctrl_start_frw( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_START_FRW      , 0, 0, 0, false);
}

//! Stop FastForward/FastRewind
//!
//! @return    false in case of error
//!
__always_inline static bool  ai_async_audio_ctrl_stop_ffw_frw( void )
{
  return ai_async_cmd_put( CMD_AI_AUDIO_CTRL_STOP_FFW_FRW   , 0, 0, 0, false);
}



//  -----------------------------------------------------------------------------------------------
//  OOOOO                          OO
// O     O                          O
// O                                O
// O       OOO OOO OO OO    OOOOO   O OO   OOO OO   OOOOO  OO OO    OOOOO  OO  OO   OOOOO
//  OOOOO   O   O   OO  O  O     O  OO  O    OO  O O     O  OO  O  O     O  O   O  O     O
//       O  O   O   O   O  O        O   O    O     O     O  O   O  O     O  O   O   OOO
//       O   O O    O   O  O        O   O    O     O     O  O   O  O     O  O   O      OO
// O     O   O O    O   O  O     O  O   O    O     O     O  O   O  O     O  O  OO  O     O
//  OOOOO     O    OOO OOO  OOOOO  OOO OOO OOOOO    OOOOO  OOO OOO  OOOOO    OO OO  OOOOO
//            O
//          OO
//  -----------------------------------------------------------------------------------------------

//! Here is the list of the SYNCHRONOUS functions.
//!

//! Tells if a device is connected or not
//!
//! @return    true if a device is connected, false otherwise.
//!
ai_device_status_t  ai_get_device_status( void );

//! Return the product identifier
//!
//! @return    product ID
//!
uint16_t  ai_get_product_id( void );

//! Return the vendor identifier
//!
//! @return    vendor ID
//!
uint16_t  ai_get_vendor_id( void );

//! Return the serial number of the connected device
//!
//! @return    serial number
//!
uint16_t  ai_get_serial_number( char** sz_sn );

//! Return the number of drives.
//!
//! @return    number of drives, 0 = NO DEVICE AVAILABLE
//!
uint8_t    ai_nav_drive_nb( void );

//! Selects the drive in current navigator but do not mount it.
//!
//! @param     u8_number    device number (0 to nav_drive_nb()-1 )
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  ai_nav_drive_set( uint8_t u8_number );

//! Returns the number of the selected drive.
//!
//! @return    0 to nav_drive_nb()-1
//! @return    0xFF in case of no drive selected
//!
uint8_t    ai_nav_drive_get( void );

//! Mounts the current drive selected.
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! If the option FS_MULTI_PARTITION is disable
//! then the mount routine select the first partition supported by file system. <br>
//! After mount, the file list corresponding at the files and directories present in ROOT directory
//! @endverbatim
//!
bool  ai_nav_drive_mount( void );

//! Get the total space in bytes available on the device
//!
//! @return    the capacity of the drive in bytes
//!
uint64_t   ai_nav_drive_total_space( void );

//! Get the free space in bytes available on the device
//!
//! @return    the free space available on the drive in bytes
//!
uint64_t   ai_nav_drive_free_space ( void );

//! Initializes the file list filtered on the root directory.
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  ai_nav_dir_root( void );

//! Enters in the current directory selected in file list filtered.
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After this routine the file list change and contains the files of new current directory,
//! also no file is selected
//! @endverbatim
//!
bool  ai_nav_dir_cd( void );

//! Goes to the parent directory of current directory corresponding at file list filtered.
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
//! @verbatim
//! After, the file list change and contains the files of parent directory,
//! also the file selected is the old directory.
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
bool  ai_nav_dir_gotoparent( void );

//! Returns the type of the selected file.
//!
//! @return    true, it is a directory
//! @return    false, in other case
//!
bool  ai_nav_file_isdir( void );

//! Goes to the next file of the filtered list.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
bool  ai_nav_file_next( void );

//! Goes to the previous file of the filtered list.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
bool  ai_nav_file_previous( void );

//! Goes to a position in the filtered list.
//!
//! @param     u16_newpos     new position to select in file list, 0 is the first position
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
bool  ai_nav_file_goto( uint32_t u32_newpos );

//! Returns the position of selected file in list filtered.
//!
//! @return    position of selected file in current directory, 0 is the first position
//! @return    FS_NO_SEL, in case of no file selected
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_nav_file_pos( void );

//! Computes the number of files in the filtered list.
//!
//! @return    number of files present in list filtered
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_nav_file_nb( void );

//! Computes the number of directories in the filtered list.
//!
//! @return    number of directories present in list filtered
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_nav_dir_nb( void );

//! Computes the number of playlist in the filtered list.
//!
//! @return    number of playlists present in list filtered
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_nav_playlist_nb( void );

//! Return the file name of current parent directory
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_nav_dir_name         ( char** string );

//! Return the file name of current file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_nav_file_name        ( char** string );

//! Return the type of the song for the selected file
//!
//! @return    Type of file
//!
uint32_t   ai_nav_file_info_type ( void );

//! Return the version of the metadata storage method used for the selected file
//!
//! @return    Metadata version
//!
St_ai_info_version   ai_nav_file_info_version ( void );

//! Return title of current file selected
//!
//! @param     sz_title   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_nav_file_info_title  ( char** sz_title );

//! Return artist of current file selected
//!
//! @param     sz_artist   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_nav_file_info_artist( char** sz_artist );

//! Return album of current file selected
//!
//! @param     sz_album   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_nav_file_info_album  ( char** sz_album );

//! Return year of current file selected
//!
//! @return    year
//!
uint16_t   ai_nav_file_info_year   ( void );

//! Return track of current file selected
//!
//! @return    track number
//!
uint32_t   ai_nav_file_info_track  ( void );

//! Return genre of current file selected
//!
//! @param     sz_genre   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_nav_file_info_genre  ( char** sz_genre );

//! Return duration of current file selected
//!
//! @return    duration (ms)
//!
uint32_t   ai_nav_file_info_duration  ( void );

//! Returns a pointer and image information of the embedded image of the selected track..
//!
//! @param     size    The desire width and height of the decoded image.
//!
//! @param     size    The actual width and height of the decoded image.
//! @return    pointer on the image data. NULL in case of error.
//!
void *ai_nav_file_info_image(ai_image_size_t *size);

//! Position the File system navigation on the file play
//!
//! @return    false in case of error
//!
bool  ai_nav_getplayfile      ( void );

//! Start the play of current file selected in File System navigation
//!
//! @return    false in case of error
//!
bool  ai_audio_nav_playfile   ( void );

//! Give complete audio context (player state, play time, repeat, shuffle, file played, explorer mode )
//! @note May be interesting before a power OFF.
//!
//! @return    ctx   structure with all information about the current context
//!
void *ai_audio_context_save( uint8_t *status, uint16_t *size );

//! Restore a audio context (eventualy restart play).
//! @note May be interesting after a power ON.
//!
//! @param     ctx   structure with all information about context to restore
//!
//! @return    false in case of error
//!
bool  ai_audio_context_restore( void *ctx );

//! Go to next file to play
//!
//! @return    false in case of error
//!
bool  ai_audio_nav_next       ( void );

//! Go to previous file to play
//!
//! @return    false in case of error
//!
bool  ai_audio_nav_previous   ( void );

//! Start next file
//! @note Call this routine after the end of song
//!
//! @return    false in case of error
//!
bool  ai_audio_nav_eof_occur( void );

//! Number of song file present in current directory played
//! or the number of file present in playlist played.
//!
//! @return nb file in directory or play list
//!
uint16_t   ai_audio_nav_nb         ( void );

//! Give the position of play file
//!
//! @return 1 to nb file in directory or play list
//!
uint16_t   ai_audio_nav_getpos     ( void );

//! Set audio navigation at a position
//!
//! @param u16_pos  position to set
//!
//! @return    false in case of error
//!
bool  ai_audio_nav_setpos     ( uint16_t u16_pos );

//! Get repeat mode
//!
//! @return repeat mode used (AUDIO_REPEAT_OFF, AUDIO_REPEAT_TRACK, AUDIO_REPEAT_FOLDER, AUDIO_REPEAT_ALL)
//!
Ai_repeat_mode ai_audio_nav_repeat_get ( void );

//! Set repeat mode
//!
//! @param repeat_mode  repeat mode to select (AUDIO_REPEAT_OFF, AUDIO_REPEAT_TRACK, AUDIO_REPEAT_FOLDER, AUDIO_REPEAT_ALL)
//!
void  ai_audio_nav_repeat_set ( Ai_repeat_mode repeat_mode );

//! This function return the shuffle mode used
//!
//! @return shuffle mode used (AUDIO_SHUFFLE_OFF, AUDIO_SHUFFLE_FOLDER, AUDIO_SHUFFLE_ALL)
//!
Ai_shuffle_mode ai_audio_nav_shuffle_get( void );

//! This function selects the shuffle mode
//!
//! @param shuffle   shuffle mode to select (AUDIO_SHUFFLE_OFF, AUDIO_SHUFFLE_FOLDER, AUDIO_SHUFFLE_ALL)
//!
void  ai_audio_nav_shuffle_set( Ai_shuffle_mode shuffle );

//! This function returns the mode used.
//!
//! @return mode used (AUDIO_EXPLORER_MODE_DISKS, AUDIO_EXPLORER_MODE_DISK, AUDIO_EXPLORER_MODE_DIRONLY, AUDIO_EXPLORER_MODE_DIRSUB)
//!
Ai_explorer_mode ai_audio_nav_expmode_get( void );

//! This function selects the loop mode
//! @note It reinit the folder level at the current position
//!
//! @param exp_mode    mode to select (AUDIO_EXPLORER_MODE_DISKS, AUDIO_EXPLORER_MODE_DISK, AUDIO_EXPLORER_MODE_DIRONLY, AUDIO_EXPLORER_MODE_DIRSUB)
//!
void  ai_audio_nav_expmode_set( Ai_explorer_mode exp_mode );

//! Get name of current file played
//!
//! @param string  buffer to fill
//!
//! @return size of string, if no 0 then you must free it
//!
uint16_t   ai_audio_nav_getname    ( char** string );

//! Return the type of the song for the selected file
//!
//! @return    Type of file
//!
uint32_t   ai_audio_nav_file_info_type ( void );

//! Return the version of the metadata storage method used for the selected file
//!
//! @return    Metadata version
//!
St_ai_info_version   ai_audio_nav_file_info_version  ( void );

//! Return title of current song file selected
//!
//! @param     sz_title   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_audio_nav_file_info_title   ( char** sz_title );

//! Return artist of current song file selected
//!
//! @param     sz_artist   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_audio_nav_file_info_artist ( char** sz_artist );

//! Return album of current song file selected
//!
//! @param     sz_album   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_audio_nav_file_info_album   ( char** sz_album );

//! Return year of current song file selected
//!
//! @return    year
//!
uint16_t   ai_audio_nav_file_info_year    ( void );

//! Return track of current song file selected
//!
//! @return    track number
//!
uint32_t   ai_audio_nav_file_info_track   ( void );

//! Return genre of current song file selected
//!
//! @param     sz_genre   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_audio_nav_file_info_genre   ( char** sz_genre );

//! Return duration of current song file selected
//!
//! @return    duration (ms)
//!
uint32_t   ai_audio_nav_file_info_duration   ( void );

//! Returns a pointer and image information of the embedded image of the selected track..
//!
//! @param     size    The desire width and height of the decoded image.
//!
//! @param     size    The actual width and height of the decoded image.
//! @return    pointer on the image data. NULL in case of error.
//!
void *ai_audio_nav_file_info_image(ai_image_size_t *size);

//! Stop the player
//!
//! @return    true the player is stopped
//!
bool  ai_audio_ctrl_stop      ( void );

//! Resume the play of the current audio file
//! @note use it after a stop or a pause
//!
//! @param     u32_time_start  time at start the play
//!
//! @return    false in case of error
//!
bool  ai_audio_ctrl_resume    ( void );

//! Pause the current audio file
//!
//! @return    false in case of error
//!
bool  ai_audio_ctrl_pause      ( void );

//! Return current time played
//!
//! @return    current time
//!
uint32_t   ai_audio_ctrl_time      ( void );

//! Return the current status of player
//!
//! @return    field of bits (PLAYER_STATUS_PLAY)
//!
Ai_player_flag_t ai_audio_ctrl_status    ( void );

//! Fast Forward
//!
//! @param     u32_duration  duration of FFW in seconds
//!
//! @return    false in case of error
//!
bool    ai_audio_ctrl_ffw    ( uint32_t u32_duration );

//! Fast Rewind
//!
//! @param     u32_duration  duration of FRW in seconds
//!
//! @return    false in case of error
//!
bool    ai_audio_ctrl_frw    ( uint32_t u32_duration );

//! Start Fast Forward
//!
//! @return    false in case of error
//!
bool    ai_audio_ctrl_start_ffw    ( void );

//! Start Fast Rewind
//!
//! @return    false in case of error
//!
bool    ai_audio_ctrl_start_frw    ( void );

//! Stop FastForward/FastRewind
//!
//! @return    false in case of error
//!
bool    ai_audio_ctrl_stop_ffw_frw    ( void );

#endif  // _AUDIO_INTERFACE_H_
