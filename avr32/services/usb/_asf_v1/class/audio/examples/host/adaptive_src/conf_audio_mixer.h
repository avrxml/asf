/*****************************************************************************
 *
 * \file
 *
 * \brief Audio mixer configuration file.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

#define AUDIO_MIXER_OUT_OF_SAMPLES_CALLBACK                                    \
{                                                                              \
  LED_Toggle(AUDIO_MIXER_UNDERRUN_LED);                                        \
}

#include "usb_stream_player.h"
#define AUDIO_MIXER_RELOAD_CALLBACK                                            \
  {                                                                            \
    if (usb_stream_get_status() & USB_STREAM_STATUS_IDLE &&                    \
        !(usb_stream_get_status() & USB_STREAM_STATUS_DATA_PROCESSING))        \
      usb_stream_task();                                                       \
  }

#define AUDIO_MIXER_OVERRUN_CALLBACK                                           \
{                                                                              \
  LED_Toggle(AUDIO_MIXER_OVERRUN_LED);                                         \
}

#define USB_STREAM_OVERRUN_CALLBACK()   AUDIO_MIXER_OVERRUN_CALLBACK


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
