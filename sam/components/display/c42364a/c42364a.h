/**
 * \file
 *
 * \brief Management of C42364A LCD Glass component.
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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

#ifndef C42364A_H_INCLUDED
#define C42364A_H_INCLUDED


#include "compiler.h"
#include "lcdca.h"
#include "conf_c42364a.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup c42364a_display_group Atmel YMCC42364AAANDCL LCD Glass component
 *
 * This is a driver for Atmel YMCC42364AAANDCL LCD component.
 * This component is the custom LCD used for SAM4L-EK.
 * This driver provides functions for initialization and control of the
 * LCD segments.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref group_sam_drivers_lcdca for Liquid Crystal Display (LCD) module.
 * @{
 */

/**
 * \name C42364A LCD Glass Common and Segment number
 * @{
 */
#define C42364A_NB_OF_COM 4
#define C42364A_NB_OF_SEG 40
/** @} */

/**
 * \name C42364A LCD Glass Connection Initialization
 * @{
 */
#define C42364A_PORT_MASK C42364A_NB_OF_SEG
#define C42364A_LCD_DUTY  (C42364A_NB_OF_COM % 4)
/** @} */

/**
 * \name Digit Decoder
 * @{
 */
/** 7-segment with 4 commons terminals */
#define C42364A_FIRST_7SEG_4C 10
#define C42364A_WIDTH_7SEG_4C 4

/** 14-segment with 4 commons terminals */
#define C42364A_FIRST_14SEG_4C 36
#define C42364A_WIDTH_14SEG_4C 7
/** @} */

/**
 * \name Icon with selected segments blinking feature
 * @{
 */
#define C42364A_ICON_USB      1,0
#define C42364A_ICON_COLON    0,0
#define C42364A_ICON_WLESS    2,0
#define C42364A_ICON_AUDIO    3,0
#define C42364A_ICON_BAT      0,1
#define C42364A_ICON_ERROR    1,1
#define C42364A_ICON_CHINESE  2,1
#define C42364A_ICON_ARM      3,1
/** @} */

/**
 * \name Icon without selected segments blinking feature
 * @{
 */
#define C42364A_ICON_MINUS          0,37
#define C42364A_ICON_MINUS_SEG1     3,30
#define C42364A_ICON_MINUS_SEG2     0,33
#define C42364A_ICON_DOT_1          0,29
#define C42364A_ICON_DOT_2          3,10
#define C42364A_ICON_DOT_3          3,8
#define C42364A_ICON_DOT_4          3,6
#define C42364A_ICON_BAT_LEVEL_1    0,21
#define C42364A_ICON_BAT_LEVEL_2    0,17
#define C42364A_ICON_BAT_LEVEL_3    0,25
#define C42364A_ICON_AUDIO_PAUSE    3,38
#define C42364A_ICON_AUDIO_PLAY     3,34
#define C42364A_ICON_AM             3,4
#define C42364A_ICON_PM             3,26
#define C42364A_ICON_DEGREE_C       3,22
#define C42364A_ICON_DEGREE_F       3,18
#define C42364A_ICON_VOLT           3,14
#define C42364A_ICON_MILLI_VOLT     0,13
/** @} */

/**
 * \name Circular Animation Shift Direction
 * @{
 */
/** Right Direction. */
#define C42364A_CSR_RIGHT       LCDCA_CSR_RIGHT
/** Left Direction. */
#define C42364A_CSR_LEFT        LCDCA_CSR_LEFT
/** @} */

/**
 * \name Value for battery Icon setting
 * @{
 */
enum c42364a_battery_value {
	C42364A_BATTERY_NONE = 0,
	C42364A_BATTERY_ONE,
	C42364A_BATTERY_TWO,
	C42364A_BATTERY_THREE,
};
/** @} */

/**
 * \name Macro Functions
 * @{
 */

/**
 * \brief Write string to c42048a lcd glass alphanumeric field.
 *
 * This function will write the input string to the alphanumeric field of the
 * lcd glass.
 *
 * \param data Pointer to the input string(max length is 7)
 */
static inline void c42364a_write_alphanum_packet(const uint8_t *data)
{
	lcdca_write_packet(LCDCA_TDG_14SEG4COM, C42364A_FIRST_14SEG_4C, data,
			C42364A_WIDTH_14SEG_4C, LCDCA_CMCFG_DREV_LEFT);
}

