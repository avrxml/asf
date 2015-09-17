/**
 * \file
 *
 * \brief AVR XMEGA LCD Driver Example
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
 * This simple example shows how to use the \ref lcd_group  to write characters,
 * set or clear icons on LCD glass.
 *
 * \section files Main files:
 *  - lcd.c LCD XMEGA Controller driver implementation
 *  - lcd.h LCD XMEGA Controller driver definitions
 *  - lcd_example.c example application
 *  - conf_example.h: configuration of the example
 *  - c42048a.h: configuration of LCD targeted
 *
 * \section driverinfo LCD Driver
 * The XMEGA LCD driver can be found \ref lcd_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an LCD module can be used.
 *
 * \section exampledescription Description of the example
 * After an intialization phase where all pixels of LCD are blinking, the LCD
 * displays a "welcome" sequence. Then, for one of the Touch-Buttons pressed,
 * a dedicated sentence is displayed and a pseudo stopwatch is started (mn:s).
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
 * The example also uses a PWM output to control the LCD backlight.
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
#include "asf.h"
#include "c42048a.h"
#include "conf_example.h"

/**
 * \brief LCD frame interrupt callback function
 *
 * This function is called when LCD interrupt has occurred and toggles LED3.
 */
static void example_lcd_interrupt_callback(void)
{
	gpio_toggle_pin(LED3_GPIO);
}



