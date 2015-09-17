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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/**
 * \name Keyboard definitions
 * @{
 */
#define KEYBOARD_NO_KEY     0

/* UP-ARROW */
#define KEYBOARD_UP         38
#define KEYBOARD_UP_MASK    2

/* DOWN-ARROW */
#define KEYBOARD_DOWN       40
#define KEYBOARD_DOWN_MASK  4

/* ENTER */
#define KEYBOARD_ENTER      13
#define KEYBOARD_ENTER_MASK 1

/* BACKSPACE */
#define KEYBOARD_BACK       8
#define KEYBOARD_BACK_MASK  8
/** @} */

/** Type to contain keycode  */
typedef uint8_t keycode_t;

/** Keyboard event types */
enum keyboard_event_type {
	KEYBOARD_NO_EVENT,
	KEYBOARD_PRESS,
	KEYBOARD_RELEASE,
};

/** Container for keycode and related event */
struct keyboard_event {
	keycode_t keycode;
	enum keyboard_event_type type;
};

void keyboard_get_key_state(struct keyboard_event *key);

#endif /* KEYBOARD_H */
