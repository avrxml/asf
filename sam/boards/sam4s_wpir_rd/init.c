/**
 * \file
 *
 * \brief SAM4S-WPIR-RD board init.
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

#include "compiler.h"
#include "board.h"
#include "conf_board.h"
#include "gpio.h"

void board_init(void)
{
	/* Disable the watchdog */
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif

	/* Configure LED pins */
	gpio_configure_pin(LED0_GPIO, LED0_FLAGS);

	/* Configure Push Button pins */
	gpio_configure_pin(GPIO_PUSH_BUTTON_1, GPIO_PUSH_BUTTON_1_FLAGS);

	/* Configure UART pins */
#ifdef CONF_BOARD_ADC
	gpio_configure_pin(ADC0_AD0_GPIO, ADC0_AD0_FLAGS);
	gpio_configure_pin(ADC0_AD4_GPIO, ADC0_AD4_FLAGS);
	gpio_configure_pin(ADC0_AD5_GPIO, ADC0_AD5_FLAGS);
#endif

	/* Configure UART pins */
#ifdef CONF_BOARD_UART_CONSOLE
	gpio_configure_group(PINS_UART_PIO, PINS_UART, PINS_UART_FLAGS);
#endif

	/* Configure TWI pins */
#ifdef CONF_BOARD_TWI0
	gpio_configure_pin(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
	gpio_configure_pin(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);
#endif

	/* Configure SPI pins */
#ifdef CONF_BOARD_SPI
	gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	gpio_configure_pin(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
#endif

	/* Configure PCK0 pins */
#ifdef CONF_BOARD_PCK0
	gpio_configure_pin(PIN_PCK0, PIN_PCK0_FLAGS);
#endif

	/* Configure VBUS pins */
#ifdef CONF_BOARD_VBUS
	gpio_configure_pin(USB_VBUS_GPIO, USB_VBUS_FLAGS);
#endif

	/* Configure LCD EBI pins */
#ifdef CONF_BOARD_ILI9325
	gpio_configure_pin(PIN_EBI_DATA_BUS_D0, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D1, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D2, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D3, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D4, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D5, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D6, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D7, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_NRD, PIN_EBI_NRD_FLAGS);
	gpio_configure_pin(PIN_EBI_NWE, PIN_EBI_NWE_FLAGS);
	gpio_configure_pin(PIN_EBI_NCS2, PIN_EBI_NCS2_FLAGS);
	gpio_configure_pin(PIN_EBI_LCD_RS, PIN_EBI_LCD_RS_FLAGS);
#endif

	/* Configure Backlight control pin */
#ifdef CONF_BOARD_AAT3155
	gpio_configure_pin(BOARD_AAT31XX_SET_GPIO, BOARD_AAT31XX_SET_FLAGS);
#endif

	/* Configure SRAM pins */
#ifdef CONF_BOARD_SRAM
	gpio_configure_pin(PIN_EBI_DATA_BUS_D0, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D1, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D2, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D3, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D4, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D5, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D6, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_DATA_BUS_D7, PIN_EBI_DATA_BUS_FLAGS);
	gpio_configure_pin(PIN_EBI_NRD, PIN_EBI_NRD_FLAGS);
	gpio_configure_pin(PIN_EBI_NWE, PIN_EBI_NWE_FLAGS);
	gpio_configure_pin(PIN_EBI_NCS0, PIN_EBI_NCS0_FLAGS);
	gpio_configure_pin(SRAM_VCC_GPIO, SRAM_VCC_FLAGS);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A0, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A1, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A2, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A3, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A4, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A5, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A6, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A7, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A8, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A9, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A10, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A11, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A12, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A13, PIN_EBI_ADDR_BUS_FLAG1);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A14, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A15, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A16, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A17, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A18, PIN_EBI_ADDR_BUS_FLAG2);
	gpio_configure_pin(PIN_EBI_ADDR_BUS_A19, PIN_EBI_ADDR_BUS_FLAG1);
#endif

	/* Configure Image sensor pins */
#ifdef CONF_BOARD_OV7740_IMAGE_SENSOR
	gpio_configure_pin(OV_SW_OVT_GPIO, OV_SW_OVT_FLAGS);
	gpio_configure_pin(OV_RST_GPIO, OV_RST_FLAGS);
	gpio_configure_pin(OV_FSIN_GPIO, OV_FSIN_FLAGS);
	gpio_configure_pin(OV_HSYNC_GPIO, OV_HSYNC_FLAGS);
	gpio_configure_pin(OV_VSYNC_GPIO, OV_VSYNC_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D2, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D3, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D4, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D5, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D6, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D7, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D8, OV_DATA_BUS_FLAGS);
	gpio_configure_pin(OV_DATA_BUS_D9, OV_DATA_BUS_FLAGS);
#endif
}
