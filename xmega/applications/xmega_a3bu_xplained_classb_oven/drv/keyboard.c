/**
 * \file
 *
 * \brief Keyboard driver
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
