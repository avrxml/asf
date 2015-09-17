/*****************************************************************************
 *
 * \file
 *
 * \brief Audio mixer.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <stddef.h>

#include "conf_audio_mixer.h"
#include "conf_audio_player.h"
#include "compiler.h"
#include "audio.h"
#include "audio_mixer.h"

#include "board.h"


//! Number of DACs.
#define AUDIO_MIXER_CODEC_COUNT   (sizeof(AUDIO_MIXER_DACS) / sizeof(AUDIO_MIXER_DACS[0]))


//! DACs interfaces.
static const struct
{
  void (*start)(uint32_t sample_rate_hz,
                uint8_t num_channels,
                uint8_t bits_per_sample,
                bool swap_channels,
                void (*callback)(uint32_t arg),
                uint32_t callback_opt,
                uint32_t pba_hz);
  void (*setup)(uint32_t sample_rate_hz,
                uint8_t num_channels,
                uint8_t bits_per_sample,
                bool swap_channels,
                void (*callback)(uint32_t arg),
                uint32_t callback_opt,
                uint32_t pba_hz);
  bool (*output)(void *sample_buffer, size_t sample_length);
  bool (*input) (void *sample_buffer, size_t sample_length);
  void (*increase_volume)(void);
  void (*decrease_volume)(void);
  uint8_t   (*get_volume)(void);
  void (*set_volume)(uint8_t volume);
  bool (*is_volume_muted)(void);
  bool (*is_volume_boosted)(void);
  void (*flush)(void);
  void (*stop)(void);
  void (*mute_audio)(bool mute);
} AUDIO_MIXER_DACS[] = AUDIO_MIXER_DACS_INTERFACES;

//! Enabled DACs mask.
static uint32_t audio_mixer_enabled_dacs_mask = 0x00000000;

//! DACs settings.
static struct
{
  uint32_t sample_rate_hz;
  uint8_t num_channels;
  uint8_t bits_per_sample;
  bool swap_channels;
  uint8_t nb_buffers;
  uint8_t current_buffer;
  void **buffers;
} audio_mixer_dacs_settings =
{
  .sample_rate_hz   = 0,
  .num_channels     = 0,
  .bits_per_sample  = 0,
  .swap_channels    = false,
  .nb_buffers       = 0,
  .current_buffer   = 0,
  .buffers          = NULL
};

/*! \brief Callback function the DACs drivers should invoke when out of samples.
 */
static void audio_mixer_callback(uint32_t arg)
{
  if( arg == AUDIO_DAC_OUT_OF_SAMPLE_CB )
  {
#if defined(AUDIO_MIXER_OUT_OF_SAMPLES_CALLBACK)
    AUDIO_MIXER_OUT_OF_SAMPLES_CALLBACK;
#endif
  }

  else if( arg == AUDIO_DAC_RELOAD_CB )
  {
#if defined(USE_AUDIO_PLAYER_BUFFERIZATION) && USE_AUDIO_PLAYER_BUFFERIZATION == true
    buff_player_dacs_end_of_pcm_buf();
#endif
#if defined(AUDIO_MIXER_RELOAD_CALLBACK)
    AUDIO_MIXER_RELOAD_CALLBACK;
#endif
  }

  else if( arg == AUDIO_ADC_OUT_OF_SAMPLE_CB )
  {
#if defined(AUDIO_MIXER_ADC_OUT_OF_SAMPLE)
    AUDIO_MIXER_ADC_OUT_OF_SAMPLE;
#endif
  }

  else if( arg == AUDIO_ADC_RELOAD_CB )
  {
#if defined(AUDIO_MIXER_ADC_RELOAD_CALLBACK)
    AUDIO_MIXER_ADC_RELOAD_CALLBACK;
#endif
  }
}

bool audio_mixer_create_buffers_direct(uint8_t nb_buffers,
                                       size_t buffer_size)
{
  int i, j;
  void **buffers;

  if (audio_mixer_dacs_settings.buffers)
    return false;

  if (!(buffers = (void **) malloc(nb_buffers*sizeof(void *))))
    return false;

  for (i = 0; i < nb_buffers; i++)
  {
    if (!(buffers[i] = malloc(buffer_size)))
    {
      for (j = 0; j < i; j++)
        free(buffers[j]);
      free(buffers);
      return false;
    }
  }

  audio_mixer_dacs_settings.nb_buffers = nb_buffers;
  audio_mixer_dacs_settings.buffers = buffers;

  return true;
}

