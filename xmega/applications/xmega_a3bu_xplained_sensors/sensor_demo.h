/**
 * \file
 *
 * \brief XMEGA-A3BU Xplained Sensor Demo Common Definitions
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
