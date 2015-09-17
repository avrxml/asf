/*****************************************************************************
 *
 * \file
 *
 * \brief WDT example application for AVR32 UC3.
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
 * - wdt.c: WDT driver;
 * - wdt.h: WDT driver header file;
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
 * - EVK1100, EVK1101, EVK1104 or EVK1105 evaluation kit;
 * - CPU clock: 12000000 MHz;
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
#include "pm.h"
#include "cycle_counter.h"

void led_task(void);
void wdt_scheduler(void);

// Min value of 1s
#define WDT_MIN_VALUE_US   1000000
// Min value of 4s
#define WDT_MAX_VALUE_US   4000000
// Step of 1s
#define WDT_CTRL_STEP_US   1000000

// To specify which step of led task
uint8_t step_led_task = 0;

// To specify which current Watchdog value
wdt_opt_t opt = {
          .us_timeout_period = WDT_MIN_VALUE_US  // Timeout Value
};

/*! \brief Led Task to scroll led before reset
 */
void led_task(void)
{
	switch (step_led_task)
	{
	case 0:
		gpio_clr_gpio_pin(LED1_GPIO);
		gpio_set_gpio_pin(LED2_GPIO);
		gpio_set_gpio_pin(LED3_GPIO);
		break;
	case 1:
		gpio_set_gpio_pin(LED1_GPIO);
		gpio_clr_gpio_pin(LED2_GPIO);
		gpio_set_gpio_pin(LED3_GPIO);
		break;
	case 2:
		gpio_set_gpio_pin(LED1_GPIO);
		gpio_set_gpio_pin(LED2_GPIO);
		gpio_clr_gpio_pin(LED3_GPIO);
		break;
	default:
		gpio_clr_gpio_pin(LED1_GPIO);
		gpio_set_gpio_pin(LED2_GPIO);
		gpio_set_gpio_pin(LED3_GPIO);
		break;
	}

	step_led_task++;
	cpu_delay_ms(300,FOSC0);
}

/*! \brief Watchdog scheduler
 */
void wdt_scheduler(void)
{
	volatile avr32_pm_t* pm = &AVR32_PM;

	// If Reset Cause is due to a Watchdog reset just relaunch Watchdog and turn
	// LED0 to 4 on to let user know that a new wdt reset has occurred.
	if (pm->RCAUSE.wdt) {
		wdt_reenable();

		gpio_clr_gpio_pin(LED0_GPIO);
		gpio_set_gpio_pin(LED1_GPIO);
		gpio_set_gpio_pin(LED2_GPIO);
		gpio_set_gpio_pin(LED3_GPIO);

		cpu_delay_ms(300,FOSC0);
	}
	// If Reset Cause is due to a Power On reset, enable Watchdog with default value
	else if (pm->RCAUSE.por) {
		opt.us_timeout_period = WDT_MIN_VALUE_US ;

		// Save current value in GPLP register
		pm_write_gplp(pm, 0, opt.us_timeout_period);
		wdt_enable(&opt);
	}
	// If Reset Cause is due to an External reset, increment opt.us_timeout_period
	else if (pm->RCAUSE.ext) {
		// Reload current value stored in GPLP register
		opt.us_timeout_period = pm_read_gplp(pm, 0);
		opt.us_timeout_period += WDT_CTRL_STEP_US;

		if (opt.us_timeout_period >= WDT_MAX_VALUE_US)
			opt.us_timeout_period = WDT_MIN_VALUE_US;

		wdt_enable(&opt);

		// Save new value in GPLP register
		pm_write_gplp(pm,0,opt.us_timeout_period);
	}
	// Else relaunch Watchdog and toggle GPIO to let user know that a new reset has occurred
	else {
		opt.us_timeout_period = WDT_MIN_VALUE_US;

		// Save start value of watchdog in GPLP register
		pm_write_gplp(pm, 0, opt.us_timeout_period);
		wdt_enable(&opt);
	}
}

/* \brief Main entry point
 * This is an example of how to use watchdog.
 */
int main(void)
{
	// Switch main clock to external oscillator 0 (crystal).
	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

	// Call Watchdog scheduler
	wdt_scheduler();

	while(1)
	{
		// Launch led task
		led_task();
	}
}
