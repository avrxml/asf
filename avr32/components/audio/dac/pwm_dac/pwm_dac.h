/*****************************************************************************
 *
 * \file
 *
 * \brief PWM mono audio DAC driver for AVR32.
 *
 * This driver constructs a 16-bit DAC output thanks to two 8-bit PWM channels
 * combined with a resistor ladder network.
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


#ifndef _PWM_DAC_H_
#define _PWM_DAC_H_

#include <stddef.h>

/**
 * \defgroup group_avr32_components_audio_dac_pwm_dac AUDIO - Mono Audio PWM DAC
 *
 * Mono Audio DAC implemented with a PWM hardware module.
 * This software module constructs a 16-bit DAC output using two 8-bit PWM channels combined
 * with a resistor ladder network. The interface provides functions to configure and control this software DAC.
 *
 * \{
 */

#include "compiler.h"
#include "conf_pwm_dac.h"


/*! \brief Starts the DAC.
 */
extern void pwm_dac_start(uint32_t sample_rate_hz,
                          uint8_t num_channels,
                          uint8_t bits_per_sample,
                          bool swap_channels,
                          void (*callback)(uint32_t arg),
                          uint32_t callback_opt,
                          uint32_t pba_hz);

/*! \brief Sets the DAC up with new settings.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_setup(uint32_t sample_rate_hz,
                          uint8_t num_channels,
                          uint8_t bits_per_sample,
                          bool swap_channels,
                          void (*callback)(uint32_t arg),
                          uint32_t callback_opt,
                          uint32_t pba_hz);

/*! \brief Outputs a sample buffer to the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern bool pwm_dac_output(void *sample_buffer, size_t sample_length);

/*! \brief Return the current volume.
 *
 * \note The DAC must have been started beforehand.
 */
extern uint8_t pwm_dac_get_volume(void);

/*! \brief Set the current volume.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_set_volume(uint8_t volume);

/*! \brief Returns the state of the mute mode of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern bool pwm_dac_is_volume_muted(void);

/*! \brief Returns the state of the boost mode of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern bool pwm_dac_is_volume_boosted(void);

/*! \brief Increases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_increase_volume(void);

/*! \brief Decreases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_decrease_volume(void);

/*! \brief Flushes the sample buffer being output to the DAC.
 */
extern void pwm_dac_flush(void);

/*! \brief Stops the DAC.
 */
extern void pwm_dac_stop(void);

/*! \brief Mute audio if mute set to true
 */
extern void pwm_dac_mute(bool mute);

/**
 * \}
 */

#endif  // _PWM_DAC_H_
