/*****************************************************************************
 *
 * \file
 *
 * \brief Audio mixer configuration file.
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


#ifndef _CONF_AUDIO_MIXER_H_
#define _CONF_AUDIO_MIXER_H_

#include "board.h"

#include "conf_audio_player.h"

#if BOARD == EVK1105
#  include "pwm_dac.h"
#  include "tlv320aic23b.h"
#endif

#include "tpa6130.h"


/*! \name DACs Bit-Masks
 */
//! @{
#if BOARD == EVK1105
#  define AUDIO_MIXER_DAC_PWM_DAC   1
#  define AUDIO_MIXER_DAC_AIC23B    2
#  define AUDIO_MIXER_DAC_ABDAC     4

#elif BOARD == EVK1104
#  define AUDIO_MIXER_DAC_ABDAC     1

#else
#  error Board not defined - You should define AUDIO_MIXER_x configuration.
#endif
//! @}

/*! \name DACs Interfaces
 */
//! @{
#if BOARD == EVK1105
#define AUDIO_MIXER_DACS_INTERFACES                                            \
{                                                                              \
  {                                                                            \
    pwm_dac_start,                                                             \
    pwm_dac_setup,                                                             \
    pwm_dac_output,                                                            \
    NULL,                                                                      \
    pwm_dac_increase_volume,                                                   \
    pwm_dac_decrease_volume,                                                   \
    pwm_dac_get_volume,                                                        \
    pwm_dac_set_volume,                                                        \
    pwm_dac_is_volume_muted,                                                   \
    pwm_dac_is_volume_boosted,                                                 \
    pwm_dac_flush,                                                             \
    pwm_dac_stop,                                                              \
    pwm_dac_mute                                                               \
  },                                                                           \
  {                                                                            \
    aic23b_codec_start,                                                        \
    aic23b_codec_setup,                                                        \
    aic23b_dac_output,                                                         \
    aic23b_adc_input,                                                          \
    aic23b_dac_increase_volume,                                                \
    aic23b_dac_decrease_volume,                                                \
    aic23b_dac_get_average_headphone_volume,                                   \
    aic23b_dac_set_average_headphone_volume,                                   \
    aic23b_dac_is_headphone_volume_muted,                                      \
    aic23b_dac_is_headphone_volume_boosted,                                    \
    aic23b_codec_flush,                                                        \
    aic23b_codec_stop,                                                         \
    aic23b_dac_mute                                                            \
  },                                                                           \
  {                                                                            \
    tpa6130_dac_start,                                                         \
    tpa6130_dac_setup,                                                         \
    tpa6130_dac_output,                                                        \
    NULL,                                                                      \
    tpa6130_dac_increase_volume,                                               \
    tpa6130_dac_decrease_volume,                                               \
    tpa6130_dac_get_volume,                                                    \
    tpa6130_dac_set_volume,                                                    \
    tpa6130_dac_is_volume_muted,                                               \
    tpa6130_dac_is_volume_boosted,                                             \
    tpa6130_dac_flush,                                                         \
    tpa6130_dac_stop,                                                          \
    tpa6130_dac_mute                                                           \
  }                                                                            \
}
#elif BOARD == EVK1104
#define AUDIO_MIXER_DACS_INTERFACES                                            \
{                                                                              \
  {                                                                            \
    tpa6130_dac_start,                                                         \
    tpa6130_dac_setup,                                                         \
    tpa6130_dac_output,                                                        \
    NULL,                                                                      \
    tpa6130_dac_increase_volume,                                               \
    tpa6130_dac_decrease_volume,                                               \
    tpa6130_dac_get_volume,                                                    \
    tpa6130_dac_set_volume,                                                    \
    tpa6130_dac_is_volume_muted,                                               \
    tpa6130_dac_is_volume_boosted,                                             \
    tpa6130_dac_flush,                                                         \
    tpa6130_dac_stop,                                                          \
    tpa6130_dac_mute                                                           \
  }                                                                            \
}
#endif

#if !defined(AUDIO_MIXER_DACS_INTERFACES)
#  error The AUDIO_MIXER_DACS_INTERFACES configuration to use is missing.
#endif

//! @}

/*! \name LEDs
 */
//! @{
#define AUDIO_MIXER_OVERRUN_LED   LED2
#define AUDIO_MIXER_UNDERRUN_LED  LED3
//! @}

#include "usb_stream_player.h"
#define AUDIO_MIXER_OUT_OF_SAMPLES_CALLBACK                                     \
{                                                                               \
  dac_underflow_cb();                                                           \
}

#define AUDIO_MIXER_RELOAD_CALLBACK                                             \
{                                                                               \
  dac_sample_sent_cb();                                                         \
}

#if (defined BOARD) && (defined DEFAULT_DACS) && (BOARD==EVK1105) && (DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B)
#define AUDIO_MIXER_ADC_RELOAD_CALLBACK                                         \
{                                                                               \
  adc_sample_sent_cb();                                                         \
}

#define AUDIO_MIXER_ADC_OUT_OF_SAMPLE                                           \
{                                                                               \
  adc_underflow_cb();                                                           \
}
#endif

// #define USE_AUDIO_PLAYER_BUFFERIZATION    true

#if(DEFAULT_DACS==AUDIO_MIXER_DAC_ABDAC)
  #define SPEAKER_VOL_MIN 0
  #define SPEAKER_VOL_MAX 63
  #define SPEAKER_VOL_RES 1
#elif(DEFAULT_DACS==AUDIO_MIXER_DAC_PWM_DAC)
  #define SPEAKER_VOL_MIN 0
  #define SPEAKER_VOL_MAX 7
  #define SPEAKER_VOL_RES 1
#elif(DEFAULT_DACS==AUDIO_MIXER_DAC_AIC23B)
  #define SPEAKER_VOL_MIN AIC23B_HP_VOL_MIN
  #define SPEAKER_VOL_MAX AIC23B_HP_VOL_MAX
  #define SPEAKER_VOL_RES 1
#endif

#endif  // _CONF_AUDIO_MIXER_H_
