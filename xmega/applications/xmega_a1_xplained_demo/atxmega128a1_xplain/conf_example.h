/**
 * \file
 *
 * \brief Board-specific example configuration
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef _CONF_EXAMPLE_H
#define _CONF_EXAMPLE_H

//! \note This is pin 0 on port A.
#define GAMEMODE_SENSE_PIN     IOPORT_CREATE_PIN(PORTA, 0)
//! \note This is pin 1 on port A.
#define GAMEMODE_GROUND_PIN    IOPORT_CREATE_PIN(PORTA, 1)
//! \note This is pin 2 on port A.
#define SOUNDMODE_SENSE_PIN    IOPORT_CREATE_PIN(PORTA, 2)
//! \note This is pin 3 on port A.
#define SOUNDMODE_GROUND_PIN   IOPORT_CREATE_PIN(PORTA, 3)

//! \note This is TC0 on port C.
#define SOUND_TIMER_MODULE     TCC0
//! \note This is overflow on TC0 on port C.
#define SOUND_TIMER_EVENT      EVSYS_CHMUX_TCC0_OVF_gc
//! \note This is TC1 on port C.
#define DEBOUNCE_TIMER_MODULE  TCC1
//! \note This is TC0 on port D.
#define TIMEOUT_TIMER_MODULE   TCD0

//! \name I/O port config for board buttons
//@{
#define BUTTON_IOPORT         PORTF
//@}

static inline uint8_t get_board_button_mask(void)
{
	uint8_t mask;

	mask = ~BUTTON_IOPORT.IN;

	return mask;
}

//! \name I/O port config for all board LEDs
//@{
#define LED_IOPORT            PORTE
//@}

static inline void write_board_led_mask(uint8_t mask)
{
	LED_IOPORT.OUT = ~mask;
}

#endif  // _CONF_EXAMPLE_H
