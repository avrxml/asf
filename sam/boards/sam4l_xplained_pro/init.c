/**
 * \file
 *
 * \brief SAM4L Xplained Pro board initialization
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
	struct wdt_config   wdt_cfg;

	wdt_get_config_defaults(&wdt_cfg);
	wdt_init(&wdt_inst, WDT, &wdt_cfg);
	wdt_disable(&wdt_inst);
#endif

	/* Initialize IOPORT */
	ioport_init();

	/* Initialize LED0, turned off */
	ioport_set_pin_dir(LED_0_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);

	/* Initialize SW0 */
	ioport_set_pin_dir(BUTTON_0_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(BUTTON_0_PIN, IOPORT_MODE_PULLUP);

#ifdef CONF_BOARD_EIC
	/* Set push button as external interrupt pin */
	ioport_set_pin_peripheral_mode(BUTTON_0_EIC_PIN,
			BUTTON_0_EIC_PIN_MUX | IOPORT_MODE_PULLUP);
#else
	/* Push button as input: already done, it's the default pin state */
#endif

#if (defined CONF_BOARD_BL)
	// Configure LCD backlight
	ioport_set_pin_dir(LCD_BL_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LCD_BL_GPIO, LCD_BL_INACTIVE_LEVEL);
#endif

#if defined (CONF_BOARD_COM_PORT)
	ioport_set_pin_peripheral_mode(COM_PORT_RX_PIN, COM_PORT_RX_MUX);
	ioport_set_pin_peripheral_mode(COM_PORT_TX_PIN, COM_PORT_TX_MUX);
#endif

#ifdef CONF_BOARD_TWIMS0
	ioport_set_pin_peripheral_mode(PIN_PA23B_TWIMS0_TWD,
			MUX_PA23B_TWIMS0_TWD);
	ioport_set_pin_peripheral_mode(PIN_PA24B_TWIMS0_TWCK,
			MUX_PA24B_TWIMS0_TWCK);
#endif

#ifdef CONF_BOARD_TWIMS3
	ioport_set_pin_peripheral_mode(PIN_PB14C_TWIMS3_TWD,
			MUX_PB14C_TWIMS3_TWD);
	ioport_set_pin_peripheral_mode(PIN_PB15C_TWIMS3_TWCK,
			MUX_PB15C_TWIMS3_TWCK);
#endif

#ifdef CONF_BOARD_USART0
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_RX, EXT1_UART_RX_MUX);
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_TX, EXT1_UART_TX_MUX);
#endif

#if (defined CONF_BOARD_USB_PORT)
	ioport_set_pin_peripheral_mode(PIN_PA25A_USBC_DM, MUX_PA25A_USBC_DM);
	ioport_set_pin_peripheral_mode(PIN_PA26A_USBC_DP, MUX_PA26A_USBC_DP);
# if defined(CONF_BOARD_USB_VBUS_DETECT)
	ioport_set_pin_dir(USB_VBUS_PIN, IOPORT_DIR_INPUT);
# endif
# if defined(CONF_BOARD_USB_ID_DETECT)
	ioport_set_pin_dir(USB_ID_PIN, IOPORT_DIR_INPUT);
