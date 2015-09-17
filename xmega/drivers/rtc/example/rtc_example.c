/**
 * \file
 *
 * \brief AVR XMEGA RTC example
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref rtc_group.
 *
 * \section files Main Files
 * - rtc.c RTC driver implementation
 * - rtc.h RTC driver definitions
 * - rtc_example.c example application
 * - conf_rtc.h RTC driver configurations
 *
 * \section device_info Device Info
 * All AVR XMEGA devices with a RTC can be used. This example have been
 * tested with the following setup:
 * - Xplain
 * - XMEGA A1 Xplained
 * - ATxmega32D4 using STK600-RC044x
 *
 * \section description Description of the example
 * The example will initalize the RTC and use the xplain LEDS or the STK600 PORTA
 * to show the last two digits from time in binary coded decimal (BCD).
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref rtc_group
 * - \ref gpio_group for LED output
 * - \ref pmic_group for interrupt handling
 * - \ref sleepmgr_group for sleep management
 *
 * \section compinfo Compilation info
 * This software was written for the GNU GCC and IAR for AVR. Other compilers
 * may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <conf_example.h>
#include <asf.h>

/**
 * \brief Alarm callback
 *
 * This outputs the last two digits of current time in BCD on the board LEDS,
 * and then reschedules the alarm in 1 second.
 */
static void alarm(uint32_t time)
{
	uint8_t bcd;

	/* Since the current time will give alarm when rolling over to
	 * next time unit, we just call with that one.
	 * This is safe to here since it's called from a time unit roll
	 * over.
	 */
	rtc_set_alarm(time);

	// Extract last two digits from time, and put them in bcd
	bcd = time % 10;
	time -= bcd;
	time /= 10;
	bcd = bcd | ((time % 10) << 4);

	// LEDS on xplain are active low, so invert the output
	bcd = ~bcd;

	LED_PORT.OUT = bcd;
}

int main(void)
{
	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();

	rtc_init();
	rtc_set_callback(alarm);

	cpu_irq_enable();

	/* We just initialized the counter so an alarm should trigger on next
	 * time unit roll over.
	 */
	rtc_set_alarm_relative(0);

	while (true) {
		/* Alarm action is handled in alarm callback so we just go to
		 * sleep here.
		 */
		sleepmgr_enter_sleep();
	}
}

