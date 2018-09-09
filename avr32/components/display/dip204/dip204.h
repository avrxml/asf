/*****************************************************************************
 *
 * \file
 *
 * \brief LCD DIP204 driver for EVK1100 board.
 *
 * This file defines a useful set of functions for the DIP204 interface on
 * AVR32 devices.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
