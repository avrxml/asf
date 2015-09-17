/**
 * \file
 *
 * \brief Management of C42048A LCD Glass component.
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
#include "c42048a.h"
#include "lcd.h"

void c42048a_init(void)
{
	// LCD_initialization
	lcd_clk_init();
	lcd_connection_init(COM_SWP, SEG_SWP, PORT_MASK, X_BIAS);
	lcd_timing_init(LCD_PRESC_16_gc, LCD_CLKDIV_DivBy4_gc,
			LCD_LP_WAVE_ENABLE_gc, LCD_DUTY);
	lcd_interrupt_init(LCD_INTLVL_OFF_gc, 16);
	lcd_blinkrate_init(LCD_BLINKRATE_2Hz_gc);
	lcd_enable();
}

void c42048a_bar_graph(uint8_t val)
{
	lcd_clear_pixel(ICON_LEVEL_1);
	lcd_clear_pixel(ICON_LEVEL_2);
	lcd_clear_pixel(ICON_LEVEL_3);
	lcd_clear_pixel(ICON_LEVEL_4);
	lcd_clear_pixel(ICON_LEVEL_5);
	lcd_clear_pixel(ICON_LEVEL_6);
	lcd_clear_pixel(ICON_LEVEL_7);
	lcd_clear_pixel(ICON_LEVEL_8);
	lcd_set_pixel(ICON_LEVELBAR);
	if (val>224) {
		lcd_set_pixel(ICON_LEVEL_8);
	}
	if (val>192) {
		lcd_set_pixel(ICON_LEVEL_7);
	}
	if (val>160) {
		lcd_set_pixel(ICON_LEVEL_6);
	}
	if (val>128) {
		lcd_set_pixel(ICON_LEVEL_5);
	}
	if (val>96) {
		lcd_set_pixel(ICON_LEVEL_4);
	}
	if (val>64) {
		lcd_set_pixel(ICON_LEVEL_3);
	}
	if (val>32) {
		lcd_set_pixel(ICON_LEVEL_2);
	}
	if (val>0) {
		lcd_set_pixel(ICON_LEVEL_1);
	}
}

/**
 * \brief Conversion of a half byte value in ASCII code
 *
 * \param val half byte to convert
 */
static uint8_t conv_ascii(uint8_t val)
{
	Assert(val<0x10);

	if (val<= 0x09) {
		val = val + 0x30;
	}
	if ((val >= 0x0A) && (val<= 0x0F)) {
		val = val + 0x37;
	}
	return val;
}

void c42048a_set_numeric_dec(uint16_t val)
{
	uint8_t lcd_num[5];
	uint16_t tmp;

	// Compute each digit char value
	tmp = val / 10000;
	if (tmp) {
		lcd_num[0] = conv_ascii(tmp);
	} else
	lcd_num[0] = ' ';

	val = val - tmp*10000;
	tmp = val / 1000;
	if (tmp) {
		lcd_num[1] = conv_ascii(tmp);
	} else
	lcd_num[1] = ' ';

	val = val - tmp*1000;
	tmp = val / 100;
	if (tmp || lcd_num[1]!=' ') {
		lcd_num[2] = conv_ascii(tmp);
	} else lcd_num[2] = ' ';
	val = val - tmp*100;

	tmp = val / 10;
	if (tmp  || lcd_num[2]!=' ' || lcd_num[1]!=' ') {
		lcd_num[3] = conv_ascii(tmp);
	} else lcd_num[3] = ' ';
	val = val - tmp*10;
	lcd_num[4] = conv_ascii(val);

#ifdef CONFIG_XMEGA_128B1_REVA
	//! Save pixels that characters generator may overwrite
	bool p1 = lcd_get_pixel(ICON_DEGREE_C);
	bool p2 = lcd_get_pixel(ICON_DEGREE_F);
	bool p3 = lcd_get_pixel(ICON_VOLT);
	bool p4 = lcd_get_pixel(ICON_MILLI_VOLT);
#endif
	//! Write to character generator
	c42048a_write_num_packet((uint8_t const*)&lcd_num);
#ifdef CONFIG_XMEGA_128B1_REVA
	//! Restore overwritten pixels
	if (p1) {
		lcd_set_pixel(ICON_DEGREE_C);
	}
	if (p2) {
		lcd_set_pixel(ICON_DEGREE_F);
	}
	if (p3) {
		lcd_set_pixel(ICON_VOLT);
	}
	if (p4) {
		lcd_set_pixel(ICON_MILLI_VOLT);
	}
#endif
}

#ifdef CONFIG_XMEGA_128B1_REVA
void c42048a_set_text(const uint8_t *data) {
	bool bar1,bar2,bar3,bar4,bar5,bar6,bar7,bar8,am,pm;

	//! Save pixels that characters generator may overwrite
	bar1 = lcd_get_pixel(ICON_LEVEL_1);
	bar2 = lcd_get_pixel(ICON_LEVEL_2);
	bar3 = lcd_get_pixel(ICON_LEVEL_3);
	bar4 = lcd_get_pixel(ICON_LEVEL_4);
	bar5 = lcd_get_pixel(ICON_LEVEL_5);
	bar6 = lcd_get_pixel(ICON_LEVEL_6);
	bar7 = lcd_get_pixel(ICON_LEVEL_7);
	bar8 = lcd_get_pixel(ICON_LEVEL_8);
	am = lcd_get_pixel(ICON_AM);
	pm = lcd_get_pixel(ICON_PM);
	//! Write to character generator
	c42048a_write_alpha_packet(data);
	//! Restore overwritten pixels
	if (bar1) {
		lcd_set_pixel(ICON_LEVEL_1);
	}
	if (bar2) {
		lcd_set_pixel(ICON_LEVEL_2);
	}
	if (bar3) {
		lcd_set_pixel(ICON_LEVEL_3);
	}
	if (bar4) {
		lcd_set_pixel(ICON_LEVEL_4);
	}
	if (bar5) {
		lcd_set_pixel(ICON_LEVEL_5);
	}
	if (bar6) {
		lcd_set_pixel(ICON_LEVEL_6);
	}
	if (bar7) {
		lcd_set_pixel(ICON_LEVEL_7);
	}
	if (bar8) {
		lcd_set_pixel(ICON_LEVEL_8);
	}
	if (am) {
		lcd_set_pixel(ICON_AM);
	}
	if (pm) {
		lcd_set_pixel(ICON_PM);
	}
}
#endif
