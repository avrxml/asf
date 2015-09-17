/**
 * \file
 *
 * \brief ILI9341 display controller component driver Example
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
 * This simple example shows how to use the \ref ili9341_group for interfacing
 * the ILI9341 display controller.
 *
 * \section files Main files:
 * - ili9341_example.c: The main example file
 * - conf_ili9341.h: configuration file for the component driver
 * - conf_board.h: configuration for the board
 * - conf_clock.h: configuration for the device clocks
 * - conf_usart_spi.h or conf_spi_master.h depending on which hardware module
 * is used for communicating with the controller
 *
 * \section apiinfo ILI9341 low level component driver API
 * The ILI9341 low level component driver API can be found
 * \ref ili9341_group "here".
 *
 * \section deviceinfo Device Info
 * All devices with some sort of SPI interface, e.g. SPI or USART in SPI mode.
 * \note a Parallel interface is also possible, but is currently not supported
 * by any standard Atmel hardware, hence the component driver does not support
 * this.
 *
 * \section exampledescription Description of the example
 * This example show how to use the functions provided by the low level
 * component driver to draw very simple graphics to the screen.
 *
 * First it uses the function \ref ili9341_duplicate_pixel to draw a color to
 * the screen 20 times. Then it uses \ref ili9341_copy_pixels_to_screen to copy
 * a 40 x 40 single color pixmap to the screen at different locations.
 *
 * For any advanced drawing of graphics please use the higher level GFX
 * service.
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
#include <asf.h>

/** Table of primary and secondary display pixel colors */
const ili9341_color_t color_table[] = {
	ILI9341_COLOR(0, 0, 255), ILI9341_COLOR(0, 255, 0),
	ILI9341_COLOR(255, 0, 0), ILI9341_COLOR(255, 255, 0),
	ILI9341_COLOR(255, 0, 255), ILI9341_COLOR(0, 255, 255)
};

/** Number of colors stored in the \ref color_table array */
#define COLOR_TABLE_SIZE          (sizeof(color_table) / sizeof(color_table[0]))

/** Display background color when clearing the display */
#define BG_COLOR                  ILI9341_COLOR(0, 0, 0)

/** Number of pixels for full screen */
#define TOTAL_PIXELS ((uint32_t)ILI9341_DEFAULT_WIDTH * ILI9341_DEFAULT_HEIGHT)

/** Create an array to hold a 40 x 40 bitmap */
ili9341_color_t bitmap[1600];

/** Pointer to the location in the array */
uint16_t bitmap_ptr = 0;

/**
 * \brief Main application
 */
int main(void)
{
	uint16_t x, y;
	uint8_t color_index;

	board_init();
	sysclk_init();

	/* Make sure to initialize the display controller */
	ili9341_init();

	/* Turn on the back light */
	ili9341_backlight_on();

	ili9341_set_top_left_limit(0, 0);
	ili9341_set_bottom_right_limit(ILI9341_DEFAULT_WIDTH,
			ILI9341_DEFAULT_HEIGHT);

	/* Draw colors to the screen, filling it */
	for (color_index = 0; color_index < COLOR_TABLE_SIZE; color_index++) {
		ili9341_set_top_left_limit(0, 0);
		ili9341_set_bottom_right_limit(ILI9341_DEFAULT_WIDTH,
				ILI9341_DEFAULT_HEIGHT);

		ili9341_duplicate_pixel(color_table[color_index],
				TOTAL_PIXELS);
	}

	/* Draw bands of colors to the screen, showing all colors at once */
	for (uint8_t step = 1; step < 10; step++) {
		for (y = 0; y < ILI9341_DEFAULT_HEIGHT / step; y++) {
			ili9341_set_top_left_limit(0, y * step);
			ili9341_duplicate_pixel(
					color_table[y % COLOR_TABLE_SIZE],
					(uint32_t)ILI9341_DEFAULT_WIDTH
					* step);
		}
	}

	/* Clear display to black */
	ili9341_set_top_left_limit(0, 0);
	ili9341_set_bottom_right_limit(ILI9341_DEFAULT_WIDTH,
			ILI9341_DEFAULT_HEIGHT);
	ili9341_duplicate_pixel(BG_COLOR, TOTAL_PIXELS);

	while (true) {
		/* Load the bitmap with a new color */
		for (bitmap_ptr = 0; bitmap_ptr < 1600; bitmap_ptr++) {
			bitmap[bitmap_ptr]
				= color_table[color_index % COLOR_TABLE_SIZE];
		}

		color_index++;

		for (x = 0; x < ILI9341_DEFAULT_WIDTH; x = x + 40) {
			for (y = 0; y < ILI9341_DEFAULT_HEIGHT; y = y + 5) {
				/* Clear the last drawn area */
				ili9341_set_top_left_limit(x, y - 5);
				ili9341_set_bottom_right_limit(x + 40, y + 5);
				ili9341_duplicate_pixel(BG_COLOR, 40UL * 10UL);

				/* Draw the moved pixmap */
				ili9341_set_top_left_limit(x, y);
				ili9341_set_bottom_right_limit(x + 40, y + 40);
				ili9341_copy_pixels_to_screen(bitmap, 40UL * 40UL);
			}
		}
	}
}
