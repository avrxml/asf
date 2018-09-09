/**
 * \file
 *
 * \brief Board configuration.
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
