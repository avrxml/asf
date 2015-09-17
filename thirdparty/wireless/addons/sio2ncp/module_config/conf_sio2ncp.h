/**
 * \file *********************************************************************
 *
 * \brief Serial Input & Output configuration
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#ifndef CONF_SIO2NCP_H_INCLUDED
#define CONF_SIO2NCP_H_INCLUDED

#warning \
	"Using a default value. Edit this conf_sio2ncp.h file to modify that define value according to the current board."
/* ! \name Configuration for Xmega */
/* ! @{ */
#if (XMEGA)
#define USART_NCP                &USARTC0
#define USART_NCP_BAUDRATE       9600
#define USART_NCP_CHAR_LENGTH    USART_CHSIZE_8BIT_gc
#define USART_NCP_PARITY         USART_PMODE_DISABLED_gc
#define USART_NCP_STOP_BITS      1

#define USART_NCP_RX_ISR_ENABLE() usart_set_rx_interrupt_level(USART_NCP, \
		USART_INT_LVL_HI)
#define USART_NCP_ISR_VECT()      ISR(USARTC0_RXC_vect)
#endif /* XMEGA */
/* ! @} */

/* ! \name Configuration for MegaRF */
/* ! @{ */
#if (MEGA_RF)
#define USART_NCP                (&USARTA0)
#define USART_NCP_BAUDRATE       9600
#define USART_NCP_CHAR_LENGTH    USART_CHSIZE_8BIT_gc
#define USART_NCP_PARITY         USART_PMODE_DISABLED_gc
#define USART_NCP_STOP_BITS      1

#define USART_NCP_RX_ISR_ENABLE() usart_rx_complete_interrupt_enable(USART_NCP)
#define USART_NCP_ISR_VECT()      ISR(USART0_RX_vect)
#endif /* MEGA_RF */
/* ! @} */

/* ! \name Configuration for UC3 */
/* ! @{ */
#if (UC3)
#define USART_NCP                &AVR32_USART0
#define USART_NCP_BAUDRATE       9600
#define USART_NCP_CHAR_LENGTH    8
#define USART_NCP_PARITY         USART_NO_PARITY
#define USART_NCP_STOP_BITS      USART_1_STOPBIT

#define USART_NCP_RX_ISR_ENABLE()
#define USART_NCP_ISR_VECT()      ISR(ncp_uart_isr, 2, 1)
#endif /* UC3 */
/* ! @} */

/* ! \name Configuration for SAM4L */
/* ! @{ */
#if (SAM && !(SAMD || SAMR21))
#define NCP_RESET_GPIO            PIN_PC00

#define USART_NCP                 USART1
#define USART_NCP_BAUDRATE        9600
#define USART_NCP_CHAR_LENGTH     US_MR_CHRL_8_BIT
#define USART_NCP_PARITY          US_MR_PAR_NO
#define USART_NCP_STOP_BITS       US_MR_NBSTOP_1_BIT

#define USART_NCP_ISR_VECT()      ISR(USART1_Handler)
#define USART_NCP_IRQn            USART1_IRQn
#define USART_NCP_RX_ISR_ENABLE() usart_enable_interrupt(USART_NCP, \
		US_IER_RXRDY); \
	NVIC_EnableIRQ(USART_NCP_IRQn);
#endif /* (SAM && !SAMD20) */
/* ! @} */
#if SAMD || SAMR21 || SAML21
#define USART_NCP                 SERCOM4

/** Baudrate setting */
#define USART_NCP_BAUDRATE        9600

#define NCP_SERCOM_MUX_SETTING    USART_RX_1_TX_0_XCK_1
#define NCP_SERCOM_PINMUX_PAD0    PINMUX_UNUSED
#define NCP_SERCOM_PINMUX_PAD1    PINMUX_UNUSED
#define NCP_SERCOM_PINMUX_PAD2    PINMUX_PB09D_SERCOM4_PAD1
#define NCP_SERCOM_PINMUX_PAD3    PINMUX_PB08D_SERCOM4_PAD0

#define USART_NCP_RX_ISR_ENABLE()  _sercom_set_handler(4, USART_NCP_ISR_VECT); \
	USART_NCP->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC; \
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SERCOM4);
#endif
#include "serial.h"

#endif /* CONF_SIO2NCP_H_INCLUDED */
