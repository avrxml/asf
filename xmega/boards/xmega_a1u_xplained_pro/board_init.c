/**
 * \file
 *
 * \brief XMEGA-A1U Xplained Pro board initialization
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

#include <board.h>
#include <conf_board.h>
#include <ioport.h>
 
void board_init(void)
{
	//Configure LED0
	ioport_configure_pin(LED0_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	
	//Configure SW0
	ioport_configure_pin(BUTTON_0_PIN, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	
	//Configure pins for OLED display
	#ifdef CONF_BOARD_OLED_UG_2832HSWEG04
		ioport_configure_pin(EXT3_PIN_5,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		ioport_configure_pin(EXT3_PIN_10,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		ioport_configure_pin(EXT3_PIN_15,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		ioport_configure_pin(EXT3_PIN_16,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		ioport_configure_pin(EXT3_PIN_18,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
		//This pin needs to be an output in order for SPI Master mode to work
		ioport_configure_pin(EXT1_PIN_15,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif

	//Configure Rx and Tx pin on PORT E
	#ifdef CONF_BOARD_ENABLE_USARTE0
		ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 3), IOPORT_DIR_OUTPUT
		| IOPORT_INIT_HIGH);
		ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 2), IOPORT_DIR_INPUT);
	#endif
}
