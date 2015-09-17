/**************************************************************************
 *
 * \file
 *
 * \brief MP3 player.
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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "compiler.h"
#include "file.h"

#include "conf_audio_player.h"

#include "audio_mixer.h"
#include "mad.h"
#include "reader_id3.h"
#include "com_task.h"
#include "navigation.h"
#include "audio_interface.h"

//! Maximal number of input bytes requested by the decoder at once.
#define MP3_MAX_DATA_REQUESTED  (2881 + MAD_BUFFER_GUARD)

//! Size of each PCM buffer in samples per channel.
#define PCM_BUF_SAMP_SIZE       1152

//! Size of each PCM buffer in bytes.
#define PCM_BUF_BYTE_SIZE       (PCM_BUF_SAMP_SIZE * 2 * sizeof(int16_t))


//! This is a private message structure.
//! A generic pointer to this structure is passed to each of the callback
//! functions.
typedef struct
{
  bool unrecoverable_error;     //!< Whether an unrecoverable error occurred.
  const char *filename;         //!< Name of the audio file to play.
  uint32_t mp3_start_file_pos;  //!< Position of start of MP3 data in audio file.
  uint32_t mp3_pos;             //!< Position in MP3 data.
  uint32_t mp3_end_file_pos;    //!< Position of end of MP3 data in audio file.
  uint8_t *frame_buf;           //!< Pointer to the frame buffer.
  uint8_t *frame_buf_guard;     //!< Pointer to the frame buffer guard.
  const uint8_t *frame_buf_end; //!< Pointer to the frame buffer end.
  uint32_t frame_buf_data_size; //!< Size of data stored in the frame buffer.
  mad_timer_t duration_played;  //!< Duration played.
  bool verbose;                 //!< Whether to be verbose.
} mp3_play_data;

// From AudioInterface
extern Ai_player_flag_t g_player_flag;
extern uint32_t  g_player_duration;
extern uint32_t  g_player_currentTime_ms;
extern uint32_t  g_player_target_file_pos;
extern uint8_t   g_smooth_com_cnt;

/*! \brief This is the input callback.
 *
 * The purpose of this callback is to (re)fill the stream buffer which is to be
 * decoded.
 */
static enum mad_flow input(void *data,
                           struct mad_stream *stream)
{
  mp3_play_data *mp3_play_data = data;
  uint32_t remaining = 0;
  uint8_t *read_start = mp3_play_data->frame_buf;
  uint32_t read_size_req = MP3_MAX_DATA_REQUESTED;
  uint32_t read_size = 0;
  uint32_t file_pos;
  bool eof;

  if (mp3_play_data->frame_buf_guard != NULL)
  {
    // End of MP3 file.
//    fputs("Full read and decode done.\r\n", stdout);
    return MAD_FLOW_STOP;
  }

  if( g_player_flag.status == PLAYER_FLAG_STOP )
  {
    // Playback stopped
    return MAD_FLOW_STOP;
  }

  if (stream->next_frame != NULL)
  {
    remaining = mp3_play_data->frame_buf_end - stream->next_frame;
    memmove(mp3_play_data->frame_buf, stream->next_frame, remaining);
    read_start += remaining;
    read_size_req -= remaining;
  }

  nav_checkdisk_disable();
  read_size = file_read_buf(read_start, read_size_req);
  file_pos = file_getpos();
  eof = file_eof();
  nav_checkdisk_enable();

  if (read_size < read_size_req && !eof)
  {
    mp3_play_data->unrecoverable_error = true;
    /*fprintf(stderr, "** Error while reading the MP3 file at byte offset %lu.\r\n",
                    mp3_play_data->mp3_start_file_pos + mp3_play_data->mp3_pos);*/
    return MAD_FLOW_BREAK;
  }

  if (file_pos > mp3_play_data->mp3_end_file_pos)
    read_size -= file_pos - 1 - mp3_play_data->mp3_end_file_pos;

  if(remaining)
  {
    mp3_play_data->mp3_pos += (MP3_MAX_DATA_REQUESTED -remaining); // Replaced to be the file position of the current frame
  }
  mp3_play_data->frame_buf_data_size = remaining + read_size;

  if (eof || file_pos > mp3_play_data->mp3_end_file_pos)
  {
    mp3_play_data->frame_buf_guard = read_start + read_size;
    memset(mp3_play_data->frame_buf_guard, 0x00, MAD_BUFFER_GUARD);
    read_size += MAD_BUFFER_GUARD;
  }

