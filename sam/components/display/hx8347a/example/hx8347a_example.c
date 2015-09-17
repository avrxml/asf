/**
 * \file
 *
 * \brief lcd controller HX8347A example.
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

/**
 * \mainpage lcd controller HX8347A example
 *
 * \section Purpose
 *
 * This example demonstrates how to configure lcd controller HX8347A
 * to control the LCD on the board.
 *
 * \section Requirements
 *
 * This example can be used with SAM3U and SAM3X evaluation kits now.
 *
 * \section Description
 *
 * This example first configures HX8347A for access the LCD controller,
 * then initialize the LCD, finally draw some text, image, basic shapes (line,
 * rectangle, circle) on LCD.
 *
 * \section Usage
 *
 * -# Build the program and download it inside the evaluation board.
 * -# Some text, image and basic shapes should be displayed on the LCD.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

struct hx8347a_opt_t g_hx8347a_display_opt;

/* Convert 24-bits color to 16-bits color */
static hx8347a_color_t rgb24_to_rgb16(uint32_t ul_color)
{
	hx8347a_color_t result_color;
	result_color = (((ul_color >> 8) & 0xF800) |
			((ul_color >> 5) & 0x7E0) | ((ul_color >> 3) & 0x1F));
	return result_color;
}

/**
 *  Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Application entry point for smc_lcd example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	sysclk_init();
	board_init();

	/* Initialize debug console */
	configure_console();

	/* Enable peripheral clock */
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
	g_hx8347a_display_opt.ul_width = HX8347A_LCD_WIDTH;
	g_hx8347a_display_opt.ul_height = HX8347A_LCD_HEIGHT;
	g_hx8347a_display_opt.foreground_color = rgb24_to_rgb16(COLOR_BLACK);
	g_hx8347a_display_opt.background_color = rgb24_to_rgb16(COLOR_WHITE);

	/* Switch off backlight */
	aat31xx_disable_backlight();

	/* Initialize LCD */
	if(hx8347a_init(&g_hx8347a_display_opt)){
		puts("Read HX8347A chip ID error, please check the configuration.\r");
	}

	/* Set backlight level */
	aat31xx_set_backlight(AAT31XX_AVG_BACKLIGHT_LEVEL);

	hx8347a_set_foreground_color(rgb24_to_rgb16(COLOR_WHITE));
	hx8347a_draw_filled_rectangle(0, 0, HX8347A_LCD_WIDTH - 1, HX8347A_LCD_HEIGHT - 1);

	/* Turn on LCD */
	hx8347a_display_on();

	/* Draw text, image and basic shapes on the LCD */
	hx8347a_set_foreground_color(rgb24_to_rgb16(COLOR_BLACK));
	hx8347a_draw_string(10, 20, (uint8_t *)"HX8347A LCD Example");

	hx8347a_set_foreground_color(rgb24_to_rgb16(COLOR_RED));
	hx8347a_draw_circle(60, 120, 40);
	hx8347a_set_foreground_color(rgb24_to_rgb16(COLOR_GREEN));
	hx8347a_draw_circle(120, 120, 40);
	hx8347a_set_foreground_color(rgb24_to_rgb16(COLOR_BLUE));
	hx8347a_draw_circle(180, 120, 40);

	hx8347a_set_foreground_color(rgb24_to_rgb16(COLOR_VIOLET));
	hx8347a_draw_rectangle(40, 200, 200, 260);
	hx8347a_draw_line(40, 200, 200, 260);
	hx8347a_draw_line(40, 260, 200, 200);
	hx8347a_draw_line(40, 230, 200, 230);
	hx8347a_draw_line(120, 200, 120, 260);

	while (1) {
		/* Busy-wait forever. */
	}
}

