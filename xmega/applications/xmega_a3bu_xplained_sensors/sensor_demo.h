/**
 * \file
 *
 * \brief XMEGA-A3BU Xplained Sensor Demo Common Definitions
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
#ifndef _SENSOR_DEMO_H_
#define _SENSOR_DEMO_H_

#include <stdio.h>
#include <asf.h>
#include <led.h>
#include <sensor.h>

/* Hardware Environment Constants */
#define CHARS_PER_LINE      (LCD_WIDTH_PIXELS  / SYSFONT_WIDTH)
#define LINES_PER_SCREEN    (LCD_HEIGHT_PIXELS / SYSFONT_LINESPACING)

#define PROMPT_LED          (LED0)
#define ALL_LEDS            (LED0 | LED1 | LED2 | LED3)

#define REFRESH_PERIOD      (2 /* Hertz */)
#define SW1                 (GPIO_PUSH_BUTTON_1)

/* Convenience Macros */
#define radians(a)          (DEG_TO_RAD * (a))
#define hz_to_ms(hz)        ((unsigned long)((1000 / (hz)) + 0.5))

#define switch_pressed(sw)  (ioport_get_value(sw) == false)

#define clear_screen()  gfx_mono_draw_filled_rect \
		(0, 0, LCD_WIDTH_PIXELS, LCD_HEIGHT_PIXELS, GFX_PIXEL_CLR)

#define screen_border() gfx_mono_draw_rect \
		(0, 0, LCD_WIDTH_PIXELS, LCD_HEIGHT_PIXELS, GFX_PIXEL_SET)

#endif
