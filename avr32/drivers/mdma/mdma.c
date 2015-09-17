/*****************************************************************************
 *
 * \file
 *
 * \brief MDMA driver for AVR32 UC3.
 *
 * AVR32 MDMA driver module.
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


#include <avr32/io.h>
#include "compiler.h"
#include "mdma.h"

unsigned long mdma_get_interrupt_settings(volatile avr32_mdma_t *mdma)
{
  return mdma->imr;
}

void mdma_configure_interrupts(volatile avr32_mdma_t *mdma, const mdma_interrupt_t *bitfield)
{
  bool global_interrupt_enabled = Is_global_interrupt_enabled();

  // Enable the appropriate interrupts.
  mdma->ier            = bitfield->ch0c << AVR32_MDMA_IER_CH0C_OFFSET |
                         bitfield->ch1c << AVR32_MDMA_IER_CH1C_OFFSET |
                         bitfield->ch2c << AVR32_MDMA_IER_CH2C_OFFSET |
                         bitfield->ch3c << AVR32_MDMA_IER_CH3C_OFFSET |
                         bitfield->berr0 << AVR32_MDMA_IER_BERR0_OFFSET |
                         bitfield->berr1 << AVR32_MDMA_IER_BERR1_OFFSET |
                         bitfield->berr2 << AVR32_MDMA_IER_BERR2_OFFSET |
                         bitfield->berr3 << AVR32_MDMA_IER_BERR3_OFFSET ;


  // Disable the appropriate interrupts.
  if (global_interrupt_enabled) Disable_global_interrupt();
  mdma->idr            = (~bitfield->ch0c & 1)  << AVR32_MDMA_IDR_CH0C_OFFSET |
                         (~bitfield->ch1c & 1)  << AVR32_MDMA_IDR_CH1C_OFFSET |
                         (~bitfield->ch2c & 1)  << AVR32_MDMA_IDR_CH2C_OFFSET |
                         (~bitfield->ch3c & 1)  << AVR32_MDMA_IDR_CH3C_OFFSET |
                         (~bitfield->berr0 & 1) << AVR32_MDMA_IDR_BERR0_OFFSET |
                         (~bitfield->berr1 & 1) << AVR32_MDMA_IDR_BERR1_OFFSET |
                         (~bitfield->berr2 & 1) << AVR32_MDMA_IDR_BERR2_OFFSET |
                         (~bitfield->berr3 & 1) << AVR32_MDMA_IDR_BERR3_OFFSET ;

  if (global_interrupt_enabled) Enable_global_interrupt();

}

void mdma_single_mode_xfert_init(volatile avr32_mdma_t *mdma, U32 channel, const mdma_opt_t *opt, U32* src, U32* dest)
{

  mdma->channel[channel].ccr    = opt->count        << AVR32_MDMA_CCR0_TCNT_OFFSET  |
                                  opt->size         << AVR32_MDMA_CCR0_SIZE_OFFSET  |
                                  opt->burst_size   << AVR32_MDMA_CCR0_BURST_OFFSET |
                                  opt->tc_ienable   << AVR32_MDMA_CCR0_TCIE_OFFSET ;
  mdma->channel[channel].rar    = (U32)src ;
  mdma->channel[channel].war    = (U32)dest;

  mdma->cr                      = (MDMA_SINGLE_TRANSFERT_MODE)<< (channel) << (AVR32_MDMA_CR_CH0M_OFFSET) ;
}

void mdma_descriptor_mode_xfert_init(volatile avr32_mdma_t *mdma, U32 channel, U32* start_adress)
{
#if AVR32_MDMA_H_VERSION < 101
  mdma->descriptor_channel[channel].dsa   = (U32)start_adress;
#else
  mdma->descriptor_channel[channel].dsar   = (U32)start_adress;
#endif
  mdma->channel[channel].cdar             = (U32)start_adress;
}

void mdma_start_single_xfert(volatile avr32_mdma_t *mdma, U32 channels, U8 arbitration)
{
  mdma->CR.arb = arbitration;
  mdma->cr     = (1<<channels)<< (AVR32_MDMA_CR_CH0EN_OFFSET);
}

void mdma_start_descriptor_xfert(volatile avr32_mdma_t *mdma, U32 channels, U8 arbitration)
{
  mdma->CR.arb = arbitration;
  mdma->cr    |= ((1<<channels)<<AVR32_MDMA_CR_CH0EN_OFFSET)|(((MDMA_DESCRIPTOR_MODE)<<channels)<<AVR32_MDMA_CR_CH0M_OFFSET);
}

int mdma_stop_channels(volatile avr32_mdma_t *mdma, U32 channels)
{
  int timeout = MDMA_DEFAULT_TIMEOUT;

  while (mdma->cr & (1<<channels)<< (AVR32_MDMA_CR_CH0DIS_OFFSET) )
  {
    if (!timeout--) return MDMA_FAILURE;
  }
  return MDMA_SUCCESS;
}

int mdma_channels_is_enable(volatile avr32_mdma_t *mdma, U32 channels)
{
  if(mdma->cr&((1<<channels)<<(AVR32_MDMA_CR_CH0EN_OFFSET)))
      return MDMA_SUCCESS;
  else
    return MDMA_FAILURE;
}
