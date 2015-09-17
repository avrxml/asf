/**
 * \file
 *
 * \brief AVR XMEGA C42048A LCD component Example
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
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref c42048a_group  to write characters,
 * set or clear icons on LCD glass.
 *
 * \section files Main files:
 *  - c42048a.c LCD component driver implementation
 *  - c42048a.h LCD configuration of LCD targeted
 *  - lcd.h LCD XMEGA Controller driver definitions
 *  - lcd.c LCD XMEGA Controller driver implementation
 *  - lcd.h LCD XMEGA Controller driver definitions
 *  - lcd_c42048a_example.c example application
 *  - conf_example.h: configuration of the example
 *
 * \section driverinfo LCD Component Driver
 * The YMMC42048A LCD component driver can be found \ref c42048a_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an tc can be used.
 *
 * \section exampledescription Description of the example
 *
 * The example configures the LCD Controller in a mode defined by the LCD glass
 * board connections and the technical characteristics of the LCD component
 * used. It used the following LCD controller features:
 *  - the LCD interrupt,
 *  - the hardware blinking,
 *  - the digit hardware encoder,
 *  - the LCD contrast control.
 *
 * The LCD is setup to use the 32.768 kHz from ULP and generates LCD frames at
 * 64 Hz using a low power waveform to reduce toggle activity and hence power
 * consumption. To show the LCD controller capability to run in power-save mode,
 * this mode is applied when it is possible.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <compiler.h>
#include <board.h>
#include <conf_example.h>
#include <pmic.h>
#include <sleepmgr.h>
#include <string.h>
#include <sysclk.h>
#include <gpio.h>
#include <c42048a.h>

int main(void)
{
	uint8_t lcd_text[]    = "XmegaB1";
	uint16_t i;

	sysclk_init();
	pmic_init();
	sleepmgr_init();
	board_init();
	c42048a_init();

	c42048a_set_contrast(60);
	c42048a_blinkrate_init(LCD_BLINKRATE_1Hz_gc);

	// Alphanumeric
	c42048a_set_text(lcd_text);
	// Numeric
	c42048a_set_numeric_dec(1245);

	// All pixels "on" blinking
	c42048a_set_blink_screen();
	c42048a_wait_int_period(16);
	c42048a_clear_blink_screen();

	// AVR icon blinking alone
	c42048a_blink_pixel(ICON_AVR);
	c42048a_wait_int_period(16);

	// AVR icon on
	c42048a_set_pixel(ICON_AVR);

	// USB icon blinking
	c42048a_blink_pixel(ICON_USB);
	// AM is not part of blinking icons
	// AM will be ON only
	c42048a_blink_pixel(ICON_AM);

	// Display a progress bar graph value
	for(i=1; i<256; i+=16) {
		c42048a_bar_graph((uint8_t)i);
		c42048a_wait_int_period(1);
	}
	c42048a_wait_int_period(4);

	// Blink entire screen 8 times
	c42048a_set_blink_screen();
	c42048a_wait_int_period(16);

	// Unblink all the screen
	c42048a_clear_blink_screen();

	while(true) {
		sleepmgr_enter_sleep();
	}
}
