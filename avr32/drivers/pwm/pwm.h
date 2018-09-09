/*****************************************************************************
 *
 * \file
 *
 * \brief PWM driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PWM interface on AVR32
 * devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
