/**
 * \file serial.h
 *
 * \brief API of the usart interface functions
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
#ifndef _SERIAL_H_
#define _SERIAL_H_

//! \name The channel instance for the USART, PDCA channel 0 for RX (highest priority).
//! @{
#define PDCA_CHANNEL_RX_USART 2
#define PDCA_CHANNEL_TX_USART 3
   //! @}

/*! \name USART BAUD RATE
 */
//! @{
#define USART_BAUD_RATE 115200
//! @}

/*! \name System Clock Frequencies
 */
//! @{
#define TARGET_PBA_FREQ_HZ         (48000000UL)
#define TARGET_CPU_FREQ_HZ         (48000000UL)
//! @}


// UC3L_EK Board definition
#  define USART               (&AVR32_USART3)
#  define USART_RX_PIN        AVR32_USART3_RXD_0_0_PIN
#  define USART_RX_FUNCTION   AVR32_USART3_RXD_0_0_FUNCTION
#  define USART_TX_PIN        AVR32_USART3_TXD_0_0_PIN
#  define USART_TX_FUNCTION   AVR32_USART3_TXD_0_0_FUNCTION
#  define USART_IRQ           AVR32_USART3_IRQ
#  define USART_IRQ_GROUP     AVR32_USART3_IRQ_GROUP
#  define PDCA_PID_USART_TX   AVR32_PDCA_PID_USART3_TX
#  define PDCA_PID_USART_RX   AVR32_PDCA_PID_USART3_RX
#  define PDCA_IRQ            AVR32_PDCA_IRQ_2 // IRQ from PDCA RX Channel 2
#  define PDCA_IRQ_GROUP      AVR32_PDCA_IRQ_GROUP0


void SERIAL_Init(void);

void SERIAL_Send_Message(void);

void SERIAL_Retrieve_Message(void);

#endif //_SERIAL_H_
