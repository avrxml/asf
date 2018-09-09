/**
 * \file
 *
 * \brief XMEGA-C3 Xplained board init.
 *
 * This file contains board initialization function.
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