  // Instruct the decoder to decode data.
  mad_stream_buffer(stream, mp3_play_data->frame_buf, remaining + read_size);
  return MAD_FLOW_CONTINUE;
}


/*! \brief This is the output callback function.
 *
 * It is called after each frame of MPEG audio data has been completely decoded.
 * The purpose of this callback is to output (or play) the decoded PCM audio.
 */
static enum mad_flow output(void *data,
                            const struct mad_header *header,
                            struct mad_pcm *pcm)
{
  mp3_play_data *mp3_play_data = data;
  int16_t *pcm_buf = NULL;
  int i;
  enum
  {
    STATE_MALLOC,
    STATE_POST_PROCESSING,
    STATE_OUTPUT,
    STATE_END
  } state = STATE_MALLOC;
  bool exit = false;

  if (!mad_timer_compare(mp3_play_data->duration_played, mad_timer_zero))
  {
/*
    static const char *MP3_LAYER[] =
    {
      "I",
      "II",
      "III"
    };

    static const char *MP3_MODE[] =
    {
      "Single channel",
      "Dual channel",
      "Joint stereo",
      "Stereo"
    };

    static const char *MP3_EMPHASIS[] =
    {
      "None",
      "50/15 microseconds",
      "Unknown",
      "CCITT J.17"
    };

    // Display content description.
    printf("++      MP3 file: %s\r\n", mp3_play_data->filename);
    printf("++   Audio layer: %s\r\n", MP3_LAYER[header->layer - 1]);
    printf("++ Sampling rate: %u Hz\r\n", header->samplerate);
    printf("++  Channel mode: %s\r\n", MP3_MODE[header->mode]);
    printf("++      Bit-rate: %lu bps\r\n", header->bitrate);
    printf("++      Emphasis: %s\r\n", MP3_EMPHASIS[header->emphasis]);
*/
    // Start the renderer.
//    fputs("Playing MP3 file... Press any key to stop.\r\n", stdout);
    audio_mixer_dacs_setup(pcm->samplerate,
                           2 /*pcm->channels*/,
                           16,
                           false);
  }

  while (!exit)
  {
    // Select the audio navigator
    nav_select(FS_NAV_ID_AUDIO);

    // Compute the command
    if (g_smooth_com_cnt)
      g_smooth_com_cnt--;
    else if (fat_check_mount_select_open())
    {
      com_task();
      nav_select(FS_NAV_ID_AUDIO);
    }

    // Handle the stop state
    if (g_player_flag.status == PLAYER_FLAG_STOP)
    {
      audio_mixer_dacs_flush(true);
      return MAD_FLOW_STOP;
    }

    switch (state)
    {

    case STATE_MALLOC:
      if (g_player_flag.status == PLAYER_FLAG_PAUSE)
        state = STATE_END;
      else if ((pcm_buf = audio_mixer_get_buffer(pcm->length)))
        state = STATE_POST_PROCESSING;
      break;

    case STATE_POST_PROCESSING:
      // Get reconstructed data.
      if (pcm->channels == 1)
      {
        for (i = 0; i < pcm->length; i++)
        {
          pcm_buf[i * 2]     = pcm->samples[0][i];
          pcm_buf[i * 2 + 1] = pcm->samples[0][i];
        }
      }
      else
      {
        for (i = 0; i < pcm->length; i++)
        {
          pcm_buf[i * 2]     = pcm->samples[0][i];
          pcm_buf[i * 2 + 1] = pcm->samples[1][i];
        }
      }
      state = STATE_OUTPUT;
      break;

    case STATE_OUTPUT:
      if (audio_mixer_dacs_output(pcm_buf, pcm->length))
        state = STATE_END;
      break;

    case STATE_END:
    default:
      exit = true;
    }
  }

  mad_timer_add(&mp3_play_data->duration_played, header->duration);

  nav_select(FS_NAV_ID_AUDIO);

  // Mute the audio
  if (g_player_flag.status == PLAYER_FLAG_PAUSE)
    audio_mixer_mute_audio(true);

  do
  {
    com_task();
    nav_select(FS_NAV_ID_AUDIO);
  } while (g_player_flag.status == PLAYER_FLAG_PAUSE &&
           g_player_flag.status_fast == PLAYER_FLAG_NORMAL &&
           fat_check_mount_select_open());

  // Unmute the audio if needed
  if (g_player_flag.status == PLAYER_FLAG_PLAY)
    audio_mixer_mute_audio(false);

  if (g_player_flag.status_fast != PLAYER_FLAG_NORMAL)
    audio_mixer_dacs_flush(true);

