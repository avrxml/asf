/**
 * \file
 *
 * \brief Board configuration.
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

/* BOARD Rev. */
#define BOARD_REV BOARD_REV_2

/** Enable watchdog */
//#define CONF_BOARD_KEEP_WATCHDOG_AT_INIT

/** Configure Slow Clock as External Crystal */
#define CONF_BOARD_32K_XTAL

/** Enable UART 0 */
#define CONF_BOARD_UART0

/** Enable UART 1 */
//#define CONF_BOARD_UART1

/** Enable Com Port. */
#define CONF_BOARD_UART_CONSOLE

/** Enable USART 0 */
//#define CONF_BOARD_USART0_RXD
//#define CONF_BOARD_USART0_TXD
//#define CONF_BOARD_USART0_CTS
//#define CONF_BOARD_USART0_RTS

/** Enable USART 1 */
//#define CONF_BOARD_USART1_RXD
//#define CONF_BOARD_USART1_TXD
//#define CONF_BOARD_USART1_CTS
//#define CONF_BOARD_USART1_RTS

/** Enable PLC SPI_0 */
#define CONF_BOARD_SPI0

/** Enable PLC SPI_1 */
//#define CONF_BOARD_SPI1

/** Enable PLC TWI_0 */
//#define CONF_BOARD_TWI0

/* Configure PPLC reset pins */
#define CONF_BOARD_PPLC_ARST
#define CONF_BOARD_PPLC_SRST

/** Enable LCD */
#define CONF_BOARD_LCD_EN

/** Enable Xplain PRO SLP pin */
//#define CONF_BOARD_XP_SLP


#endif /* CONF_BOARD_H_INCLUDED */
