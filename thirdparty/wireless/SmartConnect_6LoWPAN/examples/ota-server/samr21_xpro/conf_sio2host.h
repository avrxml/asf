/**
 * \file conf_sio2host.h
 *
 * \brief Serial Input & Output configuration
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#ifndef CONF_SIO2HOST_H_INCLUDED
#define CONF_SIO2HOST_H_INCLUDED
#define SERIAL_RX_BUF_SIZE_HOST    156

#define USART_HOST                 EDBG_CDC_MODULE
#define HOST_SERCOM_MUX_SETTING    EDBG_CDC_SERCOM_MUX_SETTING
#define HOST_SERCOM_PINMUX_PAD0	   EDBG_CDC_SERCOM_PINMUX_PAD0
#define HOST_SERCOM_PINMUX_PAD1    EDBG_CDC_SERCOM_PINMUX_PAD1
#define HOST_SERCOM_PINMUX_PAD2    EDBG_CDC_SERCOM_PINMUX_PAD2
#define HOST_SERCOM_PINMUX_PAD3    EDBG_CDC_SERCOM_PINMUX_PAD3
/** Baudrate setting */
#define USART_HOST_BAUDRATE        38400

#define USART_HOST_RX_ISR_ENABLE()  _sercom_set_handler(0, USART_HOST_ISR_VECT); \
USART_HOST->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC; \
system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SERCOM0);
#endif /* CONF_SIO2HOST_H_INCLUDED */