# endif
# if defined(CONF_BOARD_USB_VBUS_CONTROL)
	ioport_set_pin_dir(USB_VBOF_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(USB_VBOF_PIN, USB_VBOF_INACTIVE_LEVEL);
# endif
#endif

#ifdef CONF_BOARD_AT86RFX
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_MISO, AT86RFX_SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_MOSI, AT86RFX_SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_SCK, AT86RFX_SPI_SCK_FLAGS);

	ioport_set_pin_peripheral_mode(AT86RFX_SPI_CS_PIN, AT86RFX_SPI_CS_FLAGS);

	/* Initialize TRX_RST and SLP_TR as GPIO. */
	ioport_set_pin_dir(AT86RFX_RST_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_RST_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(AT86RFX_SLP_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_SLP_PIN, IOPORT_PIN_LEVEL_HIGH);
	
#ifdef EXT_RF_FRONT_END_CTRL
	ioport_set_pin_dir(AT86RFX_CPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_CPS, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(AT86RFX_CSD, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_CSD, IOPORT_PIN_LEVEL_HIGH);	
#endif	

#endif

#if defined(CONF_BOARD_SPI) || defined(CONF_BOARD_SD_MMC_SPI)
	ioport_set_pin_peripheral_mode(PIN_PA21A_SPI_MISO, MUX_PA21A_SPI_MISO);
	ioport_set_pin_peripheral_mode(PIN_PA22A_SPI_MOSI, MUX_PA22A_SPI_MOSI);
	ioport_set_pin_peripheral_mode(PIN_PC30B_SPI_SCK, MUX_PC30B_SPI_SCK);

	#ifdef CONF_BOARD_SD_MMC_SPI
		/* Setting SD detection pin */
		ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);
		ioport_set_pin_mode(SD_MMC_0_CD_GPIO, IOPORT_MODE_PULLUP);

		/* Setting SD CS pin */
		ioport_set_pin_peripheral_mode(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
	#endif
	#ifdef CONF_BOARD_SPI_NPCS0
		ioport_set_pin_peripheral_mode(PIN_PC03A_SPI_NPCS0,
				MUX_PC03A_SPI_NPCS0);
	#endif
	#ifdef CONF_BOARD_SPI_NPCS1
		ioport_set_pin_peripheral_mode(PIN_PB13B_SPI_NPCS1,
				MUX_PB13B_SPI_NPCS1);
	#endif
	#ifdef CONF_BOARD_SPI_NPCS2
		ioport_set_pin_peripheral_mode(PIN_PB11B_SPI_NPCS2,
				MUX_PB11B_SPI_NPCS2);
	#endif
#endif

#ifdef CONF_BOARD_DACC_VOUT
	ioport_set_pin_peripheral_mode(DACC_VOUT_PIN, DACC_VOUT_MUX);
#endif

#ifdef CONF_BOARD_ACIFC
	ioport_set_pin_peripheral_mode(PIN_PA06E_ACIFC_ACAN0, MUX_PA06E_ACIFC_ACAN0);
	ioport_set_pin_peripheral_mode(PIN_PA07E_ACIFC_ACAP0, MUX_PA07E_ACIFC_ACAP0);
#endif

#ifdef CONF_BOARD_PARC
	ioport_set_pin_peripheral_mode(PIN_PA17D_PARC_PCCK, MUX_PA17D_PARC_PCCK);
	ioport_set_pin_peripheral_mode(PIN_PA09D_PARC_PCDATA0, MUX_PA09D_PARC_PCDATA0);
	ioport_set_pin_peripheral_mode(PIN_PA10D_PARC_PCDATA1, MUX_PA10D_PARC_PCDATA1);
	ioport_set_pin_peripheral_mode(PIN_PA11D_PARC_PCDATA2, MUX_PA11D_PARC_PCDATA2);
	ioport_set_pin_peripheral_mode(PIN_PA12D_PARC_PCDATA3, MUX_PA12D_PARC_PCDATA3);
	ioport_set_pin_peripheral_mode(PIN_PA13D_PARC_PCDATA4, MUX_PA13D_PARC_PCDATA4);
	ioport_set_pin_peripheral_mode(PIN_PA14D_PARC_PCDATA5, MUX_PA14D_PARC_PCDATA5);
	ioport_set_pin_peripheral_mode(PIN_PA15D_PARC_PCDATA6, MUX_PA15D_PARC_PCDATA6);
	ioport_set_pin_peripheral_mode(PIN_PA16D_PARC_PCDATA7, MUX_PA16D_PARC_PCDATA7);
	ioport_set_pin_peripheral_mode(PIN_PA18D_PARC_PCEN1, MUX_PA18D_PARC_PCEN1);
	ioport_set_pin_peripheral_mode(PIN_PA19D_PARC_PCEN2, MUX_PA19D_PARC_PCEN2);
#endif

#if (defined CONF_BOARD_USB_PORT)
	ioport_set_pin_peripheral_mode(PIN_PA25A_USBC_DM, MUX_PA25A_USBC_DM);
	ioport_set_pin_peripheral_mode(PIN_PA26A_USBC_DP, MUX_PA26A_USBC_DP);
#endif /* (defined CONF_BOARD_USB_PORT) */

#ifdef CONF_BOARD_OLED_UG_2832HSWEG04
	ioport_set_pin_dir(UG_2832HSWEG04_DATA_CMD_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(UG_2832HSWEG04_DATA_CMD_GPIO, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(UG_2832HSWEG04_RESET_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(UG_2832HSWEG04_RESET_GPIO, IOPORT_MODE_PULLUP);
#endif
}

/** @} */
