/**
 * \file
 *
 * \brief SAM4L User Board init.
 *
 * This file contains board initialization function.
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
#include "compiler.h"
#include "conf_board.h"
#include "ioport.h"
#include "board.h"

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

void board_init(void)
{
	// Initialize IOPORTs
	ioport_init();

	// Put all pins to default state (input & pull-up)
	uint32_t pin;

	for (pin = PIN_PA00; pin <= PIN_PC31; pin ++) {
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);
		ioport_set_pin_mode(pin, IOPORT_MODE_PULLUP);
	}

#if defined (CONF_BOARD_COM_PORT)
	ioport_set_pin_peripheral_mode(COM_PORT_RX_PIN, COM_PORT_RX_MUX);
	ioport_set_pin_peripheral_mode(COM_PORT_TX_PIN, COM_PORT_TX_MUX);
#endif

#ifdef CONF_BOARD_GLOC_LUT0_PORT
	ioport_set_pin_peripheral_mode(PIN_PA20D_GLOC_IN0, MUX_PA20D_GLOC_IN0);
	ioport_set_pin_peripheral_mode(PIN_PA21D_GLOC_IN1, MUX_PA21D_GLOC_IN1);
	ioport_set_pin_peripheral_mode(PIN_PA22D_GLOC_IN2, MUX_PA22D_GLOC_IN2);
	ioport_set_pin_peripheral_mode(PIN_PA07D_GLOC_IN3, MUX_PA07D_GLOC_IN3);
	ioport_set_pin_peripheral_mode(PIN_PA24D_GLOC_OUT0, MUX_PA24D_GLOC_OUT0);
#endif
}