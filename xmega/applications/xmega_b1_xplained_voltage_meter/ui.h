/**
 * \file
 *
 * \brief Common User Interface for HID Mouse application
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
#define LCD_TEXT_DISPLAY_SIZE   7

#define LCD_TEXT_MEMORY (LCD_TEXT_STRING_SIZE + 2 * LCD_TEXT_DISPLAY_SIZE)

#endif // _UI_H_
