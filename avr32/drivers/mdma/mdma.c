/*****************************************************************************
 *
 * \file
 *
 * \brief MDMA driver for AVR32 UC3.
 *
 * AVR32 MDMA driver module.
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
