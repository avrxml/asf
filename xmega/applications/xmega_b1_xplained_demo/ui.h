/**
 * \file
 *
 * \brief Common User Interface for HID Mouse application
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

#ifndef _UI_H_
#define _UI_H_


#include "c42048a.h"
#include "board.h"

/*! \brief Enumeration of unit symbol to be display on the LCD.
 */
enum ui_num_unit {
	UI_UNIT_NONE,
	UI_UNIT_MILIVOLT,
	UI_UNIT_DEGREE_C
};

/*! \brief Initialize the User interface (LCD, LEDs...)
 */
void ui_init(void);

/*! \brief Called by UDD when a suspend is received
 * Callback running when USB Host set USB line in suspend state
 */
void ui_powerdown(void);

/*! \brief Allows to prepare user interface for DFU session mode.
 */
void ui_dfu_session(void);

/*! \brief Called by UDD when the USB line exit of suspend state
 */
void ui_wakeup(void);

/*! \brief Read buttons value.
 *
 *  \return  read value read from buttons.
 */
uint8_t ui_button(void);

/*! \brief Output a value to the LEDs.
 *
 *  \param led_num The value to be output to the LEDs.
 */
void ui_led(uint8_t led_num);

/*! \brief Display a text string on the LCD.
 *
 *  \note The text string can then be scrolled using the ui_display_text function
 *
 *  \param msg Pointer to the string message to be displayed
 *  \param len Length of this text message
 */
void ui_display_text(uint8_t *msg, uint8_t len);

/*! \brief Scroll the LCD test message.
 *
 *  When called, this function will scroll the LCD text display one character
 *  on the left direction.
 */
void ui_scroll_text(void);

/*! \brief Display an on-board sensor decimal value and its unit on the LCD.
 *
 *  \param val The sensor value
 *  \param unit The unit symbol to be display as icon should be one of:
 *     (UI_UNIT_NONE, UI_UNIT_MILIVOLT, UI_UNIT_DEGREE_C).
 */
static inline void ui_display_sensor(uint16_t val,enum ui_num_unit unit)
{
	c42048a_set_numeric_dec(val);
	c42048a_bar_graph(val>>4);
	c42048a_clear_pixel(ICON_DEGREE_C);
	c42048a_clear_pixel(ICON_MILLI_VOLT);
	switch (unit) {
		case UI_UNIT_MILIVOLT:
			c42048a_set_pixel(ICON_MILLI_VOLT);
			break;
		case UI_UNIT_DEGREE_C:
			c42048a_set_pixel(ICON_DEGREE_C);
			break;
		case  UI_UNIT_NONE:
		default:
			break;
	}
}
/*! \brief Display an error message about incorrect Silicon revision used.
 *
 *  This function is called to signal the Silicon revision used does not match
 *  the firmware revision target.
 */
void ui_si_revision_error(void);

/*! \brief Control the LCD glass contrast.
 *
 *  \param val The contrast value.
 */
static inline void ui_set_contrast(uint8_t val)
{
	lcd_set_contrast(val);
}

/*! \brief Control the LCD backlight level.
 *
 *  \param val The backlight level.
 */
static inline void ui_set_backlight(uint16_t val)
{
	backlight_set_pwm(val);
}

/*! \brief Test if Push Button 0 is pressed.
 *
 *  \return true the button 0 is activated
 */
static inline bool ui_pusb_button_0(void)
{
	return gpio_pin_is_low(GPIO_PUSH_BUTTON_0);
}

/*! \brief Test if Push Button 1 is pressed.
 *
 *  \return true the button 1 is activated
 */
static inline bool ui_pusb_button_1(void)
{
	return gpio_pin_is_low(GPIO_PUSH_BUTTON_1);
}

/*! \brief Test if Push Button 2 is pressed.
 *
 *  \return true the button 2 is activated
 */
static inline bool ui_pusb_button_2(void)
{
	return gpio_pin_is_low(GPIO_PUSH_BUTTON_2);
}

/*! \brief Test if Push Button 3 is pressed.
 *
 *  \return true the button 3 is activated
 */
static inline bool ui_pusb_button_3(void)
{
	return gpio_pin_is_low(GPIO_PUSH_BUTTON_3);
}

//! The maximum text length of LCD text message for scrolling
#define LCD_TEXT_STRING_SIZE    20
//! The maximum text length visible text string on the LCD
#define LCD_TEST_DISPLAY_SIZE   7

#endif // _UI_H_
