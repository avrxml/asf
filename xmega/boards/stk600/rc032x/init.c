/**
 * \file
 *
 * \brief STK600 with RC032X routing board init.
 *
 * To use this board, define BOARD=STK600_RC032X.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
#include "stk600_rc032x.h"
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

	ioport_configure_pin(GPIO_PUSH_BUTTON_0, IOPORT_DIR_INPUT
			| IOPORT_PULL_UP | IOPORT_FORCE_ENABLE);
	ioport_configure_pin(GPIO_PUSH_BUTTON_1, IOPORT_DIR_INPUT
			| IOPORT_PULL_UP | IOPORT_FORCE_ENABLE);
	ioport_configure_pin(GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT
			| IOPORT_PULL_UP | IOPORT_FORCE_ENABLE);
	ioport_configure_pin(GPIO_PUSH_BUTTON_3, IOPORT_DIR_INPUT
			| IOPORT_PULL_UP | IOPORT_FORCE_ENABLE);

#ifdef CONF_BOARD_ENABLE_AC_PINS
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 0), IOPORT_DIR_INPUT);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 2), IOPORT_DIR_INPUT);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTB, 1), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_AT45DBX
	ioport_configure_pin(AT45DBX_MASTER_SCK, IOPORT_DIR_OUTPUT |
			IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MOSI, IOPORT_DIR_OUTPUT |
			IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(AT45DBX_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

#ifdef CONF_BOARD_ENABLE_USARTC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif

#ifdef CONF_BOARD_ENABLE_USARTD0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif

#ifdef CONF_BOARD_ENABLE_USARTC0_ONEWIRE
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_ENABLE_USARTD0_ONEWIRE
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_CLKOUT_PC7
	PORTC.DIR |= 0x80;
	PORTCFG.CLKOUT |= PORTCFG_CLKOUT_PC7_gc;
#endif

	/* XCL OUT0 configurations */
#ifdef CONF_BOARD_XCL_OUT0_PD0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 0), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PD4
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 4), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 0), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PC4
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 4), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PD0_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 0), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PD4_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 4), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PC0_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 0), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_OUT0_PC4_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 4),
			IOPORT_DIR_OUTPUT | IOPORT_INV_ENABLED);
#endif

	/* XCL CC0 and CC1 configurations */
#ifdef CONF_BOARD_XCL_CC0_PC2
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_CC0_PC3
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_CC0_PD2
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_CC0_PD3
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_XCL_CC0_PC2_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_CC0_PC3_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_CC0_PD2_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_CC0_PD3_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INV_ENABLED);
#endif

	/* XCL IN0 configurations */
#ifdef CONF_BOARD_XCL_IN0_PD2
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN0_PD2_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN0_PD6
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 6), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN0_PD6_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 6), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN0_PC2
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN0_PC2_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN0_PC6
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 6), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN0_PC6_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 6), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif

	/* XCL IN1 Configurations */
#ifdef CONF_BOARD_XCL_IN1_PD0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 0),
			IOPORT_DIR_INPUT | IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN1_PD0_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 0), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN1_PD4
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 4), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN1_PD4_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 4), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN1_PC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 0), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN1_PC0_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 0), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN1_PC4
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 4), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN1_PC4_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 4), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif

	/* XCL IN2 Configurations */
#ifdef CONF_BOARD_XCL_IN2_PD1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 1), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN2_PD1_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 1), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN2_PD5
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 5), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN2_PD5_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 5), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN2_PC1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 1), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN2_PC1_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 1), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN2_PC5
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 5), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN2_PC5_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 5), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif

	/* XCL IN3 Configurations */
#ifdef CONF_BOARD_XCL_IN3_PD3
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN3_PD3_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN3_PD7
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 7), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN3_PD7_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 7), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN3_PC3
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN3_PC3_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif
#ifdef CONF_BOARD_XCL_IN3_PC7
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE);
#endif
#ifdef CONF_BOARD_XCL_IN3_PC7_INV
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_INPUT
			| IOPORT_FORCE_ENABLE | IOPORT_INV_ENABLED);
#endif

	/* TC45 Output Configurations */
#ifdef CONF_BOARD_TC45_OUT0_PC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 0), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 1), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC2
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC3
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC4
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 4), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC5
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 5), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC6
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 6), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PC7
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_OUTPUT);
#endif
#ifdef CONF_BOARD_TC45_OUT0_PD4
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 4), IOPORT_DIR_OUTPUT);
#endif
}
