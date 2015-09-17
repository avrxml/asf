/**
 * \file
 *
 * \brief Board specific definition for resistive touch example.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef RESISTIVE_TOUCH_BOARD_H_INCLUDED
#define RESISTIVE_TOUCH_BOARD_H_INCLUDED

#include "hx8347a.h"

#define lcd_set_foreground_color  hx8347a_set_foreground_color
#define lcd_draw_filled_rectangle hx8347a_draw_filled_rectangle
#define lcd_display_on hx8347a_display_on
#define lcd_fill  hx8347a_fill
#define lcd_draw_string hx8347a_draw_string

#define LCD_WIDTH HX8347A_LCD_WIDTH
#define LCD_HEIGHT HX8347A_LCD_HEIGHT

#define UNI_COLOR_BLACK  rgb24_to_rgb16(COLOR_BLACK)
#define UNI_COLOR_WHITE  rgb24_to_rgb16(COLOR_WHITE)
#define UNI_COLOR_RED  rgb24_to_rgb16(COLOR_RED)

/* Convert 24-bits color to 16-bits color */
static hx8347a_color_t rgb24_to_rgb16(uint32_t ul_color)
{
	hx8347a_color_t result_color;
	result_color = (((ul_color >> 8) & 0xF800) |
			((ul_color >> 5) & 0x7E0) | ((ul_color >> 3) & 0x1F));
	return result_color;
}

static void lcd_init(void)
{
	struct hx8347a_opt_t g_display_opt;
	
	/* Enable SMC clock */
	pmc_enable_periph_clk(ID_SMC);

	/* Configure SMC interface for Lcd */
	smc_set_setup_timing(SMC, CONF_BOARD_HX8347A_LCD_CS, SMC_SETUP_NWE_SETUP(1)
			| SMC_SETUP_NCS_WR_SETUP(1)
			| SMC_SETUP_NRD_SETUP(9)
			| SMC_SETUP_NCS_RD_SETUP(9));
	smc_set_pulse_timing(SMC, CONF_BOARD_HX8347A_LCD_CS, SMC_PULSE_NWE_PULSE(4)
			| SMC_PULSE_NCS_WR_PULSE(4)
			| SMC_PULSE_NRD_PULSE(36)
			| SMC_PULSE_NCS_RD_PULSE(36));
	smc_set_cycle_timing(SMC, CONF_BOARD_HX8347A_LCD_CS, SMC_CYCLE_NWE_CYCLE(10)
			| SMC_CYCLE_NRD_CYCLE(45));
	smc_set_mode(SMC, CONF_BOARD_HX8347A_LCD_CS, SMC_MODE_READ_MODE
			| SMC_MODE_WRITE_MODE | SMC_MODE_DBW_BIT_16);

	/* Initialize display parameter */
	g_display_opt.ul_width = HX8347A_LCD_WIDTH;
	g_display_opt.ul_height = HX8347A_LCD_HEIGHT;
	g_display_opt.foreground_color = rgb24_to_rgb16(COLOR_BLACK);
	g_display_opt.background_color = rgb24_to_rgb16(COLOR_WHITE);

	/* Initialize hx8347a */
	hx8347a_init(&g_display_opt);
}

#endif /* RESISTIVE_TOUCH_BOARD_H_INCLUDED */
