/**
 * \file
 *
 * \brief OLED1 Xplained Pro LED and button driver for FreeRTOS demo.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef OLED1_H
#define OLED1_H

#include <board.h>
#include <compiler.h>
#include <port.h>

/**
 * \defgroup oled1_xpro_io_group OLED1 Xplained Pro LED and button driver
 *
 * This is a simple driver for manipulating LEDs and reading buttons on any
 * number of OLED1 Xplained Pro wing boards.
 *
 * The display is not supported by this driver. To operate the display, see
 * \ref asfdoc_common2_gfx_mono.
 *
 * @{
 */

//! \name Constants
//@{

//! Active level for LEDs
#define OLED1_LED_ACTIVE     false

//! Active level for buttons
#define OLED1_BUTTON_ACTIVE  false

//@}


/**
 * \brief Create a driver instance
 *
 * This macro creates a \ref oled1_instance struct with the specified name, and
 * populates it with pin identifiers that correspond to the specified
 * extension header.
 *
 * This macro is created for Xplained Pro-style board definitions as it makes
 * use of the EXT_PIN_x macros.
 *
 * To initialize the hardware, the function \ref oled1_init() must be called on
 * each driver instance.
 *
 * \param name Name to use for driver instance.
 * \param ext_header Extension header to use, on the form EXTn (f.ex. EXT1).
 */
#define OLED1_CREATE_INSTANCE(name, ext_header)  \
	_OLED1_CREATE_INSTANCE(name, ext_header)

#define _OLED1_CREATE_INSTANCE(name, ext_header)  \
	const struct oled1_instance name = {          \
		.led0_pin = ext_header ## _PIN_7,         \
		.led1_pin = ext_header ## _PIN_8,         \
		.led2_pin = ext_header ## _PIN_6,         \
		.button0_pin = ext_header ## _PIN_9,      \
		.button1_pin = ext_header ## _PIN_3,      \
		.button2_pin = ext_header ## _PIN_4,      \
	}


/**
 * \brief Driver instance struct
 *
 * This struct contains identifiers for the LED and button pins. It should be
 * created with the macro \ref OLED1_CREATE_INSTANCE(), and the hardware be
 * initialized with the function \ref oled1_init().
 */
struct oled1_instance {
	//! Pin identifier for first LED
	uint8_t led0_pin;
	//! Pin identifier for second LED
	uint8_t led1_pin;
	//! Pin identifier for third LED
	uint8_t led2_pin;
	//! Pin identifier for first button
	uint8_t button0_pin;
	//! Pin identifier for second button
	uint8_t button1_pin;
	//! Pin identifier for third button
	uint8_t button2_pin;
};

//! IDs for each of the on-board LEDs
enum oled1_led_id {
	OLED1_LED1_ID,
	OLED1_LED2_ID,
	OLED1_LED3_ID,
};

//! IDs for each of the on-board buttons
enum oled1_button_id {
	OLED1_BUTTON1_ID,
	OLED1_BUTTON2_ID,
	OLED1_BUTTON3_ID,
};

/**
 * \brief Initialize hardware for driver instance
 *
 * This function initializes the hardware (pins) that belong to the specified
 * driver instance.
 *
 * \param oled1 Address of driver instance to initialize hardware for.
 */
static inline void oled1_init(const struct oled1_instance *const oled1)
{
	struct port_config pin_conf;

	port_get_config_defaults(&pin_conf);

	port_pin_set_output_level(oled1->led0_pin, !OLED1_LED_ACTIVE);
	port_pin_set_output_level(oled1->led1_pin, !OLED1_LED_ACTIVE);
	port_pin_set_output_level(oled1->led2_pin, !OLED1_LED_ACTIVE);

	pin_conf.direction  = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(oled1->led0_pin, &pin_conf);
	port_pin_set_config(oled1->led1_pin, &pin_conf);
	port_pin_set_config(oled1->led2_pin, &pin_conf);

	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(oled1->button0_pin, &pin_conf);
	port_pin_set_config(oled1->button1_pin, &pin_conf);
	port_pin_set_config(oled1->button2_pin, &pin_conf);
}

