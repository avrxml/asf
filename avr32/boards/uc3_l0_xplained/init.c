/**
 * \file
 *
 * \brief UC3-L0-Xplained board initialization.
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
#include "uc3_l0_xplained.h"
#include "conf_board.h"
#include "board.h"
#include "gpio.h"

#if defined (CONF_BOARD_AT45DBX)
# define AT45DBX_MEM_CNT  1
#endif

void board_init(void)
{
#if defined (CONF_BOARD_AT45DBX)
	// SPI GPIO pin configuration.
	gpio_map_t AT45DBX_SPI_GPIO_MAP =
		{
			{AT45DBX_SPI_SCK_PIN,  AT45DBX_SPI_SCK_FUNCTION },  // SPI Clock.
			{AT45DBX_SPI_MISO_PIN, AT45DBX_SPI_MISO_FUNCTION},  // MISO.
			{AT45DBX_SPI_MOSI_PIN, AT45DBX_SPI_MOSI_FUNCTION},  // MOSI.
#  define AT45DBX_ENABLE_NPCS_PIN(NPCS, unused) \
		{AT45DBX_SPI_NPCS##NPCS##_PIN, AT45DBX_SPI_NPCS##NPCS##_FUNCTION},
			MREPEAT(AT45DBX_MEM_CNT, AT45DBX_ENABLE_NPCS_PIN, ~)
#  undef AT45DBX_ENABLE_NPCS_PIN
		};

	gpio_enable_module( AT45DBX_SPI_GPIO_MAP,
			sizeof (AT45DBX_SPI_GPIO_MAP) / sizeof (AT45DBX_SPI_GPIO_MAP[0]));
#endif

	/* Configure the pins connected to LEDs as output and set their
	* default initial state to high (LEDs off).  Note that on
	* UC3-L0-Xplained, the LEDs have to be turned off in order to
	* sense the button-values.
	*/
	LED_Off((U32) -1);

#if defined (SHARED_PIN_PA11_IS_INPUT)
	// Set push button as input
	gpio_configure_pin(GPIO_PUSH_BUTTON_0, (GPIO_DIR_INPUT | GPIO_PULL_UP));
#endif


#if defined (CONF_BOARD_TWI)
	// TWI Master GPIO pin configuration.
	gpio_map_t TWI_GPIO_MAP =
		{
			{TWIMS0_TWD_PIN,  TWIMS0_TWD_FUNCTION  },
			{TWIMS0_TWCK_PIN, TWIMS0_TWCK_FUNCTION }
		};

	gpio_enable_module(TWI_GPIO_MAP,
			sizeof(TWI_GPIO_MAP) / sizeof (TWI_GPIO_MAP[0]));
#endif


#if defined (CONF_BOARD_COM_PORT)
	// USART on USB Virtual Com GPIO pin configuration.
	gpio_map_t COMPORT_GPIO_MAP =
		{
			{ USART_RXD_PIN, USART_RXD_FUNCTION },
			{ USART_TXD_PIN, USART_TXD_FUNCTION }
		};

	gpio_enable_module(COMPORT_GPIO_MAP,
			sizeof(COMPORT_GPIO_MAP) / sizeof (COMPORT_GPIO_MAP[0]));
#endif

#if defined (CONF_BOARD_COM_PORT_0)
	// USART on J4 header GPIO pin configuration.
	gpio_map_t COMPORT0_GPIO_MAP =
		{
			{ USART0_RXD_PIN, USART0_RXD_FUNCTION },
			{ USART0_TXD_PIN, USART0_TXD_FUNCTION }
		};

	gpio_enable_module(COMPORT0_GPIO_MAP,
			sizeof(COMPORT0_GPIO_MAP) / sizeof (COMPORT0_GPIO_MAP[0]));
#endif

#if defined(CONF_BOARD_ENABLE_MXT143E_XPLAINED)
	/* MXT143E GPIO pin configuration. */
	static const gpio_map_t MXT143E_GPIO_MAP =
		{
			{MXT143E_XPLAINED_SCK,  MXT143E_XPLAINED_SCK_FUNCTION  },
			{MXT143E_XPLAINED_MOSI, MXT143E_XPLAINED_MOSI_FUNCTION },
			{MXT143E_XPLAINED_MISO, MXT143E_XPLAINED_MISO_FUNCTION },
		};
	gpio_enable_module(MXT143E_GPIO_MAP,
			sizeof(MXT143E_GPIO_MAP) / sizeof(MXT143E_GPIO_MAP[0]));
			
	gpio_configure_pin(MXT143E_XPLAINED_CS, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(MXT143E_XPLAINED_CHG, GPIO_DIR_INPUT);
	gpio_configure_pin(MXT143E_XPLAINED_DC, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
	#ifndef MXT143E_XPLAINED_BACKLIGHT_DISABLE
	gpio_configure_pin(MXT143E_XPLAINED_BACKLIGHT, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
	#endif
	gpio_configure_pin(MXT143E_XPLAINED_LCD_RESET, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
#endif

#if defined (SENSORS_XPLAINED_BOARD)
	/* Configure the Xplained Sensor extension board, if any, after
	* the platform Xplained board has configured basic clock settings,
	* GPIO pin mapping, interrupt controller options, etc.
	*/
	sensor_board_init();
#endif
}
