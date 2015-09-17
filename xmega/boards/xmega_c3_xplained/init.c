/**
 * \file
 *
 * \brief XMEGA-C3 Xplained board init.
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
#include <conf_board.h>
#include <board.h>
#include <ioport.h>

/**
 * \addtogroup xmega_c3_xplained_group
 * @{
 */

void board_init(void)
{
	ioport_configure_pin(LED0_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED1_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED2_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED3_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW
			| IOPORT_INV_ENABLED);

	ioport_configure_pin(GPIO_PUSH_BUTTON_0, IOPORT_DIR_INPUT
			| IOPORT_LEVEL | IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_1, IOPORT_DIR_INPUT
			| IOPORT_LEVEL | IOPORT_PULL_UP);

	// Always enable Chip Select pin of OLED
	// to unselect this component at default
	ioport_configure_pin(UG_2832HSWEG04_SS, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
#ifdef CONF_BOARD_OLED_UG_2832HSWEG04
	ioport_configure_pin(UG_2832HSWEG04_DATA_CMD, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(UG_2832HSWEG04_RESET, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
#endif

	// Always enable Chip Select pin of MicroSD
	// to unselect this component at default
	ioport_configure_pin(SD_MMC_SPI_0_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#ifdef CONF_BOARD_SD_MMC_SPI
	ioport_configure_pin(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT
			| IOPORT_LEVEL | IOPORT_PULL_UP);
#endif

#if (defined CONF_BOARD_SD_MMC_SPI) || (defined CONF_BOARD_OLED_UG_2832HSWEG04)
	// Enable common SPI for MicroSD and OLED
	ioport_configure_pin(SD_MMC_SPI_SCK, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(SD_MMC_SPI_MOSI, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(SD_MMC_SPI_MISO, IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_LIGHT_SENSOR
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 0), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_TEMPERATURE_SENSOR
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 1), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_ANALOG_FILTER
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 2), IOPORT_DIR_INPUT);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTF, 0), IOPORT_DIR_OUTPUT);
#endif

#ifdef CONF_BOARD_ENABLE_USARTC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_ENABLE_USARTD0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_ENABLE_USARTE0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 2), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_ENABLE_MXT143E_XPLAINED
	ioport_configure_pin(MXT143E_XPLAINED_SCK, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(MXT143E_XPLAINED_MOSI, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(MXT143E_XPLAINED_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(MXT143E_XPLAINED_CS, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(MXT143E_XPLAINED_CHG, IOPORT_DIR_INPUT);
	ioport_configure_pin(MXT143E_XPLAINED_DC, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
	#ifndef MXT143E_XPLAINED_BACKLIGHT_DISABLE
	ioport_configure_pin(MXT143E_XPLAINED_BACKLIGHT, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
	#endif
	ioport_configure_pin(MXT143E_XPLAINED_LCD_RESET, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
	// Setting PORT_USART0_bm to one will move the pin location
	// of USARTC0 (in mode SPI) from Px[3:0] to Px[7:4].
	PORTC.REMAP |= PORT_USART0_bm;
#endif
}

/**
 * @}
 */
