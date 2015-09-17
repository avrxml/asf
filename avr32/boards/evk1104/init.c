/**
 * \file
 *
 * \brief EVK1104 board init.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "evk1104.h"
#include "conf_board.h"
#include "gpio.h"
#include "board.h"

#if defined (CONF_BOARD_AT45DBX)
# define AT45DBX_MEM_CNT  1
#endif

void board_init(void)
{
	gpio_configure_pin(LED0_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED1_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED2_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(LED3_GPIO,GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	gpio_configure_pin(GPIO_PUSH_BUTTON_SW2,GPIO_DIR_INPUT);

#if defined (CONF_BOARD_AT45DBX)
	static const gpio_map_t AT45DBX_SPI_GPIO_MAP = {
		{AT45DBX_SPI_SCK_PIN,  AT45DBX_SPI_SCK_FUNCTION },
		{AT45DBX_SPI_MISO_PIN, AT45DBX_SPI_MISO_FUNCTION},
		{AT45DBX_SPI_MOSI_PIN, AT45DBX_SPI_MOSI_FUNCTION},
# define AT45DBX_ENABLE_NPCS_PIN(NPCS, unused) \
	{AT45DBX_SPI_NPCS##NPCS##_PIN, AT45DBX_SPI_NPCS##NPCS##_FUNCTION},
	MREPEAT(AT45DBX_MEM_CNT, AT45DBX_ENABLE_NPCS_PIN, ~)
# undef AT45DBX_ENABLE_NPCS_PIN
	};
	gpio_enable_module(AT45DBX_SPI_GPIO_MAP,
			sizeof(AT45DBX_SPI_GPIO_MAP) / sizeof(AT45DBX_SPI_GPIO_MAP[0]));
#endif

#if defined (CONF_BOARD_SD_MMC_MCI)
	static const gpio_map_t SD_MMC_MCI_GPIO_MAP = {
		{SD_SLOT_8BITS_CLK_PIN, SD_SLOT_8BITS_CLK_FUNCTION},
		{SD_SLOT_8BITS_CMD_PIN, SD_SLOT_8BITS_CMD_FUNCTION},
		{SD_SLOT_8BITS_DATA0_PIN, SD_SLOT_8BITS_DATA0_FUNCTION},
		{SD_SLOT_8BITS_DATA1_PIN, SD_SLOT_8BITS_DATA1_FUNCTION},
		{SD_SLOT_8BITS_DATA2_PIN, SD_SLOT_8BITS_DATA2_FUNCTION},
		{SD_SLOT_8BITS_DATA3_PIN, SD_SLOT_8BITS_DATA3_FUNCTION},
		{SD_SLOT_8BITS_DATA4_PIN, SD_SLOT_8BITS_DATA4_FUNCTION},
		{SD_SLOT_8BITS_DATA5_PIN, SD_SLOT_8BITS_DATA5_FUNCTION},
		{SD_SLOT_8BITS_DATA6_PIN, SD_SLOT_8BITS_DATA6_FUNCTION},
		{SD_SLOT_8BITS_DATA7_PIN, SD_SLOT_8BITS_DATA7_FUNCTION},
		{SD_SLOT_4BITS_CMD_PIN, SD_SLOT_4BITS_CMD_FUNCTION},
		{SD_SLOT_4BITS_DATA0_PIN, SD_SLOT_4BITS_DATA0_FUNCTION},
		{SD_SLOT_4BITS_DATA1_PIN, SD_SLOT_4BITS_DATA1_FUNCTION},
		{SD_SLOT_4BITS_DATA2_PIN, SD_SLOT_4BITS_DATA2_FUNCTION},
		{SD_SLOT_4BITS_DATA3_PIN, SD_SLOT_4BITS_DATA3_FUNCTION},
	};
	gpio_enable_module(SD_MMC_MCI_GPIO_MAP,
			sizeof(SD_MMC_MCI_GPIO_MAP) / sizeof(SD_MMC_MCI_GPIO_MAP[0]));
	gpio_enable_pin_pull_up(SD_MMC_0_CD_GPIO);
	gpio_enable_pin_pull_up(SD_MMC_0_WP_GPIO);
	gpio_enable_pin_pull_up(SD_MMC_1_CD_GPIO);
	gpio_enable_pin_pull_up(SD_MMC_1_WP_GPIO);
#endif

#if defined (CONF_BOARD_TWI)
	static const gpio_map_t TWI_GPIO_MAP = {
		{AVR32_TWIMS0_TWD_0_0_PIN, AVR32_TWIMS0_TWD_0_0_FUNCTION},
		{AVR32_TWIMS0_TWCK_0_0_PIN, AVR32_TWIMS0_TWCK_0_0_FUNCTION},
	};
	gpio_enable_module(TWI_GPIO_MAP,
			sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
#endif

#if defined (CONF_BOARD_COM_PORT)
	static const gpio_map_t COMPORT_GPIO_MAP = {
		{USART_RXD_PIN, USART_RXD_FUNCTION },
		{USART_TXD_PIN, USART_TXD_FUNCTION },
	};
	gpio_enable_module(COMPORT_GPIO_MAP,
			sizeof(COMPORT_GPIO_MAP) / sizeof(COMPORT_GPIO_MAP[0]));
#endif

#if defined (CONF_BOARD_COM_PORT3)
	static const gpio_map_t COMPORT3_GPIO_MAP = {
		{AVR32_USART3_RXD_0_2_PIN, AVR32_USART3_RXD_0_2_FUNCTION },
		{AVR32_USART3_TXD_0_3_PIN, AVR32_USART3_TXD_0_3_FUNCTION },
	};
	gpio_enable_module(COMPORT3_GPIO_MAP,
			sizeof(COMPORT3_GPIO_MAP) / sizeof(COMPORT3_GPIO_MAP[0]));
#endif
}
