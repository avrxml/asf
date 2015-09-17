/**
 * \file
 *
 * \brief SAM4C-EK board configuration.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/* Keep watchdog at board initialization (not disable it) */
//#define CONF_BOARD_KEEP_WATCHDOG_AT_INIT

/* Configure UART0 pins */
#define CONF_BOARD_UART_CONSOLE
#define CONF_BOARD_UART0

/* Configure LCD backlight */
//#define CONF_BOARD_UART_CONSOLE

/* Configure PWM LED0 pin */
//#define CONF_BOARD_PWM_LED0

/* Configure PWM LED1 pin */
//#define CONF_BOARD_PWM_LED1

/* Configure PWM LED2 pin */
//#define CONF_BOARD_PWM_LED2

/* Configure SPI0 pins */
#define CONF_BOARD_SPI0
#define CONF_BOARD_SPI0_NPCS0

/* Configure SPI1 pins */
//#define CONF_BOARD_SPI1
//#define CONF_BOARD_SPI0_NPCS1

/* Configure TWI0 pins */
//#define CONF_BOARD_TWI0

/* Configure TWI0 pins for AT30TSE */
//#define CONF_BOARD_AT30TSE

/* Configure TWI1 pins */
//#define CONF_BOARD_TWI1

/* Configure USART pins */
//#define CONF_BOARD_USART_RXD
//#define CONF_BOARD_USART_TXD
//#define CONF_BOARD_USART_CTS
//#define CONF_BOARD_USART_RTS
//#define CONF_BOARD_USART_SCK

/* Configure IrDA transceiver shutdown pin */
//#define CONF_BOARD_TFDU4300_SD

/* Configure RS485 transceiver RE pin */
//#define CONF_BOARD_ADM3485_RE

/* Configure ISO7816 card reset pin */
//#define CONF_BOARD_ISO7816_RST

/* Configure ISO7816 interface TXD & SCK pin */
//#define CONF_BOARD_ISO7816

/* Configure ADC pins */
//#define CONF_BOARD_ADC

#endif /* CONF_BOARD_H_INCLUDED */
