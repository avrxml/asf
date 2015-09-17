/**
 * \file
 *
 * \brief HX8347A display controller driver Example 2
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
 * This simple example shows how to use the \ref hx8347a_group for interfacing
 * the HX8347A display controller.
 *
 * \section files Main files:
 * - hx8347a_example2.c: The main example file
 * - conf_hx8347a.h: configuration file for the driver
 * - conf_board.h: configuration for the board
 * - conf_clock.h: configuration for the device clocks
 * - conf_usart_spi.h or conf_spi_master.h depending on which hardware module is
 * used for communicating with the controller
 *
 * \section apiinfo HX8347A low level driver API
 * The HX8347A low level driver API can be found \ref hx8347a_group "here".
 *
 * \section deviceinfo Device Info
 * All devices with some sort of SPI interface, e.g. SPI or USART in SPI mode.
 * \note a Parallel interface is also possible, but is currently not supported
 * by any standard Atmel hardware, hence the driver does not support this.
 *
 * \section exampledescription Description of the example
 * This example show how to use the functions provided by the low level driver
 * to draw very simple graphics to the screen.
 *
 * Five squares of different colors are drawn to the screen to have a test
 * pattern that can be used fto verify correct color operation of the screen.
 *
 * For any advanced drawing of graphics please use the higher level GFX drivers.
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

#define HX8347A_BLACK   HX8347A_COLOR(0  ,   0,   0)
#define HX8347A_RED     HX8347A_COLOR(255,   0,   0)
#define HX8347A_BLUE    HX8347A_COLOR(0  ,   0, 255)
#define HX8347A_GREEN   HX8347A_COLOR(0  , 255,   0)
#define HX8347A_WHITE   HX8347A_COLOR(255, 255, 255)
#define HX8347A_GRAY    HX8347A_COLOR(128, 128, 128)

/** Number of pixels for full screen */
#define TOTAL_PIXELS ((uint32_t)HX8347A_DEFAULT_WIDTH * HX8347A_DEFAULT_HEIGHT)

/**
 * \brief The main application routine
 */
int main(void)
{
	board_init();

	sysclk_init();

	/* Make sure to initialize the display controller */
	hx8347a_init();

	/* Turn on the back light */
	hx8347a_backlight_on();

	/* Blank the screen by drawing a black background. Notice how the
	 * drawing boundaries/limits are set to the entire screen */
	hx8347a_set_top_left_limit(0, 0);
	hx8347a_set_bottom_right_limit(HX8347A_DEFAULT_WIDTH, HX8347A_DEFAULT_HEIGHT);

	hx8347a_duplicate_pixel(HX8347A_BLACK, TOTAL_PIXELS);

	/* Draw five squares in different colors to generate a test pattern for
	 * the screen */

	/* First draw a red square */
	hx8347a_set_top_left_limit(60, 204);
	hx8347a_set_bottom_right_limit(180, 294);

	hx8347a_duplicate_pixel(HX8347A_RED, 11040);

	/* Then draw a green square */
	hx8347a_set_top_left_limit(60, 204);
	hx8347a_set_top_left_limit(60, 114);
	hx8347a_set_bottom_right_limit(180, 194);

	hx8347a_duplicate_pixel(HX8347A_GREEN, 11040);

	/* Then draw a blue square */
	hx8347a_set_top_left_limit(60, 204);
	hx8347a_set_top_left_limit(60, 12);
	hx8347a_set_bottom_right_limit(180, 102);

	hx8347a_duplicate_pixel(HX8347A_BLUE, 11040);

	/* Then draw a white square */
	hx8347a_set_top_left_limit(60, 204);
	hx8347a_set_top_left_limit(15, 12);
	hx8347a_set_bottom_right_limit(45, 294);

	hx8347a_duplicate_pixel(HX8347A_WHITE, 11040);

	/* Then draw a gray square */
	hx8347a_set_top_left_limit(60, 204);
	hx8347a_set_top_left_limit(195, 12);
	hx8347a_set_bottom_right_limit(225, 294);

	hx8347a_duplicate_pixel(HX8347A_GRAY, 11040);

	/* Application end */
	while (true) {
		/* Do nothing */
	}
}