int main(void)
{

	uint8_t lcd_init_alpha[]    = "XMEGA B";
	uint8_t lcd_init_num[]      = "12345";
	uint8_t lcd_welcome_alpha[] = "welcome";
	uint8_t lcd_welcome_num[]   = " 0000";

	uint8_t lcd_display_alpha_0[] = "      Hello AVR world       ";
	uint8_t lcd_display_alpha_1[] = "      My name is XMEGA B and I drive this LCD      ";
	uint8_t lcd_display_alpha_2[] = "      I was born in ATMEL      ";
	uint8_t lcd_display_alpha_3[] = "      My first words are for you       ";

	uint8_t lcd_array = 0;
	uint8_t n,i,j,k;
	uint8_t count, lcd_count_sec, lcd_count_mn;


	sysclk_init();
	pmic_init();
	sleepmgr_init();
	board_init();
	user_led_set(lcd_array);

	// LCD_init
	// - Clock,
	// - Connection: to c42048a component,
	// - Timing:  64 Hz frame rate & low power waveform,
	// - Interrupt: off
	// - Blink: 2 hz
	/*
	lcd_clk_init();
	lcd_connection_init(COM_SWP, SEG_SWP, PORT_MASK, X_BIAS);
	lcd_timing_init(LCD_PRESC_16_gc, LCD_CLKDIV_DivBy4_gc,
			LCD_LP_WAVE_ENABLE_gc, LCD_DUTY);
	lcd_interrupt_init(LCD_INTLVL_OFF_gc, 0);
	lcd_blinkrate_init(LCD_BLINKRATE_2Hz_gc);
	lcd_enable();
	*/
	c42048a_init();

#ifndef CONF_BOARD_LCD_BACKLIGHT_PWM
	backlight_on();
#endif

	// Configure interrupts callback functions for LCD interrupt
	lcd_set_frame_interrupt_callback(example_lcd_interrupt_callback);
	cpu_irq_enable();

	// Init contrast display (see conf_example.h)
	lcd_set_contrast(LCD_CONTRAST_LEVEL);

	// LCD displays init "words"
	// Alphanumeric
	c42048a_write_alpha_packet(lcd_init_alpha);
	// Numeric
	c42048a_write_num_packet(lcd_init_num);

	// All pixels "on" are blinking
	lcd_clear_blink_all_pixel();
	lcd_blink_enable();
	lcd_wait_n_int(16);
	lcd_blink_disable();

	// AVR icon blinking alone
	lcd_set_blink_pixel(ICON_AVR);
	lcd_set_pixel(ICON_AVR);
	lcd_blink_enable();

//! LCD displays "welcome" message
	// Alphanumeric
	c42048a_write_alpha_packet(lcd_welcome_alpha);
	// Numeric
	c42048a_write_num_packet(lcd_welcome_num);

//! eXtended Interrupt Mode Level-0 & FCIF flag generated every 16 frames => 4Hz
	lcd_interrupt_init(LCD_INTLVL_LO_gc,16);

//! LCD is ready to display up to 4 sentences
	n = 0;
	while (true) {
		k=1;
		j=0;
		while(k) {
			//! Load the selected sentences
			switch (lcd_array) {
				case 0: // At starting point
					lcd_set_blink_pixel(ICON_COLON);
					lcd_set_pixel(ICON_COLON);
					break;
				case 1: // First sentence
					for(i=0; i<WIDTH_14SEG_4C; i++) {
						lcd_welcome_alpha[i] = lcd_display_alpha_0[j+i];
					}
					if(lcd_display_alpha_0[j+WIDTH_14SEG_4C]==0) k=0;
					break;
				case 2: // Second sentence
					for(i=0; i<WIDTH_14SEG_4C; i++) {
						lcd_welcome_alpha[i] = lcd_display_alpha_1[j+i];
					}
					if(lcd_display_alpha_1[j+WIDTH_14SEG_4C]==0) k=0;
					break;
				case 3: // Third sentence
					for(i=0; i<WIDTH_14SEG_4C; i++) {
						lcd_welcome_alpha[i] = lcd_display_alpha_2[j+i];
					}
					if(lcd_display_alpha_2[j+WIDTH_14SEG_4C]==0) k=0;
					break;
				case 4: // Fourth sentence
					for(i=0; i<WIDTH_14SEG_4C; i++) {
						lcd_welcome_alpha[i] = lcd_display_alpha_3[j+i];
					}
					if(lcd_display_alpha_3[j+WIDTH_14SEG_4C]==0) k=0;
					break;
				default:
					break;
			}
			c42048a_write_alpha_packet(lcd_welcome_alpha);
			j++;

			//! Enter in sleep mode (POWER_SAVE)
			sleepmgr_enter_sleep();

			//! Select a new sentence
			if       ((is_touch3()) && (lcd_array!=4)) {
				lcd_array=4; k=n=0;
			}
			else if((is_touch2()) && (lcd_array!=3)) {
				lcd_array=3; k=n=0;
			}
			else if((is_touch1()) && (lcd_array!=2)) {
				lcd_array=2; k=n=0;
			}
			else if((is_touch0()) && (lcd_array!=1)) {
				lcd_array=1; k=n=0;
			}
			//! Display a pseudo stopwatch
			if (lcd_array!=0) {
				if (n==0) {
					// Reset display and count
					user_led_set(lcd_array);
					lcd_welcome_num[0] = ' ';
					for(i=1; i<5; i++) lcd_welcome_num[i] = '0';
					count = 0; // Reset lcd_count_sec
					lcd_count_mn = 0;
					n=1;
				}
				// Prepare seconds to display
				lcd_welcome_num[4] = (lcd_count_sec % 10) + '0';
				lcd_welcome_num[3] = (lcd_count_sec / 10) + '0';
				// Prepare minutes to display
				lcd_welcome_num[2] = (lcd_count_mn % 10) + '0';
				lcd_welcome_num[1] = (lcd_count_mn / 10) + '0';
				// Display stopwatch values
				c42048a_write_num_packet(lcd_welcome_num);
				// Update stopwatch values
				count++;
				lcd_count_sec = (count >> 2); // Because INT freq = 4 Hz
				if(count == (60*4)) {
					count = 0;
					lcd_count_sec = 0;
					lcd_count_mn++;
					if(lcd_count_mn == 60)  lcd_count_mn = 0;
				}
			}
		}
	}
}
