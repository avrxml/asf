/**
 * \file
 *
 * \brief Keyboard driver
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

#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "gpio.h"

//! \name Keyboard definitions
//@{
#define KEYBOARD_NO_KEY 0

#define KEYBOARD_UP 38       // UP-ARROW
#define KEYBOARD_UP_MASK 2

#define KEYBOARD_DOWN 40     // DOWN ARROW
#define KEYBOARD_DOWN_MASK 4

#define KEYBOARD_ENTER 13    // ENTER
#define KEYBOARD_ENTER_MASK 1

#define KEYBOARD_BACK 8      // BACKSPACE
#define KEYBOARD_BACK_MASK 8
//@}

typedef uint8_t keycode_t;

//! Definition of the different keyboard event types
enum keyboard_event_type {
	KEYBOARD_NO_EVENT,
	KEYBOARD_PRESS,
	KEYBOARD_RELEASE,
	/* KEYBOARD_NEAR // for a possible proximity state */
};

//! Struct to hold a keycode and the related event
struct keyboard_event {
	keycode_t keycode;
	enum keyboard_event_type type;
};

void keyboard_get_key_state(struct keyboard_event *key);

#endif /* KEYBOARD_H_INCLUDED */
