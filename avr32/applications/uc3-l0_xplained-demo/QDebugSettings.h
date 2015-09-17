/**
 * \file
 *
 * \brief Touch library debug interface configuration
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

#ifndef QDEBUGSETTINGS_H_
#define QDEBUGSETTINGS_H_

//---------- Do not edit --------------------
// Project Constants
// Values from 0xF000->0xFFFF are reserved for Atmel Kits
// values from 0x0000->0xEFFF are available for other projects
#define QT8									0xF001
#define QT16								0xF002
#define QM64								0xF003
#define UC3L_EK_REV2						0xF005
#define UC3_A3_XPLAINED						0xF009
#define UC3_L0_XPLAINED						0xF00A

// Interface constants
#define TWI									1
#define SPI1W								2
#define SPI2W								3
#define UART								4

//---------- Edit Project Info -------------

// Select the type of interface to use for the debug protocol.
// Comment out the interface not used.
// Only one interface should be active.
//#define QDEBUG_SPI
//#define QDEBUG_TWI
#define QDEBUG_SERIAL

#define QDEBUG_SERIAL_BAUD_RATE				57600
#define QDEBUG_SERIAL_USART					(&AVR32_USART2)
#define QDEBUG_SERIAL_USART_RX_PIN			AVR32_USART2_RXD_0_1_PIN
#define QDEBUG_SERIAL_USART_RX_FUNCTION		AVR32_USART2_RXD_0_1_FUNCTION
#define QDEBUG_SERIAL_USART_TX_PIN			AVR32_USART2_TXD_0_1_PIN
#define QDEBUG_SERIAL_USART_TX_FUNCTION		AVR32_USART2_TXD_0_1_FUNCTION
#define QDEBUG_SERIAL_USART_IRQ				AVR32_USART2_IRQ
#define QDEBUG_SERIAL_USART_IRQ_GROUP		AVR32_USART2_IRQ_GROUP
#define QDEBUG_SERIAL_PDCA_PID_USART_TX		AVR32_PDCA_PID_USART2_TX
#define QDEBUG_SERIAL_PDCA_PID_USART_RX		AVR32_PDCA_PID_USART2_RX
#define QDEBUG_SERIAL_PDCA_IRQ				AVR32_PDCA_IRQ_2
#define QDEBUG_SERIAL_PDCA_IRQ_GROUP		AVR32_PDCA_IRQ_GROUP0
#define QDEBUG_SERIAL_PDCA_CHANNEL_RX_USART	2
#define QDEBUG_SERIAL_PDCA_CHANNEL_TX_USART	3

#define QDEBUG_CPU_FREQ_HZ					30000000
#define QDEBUG_PBA_FREQ_HZ					30000000

// Set up project info
#define PROJECT_ID							UC3_L0_XPLAINED
#define INTERFACE							UART

#endif // QDEBUGSETTINGS_H_
