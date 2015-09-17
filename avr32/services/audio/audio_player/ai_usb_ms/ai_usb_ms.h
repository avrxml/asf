/*****************************************************************************
 *
 * \file
 *
 * \brief Audio interface USB Mass Storage Services.
 *
 * This function should be connected to the audio interface to give access to the mass storage audio player
 * If you use this routines then you don't must use the other FileSystem routine provide in nav_auto.h, navigation.h, file.h, playlist.h
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

#ifndef _AI_USB_MS_H_
#define _AI_USB_MS_H_

#include "fs_com.h"
#include "fat.h"
#include "navigation.h"
#include "play_list.h"
#include "nav_filterlist.h"
#include "nav_automatic.h"
#include "audio_interface.h"
#include "conf_audio_interface.h"

typedef struct
{
  bool              b_valid;             //!< true if valid structure
  Ai_repeat_mode    repeat;              //!< Repeat mode
  Navauto_mode      exp_mode;            //!< Explorer mode
  Ai_shuffle_mode   shuffle;             //!< Shuffle mode
  Fs_index          index;               //!< FileSystem pointer on the last file play or last playlist
  uint16_t          u16_pos_level;       //!< Directory level OR position in the playlist, of last file
  Ai_player_flag_t  u8_audio_ctrl_status;//!< true if play is running
  uint32_t          u32_time;            //!< Time position in the last file when save context occur
  uint32_t          u32_file_pos;        //!< File position in the last file when save context occur
} St_audio_ctx;

//! Initialize the Mass Storage Audio Interface.
//!
//! @param     filter_file     list of the files that will be filtered in the disk navigator.
//! @param     filter_songfile list of the audio files that will be filtered in the audio navigator.
//! @param     filter_file     list of the playlist files that will be filtered in the audio navigator.
//!
void  ai_usb_ms_init( void );

//! Tells if a device is connected or not
//!
//! @return    The status of the device.
//!
ai_device_status_t ai_usb_ms_get_device_status( ai_async_status_t *cmd_ai_status );

//! Routine to notify the Audio Interface of a new device connection.
void  ai_usb_ms_new_connection( void );

//! Here is the list of the SYNCHRONOUS functions.
//!

//! Here is the list of the SYNCHRONOUS functions.
//!

//! Return the product ID of the connected USB mass storage device
//!
//! @return    product ID
//!
uint16_t   ai_usb_ms_get_product_id( ai_async_status_t *cmd_ai_status );

//! Return the vendor ID of the connected USB mass storage device
//!
//! @return    vendor ID
//!
uint16_t   ai_usb_ms_get_vendor_id( ai_async_status_t *cmd_ai_status );

//! Return the serial number of the connected USB mass storage device
//!
//! @param     sz_sn a string to store the serial number
//!
//! @return    the length in byte of the serial number
//!
uint16_t   ai_usb_ms_get_serial_number( char** sz_sn, ai_async_status_t *cmd_ai_status );

//! Return the number of drives.
//!
//! @return    number of drives, 0 = NO DEVICE AVAILABLE
//!
uint8_t    ai_usb_ms_nav_drive_nb( ai_async_status_t *cmd_ai_status );

//! Selects the drive in current navigator but do not mount it.
//!
//! @param     u8_number    device number (0 to nav_drive_nb()-1 )
//!
//! @return    false in case of error, see global value "fs_g_status" for more detail
//! @return    true otherwise
//!
bool  ai_usb_ms_nav_drive_set( uint8_t u8_number, ai_async_status_t *cmd_ai_status );

//! Returns the number of the selected drive.
//!
//! @return    0 to nav_drive_nb()-1
//! @return    0xFF in case of no drive selected
//!
uint8_t    ai_usb_ms_nav_drive_get( ai_async_status_t *cmd_ai_status );

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
bool  ai_usb_ms_nav_drive_mount( ai_async_status_t *cmd_ai_status );

//! Get the total space in bytes available on the device
//!
//! @return    the capacity of the drive in bytes
//!
uint64_t   ai_usb_ms_nav_drive_total_space( ai_async_status_t *cmd_ai_status );

//! Get the free space in bytes available on the device
//!
//! @return    the free space available on the drive in bytes
//!
uint64_t   ai_usb_ms_nav_drive_free_space ( ai_async_status_t *cmd_ai_status );

//! Initializes the file list filtered on the root directory.
//!
//! @return  false in case of error, see global value "fs_g_status" for more detail
//! @return  true otherwise
//!
bool  ai_usb_ms_nav_dir_root( ai_async_status_t *cmd_ai_status );

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
bool  ai_usb_ms_nav_dir_cd( ai_async_status_t *cmd_ai_status );

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
bool  ai_usb_ms_nav_dir_gotoparent( ai_async_status_t *cmd_ai_status );

//! Returns the type of the selected file.
//!
//! @return    true, it is a directory
//! @return    false, in other case
//!
bool  ai_usb_ms_nav_file_isdir( ai_async_status_t *cmd_ai_status );

//! Goes to the next file of the filtered list.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
bool  ai_usb_ms_nav_file_next( ai_async_status_t *cmd_ai_status );

//! Goes to the previous file of the filtered list.
//!
//! @return    cmd_out_status - false in case of error, true is success
//!
bool  ai_usb_ms_nav_file_previous( ai_async_status_t *cmd_ai_status );

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
bool  ai_usb_ms_nav_file_goto( uint32_t u32_newpos, ai_async_status_t *cmd_ai_status );

//! Returns the position of selected file in list filtered.
//!
//! @return    position of selected file in current directory, 0 is the first position
//! @return    FS_NO_SEL, in case of no file selected
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint32_t   ai_usb_ms_nav_file_pos( ai_async_status_t *cmd_ai_status );

//! Computes the number of files in the filtered list.
//!
//! @return    number of files present in list filtered
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_usb_ms_nav_file_nb( ai_async_status_t *cmd_ai_status );

//! Computes the number of directories in the filtered list.
//!
//! @return    number of directories present in list filtered
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_usb_ms_nav_dir_nb( ai_async_status_t *cmd_ai_status );

//! Computes the number of playlists in the filtered list.
//!
//! @return    number of playlists present in list filtered
//!
//! @verbatim
//! "Filter list" is a list of files filtered and directories present in current directory
//! @endverbatim
//!
uint16_t   ai_usb_ms_nav_playlist_nb( ai_async_status_t *cmd_ai_status );

//! Return the file name of current parent directory
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_nav_dir_name         ( char** string, ai_async_status_t *cmd_ai_status );

//! Return the file name of current file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_nav_file_name        ( char** string, ai_async_status_t *cmd_ai_status );

//! Return the type of the song for the selected file
//!
//! @return    Type of file
//!
uint32_t   ai_usb_ms_nav_file_info_type( ai_async_status_t *cmd_ai_status );

//! Return the version of the metadata storage method used for the selected file
//!
//! @return    Metadata version
//!
St_ai_info_version   ai_usb_ms_nav_file_info_version ( ai_async_status_t *cmd_ai_status );

//! Return title of current file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_nav_file_info_title  ( char** sz_title, ai_async_status_t *cmd_ai_status );

//! Return artist of current file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_nav_file_info_artist( char** sz_artist, ai_async_status_t *cmd_ai_status );

//! Return album of current file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_nav_file_info_album  ( char** sz_album, ai_async_status_t *cmd_ai_status );

//! Return year of current file selected
//!
//! @return    year
//!
uint16_t   ai_usb_ms_nav_file_info_year   ( ai_async_status_t *cmd_ai_status );

//! Return track of current file selected
//!
//! @return    track number
//!
uint32_t   ai_usb_ms_nav_file_info_track  ( ai_async_status_t *cmd_ai_status );

//! Return genre of current file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_nav_file_info_genre  ( char** sz_genre, ai_async_status_t *cmd_ai_status );

//! Return duration of current file selected
//!
//! @return    duration (ms)
//!
uint32_t   ai_usb_ms_nav_file_info_duration  ( ai_async_status_t *cmd_ai_status );

//! Returns a pointer and image information of the embedded image of the selected track..
//!
//! @param     size    The desire width and height of the decoded image.
//!
//! @return    pointer on the image data. NULL in case of error.
//!
void *ai_usb_ms_nav_file_info_image(ai_image_size_t *size, ai_async_status_t *cmd_ai_status);

//! Position the File system navigation on the file play
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_nav_getplayfile      ( ai_async_status_t *cmd_ai_status );

//! Start the play of current file selected in File System navigation
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_nav_playfile   ( ai_async_status_t *cmd_ai_status );

//! Give complete audio context (player state, play time, repeat, shuffle, file played, explorer mode )
//! @note May be interesting before a power OFF.
//!
//! @return    ctx   structure with all information about the current context
//!
void *ai_usb_ms_audio_context_save( uint8_t *status, uint16_t *size, ai_async_status_t *cmd_ai_status );

//! Restore a audio context (eventualy restart play).
//! @note May be interesting after a power ON.
//!
//! @param     ctx   structure with all information about context to restore
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_context_restore( void *ctx, ai_async_status_t *cmd_ai_status );

//! Go to next file to play
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_nav_next       ( ai_async_status_t *cmd_ai_status );

//! Go to previous file to play
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_nav_previous   ( ai_async_status_t *cmd_ai_status );

//! Start next file
//! @note Call this routine after the end of song
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_nav_eof_occur( ai_async_status_t *cmd_ai_status );

//! Number of song file present in current directory played
//! or the number of file present in playlist played.
//!
//! @return nb file in directory or play list
//!
uint16_t   ai_usb_ms_audio_nav_nb         ( ai_async_status_t *cmd_ai_status );

//! Give the position of play file
//!
//! @return 1 to nb file in directory or play list
//!
uint16_t   ai_usb_ms_audio_nav_getpos     ( ai_async_status_t *cmd_ai_status );

//! Set audio navigation at a position
//!
//! @param u16_pos  position to set
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_nav_setpos     ( uint16_t, ai_async_status_t *cmd_ai_status );

//! Get repeat mode
//!
//! @return repeat mode used (AUDIO_REPEAT_OFF, AUDIO_REPEAT_TRACK, AUDIO_REPEAT_FOLDER, AUDIO_REPEAT_ALL)
//!
Ai_repeat_mode ai_usb_ms_audio_nav_repeat_get ( ai_async_status_t *cmd_ai_status );

//! Set repeat mode
//!
//! @param repeat_mode  repeat mode to select (AUDIO_REPEAT_OFF, AUDIO_REPEAT_TRACK, AUDIO_REPEAT_FOLDER, AUDIO_REPEAT_ALL)
//!
void  ai_usb_ms_audio_nav_repeat_set ( Ai_repeat_mode repeat_mode, ai_async_status_t *cmd_ai_status );

//! This function returns the mode used.
//!
//! @return mode used (NAVAUTO_MODE_DISKS, NAVAUTO_MODE_DIRONLY, NAVAUTO_MODE_DIRSUB)
//!
Ai_explorer_mode ai_usb_ms_audio_nav_expmode_get( ai_async_status_t *cmd_ai_status );

//! This function selects the loop mode
//! @note It reinit the folder level at the current position
//!
//! @param exp_mode    mode to select (NAVAUTO_MODE_DISKS, NAVAUTO_MODE_DIRONLY, NAVAUTO_MODE_DIRSUB)
//!
void  ai_usb_ms_audio_nav_expmode_set( Ai_explorer_mode exp_mode, ai_async_status_t *cmd_ai_status );

//! This function return the shuffle mode used
//!
//! @return shuffle mode used (AUDIO_SHUFFLE_OFF, AUDIO_SHUFFLE_FOLDER, AUDIO_SHUFFLE_ALL)
//!
Ai_shuffle_mode ai_usb_ms_audio_nav_shuffle_get( ai_async_status_t *cmd_ai_status );

//! This function selects the shuffle mode
//!
//! @param shuffle   shuffle mode to select (AUDIO_SHUFFLE_OFF, AUDIO_SHUFFLE_FOLDER, AUDIO_SHUFFLE_ALL)
//!
void  ai_usb_ms_audio_nav_shuffle_set( Ai_shuffle_mode shuffle, ai_async_status_t *cmd_ai_status );

//! Get name of current file played
//!
//! @param string  buffer to fill
//!
//! @return size of string, if no 0 then you must free it
//!
uint16_t   ai_usb_ms_audio_nav_getname    ( char** string, ai_async_status_t *cmd_ai_status );

//! Return the type of the song for the selected file
//!
//! @return    Type of file
//!
uint32_t   ai_usb_ms_audio_nav_file_info_type( ai_async_status_t *cmd_ai_status );

//! Return the version of the metadata storage method used for the selected file
//!
//! @return    Metadata version
//!
St_ai_info_version   ai_usb_ms_audio_nav_file_info_version  ( ai_async_status_t *cmd_ai_status );

//! Return title of current song file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_audio_nav_file_info_title   ( char** sz_title, ai_async_status_t *cmd_ai_status );

//! Return artist of current song file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_audio_nav_file_info_artist ( char** sz_artist, ai_async_status_t *cmd_ai_status );

//! Return album of current song file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_audio_nav_file_info_album   ( char** sz_album, ai_async_status_t *cmd_ai_status );

//! Return year of current song file selected
//!
//! @return    year
//!
uint16_t   ai_usb_ms_audio_nav_file_info_year    ( ai_async_status_t *cmd_ai_status );

//! Return track of current song file selected
//!
//! @return    track number
//!
uint32_t   ai_usb_ms_audio_nav_file_info_track   ( ai_async_status_t *cmd_ai_status );

//! Return genre of current song file selected
//!
//! @param     string   pointer to store the string allocated by the routine
//!
//! @return    number of character (unit 2B for UNICODE) allocated
//!
uint16_t   ai_usb_ms_audio_nav_file_info_genre   ( char** sz_genre, ai_async_status_t *cmd_ai_status );

//! Return duration of current song file selected
//!
//! @return    duration (ms)
//!
uint32_t   ai_usb_ms_audio_nav_file_info_duration   ( ai_async_status_t *cmd_ai_status );

//! Returns a pointer and image information of the embedded image of the selected track..
//!
//! @param     size    The desire width and height of the decoded image.
//!
//! @return    pointer on the image data. NULL in case of error.
//!
void *ai_usb_ms_audio_nav_file_info_image(ai_image_size_t *size, ai_async_status_t *cmd_ai_status);

//! Stop the player
//!
//! @return    true the player is stopped
//!
bool  ai_usb_ms_audio_ctrl_stop      ( ai_async_status_t *cmd_ai_status );

//! Resume the play of the current audio file
//! @note use it after a stop or a pause
//!
//! @param     u32_time_start  time at start the play
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_ctrl_resume    ( ai_async_status_t *cmd_ai_status );

//! Pause the current audio file
//!
//! @return    false in case of error
//!
bool  ai_usb_ms_audio_ctrl_pause      ( ai_async_status_t *cmd_ai_status );

//! Return current time played (ms)
//!
//! @return    current time
//!
uint32_t   ai_usb_ms_audio_ctrl_time      ( ai_async_status_t *cmd_ai_status );

//! Return the current status of player
//!
//! @return    field of bits (PLAYER_STATUS_PLAY)
//!
Ai_player_flag_t ai_usb_ms_audio_ctrl_status    ( ai_async_status_t *cmd_ai_status );

//! Fast Forward
//!
//! @param     u32_duration  duration of FFW in seconds
//!
//! @return    false in case of error
//!
bool    ai_usb_ms_audio_ctrl_ffw    ( uint32_t u32_duration, ai_async_status_t *cmd_ai_status );

//! Fast Rewind
//!
//! @param     u32_duration  duration of FRW in seconds
//!
//! @return    false in case of error
//!
bool    ai_usb_ms_audio_ctrl_frw    ( uint32_t u32_duration, ai_async_status_t *cmd_ai_status );

//! Start Fast Forward
//!
//! @return    false in case of error
//!
bool ai_usb_ms_audio_ctrl_start_ffw(ai_async_status_t *cmd_ai_status);

//! Start Fast Rewind
//!
//! @return    false in case of error
//!
bool ai_usb_ms_audio_ctrl_start_frw(ai_async_status_t *cmd_ai_status);

//! Stop FastForward/FastRewind
//!
//! @return    false in case of error
//!
bool ai_usb_ms_audio_ctrl_stop_ffw_frw(ai_async_status_t *cmd_ai_status);

/****************************** Specific Functions *******************************/
#define CMD_AI_USB_MS_NAV_GET_NEXT_PLAYLIST_POS       (CMD_AI_SPECIFIC_ID_START + 0)
#define CMD_AI_USB_MS_NAV_PLAYLIST_LINK_NB            (CMD_AI_SPECIFIC_ID_START + 1)
#define CMD_AI_USB_MS_NAV_PLAYLIST_LINK_NAME          (CMD_AI_SPECIFIC_ID_START + 2)
#define CMD_AI_USB_MS_AUDIO_NAV_PLAYLIST_PLAY_LINK    (CMD_AI_SPECIFIC_ID_START + 3)
#define CMD_AI_USB_MS_AUDIO_NAV_GET_FOLDER_NAME       (CMD_AI_SPECIFIC_ID_START + 4)
#define CMD_AI_USB_MS_NAV_GET_PLAYLIST_FILE           (CMD_AI_SPECIFIC_ID_START + 5)
#define CMD_AI_USB_MS_NAV_SAVE                        (CMD_AI_SPECIFIC_ID_START + 6)
#define CMD_AI_USB_MS_NAV_RESTORE                     (CMD_AI_SPECIFIC_ID_START + 7)
#define CMD_AI_USB_MS_AUDIO_NAV_SAVE                  (CMD_AI_SPECIFIC_ID_START + 8)
#define CMD_AI_USB_MS_AUDIO_NAV_RESTORE               (CMD_AI_SPECIFIC_ID_START + 9)
#define CMD_AI_USB_MS_PLAYLIST_NAV_SAVE               (CMD_AI_SPECIFIC_ID_START + 10)
#define CMD_AI_USB_MS_PLAYLIST_NAV_RESTORE            (CMD_AI_SPECIFIC_ID_START + 11)

