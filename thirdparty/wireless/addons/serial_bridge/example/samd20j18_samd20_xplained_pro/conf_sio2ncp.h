/**
 * \file conf_sio2ncp.h
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

#define USART_NCP                 EXT1_UART_MODULE

#define NCP_SERCOM_MUX_SETTING    EXT1_UART_SERCOM_MUX_SETTING
#define NCP_SERCOM_PINMUX_PAD0    EXT1_UART_SERCOM_PINMUX_PAD0
#define NCP_SERCOM_PINMUX_PAD1    EXT1_UART_SERCOM_PINMUX_PAD1
#define NCP_SERCOM_PINMUX_PAD2    EXT1_UART_SERCOM_PINMUX_PAD2
#define NCP_SERCOM_PINMUX_PAD3    EXT1_UART_SERCOM_PINMUX_PAD3
/** Baudrate setting */
#define USART_NCP_BAUDRATE        9600

#define USART_NCP_RX_ISR_ENABLE()  _sercom_set_handler(4, USART_NCP_ISR_VECT); \
	USART_NCP->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC; \
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SERCOM4);
#endif /* CONF_SIO2NCP_H_INCLUDED */
