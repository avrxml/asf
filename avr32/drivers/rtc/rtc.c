/*****************************************************************************
 *
 * \file
 *
 * \brief RTC driver for AVR32 UC3.
 *
 * AVR32 Real Time Counter driver module.
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