//! Return the next playlist on the current directory
//!
//! @param  u16_start_file_pos  The starting file position where to start searching for the next playlist.
//!                             This value has to be set to ((uint16_t) -1) at the beginning of the list.
//!
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_usb_ms_specific_nav_get_next_playlist_pos(uint16_t u16_start_file_pos)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_NAV_GET_NEXT_PLAYLIST_POS, (uint32_t) u16_start_file_pos, 0, 0, false);
}

//! Returns the number of links inside the currently selected playlist
//!
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_usb_ms_specific_nav_playlist_link_nb(void)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_NAV_PLAYLIST_LINK_NB, 0, 0, 0, false);
}

//! Returns the name of the link inside the currently selected playlist
//!
//! @param  u16_link_pos  The position in the playlist of the link to select
//!
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_usb_ms_specific_nav_playlist_link_name(uint16_t u16_link_pos)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_NAV_PLAYLIST_LINK_NAME, (uint32_t) u16_link_pos, 0, 0, true);
}

//! Play the link inside the currently selected playlist
//!
//! @param  u16_link_pos  The position in the playlist of the link to select
//!
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_usb_ms_specific_audio_nav_playlist_play_link(uint16_t u16_link_pos)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_AUDIO_NAV_PLAYLIST_PLAY_LINK, (uint32_t) u16_link_pos, 0, 0, false);
}

