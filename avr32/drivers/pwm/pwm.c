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
