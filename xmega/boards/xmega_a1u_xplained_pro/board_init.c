/**
 * \file
 *
 * \brief XMEGA-A1U Xplained Pro board initialization
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
