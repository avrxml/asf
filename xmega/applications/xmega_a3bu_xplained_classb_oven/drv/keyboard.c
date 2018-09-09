/**
 * \file
 *
 * \brief Keyboard driver
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <board.h>
#include <gpio.h>
#include "touch.h"
#include "keyboard.h"

/**
 * \brief Check the status of the keyboard
 *
 * This function checks what keys have been pressed or released, updating their
 * status and saving the current key change (if any) in the specified buffer.
 *
 * \param keybuffer Pointer to keyboard_event struct to store status.
 */
void keyboard_get_key_state(struct keyboard_event *keybuffer)
{
	/* Variable to hold the key mask */
	static uint8_t key_state;

	/* Handle SW0 */
	if ((gpio_pin_is_low(GPIO_PUSH_BUTTON_0))
			&& !(key_state & KEYBOARD_BACK_MASK)) {
		/* Key pressed */
		key_state         |= KEYBOARD_BACK_MASK;
		keybuffer->keycode = KEYBOARD_BACK;
		keybuffer->type    = KEYBOARD_PRESS;
	} else if ((gpio_pin_is_high(GPIO_PUSH_BUTTON_0))
			&& (key_state & KEYBOARD_BACK_MASK)) {
		/* Key released */
		key_state         &= ~KEYBOARD_BACK_MASK;
		keybuffer->keycode = KEYBOARD_BACK;
		keybuffer->type    = KEYBOARD_RELEASE;
	/* Handle SW1 */
	} else if ((gpio_pin_is_low(GPIO_PUSH_BUTTON_1))
			&& !(key_state & KEYBOARD_UP_MASK)) {
		/* Key pressed */
		key_state         |= KEYBOARD_UP_MASK;
		keybuffer->keycode = KEYBOARD_UP;
		keybuffer->type    = KEYBOARD_PRESS;
	} else if ((gpio_pin_is_high(GPIO_PUSH_BUTTON_1))
			&& (key_state & KEYBOARD_UP_MASK)) {
		/* Key released */
		key_state         &= ~KEYBOARD_UP_MASK;
		keybuffer->keycode = KEYBOARD_UP;
		keybuffer->type    = KEYBOARD_RELEASE;
	/* Handle SW2 */
	} else if ((gpio_pin_is_low(GPIO_PUSH_BUTTON_2))
			&& !(key_state & KEYBOARD_DOWN_MASK)) {
		/* Key pressed */
		key_state         |= KEYBOARD_DOWN_MASK;
		keybuffer->keycode = KEYBOARD_DOWN;
		keybuffer->type    = KEYBOARD_PRESS;
	} else if ((gpio_pin_is_high(GPIO_PUSH_BUTTON_2))
			&& (key_state & KEYBOARD_DOWN_MASK)) {
		/* Key released */
		key_state         &= ~KEYBOARD_DOWN_MASK;
		keybuffer->keycode = KEYBOARD_DOWN;
		keybuffer->type    = KEYBOARD_RELEASE;
	/* Handle QTB0 */
	} else if ((touch_key_is_pressed())
			&& !(key_state & KEYBOARD_ENTER_MASK)) {
		/* Touch key pressed */
		key_state         |= KEYBOARD_ENTER_MASK;
		keybuffer->keycode = KEYBOARD_ENTER;
		keybuffer->type    = KEYBOARD_PRESS;
	} else if ((!touch_key_is_pressed())
			&& (key_state & KEYBOARD_ENTER_MASK)) {
		/* Touch key released */
		key_state         &= ~KEYBOARD_ENTER_MASK;
		keybuffer->keycode = KEYBOARD_ENTER;
		keybuffer->type    = KEYBOARD_RELEASE;
	/* No keys pressed */
	} else {
		keybuffer->keycode = KEYBOARD_NO_KEY;
		keybuffer->type    = KEYBOARD_NO_EVENT;
	}
}
