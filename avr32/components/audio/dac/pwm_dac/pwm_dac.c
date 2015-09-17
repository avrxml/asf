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


#include <stddef.h>

#include "compiler.h"
#include "pwm_dac.h"

#include <avr32/io.h>
#include "intc.h"
#include "gpio.h"
#include "audio.h"

#ifndef PWM_DAC_PWM_CHANNEL_LSB
#  define PWM_DAC_PWM_CHANNEL_LSB       6             //!< Least significant 8-bit PWM channel.
#  warning 'The PWM_DAC_PWM_CHANNEL_LSB should be customized for the application'
#endif
#ifndef PWM_DAC_PWM_CHANNEL_LSB_PIN
#  define PWM_DAC_PWM_CHANNEL_LSB_PIN   _0            //!< Least significant 8-bit PWM channel pin.
#  warning 'The PWM_DAC_PWM_CHANNEL_LSB_PIN should be customized for the application'
#endif
#ifndef PWM_DAC_PWM_CHANNEL_MSB
#  define PWM_DAC_PWM_CHANNEL_MSB       4             //!< Most significant 8-bit PWM channel.
#  warning 'The PWM_DAC_PWM_CHANNEL_MSB should be customized for the application'
#endif
#ifndef PWM_DAC_PWM_CHANNEL_MSB_PIN
#  define PWM_DAC_PWM_CHANNEL_MSB_PIN   _1            //!< Most significant 8-bit PWM channel pin.
#  warning 'The PWM_DAC_PWM_CHANNEL_MSB_PIN should be customized for the application'
#endif
//! Map of the PWM pins used by the PWM DAC.
static const gpio_map_t PWM_DAC_PWM_GPIO_MAP =
{
  {ATPASTE4(AVR32_PWM_, PWM_DAC_PWM_CHANNEL_LSB, PWM_DAC_PWM_CHANNEL_LSB_PIN, _PIN),
   ATPASTE4(AVR32_PWM_, PWM_DAC_PWM_CHANNEL_LSB, PWM_DAC_PWM_CHANNEL_LSB_PIN, _FUNCTION)},
  {ATPASTE4(AVR32_PWM_, PWM_DAC_PWM_CHANNEL_MSB, PWM_DAC_PWM_CHANNEL_MSB_PIN, _PIN),
   ATPASTE4(AVR32_PWM_, PWM_DAC_PWM_CHANNEL_MSB, PWM_DAC_PWM_CHANNEL_MSB_PIN, _FUNCTION)}
};

//! Output parameters.
static struct
{
  uint8_t num_channels;
  uint8_t bits_per_sample;
  bool swap_channels;
  void (*callback)(uint32_t arg);
  uint32_t callback_opt;
  int16_t (*get_sample)(void);
  volatile UnionPtr sample_buffer;
  volatile size_t sample_length;
  volatile UnionPtr reload_sample_buffer;
  volatile size_t reload_sample_length;
  uint8_t volume_shift;
  bool flushing;
} pwm_dac_output_params =
{
  .num_channels             = 0,
  .bits_per_sample          = 0,
  .swap_channels            = false,
  .callback                 = NULL,
  .callback_opt             = 0,
  .get_sample               = NULL,
  .sample_buffer            = {NULL},
  .sample_length            = 0,
  .reload_sample_buffer     = {NULL},
  .reload_sample_length     = 0,
  .volume_shift             = 0,
  .flushing                 = false
};


/*! \brief Returns the logarithm to base 2 of the aligned byte size required to
 *         store the \a bit_size bit size saturated to the range [|8, 32|].
 */
static __inline__ size_t pwm_dac_get_log2_aligned_byte_size(size_t bit_size)
{
  return 32 - clz((min(max(bit_size, 8), 32) << 1) - 1) - 1 - 3;
}


/*! \brief Generates a 16-bit sample from a sample stored in 8 bits.
 */
static int16_t pwm_dac_get_sample_8_bits(void)
{
  int16_t sample =
    *pwm_dac_output_params.sample_buffer.s8ptr << (16 - pwm_dac_output_params.bits_per_sample);

  pwm_dac_output_params.sample_buffer.s8ptr += pwm_dac_output_params.num_channels;

  return sample;
}


/*! \brief Generates a 16-bit sample from a sample stored in 16 bits.
 */
static int16_t pwm_dac_get_sample_16_bits(void)
{
  int16_t sample =
    *pwm_dac_output_params.sample_buffer.s16ptr << (16 - pwm_dac_output_params.bits_per_sample);

  pwm_dac_output_params.sample_buffer.s16ptr += pwm_dac_output_params.num_channels;

  return sample;
}


