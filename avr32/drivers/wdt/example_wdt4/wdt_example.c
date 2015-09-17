/*****************************************************************************
 *
 * \file
 *
 * \brief WDT example application for AVR32 UC3.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the WDT software driver.\n It also comes
 * bundled with an example.
 *
 * This example will start a watchdog scheduler and scroll led until a reset occurs.
 * \n At first, the watchdog is enabled with the min value WDT_MIN_VALUE(i.e. 1s),
 * then :
 *       - If a Watchdog reset occurs, the watchdog is re-enabled with the current value
 *       - If a External reset occurs, the watchdog is enabled with the current value incremented of 1s.
 * When the WDT timeout period reaches 4s, it is reset back to 1s. So, possible wdt delays(in this example) are 1s, 2s and 3s.
 *       - If a Power On reset occurs, the watchdog is reset and enabled with the default value, WDT_MIN_VALUE(i.e. 1s).
 *
 *
 * \section files Main Files
 * - wdt4.c: WDT driver;
 * - wdt4.h: WDT driver header file;
 * - wdt_example.c: WDT example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a WDT module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - STK600+RCUC3D routing card;
 * - CPU clock:
 *              - 16000000 MHz: UC3C_EK;
 *              - 12000000 MHz: STK600+RCUC3D;
 * - STK600+RCUC3D Board Configuration :
 *     - connect STK600.PA4 to STK600.LEDS.LED0
 *     - connect STK600.PA5 to STK600.LEDS.LED1
 *     - connect STK600.PA6 to STK600.LEDS.LED2
 *     - connect STK600.PA7 to STK600.LEDS.LED3
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "board.h"
#include "compiler.h"
#include "gpio.h"
#include "wdt.h"
#include "power_clocks_lib.h"
#include "cycle_counter.h"

// Min value of 1s
#define WDT_MIN_VALUE_US   1000000
// Min value of 4s
#define WDT_MAX_VALUE_US   4000000
// Step of 1s
#define WDT_CTRL_STEP_US   1000000

// To specify which current Watchdog value
volatile U32 current_wdt_value = WDT_MIN_VALUE_US;
// To specify which step of led task
volatile U8 step_led_task = 0;
// To specify which step of led task
wdt_opt_t opt = {
          .dar   = false,     // After a watchdog reset, the WDT will still be enabled.
          .mode  = WDT_BASIC_MODE,    // The WDT is in basic mode, only PSEL time is used.
          .sfv   = false,     // WDT Control Register is not locked.
          .fcd   = false,     // The flash calibration will be redone after a watchdog reset.
          .cssel = WDT_CLOCK_SOURCE_SELECT_RCSYS,       // Select the system RC oscillator (RCSYS) as clock source.
          .us_timeout_period = WDT_MIN_VALUE_US  // Timeout Value
};

/*! \brief Led Task to scroll led before reset
 */
void led_task()
{
  switch(step_led_task)
  {
    case 0:
      gpio_clr_gpio_pin(LED1_GPIO);
      gpio_set_gpio_pin(LED2_GPIO);
      gpio_set_gpio_pin(LED3_GPIO);
      step_led_task=1;
      cpu_delay_ms(300,FOSC0);
      break;
    case 1:
      gpio_set_gpio_pin(LED1_GPIO);
      gpio_clr_gpio_pin(LED2_GPIO);
      gpio_set_gpio_pin(LED3_GPIO);
      step_led_task=2;
      cpu_delay_ms(300,FOSC0);
      break;
    case 2:
      gpio_set_gpio_pin(LED1_GPIO);
      gpio_set_gpio_pin(LED2_GPIO);
      gpio_clr_gpio_pin(LED3_GPIO);
      step_led_task=0;
      cpu_delay_ms(300,FOSC0);
      break;
    default :
      gpio_clr_gpio_pin(LED1_GPIO);
      gpio_set_gpio_pin(LED2_GPIO);
      gpio_set_gpio_pin(LED3_GPIO);
      step_led_task=1;
      cpu_delay_ms(300,FOSC0);
      break;
  }
}

/*! \brief Watchdog scheduler
 */
void wdt_scheduler(void)
{
  // If Reset Cause is due to a Watchdog reset just relaunch Watchdog and turn
  // LED0 to 4 on to let user know that a new wdt reset has occurred.
  if(AVR32_PM.RCAUSE.wdt) {
      wdt_reenable();
      gpio_clr_gpio_pin(LED0_GPIO);
      gpio_clr_gpio_pin(LED1_GPIO);
      gpio_clr_gpio_pin(LED2_GPIO);
      gpio_clr_gpio_pin(LED3_GPIO);
      cpu_delay_ms(300,FOSC0);
  // If Reset Cause is due to a Power On reset, enable Watchdog with default value
  }else if (AVR32_PM.RCAUSE.por) {
      current_wdt_value = WDT_MIN_VALUE_US ;//WDT_MIN_VALUE;
      // Save current value in GPLP register
      pcl_write_gplp(0,current_wdt_value);
      opt.us_timeout_period = current_wdt_value;
      wdt_enable(&opt);
  // If Reset Cause is due to an External reset, increment current_wdt_value
  }else if (AVR32_PM.RCAUSE.ext) {
      // Reload current value stored in GPLP register
      current_wdt_value = pcl_read_gplp(0);
      current_wdt_value += WDT_CTRL_STEP_US; //WDT_CTRL_STEP;
      if (current_wdt_value >=  WDT_MAX_VALUE_US) current_wdt_value = WDT_MIN_VALUE_US;
      opt.us_timeout_period = current_wdt_value;
      wdt_enable(&opt);
      // Save new value in GPLP register
      pcl_write_gplp(0,current_wdt_value);
  // Else relaunch Watchdog and toggle GPIO to let user know that a new reset has occurred
  }else{
      current_wdt_value = WDT_MIN_VALUE_US; //WDT_MIN_VALUE
      // Save start value of watchdog in GPLP register
      pcl_write_gplp(0,current_wdt_value);
      opt.us_timeout_period = current_wdt_value;
      wdt_enable(&opt);
  }
}

/* \brief Main entry point
 * This is an example of how to use watchdog.
 */
int main(void)
{
  // Switch main clock to external oscillator 0 (crystal).
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

  // Call Watchdog scheduler
  wdt_scheduler();

  while(1)
  {
    // Launch led task
    led_task();
  }

}
