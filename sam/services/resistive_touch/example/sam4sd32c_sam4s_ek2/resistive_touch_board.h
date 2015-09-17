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

#include "ili9325.h"

#define lcd_set_foreground_color  ili9325_set_foreground_color
#define lcd_draw_filled_rectangle ili9325_draw_filled_rectangle
#define lcd_display_on ili9325_display_on
#define lcd_fill  ili9325_fill
#define lcd_draw_string ili9325_draw_string

#define LCD_WIDTH ILI9325_LCD_WIDTH
#define LCD_HEIGHT ILI9325_LCD_WIDTH

#define UNI_COLOR_BLACK  COLOR_BLACK
#define UNI_COLOR_WHITE  COLOR_WHITE
#define UNI_COLOR_RED  COLOR_RED

static inline void lcd_init(void)
{
	struct ili9325_opt_t g_display_opt;

	/* Enable SMC clock */
	pmc_enable_periph_clk(ID_SMC);

	smc_set_setup_timing(SMC, 1, SMC_SETUP_NWE_SETUP(2)
			| SMC_SETUP_NCS_WR_SETUP(2)
			| SMC_SETUP_NRD_SETUP(2)
			| SMC_SETUP_NCS_RD_SETUP(2));
	smc_set_pulse_timing(SMC, 1, SMC_PULSE_NWE_PULSE(4)
			| SMC_PULSE_NCS_WR_PULSE(4)
			| SMC_PULSE_NRD_PULSE(10)
			| SMC_PULSE_NCS_RD_PULSE(10));
	smc_set_cycle_timing(SMC, 1, SMC_CYCLE_NWE_CYCLE(10)
			| SMC_CYCLE_NRD_CYCLE(22));
	smc_set_mode(SMC, 1, SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE);

	g_display_opt.ul_width = LCD_WIDTH;
	g_display_opt.ul_height = LCD_HEIGHT;
	g_display_opt.foreground_color = COLOR_BLACK;
	g_display_opt.background_color = COLOR_WHITE;

	/* Initialize ili9325 */
	ili9325_init(&g_display_opt);
}

#endif /* RESISTIVE_TOUCH_BOARD_H_INCLUDED */
