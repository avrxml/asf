/**
 * \file serial.h
 *
 * \brief API of the usart interface functions
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
