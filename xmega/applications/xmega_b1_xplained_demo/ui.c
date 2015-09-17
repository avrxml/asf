/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "asf.h"
#include "led.h"
#include <string.h>

//! Alphanumeric Display memory (visible)
static uint8_t lcd_alpha_display[LCD_TEST_DISPLAY_SIZE]  = "       ";

//! Counter for alphanumeric scrolling current position
static uint8_t alpha_offset=0;

//! Alphanumeric Display memory (visible+non visible for text scrolling)
static uint8_t lcd_alpha[LCD_TEXT_STRING_SIZE+2*LCD_TEST_DISPLAY_SIZE]
	= "      ";

//! Welcome text string
static uint8_t lcd_welcome[]
	= "XMEGA B1";

//! Error text string when Silicon version does not match firmware
static uint8_t lcd_rev_error[]
	= "REV ERR";

//! Error text string when Silicon version does not match firmware
static uint8_t lcd_dfu[]
	= "  DFU  ";

void ui_init(void)
{
	c42048a_init();
	ui_display_text(lcd_welcome,sizeof(lcd_welcome));
	c42048a_set_pixel(ICON_AVR);
	c42048a_blink_pixel(ICON_USB);
}

void ui_powerdown(void)
{
	backlight_stop_pwm();
	backlight_off();
	adc_disable(&LIGHT_SENSOR_ADC_MODULE);
	led_power_off();
}

void ui_si_revision_error(void)
{
	c42048a_blink_pixel(ICON_WARNING);
	c42048a_blink_pixel(ICON_ERROR);
	c42048a_set_text(lcd_rev_error);
}

void ui_dfu_session(void)
{
	c42048a_set_text(lcd_dfu);
	c42048a_blink_pixel(ICON_WARNING);
	c42048a_clear_pixel(ICON_AVR);
	led_power_off();
}

void ui_wakeup(void)
{
	backlight_start_pwm();
	adc_enable(&LIGHT_SENSOR_ADC_MODULE);
	led_power_on();
}

void ui_led(uint8_t led_val)
{
	if (led_val&0x01) {
		LED_On(LED0_GPIO);
	} else {
		LED_Off(LED0_GPIO);
	}
	if (led_val&0x02) {
		LED_On(LED1_GPIO);
	} else {
		LED_Off(LED1_GPIO);
	}
	if (led_val&0x04) {
		LED_On(LED2_GPIO);
	} else {
		LED_Off(LED2_GPIO);
	}
	if (led_val&0x08) {
		LED_On(LED3_GPIO);
	} else {
		LED_Off(LED3_GPIO);
	}
}

uint8_t ui_button(void)
{
	return ui_pusb_button_0() | ui_pusb_button_1()<<1
			| ui_pusb_button_2()<<2 | ui_pusb_button_3()<<3;
}


void ui_display_text(uint8_t *msg, uint8_t len)
{
	uint8_t i;

	memcpy( &lcd_alpha[LCD_TEST_DISPLAY_SIZE-1], msg, len);
	for (i=len+LCD_TEST_DISPLAY_SIZE-2; i<sizeof(lcd_alpha); i++) {
		lcd_alpha[i]=' ';
	}
	alpha_offset=0;
}

void ui_scroll_text(void)
{
	memcpy( lcd_alpha_display,&lcd_alpha[alpha_offset],
		LCD_TEST_DISPLAY_SIZE);
	c42048a_set_text(lcd_alpha_display);
	if (++alpha_offset>LCD_TEXT_STRING_SIZE) {
		alpha_offset=0;
	}
}