void audio_mixer_close_direct(void)
{
  int i;

  if (audio_mixer_dacs_settings.buffers)
  {
    for (i = 0; i < audio_mixer_dacs_settings.nb_buffers; i++)
      free(audio_mixer_dacs_settings.buffers[i]);
    free(audio_mixer_dacs_settings.buffers);
    audio_mixer_dacs_settings.buffers = NULL;
  }
}

void *audio_mixer_get_buffer_direct(size_t size)
{
  void *buffer;

  if (!audio_mixer_dacs_output(NULL, 0))
    return NULL;
  buffer = audio_mixer_dacs_settings.buffers[audio_mixer_dacs_settings.current_buffer];
  audio_mixer_dacs_settings.current_buffer = (audio_mixer_dacs_settings.current_buffer + 1)
                                             % audio_mixer_dacs_settings.nb_buffers;
  return buffer;
}


void audio_mixer_enable_dacs_direct(uint32_t dacs_mask)
{
  uint32_t sav_audio_mixer_enabled_dacs_mask;

  sav_audio_mixer_enabled_dacs_mask = audio_mixer_enabled_dacs_mask;
  audio_mixer_enabled_dacs_mask = (dacs_mask & ~audio_mixer_enabled_dacs_mask);

  audio_mixer_dacs_start_direct(audio_mixer_dacs_settings.sample_rate_hz,
                                audio_mixer_dacs_settings.num_channels,
                                audio_mixer_dacs_settings.bits_per_sample,
                                audio_mixer_dacs_settings.swap_channels);

  audio_mixer_enabled_dacs_mask = sav_audio_mixer_enabled_dacs_mask | dacs_mask;
}


void audio_mixer_dacs_start_direct(uint32_t sample_rate_hz,
                                   uint8_t num_channels,
                                   uint8_t bits_per_sample,
                                   bool swap_channels)
{
  audio_mixer_dacs_settings.sample_rate_hz  = sample_rate_hz;
  audio_mixer_dacs_settings.num_channels    = num_channels;
  audio_mixer_dacs_settings.bits_per_sample = bits_per_sample;
  audio_mixer_dacs_settings.swap_channels   = swap_channels;

  if (audio_mixer_dacs_settings.sample_rate_hz)
  {
    int i;

    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i))
        AUDIO_MIXER_DACS[i].start(audio_mixer_dacs_settings.sample_rate_hz,
                                  audio_mixer_dacs_settings.num_channels,
                                  audio_mixer_dacs_settings.bits_per_sample,
                                  audio_mixer_dacs_settings.swap_channels,
                                  audio_mixer_callback,
                                  0
                                  #if (defined AUDIO_MIXER_OUT_OF_SAMPLES_CALLBACK)
                                  | AUDIO_DAC_OUT_OF_SAMPLE_CB
                                  #endif
                                  #if (defined AUDIO_MIXER_RELOAD_CALLBACK || defined USE_AUDIO_PLAYER_BUFFERIZATION)
                                  | AUDIO_DAC_RELOAD_CB
                                  #endif
                                  #if (defined AUDIO_MIXER_ADC_OUT_OF_SAMPLE)
                                  | AUDIO_ADC_OUT_OF_SAMPLE_CB
                                  #endif
                                  #if (defined AUDIO_MIXER_ADC_RELOAD_CALLBACK)
                                  | AUDIO_ADC_RELOAD_CB
                                  #endif
                                  ,
                                  FPBA_HZ);
    }
  }
}


void audio_mixer_dacs_setup_direct(uint32_t sample_rate_hz,
                                   uint8_t num_channels,
                                   uint8_t bits_per_sample,
                                   bool swap_channels)
{
  audio_mixer_dacs_settings.sample_rate_hz  = sample_rate_hz;
  audio_mixer_dacs_settings.num_channels    = num_channels;
  audio_mixer_dacs_settings.bits_per_sample = bits_per_sample;
  audio_mixer_dacs_settings.swap_channels   = swap_channels;

  if (audio_mixer_dacs_settings.sample_rate_hz)
  {
    int i;

    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i))
        AUDIO_MIXER_DACS[i].setup(audio_mixer_dacs_settings.sample_rate_hz,
                                  audio_mixer_dacs_settings.num_channels,
                                  audio_mixer_dacs_settings.bits_per_sample,
                                  audio_mixer_dacs_settings.swap_channels,
                                  audio_mixer_callback,
                                  0
                                  #if (defined AUDIO_MIXER_OUT_OF_SAMPLES_CALLBACK)
                                  | AUDIO_DAC_OUT_OF_SAMPLE_CB
                                  #endif
                                  #if (defined AUDIO_MIXER_RELOAD_CALLBACK || defined USE_AUDIO_PLAYER_BUFFERIZATION)
                                  | AUDIO_DAC_RELOAD_CB
                                  #endif
                                  #if (defined AUDIO_MIXER_ADC_OUT_OF_SAMPLE)
                                  | AUDIO_ADC_OUT_OF_SAMPLE_CB
                                  #endif
                                  #if (defined AUDIO_MIXER_ADC_RELOAD_CALLBACK)
                                  | AUDIO_ADC_RELOAD_CB
                                  #endif
                                  ,
                                  FPBA_HZ);
    }
  }
}

