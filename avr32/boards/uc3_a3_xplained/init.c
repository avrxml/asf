/**
 * \file
 *
 * \brief UC3-A3-Xplained board initialization.
 *
 * This file contains board initialization function.
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "conf_board.h"
#include "board.h"
#include "gpio.h"



#if defined (CONF_BOARD_AT45DBX)
#   define AT45DBX_MEM_CNT          1
#endif



void board_init (void)
{
    gpio_configure_pin(LED0_GPIO, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(LED1_GPIO, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(LED2_GPIO, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(LED3_GPIO, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

    gpio_configure_pin(GPIO_PUSH_BUTTON_0, (GPIO_DIR_INPUT | GPIO_PULL_UP));

#if defined(CONF_BOARD_TWI)
    /* TWI Master GPIO pin configuration. */
    const gpio_map_t TWI_GPIO_MAP =
		{
			{TWIMS0_TWD_PIN,  TWIMS0_TWD_FUNCTION  },
			{TWIMS0_TWCK_PIN, TWIMS0_TWCK_FUNCTION }
		};
    gpio_enable_module(TWI_GPIO_MAP,
        sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
#endif

#if defined(CONF_BOARD_COM_PORT)
	/* USART GPIO pin configuration. */
	static const gpio_map_t COMPORT_GPIO_MAP =
		{
			{USART_RXD_PIN, USART_RXD_FUNCTION },
			{USART_TXD_PIN, USART_TXD_FUNCTION }
		};
	gpio_enable_module(COMPORT_GPIO_MAP,
			sizeof(COMPORT_GPIO_MAP) / sizeof(COMPORT_GPIO_MAP[0]));
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

#if defined(SENSORS_XPLAINED_BOARD)
    /* Configure the Xplained Sensor extension board, if any, after
     * the platform Xplained board has configured basic clock settings,
     * GPIO pin mapping, interrupt controller options, etc.
     */
    sensor_board_init();
#endif

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
	};
	gpio_enable_module(SD_MMC_MCI_GPIO_MAP,
			sizeof(SD_MMC_MCI_GPIO_MAP) / sizeof(SD_MMC_MCI_GPIO_MAP[0]));
	gpio_enable_pin_pull_up(SD_SLOT_8BITS_CARD_DETECT);
	gpio_enable_pin_pull_up(SD_SLOT_8BITS_WRITE_PROTECT);
#endif

}
