/**
 * \file
 *
 * \brief XMEGA-A3BU Xplained board init.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <conf_board.h>
#include <board.h>
#include <ioport.h>

/**
 * \addtogroup xmega_a3bu_xplained_group
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
	ioport_configure_pin(GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT
			| IOPORT_LEVEL | IOPORT_PULL_UP);

#ifdef CONF_BOARD_C12832A1Z
	ioport_configure_pin(NHD_C12832A1Z_SPI_SCK, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(NHD_C12832A1Z_SPI_MOSI, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(NHD_C12832A1Z_CSN, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(NHD_C12832A1Z_REGISTER_SELECT, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(NHD_C12832A1Z_RESETN, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(NHD_C12832A1Z_BACKLIGHT, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
#endif

#ifdef CONF_BOARD_AT45DBX
	ioport_configure_pin(AT45DBX_MASTER_SCK, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MOSI, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(AT45DBX_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
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
#endif

#ifdef CONF_BOARD_ENABLE_AC_PINS
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 0), IOPORT_DIR_INPUT);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTA, 2), IOPORT_DIR_INPUT);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTB, 1), IOPORT_DIR_INPUT);
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

#if defined (SENSORS_XPLAINED_BOARD)
	/* Configure the Xplained Sensor extension board, if any, after
	 * the platform Xplained board has configured basic clock settings,
	 * GPIO pin mapping, interrupt controller options, etc.
	 */
	sensor_board_init ();
#endif

#ifdef CONF_BOARD_AT86RFX
	ioport_configure_pin(AT86RFX_SPI_SCK, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(AT86RFX_SPI_MOSI, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(AT86RFX_SPI_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(AT86RFX_SPI_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

	/* Initialize TRX_RST and SLP_TR as GPIO. */
	ioport_configure_pin(AT86RFX_RST_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(AT86RFX_SLP_PIN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif
}

/**
 * @}
 */
