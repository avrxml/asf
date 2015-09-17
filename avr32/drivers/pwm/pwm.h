/*****************************************************************************
 *
 * \file
 *
 * \brief PWM driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PWM interface on AVR32
 * devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _PWM_H_
#define _PWM_H_

/**
 * \defgroup group_avr32_drivers_pwm PWM - Pulse Width Modulation
 *
 * Pulse Width Modulation (PWM) Software Driver for modules below revision v4.0.0.
 * This driver provides an API to get access to the main features of the PWM controller.
 *
 * \{
 */

#include <avr32/io.h>


//! Value returned by function when it completed successfully.
#define PWM_SUCCESS 0

//! Value returned by function when it was unable to complete successfully
//! for some unspecified reason.
#define PWM_FAILURE -1

//! Value returned by function when the input parameters are out of range.
#define PWM_INVALID_INPUT 1

//! Value returned by function when the channel number is invalid.
#define PWM_INVALID_ARGUMENT 1

//! Operate PWM channel in left aligned mode.
#define PWM_MODE_LEFT_ALIGNED 0

//! Operate PWM channel in center aligned mode.
#define PWM_MODE_CENTER_ALIGNED 1

//! PWM channel starts output low level.
#define PWM_POLARITY_LOW 0

//! PWM channel starts output high level.
#define PWM_POLARITY_HIGH 1

//! PWM channel write in CUPDx updates duty cycle at the next period start event.
#define PWM_UPDATE_DUTY 0

//! PWM channel write in CUPDx updates period at the next period start event.
#define PWM_UPDATE_PERIOD 1


//! Input parameters when initializing a PWM channel.
typedef struct
{
  //! CLKB divide factor.
  unsigned int divb;

  //! CLKA divide factor.
  unsigned int diva;

  //! Divider input clock B.
  unsigned int preb;

  //! Divider input clock A.
  unsigned int prea;
} pwm_opt_t;


/*! \brief This function initialize the PWM controller (mode register) and disable the interrupt.
 * \param opt PWM Channel structure parameter
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_init(const pwm_opt_t *opt);

/*! \brief Initialize a specific PWM channel.
 * \param channel_id The channel identifier mask
 * \param *pwm_channel Pointer to PWM channel struct avr32_pwm_channel_t
 * \return PWM_SUCCESS, PWM_INVALID_INPUT or PWM_INVALID_ARGUMENT
 */
extern int pwm_channel_init(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/*! \brief Start PWM channels.
 * \param channels_bitmask A bit-mask with set bits indicating channels to start.
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_start_channels(unsigned long channels_bitmask);

/*! \brief Stop PWM channels.
 * \param channels_bitmask A bit-mask with set bits indicating channels to stop.
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 */
extern int pwm_stop_channels(unsigned long channels_bitmask);

/*! \brief Update channel register CPRDx or CDTYx by forcing synchronization with the PWM period.
 * This function uses the CUPDx register as a double buffer for the period or the duty cycle.
 * Only the first 20 bits of cupd are significant.
 * \param channel_id The channel identifier (0 to max channel-1)
 * \param *pwm_channel Pointer to PWM channel struct avr32_pwm_channel_t
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 * \note This update function should be preferred when updating a PWM channel by polling.
 */
extern int pwm_sync_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/*! \brief Update channel register CPRDx or CDTYx without synchronizing with the PWM period.
 * This function uses the CUPDx register as a double buffer for the period or the duty cycle.
 * Only the first 20 bits of cupd are significant.
 * \param channel_id The channel identifier (0 to max channel-1)
 * \param *pwm_channel Pointer to PWM channel struct avr32_pwm_channel_t
 * \return PWM_SUCCESS or PWM_INVALID_INPUT
 * \warning Calling this function several times in a row may result in some update values never being
 * issued to PWM if some external synchronizing mechanism like an interrupt is not used.
 * \note This update function should be preferred when updating a PWM channel from an interrupt handler.
 */
extern int pwm_async_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel);

/**
 * \}
 */

#endif  // _PWM_H_
