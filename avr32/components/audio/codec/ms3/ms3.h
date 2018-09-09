/**************************************************************************
 *
 * \file
 *
 * \brief Bang & Olufsen MS3 header file.
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


#include "compiler.h"

/**
 * \defgroup group_avr32_components_audio_codec_ms3_common AUDIO - MS3 Audio Codec
 *
 * Audio Codec Software Driver for the MobileSound 3 (MS3) from Bang & Olufsen ICEPower
 * configured to be used with the external CS2200 clock synthesizer.
 *
 * \{
 */

// Configuration for the hardware module (TWI or TWIM) depending of the UC3 series
#if (UC3A3 || UC3C || UC3L)
#  define MS3_TWI                  (&AVR32_TWIM0)
#else
#  define MS3_TWI                  (&AVR32_TWI)
#endif

#define MS3_TWI_ADDRESS   (0xDC >> 1)
#define MS3_HP_VOL_L      0x0C
#define MS3_HP_VOL_R      0x0D
#define MS3_SP_VOL_L      0x0A
#define MS3_SP_VOL_R      0x0B
#define MS3_MUTE_VOL_CTRL 0x0E
#define MS3_LEFT_CHANNEL  1
#define MS3_RIGHT_CHANNEL 2
#define MS3_VOL_MIN       0x30
#define MS3_VOL_MAX       0xA8
#define MS3_VOL_STEP      3

extern void ms3_dac_start(U32 sample_rate_hz,
                             U8 num_channels,
                             U8 bits_per_sample,
                             bool swap_channels,
                             void (*callback)(U32 arg),
                             U32 callback_opt,
                             U32 pba_hz);


extern void ms3_dac_setup(U32 sample_rate_hz,
                             U8 num_channels,
                             U8 bits_per_sample,
                             bool swap_channels,
                             void (*callback)(U32 arg),
                             U32 callback_opt,
                             U32 pba_hz);

extern bool ms3_dac_output(void *sample_buffer, size_t sample_length);

extern void ms3_dac_increase_volume(void);

extern void ms3_dac_decrease_volume(void);

extern void ms3_dac_flush(void);

extern void ms3_dac_stop(void);

extern void ms3_dac_set_volume(U8 volume);

extern U8 ms3_dac_get_volume(void);

extern bool ms3_dac_is_volume_boosted(void);

extern void ms3_dac_mute(bool mute);

extern bool ms3_dac_is_volume_muted(void);

/**
 * \}
 */