//! Returns the folder name of the current file selected in the navigation navigator
//!
//! @param  u8_size_max  The maximal size of the buffer
//!
//! @return cmd_out_status - false in case of error, true is success
//!
__always_inline static bool ai_async_usb_ms_specific_audio_nav_get_folder_name(uint8_t u8_size_max)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_AUDIO_NAV_GET_FOLDER_NAME, (uint32_t) u8_size_max, 0, 0, true);
}

//! Position the File system navigation on the playlist file
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_nav_getplaylistfile(void)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_NAV_GET_PLAYLIST_FILE, 0, 0, 0, false);
}

//! Save the context of the filesystem navigator
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_nav_save(void **context)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_NAV_SAVE, (uint32_t) context, 0, 0, false);
}

//! Restore the context of the filesystem navigator
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_nav_restore(void **context)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_NAV_RESTORE, (uint32_t) context, 0, 0, false);
}

//! Save the context of the audio navigator
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_audio_nav_save(void **context)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_AUDIO_NAV_SAVE, (uint32_t) context, 0, 0, false);
}

//! Restore the context of the audio navigator
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_audio_nav_restore(void **context)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_AUDIO_NAV_RESTORE, (uint32_t) context, 0, 0, false);
}

//! Save the context of the playlist navigator
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_playlist_nav_save(void **context)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_PLAYLIST_NAV_SAVE, (uint32_t) context, 0, 0, false);
}

//! Restore the context of the playlist navigator
//!
//! @return    false in case of error
//!
__always_inline static bool ai_async_usb_ms_specific_playlist_nav_restore(void **context)
{
  return ai_async_cmd_put((cmd_ai_id_t) CMD_AI_USB_MS_PLAYLIST_NAV_RESTORE, (uint32_t) context, 0, 0, false);
}


#endif  // _AI_USB_MS_H_
