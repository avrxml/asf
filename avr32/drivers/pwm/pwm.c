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


#include "compiler.h"
#include "pwm.h"


int pwm_init(const pwm_opt_t *opt)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;
  bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (opt == 0 ) // Null pointer.
    return PWM_INVALID_INPUT;

  // Disable interrupt.
  if (global_interrupt_enabled) Disable_global_interrupt();
  pwm->idr = ((1 << (AVR32_PWM_LINES_MSB + 1)) - 1) << AVR32_PWM_IDR_CHID0_OFFSET;
  pwm->isr;
  if (global_interrupt_enabled) Enable_global_interrupt();

  // Set PWM mode register.
  pwm->mr =
    ((opt->diva)<<AVR32_PWM_DIVA_OFFSET) |
    ((opt->divb)<<AVR32_PWM_DIVB_OFFSET) |
    ((opt->prea)<<AVR32_PWM_PREA_OFFSET) |
    ((opt->preb)<<AVR32_PWM_PREB_OFFSET)
    ;

  return PWM_SUCCESS;
}


int pwm_channel_init( unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (pwm_channel == 0) // Null pointer.
    return PWM_INVALID_ARGUMENT;
  if (channel_id > AVR32_PWM_LINES_MSB) // Control input values.
    return PWM_INVALID_INPUT;

  pwm->channel[channel_id].cmr= pwm_channel->cmr;   // Channel mode.
  pwm->channel[channel_id].cdty= pwm_channel->cdty; // Duty cycle, should be < CPRD.
  pwm->channel[channel_id].cprd= pwm_channel->cprd; // Channel period.

  return PWM_SUCCESS;
}


int pwm_start_channels(unsigned long channels_bitmask)
{
  if (channels_bitmask & ~((1 << (AVR32_PWM_LINES_MSB + 1)) - 1))
    return PWM_INVALID_INPUT;

  AVR32_PWM.ena = channels_bitmask; // Enable channels.

  return PWM_SUCCESS;
}


int pwm_stop_channels(unsigned long channels_bitmask)
{
  if (channels_bitmask & ~((1 << (AVR32_PWM_LINES_MSB + 1)) - 1))
    return PWM_INVALID_INPUT;

  AVR32_PWM.dis = channels_bitmask; // Disable channels.

  return PWM_SUCCESS;
}


int pwm_sync_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (channel_id > AVR32_PWM_LINES_MSB)
     return PWM_INVALID_INPUT;

  AVR32_PWM.isr;                                    // Acknowledgement and clear previous register state.
  pwm->channel[channel_id].cmr= pwm_channel->cmr;   // Channel mode register: update of the period or duty cycle.
  while (!(AVR32_PWM.isr & (1 << channel_id)));     // Wait until the last write has been taken into account.
  pwm->channel[channel_id].cupd= pwm_channel->cupd; // Channel update CPRDx or CDTYx according to CPD value in CMRx.

  return PWM_SUCCESS;
}


int pwm_async_update_channel(unsigned int channel_id, const avr32_pwm_channel_t *pwm_channel)
{
  volatile avr32_pwm_t *pwm = &AVR32_PWM;

  if (channel_id > AVR32_PWM_LINES_MSB)
     return PWM_INVALID_INPUT;

  pwm->channel[channel_id].cmr= pwm_channel->cmr;   // Channel mode register: update of the period or duty cycle.
  pwm->channel[channel_id].cupd= pwm_channel->cupd; // Channel update CPRDx or CDTYx according to CPD value in CMRx.

  return PWM_SUCCESS;
}
