/**
 * \file
 *
 * \brief STK600 board + RC100X + LCDX initialization.
 *
 * To use this board, define BOARD=STK600_RC064X_LCDX
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
#include "board.h"
#include "compiler.h"
#include "stk600_rc064x_lcdx.h"
#include "conf_board.h"
#include "ioport.h"

void board_init(void)
{
	ioport_configure_pin(LED0_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED1_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED2_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED3_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED4_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED5_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED6_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED7_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

	ioport_configure_pin(GPIO_PUSH_BUTTON_0,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_1,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_2,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_3,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_4,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_5,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_6,IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_7,IOPORT_DIR_INPUT | IOPORT_PULL_UP);

	#ifdef CONF_BOARD_AT45DBX
	ioport_configure_pin(AT45DBX_MASTER_SCK,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MOSI,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(AT45DBX_CS,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif

	#ifdef CONF_BOARD_ENABLE_USARTC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT);
	#endif

	#ifdef CONF_BOARD_ENABLE_USARTC1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 6), IOPORT_DIR_INPUT);
	#endif

}
