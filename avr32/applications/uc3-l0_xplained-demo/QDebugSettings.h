/**
 * \file
 *
 * \brief Touch library debug interface configuration
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