  return MAD_FLOW_CONTINUE;
}


/*! \brief This is the error callback function.
 *
 * It is called whenever a decoding error occurs. The error is indicated by
 * stream->error; the list of possible MAD_ERROR_* errors can be found in the
 * mad.h (or stream.h) header file.
 */
static enum mad_flow error(void *data,
                           struct mad_stream *stream,
                           struct mad_frame *frame)
{
  mp3_play_data *mp3_play_data = data;

  if ((stream->this_frame != mp3_play_data->frame_buf_guard ||
       stream->error != MAD_ERROR_LOSTSYNC) &&
      mp3_play_data->verbose)
  {
    /*fprintf(stderr, "** Decoding error 0x%.4X (%s) at byte offset %lu.\r\n",
                    stream->error,
                    mad_stream_errorstr(stream),
                    mp3_play_data->mp3_start_file_pos + mp3_play_data->mp3_pos - mp3_play_data->frame_buf_data_size +
                    stream->ptr.byte - mp3_play_data->frame_buf);*/
  }

  return MAD_FLOW_CONTINUE;
}


/*! \brief This is the header callback function.
 *
 * It is called whenever a asf header is found
 */

uint32_t time_goal;
#define NBSAVFILEPOS 200  // Number of timestamps available
#define OFFSETFILEPOS 50  //
#define FILEPOSDELAY 4000 // Distance in ms between 2 timestamps
uint32_t *tSavFilePos; // tab to sav File Position at each timestamp
uint16_t FilePosPtr; // next timestamp index
uint16_t cptms; // millisecond counter used between 2 timestamps
uint32_t TimeStampOffset;

static enum mad_flow header(void *data,
                            struct mad_header const *header)
{
  mp3_play_data *mp3_play_data = data;

  // AudioInterface ctrl_time variable is set
  g_player_currentTime_ms = ((FilePosPtr-1) * FILEPOSDELAY) + cptms + TimeStampOffset;

  // Save File Pos
  if(cptms > FILEPOSDELAY)
  {
    tSavFilePos[FilePosPtr++] = mp3_play_data->mp3_pos;
    if(FilePosPtr == NBSAVFILEPOS)
    {
      int i;
      for(i=0;i<(NBSAVFILEPOS-OFFSETFILEPOS); i++)
      {
         tSavFilePos[i]= tSavFilePos[i+OFFSETFILEPOS];
      }
      TimeStampOffset += OFFSETFILEPOS * FILEPOSDELAY;
      FilePosPtr -= OFFSETFILEPOS;
    }
    cptms = cptms - FILEPOSDELAY + ((header->duration.fraction ) / (MAD_TIMER_RESOLUTION / 1000));
  }
  else
  {
    cptms += header->duration.fraction / (MAD_TIMER_RESOLUTION / 1000); // 26 ms per frame
  }

  if (g_player_target_file_pos && g_player_flag.status_fast != PLAYER_FLAG_NORMAL)
  {
    file_seek(g_player_target_file_pos, FS_SEEK_SET);
    mp3_play_data->mp3_pos = g_player_target_file_pos - mp3_play_data->mp3_start_file_pos;
    g_player_target_file_pos = 0;
    g_player_currentTime_ms = g_player_duration * 1000;
    time_goal = 0;
    tSavFilePos[0] = 0;
    FilePosPtr = 1;
    TimeStampOffset = g_player_currentTime_ms;
    cptms = 0;
    g_player_flag.status_fast = PLAYER_FLAG_NORMAL;
    return MAD_FLOW_IGNORE;
  }

  // Fast Forward Test
  if(g_player_flag.status_fast == PLAYER_FLAG_FFW)
  {
    if(!time_goal)
    {
      time_goal = g_player_currentTime_ms + (g_player_duration * 1000);
    }
    if(g_player_currentTime_ms < time_goal)
    {
      // Skip this frame
      return MAD_FLOW_IGNORE;
    }
    else
    {
      // Goal Time is reached, Clear FFW and decode frame
      time_goal = 0;
      g_player_flag.status_fast = PLAYER_FLAG_NORMAL;
      return MAD_FLOW_IGNORE;
    }
  }
  // Fast Rewind Test
  if(g_player_flag.status_fast == PLAYER_FLAG_FRW)
  {
      if(g_player_currentTime_ms < (g_player_duration * 1000))
      {
        time_goal = 1; // in order to avoid time_goal=0 in ffw
      }
      else
      {
        time_goal = g_player_currentTime_ms - (g_player_duration * 1000);
      }

      if(  time_goal > TimeStampOffset)
      {
        FilePosPtr = (time_goal- TimeStampOffset) / FILEPOSDELAY;
      }
      else
      {
        FilePosPtr = 0;
        TimeStampOffset = 0;
      }
      cptms = 0;
      mp3_play_data->mp3_pos = tSavFilePos[FilePosPtr];
      file_seek(mp3_play_data->mp3_start_file_pos + mp3_play_data->mp3_pos, FS_SEEK_SET);
      g_player_currentTime_ms = FilePosPtr * FILEPOSDELAY + TimeStampOffset;
      g_player_duration = time_goal - (FilePosPtr * FILEPOSDELAY) + TimeStampOffset;
      FilePosPtr++;
      g_player_flag.status_fast = PLAYER_FLAG_FFW;  // After the fast rewind to timestamp, ffw to time_goal
      return MAD_FLOW_IGNORE;

  }
  return MAD_FLOW_CONTINUE;
}


