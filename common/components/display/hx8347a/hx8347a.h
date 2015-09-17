/**
 * \file
 *
 * \brief HX8347A display controller driver
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
#ifndef HX8347A_H_INCLUDED
#define HX8347A_H_INCLUDED

#include <gpio.h>
#include <compiler.h>

/* Controller and interface configuration file */
#include "conf_hx8347a.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup hx8347a_group HX8347A Display Controller Low Level driver
 *
 * This is the low level driver for the HX8347A display controller. It provides
 * basic functions for initializing, writing and reading the controller. In
 * addition to hardware control and use of the LCD controller internal
 * functions.
 *
 * \warning This driver is not reentrant and can not be used in interrupt
 * service routines without extra care
 *
 * Before writing data to the display call \ref hx8347a_init() which will set up
 * the physical interface and the display. A configuration file
 * \ref conf_hx8347a.h is needed to define which interface to use, pin
 * connections and communication speed.
 *
 * An example \ref conf_hx8347a.h file for the XMEGA could look like:
 * \code
	 #define CONF_HX8347A_USART_SPI     &USARTD1
	 #define CONF_HX8347A_CLOCK_SPEED   500000UL

	 #define CONF_HX8347A_CS_PIN        IOPORT_CREATE_PIN(PORTD, 4)
	 #define CONF_HX8347A_TE_PIN        IOPORT_CREATE_PIN(PORTD, 2)
	 #define CONF_HX8347A_BACKLIGHT_PIN IOPORT_CREATE_PIN(PORTD, 0)
	 #define CONF_HX8347A_RESET_PIN     IOPORT_CREATE_PIN(PORTD, 1)
\endcode
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref gpio_group for IO port control.
 * - \ref sysclk_group for clock speed and functions.
 * - \ref usart_spi_group or \ref spi_group for communication with the
 * controller.
 * @{
 */

#if defined(CONF_HX8347A_USART_SPI)
#  include <usart_spi.h>
#elif defined(CONF_HX8347A_SPI)
#  include <spi_master.h>
#else
#error "Interface not supported by the driver"
#endif

/** This macro generates a 16-bit native color for the display from a
 *  24-bit RGB value.
 */
#define HX8347A_COLOR(r, g, b)\
	Swap16((((uint16_t)b >> 3) << 11) |\
		   (((uint16_t)g >> 2) << 5) |\
		   (((uint16_t)r >> 3) << 0))

typedef uint16_t hx8347a_color_t;
typedef int16_t hx8347a_coord_t;

/** Bit mask for flipping X for hx8347a_set_orientation() */
#define HX8347A_FLIP_X 1
/** Bit mask for flipping Y for hx8347a_set_orientation() */
#define HX8347A_FLIP_Y 2
/** Bit mask for swapping X and Y for hx8347a_set_orientation() */
#define HX8347A_SWITCH_XY 4

/** Height of display using default orientation */
#define HX8347A_DEFAULT_HEIGHT   320

/** Width of display using default orientation */
#define HX8347A_DEFAULT_WIDTH    240

/** Height of display using swapped X/Y orientation */
#define HX8347A_SWITCH_XY_HEIGHT 240

/** Width of display using swapped X/Y orientation */
#define HX8347A_SWITCH_XY_WIDTH  320

/**
 * \name Register manipulation functions
 * @{
 */
void hx8347a_write_register(uint8_t address, uint8_t value);

uint8_t hx8347a_read_register(uint8_t address);

void hx8347a_clear_register(uint8_t address, uint8_t bitmask);

void hx8347a_set_register(uint8_t address, uint8_t bitmask);

/** @} */

/**
 * \name Controller primitive graphical functions
 * @{
 */
hx8347a_color_t hx8347a_read_gram(void);

void hx8347a_write_gram(hx8347a_color_t color);

void hx8347a_set_top_left_limit(hx8347a_coord_t x, hx8347a_coord_t y);

void hx8347a_set_bottom_right_limit(hx8347a_coord_t x, hx8347a_coord_t y);

void hx8347a_set_limits(hx8347a_coord_t start_x, hx8347a_coord_t start_y,
		hx8347a_coord_t end_x, hx8347a_coord_t end_y);
		
void hx8347a_set_orientation(uint8_t flags);

void hx8347a_copy_pixels_to_screen(const hx8347a_color_t *pixels, uint32_t count);

void hx8347a_copy_progmem_pixels_to_screen(
		hx8347a_color_t PROGMEM_PTR_T pixels, uint32_t count);

void hx8347a_copy_pixels_from_screen(hx8347a_color_t *pixels, uint32_t count);

void hx8347a_duplicate_pixel(const hx8347a_color_t color, uint32_t count);

/** @} */

/**
 * \name Controller and display initialization and management
 * @{
 */

void hx8347a_init(void);

void hx8347a_exit_standby(void);

void hx8347a_reset_display(void);

/**
 * \brief Function to turn on the display back light
 *
 * Use this function to simply set the pin controlling the back light high to
 * turn on the back light.
 *
 * \note It is up to the top application if other means of controlling this pin
 * should be used, e.g. a PWM signal to be able to dim the display.
 *
 */
__always_inline static void hx8347a_backlight_on(void)
{
	gpio_set_pin_high(CONF_HX8347A_BACKLIGHT_PIN);
}

/**
 * \brief Function to turn off the display back light
 *
 * Use this function to simply set the pin controlling the back light low to
 * turn off the back light.
 *
 */
__always_inline static void hx8347a_backlight_off(void)
{
	gpio_set_pin_low(CONF_HX8347A_BACKLIGHT_PIN);
}

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HX8347A_H_INCLUDED */
