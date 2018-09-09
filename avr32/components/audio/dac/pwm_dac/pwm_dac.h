/*****************************************************************************
 *
 * \file
 *
 * \brief PWM mono audio DAC driver for AVR32.
 *
 * This driver constructs a 16-bit DAC output thanks to two 8-bit PWM channels
 * combined with a resistor ladder network.
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