bool play_mp3(const char *filename, bool verbose);

bool play_mp3(const char *filename, bool verbose)
{
  bool retval;

  // Initialize the private message structure.
  mp3_play_data mp3_play_data =
  {
    .unrecoverable_error  = false,
    .filename             = filename,
    .mp3_start_file_pos   = 0,
    .mp3_pos              = 0,
    .mp3_end_file_pos     = 0,
    .frame_buf            = NULL,
    .frame_buf_guard      = NULL,
    .frame_buf_end        = NULL,
    .frame_buf_data_size  = 0,
    .duration_played      = mad_timer_zero,
    .verbose              = verbose
  };

  struct mad_decoder decoder;

  nav_select(FS_NAV_ID_AUDIO);

  St_id3size id3size = reader_id3_size();
  mp3_play_data.mp3_start_file_pos = id3size.prepended_tag;
  mp3_play_data.mp3_end_file_pos = nav_file_lgt() - 1 - id3size.appended_tag;

  // Open the MP3 file.
  if (!file_open(FOPEN_MODE_R))
  {
//    fputs("** Cannot open the MP3 file.\r\n", stderr);
    goto lerror;
  }

  // Seek MP3 data.
  if (!file_seek(mp3_play_data.mp3_start_file_pos, FS_SEEK_SET))
  {
//    fputs("** Cannot seek MP3 data.\r\n", stderr);
    goto lerror;
  }

  // Allocate the frame buffer.
  if ((mp3_play_data.frame_buf = malloc(MP3_MAX_DATA_REQUESTED)) == NULL)
  {
//    fputs("** Cannot allocate the frame buffer.\r\n", stderr);
    goto lerror;
  }
  mp3_play_data.frame_buf_end = mp3_play_data.frame_buf + MP3_MAX_DATA_REQUESTED;

  // Allocate the PCM buffers.
  if (!audio_mixer_create_buffers(2, PCM_BUF_BYTE_SIZE))
    goto lerror;

  // Allocate Position File Registers
  if ((tSavFilePos = malloc(NBSAVFILEPOS*4)) == NULL)
  {
    goto lerror;
  }
  tSavFilePos[0] = 0;
  FilePosPtr = 1;
  cptms = 0;
  time_goal = 0;
  TimeStampOffset = 0;

  // Initialize the decoder.
  mad_decoder_init(&decoder, &mp3_play_data,
                             input,
                             header,  // header
                             NULL,  // filter
                             output,
                             error,
                             NULL); // message

  // Start the decoder.
  if (mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC) && !mp3_play_data.unrecoverable_error)
  {
    /*fprintf(stderr, "** Memory allocation error at byte offset %lu.\r\n",
                    mp3_play_data->mp3_start_file_pos + mp3_play_data.mp3_pos - mp3_play_data.frame_buf_data_size);*/
  }

  // Wait for end of rendering.
  audio_mixer_dacs_flush(false);

  retval = true;
  goto lexit;

lerror:
  retval = false;

lexit:

  // Close the decoder and free all allocated memories.
  mad_decoder_finish(&decoder);
  audio_mixer_close();
  mp3_play_data.frame_buf_end = NULL;
  mp3_play_data.frame_buf_guard = NULL;
  free(mp3_play_data.frame_buf), mp3_play_data.frame_buf = NULL;
  free(tSavFilePos);

  // Close the MP3 file.
  file_close();
/*
  printf("Playback lasted %ld ms.\r\n\r\n",
         mad_timer_count(mp3_play_data.duration_played, MAD_UNITS_MILLISECONDS));
*/

  return retval;
}