/*! \brief Generates a 16-bit sample from a sample stored in 32 bits.
 */
static int16_t pwm_dac_get_sample_32_bits(void)
{
  int16_t sample =
    *pwm_dac_output_params.sample_buffer.s32ptr >> (pwm_dac_output_params.bits_per_sample - 16);

  pwm_dac_output_params.sample_buffer.s32ptr += pwm_dac_output_params.num_channels;

  return sample;
}


/*! \brief Table of pointers to the functions generating 16-bit samples from
 *         stored samples.
 */
static int16_t (*const PWM_DAC_GET_SAMPLE[])(void) =
{
  pwm_dac_get_sample_8_bits,
  pwm_dac_get_sample_16_bits,
  pwm_dac_get_sample_32_bits
};


/*! \brief Outputs a sample to the DAC.
 */
static __inline__ void pwm_dac_output_sample(int16_t sample)
{
  AVR32_PWM.channel[PWM_DAC_PWM_CHANNEL_LSB].cupd = (uint8_t)sample;
  AVR32_PWM.channel[PWM_DAC_PWM_CHANNEL_MSB].cupd = (uint8_t)((sample + 0x8000) >> 8);
}


/*! \brief PWM interrupt handler managing the DAC.
 */
#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void pwm_dac_pwm_int_handler(void)
{
  AVR32_PWM.isr;

  if (!pwm_dac_output_params.sample_length)
  {
    if (!pwm_dac_output_params.reload_sample_length)
    {
      AVR32_PWM.idr = 1 << PWM_DAC_PWM_CHANNEL_LSB |
                      1 << PWM_DAC_PWM_CHANNEL_MSB;
      AVR32_PWM.isr;

      pwm_dac_output_params.sample_buffer.u8ptr = NULL;

      if (!pwm_dac_output_params.flushing)
      {
        if (pwm_dac_output_params.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
          pwm_dac_output_params.callback(AUDIO_DAC_OUT_OF_SAMPLE_CB);
        if (pwm_dac_output_params.callback_opt & AUDIO_DAC_RELOAD_CB)
          pwm_dac_output_params.callback(AUDIO_DAC_RELOAD_CB);
      }
      return;
    }

    pwm_dac_output_params.sample_buffer.u8ptr = pwm_dac_output_params.reload_sample_buffer.u8ptr;
    pwm_dac_output_params.sample_length       = pwm_dac_output_params.reload_sample_length;
    pwm_dac_output_params.reload_sample_buffer.u8ptr  = NULL;
    pwm_dac_output_params.reload_sample_length        = 0;
  }

  pwm_dac_output_sample(pwm_dac_output_params.get_sample() >> pwm_dac_output_params.volume_shift);
  pwm_dac_output_params.sample_length--;
}


void pwm_dac_start(uint32_t sample_rate_hz,
                   uint8_t num_channels,
                   uint8_t bits_per_sample,
                   bool swap_channels,
                   void (*callback)(uint32_t arg),
                   uint32_t callback_opt,
                   uint32_t pba_hz)
{
  pwm_dac_stop();

  gpio_enable_module(PWM_DAC_PWM_GPIO_MAP,
                     sizeof(PWM_DAC_PWM_GPIO_MAP) / sizeof(PWM_DAC_PWM_GPIO_MAP[0]));

  pwm_dac_setup(sample_rate_hz,
                num_channels,
                bits_per_sample,
                swap_channels,
                callback,
                callback_opt,
                pba_hz);

  AVR32_PWM.channel[PWM_DAC_PWM_CHANNEL_LSB].cmr = (AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET |
                                                    AVR32_PWM_CPOL_MASK) &
                                                   ~(AVR32_PWM_CALG_MASK |
                                                     AVR32_PWM_CPD_MASK);
  AVR32_PWM.channel[PWM_DAC_PWM_CHANNEL_LSB].cprd = 0x000000FF;

  AVR32_PWM.channel[PWM_DAC_PWM_CHANNEL_MSB].cmr = (AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET |
                                                    AVR32_PWM_CPOL_MASK) &
                                                   ~(AVR32_PWM_CALG_MASK |
                                                     AVR32_PWM_CPD_MASK);
  AVR32_PWM.channel[PWM_DAC_PWM_CHANNEL_MSB].cprd = 0x000000FF;

  AVR32_PWM.ena = 1 << PWM_DAC_PWM_CHANNEL_LSB |
                  1 << PWM_DAC_PWM_CHANNEL_MSB;

  pwm_dac_output_sample(0x0000);

  INTC_register_interrupt(pwm_dac_pwm_int_handler, PWM_DAC_PWM_IRQ, PWM_DAC_PWM_INT_LEVEL);
}


void pwm_dac_setup(uint32_t sample_rate_hz,
                   uint8_t num_channels,
                   uint8_t bits_per_sample,
                   bool swap_channels,
                   void (*callback)(uint32_t arg),
                   uint32_t callback_opt,
                   uint32_t pba_hz)
{
  uint32_t diva = (pba_hz + (sample_rate_hz << 7)) / (sample_rate_hz << 8);
  diva = min(max(diva, AVR32_PWM_MR_DIVA_CLK_SEL_PREAB), (1 << AVR32_PWM_MR_DIVA_SIZE) - 1);

  AVR32_PWM.mr = diva                      << AVR32_PWM_MR_DIVA_OFFSET |
                 AVR32_PWM_MR_PREA_MCK     << AVR32_PWM_MR_PREA_OFFSET |
                 AVR32_PWM_MR_DIVB_CLK_OFF << AVR32_PWM_MR_DIVB_OFFSET |
                 AVR32_PWM_MR_PREB_MCK     << AVR32_PWM_MR_PREB_OFFSET;

  pwm_dac_output_params.num_channels            = num_channels;
  pwm_dac_output_params.bits_per_sample         = bits_per_sample;
  pwm_dac_output_params.swap_channels           = swap_channels;
  pwm_dac_output_params.callback                = callback;
  pwm_dac_output_params.callback_opt            = callback_opt;

  if (pwm_dac_output_params.num_channels < 2) pwm_dac_output_params.swap_channels = false;

  pwm_dac_output_params.get_sample =
    PWM_DAC_GET_SAMPLE[pwm_dac_get_log2_aligned_byte_size(pwm_dac_output_params.bits_per_sample)];
}


bool pwm_dac_output(void *sample_buffer, size_t sample_length)
{
  if (pwm_dac_output_params.reload_sample_length) return false;

  if (sample_length)
  {
    pwm_dac_output_params.reload_sample_buffer.u8ptr  = sample_buffer;

    if (pwm_dac_output_params.swap_channels)
      pwm_dac_output_params.reload_sample_buffer.u8ptr +=
        1 << pwm_dac_get_log2_aligned_byte_size(pwm_dac_output_params.bits_per_sample);

    pwm_dac_output_params.reload_sample_length        = sample_length;

    AVR32_PWM.ier = 1 << PWM_DAC_PWM_CHANNEL_LSB |
                    1 << PWM_DAC_PWM_CHANNEL_MSB;
  }

  return true;
}


void pwm_dac_increase_volume(void)
{
  if (pwm_dac_output_params.volume_shift) pwm_dac_output_params.volume_shift--;
}


void pwm_dac_decrease_volume(void)
{
  if (pwm_dac_output_params.volume_shift < 16) pwm_dac_output_params.volume_shift++;
}

uint8_t pwm_dac_get_volume(void)
{
  return (uint8_t) (((uint32_t) pwm_dac_output_params.volume_shift * 255) / 16);
}

void pwm_dac_set_volume(uint8_t volume)
{

  pwm_dac_output_params.volume_shift =(uint8_t)(((uint32_t)volume *16) / 255);
}

bool pwm_dac_is_volume_muted(void)
{
  return false;
}

bool pwm_dac_is_volume_boosted(void)
{
  return false;
}

void pwm_dac_flush(void)
{
  pwm_dac_output_params.flushing = true;

  while (AVR32_PWM.imr & (1 << PWM_DAC_PWM_CHANNEL_LSB |
                          1 << PWM_DAC_PWM_CHANNEL_MSB));

  pwm_dac_output_sample(0x0000);

  pwm_dac_output_params.flushing = false;
}


void pwm_dac_stop(void)
{
  pwm_dac_flush();

  AVR32_PWM.dis = 1 << PWM_DAC_PWM_CHANNEL_LSB |
                  1 << PWM_DAC_PWM_CHANNEL_MSB;

  gpio_enable_gpio(PWM_DAC_PWM_GPIO_MAP,
                   sizeof(PWM_DAC_PWM_GPIO_MAP) / sizeof(PWM_DAC_PWM_GPIO_MAP[0]));

  pwm_dac_output_params.num_channels            = 0;
  pwm_dac_output_params.bits_per_sample         = 0;
  pwm_dac_output_params.swap_channels           = false;
  pwm_dac_output_params.callback                = NULL;
  pwm_dac_output_params.callback_opt            = 0;
  pwm_dac_output_params.get_sample              = NULL;
  pwm_dac_output_params.volume_shift            = 0;
}

void pwm_dac_mute(bool mute)
{
}
