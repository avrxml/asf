/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
