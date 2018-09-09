/**
 * \file
 *
 * \brief AVR XMEGA RTC32 example
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
 * This simple example shows how to use the \ref rtc32_group.
 *
 * \section files Main Files
 *  - rtc32.c RTC32 driver implementation
 *  - rtc32.h RTC32 driver definitions
 *  - rtc32_example.c example application
 *  - conf_rtc32.h RTC32 driver configurations
 *
 * \section device_info Device Info
 *  All AVR XMEGA devices with a RTC32 can be used. This example have been
 *  tested with the following setup:
 *  - STK600 and RC064X/TQFP64 with ATxmega256A3B
 *
 * \section setup Setup
 * The following setup is required on STK600:
 * - On connector AUX: Connect 32KHz to TOSC2 in order to have a clock source
 * for the RTC32
 * - For LED output: Connect full LEDS connector to PORTD
 *
 * Optional:
 * - For keeping RTC32 running between reset connect PF5 (VBAT) to VTG. Or to
 * have it running RTC32 on missing VTG connect PF5 to a external power source.
 *
 * \section description Description of the example
 * The example will initalize the RTC32 and use the STK600 LEDS to show the
 * last two digits from time in binary coded decimal (BCD).
 *
 * Due to the nature of the RTC32; alarms can't be given faster than every third
 * RTC32 clock cycle. And with a RTC32 clock cycle of 1Hz the LEDS will only
 * update every third second.
 *
 * Since the RTC32 is battery backed up it can keep track of it's time in the
 * case of reset or power off. See \ref setup for details on this.
 *
 * \section dependencies Dependencies
 * This example depends on the following modules:
 * - \ref rtc32_group
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
 * and then reschedules the alarm in 3 seconds.
 */
static void alarm(uint32_t time)
{
	uint8_t bcd;

	// Extract last two digits from time, and put them in bcd
	bcd = time % 10;
	time -= bcd;
	time /= 10;
	bcd = bcd | ((time % 10) << 4);

	// LEDS on STK600 are active low, so invert the output
	bcd = ~bcd;

	LED_PORT.OUT = bcd;

	/* Since the next alarm will be rounded up to the next second pass, this will
	 * actually happen in 3 seconds.
	 */
	rtc_set_alarm_relative(2);
}

/**
 * \brief main function
 */
int main(void)
{
	pmic_init();
	board_init();
	sysclk_init();
	sleepmgr_init();

	rtc_init();
	rtc_set_callback(alarm);

	cpu_irq_enable();

	/* The lowest value which is safe to use is 3. This the use of 2 could
	 * happen in a second change, and we would not get an interrupt. A
	 * value of 3 causes the alarm to be set of in 3-4 seconds.
	 */
	rtc_set_alarm_relative(3);

	while (true) {
		/* Alarm action is handled in alarm callback so we just go to
		 * sleep here.
		 */
		sleepmgr_enter_sleep();
	}
}