/**
 * \brief Write string to c42048a lcd glass numeric field.
 *
 * This function will write the input string to the numeric field of the
 * lcd glass.
 *
 * \param data Pointer to the input string(max length is 4)
 */
static inline void c42364a_write_num_packet(const uint8_t *data)
{
	lcdca_write_packet(LCDCA_TDG_7SEG4COM, C42364A_FIRST_7SEG_4C, data,
			C42364A_WIDTH_7SEG_4C, LCDCA_CMCFG_DREV_LEFT);
}

/**
 * \brief Show text on C42364A LCD glass alphanumeric field.
 *
 * This function will show text on the alphanumeric field of the LCD glass.
 *
 * \param data Pointer to the input string(max length is 7)
 */
static inline void c42364a_show_text(const uint8_t *data)
{
	lcdca_write_packet(LCDCA_TDG_14SEG4COM, C42364A_FIRST_14SEG_4C, data,
			C42364A_WIDTH_14SEG_4C, LCDCA_CMCFG_DREV_LEFT);
}

/**
 * \brief Clear a specific icon on the LCD glass.
 *
 * This function will clear a specific icon.
 *
 * \param icon_com  Pixel coordinate - COMx - of the icon.
 * \param icon_seg  Pixel coordinate - SEGy - of the icon.
 *
 * \note Use the icon define in header file.
 */
static inline void c42364a_clear_icon(uint8_t icon_com, uint8_t icon_seg)
{
	lcdca_clear_pixel(icon_com, icon_seg);
}

/**
 * \brief Show a specific icon on the LCD glass.
 *
 * This function will show a specific icon.
 *
 * \param icon_com  Pixel coordinate - COMx - of the icon.
 * \param icon_seg  Pixel coordinate - SEGy - of the icon.
 *
 * \note Use the icon define in header file.
 */
static inline void c42364a_show_icon(uint8_t icon_com, uint8_t icon_seg)
{
	if (icon_com < 2) {
		lcdca_clear_blink_pixel(icon_com, icon_seg);
	}

	lcdca_set_pixel(icon_com, icon_seg);
}

/**
 * \brief Stop blink a specific icon on the LCD glass.
 *
 * This function will stop blink a specific icon.
 *
 * \param icon_com  Pixel coordinate - COMx - of the icon.
 * \param icon_seg  Pixel coordinate - SEGy - of the icon.
 *
 * \note Use the icon define in header file(with selected segments
 * blinking feature).
 */
static inline void c42364a_blink_icon_stop(uint8_t icon_com, uint8_t icon_seg)
{
	lcdca_clear_blink_pixel(icon_com, icon_seg);
}

/**
 * \brief Disable all Blink.
 *
 * This function will disable all Blink content.
 *
 */
static inline void c42364a_blink_disable(void)
{
	lcdca_blink_disable();
}

/**
 * \brief Set the C42364A LCD glass contrast.
 *
 *  This function allows to adjust the contrast of the C42364A LCD glass.
 *
 *  \param contrast  -32 <= signed contrast value <= 31.
 */
static inline void c42364a_set_contrast(int8_t contrast)
{
	lcdca_set_contrast(contrast);
}

/**
 * \brief Stop circular animation.
 *
 *  This function stop the circular animation.
 */
static inline void c42364a_circular_animation_stop(void)
{
	lcdca_circular_shift_disable();
}

/**
 * \brief Scrolling stop.
 *
 * This function stop the text scrolling.
 */
static inline void c42364a_text_scrolling_stop(void)
{
	lcdca_automated_char_stop();
}

/** @} */

/**
 * \name Function Prototypes
 * @{
 */

/**
 * \brief Initialize the C42364A LCD Glass component.
 *
 * This function initializes the LCD driver to control the LCD glass.
 * It perform LCD module intialization according to the C42364A characteristics.
 *
 */
void c42364a_init(void);

/**
 * \brief Show all content of the LCD glass.
 *
 * This function sets all pixels and areas of the LCD glass C42364A.
 *
 */
void c42364a_show_all(void);

/**
 * \brief Clear all content of the LCD glass.
 *
 * This function clears all pixels and areas of the LCD glass C42364A.
 *
 */
void c42364a_clear_all(void);

/**
 * \brief Clear C42364A LCD glass alphanumeric field.
 *
 * This function will clear the alphanumeric field of the LCD glass.
 */
void c42364a_clear_text(void);

/**
 * \brief Clear C42364A LCD glass numeric field and the three
 * C42364A_ICON_MINUS*** icons.
 *
 * This function will clear the numeric field of the LCD glass.
 */
