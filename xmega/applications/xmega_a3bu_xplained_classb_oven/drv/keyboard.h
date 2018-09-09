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
