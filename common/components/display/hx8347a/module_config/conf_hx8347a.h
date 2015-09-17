/**
 * \file
 *
 * \brief HX8347A display controller driver default config header
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
#ifndef CONF_HX8347A_H_INCLUDED
#define CONF_HX8347A_H_INCLUDED

#include <compiler.h>

/**
 * \brief Select a SPI clock speed
 *
 * This selects the clock speed for the SPI clock used to communicate with the
 * display controller. Higher clock speeds allow for higher frame rates.
 * \note That the clock speed may be limited by the speed of the microcontroller
 * a normal limitation would be CPUclk/2. For more details please refer to the
 * device datasheet.
 */
#define CONF_HX8347A_CLOCK_SPEED   500000UL

#if XMEGA || defined(__DOXYGEN__)

/**
 * \name XMEGA typical configurations
 * @{
 */

/**
 * \brief Select the correct hardware interface
 *
 * Currently supported interfaces are the SPI interface and the USART Master SPI
 * interface.
 */
#define CONF_HX8347A_SPI           &SPIC
/* #define CONF_HX8347A_USART_SPI     &USARTD1 */

/** \brief Define what MCU pin the HX8347A chip select pin is connected to */
#define CONF_HX8347A_CS_PIN        IOPORT_CREATE_PIN(PORTD, 4)

/** \brief Define what MCU pin the HX8347A TE pin is connected to */
#define CONF_HX8347A_TE_PIN        IOPORT_CREATE_PIN(PORTD, 2)

/** \brief Define what MCU pin the HX8347A back light pin is connected to */
#define CONF_HX8347A_BACKLIGHT_PIN IOPORT_CREATE_PIN(PORTD, 0)

/** \brief Define what MCU pin the HX8347A reset is connected to */
#define CONF_HX8347A_RESET_PIN     IOPORT_CREATE_PIN(PORTD, 1)

/** @} */
#endif /* XMEGA */

#if UC3

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
#define CONF_HX8347A_SPI           &AVR32_SPI
/* #define CONF_HX8347A_USART_SPI     &AVR32_USART0 */

/** \brief Define what MCU pin the HX8347A chip select pin is connected to */
#define CONF_HX8347A_CS_PIN        AVR32_PIN_PA00

/** \brief Define what MCU pin the HX8347A TE pin is connected to */
#define CONF_HX8347A_TE_PIN        AVR32_PIN_PA01

/** \brief Define what MCU pin the HX8347A back light pin is connected to */
#define CONF_HX8347A_BACKLIGHT_PIN AVR32_PIN_PA02

/** \brief Define what MCU pin the HX8347A reset is connected to */
#define CONF_HX8347A_RESET_PIN     AVR32_PIN_PA03

/** @} */
#endif /* UC3 */

#endif /* CONF_HX8347A_H_INCLUDED */
