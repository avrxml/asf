/**
 * \file
 *
 * \brief ILI9341 display controller component driver default config header
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
#ifndef CONF_ILI9341_H_INCLUDED
#define CONF_ILI9341_H_INCLUDED

#include <compiler.h>

/**
 * \brief Select a SPI clock speed
 *
 * This selects the clock speed for the SPI clock used to communicate with the
 * display controller. Higher clock speeds allow for higher frame rates.
 * \note That the clock speed may be limited by the speed of the
 * microcontroller a normal limitation would be CPUclk/2. For more details
 * please refer to the device datasheet.
 */
#define CONF_ILI9341_CLOCK_SPEED   8000000UL

/**
 * \name UC3 typical configurations
 * @{
 */

/**
 * \brief Select the correct hardware interface
 *
 * Currently supported interfaces are the SPI interface and the USART Master SPI
 * interface.
 */
#if (AVR32_SPI_NUM == 1)
#  define CONF_ILI9341_SPI         &AVR32_SPI
#else
#  define CONF_ILI9341_SPI         &AVR32_SPI0
#endif
/* #define CONF_ILI9341_USART_SPI    &AVR32_USART0 */

/** \brief Define what MCU pin the ILI9341 chip select pin is connected to */
#define CONF_ILI9341_CS_PIN        AVR32_PIN_PA08

/** \brief Define what MCU pin the ILI9341 DC pin is connected to */
#define CONF_ILI9341_DC_PIN        AVR32_PIN_PB10

/** \brief Define what MCU pin the ILI9341 back light pin is connected to */
#define CONF_ILI9341_BACKLIGHT_PIN AVR32_PIN_PB07

/** \brief Define what MCU pin the ILI9341 reset is connected to */
#define CONF_ILI9341_RESET_PIN     AVR32_PIN_PB06

/** \brief Define what peripheral DMA channel to use for the display */
#define CONF_ILI9341_PDCA_CHANNEL  0

/** \brief Define what peripheral ID to use for the display output */
#if defined(AVR32_PDCA_PID_SPI0_TX)
#  define CONF_ILI9341_PDCA_PID    AVR32_PDCA_PID_SPI0_TX
#else
#  define CONF_ILI9341_PDCA_PID    AVR32_PDCA_PID_SPI_TX
#endif

/** @} */

#endif /* CONF_ILI9341_H_INCLUDED */
