/*****************************************************************************
 *
 * \file
 *
 * \brief RTC driver for AVR32 UC3.
 *
 * AVR32 Real Time Counter driver module.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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


#include <avr32/io.h>
#include "compiler.h"
#include "pm.h"
#include "rtc.h"


int rtc_is_busy(volatile avr32_rtc_t *rtc)
{
  return (rtc->ctrl & AVR32_RTC_CTRL_BUSY_MASK) != 0;
}


int rtc_init(volatile avr32_rtc_t *rtc, unsigned char osc_type, unsigned char psel)
{
  // If exit, it means that the configuration has not been set correctly
  if (osc_type > (1 << AVR32_RTC_CTRL_CLK32_SIZE) - 1 ||
      psel > (1 << AVR32_RTC_CTRL_PSEL_SIZE) - 1)
    return 0;

  // If we use the 32-kHz oscillator, we have to enable it first
  if (osc_type == RTC_OSC_32KHZ)
  {
    // Select the 32-kHz oscillator crystal
    pm_enable_osc32_crystal(&AVR32_PM);
    // Enable the 32-kHz clock and wait until the osc32 clock is ready.
    pm_enable_clk32(&AVR32_PM, AVR32_PM_OSCCTRL32_STARTUP_0_RCOSC);
  }

  // Wait until the rtc accepts writes to the CTRL register
  while (rtc_is_busy(rtc));

  // Set the new RTC configuration
  rtc->ctrl = osc_type << AVR32_RTC_CTRL_CLK32_OFFSET |
              psel << AVR32_RTC_CTRL_PSEL_OFFSET |
              AVR32_RTC_CTRL_CLKEN_MASK;

  // Wait until write is done
  while (rtc_is_busy(rtc));

  // Set the counter value to 0
  rtc_set_value(rtc, 0x00000000);
  // Set the top value to 0xFFFFFFFF
  rtc_set_top_value(rtc, 0xFFFFFFFF);

  return 1;
}


void rtc_set_value(volatile avr32_rtc_t *rtc, unsigned long val)
{
  // Wait until we can write into the VAL register
  while (rtc_is_busy(rtc));
  // Set the new val value
  rtc->val = val;
  // Wait until write is done
  while (rtc_is_busy(rtc));
}


unsigned long rtc_get_value(volatile avr32_rtc_t *rtc)
{
  return rtc->val;
}


void rtc_enable_wake_up(volatile avr32_rtc_t *rtc)
{
  // Wait until the rtc CTRL register is up-to-date
  while (rtc_is_busy(rtc));
  // Enable the wake up of the RTC
  rtc->ctrl |= AVR32_RTC_CTRL_WAKE_EN_MASK;
  // Wait until write is done
  while (rtc_is_busy(rtc));
}


void rtc_disable_wake_up(volatile avr32_rtc_t *rtc)
{
  // Wait until the rtc CTRL register is up-to-date
  while (rtc_is_busy(rtc));
  // Disable the wake up of the RTC
  rtc->ctrl &= ~AVR32_RTC_CTRL_WAKE_EN_MASK;
  // Wait until write is done
  while (rtc_is_busy(rtc));
}


void rtc_enable(volatile avr32_rtc_t *rtc)
{
  // Wait until the rtc CTRL register is up-to-date
  while (rtc_is_busy(rtc));
  // Enable the RTC
  rtc->ctrl |= AVR32_RTC_CTRL_EN_MASK;
  // Wait until write is done
  while (rtc_is_busy(rtc));
}


void rtc_disable(volatile avr32_rtc_t *rtc)
{
  // Wait until the rtc CTRL register is up-to-date
  while (rtc_is_busy(rtc));
  // Disable the RTC
  rtc->ctrl &= ~AVR32_RTC_CTRL_EN_MASK;
  // Wait until write is done
  while (rtc_is_busy(rtc));
}


void rtc_enable_interrupt(volatile avr32_rtc_t *rtc)
{
  rtc->ier = AVR32_RTC_IER_TOPI_MASK;
}


void rtc_disable_interrupt(volatile avr32_rtc_t *rtc)
{
  bool global_interrupt_enabled = cpu_irq_is_enabled();

  if (global_interrupt_enabled) cpu_irq_disable();
  rtc->idr = AVR32_RTC_IDR_TOPI_MASK;
  rtc->imr;
  if (global_interrupt_enabled) cpu_irq_enable();
}


void rtc_clear_interrupt(volatile avr32_rtc_t *rtc)
{
  bool global_interrupt_enabled = cpu_irq_is_enabled();

  if (global_interrupt_enabled) cpu_irq_disable();
  rtc->icr = AVR32_RTC_ICR_TOPI_MASK;
  rtc->isr;
  if (global_interrupt_enabled) cpu_irq_enable();
}


void rtc_set_top_value(volatile avr32_rtc_t *rtc, unsigned long top)
{
  // Wait until we can write into the VAL register
  while (rtc_is_busy(rtc));
  // Set the new val value
  rtc->top = top;
  // Wait until write is done
  while (rtc_is_busy(rtc));
}


unsigned long rtc_get_top_value(volatile avr32_rtc_t *rtc)
{
  return rtc->top;
}


int rtc_interrupt_enabled(volatile avr32_rtc_t *rtc)
{
  return (rtc->imr & AVR32_RTC_IMR_TOPI_MASK) != 0;
}


int rtc_is_interrupt(volatile avr32_rtc_t *rtc)
{
  return (rtc->isr & AVR32_RTC_ISR_TOPI_MASK) != 0;
}
