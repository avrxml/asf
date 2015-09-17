/*****************************************************************************
 *
 * \file
 *
 * \brief TPA6130 amplifier driver header file.
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


#ifndef _TPA6130_H_
#define _TPA6130_H_

/**
 * \defgroup group_avr32_components_audio_amp_tpa6130a2 AUDIO - TPA6130A2 Audio Amplifier
 *
 * Software Driver for the TPA6130A2 Audio Amplifier. The TPA6130A2 is connected to the AVR UC3
 * through the ABDAC hardware module for the data stream and through the TWI hardware module for the control stream.
 *
 * \{
 */

#include <stddef.h>
#include "compiler.h"

/* Set TPA6130_MODE to one of the following selections
 * TPA6130_MODE_STEREO, left and right channels are connected to DAC
 * TPA6130_MODE_DUAL_MONO, left channel is connected but this signal is
 * used to feed both amplifiers for right and left channel output
 * TPA6130_MONO_L, only left channel is used with the DAC
 * TPA6130_MONO_R, only right channel is used with the DAC
 */
#define TPA6130_MODE_STEREO 0
#define TPA6130_MODE_MONO 1
/*
#define TPA6130_MODE_MONO_L 2
#define TPA6130_MODE_MONO_R 3
*/
/* Set TPA6130_SIG to:
 * TPA6130_SIG_DIFF, both, normal and inverted, signals are used from the DAC
 * TPA6130_SIG_POS, uses only the positive signals from the DAC
 */

#define TPA6130_SIG_DIFF  0
#define TPA6130_SIG_POS   1


/*#define TPA6130_SIG_NEG   2*/

/* TWI slave address of the amplifier
 * This device has a fixed slave address.
 * -> No hardware option to change it
 */
#define TPA6130_TWI_ADDRESS 0x60


/*! \name Volume Control
 */
//! @{

#define TPA6130_VOL_MIN       ((int8_t)(  0)) // -59dB
#define TPA6130_VOL_MAX       ((int8_t)( 63)) // : +4dB

//! @}


/*! \brief Sets the DACs up with new settings.
 *
 * \note The DACs must have been started beforehand.
 */

extern void tpa6130_dac_setup(uint32_t sample_rate_hz,
                              uint8_t num_channels,
                              uint8_t bits_per_sample,
                              bool swap_channels,
                              void (*callback)(uint32_t arg),
                              uint32_t callback_opt,
                              uint32_t pba_hz);


extern void tpa6130_dac_start(uint32_t sample_rate_hz,
                              uint8_t num_channels,
                              uint8_t bits_per_sample,
                              bool swap_channels,
                              void (*callback)(uint32_t arg),
                              uint32_t callback_opt,
                              uint32_t pba_hz);

/*! \brief Outputs a sample buffer to the DACs.
 *
 * \note The DACs must have been started beforehand.
 */
extern bool tpa6130_dac_output(void *sample_buffer, size_t sample_length);

/*! \brief Returns the current volume of the DAC.
 */
extern uint8_t tpa6130_dac_get_volume(void);

/*! \brief Set the volume of the DAC.
 */
extern void tpa6130_dac_set_volume(uint8_t volume);

/*! \brief Increases the output volume of the DACs.
 *
 * \note The DACs must have been started beforehand.
 */
extern void tpa6130_dac_increase_volume(void);

/*! \brief Decreases the output volume of the DACs.
 *
 * \note The DACs must have been started beforehand.
 */
extern void tpa6130_dac_decrease_volume(void);

/*! \brief Flushes the sample buffer being output to the DACs.
 */
extern void tpa6130_dac_flush(void);

/*! \brief Stops the DACs.
 */
extern void tpa6130_dac_stop(void);

/*! \brief Sets the volume of the amplifier.
 *  Valid values are between 0 (min -59dB) and 63 (max 4dB) although
 *  the function takes care of any values higher than that by setting
 *  it to max.
 */
extern void tpa6130_set_volume(int8_t volume);

/*! \brief Returns the state of the mute mode of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern bool tpa6130_dac_is_volume_muted(void);

/*! \brief Returns the state of the boost mode of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern bool tpa6130_dac_is_volume_boosted(void);

/*! \brief Gets the current volume settings.
 *  \returns Current volume settings. Value is between 0 (-59dB) and
 *  63 (4dB).
 */
extern int8_t tpa6130_get_volume(void);

/*! \brief Shuts down the amplifier and sets it into low power mode.
 *  This is the software low power mode described in the datasheet.
 */
extern void tpa6130_shutdown(void);

/*! \brief Powers up the amplifier from low power mode.
 *  This is the software low power mode described in the datasheet.
 */
extern void tpa6130_powerup(void);

/*! \brief No more audio if input is true.
 *
 */
extern void tpa6130_dac_mute(bool mute);

/*! \brief Probes and initializes the amplifier.
 *  Probes the TWI bus for the amplifier by using the slave address
 *  specified in the configuration (TPA6130_TWI_ADDRESS).
 *  If the device responds with an ACK the version register is read out
 *  and compared to the valid versions (TPA6130_VERSION).
 *  Last step is to set volume to 0, unmute and set the configuration
 *  specified in the conf_tpa6130.h file (stereo, mono ..).
 *
 *  \note This function is called by tpa6130_dac_setup()
 *
 *  \returns A positive value upon success and a negative value upon failure.
 */
extern int8_t tpa6130_init(void);

/**
 * \}
 */

#endif  // _TPA6130_H_