//! \name LED manipulation
//@{

/**
 * \internal
 * \brief Get the pin identifier for a given LED number in instance
 *
 * \param oled1 Address of driver instance to fetch pin identifier from.
 * \param led_id ID of the LED to fetch pin identifier for.
 */
static inline uint8_t _oled1_get_led_pin(
		const struct oled1_instance *const oled1,
		const enum oled1_led_id led_id)
{
	uint8_t pin;

	switch (led_id) {
	case OLED1_LED1_ID:
		pin = oled1->led0_pin;
		break;

	case OLED1_LED2_ID:
		pin = oled1->led1_pin;
		break;

	default:
	case OLED1_LED3_ID:
		pin = oled1->led2_pin;
		break;
	}

	return pin;
}

/**
 * \brief Get the state of a LED
 *
 * \param oled1 Address of driver instance to operate on.
 * \param led_id ID of the LED to get state of.
 *
 * \retval true if LED is active, i.e., lit up.
 * \retval false if LED is not active.
 */
static inline bool oled1_get_led_state(const struct oled1_instance *const oled1,
		const enum oled1_led_id led_id)
{
	uint8_t pin = _oled1_get_led_pin(oled1, led_id);
	return port_pin_get_input_level(pin) == OLED1_LED_ACTIVE;
}

/**
 * \brief Set the state of a LED
 *
 * \param oled1 Address of driver instance to operate on.
 * \param led_id ID of the LED to set state of.
 * \param state State to set LED to:
 * \arg true to activate, i.e., light it up.
 * \arg false to deactivate.
 */
static inline void oled1_set_led_state(const struct oled1_instance *const oled1,
		const enum oled1_led_id led_id, const bool state)
{
	uint8_t pin = _oled1_get_led_pin(oled1, led_id);
	port_pin_set_output_level(pin, state == OLED1_LED_ACTIVE);
}

/**
 * \brief Set the state of a LED
 *
 * \param oled1 Address of driver instance to operate on.
 * \param led_id ID of the LED to toggle state of.
 */
static inline void oled1_toggle_led_state(
		const struct oled1_instance *const oled1,
		const enum oled1_led_id led_id)
{
	uint8_t pin = _oled1_get_led_pin(oled1, led_id);
	port_pin_toggle_output_level(pin);
}

//@}

//! \name Button manipulation
//@{

/**
 * \internal
 * \brief Get the pin identifier for a given button number in instance
 *
 * \param oled1_instance Driver instance to fetch pin identifier from.
 * \param led_id ID of the button to fetch pin identifier for.
 *
 * \return Identifier of pin for specified button.
 */
static inline uint8_t _oled1_get_button_pin(
		const struct oled1_instance *const oled1,
		const enum oled1_button_id button_id)
{
	uint8_t pin;

	switch (button_id) {
	case OLED1_BUTTON1_ID:
		pin = oled1->button0_pin;
		break;

	case OLED1_BUTTON2_ID:
		pin = oled1->button1_pin;
		break;

	default:
	case OLED1_BUTTON3_ID:
		pin = oled1->button2_pin;
		break;
	}

	return pin;
}

/**
 * \brief Get the state of a button
 *
 * \param oled1 Address of driver instance to operate on.
 * \param led_id ID of the button to get state of.
 *
 * \retval true if button is active, i.e., pressed.
 * \retval false if button is not active.
 */
static inline bool oled1_get_button_state(
		const struct oled1_instance *const oled1,
		const enum oled1_button_id button_id)
{
	uint8_t pin = _oled1_get_button_pin(oled1, button_id);
	return port_pin_get_input_level(pin) == OLED1_BUTTON_ACTIVE;
}

//@}

/** @} */

#endif // OLED1_H
