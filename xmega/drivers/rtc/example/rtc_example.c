/**
 * \file
 *
 * \brief AVR XMEGA RTC example
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 * For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