bool audio_mixer_dacs_output_direct(void *sample_buffer, size_t sample_length)
{
  int i;

  if (!sample_length)
  {
    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i) &&
          !AUDIO_MIXER_DACS[i].output(sample_buffer, sample_length))
      {
        // LED_On(AUDIO_MIXER_OVERRUN_LED);

        return false;
      }
    }

    // LED_Off(AUDIO_MIXER_OVERRUN_LED);
  }
  else
  {
    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i))
      {
        while (!AUDIO_MIXER_DACS[i].output(sample_buffer, sample_length));
          // LED_On(AUDIO_MIXER_OVERRUN_LED);

        // LED_Off(AUDIO_MIXER_OVERRUN_LED);
      }
    }

    // LED_Off(AUDIO_MIXER_UNDERRUN_LED);
  }

  return true;
}


void audio_mixer_dacs_increase_volume_direct(void)
{
  int i;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
      AUDIO_MIXER_DACS[i].increase_volume();
  }
}


void audio_mixer_dacs_decrease_volume_direct(void)
{
  int i;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
      AUDIO_MIXER_DACS[i].decrease_volume();
  }
}

uint8_t audio_mixer_dacs_get_volume_direct(void)
{
  int i, count = 0;
  uint32_t volume = 0;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
    {
      volume += AUDIO_MIXER_DACS[i].get_volume();
      count++;
    }
  }

  return (uint8_t) (volume / count);
}

void audio_mixer_dacs_set_volume_direct(uint8_t volume)
{
  int i, count = 0;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
    {
      AUDIO_MIXER_DACS[i].set_volume(volume);
      count++;
    }
  }
}

bool audio_mixer_dacs_is_volume_muted_direct(void)
{
  int i, count = 0;
  bool is_muted = true;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
    {
      is_muted &= AUDIO_MIXER_DACS[i].is_volume_muted();
      count++;
    }
  }

  return is_muted;
}

void audio_mixer_dacs_flush_direct(bool force)
{
  int i;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
      AUDIO_MIXER_DACS[i].flush();
  }

  // LED_Off(AUDIO_MIXER_UNDERRUN_LED);
}


void audio_mixer_dacs_stop_direct(void)
{
  int i;

  for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
  {
    if (audio_mixer_enabled_dacs_mask & (1 << i))
      AUDIO_MIXER_DACS[i].stop();
  }

  audio_mixer_dacs_settings.sample_rate_hz  = 0;
  audio_mixer_dacs_settings.num_channels    = 0;
  audio_mixer_dacs_settings.bits_per_sample = 0;
  audio_mixer_dacs_settings.swap_channels   = false;
}


void audio_mixer_disable_dacs_direct(uint32_t dacs_mask)
{
  dacs_mask &= audio_mixer_enabled_dacs_mask;

  if (audio_mixer_dacs_settings.sample_rate_hz)
  {
    int i;

    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (dacs_mask & (1 << i))
        AUDIO_MIXER_DACS[i].stop();
    }
  }

  audio_mixer_enabled_dacs_mask &= ~dacs_mask;
}


void audio_mixer_mute_audio_direct(bool mute)
{
    int i;
    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i))
        AUDIO_MIXER_DACS[i].mute_audio(mute);
    }
}


bool audio_mixer_dacs_input_direct(void *sample_buffer, size_t sample_length)
{
  int i;

  if (!sample_length)
  {
    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i) &&
          !AUDIO_MIXER_DACS[i].input(sample_buffer, sample_length))
      {
        return false;
      }
    }
  }
  else
  {
    for (i = 0; i < AUDIO_MIXER_CODEC_COUNT; i++)
    {
      if (audio_mixer_enabled_dacs_mask & (1 << i))
      {
        while (!AUDIO_MIXER_DACS[i].input(sample_buffer, sample_length));
      }
    }
  }

  return true;
}
