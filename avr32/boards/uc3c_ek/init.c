/*****************************************************************************
 *
 * \file
 *
 * \brief AT32UC3C-EK board init.
 *
 * This file contains board initialization function.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */



#include "compiler.h"
#include "uc3c_ek.h"
#include "conf_board.h"
#include "gpio.h"
#include "board.h"

#if defined (CONF_BOARD_AT45DBX)
#define AT45DBX_MEM_CNT             1
#endif

void board_init(void)
{

#if defined (CONF_BOARD_AT45DBX)
	static const gpio_map_t AT45DBX_SPI_GPIO_MAP = {
		{AT45DBX_SPI_SCK_PIN,  AT45DBX_SPI_SCK_FUNCTION},
		{AT45DBX_SPI_MISO_PIN, AT45DBX_SPI_MISO_FUNCTION},
		{AT45DBX_SPI_MOSI_PIN, AT45DBX_SPI_MOSI_FUNCTION},
#  define AT45DBX_ENABLE_NPCS_PIN(npcs, unused) \
		{AT45DBX_SPI_NPCS##npcs##_PIN, AT45DBX_SPI_NPCS##npcs##_FUNCTION},
		MREPEAT(AT45DBX_MEM_CNT, AT45DBX_ENABLE_NPCS_PIN, ~)
#  undef AT45DBX_ENABLE_NPCS_PIN
	};

	// Assign I/Os to SPI.
	gpio_enable_module(AT45DBX_SPI_GPIO_MAP,
			sizeof(AT45DBX_SPI_GPIO_MAP) / sizeof(AT45DBX_SPI_GPIO_MAP[0]));
#endif
#if defined (CONF_BOARD_SD_MMC_SPI)
	static const gpio_map_t SD_MMC_SPI_GPIO_MAP = {
		{SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },
		{SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},
		{SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},
		{SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION},
	};

	// Assign I/Os to SPI.
	gpio_enable_module(SD_MMC_SPI_GPIO_MAP,
			sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

	// Configure SD/MMC card detect and write protect pins
#  define SD_MMC_CONFIGURE_CD_WP_PIN(slot, unused) \
	gpio_configure_pin(SD_MMC_##slot##_CD_GPIO,GPIO_DIR_INPUT); \
	gpio_configure_pin(SD_MMC_##slot##_WP_GPIO,GPIO_DIR_INPUT);
	MREPEAT(SD_MMC_SPI_MEM_CNT, SD_MMC_CONFIGURE_CD_WP_PIN, ~)
#  undef SD_MMC_CONFIGURE_CD_WP_PIN
	
#endif
	// Configure the pins connected to LEDs as output and set their default
	// initial state to high (LEDs off).
	gpio_configure_pin(LED0_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED1_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED2_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED3_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	// Configure the pin connected to the button 0 and 1 button as input.
	gpio_configure_pin(GPIO_PUSH_BUTTON_0,GPIO_DIR_INPUT);
	gpio_configure_pin(GPIO_PUSH_BUTTON_1,GPIO_DIR_INPUT);

#if defined (CONF_BOARD_COM_PORT)
	// USART GPIO pin configuration.
	static const gpio_map_t COMPORT_GPIO_MAP = {
		{USART_RXD_PIN, USART_RXD_FUNCTION},
		{USART_TXD_PIN, USART_TXD_FUNCTION},
	};
	gpio_enable_module(COMPORT_GPIO_MAP,
			sizeof(COMPORT_GPIO_MAP) / sizeof(COMPORT_GPIO_MAP[0]));
#endif

#if defined (CONF_BOARD_TWI)
	static const gpio_map_t TWI_GPIO_MAP = {
		{AVR32_TWIMS0_TWD_0_0_PIN, AVR32_TWIMS0_TWD_0_0_FUNCTION},
		{AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
	};

	// TWI gpio pins configuration
	gpio_enable_module(TWI_GPIO_MAP,
			sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
#endif
}