void c42364a_clear_numeric_dec(void);

/**
 * \brief Blink a specific icon on the LCD glass.
 *
 * This function will blink a specific icon.
 *
 * \param icon_com  Pixel coordinate - COMx - of the icon.
 * \param icon_seg  Pixel coordinate - SEGy - of the icon.
 *
 * \note Use the icon define in header file(with selected segments
 * blinking feature).
 */
void c42364a_blink_icon_start(uint8_t icon_com, uint8_t icon_seg);

/**
 * \brief Blink the current screen content.
 *
 * This function will make the current screen blink.
 *
 */
void c42364a_blink_screen(void);

/**
 * \brief Start circular animation.
 *
 * This function start the circular animation.
 *
 * \param csr_dir Direction.
 * \param size Defines the size of the circular animation icon.
 * \param data Circular Shift Register Value.
 */
void c42364a_circular_animation_start(uint8_t csr_dir,
		uint8_t size, uint8_t data);

/**
 * \brief Show battery condition by the battery icons on the LCD glass.
 *
 * This function allows to Show battery condition by the battery icons
 * on the LCD glass..
 *
 * \param val The 0 to 3 value which show the battery condition.
 */
void c42364a_show_battery(enum c42364a_battery_value val);

/**
 * \brief Show a decimal numeric value to LCD glass.
 *
 * This function displays an int32 value to the LCD numeric field of the glass.
 *
 * \param value The int32_t value to be displayed
 *
 * \note The value range is [-19999,19999].
 */
void c42364a_show_numeric_dec(int32_t value);

/**
 * \brief Scrolling start.
 *
 * This function start the text scrolling.
 *
 * \param data Data string buffer.
 * \param length Data string length.
 */
void c42364a_text_scrolling_start(const uint8_t *data, uint32_t length);

/** @} */


/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page sam_component_c42364a_quick_start Quick Start Guide for the C42364A
 * LCD Glass component.
 *
 * This is the quick start guide for the \ref c42364a_display_group, with
 * step-by-step instructions on how to configure and use the driver for
 * a specific use case.The code examples can be copied into e.g the main
 * application loop or any other function that will need to control the
 * C42364A LCD Glass component module.
 *
 * \section c42364a_qs_use_cases Use cases
 * - \ref c42364a_basic
 *
 * \section c42364a_basic C42364A LCD Glass basic usage
 *
 * This use case will demonstrate how to initialize the C42364A LCD Glass
 * module.
 *
 *
 * \section c42364a_basic_setup Setup steps
 *
 * \subsection c42364a_basic_prereq Prerequisites
 *
 * This module requires the following driver
 * - \ref group_sam_drivers_lcdca
 *
 * \subsection c42364a_basic_setup_code
 *
 * Add this to the main loop or a setup function:
 * \code
 * c42364a_init();
 * \endcode
 *
 * \subsection c42364a_basic_setup_workflow
 *
 * -# Initialize the c42364a module
 *  - \code c42364a_init(); \endcode
 *
 * \section c42364a_basic_usage Usage steps
 *
 * \subsection c42364a_basic_usage_code
 *
 * We can set all the contents by
 * \code
 * c42364a_set_all();;
 * \endcode
 * Or we can clear all the contents by
 * \code
 * c42364a_clear_all();;
 * \endcode
 *
 * We can use the basic show feature by
 * \code
 * int32_t value = -12345;
 * c42364a_show_icon(C42364A_ICON_ARM);
 * c42364a_show_numeric_dec(value);
 * c42364a_show_text("Welcome");
 * c42364a_show_battery(c42364a_battery_two);
 * \endcode
 *
 * We can use the blink feature by
 * \code
 * c42364a_blink_icon_start(C42364A_ICON_COLON);;
 * \endcode
 *
 * We can start/stop the circular animation feature by
 * \code
 * c42364a_circular_animation_start(C42364A_CSR_RIGHT, 7, 0x03);
 *
 * c42364a_circular_animation_stop();
 * \endcode
 *
 * We can start/stop the text scrolling feature by
 * \code
 * uint8_t const scrolling_str[] = "C42364A Example  ";
 *
 * c42364a_text_scrolling_start(scrolling_str,
 *         strlen((char const *)scrolling_str));
 *
 * c42364a_text_scrolling_stop();
 * \endcode
 */

#endif  /* C42364A_H_INCLUDED */
