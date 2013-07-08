/**
 * \file
 *
 * \brief SAM4L Xplained Pro board initialization
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
 */

#include <board.h>
#include <ioport.h>
#include <wdt_sam4l.h>

/**
 * \addtogroup sam4l_xplained_pro_group
 * @{
 */

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

void board_init(void)
{

#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	struct wdt_dev_inst wdt_inst;
	wdt_init(&wdt_inst, WDT, NULL);
	wdt_disable(&wdt_inst);
#endif

	// Initialize IOPORT
	ioport_init();

	// Initialize LED0, turned off
	ioport_set_pin_dir(LED_0_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);

	// Initialize SW0
	ioport_set_pin_dir(BUTTON_0_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(BUTTON_0_PIN, IOPORT_MODE_PULLUP);

#ifdef  CONF_BOARD_EIC
	// Set push button as external interrupt pin
	ioport_set_pin_peripheral_mode(BUTTON_0_EIC_PIN,
	BUTTON_0_EIC_PIN_MUX|IOPORT_MODE_PULLUP);
#else
	// Push button as input: already done, it's the default pin state
#endif

#if defined (CONF_BOARD_COM_PORT)
	ioport_set_pin_peripheral_mode(COM_PORT_RX_PIN, COM_PORT_RX_MUX);
	ioport_set_pin_peripheral_mode(COM_PORT_TX_PIN, COM_PORT_TX_MUX);
#endif

#ifdef CONF_BOARD_USART0
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_RX, EXT1_UART_RX_MUX);
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_TX, EXT1_UART_TX_MUX);
#endif

#ifdef CONF_BOARD_AT86RFX
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_MISO, MUX_PA21A_SPI_MISO);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_MOSI, MUX_PA22A_SPI_MOSI);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_SCK, MUX_PC30B_SPI_SCK);

	ioport_set_pin_peripheral_mode(EXT1_PIN_15, MUX_PC03A_SPI_NPCS0);

	/* Initialize TRX_RST and SLP_TR as GPIO. */
	ioport_set_pin_dir(AT86RFX_RST_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_RST_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(AT86RFX_SLP_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_SLP_PIN, IOPORT_PIN_LEVEL_HIGH);
#endif
}

/** @} */
