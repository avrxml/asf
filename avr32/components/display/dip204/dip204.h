/*****************************************************************************
 *
 * \file
 *
 * \brief LCD DIP204 driver for EVK1100 board.
 *
 * This file defines a useful set of functions for the DIP204 interface on
 * AVR32 devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _AVR32_DIP204_H_
#define _AVR32_DIP204_H_

/**
 * \defgroup group_avr32_components_display_dip204 DISPLAY - LCD Display - DIP204B-4ORT01
 *
 * Driver for Ortek DIP204B-4ORT01 LCD module (4*20 characters).
 * Provides functions for initialization, controlling back-light, and writing characters and strings to the display.
 *
 * \{
 */

#include "compiler.h"


/*! type for Backlight options : use PWM or IO to drive the backlight
 *
 */
typedef enum {
  backlight_IO = 0,
  backlight_PWM
}backlight_options;

/*! type for Backlight power : increase or decrease the backlight
 *
 */
typedef enum {
  backlight_power_increase = 0,
  backlight_power_decrease
}backlight_power;


/*! Initialize the LCD
 *         (need void delay_ms(unsigned short time_ms) function to perform active wait)
 *
 * \param option backlight_IO if no PWM needed, backlight_PWM if PWM needed...
 * \param backlight_on Whether to start with backlight on or off.
 */
extern void dip204_init(backlight_options option, bool backlight_on);

/*! Change the backlight power
 *
 * \param power increase or decrease the backlight...
 */
extern void dip204_set_backlight(backlight_power power);

/*! Show blinking cursor
 *
 */
extern void dip204_show_cursor(void);

/*! Hide cursor
 *
 */
extern void dip204_hide_cursor(void);

/*! Write a byte at current position
 *
 * \param  data   Input. data to display
 *
 */
extern void dip204_write_data(unsigned char data);

/*! Read data at current position
 *
 * \param  data   Output. data read at current position
 *
 */
extern void dip204_read_data(unsigned char *data);

/*! Create a new ASCII character
 *
 * \param  ascii_code   Input. ascii code of the new character. Must fit in the range [0; 7].
 * \param  data         Input. pixel map of the character. It is composed of 5 columns and 8 lines.
 *
 */
extern void dip204_create_char(char ascii_code, const unsigned char data[8]);

/*! Set cursor to given position
 *
 * \param  column   Input. Column where to set cursor (from 1 to 20).
 * \param  line     Input. Line where to set cursor (from 1 to 4).
 *
 */
extern void dip204_set_cursor_position(unsigned char column, unsigned char line);

/*! Clear the LCD screen
 *         (need void delay_ms(unsigned short time_ms) function to perform active wait)
 *
 */
extern void dip204_clear_display(void);

/*! Write a string
 *
 * \param  string   Input. null terminated string to display
 *
 */
extern void dip204_write_string(const char *string);

/*! Write a formatted string
 *
 * \param  format   Input. Formatted null terminated string to display
 *
 */
extern void dip204_printf_string(const char *format, ...);

/**
 * \}
 */

#endif // _AVR32_DIP204_H_
